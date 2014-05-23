/* ifload.c - Load an inverted file from sorted link files
*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#if CIAPI
#include "ciapi.h"
#else /* CIAPI */
#include "cisis.h"
#endif /* CIAPI */

#if IFLOADFUN
int cisis_ifload(stringp)
char *stringp;
{
#define MAXARGS1 512
    int argc=0;
    char *argv[MAXARGS1+1];
    char *p,*q,c1,c2;
    int strip;
    int rc;
#if LIND
    for (argv[argc++]="iflind", p=stringp; *p; ) {
#else /* LIND */
    for (argv[argc++]="ifload", p=stringp; *p; ) {
#endif /* LIND */
        while (isspace(*p)) p++;
        c1='\r'; c2='\n'; if (*p == '"') { p++; c1=c2='"'; }
        argv[argc++]=p;
        for (strip=0, q=p; *p; ) {
            if (*p == c1) { *p++='\0'; if (*p == c2) p++; break; }
            if (*p == c2) { *p++='\0'; if (*p == c1) p++; break; }
            if (*p) { *q++ = *p++; strip++; }
        }
#if BEFORE401R2 /* because must keep trailing space in "seq=x " */
        if (c1 != '"') while (strip--) if (isspace(*(q-1))) q--; else break;
#endif
        *q='\0';
    }
    argv[argc]=NULL;
    rc=ifloadexec(argc,argv);
    return(rc);
}
#endif /* IFLOADFUN */

#if !CICPP && !CIAPI
extern int ifl_balan; /* cisis.h */
#endif /* !CICPP */

/* function ifloadexec(argc,argv) derived from the original ifload.c source code

   AOT, 21/02/2001

   update ifloadexec(argc,argv) included code from the original iflind.c source

   AOT, 18/05/2001
*/

int ifloadexec(argc,argv)
int argc;
char *argv[];
{
    char *dbnp,*fillk1p,*fillk2p,*rdbnp;
    int pstflag=IFUPISIS;
    LONGX parmtell=0L;
    int parmtrace=0;
#if LIND
    LONGX parmxmfn=0L;
    char *parmxmfp=NULL;
    char *parmlogp=NULL;
    int parmreset=0;
#else /* LIND */
    int parmreset=1;
#endif /* LIND */
    int argnext=1;
    char *p;
    LONGX nlinks;
#if CIAPI
    void *ciapip;
#endif /* CIAPI */

#if LIND
    if (argc < 4) {
        printf("%s",cicopyr("Utility IFLIND"));
        printf("\n");
        printf("iflind <dbname> {<file_lk1>|void} {<file_lk2>|void} [<option> [...]] \n");
        printf("\n");
        printf(" <dbname>               -> output inverted file \n");
        printf(" <file_lk1>             -> sorted %2d-byte link file\n",LE1);
        printf(" <file_lk2>             -> sorted %2d-byte link file\n",LE2);
        printf("\n");
        printf("options: \n");
        printf("\n");
        printf(" -posts                 -> do not load .iyp \n");
        printf(" -balan                 -> do not rebalance the dict \n");
        printf(" maxmfn=<n>             -> max master file number \n");
        printf(" master=<dbn>           -> master file to get maxmfn \n");
        printf(" log=<file>             -> log file name \n");
        printf(" tell=<n>               -> tell <n> links has been loaded \n");
#if !LINK_7544KEY
#if CIIFLFIX
        printf(" +fix[/m]               -> read fixed length link records \n");
#endif
#endif
        printf(" +reset[=<dbn>]         -> reset IF update is pending \n");
        printf(" \n");
        return(1); /* error */
    }
#else /* LIND */
    if (argc < 4) {
        printf("%s",cicopyr("Utility IFLOAD"));
        printf("\n");
        printf("ifload <dbname> {<file_lk1>|void} {<file_lk2>|void} [<option> [...]] \n");
        printf("\n");
        printf(" <dbname>               -> output inverted file \n");
        printf(" <file_lk1>             -> sorted %2d-byte link file\n",LE1);
        printf(" <file_lk2>             -> sorted %2d-byte link file\n",LE2);
        printf("\n");
        printf("options: \n");
        printf("\n");
        printf(" master=<name>          -> alternate master file \n");
        printf(" -reset                 -> keep IF update is pending \n");
        printf(" -posts                 -> do not load .ifp \n");
        printf(" -balan                 -> do not rebalance the dict \n");
        printf(" tell=<n>               -> tell <n> links has been loaded \n");
#if !LINK_7544KEY
#if CIIFLFIX
        printf(" +fix[/m]               -> read fixed link records \n");
#endif
#endif
        printf(" \n");
        return(1); /* error */
    }
#endif /* LIND */


#if CIAPI
    ciapip = cisisApplication( MAXNREC,MAXNTRM );/* mandatory for CIAPI definitions */
#endif /* CIAPI */

    /* get positional parms */
    dbnp=rdbnp=argv[argnext++];
    fillk1p=argv[argnext++];
    fillk2p=argv[argnext++];

    /* get optional parms */
    for (; argnext < argc; argnext++) {
        p=argv[argnext];
        if (strncmp(p,"tell?",5) == 0 || strncmp(p,"tell=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&parmtell) != 1) fatal(p);
            if (parmtell < 0) fatal(p);
            continue;
        }
        if (strcmp(p,"-balan") == 0) {
            ifl_balan=0;
            continue;
        }
        if (strcmp(p,"-posts") == 0) {
            pstflag=IFUPDICT;
            continue;
        }
#if LIND
        if (strncmp(p,"maxmfn?",7) == 0 || strncmp(p,"maxmfn=",7) == 0) {
            if (sscanf(p+7,"%"_LD_,&parmxmfn) != 1) fatal(p);
            if (parmxmfn < 1) fatal(p);
            continue;
        }
        if (strncmp(p,"master?",7) == 0 || strncmp(p,"master=",7) == 0) {
            parmxmfp=p+7;
            if (!*parmxmfp) fatal(p);
            continue;
        }
        if (strncmp(p,"log?",4) == 0 || strncmp(p,"log=",4) == 0) {
            parmlogp=p+4;
            if (!*parmlogp) fatal(p);
            continue;
        }
        if (strncmp(p,"+reset?",7) == 0 || strncmp(p,"+reset=",7) == 0) {
            rdbnp=p+7;
            if (!*rdbnp) fatal(p);
            continue;
        }
        if (strcmp(p,"+reset") == 0) {
            parmreset=1;
            continue;
        }
#else /* LIND */
        if (strncmp(p,"master?",7) == 0 || strncmp(p,"master=",7) == 0) {
            rdbnp=p+7;
            if (!*rdbnp) fatal(p);
            continue;
        }
        if (strcmp(p,"-reset") == 0) {
            parmreset=0;
            continue;
        }
#endif /* LIND */

#if !LINK_7544KEY
#if CIIFLFIX
        if (strcmp(p,"+fix") == 0) {
            ciiflfix=1;
            continue;
        }
        if (strcmp(p,"+fix/m") == 0) {
            ciiflfix=ciiflfim=1;
            continue;
        }
#endif
#endif
        if (strcmp(p,"trace") == 0) {
            parmtrace=1;
            dbxtrace=1; trmtrace=1;
            continue;
        }
        fatal(p);
    }

#if LIND
    /* get maxmfn */
    if (!parmxmfn) {
        LONGX irecx;
        if (!parmxmfp) parmxmfp=dbnp;
        parmxmfn=VMF0nxtmfn(record(irecx=nrecs,parmxmfp,0L))-1;
        FREE(vrecp[irecx]); vrecp[irecx]=NULL; nrecs-=1;
        dbxflush(parmxmfp);
    }
    /* should pause */
    if (parmtrace || parmtell) {
        printf("+++ create=%s \n",dbnp);
        printf("+++ maxmfn=%"_LD_"\n",parmxmfn);
    }
    /* initialize IF and load link files */
    nlinks=svdiflind (dbnp, fillk1p ,fillk2p, parmxmfn, parmlogp, pstflag, parmtell);
#else /* LIND */
    /* should pause */
    if (parmtrace || parmtell) {
        printf("+++ data base: %s\n",dbnp);
        printf("+++ .lk1 file: %s\n",fillk1p);
        printf("+++ .lk2 file: %s\n",fillk2p);
    }
    /* initialize IF and load link files */
    nlinks=svdifload (dbnp, fillk1p ,fillk2p, pstflag, parmtell);
#endif /* LIND */
    dbxflush(dbnp);

    /* reset "I/F update is pending" */
    if (parmreset) {
        recreset(rdbnp,1L,MAXUPDMFN,0L,parmtell);
    }

#if CIAPI
    cisisApplicationDelete( ciapip );
#endif /* CIAPI */

    return( (nlinks < 0) ? /* error */ 1 + (int)(-nlinks) : 0);
}

