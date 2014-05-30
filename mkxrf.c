/*
mkxrf.c         -       Make a .XRF MicroISIS file from its .MST data file

                        Skip EACHSKIP bytes and keep trying to find a valid
                        MicroISIS master file record leader whenever the one
                        just read do not pass the consistency criteria.

                        Up to the point the execution ends the .XRF is ok.

                        A.O.Tardelli, BRM/1989
*/

#include <stdio.h>
#include <string.h>

#include "cisis.h"      /* CISIS Interface */
#include "cirun.h"      /* runtime area and defines by AOT */

#define EACHSKIP        2
#define FIRSTCOMB       1
#define FIRSTCOMP       64

#define M0BSIZ          sizeof(M0STRU)

#define TRACE1          0
#define TRACE2          0

char ctlarea[BUFSIZ];

void main(argc,argv)
int argc;
char *argv[];
{
    RECSTRU *recp; /* mandatory for defines REC RDB MF0 MFR DIR, FIELDP */

    unsigned char *dbnp,line[CIMPL+1];
    LONGX lastmfn;

    LONGX comb;      /* gdb getmfr comb */
    int comp;       /* gdb getmfr comp */

    LONGX xrftiv;    /* .xrf pointer */

    int n,fd,nr;
#if CNV_PCFILES
    unsigned char *p;
#endif

    off_t nbytes;
    LONGX nxtmfb,nxtmfn,ok=0L,totread=0L;
    int  nxtmfp,x;

#if CNV_PCFILES
    char unibuff[LEADER];
#endif


    if (argc < 2) {
        printf("%s",cicopyr("Utility MKXRF"));
        printf("\n");
        printf("mkxrf <dbn> \n"); 
        printf("\n");
	exit(1);
    }
    
    dbnp=argv[1];

    comb=FIRSTCOMB; comp=FIRSTCOMP;

    fd=dbxopen("",dbnp,".mst");
    nbytes=LSEEK64(fd,0L,SEEK_END);

#if 0
    printf("Max MFN: ");
    if (scanf("%"_LD_,&nxtmfn) != 1) fatal("Max MFN");
    nxtmfn++;
    nxtmfb=nbytes/MSBSIZ;
    nxtmfp=MSBSIZ;
#else
    LSEEK64(fd,0L,SEEK_SET);
    recp=(RECSTRU *)ctlarea;
    if (CIREAD(fd,MFX,M0BSIZ) != M0BSIZ) fatal("read/ctlmfn");
#if CNV_PCBINUM
    ConvertMST_CTLSTRUCT(MFX);
#endif

    printf("\n");
    prtcontrol(recp,dbnp);
    printf("\nok ?  ");
    if (strcmp(gets(line),"ok")) fatal("ok expected");
    printf("\n");

    nxtmfn=MF0nxtmfn;
    nxtmfb=MF0nxtmfb;
    nxtmfp=MF0nxtmfp;
    if (nxtmfn < 1 || nxtmfn > MAXUPDMFN)
        fatal("invalid nxtmfn - use CTLMFN first");
    if (nxtmfb < 1 || nxtmfb > nbytes/MSBSIZ)
        if (nxtmfb != nbytes/MSBSIZ+1 || nxtmfp != 1)
            fatal("invalid nxtmfb/nxtmfp - use CTLMFN first");
    if (nxtmfp < 1 || nxtmfp > MSBSIZ)
        fatal("invalid nxtmfp - use CTLMFN first");
#endif

    CLOSE(fd);

    recisis0_m=0; /* recisis0() keep .mst */
    if (recisis0(dbnp) <= 0) fatal("mkxrf/cannot reinitialize your .XRF");

    lastmfn=0;

    RECORD(nrecs,dbnp,1L); /* RECdbxp and mstsetup() */
    dbxopenw("",RDBname,xx1extp,&RDBxropn,&RDBxropw,"xropn/w"); /* recwxref() */
    fd=RDBmsopn;

    if (LSEEK64(fd,(off_t)comp,SEEK_SET) != comp)
        fatal("mkxrf/lseek");
    totread+=comp;


LOOP:

#if TRACE1
    printf("\n+++ comb=%"_LD_"  comp=%d \n",comb,comp);
#endif

    while (comp >= MSBSIZ) {
        comb++;
        comp-=MSBSIZ;
#if TRACE1
        printf("new comb=%"_LD_"  comp=%d \n",comb,comp);
#endif
        if ((comb%10) == 0)
            fprintf(stderr,"+++ %"_LD_" blks, %"_LD_" recs\n",comb,ok);
    }

    if (comp > (MSBSIZ-MSNVSPLT)) {
        comb++;
        LSEEK64(fd,(off_t)(MSBSIZ-comp),SEEK_CUR);
        comp=0;
#if TRACE1
        printf("now comb=%"_LD_"  comp=%d \n",comb,comp);
#endif
        if ((comb%10) == 0)
            fprintf(stderr,"+++ %"_LD_" blks, %"_LD_" recs\n",comb,ok);
    }

    if (comb >= nxtmfb)
        if (comp+LEADER >= nxtmfp) {
            fprintf(stderr,"+++ %"_LD_" blks, %"_LD_" recs\n",comb,ok);
            printf("end of mkxrf\n");
            exit(0);
        }

#if CNV_PCFILES
    if ((n=CIREAD(fd,unibuff,LEADER)) == LEADER) {
	p=MFX;
        memmove(p+0,unibuff+0,4); /* mfn= */
        memmove(p+4,unibuff+6,4); /* mfbwb= */
        memmove(p+8,unibuff+4,2); /* mfrl= */
        memmove(p+10,unibuff+10,LEADER-10); /* LEADER was read */
    } else {
#else
    if ((n=CIREAD(fd,MFX,LEADER)) != LEADER) {
#endif
        fprintf(stderr,"+++ %"_LD_" blks, %"_LD_" recs\n",comb,ok);
        /* fatal("read/LEADER"); */
        printf("end of mkxrf [read/[LEADER=%d]=%d] [totread=%"_LD_"]\n",LEADER,n,totread);
        exit(0);
    }

#if CNV_PCBINUM
    ConvertMST_LEADER(MFX);
#endif    

    if (MFRmfn < 1 || MFRmfn >= nxtmfn ||
        MFRmfrl < LEADER || MFRmfrl > RECnbytes  ||
        MFRbase > MFRmfrl ||
        MFRmfbwb < 0 || MFRmfbwb > comb ||
        MFRmfbwp < 0 || MFRmfbwp > MSBSIZ ||
        MFRnvf < 0 ||
        MFRstatus != ACTIVE && MFRstatus != DELETED ||
        MFRbase != (LEADER + MFRnvf*sizeof(DIRSTRU))) {
#if TRACE2
        if (argc > 2)
            printf(
            "*** check (%"_LD_"/%d) - mfn=%"_LD_" mfrl=%d base=%d (%d x %d)\n",
                comb,comp,MFRmfn,MFRmfrl,MFRbase,MFRnvf,sizeof(DIRSTRU));
#endif
        LSEEK64(fd,(off_t)(-(LEADER-EACHSKIP)),SEEK_CUR);
        comp+=EACHSKIP; totread+=EACHSKIP;
        goto LOOP;
    }

    totread+=LEADER;

    if ((nr=CIREAD(fd,MFX+LEADER,(n=MFRmfrl-LEADER))) != n) {
        fprintf(stderr,"+++ %"_LD_" blks, %"_LD_" recs\n",comb,ok);
        /* fatal("read/n"); */
        printf("end of mkxrf [read/[n=%d]=%d] [totread=%"_LD_"]\n",n,nr,totread);
        exit(0);
    }

    for (x=0; x < MFRnvf; x++) {
        if (DIRpos(x) + DIRlen(x) + MFRbase > MFRmfrl) {
#if TRACE2
            if (argc > 2)
                printf(
                "*** check dir - mfn=%"_LD_" dir=%d (tag=%"_LD_" pos=%"_LD_" len=%"_LD_")\n",
		    MFRmfn,x+1,
		    (LONGX)DIRtag(x),(LONGX)DIRpos(x),(LONGX)DIRlen(x));
#endif
            LSEEK64(fd,(off_t)(-(MFRmfrl-EACHSKIP)),SEEK_CUR);
            comp+=EACHSKIP; totread+=EACHSKIP;
	    totread-=LEADER;
            goto LOOP;
	}
    }

    totread+=nr;
    ok++;

    if (argc > 2)
        printf("#%"_LD_": %"_LD_"/%d - %"_LD_",%d,%"_LD_",%d,%d,%d,%d\n",
                ok,comb,comp,
                MFRmfn,MFRmfrl,MFRmfbwb,MFRmfbwp,MFRbase,
                MFRnvf,MFRstatus);

    if (argc > 3)
        prtfields(recp,MFRmfn);

    xrftiv = comb * XRXDIVIDE + comp;
    if (MFRstatus == DELETED)
        xrftiv= -xrftiv;

    recwxref(recp,xrftiv,lastmfn);

    if (MFRmfn > lastmfn)
        lastmfn=MFRmfn;

    comp+=(MFRmfrl);

    goto LOOP;
}
