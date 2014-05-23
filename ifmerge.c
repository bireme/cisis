/* ifmerge.c - Load an inverted file from existing inverted files
*/
#include <stdio.h>
#include <string.h>
#include "cisis.h"
#include "ciupi.h"
#include "cirun.h"
extern int ifl_balan; /* cisis.h */

void main(argc,argv)
int argc;
char *argv[];
{
    char *dbnp=NULL;
    char *vifnamp[MAXIFMRG+1];
    LONGX vifmfns[MAXIFMRG];
    int vifxmst[MAXIFMRG];
    int nifs=0;
    LONGX totmfns=0L;
    int mstxrftag=0;
    int pstflag=IFUPISIS;
    LONGX parmxmfn=0L;
    char *parmlogp=NULL;
    LONGX parmtell=0L;
    int parmtrace=0;
    int argnext=1;
    char *p,*q,*namp;

    if (argc < 3) {
        printf("%s",cicopyr("Utility IFMERGE"));
        printf("\n");
#if LIND
        printf("ifmerge <out>[,n] <if1>[,n1] <if2>[,n2] [...] [<option> [...]] \n");
        printf("\n");
        printf(" <out>[,n]              -> output inverted file, maxmfn \n");
#else
        printf("ifmerge <out> <if1>[,n1] <if2>[,n2] [...] [<option> [...]] \n");
        printf("\n");
        printf(" <out>                  -> output inverted file \n");
#endif
        printf(" <if1>[,n1]             -> input inverted file #1, maxmfn#1 \n");
        printf(" <if2>[,n2]             -> input inverted file #2, maxmfn#2 \n");
        printf("  ... \n");
        printf("\n");
        printf("options: \n");
        printf("\n");
        printf(" +mstxrf                -> create <out> M/F and its mstxrf <out>.pft \n");
        printf(" -posts                 -> do not load %s \n",ix1extp);
        printf(" -balan                 -> do not rebalance the dict \n");
#if LIND
        printf(" log=<file>             -> log file name \n");
#endif
        printf(" tell=<n>               -> tell <n> keys has been loaded \n");
        printf(" \n");
        exit(1);
    }


    /* get parms */
    for (; argnext < argc; argnext++) {
        p=argv[argnext];
#if LIND
        if (strncmp(p,"log?",4) == 0 || strncmp(p,"log=",4) == 0) {
            parmlogp=p+4;
            if (!*parmlogp) fatal(p);
            continue;
        }
#endif
        if (strncmp(p,"tell?",5) == 0 || strncmp(p,"tell=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&parmtell) != 1) fatal(p);
            if (parmtell < 0) fatal(p);
            continue;
        }
        if (strcmp(p,"+mstxrf") == 0) {
            mstxrftag=32701;
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
        if (strcmp(p,"trace") == 0) {
            parmtrace=1;
            continue;
        }
        if (strcmp(p,"trace=all") == 0) {
            parmtrace=1;
            dbxtrace=1; trmtrace=1;
            continue;
        }

	if (nifs >= MAXIFMRG) fatal("MAXIFMRG overflow");

	q=strchr((namp=p),','); vifxmst[nifs]=0;
	if (q) {
	    *q='\0';
	    if (sscanf(q+1,"%"_LD_,&vifmfns[nifs]) != 1) fatal(p);
	    if (vifmfns[nifs] < 0) fatal(p);
	}
	else if (dbnp) {
            RECSTRU *recp;
            LONGX irec=nrecs;
            RECORD(irec,namp,0L);
            vifmfns[nifs]=MF0nxtmfn-1;
            vifxmst[nifs]=1;
            FREE(vrecp[irec]); vrecp[irec]=NULL; nrecs--;
	         dbxflush(namp);
	} else vifmfns[0]=0;

	if (parmtrace || parmtell)
	    printf("+++ #%d: %s  mfns=%"_LD_"\n",nifs,namp,vifmfns[nifs]);

	if (!dbnp) {
	    dbnp=namp;
	    parmxmfn=vifmfns[0];
	}
	else {
	    totmfns+=vifmfns[nifs];
	    vifnamp[nifs]=namp;
	    nifs++;
	}
    }

    if (nifs < 1) fatal("no I/F to merge");
    vifnamp[nifs]=NULL; /* end of list */

    /* setup maxmfn */
    if (!parmxmfn) parmxmfn=totmfns;

    /* should pause */
    if (parmtrace || parmtell) {
	printf("+++ create=%s \n",dbnp);
        printf("+++ maxmfn=%"_LD_"\n",parmxmfn);
    }

    /* create mstxrf M/F and reading .pft */
    if (mstxrftag) {
        char buffup[BUFSIZ];
        FILE *fp;
        int xif;
        recisis0(dbnp);
        sprintf(buffup,"%s.pft",dbnp);
        strcpy(buffup,dbxcipar(dbnp,buffup,'='));
        if ((fp=fopen(buffup,"w")) == NULL) fatal(buffup);
        fprintf(fp,"proc('='f(val(v32701^m),1,0)), \n");
        fprintf(fp,"proc('R'v32701^*,',',v32701^m), \n");
        for (xif=0; xif < nifs; xif++) {
            fprintf(fp,"if v32701^*='%s' then @%s.pft, fi, \n",vifnamp[xif],vifnamp[xif]);
        }
        fclose(fp);
        if (totmfns) {
            LONGX outmfn=1;
            LONGX crec,irec;
            record(crec=nrecs,dbnp,0L);
            record(irec=nrecs,dbnp,1L);
            for (xif=0; xif < nifs; xif++) {
                LONGX xmfn=1,xloop;
                LONGX xrec;
                RECSTRU *recp,*xrecp=NULL;
                if (vifxmst[xif]) {
                    RECORD(xrec=nrecs,vifnamp[xif],0L); xrecp=recp;
                }
                for (xloop=vifmfns[xif]; xloop--; outmfn++, xmfn++) {
                    int xrc=RCNORMAL;
                    LONGX xcomb;
                    int xcomp;
                    if (vifxmst[xif]) xrc=recxref(xrecp,xmfn,&xcomb,&xcomp);
                    if (xrc == RCNORMAL) {
                        RECORD(irec,dbnp,outmfn);
                        RECrc=RCNORMAL;
                        MFRstatus=ACTIVE;
                        sprintf(buffup,"A%"_LD_"~%s^m%"_LD_"~",(LONGX)mstxrftag,vifnamp[xif],xmfn);
                        if (fldupdat(irec,buffup)) fatal("ifmerge/fldupdat");
                        recupdat(crec,irec);
                    }
                }
                if (xrecp) {
                    FREE(vrecp[xrec]); vrecp[xrec]=NULL; nrecs--;
                    dbxflush(vifnamp[xif]);
                }
            } /* end for nifs */
            FREE(vrecp[irec]); vrecp[irec]=NULL; nrecs--;
            FREE(vrecp[crec]); vrecp[crec]=NULL; nrecs--;
            mstflush(dbnp);
        }
    }


    /* initialize IF and load link files */
    svdifmerg (dbnp, vifnamp, vifmfns, parmxmfn, parmlogp, pstflag, parmtell);

    exit(0);
}
