/* Program A1 - Search and MF display according an in-stream format
		specification, with fatal error trapping
*/
#include <stdio.h>
#include <string.h>

#include "cisis.h"   /* CISIS Interface header file */
#include "cirun.h"   /* CISIS Interface runtime declarations */


#define TRAP_FATAL 1

/* program parameters */
char *dbnp;
char *qryp;

/* b4_run() options */
char *savedbnp=NULL;		/* b4_xxx() MF to save queries/hits */
LONGX trec=0L;			/* 1-rec MF:	a101#^p*^yIF_DEFAULT#
						a101#^pPFX1 ^yIF1#
						a101#^pPFX2 ^yIF1#
						...
						.....
						a101#^pPFXlast ^yIFlast# */
LONGX nbytes2=4096L;		/* b4_run() nbytes2 parameter */
LONGX msglevel=1L;		/* b4_run() display operations & main Hits= */
LONGX bhsize=MAXMFRL;            /* b4_xxx() record size for expr/pcode/ctrl */

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

    LONGX nbytes1;		/* b4_run() nbytes1 parameter */
    LONGX boolidx;		/* b4_xxx() record */
    char *boolp;		/* b4_xxx() return string */
    int boolerr;		/* b4_xxx() return code */
    LONGX nord;			/* b4_hit() hit number */
#if TRAP_FATAL
    int rc;			/* setjmp rc */
#endif

    char *prompt=NULL;		/* printf+getchar after each record */
    LONGX parmfrom=1,parmto=LONGX_MAX,parmcount=LONGX_MAX;
    LONGX count;
    int argnext;
    char *p,*q;


    if (argc < (argnext=3)) {
        printf("%s",cicopyr("Utility A1"));
        printf("\n");
        printf("a1 <dbn> {<query>|@<file>} [<option> [...]] \n");
        printf("\n");
        printf("options: {pfx|save}=<dbn> \n");
        printf("         bhsize=<n>          default=%"_LD_"\n",bhsize);
        printf("         nomsg|allmsg \n");
        printf("         {from|to|count}=<n> \n");
	printf("         pft={<fmt>|@[<file>]} \n");
        printf("         lw=<line width> \n");
        printf("         prompt={<text>|<file>} \n");
        printf("\n");
        exit(1);
    }


    /* get default values */
    dbnp=argv[1]; 

    qryp=loadfile(dbnp,'@',argv[2],NULL,0L,'\n');
    if (!qryp) fatal(argv[2]);


    /*
        get optional parameters
    */
    for ( ; argnext < argc; argnext++) {

        p=argv[argnext];

        if (strncmp(p,"fsiz=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&fmt_fsiz) != 1)
                fatal(p);
            continue;
        }
        if (strncmp(p,"from=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&parmfrom) != 1)
                fatal(p);
            continue;
        }
        if (strncmp(p,"to=",3) == 0) {
            if (sscanf(p+3,"%"_LD_,&parmto) != 1)
                fatal(p);
            continue;
        }
        if (strncmp(p,"count=",6) == 0) {
            if (sscanf(p+6,"%"_LD_,&parmcount) != 1)
                fatal(p);
            continue;
        }

        if (strncmp(p,"prompt=",7) == 0) {
            q=p+7;
            prompt=loadfile(NULL,'@',q,prompt,0L,'\n');
	    if (!prompt) fatal(q);
            continue;
        }

        if (strncmp(p,"pft=",4) == 0) {
            q=p+4;
            if (strcmp(q,"@") == 0) sprintf(q=fmtarea,"@%s.pft",dbnp);
            fmtp=loadfile(dbnp,'@',q,fmtp,0L,'\0');
	    if (!fmtp) fatal(q);
            if (!*fmtp) fatal(p);
            continue;
        }

        if (strncmp(p,"lw=",3) == 0) {
            if (sscanf(p+3,"%"_LD_,&parmlw) != 1)
                fatal(p);
            continue;
        }

        if (strncmp(p,"bhsize=",7) == 0) {
            if (sscanf(p+7,"%"_LD_,&bhsize) != 1)
                fatal(p);
            continue;
        }

        if (strncmp(p,"pfx=",4) == 0) {
            record(trec=maxnrec/2,p+4,1L); /* any, but 0L */
            continue;
        }
        if (strncmp(p,"save=",5) == 0) {
            savedbnp=p+5;
            continue;
        }
        if (strcmp(p,"nomsg") == 0) {
            msglevel=0L;
            continue;
        }
        if (strcmp(p,"allmsg") == 0) {
            msglevel=2L;
            continue;
        }

	fatal(p);
    }

    /* trapping fatal errors */
#if TRAP_FATAL
    strcpy(fatal_iomsg,"Not Null for trapping fatal errors");
    rc = setjmp(fatal_jumper);
    if (rc != 0) {
	printf("\n*** An error has occurred.  End of Execution.\n");
	printf("\n*** Please report the following error has occurred:\n");
	printf("\n    %s\n\n",fatal_iomsg);
	exit(1);
    }
#endif

    /* initialize ridx and b4_xxx() parameters */

    /* get maxmfn */
    RECORD(ridx=nrecs,dbnp,0L);
    /* allocate b4_xxx() working record */
    nbytes1=MF0nxtmfn/8+1;	/* one bit per mfn for b4_run bit strings */
    recalloc(boolidx=nrecs,bhsize+nbytes1);
    /* buffer size for b4_run() tmp file */
    nbytes2=4096L;


    /* compile/execute boolean expression */

    boolp=b4_exp(boolidx,dbnp,qryp,savedbnp,&boolerr);
    if (boolp) {
	printf("Error #%d (%s)\n",boolerr,boolp);
	fatal("Expression syntax error");
    }
    boolp=b4_run(boolidx,dbnp,nbytes1,nbytes2,msglevel,isisuctab,trec,&boolerr);
    if (boolp) {
	printf("Error #%d (%s)\n",boolerr,boolp);
	fatal("Expression execution error");
    }


    /* get retrieved records, format and display */

    if (msglevel) printf("Hits=%"_LD_" \n",b4_hit(boolidx,0L));
    
    for (count=0, nord=1L; ; nord++) {
	mfn=b4_hit(boolidx,nord);
	if (mfn < 0L) break;
	if (mfn < parmfrom) continue;
	if (mfn > parmto) break;
	if (count++ >= parmcount) break;
	RECORD(ridx,dbnp,mfn);
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

