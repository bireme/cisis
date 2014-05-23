#include <stdio.h>
#if CIAPI
#include "ciapi.h"
#if !TLS
#define ciapip cib7p->ciapip
#else
DWORD extern ciapiTlsIndex;
#endif
#else /* CIAPI */
#include "cisis.h"
#include "cirun.h"
#endif /* CIAPI */
#include "cib70.h"

void main(argc,argv)
int argc;
char *argv[];
{
	RECSTRU *recp;
	char *dbnp,*p,*xb70p="xb70";
   LONGX mfn,irec,crec,nord,hit,maxhits=LONGX_MAX,tlirec=0L;
   int xfd,chkhits;
   char buffup[512];
   int pref;
   int tell;
   b7_CIB7 *cib7p=NULL;

	cib7p = b7_cib7( cib7p, 0 );

   chkhits=1;
   pref=1;
   tell=1;

   if (argc < 3) {
   	printf("%s",cicopyr("Utility B7"));
      printf("\n");
      printf("b7 <dbn> <expr> [options]");
      printf("\n");
      printf("options: <pref>          -> use prefix if pref != x\n");
      printf("         <hits>          -> max hits to show \n");
      printf("         <x>             -> b7tell \n");
      printf("\n");
      exit(1);
   }

   if (argc > 3) pref=strcmp( argv[3], "x" );
   if (argc > 4) maxhits=atol(argv[4]);
   if (argc > 5) tell=atol(argv[5]);



   for (irec=nrecs, dbnp=argv[1], mfn=1; ; mfn++) {
   	RECORD(irec,dbnp,mfn);
      if (RECrc == RCEOF) fatal(dbnp);
      if (RECrc == RCNORMAL) break;
   }

   if ((p=(UBYTE *)getenv("B7")) == NULL) p=(UBYTE *)getenv("b7");

   if( p && pref )
   	record(tlirec=nrecs,p,1L);

   dbxopt_fatal=0;
   xfd=dbxopen("",xb70p,xx1extp);

   if (xfd > 0)
   	CLOSE(xfd);
   else
   	recisis0(xb70p);

   RECORD(crec=nrecs,xb70p,0L);
   RECORD(crec,xb70p,b7setno=MF0nxtmfn-1L);
   RECORD(crec,xb70p,0L);

   p=b7_exp(cib7p,irec,dbnp,argv[2],buffup,xb70p,crec,&errno);

	if (p || errno)
   	printf("Expression syntax error %d: '%s'\n",errno,p);
	else{
   	p=b7_run(cib7p,irec,dbnp,buffup,sizeof(buffup),
      			tell,isisuctab,"/* */",tlirec,crec,&errno);

      printf("Hits=%"_LD_" \n",b7_hit(cib7p,irec,0L,mfn));

      if (chkhits)

      for (hit=nord=1L; hit > 0; nord++) {
			hit=b7_hit(cib7p,irec,nord,mfn);
			if (nord <= maxhits && hit > 0 )
         	printf("mfn=%"_LD_"\n",hit);
    	}
   }

   getchar();

}
