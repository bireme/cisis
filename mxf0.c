/* Copyright (c) 1990, 1991 Adalberto Otranto Tardelli. All rights reserved.
 * Written by A.O.Tardelli 7/90
 * Redistribution and use in source and binary forms are freely permitted
 * provided that the above copyright notice and attribution and date of work
 * and this paragraph are duplicated in all such forms.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 * This program gets the number of records, status, tag and chars frequencies
 * of a MicroISIS master file and writes the results in a record that is
 * appended in another master file.
 * Note that the resulting master file should not be exported as an ISO-2709
 * file, because fields with tags greater than 1000 are used.
 * Currently works with CISIS Interface 1.1 and TurboC 2.0 (Huge model)
*/

/* ------------------------------ mxf0.c ----------------------------------- */

#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <string.h>

#if 1
#include "cisis.h"   /* CISIS Interface header file */
#include "cirun.h"   /* CISIS Interface runtime declarations */
#else
#include "\cisis\include\cisis.h"   /* CISIS Interface header file */
#include "\cisis\include\cirun.h"   /* CISIS Interface runtime declarations */
#endif

/* program parameters */
UBYTE dbname[CIMPL+1],line[CIMPL+1],dbnout[CIMPL+1];
UBYTE *dbnamep,*dbnoutp;
LONGX parmtnrecs=0L;
int parmedit;
LONGX parmtell;
LONGX parmfrom,parmto,parmloop,parmcount;
int parmtbytes;


/* cisis/update working area */
UBYTE buffup[MAXMFRL+MAXMFRL+BUFSIZ];
UBYTE buffid[BUFSIZ];
#define FLDUPDLM 0x01


typedef struct freqstru {
    int  tag;
    LONGX freqtag;
    LONGX freqocc;
    int  minsize;
    int  maxsize;
    LONGX totbytes;
    char newdoc;
    struct freqstru *nextp;
} FREQSTRU;

FREQSTRU *freqhp,*freqtagp,*freqp,*freqlp;

LONGX freqch[256];
LONGX freqrc[RCPDEL+1];


/* prototypes */
#if ANSI
int getparms(int argc, char *argv[]);
FREQSTRU *freqalloc(void);
#else
int getparms();
FREQSTRU *freqalloc();
#endif


