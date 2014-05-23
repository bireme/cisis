/* ------------------------------ id2i.c ----------------------------- */

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

#include "cisis.h" /* CISIS Interface */
#include "cirun.h" /* runtime area and defines by AOT */

/* Global */
#if BEFORE20011909
UBYTE line[32768/*MAXMFRL*/],*buffup;
#endif

void main(argc,argv)
int argc;
char *argv[];
{
UBYTE *line,*buffup;

char *filinp,*dbnoutp,*batchp;
FILE *fpinp;	/* input file pointer */

LONGX count1=0;
LONGX count2=0;

LONGX idmfn;
LONGX idtag;

    RECSTRU *recp; 	/* mandatory for defines REC RDB MF0 MFR DIR, FIELDP */

    LONGX irec;		/* current master file record */
    LONGX crec;		/* output control record */

    int argnext,parmtrace,n,append=0;
    LONGX parmfrom,parmto,parmloop,parmcount,parm1000=0L;
    LONGX parmoffset,parmtell;

    LONGX mfn,dummy,nbytes,linenbytes;
    UBYTE *p,*linep;

    if (argc < 3) {
        printf("%s",cicopyr("Utility ID2I"));
        printf("\n");
	printf("id2i <filein> [create[/app]=]<dbout> [option [option] ... ] \n");
        printf("\n");
	printf("options: {from/to/loop/count/offset/tell/id}=<n> \n");
	printf("\n");
	printf("\n");
	printf("The logical record of <filin> has the format:\n");
	printf("\n");
	printf("!ID mmmmmm \n");
	printf("!vXXX!...contents of tag XXX............. \n");
	printf("!vYYY!...contents of tag YYY............. \n");
	printf("... \n");
	printf("\n");
	printf("where mmmmmm is the MFN to be loaded \n");
	printf("\n");
	printf("The contents of a tag may be split in 2 or more lines \n");
	/* printf("each of them up to %d characters LONGX \n",sizeof(line)-1-1); */
	printf("\n");
	exit(1);
    }


    argnext=1;

    filinp=argv[argnext++];
    dbnoutp=argv[argnext++];

    parmtrace=0;
    parmfrom=1; parmto=0; parmloop=1; parmcount=LONGX_MAX;
    parmoffset=0; parmtell=0;

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

	if (strncmp(p,"id=",3) == 0) {
	    if (sscanf(p+3,"%"_LD_,&parm1000) != 1)
		fatal(p);
	    continue;
	}

	if (strcmp(p,"trace") == 0) {
	    parmtrace=1;
	    continue;
	}

	fatal(p);
    }

    if (parmtrace) {
	printf("+++ input data base:  %s\n",filinp);
	printf("+++ output data base: %s\n",dbnoutp);
    }


    /*
	setup input file
    */
    fpinp=fopen(dbxcipar(NULL,filinp,'='),"r");
    if (fpinp == NULL) fatal(filinp);

    if (parmtrace && !parmtell) {
	parmtell=1;
    }

    /*
	setup output master file
    */
    if (strncmp(dbnoutp,"create/app=",11) == 0) {recisis0((dbnoutp+=11)); append=1;}

    if (strncmp(dbnoutp,"create=",7) == 0) recisis0((dbnoutp+=7));

    /*
	open output and load output control record
    */
    recallok(irec=nrecs,rec_maxmfrl);
    RECORD(irec,dbnoutp,1L);
    recallok(crec=nrecs,BUFSIZ);
    RECORD(crec,dbnoutp,0L);

    /*
	allocate input line
    */
    linenbytes=rec_maxmfrl;
    line=(UBYTE *)ALLOC((ALLOPARM)linenbytes); /* AOT, 19/09/2001 */
    if (line == (UBYTE *)ALLONULL) fatal("Not enough memory for line");

    /*
	allocate buffup
    */
    nbytes=2*rec_maxmfrl;
    if (nbytes > ALLOMAXV) nbytes=ALLOMAXV;
    buffup=(UBYTE *)ALLOC((ALLOPARM)/*ALLOMAXV*/nbytes); /* AOT, 05/04/2001 */
    if (buffup == (UBYTE *)ALLONULL) fatal("Not enough memory for buffup");

    /*
	get 1st real input line in advance
    */
    linep=fgets(line,linenbytes-1,fpinp);             
    while (linep)
	if (line[0] == '\n') linep=fgets(line,linenbytes-1,fpinp);
	else break;
    line[strlen(line)-1]='\0';

    /*
	main loop
    */
    for (recp=vrecp[irec], mfn=parmfrom; linep; mfn+=parmloop) {

	if (parmtell)
	    if (!(count1 % parmtell))
		if (count1)
		    fprintf(stderr,"+++ %"_LD_"/%"_LD_" recs\n",count1,count2);

	if (parmto)
	    if (mfn > parmto)
		break;

	if (parmcount)
	    if (count2 >= parmcount)
		break;

	/*
	    =RECORD(irec,filinp,mfn);
	*/
	count1++;
	if (sscanf(line,"!ID %"_LD_"\n",&idmfn) != 1) fatal(line);
	if (idmfn < 0 || idmfn > 9999999L) fatal(line);
	if (parmtrace)
	    fprintf(stderr,"+++ %7"_LD_"  ID %7"_LD_"\n",count1,idmfn);
	sprintf(buffup,"D*");
	if ((p=fldupdat(irec,buffup)) != NULL) fatal(p);
	if (parm1000) { sprintf(buffup,"D*A%"_LD_"/%07"_LD_"/",parm1000,idmfn);
	                if ((p=fldupdat(irec,buffup)) != NULL) fatal(p); }

	batchp=buffup; *buffup='\0';

	linep=fgets(line,linenbytes-1,fpinp);
	if (!linep) fatal("!v999! expected");
	if (line[0] == '\n') {
	    while (linep)
		if (line[0] == '\n') linep=fgets(line,linenbytes-1,fpinp);
		else break;
	    continue;
	}
	line[strlen(line)-1]='\0';

NEWTAG:
	if (sscanf(line,"!v%"_LD_"!",&idtag) != 1) fatal(line);
	for (n=2; ; n++) if (!isdigit(line[n])) break;
	if (idtag < 0 || idtag > USHRT_MAX) fatal(line);
	sprintf(batchp,"A%"_LD_"%c",idtag,0x01); batchp+=strlen(batchp);
	strcpy(batchp,line+n+1); batchp+=strlen(batchp);

	while (linep) {
	    linep=fgets(line,linenbytes-1,fpinp);
	    if (line[0] == '\n') linep=fgets(line,linenbytes-1,fpinp);
	    line[strlen(line)-1]='\0';
	    if (!linep) {
		*batchp++=0x01;
		break;
	    }
	    if (sscanf(line,"!ID %7"_LD_"\n",&dummy) == 1) {
		*batchp++=0x01;
		break;
	    }
	    if (sscanf(line,"!v%"_LD_"!",&idtag) == 1) {
		*batchp++=0x01;
		goto NEWTAG;
	    }

	    strcpy(batchp,line); batchp+=strlen(batchp);
	}

        /*
	    output the current master file record
	*/
	if (*buffup) {
	    *batchp='\0';

	    if ((p=fldupdat(irec,buffup)) != NULL) fatal(p);
	    MFRmfn = (idmfn) ? idmfn: mfn+parmoffset;

	    if (append) MFRmfn=VMF0nxtmfn(crec);
	    MFRstatus=ACTIVE; /* good */
            
	    if (recupdat(crec,irec)) fatal("recupdat");
	    count2++;
	}

    } /* end of main loop */


    /* libera areas */
    FREE(buffup);
    FREE(line);
    FREE(vrecp[crec]); vrecp[crec]=NULL; nrecs--;
    FREE(vrecp[irec]); vrecp[irec]=NULL; nrecs--;
    fclose(fpinp);

    if (parmtell) fprintf(stderr,"+++ %"_LD_"/%"_LD_" recs\n",count1,count2);


    exit(0);
}
