/* asael1.c    -   extrai termos de arquivo log

    Autor:  AOT, 25/03/2003

--------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>

#include "cisis.h"          /* CISIS Interface header file */
#include "cirun.h"          /* CISIS Interface runtime declarations */

#include "cib70.h"

void main(argc,argv)
int argc;
char *argv[];
{
    RECSTRU *recp;
    LONGX irec,mfn;

    LONGX upcrec,upirec;

    char *p;

    char *dbnp;
    char *dbnoutp;
    UWORD parmtag;
    LONGX parmfrom,parmto,parmloop,parmcount,parmtell;
    int parmtrace=0;

    LONGX xtag;
    LONGX count=0;

    int argnext;

    char *boolqryp=NULL;
    LONGX boolridx=0;
    char *booldbnp = NULL;          /* b4x query dbn */
    b7_CIB7 *cib7p=NULL;
    void *ciapip = NULL;
    LONGX crec=0L; /* b5_exp usa crec condicionado a booldbnp */
    int errno;
    
    int xdir;

    char *buffup=NULL;	        /* outfreq/fldupdat, now allocated - AOT, 28/01/2002 */
    LONGX buffuplen=MAXMFRL+BUFSIZ;   /* ALLOMAXV? */


    if (argc < 4) {
        printf("%s",cicopyr("Utility ASAEL1"));
        printf(" \n");
        printf("asael1 <dbn> <tag> [create=]<dbnout> [<option> [...]] \n");
        printf(" \n");
        printf("options: {from|to|loop|count|tell}=<n> \n");
        printf(" \n");
        exit(1);
    }


    /*  init processing
    */
    parmfrom=1; parmto=999999999L; parmloop=1; parmcount=0; parmtell=0;

    argnext=1;
    RECORD(irec=nrecs,dbnp=argv[argnext++],0L);

    p=argv[argnext++];
    if (sscanf(p,"%"_LD_,&xtag) != 1) fatal(p);
    parmtag=(UWORD)xtag;

    dbnoutp=argv[argnext++];

    /*  get optional parms
    */
    for (; argnext < argc; argnext++) {
        p=argv[argnext];
        if (strncmp(p,"from?",5) == 0 || strncmp(p,"from=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&parmfrom) != 1)
                fatal(p);
            continue;
        }
        if (strncmp(p,"to?",3) == 0 || strncmp(p,"to=",3) == 0) {
            if (sscanf(p+3,"%"_LD_,&parmto) != 1)
                fatal(p);
            continue;
        }
        if (strncmp(p,"loop?",5) == 0 || strncmp(p,"loop=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&parmloop) != 1)
                fatal(p);
            continue;
        }
        if (strncmp(p,"count?",6) == 0 || strncmp(p,"count=",6) == 0) {
            if (sscanf(p+6,"%"_LD_,&parmcount) != 1)
                fatal(p);
            continue;
        }
        if (strncmp(p,"tell?",5) == 0 || strncmp(p,"tell=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&parmtell) != 1)
                fatal(p);
            continue;
        }

	    if (strcmp(p,"trace") == 0) {
	        parmtrace=1;
            dbxtrace=rectrace=trmtrace=b70trace=1;
            continue;
        }
    }


    /* allocate buffup
    */
    buffup=loadfile(NULL,'@',"",NULL,buffuplen,'\0');


    /* initialize ridx and b4_xxx() parameters
    */
#if CIAPI
    ciapip = cisisApplication( MAXNREC,MAXNTRM );/* mandatory for CIAPI definitions */
#endif /* CIAPI */

    cib7p=b7_cib7(cib7p,ciapip);

    for (boolridx=nrecs, mfn=1; ; mfn++) { /* get actual maxmfn */
            record(boolridx,dbnp,mfn);
    	    if (VRECrc(boolridx) == RCEOF) fatal(dbnp);
    	    if (VRECrc(boolridx) == RCNORMAL) break;
    }
    VRECdbxp(boolridx)=NULL; /* nao usa base de log */
    VMFRmfn(boolridx)=0L;

    /* create output
    */
    if (!strncmp(dbnoutp,"create=",7)) {
        dbnoutp+=7;
        recisis0(dbnoutp);
    }

    /* init recupdat()
    */
    record(upcrec=nrecs,dbnoutp,0L);
    record(upirec=nrecs,dbnoutp,1L);


    /*
        loop principal
    */

    for (mfn=parmfrom; mfn <= parmto ; mfn+=parmloop) {

	    RECORD(irec,dbnp,mfn);
	    if (RECrc == RCEOF) break;
        if (RECrc != RCNORMAL) continue;
        if (parmcount) if (count >= parmcount) break;

        xdir=fieldx(irec,parmtag,1);

        if (xdir >= 0) {

            char *batchp=buffup;

            boolqryp=FIELDP(xdir);
            boolqryp[DIRlen(xdir)]='\0';

            p=b7_exp(cib7p,boolridx,dbnp,boolqryp,buffup,booldbnp,crec,&errno);
            if (p || errno) {
                sprintf(buffup,"<1 0>Expression syntax error %d: '%s'</1>",errno,p);
            }
            else {
                char *p;
                int xdir;
                xdir=fieldx(boolridx,MFQTOBJ0,1);
                if (xdir < 0) fatal("asael1/MFQTOBJ0");
                p=VFIELDP(boolridx,xdir);
                sprintf(batchp,"<1 0>%d</1>",errno); batchp+=strlen(batchp);
                while (*p != OPRXEND) {
                    if (*p != ZLDV) { p++; continue; }
                    p++;
                    p++;  /* skip PSTLEVEL */
                    sprintf(batchp,"<2 0>"); batchp+=strlen(batchp);
                    while (*p != OPRNULL) *batchp++ = *p++;
                    sprintf(batchp,"</2>"); batchp+=strlen(batchp);
                }
            }
            RECORD(upirec,dbnoutp,mfn);
            MFRstatus=ACTIVE; MFRmfrl=MFRbase=LEADER;
            if (fldupdat(upirec,buffup)) fatal("asael1/fldupdat");
            recupdat(upcrec,upirec);
            if (parmtrace) { printf(".."); getchar(); }
        }

        count++;
        if (parmtell) if ((count % parmtell) == 0) fprintf(stderr,"+++ %"_LD_"\n",count);
    }

    if (parmtell) fprintf(stdout,"asael1: %"_LD_" recs in\n",count);

    if (cib7p) cib7p=b7_cib7_delete(cib7p);
#if CIAPI
    cisisApplicationDelete(ciapip);
    ciapip=NULL;
#endif /* CIAPI */

    exit(0);
}


#if 0

void outfreq(n)
UINT n;
{
    int widx,wlen;
    char *wptr;
    char *batchp,*p;

    widx=0;
    wptr=tabkey;
    do {
        if (widx == maxhash)
	    fatal("asael1/outfreq/widx == maxhash");
        if (*wptr) {
	    batchp=buffup;
	    for (itabx=0;itabx<ntabx;itabx++) {
		for (wlen=0, p=wptr; *p; p++)
		    if (wlen < lentabx[itabx]) wlen++;
		    else break;
		sprintf(batchp,"H%d %d ",itabx+1,wlen); batchp+=strlen(batchp);
		memmove(batchp,wptr,wlen);
		batchp+=wlen;
                wptr+=lentabx[itabx];

            }
            wptr++; /* NULL */
	    sprintf(batchp,"A998/%09"_LD_"/A999/%"_LD_"/",
		999999999L-tabcnt[widx],tabcnt[widx]);
	    batchp+=strlen(batchp);
	    *batchp='\0';
	    record(upirec,dbnoutp,VMF0nxtmfn(upcrec));
	    VMFRstatus(upirec)=ACTIVE;
	    if (fldupdat(upirec,buffup)) fatal(fldupdat(upirec,buffup));
	    recupdat(upcrec,upirec);
            n--;
        }
        else
            wptr+=tabwidth;
        widx++;
    } while (n);

}
#endif
