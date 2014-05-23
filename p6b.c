/* Program P6B: dumps the field contents of all active records
               of a given data base
*/
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "cisis.h"   /* CISIS Interface header file */
#include "cirun.h"   /* CISIS Interface runtime declarations */

#define MAXTEMS 40000
#define TAG20 2

#define MAXRELAT 101


//LONGX Q = 0;
//float QVN[MAXTEMS];
float QVW[MAXTEMS];
//LONGX QVMFN[MAXMFNS];

float TABSIM[MAXRELAT];
LONGX TABSIMMFN[MAXRELAT];

float wrkarea;
int idx1,idx2,idx3,idx4,point1;

 int *DVN;
unsigned dvnsize;
 float *DVW;
unsigned dvwsize;


void main(int argc, char *argv[])
{
    RECSTRU *recp;    /* mandatory for defines REC,MFR,DIR,FIELDP */

    LONGX mfn,maxmfn,mfn2,maxmfn2,mfnq,mfnd;
    int diridx,dirloop,diridx2,dirloop2;
    char *p;

    char areaval[100+1];
    int i, g1, g2;

#if 0
    int c1;
#endif

    float W, DW;
    float somap,somaq2,somad2,denom,sim;

    LONGX parmfrom=1,parmto=99999999L,parmloop=1;
    LONGX parmdfrom=1,parmdto=99999999L;

    if (argc < 2) {
	   printf("P6B <dbname> <parmfrom> <parmto> \n");
	   exit(1);
    }

   if (argc >= 2) if (sscanf(argv[2],"%"_LD_,&parmfrom) != 1) fatal(argv[2]);
   if (argc >= 3) if (sscanf(argv[3],"%"_LD_,&parmto) != 1) fatal(argv[3]);

   p=ALLOC((ALLOPARM)(dvnsize=sizeof(int)*MAXTEMS));
   if (p == NULL) fatal("ALLOC/DVN");
   DVN=(int *)p;
   p=ALLOC((ALLOPARM)(dvwsize=sizeof(float)*MAXTEMS));
   if (p == NULL) fatal("ALLOC/DVW");
   DVW=(float *)p;

   for (mfn=parmfrom; mfn<=parmto; mfn+=parmloop) {
    RECORD(0L,argv[1],mfn);
    if (RECrc == RCEOF) break;
	 if (RECrc != RCNORMAL) continue;
        memset(QVW,0,dvnsize);
     mfnq=mfn;
 	  for (diridx=0, dirloop=MFRnvf; dirloop--; diridx++) {
	    if (DIRtag(diridx) != TAG20) continue;
         p=FIELDP(diridx);
       subfield((UBYTE *)p, DIRlen(diridx), 'f', (UBYTE *)areaval);
       W=atof(areaval);
       subfield((UBYTE *)p, DIRlen(diridx), 'd', (UBYTE *)areaval);
       g1=atoi(areaval);
 //      printf("1. %6"_LD_"|%6d|%2.9f \n",mfn,g1,W);
       QVW[g1]=W;
     }
      for (mfn2=parmdfrom; mfn2<=parmdto; mfn2+=parmloop) {
        RECSTRU *recp; /* Torando o registro como "corrente" */
        RECORD(1L,argv[1],mfn2);
        if (RECrc == RCEOF) break;
	     if (RECrc != RCNORMAL) continue;
        memset(DVW,0,dvwsize);
        mfnd=mfn2;
        for (diridx2=0, dirloop2=MFRnvf; dirloop2--; diridx2++) {
	      if (DIRtag(diridx2) != TAG20) continue;
         p=FIELDP(diridx2);
         subfield((UBYTE *)p, DIRlen(diridx2), 'd', (UBYTE *)areaval);
         g2=atoi(areaval);
         subfield((UBYTE *)p, DIRlen(diridx2), 'f', (UBYTE *)areaval);
         DW=atof(areaval);
  //     printf("2. %6"_LD_"|%6d|%2.9f \n",mfn2,g2,DW);
         DVW[g2]=DW;
        }
   // Calcula a similaridade
         for (somap=somaq2=somad2=0, i=0; i<MAXTEMS; i++) {
              W=QVW[i]; DW= *(DVW+i);
	    	     somap += W*DW;
		        somaq2 += W*W;
              somad2 += DW*DW;
            }
	      denom=sqrt(somaq2*somad2);
	       if (denom > 0) sim=somap/denom;
	       else sim=0;
  //        if (sim > 0)
  //          printf("==> %6"_LD_"|%6"_LD_"|%9.6f \n",mfn, mfn2,sim );

  //Coloca na fila de MAXRELAT maiores;
          if (sim == 0) continue;

          for (idx1=1 ; idx1 < MAXRELAT; ++idx1) {
            if (sim > TABSIM[idx1]) break;
          }
          idx1++;
          for (idx2=MAXRELAT-1; idx2 >= idx1; --idx2) {
              idx4=idx2-1;
              TABSIM[idx2]=TABSIM[idx4];
              TABSIMMFN[idx2]=TABSIMMFN[idx4];
          }
          idx3=idx1-1;
          TABSIM[idx3]=sim;
          TABSIMMFN[idx3]=mfn2;
    //      printf(". %"_LD_"|%"_LD_" \n",mfn2,TABSIMMFN[idx3]);
       }
  }

  for (i=1; i<MAXRELAT; i++)  {
             if (TABSIM[i] == 0) break;
              printf("%"_LD_"|%"_LD_"|%9.6f|",i,mfnq,TABSIM[i]);
              printf("%"_LD_" ",TABSIMMFN[i]);
              printf(" \n");
          }
  exit;
}
/* end of main */
