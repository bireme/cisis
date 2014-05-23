#include <stdio.h>
#include <string.h>

#include "cisis.h"       /* CISIS Interface */
#include "cirun.h"       /* runtime area and defines by AOT */

void main(argc,argv)
int argc;
char *argv[];
{
    TRMSTRU *trmp;      /* mandatory for defines TRM, TDB */
    LONGX itrm;          /* term "shelf" index */
    char *dbnp;
    UBYTE key[LE2+1],*keyp,*p;
    char line[BUFSIZ];
    int argnext=1;
    int found=0;
    LONGX mfn=0L;

#if BEFORE20010814
    if (argc < 3) fatal("chkterm <dbn> <key> [mfn] \n");
#endif

    if (argc < 3) {
        printf("%s",cicopyr("Utility CHKTERM"));
        printf("\n");
        printf("chkterm <dbn> <key> [<mfn>] \n");
        printf("\n");
        printf("check term <key> in <dbn> \n");
        printf("if <key> is absent in inverted file <dbn> then display \n");
        printf("*** dbn: <dbn> \n");
        printf("*** key: <key> \n");
        printf("and exit 1 \n");
        printf("\n");
        printf("if <key> has no postings for record <mfn> then display \n");
        printf("*** dbn: <dbn> \n");
        printf("*** key: <key> \n");
        printf("*** mfn: <mfn> \n");
        printf("and exit 2 \n");
        printf("\n");
        printf("otherwise produce no output and exit 0 \n");
        printf("\n");
        exit(1);
    }

    dbnp=argv[argnext++];

    keyp=argv[argnext++];
    if (strlen(keyp) > LE2) fatal(keyp);

    if (argnext < argc) {
        p=argv[argnext++];
        if (sscanf(p,"%"_LD_,&mfn) != 1) fatal(p);
        if (mfn < 1) fatal(p);
    }

    for (p=key; *keyp; p++, keyp++) *p = isisuctab[*keyp];
    *p='\0';

    TERM(itrm=ntrms,dbnp,key); p=line;
    if (TRMrc != RCNORMAL) {
        sprintf(p,"*** dbn: %s\n",dbnp); p+=strlen(p);
        sprintf(p,"*** key: %s\n",key);
        printf("%s",line);
        exit(1);
    }

    if (mfn) {
        while (posting(itrm,TRMpost+1) > 0)
            if (TRMpmfn == mfn) { found=1; break; }
        if (!found) {
            sprintf(p,"*** dbn: %s\n",dbnp); p+=strlen(p);
            sprintf(p,"*** key: %s\n",key);  p+=strlen(p);
            sprintf(p,"*** mfn: %6"_LD_"\n",mfn);
            printf("%s",line);
            exit(2);
        }
    }

    exit(0);
}
