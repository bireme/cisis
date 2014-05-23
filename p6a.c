/* Program P6A: dumps the field contents of all active records
               of a given data base
*/
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "cisis.h"   /* CISIS Interface header file */
#include "cirun.h"   /* CISIS Interface runtime declarations */

#define MAXTEMS 45000
#define TAG20 287
#define TAG1 1

int QVN[MAXTEMS];
float QVW[MAXTEMS];

int *DVN;
unsigned dvnsize;
float *DVW;
unsigned dvwsize;

void main(int argc, char *argv[])
{
    RECSTRU *recp;    /* mandatory for defines REC,MFR,DIR,FIELDP */
    LONGX mfn,maxmfn;
    int diridx,dirloop;
    char *p;

    int i;

#if BEFORE20030124
    char *dsp,*nsp,*csp;
#endif
    char areaval[80+1];
    int g;
    LONGX N1;
#if 0
    int c1;
#endif

    float soma2,fator,invsqrts2;
    float W;

    LONGX parmqfrom=1,parmqto=99999999L;
    LONGX parmtell=0;
    int parmtrace=0;
    LONGX count=0;

    if (argc < 2) {
	printf("P6A <dbname> <qfrom> <qto>  <tell> <trace>\n");
	exit(1);
    }

    if (argc >= 2) if (sscanf(argv[2],"%"_LD_,&parmqfrom) != 1) fatal(argv[2]);
    if (argc >= 3) if (sscanf(argv[3],"%"_LD_,&parmqto  ) != 1) fatal(argv[3]);
    if (argc >= 4) if (sscanf(argv[4],"%"_LD_,&parmtell ) != 1) fatal(argv[7]);
    if (argc >= 5) if (sscanf(argv[5],"%d",&parmtrace ) != 1) fatal(argv[8]);

   RECORD(0L,argv[1],0L); maxmfn=MF0nxtmfn-1;

   for (mfn=parmqfrom; mfn<=parmqto; mfn++) {
        RECORD(0L,argv[1],mfn);
        if (RECrc == RCEOF) break;
	if (RECrc != RCNORMAL) continue;
     printf("%"_LD_"|",mfn);
	memset(QVN,0x00,sizeof(QVN));
	memset(QVW,0x00,sizeof(QVW));
	soma2=0;
   for (diridx=0, dirloop=MFRnvf; dirloop--; diridx++) {
       if (DIRtag(diridx) != TAG20) continue;
         p=FIELDP(diridx);

#if  BEFORE20030124
	    dsp=strchr(p,'d'); g=atoi(dsp+1);
	    nsp=strchr(p,'n'); N1=atol(nsp+1);
#else
       subfield((UBYTE *)p, DIRlen(diridx), 'd', (UBYTE *)areaval);
       g=atoi(areaval);
       subfield((UBYTE *)p, DIRlen(diridx), 'n', (UBYTE *)areaval);
       N1=atol(areaval);
#endif
	    if (N1 <1) continue;
	    QVN[g-1]=N1;
	    fator=1*log((float)maxmfn/N1);
	    soma2+=fator*fator;
	}
	if (!soma2) continue;
	for ( i=0; i<MAXTEMS; i++) {
    invsqrts2=1/sqrt(soma2);
	    if (QVN[i]) {
		W=1*log((float)maxmfn/QVN[i])*invsqrts2;
		QVW[i]=W;
      printf("^d%"_LD_"^f%f%", i+1,  QVW[i]);
	    }
	}

   printf("\n");

   }

	count++;
	if (parmtell) if (count%parmtell == 0) fprintf(stderr,"+++ %"_LD_"\n",count);


} /* end of main */
