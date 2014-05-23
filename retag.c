/* Program RETAG: retag field tags, in place
                  or mfunlock
*/
#include <stdio.h>
#include <string.h>
#include "cisis.h"   /* CISIS Interface header file */
#include "cirun.h"   /* CISIS Interface runtime declarations */
#define TRACEX  0
#define TRACEY  0
#define TRACEW  01
#define MAXNVF (MAXMFRL/sizeof(DIRSTRU))
int tag1[MAXNVF];
int tag2[MAXNVF];
int ntags=0;
void main(argc,argv)
int argc;
char *argv[];
{
    RECSTRU *recp;      /* mandatory for defines REC,MFR,DIR,FIELDP */
    LONGX irec,mfn;
    int dirloop,loop;
    DIRSTRU *dirp;
    char *dbnp;
    int rc;
    FILE *fp;
    int changed,tag;
    int iarg;
    char *p;
    LONGX parmtell=0;
    int unlock=0,wlen=2;				/* leader + dir */
    LONGX count=0,count2=0;
    LONGX parmfrom=1,parmto=999999999L; 
    int parmshift=0;

    if (argc < 3) {
        printf("%s",cicopyr("Utility RETAG"));
        printf(" \n");
        printf("retag <dbname> {<retag.tab>|unlock} [<option> [...]] \n");
        printf(" \n");
        printf(" <dbname>    master file to be retagged/unlocked \n");
        printf(" <retag.tab> retag table \n");
	printf(" \n");
	printf("options:     {from|to|tell|shift}=<n> \n");
        printf(" \n");
        printf("The retag table is a sequential file in the format: \n");
        printf(" \n");
        printf("   <tag> <new tag>   (max %d entries)\n",MAXNVF);
        printf(" \n");
        printf("The table: \n");
        printf(" \n");
        printf(" 24 240 \n");
        printf(" 70 700 \n");
        printf(" \n");
        printf("changes tag 24 to 240 and 70 to 700 \n");
        printf(" \n");
        exit(1);
    }
    dbnp=argv[1];
    if (strcmp(argv[2],"unlock") != 0) {
        if ((fp=fopen(dbxcipar(NULL,argv[2],'='),"r")) == NULL) fatal(argv[2]);
        for ( ; ; ) {
        if ((rc=fscanf(fp,"%d %d\n",&tag1[ntags],&tag2[ntags])) != 2)
            if (rc == EOF) break;
            else fatal(argv[2]);
#if TRACEX
printf("+++ tag1=%3d  tag2=%3d\n",tag1[ntags],tag2[ntags]);
#endif
            ntags++;
        }
        fclose(fp);
    }
    else unlock=1;
    
    for (iarg=3; iarg < argc; iarg++) {
        p=argv[iarg];
        if (strncmp(p,"from?",5) == 0 || strncmp(p,"from=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&parmfrom) != 1)
                fatal(p);
            continue;
        }
        if (strncmp(p,"to?",3) == 0 || strncmp(p,"to=",3) == 0) {
            if (sscanf(p+3,"%"_LD_,&parmto) != 1)
                fatal(p);
            continue;
        }
        if (strncmp(p,"tell?",5) == 0 || strncmp(p,"tell=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&parmtell) != 1)
                fatal(p);
            continue;
        }
        if (strncmp(p,"shift?",6) == 0 || strncmp(p,"shift=",6) == 0) {
            if (sscanf(p+6,"%d",&parmshift) != 1)
                fatal(p);
            continue;
        }
        if (strcmp(p,"trace") == 0) {
            dbxtrace=rectrace=1;
            continue;
        }
        fatal(p);
    }

    if (parmshift)
	for (ntags=0; ntags < MAXNVF; ntags++) {
	    tag1[ntags] = ntags+1;
	    tag2[ntags] = ntags+1 + parmshift;
	    if (tag2[ntags] < 0) tag2[ntags]=0;
	}
    
    RECORD(irec=nrecs,dbnp,0L); /* RECdbxp and mstsetup() */

    dbxopenw("",RDBname,mx1extp,&RDBmsopn,&RDBmsopw,"msopn/w"); /* recwmast() */

    if (unlock) {
        if (MF0mfcxx2+MF0mfcxx3) {
            printf("unlock: mfcxx2=%"_LD_" mfcxx3=%"_LD_"\n",MF0mfcxx2,MF0mfcxx3);
            MF0mfcxx2=MF0mfcxx3=0;
	    recwmast(recp,NULL,0L,0,0,sizeof(M0STRU));	/* ctl */
            count++;
        }
	wlen=1;						/* leader */
    }
    
    for (loop=0; loop < ntags; loop++)
        if (tag2[loop] == 0) { wlen=0; break; }

    for (mfn=parmfrom; mfn <= parmto; mfn++) {
        recreadl=wlen; recread(recp,mfn);		/* leader/+dir */
        if (RECrc == RCEOF) break;
        if (RECrc == RCPDEL) continue;
        for (changed=0, loop=0; loop < ntags; loop++) {
            tag=tag1[loop];
            for (dirp=MFRdir, dirloop=MFRnvf; dirloop--; dirp++) {
                if (dirp->tag == tag) {
		  if (tag2[loop]) {
                    dirp->tag = tag2[loop];
#if TRACEY
printf("+++ now %3d -> %3d\n",tag,dirp->tag);
#endif
                  } else {
		    char lineup[10];
		    sprintf(lineup,"D%"_LD_,(LONGX)dirp->tag);
		    if (fldupdat(irec,lineup)) fatal(lineup);
		    if (dirloop) { dirloop--; dirp--; }
#if TRACEY
printf("+++ del %3d -> %3d\n",tag,0);
#endif
                  }
                  changed++;
                }
            }
        }
        if (unlock) {
            if (RECgdbw) {
                printf("unlock: mfn=%"_LD_" mfrl=%"_LD_"\n",MFRmfn,(LONGX)MFRmfrl);
#if !MULTI
		MFRmfrl=(UWORD)0-MFRmfrl;
#endif
                changed=1;
            }
        }
        if (changed) {
	    recwmast(NULL,recp,recxrefb,recxrefp,0,wlen); /* leader/+dir */
            count2++;
        }
        if (parmtell)
            if (mfn % parmtell == 0)
		fprintf(stderr,"+++ %"_LD_"/%"_LD_" [%"_LD_"+%"_LD_"] \n",
		    mfn,RDBmsmfn-1,count,count2);
    }
    if (parmtell)
        if ((mfn-1) % parmtell != 0)
	    fprintf(stderr,"+++ %"_LD_"/%"_LD_" [%"_LD_"+%"_LD_"] \n",
		mfn-1,RDBmsmfn-1,count,count2);
    exit(0);

} /* end of main */
