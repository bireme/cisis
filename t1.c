/* Program T1: dumps the dictionary and postings of a given data base
*/
#include <stdio.h>
#include "cisis.h"   /* CISIS Interface header file */
#include "cirun.h"   /* CISIS Interface runtime declarations */

void main(int argc, char *argv[])
{
	TRMSTRU *trmp;	/* mandatory for defines TRM */
	LONGX itrm;

	if (argc != 2) {
	        printf("%s",cicopyr("t1 <dbname>"));
	        exit(1);
	}

	TERM(itrm=ntrms,argv[1],"!");
	while (TRMrc != RCEOF) {
		printf("%6"_LD_"  %s \n",TRMxytotp,TRMkey);
		while (posting(itrm,TRMpost+1) > 0L)
			printf("   %6"_LD_"  %"_LD_"/%d/%d/%d \n",
				TRMpost,TRMpmfn,TRMptag,TRMpocc,TRMpcnt);
		NXTERM(itrm);
	}
} /* end of main */
