/* Program P6B4: dumps the field contents of all active records
			   of a given data base
            use b7_()
*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>

#include "cisis.h"				 /* CISIS Interface header file */
#include "cirun.h"				 /* CISIS Interface runtime declarations */
#include "cib70.h"

#define MAXTEMS 40000
#define TAG20 2

#define MAXRELAT 101

float QVW[MAXTEMS];

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
	RECSTRU *recp;				 /* mandatory for defines REC,MFR,DIR,FIELDP */
   LONGX irec;

   char *dbnp;

	LONGX mfn,mfnq;
	int diridx,dirloop,diridx2,dirloop2;

	char *p;

   /* bool */
/* areas para b7_() */
LONGX boolridx;
LONGX boolhits=0,boolmfn,boolh;
LONGX booltell=1L;               /* 0L = no msg */
LONGX booltlir=0L;               /* b40 invx */
b7_CIB7 *cib7p=NULL;
char *booldbnp = NULL;          /* b4x query dbn */
LONGX crec=0L; /* b5_exp usa crec condicionado a booldbnp - ie, nao vai usar */
char booltmpnam[CIMPL+1]={'\0'};    /* b72 MFQTHMEM tmp file */
char boolqryp[30*(6+1)+1]; /* 30 chaves de 6 chars+operadorOR + null final */
char *buffup=NULL; /* area de trabalho = parametro do b7_() */
LONGX buffuplen=0;
/* end areas para b7_() */
   char *bp;
   void *ciapip = NULL;
   /* end bool */

	char areaval[100+1];
	int i,g1,g2;

	float W, DW;
	float somap,somaq2,somad2,denom,sim;

	LONGX parmfrom=1,parmto=99999999L,parmloop=1;
	LONGX parmdfrom=1,parmdto=99999999L;
   int parmtrace=1;

	if (argc < 2)
	{
		printf("P6B4 <dbname> <parmfrom> <parmto> <parmdfrom> <parmdto> \n");
		exit(1);
	}

	if (argc >= 2) dbnp=argv[1];
	if (argc >= 3) if (sscanf(argv[2],"%"_LD_,&parmfrom) != 1) fatal(argv[2]);
	if (argc >= 4) if (sscanf(argv[3],"%"_LD_,&parmto) != 1) fatal(argv[3]);
	if (argc >= 5) if (sscanf(argv[4],"%"_LD_,&parmdfrom) != 1) fatal(argv[4]);
	if (argc >= 6) if (sscanf(argv[5],"%"_LD_,&parmdto) != 1) fatal(argv[5]);

	p=ALLOC((ALLOPARM)(dvnsize=sizeof(int)*MAXTEMS));
	if (p == NULL) fatal("ALLOC/DVN");
	DVN=(int *)p;
	p=ALLOC((ALLOPARM)(dvwsize=sizeof(float)*MAXTEMS));
	if (p == NULL) fatal("ALLOC/DVW");
	DVW=(float *)p;

   /* init b7_() */
#if CIAPI
    ciapip = cisisApplication( MAXNREC,MAXNTRM );/* mandatory for CIAPI definitions */
