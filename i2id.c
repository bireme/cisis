/* Program R1: dumps the field contents of all active records
               of a given data base
*/
#include <stdio.h>
#include <string.h>
#include "cisis.h"   /* CISIS Interface header file */
#include "cirun.h"   /* CISIS Interface runtime declarations */

LONGX count1=0;
LONGX count2=0;

void main(argc,argv)
int argc;
char *argv[];
{
	RECSTRU *recp;	/* mandatory for defines REC,MFR,DIR,FIELDP */
	LONGX mfn;
	int diridx,dirloop,chrloop;
	char *p;
    int parmtrace;
    LONGX parmfrom,parmto,parmloop,parmcount;
    LONGX parmoffset,parmtell;
    char *dbnp;
    int argnext;
    int tag1rec=0;
    char val1rec[200];
    int has1recprev=0,has1rec;

	if (argc < 2) {
	  printf("%s",cicopyr("Utility I2ID"));
	  printf("\n");
	  printf("i2id <dbn> [option [option] ... ] \n");
	  printf("\n");
/**/
	  printf("options: {from/to/loop/count/offset/tell}=<n> \n");
	  printf("          1record=<tag>=<value> \n");
	  printf("\n");
/**/
	  printf("\n");
	  printf("The following output is produced on stdout:\n");
	  printf("\n");
	  printf("!ID mmmmmmm \n");
	  printf("!vXXX!...contents of tag XXX............. \n");
	  printf("!vYYY!...contents of tag YYY............. \n");
	  printf("... \n");
	  printf("\n");
	  printf("where mmmmmmm is the MFN \n");
	  printf("\n");
	  exit(1);
	}

    parmtrace=0;
    parmfrom=1; parmto=0; parmloop=1; parmcount=LONGX_MAX;
    parmoffset=0; parmtell=0;

    dbnp=argv[argnext=1]; argnext++;

    /*
	get optional parameters
    */
    for ( ; argnext < argc; argnext++) {
	p=argv[argnext];

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
	if (strncmp(p,"loop=",5) == 0) {
	    if (sscanf(p+5,"%"_LD_,&parmloop) != 1)
		fatal(p);
	    continue;
	}
	if (strncmp(p,"offset=",7) == 0) {
	    if (sscanf(p+7,"%"_LD_,&parmoffset) != 1)
		fatal(p);
	    continue;
	}

	if (strncmp(p,"tell=",5) == 0) {
	    if (sscanf(p+5,"%"_LD_,&parmtell) != 1)
		fatal(p);
	    continue;
	}

	if (strncmp(p,"1record=",8) == 0) {
	    if (sscanf(p+8,"%d=%s",&tag1rec,val1rec) != 2)
		fatal(p);
	    continue;
	}

	if (strcmp(p,"trace") == 0) {
	    parmtrace=1;
	    continue;
	}

	fatal(p);
    }

    if (!parmto) parmto=LONGX_MAX;
    if (parmtrace) parmtell=1;

    if (parmtrace) {
	    printf("+++ input data base:  %s\n",dbnp);
	    printf("+++ 1record:           tag %d=%s\n",tag1rec,val1rec);
    }

	for (mfn=parmfrom; mfn <= parmto; mfn+=parmloop) {
		RECORD(0L,dbnp,mfn);
		if (RECrc == RCEOF) break;
        count1++; if (count1 > parmcount) break;
        if (parmtell) if (!(count1 % parmtell))	fprintf(stderr,"+++ %"_LD_"/%"_LD_" recs\n",count1,count2);
		if (RECrc != RCNORMAL) continue;

      has1rec=0;
      if (tag1rec) {
		    for (diridx=0, dirloop=MFRnvf; dirloop--; diridx++) {
		        if (DIRtag(diridx) == tag1rec) {
                  p=FIELDP(diridx);
                  if (DIRlen(diridx) == strlen(val1rec))
                      if (memcmp(p,val1rec,DIRlen(diridx)) == 0) {
                          has1rec=1;
                      }
                  break;
              }
          }
      }

      if (has1recprev == 0 || has1rec == 0) {
          count2++;
		    printf("!ID %07"_LD_"\n",mfn+parmoffset);
      }
      has1recprev=has1rec;

		for (diridx=0, dirloop=MFRnvf; dirloop--; diridx++) {
/*
		        if (DIRtag(diridx) > 999) continue;
*/
			printf("!v%03d!",DIRtag(diridx));
			p=FIELDP(diridx);
			for (chrloop=DIRlen(diridx); chrloop--; p++)
				printf("%c",*p);
			printf("\n");
		}
	}
    if (parmtell) fprintf(stderr,"+++ %"_LD_"/%"_LD_" recs\n",count1,count2);
} /* end of main */