void main(argc,argv)
int argc;
char *argv[];
{
RECSTRU *recp;          /* mandatory for defines REC RDB MF0 MFR DIR, FIELDP */
LONGX irec;
LONGX upcrec,upirec;
int loopdir,xdir,tag,len;
UBYTE *batchp;

    int rc,xfd;
    LONGX mfn;
    LONGX wtime;
    struct tm *t;
    unsigned char c,*p;
    int loop;

    LONGX count=0L;

    getparms(argc,argv);
    if (parmtell) fprintf(stderr,"+++ input data base:  %s\n",dbnamep);
    if (parmtell) fprintf(stderr,"+++ output data base: %s\n",dbnoutp);


    /*
	create output data base
    */
    if (strncmp(dbnoutp,"create=",7) == 0) {
	rc=recisis0((dbnoutp+=7));
	/* if (parmtell) fprintf(stderr,"+++ recisis0=%d\n",rc); */
    }
    else {
	if (strncmp(dbnoutp,"append=",7) == 0) dbnoutp+=7;
	dbxopt_fatal=0;
	if ((xfd=dbxopen("",dbnoutp,mx1extp)) <= 0) {
	    rc=recisis0(dbnoutp);
	    /* fprintf(stderr,"+++ recisis0=%d\n",rc); */
	}
	else
	    CLOSE(xfd);
    }

    /*
        get output data base control record and write this execution
    */

    RECORD((upcrec=nrecs),dbnoutp,0L);

    time(&wtime);
#if MPE
    system("setvar TZ 'TMZ1XXX'");
#endif

    sprintf(batchp=buffid,"A1001\n%s\n",dbnamep);
    batchp+=strlen(batchp);

    t=localtime(&wtime);
    sprintf(batchp,"A1003\n%04d%02d%02d %02d:%02d:%02d %.3s\n",
        t->tm_year+1900,t->tm_mon+1,t->tm_mday,
        t->tm_hour,t->tm_min,t->tm_sec,
        ctime(&wtime));

    strcpy(buffup,buffid);


    /*
        choose the next output master file record "shelf"
        (it's a good programming pratice !!!)
    */
    upirec=nrecs;
    recallok(upirec,MAXMFRL+MAXMFRL);

    /*
        get the next output master file record and make it active
        (at this point, defines MF0 are still valid !!!)

    */
    RECORD(upirec,dbnoutp,MF0nxtmfn); RECrc=RCNORMAL; MFRstatus=ACTIVE;


    /*
        call the field update routine and create master file record
    */
    if ((p=fldupdat(upirec,buffup)) != NULL)
        fatal(p);
    recupdat(upcrec,upirec);


    /*
        inicializa frequencias
    */
    freqhp=NULL;
    for (loop=256; loop--; )
        freqch[loop]=0;
    freqrc[RCNORMAL]=freqrc[RCLDEL]=freqrc[RCPDEL]=0;


    /*
        choose next record "shelf" for processing and tabulate the data
    */
    for (irec=nrecs, mfn=parmfrom; ; mfn+=parmloop) {

        if (parmto) if (mfn > parmto) break;
        if (parmcount) if (count > parmcount) break;

        RECORD(irec,dbnamep,mfn);
        if (RECrc == RCEOF)
            break;

        count++;
        freqrc[RECrc]++;

        if (RECrc != RCNORMAL)
            continue;

        for (freqp=freqhp; freqp; freqp=freqp->nextp)
            freqp->newdoc=1;

        for (loopdir=MFRnvf, xdir=0; loopdir--; xdir++) {

            tag=DIRtag(xdir); len=DIRlen(xdir);

            for (freqtagp=NULL, freqp=freqhp; freqp; freqp=freqp->nextp)
                if (freqp->tag == tag) {
                    freqtagp=freqp;
                    break;
                }

            if (!freqtagp) {
                freqtagp=freqalloc();
                if (!freqhp) {
                    freqhp=freqtagp;
                    freqtagp->nextp=NULL;
                }
                else {
                    for (freqlp=freqp=freqhp; freqp; freqp=freqp->nextp)
                        if (freqp->tag > tag)
                            break;
                        else
                            freqlp=freqp;
                    freqtagp->nextp=freqp;
                    if (freqhp == freqp)
                        freqhp=freqtagp;
                    else
                        freqlp->nextp=freqtagp;
                }
                freqtagp->tag=tag;
                freqtagp->newdoc=1;
            }

            if (freqtagp->newdoc == 1) {
                freqtagp->freqtag++;
                freqtagp->newdoc=0;
            }
            freqtagp->freqocc++;

            if (len < freqtagp->minsize)
                freqtagp->minsize=len;
            if (len > freqtagp->maxsize)
                freqtagp->maxsize=len;

            freqtagp->totbytes+=len;

            for (p=FIELDP(xdir), loop=len; loop--; p++) {
                    c= *p;
                    freqch[c]++;
            }
        }

        if (parmtell)
            if (!(count % parmtell))
                fprintf(stderr,"+++ %"_LD_" recs\n",count);

    }


    /*
        setup frequencies results via 'A' commands for fldupdat
    */
    if (parmedit)
        sprintf(batchp=buffup,
       "A1009\n%9"_LD_"\n A1010\n%9"_LD_"\n A1011\n%9"_LD_"\n A1012\n%9"_LD_"\n A1013\n%9"_LD_"\n",
             count,freqrc[RCNORMAL],freqrc[RCLDEL],freqrc[RCPDEL],mfn);
    else
        sprintf(batchp=buffup,
            "A1009\n%"_LD_"\n A1010\n%"_LD_"\n A1011\n%"_LD_"\n A1012\n%"_LD_"\n A1013\n%"_LD_"\n",
             count,freqrc[RCNORMAL],freqrc[RCLDEL],freqrc[RCPDEL],mfn);
    batchp+=strlen(batchp);

    if (parmtbytes) {
        LONGX tbytes=0;
        for (freqp=freqhp; freqp; freqp=freqp->nextp) tbytes+=freqp->totbytes;
        if (parmedit)
            sprintf(batchp,"A1014\n^tall^n%10"_LD_"\n",tbytes);
        else
            sprintf(batchp,"A1014\n^tall^n%"_LD_"\n",tbytes);
        batchp+=strlen(batchp);
    }

    for (freqp=freqhp; freqp; freqp=freqp->nextp) {
        if (parmedit)
            sprintf(batchp,"A1020\n^t%03d^d%9"_LD_"^o%9"_LD_"^l%5d^u%5d^n%10"_LD_"\n",
                freqp->tag,
                freqp->freqtag,freqp->freqocc,
                freqp->minsize,freqp->maxsize,
                freqp->totbytes);
        else
            sprintf(batchp,"A1020\n^t%03d^d%"_LD_"^o%"_LD_"^l%d^u%d^n%"_LD_"\n",
                freqp->tag,
                freqp->freqtag,freqp->freqocc,
                freqp->minsize,freqp->maxsize,
                freqp->totbytes);
        batchp+=strlen(batchp);
    }

    for (loop=0; loop<256; loop++)
        if (freqch[loop]) {
            if (parmedit)
                sprintf(batchp,"A1030\n^tall^x%02x^n%10"_LD_"\n",
                                                        loop,freqch[loop]);
            else
                sprintf(batchp,"A1030\n^tall^x%02x^n%"_LD_"\n",
                                                        loop,freqch[loop]);
            batchp+=strlen(batchp);
        }


    /*
        call the field update routine and update master file record
    */
    if ((p=fldupdat(upirec,buffup)) != NULL)
        fatal(p);

    recupdat(upcrec,upirec);


    if (parmtell) fprintf(stderr,
        "mxf0: %"_LD_" recs\n",count);

    if (count == parmtnrecs)
        exit(0);
    else
        exit(1);

}