#endif /* CIAPI */
    cib7p=b7_cib7(cib7p,ciapip);
    for (boolridx=nrecs, mfn=1; ; mfn++) { /* get actual maxmfn */
        RECORD(boolridx,dbnp,mfn);
    	  if (RECrc == RCEOF) fatal(dbnp);
    	  if (RECrc == RCNORMAL) break;
    }
    VRECdbxp(boolridx)=NULL; /* nao usa base de log */
    VMFRmfn(boolridx)=0L;

   /* allocate buffup para b7_() */
    if (!buffup) {
        if (boolqryp) buffuplen=
               strlen(dbnp) +                     /* field MFQTDBN0=11 (may be longer than CIMPL+1) */
               sizeof(boolqryp) +                 /* the expression */
               2*sizeof(boolqryp) +               /* the terms + pseudo operators _1termX+11X */
               BUFSIZ+strlen(dbnp) +              /* field MFQSWDBN=20 + a diferent db */
               24*(strlen(B7DELADD)+3*B7UPLEN) +  /* 2 duzias de control fields */  /* DxxAxx|xxx| */
               sizeof(boolqryp)+4 +               /* tmpfile .%03x */
               BUFSIZ;                            /* por si las moscas */
        buffup=loadfile(NULL,'@',"",NULL,buffuplen,'\0');
        if (buffup == NULL) fatal("ALLOC/buffup");
    }
   /* end allocate buffup para b7_() */
   /* end init b7_() */



	irec=nrecs;
   for (mfn=parmfrom; mfn<=parmto; mfn+=parmloop)
	{
		RECORD(irec,dbnp,mfn);
		if (RECrc == RCEOF) break;
		if (RECrc != RCNORMAL) continue;
		memset(QVW,0,dvnsize);
		mfnq=mfn;
      /* init boolqryp */
      bp=boolqryp; *bp='\0';

		for (diridx=0, dirloop=MFRnvf; dirloop--; diridx++)
		{
			if (DIRtag(diridx) != TAG20) continue;
			p=FIELDP(diridx);
			subfield((UBYTE *)p, DIRlen(diridx), 'f', (UBYTE *)areaval);
			W=atof(areaval);
			subfield((UBYTE *)p, DIRlen(diridx), 'd', (UBYTE *)areaval);
			g1=atoi(areaval);
			QVW[g1]=W;
         /* monta boolqryp */
         if (boolqryp[0]) *bp++ = '+'; /* OR */
         memmove(bp,areaval,DIRlen(diridx)); bp+=DIRlen(diridx); *bp='\0';

		}

         /* recupera boolqryp */
    if (boolqryp) { /* mxbool/boolh */
      p=b7_exp(cib7p,boolridx,dbnp,boolqryp,buffup,booldbnp,crec,&errno);
      if (p || errno) printf("Expression syntax error %d: '%s'\n",errno,p);
      else {
        /* set hit file name to "null=don't write" or as provided by "b70x=" */
        if (!booldbnp || booltmpnam[0]) {
            char *hitfnamp="null";
            if (booltmpnam[0]) hitfnamp=booltmpnam;
    	      sprintf(buffup,"D%dA%d|%s|",MFQTHMEM,MFQTHMEM,hitfnamp);
    	      if (fldupdat(boolridx,buffup) != NULL) fatal("mxtb/boolridx/fldupdat");
    	      if (b70trace) prtfields(vrecp[boolridx],VMFRmfn(boolridx));
    	  }
        /* b7_run faz "trirecp=vrep[tlirec]" */
        if (!booltlir) record(booltlir=nrecs,"null",1L);

    	  p=b7_run(cib7p,boolridx,dbnp,buffup,buffuplen,booltell,isisuctab,"",booltlir,crec,&errno);
    	  if (p || errno) printf("Execution error %d: '%s'\n",errno,p);
    	  else {
    	    boolhits=b7_hit(cib7p,boolridx,0L,b7setno);
    	    if (booltell) printf("Hits=%"_LD_"\n",boolhits);
    	  }
      }
      if (p || errno) fatal("Expression execution error");
    } /* end mxbool/boolh */
         /* end recupera boolqryp */


			boolh=1;
         while((boolmfn=b7_hit(cib7p,boolridx,boolh,b7setno))>0L) {
            boolh++;
            if (parmtrace) printf("+++ boolmfn=%"_LD_"\n",boolmfn);
            if (boolmfn <= 0L)      break;
            if (boolmfn < parmdfrom) continue;
            if (boolmfn > parmdto) break;

				RECORD(irec,dbnp,boolmfn);
				memset(DVW,0,dvwsize);
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
					TABSIMMFN[idx3]=boolmfn;
					//printf(". %"_LD_"|%"_LD_" \n",mfn2,TABSIMMFN[idx3]);
		}
	}

	for (i=1; i<MAXRELAT; i++)
	{
		if (TABSIM[i] == 0) break;
		printf("%"_LD_"|%"_LD_"|%9.6f|",i,mfnq,TABSIM[i]);
		printf("%"_LD_" ",TABSIMMFN[i]);
		printf(" \n");
	}
   
	exit(0);
}


/* end of main */
