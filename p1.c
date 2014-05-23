/* Program P1 - MF dump or display according an in-stream/file 
                format specification
*/

#include <stdio.h>
#include <string.h>

#include "cisis.h"   /* CISIS Interface header file */
#include "cirun.h"   /* CISIS Interface runtime declarations */

#if !DOS32BITS
#define OPEN30	1
#endif

/* program parameters */
char *dbnp;
/* recfmt() */
char *fmtp=NULL;		
char fmtarea[MAXMFRL];		/* format spec&results */
LONGX parmlw=79L;		/* line width */

void main(argc,argv)
int argc;
char *argv[];
{
    RECSTRU *recp;		/* current vrecp pointer */
    LONGX ridx;			/* current vrecp index */
    LONGX mfn;			/* current mfn */

    char *prompt=NULL;		/* printf+getchar after each record */
    LONGX parmfrom=1,parmto=LONGX_MAX,parmcount=LONGX_MAX;
    LONGX count;
    int argnext;
    char *p,*q;

#if OPEN30
    union REGS      d33_regs;
    struct SREGS    d33_sregs;
    unsigned short  newax;
    int n;
#endif

    argnext=1;
    if (argc <= argnext) {
        printf("%s",cicopyr("Utility P1"));
        printf("\n");
        printf("p1 <dbn> [<option> [...]] \n");
        printf("\n");
        printf("options: {from|to|count}=<n> \n");
	printf("         pft={<fmt>|@[<file>]} \n");
        printf("         lw=<line width> \n");
        printf("         prompt={<text>|@<file>} \n");
#if OPEN30
        printf("         files=<n> \n");
#endif
        printf("\n");
        exit(1);
    }

    /* get default values */
    dbnp=argv[argnext++];

    /*
        get optional parameters
    */
    for ( ; argnext < argc; argnext++) {

        p=argv[argnext];

#if OPEN30
	if (sscanf(p,"files=%d",&n) == 1) {
            d33_regs.h.ah = 0x67;   /* Set handle count */
            d33_regs.x.bx = n;      /* no of handles */
            newax=intdosx(&d33_regs,&d33_regs,&d33_sregs);
            if (d33_regs.x.cflag) fprintf(stderr,"AX = %04x [%02d]\n",newax,n);
            continue;
        }
#endif
        if (strncmp(p,"from=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&parmfrom) != 1) fatal(p);
            continue;
        }
        if (strncmp(p,"to=",3) == 0) {
            if (sscanf(p+3,"%"_LD_,&parmto) != 1) fatal(p);
            continue;
        }
        if (strncmp(p,"count=",6) == 0) {
            if (sscanf(p+6,"%"_LD_,&parmcount) != 1) fatal(p);
            continue;
        }
        if (strncmp(p,"prompt=",7) == 0) {
            q=p+7;
            prompt=loadfile(NULL,'@',q,prompt,0L,'\n');
	    if (!prompt) fatal(q);
            continue;
        }

        if (strncmp(p,"fsiz=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&fmt_fsiz) != 1) fatal(p);
            continue;
        }
        if (strncmp(p,"lw=",3) == 0) {
            if (sscanf(p+3,"%"_LD_,&parmlw) != 1) fatal(p);
            continue;
        }
        if (strncmp(p,"pft=",4) == 0) {
	    q=p+4;
	    if (strcmp(q,"@") == 0) sprintf(q=fmtarea,"@%s.pft",dbnp);
	    fmtp=loadfile(dbnp,'@',q,NULL,0L,'\0');
            if (!fmtp) fatal(p);
	    continue;
        }
    
	fatal(p);
    }

    ridx=nrecs;
    for (count=0, mfn=parmfrom; mfn <= parmto; mfn++) {
	if (count++ >= parmcount) break;
	RECORD(ridx,dbnp,mfn);
	if (RECrc == RCEOF) break;
	if (fmtp) {
	    if (recfmt(ridx,parmlw,fmtp,fmtarea,sizeof(fmtarea)-1L) < 0)
	        fatal(fmtp+fmt_errof);
	    printf("%s",fmtarea);
	}
	else prtfields(recp,MFRmfn);
	if (prompt) { printf("%s",prompt); if (getchar() == 'x') break; }
    }

    exit(0);
}