FREQSTRU *freqalloc()
{
    FREQSTRU *freqp;
    unsigned char *p;
    int loop;

    freqp=(FREQSTRU *)malloc(sizeof(FREQSTRU));
    if (freqp == NULL)
        fatal("erro ao alocar FREQSTRU");

    for (p=(unsigned char *)freqp, loop=sizeof(FREQSTRU); loop--; )
        *p++ = NULL;

    freqp->minsize=INT_MAX;
    freqp->maxsize=INT_MIN;

    return(freqp);
}



int getparms(argc,argv)                                                     /*
    --------                                                                */
int argc;
char *argv[];
{
    int argnext=1;
    UBYTE *p;

    if (argc < 3) {
        printf("%s",cicopyr("Utility MXF0"));
        printf("\n");
	printf("mxf0 <dbname> [create=]<dbnout> [<tnrecs>]");
	printf(" [noedit] [tell=<n>] [total] \n");
	printf("\n");
	printf(" <dbname> input master file \n");
	printf(" <dbnout> output master file for results \n");
	printf(" <tnrecs> expected number of input records (default=%"_LD_") \n",parmtnrecs);
	printf(" \n");
	printf(" noedit   suppress all leading spaces in the output");
	printf(" data fields\n");
	printf(" tell=<n> produce a message to the stderr every each <n>");
	printf(" input records\n");
	printf(" \n");
	printf(" The following tags are output to <dbnout>:\n");
	printf(" \n");
	printf("  1001: input master file name\n");
	printf("  1003: date & time stamp\n");
	printf("  1009: total number of records\n");
	printf("  1010: number of active records\n");
	printf("  1011: number of logically deleted records\n");
	printf("  1012: number of physically deleted records\n");
	printf("  1013: next MFN to be assigned\n");
	printf("  1020: ^tTAG ^dDOCS ^oOCCS ^lMINLEN ^uMAXLEN ^nDATA BYTES\n");
	printf("  1030: ^tall ^xCHRCODE ^nCHRFREQ \n");
	printf("\n");
	printf(" Exit code is 0 if the total number of records is");
	printf(" equal to <tnrecs> \n");
	printf("\n");
	exit(1);
    }

    dbnamep=argv[argnext++];
    dbnoutp=argv[argnext++];
    p=argv[argnext++];
    if (p) if (sscanf(p,"%"_LD_,&parmtnrecs) != 1) fatal(p);

    parmedit=1;
    parmtell=0;

    parmfrom=1; parmto=0; parmloop=1; parmcount=LONGX_MAX;
    parmtbytes=0;

    for (; argnext < argc; argnext++) {
        p=argv[argnext];

        if (strcmp(p,"noedit") == 0) {
            parmedit=0;
            continue;
        }

        if (strncmp(p,"tell=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&parmtell) != 1)
                fatal(p);
            continue;
        }

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
        if (strncmp(p,"loop=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&parmloop) != 1)
                fatal(p);
            continue;
        }
        if (strncmp(p,"count=",6) == 0) {
            if (sscanf(p+6,"%"_LD_,&parmcount) != 1)
                fatal(p);
            continue;
        }

        if (strcmp(p,"total") == 0) {
            parmtbytes=1;
            continue;
        }

        fatal(p);
    }

    return(0);
}

