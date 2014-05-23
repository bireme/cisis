/* Program R1: dumps the field contents of all active records
               of a given data base
*/
#include <stdio.h>
#include "cisis.h"   /* CISIS Interface header file */
#include "cirun.h"   /* CISIS Interface runtime declarations */

void main(int argc, char *argv[])
{
	RECSTRU *recp;	/* mandatory for defines REC,MFR,DIR,FIELDP */
	LONGX mfn;
	int diridx,dirloop,chrloop;
	char *p;

	if (argc != 2) { printf("\nR1 <dbname> \n"); exit(1); }

	for (mfn=1L; ; mfn++) {
		RECORD(0L,argv[1],mfn);
		if (RECrc == RCEOF) break;
		if (RECrc != RCNORMAL) continue;
		printf("mfn %"_LD_"\n",mfn);
		for (diridx=0, dirloop=MFRnvf; dirloop--; diridx++) {
			printf("%3u  ",DIRtag(diridx));
			p=FIELDP(diridx);
			for (chrloop=DIRlen(diridx); chrloop--; p++)
				printf("%c",*p);
			printf("\n");
		}
	}
} /* end of main */
