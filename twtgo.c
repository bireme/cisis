
/* ------------------------------ twtgo.c ----------------------------------- */

#include <stdio.h>
#include <math.h>
#include <string.h>

#if 1
#include "cisis.h"   /* CISIS Interface header file */
#include "cirun.h"   /* CISIS Interface runtime declarations */
#else
#include "\cisis\include\cisis.h"   /* CISIS Interface header file */
#include "\cisis\include\cirun.h"   /* CISIS Interface runtime declarations */
#endif

/* cisis/update working area */
UBYTE buffup[MAXMFRL+BUFSIZ];

void main(argc,argv)
int argc;
char *argv[];
{
    RECSTRU *recp;          /* mandatory for defines REC RDB MF0 MFR DIR, FIELDP */
    LONGX irec;
    LONGX upcrec;
    LONGX mfn;
    UBYTE *batchp,*p;
    int xfd;

    UBYTE *dbnamep,*dbnoutp;
    LONGX parmtell;
    LONGX parmfrom,parmto,parmloop,parmcount;
    LONGX count=0L;

    LONGX v999;
    LONGX v99;

    int argnext=1;

    if (argc < 3) {
        printf("%s",cicopyr("Utility TWTGO"));
        printf("\n");
    	printf("twtgo <dbname> [create=]<dbnout> \n");
    	printf("\n");
    	printf(" <dbname> input master file \n");
    	printf(" <dbnout> output master file for results \n");
    	printf("\n");
    	exit(1);
    }

    dbnamep=argv[argnext++];
    dbnoutp=argv[argnext++];

    parmtell=0;
    parmfrom=1; parmto=0; parmloop=1; parmcount=LONGX_MAX;

    for (; argnext < argc; argnext++) {
        p=argv[argnext];

        if (strncmp(p,"tell=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&parmtell) != 1)
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
        if (strncmp(p,"loop=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&parmloop) != 1)
                fatal(p);
            continue;
        }
        if (strncmp(p,"count=",6) == 0) {
            if (sscanf(p+6,"%"_LD_,&parmcount) != 1)
                fatal(p);
            continue;
        }

        fatal(p);
    }
    if (parmtell) fprintf(stderr,"+++ input data base:  %s\n",dbnamep);
    if (parmtell) fprintf(stderr,"+++ output data base: %s\n",dbnoutp);

    /* init */
    if (strncmp(dbnoutp,"create=",7) == 0) {
      recisis0((dbnoutp+=7));
    }
    else {
	  if (strncmp(dbnoutp,"append=",7) == 0) dbnoutp+=7;
	  dbxopt_fatal=0;
	  if ((xfd=dbxopen("",dbnoutp,mx1extp)) <= 0) {
	    recisis0(dbnoutp);
	}
	else
	    CLOSE(xfd);
    }

    RECORD((upcrec=nrecs),dbnoutp,0L);

    /*
        main loop
    */
    for (irec=nrecs, mfn=parmfrom; ; mfn+=parmloop) {

        if (parmto) if (mfn > parmto) break;
        if (parmcount) if (count >= parmcount) break;

        RECORD(irec,dbnamep,mfn);
        if (RECrc == RCEOF) break;
        count++;
        if (RECrc != RCNORMAL) continue;

        recfield(buffup, irec, 999, 1, "0"); v999=atol(buffup);
        recfield(buffup, irec,  99, 1, "0");  v99=atol(buffup);

        if (v99>0) {
            double num = (double)v999;
            double den = (double)v99;
            float x,w,w5,w6,w7;

            x  = (float)v999/(float)v99;
            w5 = (float)log(1+(1/den));
            w6 = (float)(1/den);
            w7 = w5 / w6;                                             
            w  = w7 * x;

            sprintf(batchp=buffup,"A3|%9.7f| A4|%9.7f| A5|%9.7f| A6|%9.7f| A7|%9.7f|",w,x,w5,w6,w7);

            batchp+=strlen(batchp); *batchp='\0';
            if ((p=fldupdat(irec,buffup)) != NULL) fatal(p);
            recupdat(upcrec,irec);
        }

        if (parmtell) if (!(count % parmtell)) fprintf(stderr,"+++ %"_LD_" recs\n",count);
    }

    if (parmtell) fprintf(stderr,"twtgo: %"_LD_" recs\n",count);
    exit(0);
}




