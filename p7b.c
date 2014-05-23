/* Program P7B: dumps the field contents of all active records
			   of a given data base
            use b7_()
*/
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "cisis.h"				 /* CISIS Interface header file */
#include "cirun.h"				 /* CISIS Interface runtime declarations */

#define TAG20 2
#define MAXRELAT 101

typedef struct termweight {
    unsigned short d;  /* numero do termo (dct word) */
    float          w;  /* seu peso */
} TERMWEIGHT;

#define MAXREGISTROS 10000
#define MAXTERMOS 6

TERMWEIGHT Vetores[MAXREGISTROS+1][MAXTERMOS];

void main(int argc, char *argv[])
{

   RECSTRU *recp;				 /* mandatory for defines REC,MFR,DIR,FIELDP */
   LONGX irec;

   char *dbnp;

	LONGX mfn,mfn1,mfn2,maxmfn=0;
	int diridx,dirloop;

	char *p;

	char areaval[80+1];
	unsigned short i,j,g1;

	float W, DW;
	float somap,somaq2,somad2,denom,sim;

   float TABSIM[MAXRELAT];
   LONGX TABSIMMFN[MAXRELAT];
   int idx1,idx2,idx3,idx4;

	LONGX parmfrom=1,parmto=99999999L;
	LONGX parmdfrom=1,parmdto=99999999L;
   int parmtrace=0;

	if (argc < 2)
	{
		printf("P7B <dbname> <parmfrom> <parmto> <parmdfrom> <parmdto> [<parmtrace>]\n");
		exit(1);
	}

	if (argc >= 2) dbnp=argv[1];
	if (argc >= 3) if (sscanf(argv[2],"%"_LD_,&parmfrom) != 1) fatal(argv[2]);
	if (argc >= 4) if (sscanf(argv[3],"%"_LD_,&parmto) != 1) fatal(argv[3]);
	if (argc >= 5) if (sscanf(argv[4],"%"_LD_,&parmdfrom) != 1) fatal(argv[4]);
	if (argc >= 6) if (sscanf(argv[5],"%"_LD_,&parmdto) != 1) fatal(argv[5]);
	if (argc >= 7) if (sscanf(argv[6],"%d",&parmtrace) != 1) fatal(argv[6]);

   if (parmto>MAXREGISTROS) fatal("parmto>=MAXREGISTROS");

   /* arma Vetores */
   memset(Vetores,0x00,sizeof(Vetores));

	irec=nrecs;
   for (mfn=parmfrom; mfn<=parmto; mfn++)
	{
		int icol=0;
      RECORD(irec,dbnp,mfn);
		if (RECrc == RCEOF) break;
		if (RECrc != RCNORMAL) continue;
		maxmfn=mfn;

		for (diridx=0, dirloop=MFRnvf; dirloop--; diridx++)
		{
			if (DIRtag(diridx) != TAG20) continue;
			p=FIELDP(diridx);
			subfield((UBYTE *)p, DIRlen(diridx), 'w', (UBYTE *)areaval);
			W=atof(areaval);
			subfield((UBYTE *)p, DIRlen(diridx), 'd', (UBYTE *)areaval);
			g1=(unsigned short)atoi(areaval);
         /* armazena ate MAXTERMOS celulas */
         if (icol<MAXTERMOS) {
           Vetores[mfn][icol].d = g1;
           Vetores[mfn][icol].w = W;
           icol++;
         }
         else {
           // compara, reloca etc
         }
		}
   }


   for (mfn1=parmfrom; mfn1<=maxmfn; mfn1++)
	{
       if (parmtrace & 0x01) fprintf(stderr,"+++ %"_LD_"\n",mfn1);
       printf("%"_LD_,mfn1);

       memset(TABSIM,0x00,sizeof(TABSIM));
       memset(TABSIMMFN,0x00,sizeof(TABSIMMFN));

       for (mfn2=parmfrom; mfn2<=maxmfn; mfn2++) {

            if (parmtrace & 0x02) fprintf(stderr,"+++ mfn2=%"_LD_"\n",mfn2);
            if (mfn2 <= 0L)      break;
            if (mfn2 < parmdfrom) continue;
            if (mfn2 > parmdto) break;

				// Calcula a similaridade
				for (somap=somaq2=somad2=0, i=0; i<MAXTERMOS; i++) {
                W=Vetores[mfn1][i].w;
                somaq2 += W*W;
            }
				for (i=0; i<MAXTERMOS; i++) {
               g1=Vetores[mfn1][i].d;
               for (j=0; j<MAXTERMOS; j++) {
                  if (g1 == Vetores[mfn2][j].d) {
                       W=Vetores[mfn1][i].w;
                      DW=Vetores[mfn2][j].w;
                      somap += W*DW;
                  }
               }
				}
            for (j=0; j<MAXTERMOS; j++) {
               DW=Vetores[mfn2][j].w;
               somad2 += DW*DW;
            }
				denom=sqrt(somaq2*somad2);
				if (denom > 0) sim=somap/denom;
				else sim=0;

            if (parmtrace & 0x04) fprintf(stderr,"+++ %"_LD_"|%"_LD_"|%f\n",mfn1,mfn2,sim);
				//Coloca na fila de maiores;

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
					//fprintf(stderr,". %"_LD_"|%f\n",mfn2,TABSIMMFN[idx3]);
		}

  	   for (i=1; i<MAXRELAT; i++)
	   {
		   if (TABSIM[i] == 0) break;
		   printf("|%"_LD_"|%9.6f",TABSIMMFN[i],TABSIM[i]);
	   }
      printf(" \n");

   }

	exit(0);
}


/* end of main */
