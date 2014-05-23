#include <stdio.h>
#include <string.h>


#if 1
#include "cisis.h"   /* CISIS Interface header file */
#include "cirun.h"   /* CISIS Interface runtime declarations */
#else
#include "\cisis\include\cisis.h"   /* CISIS Interface header file */
#include "\cisis\include\cirun.h"   /* CISIS Interface runtime declarations */
#endif

#define MAXTAG 9999
LONGX vtag[MAXTAG+1];

void main(argc,argv)
int argc;
char *argv[];
{
	TRMSTRU *trmp;	/* mandatory for defines TRM */
	LONGX itag,mtag;
	int parmtags=0;
	int parmdiac=0;
	LONGX parmtell=0L;
	LONGX itrm,count=0;
	unsigned char *dbnp,*k1p="",*k2p=NULL,k1[LE2+1],k2[LE2+1];
	unsigned char *p;
	int loop,i;

	if (argc < 2) {
            printf("%s",cicopyr("Utility IFKEYS"));
	    printf(" \n");
	    printf("ifkeys <dbname> [from=<key>] [to=<key>] [+tags] \n");
	    printf(" \n");
	    printf(" <dbname>   input inverted file \n");
	    printf(" from=<key> starting key \n");
	    printf(" to=<key>   ending key \n");
	    printf(" +tags      output tag information \n");
            printf(" tell=<n>   produce a message every each <n> keys \n");
	    printf(" \n");
	    printf(" Displays the dictionary terms preceeded by the");
	    printf(" total number of postings \n");
	    printf(" \n");
	    exit(1);
#if 0
     2|    24|PLACE
     2|    24|PLAINS
     1|    24|PLAN
     1|    24|PLANKTON
     1|    69|PLANKTON
     1|    24|PLANNING
#endif
	}
	dbnp=argv[i=1];
	for (i++; i < argc; i++) {
	    p=argv[i];
	    if (strncmp(p,"key1=",5) == 0 || strncmp(p,"from=",5) == 0) {
		for (p+=5,k1p=k1; *p; ) *k1p++ = isisuctab[*p++];
		*k1p='\0'; k1p=k1;
		continue;
	    }
	    if (strncmp(p,"key2=",5) == 0 || strncmp(p,"to=",3) == 0) {
	        if (strncmp(p,"key2=",5) == 0) p+=5; else p+=3;
		for (k2p=k2; *p; ) *k2p++ = isisuctab[*p++];
		*k2p='\0'; k2p=k2;
		for (loop=strlen(k2p), p=k2p+loop; loop <= LE2; )
		    k2[loop++]= *highv;
		continue;
	    }
	    if (strcmp(p,"+tags") == 0) {
		parmtags=1;
		continue;
	    }
	    if (strcmp(p,"+diac") == 0) {
		parmdiac=1;
		continue;
	    }
	    if (strncmp(p,"tell=",5) == 0) {
	        if (sscanf(p+5,"%"_LD_,&parmtell) != 1) fatal(p);
	        continue;
	    }
	    fatal(p);
	}
	
	for (itag=0; itag <= MAXTAG ; itag++) vtag[itag]=0;
	
	for (TERM(itrm=ntrms,dbnp,k1p); TRMrc != RCEOF; ) {

	        if (parmtell)
	            if (!(++count % parmtell))
			fprintf(stderr,"+++ %"_LD_" %s\n",count,TRMkey);

		loop=(TRMlcase) ? LE2 : LE1;
		for (p=TRMkey+loop-1; loop--; p--)
			if (*p == '\0' || *p == ' ')
				*p = '\0';
			else
				break;

		if (parmtags) {
		    mtag=0;
		    while (posting(itrm,TRMpost+1) > 0L)
			if (TRMptag <= 0 || TRMptag >= MAXTAG)
			    vtag[0]++;
			else {
			    vtag[TRMptag]++;
			    if (mtag < TRMptag) mtag=TRMptag;
			}
		    for (itag=0; itag <= mtag; itag++)
			if (vtag[itag]) {
			    printf("%6"_LD_"|%5"_LD_"|%s\n",
				vtag[itag],(LONGX)itag,TRMkey);
			    vtag[itag]=0;
			}
		}
		else {
		    if (parmdiac) {
			for (p=TRMkey; *p; p++)
			    if (*p >= 128) {
				printf("%6"_LD_"|%s\n",TRMxytotp,TRMkey);
				break;
			    }
		    }
		    else 
		        printf("%6"_LD_"|%s\n",TRMxytotp,TRMkey);
		}

		NXTERM(itrm);
		if (k2p)
		    if (strcmp(TRMkey,k2p) > 0)
			break;
	}
	exit(0); /* 20/07/98 */
}
