/* ---------------------------------------------------------------- GENQLF.C */

#include "wheredef.h"

#define VERSION_UPDATE    1
#define VERSION_UNIX 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if VERSION_PC
#include "..\include\cisis.h"
#include "..\include\cirun.h"
#include "..\include\ciscr.h"
#include "..\include\cienv.h"
#endif
#if VERSION_HP
#include "cisis.h"
#include "cirun.h"
#include "cienv.h"
#endif
#if VERSION_UNIX
#include <curses.h>
#include "cisis.h"
#include "cirun.h"
#include "globdef.h"
//#include "ciscr.h"
#include "cienv.h"
#endif

#define FILEQLF        argv[1]
#define DBDECSREC      argv[2]

char buffup[7001],sup[501];

void main(argc,argv)
int argc;
char *argv[];
{
   RECSTRU *recp;
   FILE    *f;
   char     prevtype[5],type[5],qlf[5];
   long     limit,qtde = 0L,prevmfn,mfn,nd;
   int      tag1,tag2,tell,tag = 0,count = 0,nread;

   strcpy(prevtype,"");
   strcpy(type,"  ");
   strcpy(qlf,"   ");
   if (argc < 7) {
      printf("parms: fileqlf dbdecsrec tag1 tag2 tell limit\n");
      exit(0);
   }
   tag1 = atoi(argv[3]);
   tag2 = atoi(argv[4]);
   tell = atoi(argv[5]);
   limit = atol(argv[6]);

   if ((f = fopen(FILEQLF,"r")) == NULL) fatal(FILEQLF);

   record(2L,DBDECSREC,0L);
   if (VRECrc(2L) != RCNORMAL) fatal("genqlf/record");
   for (prevmfn = 0L; ; ) {
      nread = fscanf(f,"%"_LD_"|%c%c|%c%c%c|%"_LD_"\n",
         &mfn,&type[0],&type[1],&qlf[0],&qlf[1],&qlf[2],&nd);

#if VERSION_UPDATE
#else
      printf("%6"_LD_"|%s|%s|%10"_LD_"\n",mfn,type,qlf,nd);
#endif
      if (mfn != prevmfn || strcmp(type,prevtype) != 0 || nread != 7) {
         if (prevmfn) {
#if VERSION_UPDATE
            RECORD(1L,DBDECSREC,prevmfn);
            if (RECrc != RCNORMAL) {
               printf("n„o encontrado em %s(mfn=%"_LD_").\n",DBDECSREC,
                  prevmfn);
               prevmfn = mfn;
               strcpy(prevtype,type);
               continue;
            }
            env_modfld(1L,tag,buffup,0L);
            if (recupdat(2L,1L)) fatal("genqlf/recupdat");
#else
            printf("%s",buffup);
            getchar();
#endif
            if (mfn != prevmfn) {
               if (++count >= tell) { printf("%"_LD_"\n",prevmfn); count = 0; }
               if (limit) if ((++qtde) >= limit)
                  { printf("limite encontrado\n"); break; }
            }
         }
         if (nread != 7) break;
         *buffup = '\0';
         tag = type[0] != '.' ? tag1 : tag2;
         prevmfn = mfn;
         strcpy(prevtype,type);
      }
      if (strcmp(qlf,"   ") == 0) strcpy(qlf+1,"TT");
      sprintf(sup,"^q%-.2s^t%"_LD_,qlf+1,nd);
      strcat(buffup,sup);
   }
   if (nread != EOF) fatal("NOT EOF");
   fclose(f);
   exit(0);
}

