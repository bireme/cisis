/*
CCOPTS = -DINCPRECX=0 -DEXCFMCGI=1 -DEXCFMXML=1 
$(CC) -DINCPROCX=0 $(CCOPTS) wtrig2.c
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#define DIR_FILE 1                  /* list=directory */
#if DIR_FILE
#include <sys/types.h>
#include <dirent.h>
#endif
#include "cisis.h"

#ifndef PROCXSLT
#if UNIX
#define PROCXSLT  0
#else /* UNIX */
#define PROCXSLT  0
#endif /* UNIX */
#endif
#if PROCXSLT
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>
#endif /* PROCXSLT */

#ifndef INCPROCX      // proc Y / proc T (bug if called by cifm3.c)      /* usa CIIFU (+FST(+FMT)) */
#define INCPROCX  1
#define INCPROCXT 1
#endif

#ifndef INCPRECX      // 6words/if= in Gsplit (ciupdsplt.c)              /* usa CITRM */
#define INCPRECX  1
#endif

#ifndef INCPRCCX      // socket client with Gload (ciupdsocx.c)          /* */
#define INCPRCCX  1
#endif

#ifndef INCPRSSX      // server                                          /* */
#define INCPRSSX  0   // 1
#endif

#if INCPRCCX
#ifndef PROCXSOCKREC
#define PROCXSOCKREC 1    // include ciupdsocx.c
#endif /* PROCXSOCKREC */
#endif /* INCPRCCX */


#if CICPP
#if INCPROCX
#include "ciifu.hpp"
#endif
#endif

#if CICPP
#if INCPRECX
#include "citrm.hpp"
#endif
#endif

#ifdef USE_ERROR_SYS
#include <ui_win.hpp>
#include <errorsys.hpp>
extern MY_ERROR_SYSTEM * errsys;
extern MESS_SYSTEM *mess;
extern UIW_WINDOW * wprogress;
#include <textdb.hpp>
#define USE_INFO_SYS
#endif

/* ----------------------------- upd.c ------------------------------ */

#define RUCTRACE    00
#define RUCTRACX    00
#define RUCTRAC2    00
#define RUFTRACE    0
#define RUGTRACE    0
#define RUHTRACE    0
#define RUITRACE    0
#define RUJTRACE    0
#define RUKTRACE    0
#define RUFxRACE    0

#define RUUTRACE    0

#define MULTRACE    1           /* multrace runtime switch */

#if !CICPP

int  recisis0_m=1;              /* recisis0() init .mst */
int  recisis0_s=0;              /* recisis0() system file type */

#endif /* CICPP */

#if !CICPP
#define CIUPD_CISISX_SOURCE 1
#include "cisisx.c"
#endif

#if PROCXSLT
#include "cidump.c"
#endif /* PROCXSLT */ 

#if CICPP
#define NO_IREC  -1L
#endif /* CICPP */

#if MULTI

#if CICPP
int RECSTRU :: xrecunlck(int option)
#else /* CICPP */
int recunlck(irec,option)                                              /*
------------
		    se option = EWLOCK faz EWL unlock
		    se option = DELOCK faz DEL unlock
		    se option = RLOCK faz RL unlock;
		    retorna RCNORMAL ou aborta
								      */
LONGX irec;          /* vrecp[] element defining RECdbxp/MFRmfn */
int option;
#endif /* CICPP */
{
    RECSTRU *recp;
    DBXSTRU *dbxp;
    FFI oldgdbl;
    LONGX mfn;
    int fd,rc;
    FFI n;
    int uok=0,force=0;
    LONGX comb;      /* gdb getmfr comb */
    int comp;       /* gdb getmfr comp */

#if CICPP
#if RUUTRACE
    LONGX irec = NO_IREC;
#endif
    RECSTRU *wrecp=NULL;
    recp=this;
#else
    LONGX wrec;
    if (irec < 0 || irec >= maxnrec) fatal("recunlck/irec");
    if (!nrecs || !vrecp[irec]) fatal("recunlck/recinit");
    recp=vrecp[irec];
#endif

    /* get input information */
    dbxp=RECdbxp;
    mfn=MFRmfn;

    /* keep old RECgdbl */
    oldgdbl=RECgdbl;

    /* allocate working control record / record leader */
#if CICPP
    try { wrecp=new RECSTRU(cisisxp); wrecp->xrecalloc(sizeof(M0STRU)); }
    catch (BAD_ALLOC) { return(-1); }
    wrecp->recdbxp=dbxp;
    recp=wrecp;
#else /* CICPP */
    for (wrec=maxnrec; wrec--; ) {
	if (!vrecp[wrec]) /* ja' decrementado */ break;
    }
    if (wrec < 0) return(-1);
    recallok(wrec,sizeof(M0STRU));
    VRECdbxp(wrec)=dbxp;
    recp=vrecp[wrec];
#endif /* CICPP */


    if (option & FORCE) { force=1; option-=FORCE; }

    if (option & RDELOCK) option = (mfn) ? RLOCK : DELOCK;

    if (option != EWLOCK && option != DELOCK && option != RLOCK)
	fatal("recunlck/option");

#if RUUTRACE
 printf("recunlck - irec=%"_LD_"  dbn=%s/%"_LD_"  unlck=%d\n",irec,DBXname,mfn,option);
#endif

    if (DBXnetws == MONONETS) {
	if (multrace)
	    printf("<u> %s/%"_LD_" [%s MONONETS]\n",
	      DBXname,mfn,(option&RDELOCK)?((option==RLOCK)?"RL":"DEL"):"EWL");
	return(RCNORMAL);
    }

    if (!DBXmsopn) fatal("recunlck/msopn");
    dbxopenw(DBXname,DBXname,mx1extp,&DBXmsopn,&DBXmsopw,"recunlck/msopn/w");
    fd=DBXmsopn;

    /* step 1: lock the .mst file and get the ctl rec */

    if (multrace) printf("<u> %s .mst lock \n",DBXname);
#if BEFORE950220
    /* .mst file lock: lock */
    if (dbxflock(dbxp,"M")) fatal("recunlck/file lock");
    /* get the current ctl rec */
    keeplock=REClock; REClock=NOLOCK;
    if (recread(recp,0L) != RCNORMAL) fatal("recunlck/read/ctl");
    REClock=keeplock;
    /* check another's exclusive write lock */
    if (MF0mfcxx3 && !DBXewlxx && !force) {
	if (multrace) printf("<u> %s has exclusive write lock\n",DBXname);
	fatal("recunlck/exclusive write lock");
    }
#else
    if (dbxflock(dbxp,"M")) {
	if (multrace) printf("<u> %s .mst lock/denied \n",DBXname);
	if (dbxewlrc) return(RCLOCK);
	fatal("recunlck/file lock");
    }
    /* wait another's exclusive write lock */
    if (force) {
	if (LSEEK64(DBXmsopn,0L,SEEK_SET) != 0) fatal("recunlck/force/lseek");
	n=CIREAD(DBXmsopn,MFX,sizeof(M0STRU));
#if CNV_PCBINUM
	ConvertMST_CTLSTRUCT(MFX);
#endif
	if (n != sizeof(M0STRU)) {
	    if (dbxewlrc) return(RCLOCK);
	    fatal("recunlck/force/read");
	}
    }
    else
        if (dbxwlock(dbxp,MFX,dbxwloop)) {
	    if (multrace) printf("<u> %s .mst ewl is on !!!\n",DBXname);
	    if (dbxewlrc) return(RCLOCK);
	    fatal("recunlck/must wait ewl");
        }
    /* copy MFX to what will be written */
    memmove(DBXmsibp,MFX,sizeof(M0STRU));
#endif

    /* check data entry lock */
    if (option & RDELOCK && (!MF0mfcxx2 || !DBXdelxx) && !force) {
	if (multrace)
	    printf("<u> %s/mfcxx2=%"_LD_" delxx=%d\n",DBXname,MF0mfcxx2,DBXdelxx);
	fatal("recunlck/data entry lock is off");
    }

    /* step 2: logical unlock */

    if (option == EWLOCK || option == DELOCK) {         /* EWL or DEL */
	if (option == EWLOCK) {
	    /* unflag exclusive write lock */
	    if (MF0mfcxx3 || DBXewlxx) {
		if (MF0mfcxx3) MF0mfcxx3--;
		if (DBXewlxx) DBXewlxx--;
		/* update input buffer */
		((M0STRU *)DBXmsibp)->m0mfcxx3=MF0mfcxx3;
		uok=1;
	    }
	}
	else {
	    /* unflag data entry lock */
	    if (MF0mfcxx2 || DBXdelxx) {
		if (MF0mfcxx2) MF0mfcxx2--;
		if (DBXdelxx) DBXdelxx--;
		/* update input buffer */
		((M0STRU *)DBXmsibp)->m0mfcxx2=MF0mfcxx2;
		uok=1;
	    }
	}
	/* write it back */
	if (uok) {
	    if (LSEEK64(fd,0L,SEEK_SET) != 0)
		fatal("recunlck/lseek/ctl");
#if DBXMSTXL /* AOT 18/06/2002 */
    if (DBXmstxl) {
		((M0STRU *)DBXmsibp)->m0mftype += DBXmstxl * 256;
    }
#endif
#if CNV_PCBINUM
	    memmove(cnv_pcbuff,DBXmsibp,sizeof(M0STRU));
	    ConvertMST_CTLSTRUCT(cnv_pcbuff);
	    if (CIWRITE(fd,cnv_pcbuff,sizeof(M0STRU)) != sizeof(M0STRU))
#else
	    if (CIWRITE(fd,DBXmsibp,sizeof(M0STRU)) != sizeof(M0STRU))
#endif
		fatal("recunlck/write/ctl");
#if DBXMSTXL /* AOT 18/06/2002 */
    if (DBXmstxl) {
		((M0STRU *)DBXmsibp)->m0mftype &= 0x00FF;
    }
#endif
	}

    } /* end of data entry lock */

    else {                                              /* RL */

	/* get its xref to rewrite the leader segment */
	recreadl=1; /* leader only */
	rc=recread(recp,mfn);
	comb=recxrefb; comp=recxrefp;
#ifndef USE_ERROR_SYS
	if (rc == RCEOF || rc == RCPDEL) fatal("recunlck/EOFPDEL");
#else
	if (!(rc == RCEOF || rc == RCPDEL)) {
#endif

	/* reset existing record lock and call recwmast */
	if (oldgdbl || (force && RECgdbw)) {
	    if (multrace)
	      printf("<u> %s/%"_LD_" mfrl=%04x=+%d\n",DBXname,mfn,RECgdbl,MFRmfrl);
	    /* write back the leader segment */
#if CICPP
	    recwmast(NULL,comb,comp,0,1); /* leader only */
#else
	    recwmast(NULL,recp,comb,comp,0,1); /* leader only */
#endif /* CICPP */
	    uok=1;
	}
#ifdef USE_ERROR_SYS
		}
#endif
    } /* end of record unlock */

    /* step 3: .mst file unlock */

    if (multrace)
	printf("<u> %s .mst unlock [%s %s]\n",DBXname,
	      (option&RDELOCK)?((option==RLOCK)?"RL":"DEL"):"EWL",
	      (uok)?"off":"unlock denied");

    if (dbxulock(dbxp,"M")) fatal("recunlck/file unlock");

#if CICPP
    if (wrecp) delete wrecp;
    recp=this;
#else
#if BEFORE20000809
    if (wrec) if (vrecp[wrec]) {
#else
    if (wrec >= 0) if (vrecp[wrec]) {
#endif
	FREE(vrecp[wrec]); vrecp[wrec]=NULL; nrecs--;
    }
    recp=vrecp[irec];
#endif

    if (uok) {
	RECgdbl=RECgdbw=0;
	return(RCNORMAL);
    }

    return(RCEOF);
}
#endif /* MULTI */


#if CICPP
int RECSTRU :: xrecupdat(void)
#else /* CICPP */
int recupdat(crec,irec)                                                 /*
------------
		    abenda se vrecp nao inicializado;
		    abenda se registros crec/irec ainda nao alocados;
		    abenda se tipo de crec diferente de TYPEMF0;
		    abenda se mfn menor do que 1 ou maior do que MAXMFN;
		    seta dbxp de irec igual ao de crec;
		    chama recwrite();
		    retorna RCNORMAL
								      */
LONGX crec;          /* indice de vrecp, para recwrite() */
LONGX irec;          /* indice de vrecp, para recwrite() */
#endif /* CICPP */
{

    RECSTRU *recp, *crecp;
    LONGX mfn;
    int rc;

    if (rectrace) multrace=1;

#if CICPP
    try { crecp = new RECSTRU(cisisxp); }
    catch (BAD_ALLOC) { fatal("recupdat/ALLOC/crecp"); }

    crecp->xrecalloc(sizeof(M0STRU));
    recp =this;
    crecp->xrecord(RDBname,0L);

    recp=crecp;		/* tmp */

#else /* CICPP */

    if (!nrecs)
	fatal("recupdat/RECINIT");

    if (crec < 0 || crec >= maxnrec) fatal("recupdat/crec/index");
    if (irec < 0 || irec >= maxnrec) fatal("recupdat/irec/index");

    recp=vrecp[crec];	/* tmp */

#endif /* CICPP */

    if (MF0nxtmfn < 1) fatal("recupdat/crec/nxtmfn");

    if (MF0nxtmfb < 1) fatal("recupdat/crec/nxtmfb");

    if (MF0mftype != MSMFTUSR &&
	MF0mftype != MSMFTMSG) fatal("recupdat/crec/mftype");

    if (MF0nxtmfn == 1)
        if (MF0mftype != MSMFTMSG)
            if (MF0nxtmfp <= sizeof(M0STRU)) fatal("recupdat/crec/nxtmfp/1");

    if (MF0nxtmfp > MSBSIZ) fatal("recupdat/crec/nxtmfp/MSBSIZ");

#if CICPP
    recp=this;
#else /* CICPP */
    crecp=vrecp[crec]; recp=vrecp[irec];
#endif /* CICPP */

    if (!crecp || !recp)
	fatal("recupdat/RECALLOC");

    if (crecp->rectype != TYPEMF0)
	fatal("recupdat/TYPEMF0");

    if ((mfn=MFRmfn) < 1 || mfn > MAXUPDMFN)
#if MULTI
      if (!(RECwlock & NEWREC && MFRmfn == 0))
#endif
	fatal("recupdat/mfn");

#if !CICPP
#if RUCTRACE || RUCTRAC2
printf("recupdat - crec=%"_LD_"  irec=%"_LD_"  dbn(crec)=%s  mfn=%"_LD_"  opw=%d,%d\n",
 crec,irec,VRDBname(crec),mfn,VRDBxropw(crec),VRDBmsopw(crec));
#endif
#endif /* CICPP */

    RECdbxp=crecp->recdbxp;		/* get dbxp from control record */
#if CICPP
    rc=recwrite(crecp);
#else
    rc=recwrite(crecp,recp);
#endif /* CICPP */

#if RUCTRACE
#if CICPP
    recp=crecp;
#else
    recp=vrecp[crec];
#endif /* CICPP */
    printf("recupdat - RECrc=%d  Ctlmfr: %"_LD_",%"_LD_",%"_LD_",%d,%d,%"_LD_"%"_LD_"%"_LD_"%"_LD_"\n",
	RECrc,
	MF0ctlmfn,MF0nxtmfn,MF0nxtmfb,MF0nxtmfp,
	MF0mftype,MF0reccnt,MF0mfcxx1,MF0mfcxx2,MF0mfcxx3);
#if CICPP
    recp = this;
#else
    recp=vrecp[irec];
#endif /* CICPP */
    printf("recupdat - RECrc=%d  Leader: %"_LD_",%d,%"_LD_",%d,%d,%d,%d\n",
	RECrc,MFRmfn,MFRmfrl,MFRmfbwb,MFRmfbwp,MFRbase,MFRnvf,MFRstatus);
    printf("recupdat - rc=%d\n",rc);
#endif
#if !CICPP
#if RUCTRAC2
printf("recupdat : crec=%"_LD_"  irec=%"_LD_"  dbn(crec)=%s  mfn=%"_LD_"  opw=%d,%d\n",
 crec,irec,VRDBname(crec),mfn,VRDBxropw(crec),VRDBmsopw(crec));
#endif
#endif /* CICPP */

#if CICPP
    delete crecp;
#endif /* CICPP */

    return(rc);
}


#if CICPP
int RECSTRU :: xrecwrite(RECSTRU *crecp)
#else /* CICPP */
int recwrite(crecp,recp)                                              /*
------------
		    seta dbxp e ms0p desce crecp;
		    seta mfn desde recp;
		    abenda de dbxp diferentes;
		    obtem ptr para .mst e seu (mfn) status em .xrf;
		    regrava o registro recp;
		    altera e regrava o registro crecp;
		    mantem .xrf;
		    retorna RCNORMAL ou aborta
								      */
RECSTRU *crecp;     /* elemento de vrecp, para info/store nxtmfn,mfb,mfp */
RECSTRU *recp;      /* elemento de vrecp, para info/store mfbwb,mfbwp  */
#endif /* CICPP */
{
    DBXSTRU *dbxp;
    M0STRU *ms0p;   /* defines MS0 */
    M1STRU *mshp;   /* defines MSH */

    LONGX mfn;
    LONGX lastmfn;
    int newblk;

    int rc;
    FFI n;
    XRPTR xrftiv;    /* gdb .xrf pointer */
    LONGX comb;      /* gdb getmfr comb */
    int comp;        /* gdb getmfr comp */

    M1STRU header;  /* to read LEADER bytes */

#if CNV_PCFILES
    char unibuff[MSNVSPLT]; /* CNV_PCFILES/recwrite - mshp */
#endif

    int pend;       /* if update is pending */
    int pendnew;    /* if update is pending - newrec */
    int pendupd;    /* if update is pending - update */

    LONGX thiscomb;
    off_t xbyte;
    int thiscomp;
    LONGX nxtcomp;

    int flagnew;        /* record will be written as a newrec */
    unsigned inplace;   /* NEVER or MAYBE */

#if MULTI
    int isnewrec;
#if BEFORE950220
    int keeplock;
#endif
#endif

#if DBXMSTXL
int pow, vtot;
#endif

#define NEVER 0
#define MAYBE 1

#if CICPP
    RECSTRU *recp = this;
#endif /* CICPP */

    dbxp=crecp->recdbxp;

#if BEFORE950724
    if (!DBXxropn) fatal("recwrite/xropn");
    if (!DBXmsopn) fatal("recwrite/msopn");
#else
    if (!DBXxropn || !DBXmsopn) mstsetup(DBXname,0L,0L);
#endif

    dbxopenw(DBXname,DBXname,xx1extp,&DBXxropn,&DBXxropw,"recwrite/xropn/w");
    dbxopenw(DBXname,DBXname,mx1extp,&DBXmsopn,&DBXmsopw,"recwrite/msopn/w");

    if (!DBXxropw) fatal("recwrite/xropw");
    if (!DBXmsopw) fatal("recwrite/msopw");

    if (!DBXxribp) fatal("recwrite/xribp");
    if (!DBXmsibp) fatal("recwrite/msibp");

#if CICPP
    ms0p= &crecp->recmfp->m0;
#else
    ms0p= &crecp->recmf.m0;
#endif /* CICPP */

    if (MS0mftype != MSMFTUSR && MS0mftype != MSMFTMSG)
	fatal("recwrite/mftype");

    mfn=MFRmfn;

#if RUCTRACE
printf("recwrite - dbn/%d=%s  mfn=%"_LD_"  nxtmfn=%"_LD_"  nxtmfb=%"_LD_"  nxtmfp=%d\n",
 MS0mftype,DBXname,mfn,MS0nxtmfn,MS0nxtmfb,MS0nxtmfp);
#endif

#if MULTI
    isnewrec=RECwlock & NEWREC;

    if (DBXnetws != MONONETS) {
	if ((RECwlock & WUNLOCK && RECwlock & WLOCK) ||
	   !(RECwlock & WUNLOCK || RECwlock & WLOCK))
	    fatal("recwrite/WLOCK/WUNLOCK");
	/* step 1: lock the .mst file and get the current ctl rec */
	/* assure data entry lock has been granted (also ewl) */
	if (!DBXdelxx) fatal("recwrite/lock/data entry lock is off");
	/* assure record lock has been granted */
	if (!isnewrec)
	    if (!RECgdbl) fatal("recwrite/lock/record lock needed");
	/* .mst file lock: lock */
	if (multrace) printf("<w> %s .mst lock \n",DBXname);
#if BEFORE950220
	if (dbxflock(dbxp,"M")) fatal("recwrite/lock/file lock");
	/* reset internal lock flag and get the current ctl rec */
	keeplock=crecp->reclock; crecp->reclock=NOLOCK;
	if (recread(crecp,0L) != RCNORMAL) fatal("recwrite/lock/read");
	crecp->reclock=keeplock;
	/* check another's exclusive write lock - should never occur */
	if (MS0mfcxx3 && !DBXewlxx) {
	    if (multrace) printf("<w> %s has exclusive write lock\n",DBXname);
	    fatal("recwrite/lock/exclusive write lock");
	}
#else
	if (dbxflock(dbxp,"M")) {
	    if (multrace) printf("<w> %s .mst lock/denied \n",DBXname);
	    if (dbxewlrc) return(RCLOCK);
	    fatal("recwrite/lock/file lock");
	}
	/* wait another's exclusive write lock */
	if (dbxwlock(dbxp,(char *)ms0p,dbxwloop)) {
	    if (multrace) printf("<w> %s .mst ewl is on !!!\n",DBXname);
	    if (dbxewlrc) return(RCLOCK);
	    fatal("recwrite/lock/must wait ewl");
	}
#endif

	/* check data entry lock */
	if (!MS0mfcxx2) fatal("recwrite/lock/data entry lock/mfcxx2");
	if (!DBXdelxx) fatal("recwrite/lock/data entry lock needed");

	/* assign nxtmfn */
	if (isnewrec) {
	    if (!mfn) {
	        mfn=MFRmfn=MS0nxtmfn;
	        if (multrace) printf("<w> %s -> mfn=%"_LD_"\n",DBXname,MFRmfn);
	    }
	}
    }
#endif /* MULTI */

    lastmfn=MS0nxtmfn-1;

    if (RECdbxp != dbxp)
	fatal("recwrite/dbxp");

    rc=recxref(recp,mfn,&comb,&comp);

    if (comb > MS0nxtmfb)
	fatal("recwrite/chknew/comb");
    if (comp >= MSBSIZ)
	fatal("recwrite/chknew/comp");

#if DBXMSTXL
    pendnew=recxrefn;
    pendupd=recxrefm;
#else
    xrftiv=DBXxribp->xrmfptr[(mfn-1)%XRMAXTIV]; /* not available for mfn=0 */

    pendnew=(int)(labs(xrftiv) & XRXMASKN);
    pendupd=(int)(labs(xrftiv) & XRXMASKU);
#endif

    pend=(pendnew | pendupd);
    if (pendnew && pendupd)
	fatal("recwrite/pend");

    if (MS0mftype == MSMFTMSG) {
	pend=1;                         /* to force inplace=MAYBE */
	pendnew=1;                      /* and mfbwb/mfbwp=0      */
    }

#if RUCTRACE
printf("recwrite - comb=%"_LD_"  comp=%d  new=%d  upd=%d  pend=%d  rc=%d\n",
comb,comp,pendnew,pendupd,pend,rc);
#endif


    mshp= &header;
    if (rc == RCEOF || rc == RCPDEL) {
#if MULTI
	if (DBXnetws != MONONETS) {
	    if (multrace) printf("<w> %s/%"_LD_" rc=%d\n",DBXname,MFRmfn,rc);
	    if (!isnewrec) fatal("recwrite/lock/EOFPDEL");
	}
#endif /* MULTI */
	if (pend)
	    if (MS0mftype != MSMFTMSG) /* 07/03/97 */
	        fatal("recwrite/chknew/pend/EOFPDEL");

	memset((char *)mshp,0x00,LEADER);

	MSHstatus=DELETED;
	comb=MS0nxtmfb;
	comp=MS0nxtmfp-1;
#if RUCTRACE
printf("recwrite - comb=%"_LD_"  comp=%d [%"_LD_,"%"_LD_"+%d]\n",
 comb,comp,MS0nxtmfn,MS0nxtmfb,MS0nxtmfp);
#endif
    }
    else {
#if MULTI
	if (multrace) printf("<w> %s/%"_LD_" rc=%d\n",DBXname,MFRmfn,rc);
	if (DBXnetws != MONONETS)
	    if (isnewrec)
		if (RECwlock & FORCE) ; else fatal("recwrite/lock/NORMALDEL");
#endif /* MULTI */
	xbyte=(((off_t)(comb-1))<<MSBSHIFT)+comp;
	if (LSEEK64(DBXmsopn,xbyte,SEEK_SET) != xbyte)
	    fatal("recwrite/lseek/leader");
	n=CIREAD(DBXmsopn,(char *)mshp,LEADER); /* MSNVSPLT */
#if CNV_PCBINUM
	ConvertMST_LEADER((char *)mshp,0,LEADER);
#endif
#if CNV_PCFILES
	memmove(unibuff,(char *)mshp,MSNVSPLT);
	memmove(((char *)mshp)+0,unibuff+0,4); /* mfn= */
	memmove(((char *)mshp)+4,unibuff+6,4); /* mfbwb= */
	memmove(((char *)mshp)+8,unibuff+4,2); /* mfrl= */
#endif
	if (n != LEADER) {
#if RUCTRACE
printf("recwrite - fd=%d  lseek=%"P_OFF_T"  n=%d\n",DBXmsopn,(LONG_LONG)xbyte,n);
#endif
	    fatal("recwrite/read");
	}
	if (MSHmfn != mfn)
	    fatal("recwrite/check/mfn");
	if (MS0mftype == MSMFTMSG)
	    MSHmfrl=MSBSIZ;                     /* to force inplace=MAYBE */
#if MULTI
	/* step 2: check existing record lock */
	if (DBXnetws != MONONETS) {
	    if (multrace)
	      printf("<w> %s/%"_LD_" mfrl=%04x=%d\n",DBXname,mfn,MSHmfrl,MSHmfrl);
	    if (!isnewrec)
	      if (MSHmfrl <= (FFI)MFRL_MAX) fatal("recwrite/lock/record lock");
	    if (MSHmfrl > (FFI)MFRL_MAX) {
	      MSHmfrl=(FFI)0-MSHmfrl;
	      if (multrace)
	       printf("<w> %s/%"_LD_" mfrl=%04x=+%d\n",DBXname,mfn,0x0000,MSHmfrl);
	    }
	}
#endif /* MULTI */
    }

#if RUCTRACE
printf("recwrite - rec: %"_LD_",%d,%"_LD_",%d,%d,%d,%d\n",
	MFRmfn,MFRmfrl,MFRmfbwb,MFRmfbwp,MFRbase,MFRnvf,MFRstatus);
printf("recwrite - mst: %"_LD_",%d,%"_LD_",%d,%d,%d,%d\n",
	MSHmfn,MSHmfrl,MSHmfbwb,MSHmfbwp,MSHbase,MSHnvf,MSHstatus);
#endif

    if (rc == RCEOF || rc == RCPDEL)
	if (MFRstatus == DELETED)
	    fatal("recwrite/deleted");

    if (rc == RCEOF || rc == RCPDEL || pendnew)
	flagnew=1;
    else
	flagnew=0;


    if (pend) {

	MFRmfbwb=MSHmfbwb;
	MFRmfbwp=MSHmfbwp;
	inplace=MAYBE;
    }

    else {              /* no if update is pending */

	if (MSHmfbwb != 0 && MSHmfbwp != 0)
	    fatal("recwrite/mfbwlink");

	if (rc != RCNORMAL) {                                   /* 26/09/90 */
	    MFRmfbwb=0;
	    MFRmfbwp=0;
	    inplace=MAYBE;
	}
	else {                  /* cancel xrfptr/add */
	    MFRmfbwb=comb;
	    MFRmfbwp=comp;
	    inplace=NEVER;
	}
    }

    if (MS0mftype != MSMFTMSG) {
	if (MFRmfrl & 0x01)
	    MFX[MFRmfrl++]=' '; /* gdb always an even number */
#if DBXMSTXL
    vtot = 1;
    for (pow = 1; pow <= DBXmstxl; pow++) {
        vtot *= 2;
    }
    vtot -= 1;
    while (MFRmfrl & vtot)
        MFX[MFRmfrl++]=' ';

/*	if (DBXmstxl == 2) {
	    while (MFRmfrl & 0x03)
	        MFX[MFRmfrl++]=' ';
	}
	if (DBXmstxl == 3) {
	    while (MFRmfrl & 0x07)
	        MFX[MFRmfrl++]=' ';
	} */
#endif
    }

#if MULTI
    /* step 3: check actual maximum record length */
    if (DBXnetws != MONONETS) {
	if (MFRmfrl > (FFI)MFRL_MAX) fatal("recwrite/lock/mfrl");
    }
#endif /* MULTI */


#if RUCTRACE
printf("recwrite - mfbwb=%"_LD_"  mfbwp=%d  inplace=%d  flagnew=%d\n",
 MFRmfbwb,MFRmfbwp,inplace,flagnew);
#endif


    if (MFRmfrl <= MSHmfrl && inplace == MAYBE) {
	thiscomb=comb;
	thiscomp=comp;
	newblk=0;
    }
    else {
	thiscomb=MS0nxtmfb;
	thiscomp=MS0nxtmfp-1;
#if DBXMSTXL
	/*if (thiscomp & 0x01 ||
	   (DBXmstxl == 2 && thiscomp & 0x03) ||
	   (DBXmstxl == 3 && thiscomp & 0x07))*/

    if (thiscomp & 0x01 || (thiscomp & vtot))
#else
	if (thiscomp & 0x01)
#endif
	    fatal("recwrite/check/even");

	newblk=0;
	if (thiscomp > (MSBSIZ-MSNVSPLT)) {
	    thiscomb++;
	    thiscomp=0;
	    newblk=1;
	}

	MS0nxtmfb=thiscomb;
	nxtcomp= (MS0mftype == MSMFTMSG) ? MSBSIZ : thiscomp+MFRmfrl;
	for (;;)
	    if (nxtcomp < MSBSIZ)
		break;
	    else {
		MS0nxtmfb++;
		nxtcomp-=MSBSIZ;
		newblk=1;
	    }
	if (nxtcomp > (MSBSIZ-MSNVSPLT)) {              /* 17/01/91 */
	    MS0nxtmfb++;
	    nxtcomp=0;
	    newblk=1;
	}
	MS0nxtmfp=nxtcomp+1;    /* gdb first = sizeof(M0STRU)+1 */

	if (MS0nxtmfp == 1)
	    newblk=0;
	if (thiscomp == 0)
	    newblk=1;
	if (MS0mftype == MSMFTMSG)
	    newblk=1;
    }


    if (mfn >= MS0nxtmfn) {
	MS0nxtmfn=mfn+1;
	DBXmsmfn=MS0nxtmfn;
    }

#if RUCTRACE
printf("recwrite - thiscomb=%"_LD_"  thiscomp=%d\n",thiscomb,thiscomp);
printf("recwrite - MS0nxtmfn=%"_LD_"  MS0nxtmfb=%"_LD_"  MS0nxtmfp=%d\n",
 MS0nxtmfn,MS0nxtmfb,MS0nxtmfp);
#endif


#if MULTI
    /* step 4: process record lock/unlock */
    if (DBXnetws != MONONETS) {
	/* process WUNLOCK/WLOCK */
	MFRmfrl=(FFI)0-MFRmfrl;
	RECgdbl=MFRmfrl; RECgdbw=1;
	/* reset or keep record lock */
	if (RECwlock & WUNLOCK) {
	    if (multrace)
	      printf("<w> %s/%"_LD_" mfrl=%04x=%d\n",DBXname,mfn,RECgdbl,MFRmfrl);
	    MFRmfrl=(FFI)0-MFRmfrl;
	    if (multrace)
	      printf("<w> %s/%"_LD_" mfrl=%04x=+%d\n",DBXname,mfn,RECgdbl,MFRmfrl);
	    RECgdbl=RECgdbw=0;
	}
    }
#endif /* MULTI */

#if CICPP
    recwmast(crecp,thiscomb,thiscomp,newblk,0);
#else
    recwmast(crecp,recp,thiscomb,thiscomp,newblk,0);
#endif /* CICPP */

#if MULTI
    /* step 5: process existing record lock */
    if (DBXnetws != MONONETS) {
	if (MFRmfrl > (FFI)MFRL_MAX) {
	    if (multrace)
	      printf("<w> %s/%"_LD_" mfrl=%04x=%d\n",DBXname,mfn,MFRmfrl,MFRmfrl);
	    if (!RECgdbl || !RECgdbw) fatal("recupdat/check/gdbl");
	    MFRmfrl=(FFI)0-MFRmfrl;
	    if (multrace)
	      printf("<w> %s/%"_LD_" mfrl=%04x=+%d\n",DBXname,mfn,MFRmfrl,MFRmfrl);
	}
    }
#endif /* MULTI */

    /* encode .xrf pointer for the record location */
    comb = thiscomb;
    comp = thiscomp;
    if (flagnew)
        comp |= XRXMASKN;
    else
        comp |= XRXMASKU;

#if DBXMSTXL
    xrftiv = comb * (XRXDIVIDE>>DBXmstxl) + (comp>>DBXmstxl);
#else
    xrftiv = comb * XRXDIVIDE + comp;
#endif

    /* complement .xrf pointer for logically deleted records */
    if (MFRstatus == DELETED)
	xrftiv= -xrftiv;

#if CICPP
    recwxref(xrftiv,lastmfn);
#else
    recwxref(recp,xrftiv,lastmfn);
#endif /* CICPP */    

#if MULTI
    /* step 6: .mst file lock: unlock */
    if (DBXnetws != MONONETS) {
	if (multrace) printf("<w> %s .mst unlock [RL off]\n",DBXname);
	if (dbxulock(dbxp,"M")) fatal("recwrite/lock/file unlock");
    }
#endif /* MULTI */


    return(RCNORMAL);

#undef NEVER
#undef MAYBE
}


#if CICPP
int RECSTRU :: xrecwmast(RECSTRU *crecp,
			 LONGX     comb,
			 int      comp,
			 int      newblk,
			 FFI      wlen)
#else /* CICPP */
int recwmast(crecp,recp,comb,comp,newblk,wlen)                        /*
------------
		    seta dbxp e ms0p desde crecp;
		    regrava o registro recp na posicao comb/comp;
		    preenche ultimo .mst block, se for o caso;
		    reseta DBXmsibp->msbufn;
		    regrava o registro crecp;
		    retorna RCNORMAL ou aborta
								      */
RECSTRU *crecp;     /* elemento de vrecp, para info */
RECSTRU *recp;      /* elemento de vrecp, para info */
LONGX comb;          /* gdb getmfr comb */
int comp;           /* gdb getmfr comp */
int newblk;         /* new .mst blk switch */
FFI wlen;           /* 0=MFRmfrl, 1=leader, 2=+dir, n=mfrl */
#endif /* CICPP */
{
    DBXSTRU *dbxp;

    FFI n; /* deveria ser int para pegar o retorno da CIWRITE */
    int k;
    off_t xbyte;

    FFI mfrl;

#if CNV_PCFILES
    char unibuff[MSNVSPLT]; /* CNV_PCFILES/recwmast - MFX */
#endif

#if DBXMSTXL
    UWORD keepmftype;
#endif

#if CICPP
    RECSTRU *recp = this;
#endif /* CICPP */

    if (crecp) dbxp=crecp->recdbxp; else dbxp=RECdbxp;

    xbyte=((((off_t)(comb-1))<<MSBSHIFT)+comp);

#if RUCTRACE
printf("recwmast - fd=%d  lseek=%"P_OFF_T"  newblk=%d\n",
 DBXmsopn,(LONG_LONG)xbyte,newblk);
#endif

    if (recp)
	if (LSEEK64(DBXmsopn,xbyte,SEEK_SET) != xbyte)
	    fatal("recwmast/lseek/rec");

    if (wlen) {
	if (wlen == 1) mfrl=LEADER;		/* leader */
	else if (wlen == 2) mfrl=MFRbase;	/*  + dir */
	else mfrl=wlen;				/* waall! */
    }
    else {
	mfrl=MFRmfrl;
#if MULTI
	if (DBXnetws != MONONETS)
	    if (MFRmfrl > (FFI)MFRL_MAX)
		mfrl=(FFI)0-MFRmfrl;
#endif /* MULTI */
    }

    /* retag.c calls recwmast(recp,NULL,0,0,0,sizeof(M0STRU)) */
    if (recp) {
#if CNV_PCFILES
	if (MFRmfn > 0) {
	    memmove(unibuff,MFX,MSNVSPLT);
	    memmove(MFX+0,unibuff+0,4); /* mfn= */
	    memmove(MFX+6,unibuff+4,4); /* mfbwb= */
	    memmove(MFX+4,unibuff+8,2); /* mfrl= */
	}
#endif
#if CNV_PCBINUM
	memmove(cnv_pcbuff,MFX,mfrl);
	if (MFRmfn > 0) {
	    k=MFRnvf; /* CNV_PCFILES ok */
	    ConvertMST_LEADER(cnv_pcbuff,0,LEADER);
	    if (wlen != 1) ConvertMST_DIR(cnv_pcbuff,k);
	}
	else ConvertMST_CTLSTRUCT(cnv_pcbuff);
        if ((n=CIWRITE(DBXmsopn,cnv_pcbuff,mfrl)) != mfrl) {
#else
        if ((n=CIWRITE(DBXmsopn,MFX,mfrl)) != mfrl) {
#endif
#if RUCTRACE
printf("recwmast - write/rec  n=%d  mfrl=%d\n",n,mfrl);
#endif
	    fatal("recwmast/write/rec");
        }
#if CNV_PCFILES
	if (MFRmfn > 0)
	    memmove(MFX,unibuff,MSNVSPLT);
#endif
    }

    if (newblk) {
	k=MSBSIZ-((xbyte+mfrl)&XRXMASK);

#if RUCTRACE
printf("recwmast - fill=%d\n",k);
#endif
	memset(DBXmsibp->msbuff,0x00,(size_t)k);

	if ((n=CIWRITE(DBXmsopn,DBXmsibp->msbuff,k)) != k) { /* 0x00 */

#if RUCTRACE
printf("recwmast - write/end  n=%d  k=%d\n",n,k);
#endif
	    fatal("recwmast/write/end");
	}
    }

    DBXmsibp->msbufn=0;                         /* reset .mst input buffer */

    if (!crecp) return(RCNORMAL);

    LSEEK64(DBXmsopn,0L,SEEK_SET);

    recp=crecp; /* tmp for MFX */

#if DBXMSTXL /* AOT/AARG 05/01/99 */
    if (DBXmstxl) {
        keepmftype=MF0mftype;
        MF0mftype += DBXmstxl * 256;
    }
#endif

#if CNV_PCBINUM
    memmove(cnv_pcbuff,MFX,sizeof(M0STRU));
    ConvertMST_CTLSTRUCT(cnv_pcbuff);
    n=CIWRITE(DBXmsopn,cnv_pcbuff,sizeof(M0STRU));
#else
    n=CIWRITE(DBXmsopn,MFX,sizeof(M0STRU));
#endif
    if (n != sizeof(M0STRU)) {
#if RUCTRACE
printf("recwmast - write/ctl  n=%d\n",n);
#endif
	fatal("recwmast/write/ctl");
    }

#if DBXMSTXL /* AOT/AARG 05/01/99 */
    if (DBXmstxl) {
        MF0mftype=keepmftype;
    }
#endif

    return(RCNORMAL);
}


#if CICPP
int RECSTRU :: xrecwxref(XRPTR    pointer,
			 LONGX     lastmfn)
#else /* CICPP */
int recwxref(recp,pointer,lastmfn)                                     /*
------------
		    seta dbxp/mfn desde recp;
		    atualiza .xrf;
		    retorna RCNORMAL ou aborta
								      */
RECSTRU *recp;      /* elemento de vrecp, para info */
XRPTR pointer;      /* .xrf pointer para mfn */
LONGX lastmfn;       /* ultimo mfn em .xrf */
#endif /* CICPP */
{
    DBXSTRU *dbxp;
    LONGX mfn;

    FFI n,k,j;
    int flag127;
    off_t xbyte;
    LONGX x;
    char *p;

    int thisidx,lastidx;
    XRPOS thispos,lastpos;
    XRPTR xrftiv;
#if CNV_PCBINUM
    XRSTRU *xrp;
#endif

    LONGX wcomb;
    int  wcomp;

#if CICPP
    RECSTRU *recp = this;
#endif /* CICPP */

    dbxp=RECdbxp;
    mfn=MFRmfn;

    lastpos=(lastmfn+XRMAXTV1)/XRMAXTIV;
    if (lastpos < 1)
	lastpos=1;
    flag127=0;
    if (lastmfn)
	if (mfn > lastmfn && (lastmfn % XRMAXTIV) == 0)         /* 127 ok */
	    { lastpos++; flag127=1; }

    thispos=    (mfn+XRMAXTV1)/XRMAXTIV;

    lastidx=(lastmfn-1)%XRMAXTIV;
    if (flag127)
	lastidx = -1;   /* 21/01/91 */

    thisidx=    (mfn-1)%XRMAXTIV;

#if RUCTRACX
printf("recwxref - lastmfn=%"_LD_"             \n",lastmfn);
printf("recwxref - lastpos=%"_LD_"  thispos=%"_LD_"\n",lastpos,thispos);
printf("recwxref - lastidx=%d   thisidx=%d \n",lastidx,thisidx);
#endif

    if (!lastmfn) recxref(recp,1L,&wcomb,&wcomp);       /* 28/01/91 */

    if (thispos < lastpos) {

#if RUCTRACX
printf("recwxref - thispos < lastpos\n");
#endif

#if BEFORE940815
	if (recxref(recp,mfn,&wcomb,&wcomp) == RCEOF)
	    fatal("recwxref/recxref/thispos");
	if (DBXxribp->xrxrpos != thispos)
	    fatal("recwxref/check/thispos");
	DBXxribp->xrmfptr[thisidx]=pointer;
#else
	if (DBXxribp->xrxrpos == thispos) DBXxribp->xrmfptr[thisidx]=pointer;
#endif

	xbyte=(off_t)( (((off_t)(thispos-1))<<XRSHIFT)+XRPOSSIZ+thisidx*XRPTRSIZ );
	if (LSEEK64(DBXxropn,xbyte,SEEK_SET) != xbyte)
	    fatal("recwxref/lseek/thispos");
#if BEFORE970319
	p=(char *)&DBXxribp->xrmfptr[thisidx];
#else
	p=(char *)&pointer;
#endif

#if CNV_PCBINUM
	memmove(cnv_pcbuff,p,XRPTRSIZ);
	ConvertXRF_PTR(cnv_pcbuff);
	if ((n=CIWRITE(DBXxropn,cnv_pcbuff,XRPTRSIZ)) != XRPTRSIZ) {
#else
	if ((n=CIWRITE(DBXxropn,p,XRPTRSIZ)) != XRPTRSIZ) {
#endif
#if RUCTRACX
printf("recwxref - rewrite/mfptr  n=%d\n",n);
#endif
	    fatal("recwxref/rewrite/mfptr");
	}
    }

    else /* 21/01/91 */

    if (thispos > lastpos) {

#if RUCTRACX
printf("recwxref - thispos > lastpos\n");
#endif

	if (lastmfn)
	    if (recxref(recp,lastmfn,&wcomb,&wcomp) == RCEOF)
		fatal("recwxref/recxref/lastmfn");

	x = (flag127) ? (lastpos-1) : (-lastpos);
	if (DBXxribp->xrxrpos != x)
	    fatal("recwxref/xrpos/lastpos");

	DBXxribp->xrxrpos=lastpos;        /* unflag last block */

#if DBXMSTXL /* AOT, 07/04/2003 */
    xrftiv = (-1) * (XRXDIVIDE>>DBXmstxl) + ((0)>>DBXmstxl);  /* PDEL pointer */
#else
	xrftiv=(-1)*XRXDIVIDE + (0);      /* PDEL pointer */
#endif

	for (n=lastidx+1; n<XRMAXTIV; n++)
	    DBXxribp->xrmfptr[n] = xrftiv;

	xbyte=(((off_t)(lastpos-1))<<XRSHIFT);
	if (LSEEK64(DBXxropn,xbyte,SEEK_SET) != xbyte)
	    fatal("recwxref/lseek/lastpos");
#if CNV_PCBINUM
	memmove(cnv_pcbuff,DBXxribp,XRBSIZ);
	ConvertXRF_REC(cnv_pcbuff);
	if ((n=CIWRITE(DBXxropn,cnv_pcbuff,XRBSIZ)) != XRBSIZ) {
#else
	if ((n=CIWRITE(DBXxropn,(char *)DBXxribp,XRBSIZ)) != XRBSIZ) {
#endif
#if RUCTRACX
printf("recwxref - rewrite/lastpos  n=%d\n",n);
#endif
	    fatal("recwxref/rewrite/lastpos");
	}


	while (++lastpos < thispos) {

	    DBXxribp->xrxrpos = lastpos;

	    for (n=0; n < XRMAXTIV; )
		DBXxribp->xrmfptr[n++] = xrftiv;

#if CNV_PCBINUM
	    memmove(cnv_pcbuff,DBXxribp,XRBSIZ);
	    ConvertXRF_REC(cnv_pcbuff);
	    if ((n=CIWRITE(DBXxropn,cnv_pcbuff,XRBSIZ)) != XRBSIZ) {
#else
	    if ((n=CIWRITE(DBXxropn,(char *)DBXxribp,XRBSIZ)) != XRBSIZ) {
#endif
#if RUCTRACX
printf("recwxref - write/midpos  n=%d\n",n);
#endif
		fatal("recwxref/write/midpos");
	    }
	}


	DBXxribp->xrxrpos= -thispos;      /* flag last block */

	for (n=0; n<thisidx; n++)
	    DBXxribp->xrmfptr[n] = xrftiv;

	DBXxribp->xrmfptr[thisidx] = pointer;

	xrftiv= (0)*XRXDIVIDE + (0);      /* EOF pointer */

	for (n=thisidx+1; n<XRMAXTIV; n++)
	    DBXxribp->xrmfptr[n] = xrftiv;

#if CNV_PCBINUM
	memmove(cnv_pcbuff,DBXxribp,XRBSIZ);
	ConvertXRF_REC(cnv_pcbuff);
	if ((n=CIWRITE(DBXxropn,cnv_pcbuff,XRBSIZ)) != XRBSIZ) {
#else
	if ((n=CIWRITE(DBXxropn,(char *)DBXxribp,XRBSIZ)) != XRBSIZ) {
#endif
#if RUCTRACX
printf("recwxref - write/thispos  n=%d\n",n);
#endif
	    fatal("recwxref/write/thispos");
	}
    }


    else /* 21/01/91 */

    if (thispos == lastpos) {

#if RUCTRACX
printf("recwxref - thispos == lastpos\n");
#endif

	recxref(recp,mfn,&wcomb,&wcomp);

	if (labs(DBXxribp->xrxrpos) != thispos)
	    fatal("recwxref/check/lastpos");

	k=j=0;
#if DBXMSTXL /* AOT, 07/04/2003 */
    xrftiv = (-1) * (XRXDIVIDE>>DBXmstxl) + ((0)>>DBXmstxl);  /* PDEL pointer */
#else
	xrftiv=(-1)*XRXDIVIDE + (0);      /* PDEL pointer */
#endif

	for (n=lastidx+1; n<thisidx; n++) {
	    DBXxribp->xrmfptr[n] = xrftiv;
	    k+=XRPTRSIZ; j++;
	}

	DBXxribp->xrmfptr[thisidx] = pointer;
	k+=XRPTRSIZ;

	xbyte=(off_t)( (((off_t)(thispos-1))<<XRSHIFT)+XRPOSSIZ+(thisidx-j)*XRPTRSIZ );
	if (LSEEK64(DBXxropn,xbyte,SEEK_SET) != xbyte)
	    fatal("recwxref/lseek/mfptrs");
	p=(char *)&DBXxribp->xrmfptr[thisidx-j];

#if CNV_PCBINUM
	xrp=(XRSTRU *)cnv_pcbuff;
	memmove(cnv_pcbuff,p,k);
	for (n=0; n <= j; n++) /* (j+1) ptr's*/
	    ConvertXRF_PTR(&cnv_pcbuff[n*XRPTRSIZ]);
	if ((n=CIWRITE(DBXxropn,cnv_pcbuff,k)) != k) {
#else
	if ((n=CIWRITE(DBXxropn,p,k)) != k) {
#endif
#if RUCTRACX
printf("recwxref - rewrite/mfptrs  k=%d  n=%d\n",k,n);
#endif
	    fatal("recwxref/rewrite/mfptrs");
	}
    }


    if (mfn > lastmfn && (mfn % XRMAXTIV) == 0) {       /* 127 ok */

#if RUCTRACX
printf("recwxref - thispos == lastpos 127\n");
#endif

	if (recxref(recp,mfn,&wcomb,&wcomp) == RCEOF)   /* 08/08/91 */
	    fatal("recwxref/recxref/mfn127");
//printf("mfn=%"_LD_" comp=%"_LD_" comb=%"_LD_"\n", mfn, (LONGX)wcomp, (LONGX)wcomb);
	if (DBXxribp->xrxrpos != -thispos) {
	    printf("*** %"_LD_"/%"_LD_" ",DBXxribp->xrxrpos,thispos);
	    fatal("recwxref/xrpos/thispos");
	}

	DBXxribp->xrxrpos=thispos;        /* unflag last block */

	xbyte=(((off_t)(thispos-1))<<XRSHIFT);
	if (LSEEK64(DBXxropn,xbyte,SEEK_SET) != xbyte)
	    fatal("recwxref/lseek/thispos/last");
	/* should write only xrxrpos and lseek next blk */
#if CNV_PCBINUM
	memmove(cnv_pcbuff,DBXxribp,XRBSIZ);
	ConvertXRF_REC(cnv_pcbuff);
	if ((n=CIWRITE(DBXxropn,cnv_pcbuff,XRBSIZ)) != XRBSIZ) {
#else
	if ((n=CIWRITE(DBXxropn,(char *)DBXxribp,XRBSIZ)) != XRBSIZ) {
#endif
#if RUCTRACX
printf("recwxref - rewrite/thispos/last  n=%d\n",n);
#endif
	    fatal("recwxref/rewrite/thispos/last");
	}


	DBXxribp->xrxrpos= -(thispos+1);  /* flag last block */

	xrftiv= (0)*XRXDIVIDE + (0);      /* EOF pointer */

	for (n=0; n < XRMAXTIV; )
	    DBXxribp->xrmfptr[n++] = xrftiv;

#if CNV_PCBINUM
	memmove(cnv_pcbuff,DBXxribp,XRBSIZ);
	ConvertXRF_REC(cnv_pcbuff);
	if ((n=CIWRITE(DBXxropn,cnv_pcbuff,XRBSIZ)) != XRBSIZ) {
#else
	if ((n=CIWRITE(DBXxropn,(char *)DBXxribp,XRBSIZ)) != XRBSIZ) {
#endif
#if RUCTRACX
printf("recwxref - write/newpos  n=%d\n",n);
#endif
	    fatal("recwxref/write/newpos");
	}
    }

    return(RCNORMAL);
}

#include "cihsh.c"   /* CISIS Interface hashing & binary table search source code */
#if CIWTF                                /* WTFUN support */
#if !CICPP
#if !CIAPI
//#include "cihsh.c"   /* CISIS Interface hashing & binary table search source code */
#include "wtrun.c"   /* wtfnew(); wtfset(); wtfdel(); wtnew(); wtexit(); loadcoll() source code */
#endif /* CIWTF */
#endif /* CIAPI */
#endif /* CICPP */

#if INCPRCCX
/* procx.c */
#include "ciupdsocy.c"       /* mainfile() source code */
#if PROCXSOCKREC
#include "ciupdsocx.c"       /* mainclient() source code */
#endif /* PROCXSOCKREC */
#endif /* INCPRCCX */

#if INCPRSSX
#define PROCXSERVREC 1
#if PROCXSERVREC
#if PC
#include "ciupdserwpc.c"
#else
#include "ciupdserw.c"
#endif
#endif /* PROCXSERVREC */
#endif /* INCPRSSX */




#if CICPP
char * RECSTRU :: xfldupdat(char *batchp)
#else /* CICPP */
char *fldupdat(irec,batchp)                                           /*
--------------
		    abenda se vrecp nao inicializado;
		    abenda se registro irec ainda nao alocado;
		    abenda se tipo de irec diferente de TYPEMFR;
		    executa comandos desde batchp sobre registro irec;
		    retorna NULL ou .ptr erro
								      */
LONGX irec;          /* indice de vrecp, para recwrite() */
char *batchp;       /* ptr comandos Dtag ou Atag'string' */
#endif /* CICPP */
{
#if !CICPP
    extern int bugadddel;		/* 13/03/94 */
#endif /* CICPP */
    RECSTRU *recp;

    DIRSTRU *mfdirp,*dirarea=NULL,*dirp,*dp,*dxp;
    char *srcareap=NULL;                                             /* v3.4 */
    FFI realmfrl;

    int maxdirs,dirsleft;
    FFI ndel,nadd;
    FFI vlendel,vlenadd;
    FFI lastpos,nxtpos;
    char *mfp,*nxtp,*newp;

    char *p,c,cc,*errp,*sp,*sxp;
    int tag;
    FFI n,k,loop,reploop;

    int iocc,tagocc;
    LONGX nbytes,mfn;

    int i,j,anychange,nn;
    char tempdir[sizeof(DIRSTRU)];
    int sort=0;

#define STRIPMARKUP 1

#if STRIPMARKUP
    FFI smumaxlen=(FFI)MFRL_MAX;
    FFI fldlenmin=0;
    FFI fldleft,fldqleft,striplen,erased;
    LONGX l1,l2;
    int nn1,nn2;
    int match,endmatch,tagmatch;
    char *xp,*fldp,*fldq;
#endif /* STRIPMARKUP */

/* procx.c */
#define PROCXMFUPDATE 1
#if PROCXMFUPDATE
int xfd/*,n*/,parmcopy;
#if CICPP
RECSTRU *upirecp,*upcrecp;
#else /* CICPP */
LONGX upirec,upcrec;
/*char *p;*/
#endif /* CICPP */
#endif /* PROCXMFUPDATE */

/* procx.c */
#define PROCXWRITEFILE 1
#if PROCXWRITEFILE
char *wfnamp;
int wffd,wflen;
char *wfxp;
char *wfmtspecp;
#define KEEPDBX 1
#if KEEPDBX
LONGX keepndbx;
LONGX idbx;
#endif /* KEEPDBX */
#endif /* PROCXWRITEFILE */

/* procx.c */
#if RECGIZM
#define PROCXGIZMREC 1
#endif
#if PROCXGIZMREC
#if RECDECO
#define PROCXDECOREC 1
#endif
#define PROCXSPLIREC 1
#define PROCXCLPSREC 1
#endif /* PROCXGIZMREC */

/* precx.c */
#if INCPRECX
#define PROCXMARKREC 1
#if PROCXMARKREC
#define upmF1 '1'
#define upmF2 '2'
#define upmF3 '3'
#define upmF4 '4'
#endif /* PROCXMARKREC */
#endif /* INCPRECX */

/* procx.c */
#define PROCXLOADREC 1
#if PROCXLOADREC
#endif /* PROCXLOADREC */


/* procx.c */
#define PROCXDUMPREC 1
#if PROCXDUMPREC
int isgdump=0;
#endif /* PROCXDUMPREC */

/* procx.c */
#define PROCXREADREC 1
#if PROCXREADREC
char *rfnamp;
char *rfmfnp;
LONGX rfmfn;
#if CICPP
RECSTRU *uprrecp,*uprrexp;
#else /* CICPP */
LONGX uprrec,uprrex;
#endif /* CICPP */
int xdir;
RECSTRU *keeprecp;
char *rp;
#endif /* PROCXREADREC */

/* procx.c */
#if !CICPP /* porque falta implementar em c++ */
#define PROCXIFPRESET 1
#if PROCXIFPRESET
char *ifprnamp;
char *ifprmfnp;
LONGX ifprmfn;
#endif /* PROCXIFPRESET */
#endif /* CICPP */

#if INCPROCX /* usa CIIFU (+FST(+FMT)) */
/* procx.c */
#if !CICPP /* porque falta implementar em c++ */
#define PROCXIFUPDATE 1
#if PROCXIFUPDATE
int pstflag=IFUPISIS;
int endup=IFUPWRIT; /* single record */
char *stwp=NULL;
FST_CODE *fstpgmp=NULL;
LONGX maxlk1=1000;
LONGX maxlk2=500;
char *fstspecp=NULL;
char *ifnamp;
char *mfnamp;
char *mfnp;
LONGX ifmfn;
int yfd;
#endif /* PROCXIFUPDATE */
#endif /* CICPP */
#endif /* INCPROCX */

#if INCPROCXT /* usa CIFST(+FMT) */
/* procx.c */
#if !CICPP /* porque falta implementar em c++ */
#define PROCXFSUPDATE 1
#if PROCXFSUPDATE
int tyyopth=0;
#endif /* PROCXFSUPDATE */
#endif /* CICPP */
#endif /* INCPROCXT */

#if CICPP
#define freex(srcareap,dirarea) {\
		if (srcareap) delete[] srcareap; if (dirarea) delete[] dirarea; }
#else /* CICPP */
#define freex(srcareap,dirarea) {\
		  if (srcareap) { \
            FREE(srcareap); srcareap=NULL; } \
          if (dirarea) { \
            FREE(dirarea); dirarea=NULL; } \
        }
#endif /* CICPP */

#if CICPP
#if RUFTRACE
    LONGX irec = NO_IREC;
#endif
    recp=this;
#else /* CICPP */
    if (!nrecs)
	fatal("fldupdat/RECINIT");

    recp=vrecp[irec];
#endif /* CICPP */
    if (!recp)
	fatal("fldupdat/RECALLOC");

    if (RECtype != TYPEMFR)
	fatal("fldupdat/TYPEMFR");

#if RUFTRACE
printf("fldupdat - irec=%"_LD_"  nbytes=%"_LD_"  batchp='%s'\n",
 irec,RECnbytes,batchp);
#endif

#ifndef PROCG
#define PROCG  1
#endif /* PROCG */
#if PROCG
#include "ciupg.c"
#endif /* PROCG */

#if 0
    nbytes = (RECnbytes < MAXMFRL) ? RECnbytes : MAXMFRL;
    nbytes = (RECnbytes < rec_maxmfrl) ? RECnbytes : rec_maxmfrl;
    nbytes = rec_maxmfrl;
#endif
    nbytes = (RECnbytes > rec_maxmfrl) ? RECnbytes : rec_maxmfrl;

#if CICPP
    try { dirarea = (DIRSTRU *) new char [nbytes]; }
    catch (BAD_ALLOC) { dirarea = (DIRSTRU *)ALLONULL; }
#else /* CICPP */
    dirarea=(DIRSTRU *)ALLOC((ALLOPARM)nbytes);
#endif /* CICPP */
    if (dirarea == (DIRSTRU *)ALLONULL)
	fatal("fldupdat/ALLOC/dir");

    maxdirs=nbytes/sizeof(DIRSTRU);
#if CICPP
    try { srcareap = (char *) new char [maxdirs]; }
    catch (BAD_ALLOC) { srcareap = (char *)ALLONULL; }
#else /* CICPP */
    srcareap=(char *)ALLOC((ALLOPARM)maxdirs);                  /* v3.4 */
#endif /* CICPP */
    if (srcareap == (char *)ALLONULL) {
        freex(srcareap,dirarea);
	    fatal("fldupdat/ALLOC/src");
    }

    memset(srcareap,'?',maxdirs);       /* no default */        /* v3.4 */


    dirsleft=MFRnvf;

    realmfrl=MFRbase;

    sp=srcareap; dirp=dirarea;
    for (mfdirp=MFRdir, n=dirsleft; n--; mfdirp++, dirp++, sp++) {
	dirp->tag = mfdirp->tag;
	dirp->pos = mfdirp->pos;
	dirp->len = mfdirp->len;
	*sp = 'r';              /* source is record */          /* v3.4 */
	realmfrl+=mfdirp->len;
    }
#if RUFTRACE
printf("fldupdat - RECrc=%d  Leader: %"_LD_",%d,%"_LD_",%d,%d,%d,%d\n",
 RECrc,MFRmfn,MFRmfrl,MFRmfbwb,MFRmfbwp,MFRbase,MFRnvf,MFRstatus);
printf("fldupdat - dirsleft=%d  realmfrl=%d\n",dirsleft,realmfrl);
for (dirp=dirarea, loop=0; loop<dirsleft; loop++, dirp++)
 printf("fldupdat - dir[%d]: tag=%d  pos=%d  len=%d\n",
  loop,dirp->tag,dirp->pos,dirp->len);
#endif

    MFRmfrl=realmfrl;


    for (ndel=nadd=0, vlendel=vlenadd=0, p=batchp; *p; ) {

	if (isspace(*p))  {
	    p++;
	    continue;
	}

	c=toupper(*p); errp=p++;
	switch (c) {

	case 'S':
		for (tag=0; isdigit(*p); p++) tag=tag*10+(*p-'0');
	    if (*p) {
            freex(srcareap,dirarea);
		    return(errp);
	    }
        if (tag) {
            int len,cmp;
    		for (i=0; i+1 < MFRnvf; i++) {
    		    if (DIRtag(i) != tag) continue;
    		    for (j=i+1; j < MFRnvf; j++) {
    		        if (DIRtag(j) != tag) continue;
                    len=DIRlen(i); if (len>DIRlen(j)) len=DIRlen(j);
                    cmp=memcmp(FIELDP(i),FIELDP(j),len);
        		    if (cmp > 0 || cmp==0 && DIRlen(i) > DIRlen(j)) {
            			memmove(tempdir,&MFRdir[i],sizeof(DIRSTRU));
            			memmove(&MFRdir[i],&MFRdir[j],sizeof(DIRSTRU));
            			memmove(&MFRdir[j],tempdir,sizeof(DIRSTRU));
            		}
                }
        	}
	        break;
        }
	    for (anychange=1; anychange; ) {
    		anychange=0;
    		for (i=0; i+1 < MFRnvf; i++) {
    		    j=i+1;
    		    if (DIRtag(i) > DIRtag(j)) {
        			memmove(tempdir,&MFRdir[i],sizeof(DIRSTRU));
        			memmove(&MFRdir[i],&MFRdir[j],sizeof(DIRSTRU));
        			memmove(&MFRdir[j],tempdir,sizeof(DIRSTRU));
        			anychange=1; sort=1;
        		}
        	}
        }
	    break;

	case '=':
	    for (mfn=0; isdigit(*p); p++) 
		mfn=mfn*10+(*p-'0');
	    if (mfn == 0) {
        freex(srcareap,dirarea);
		return(errp);
	    }
	    MFRmfn=mfn;
	    break;

	case 'D':
	    while (isspace(*p))
		p++;
	    if (*p == '.') {			/* logically deleted */
		MFRstatus=DELETED;
		p++;
		break;
	    }
	    if (*p == ':') {		/* logically deleted/active */
		MFRstatus=(MFRstatus==DELETED)?ACTIVE:DELETED;
		p++;
		break;
	    }
	    if (*p == '*') {
		tag=0;
		p++;
	    }
	    else {
		for (tag=0; isdigit(*p); p++)
		    tag=tag*10+(*p-'0');
		if (tag == 0) {
            freex(srcareap,dirarea);
		    return(errp);
		}
	    }
	    while (isspace(*p))
		p++;
	    if (tag > 0 && *p == '/') {
		p++;
		while (isspace(*p))
		    p++;
		for (tagocc=0; isdigit(*p); p++)
		    tagocc=tagocc*10+(*p-'0');
#if RUJTRACE
printf("fldupdat - D/r - tag=%d  tagocc=%d\n",
 tag,tagocc);
#endif
	    }
	    else
		tagocc=0;
	    for (sp=srcareap, iocc=0, dirp=dirarea, n=dirsleft; n--; ) {
		if (dirp->tag == tag || tag == 0) {
		    if (tagocc) {
			iocc++;
			if (iocc != tagocc) {
			    dirp++; sp++;                       /* v3.4 */
			    continue;
			}
		    }
#if RUJTRACE
printf("fldupdat - D/r - tag=%d  tagocc=%d  iocc=%d\n",
 tag,tagocc,iocc);
#endif
		    if (*sp != 'r') {                           /* v3.4 */
            freex(srcareap,dirarea);
			return(errp);
		    }
#if 1 /* BUGADDDEL */
		    if (bugadddel) if (nadd) {
            freex(srcareap,dirarea);
			return(errp);
		    }
#endif
		    vlendel+=dirp->len;
		    for (sxp=sp, dp=dirp, loop=n; loop--; ) {
			dxp=dp; dp++;
			dxp->tag = dp->tag;
			dxp->pos = dp->pos;
			dxp->len = dp->len;
			*sxp = *(sxp+1); sxp++;                 /* v3.4 */
		    }
		    dirsleft--;
		    ndel++;
#if RUFTRACE
printf("fldupdat - D - tag=%d  dirsleft=%d  vlendel=%d\n",
 tag,dirsleft,vlendel);
for (sxp=srcareap, dp=dirarea, loop=0; loop<dirsleft; loop++, dp++, sxp++)
 printf("fldupdat - dir[%d]: tag=%d  pos=%d(%c)  len=%d\n",
  loop,dp->tag,dp->pos,*sxp,dp->len);
#endif
		}
		else
		    dirp++;
	    }
	    break;

	case 'A':
	case 'H':
#if STRIPMARKUP
	case '<':
        erased=0; endmatch=0;
#endif /* STRIPMARKUP */
	    cc=c;
	    while (isspace(*p))
		p++;
	    for (tag=0; isdigit(*p); p++)
		tag=tag*10+(*p-'0');
	    if (*p == '\0' || tag == 0) {
            freex(srcareap,dirarea);
		    return(errp);
	    }
	    while (*p == ' ')           /* enable any delimiter, but space */
		p++;
/* CMD 'H' */
	    if (c == 'H') {
		for (nn=n=0; isdigit(*p); p++, nn++)
		    n=n*10+(*p-'0');
#if RUKTRACE
printf("+++c=%c tag=%d n=%d *p='%c'\n",c,tag,n,*p);
#endif
		if (!nn || (n && !isspace(*p))) {
            freex(srcareap,dirarea);
		    return(errp);
		}
		if (n) p++;
		p+=n;
	    }
#if STRIPMARKUP
/* CMD '<' */
	    else if (c == '<') {

		nn1=0; l1=0;
		nn2=0; l2=0;
        if (isdigit(*p)) {
            for (; isdigit(*p); ) { l1=l1*10+(*p-'0'); p++, nn1++; }
		    if (nn1) while (isspace(*p)) p++;
            for (; isdigit(*p); ) { l2=l2*10+(*p-'0'); p++, nn2++; }
		    if (nn2) while (isspace(*p)) p++;
        }
        else while (*p) {
            if (*p == '>') break;
            if (strncmp(p,"markmax=",8) == 0) {
                for (p+=8; isdigit(*p); ) { l1=l1*10+(*p-'0'); p++, nn1++; }
                if (nn1) nn1+=8;
                while (isspace(*p)) p++;
                continue;
            }
            if (strncmp(p,"flenmin=",8) == 0) {
                for (p+=8; isdigit(*p); ) { l2=l2*10+(*p-'0'); p++, nn2++; }
                if (nn2) nn2+=8;
                while (isspace(*p)) p++;
                continue;
            }
            break;
        }
		if (*p++ != '>') {
            freex(srcareap,dirarea);
		    return(errp);
		}
        if (nn1) smumaxlen=(FFI)l1;
        if (nn2) fldlenmin=(FFI)l2;
        /*...*/
            for (n=0, fldp=p; *fldp; fldp++, n++) {
                xp=fldp;
                if (*xp++ != '<') continue;
                if (*xp++ != '/') continue;
                nn=0; while (isspace(*xp)) { xp++; nn++; }
                for (tagmatch=0; isdigit(*xp); xp++, nn++)
                    tagmatch=tagmatch*10+(*xp-'0');
                while (isspace(*xp)) { xp++; nn++; }
                if (*xp++ != '>') continue;
                if (tagmatch != tag) continue;
                endmatch=2+nn+1; break;
            }
            if (!endmatch) {
                freex(srcareap,dirarea);
                return(errp);
            }

            fldp=p;
            fldleft=n;
            for (xp=fldp; fldleft > 0; ) {
                if (*fldp != '<') {
                    if (erased) *xp = *fldp; xp++; fldp++; fldleft--; continue;
                }
                for (striplen=1, match=0, fldq=fldp, fldqleft=fldleft; fldqleft > 0; ) {
                    if (striplen > smumaxlen) break;
                    if (*fldq == '>') { match=1; break; }
                    fldq++; fldqleft--; striplen++; continue;
                }
                if (!match) {
                    if (erased) *xp = *fldp; xp++; fldp++; fldleft--; continue;
                }
                fldp+=striplen; fldleft-=striplen; erased+=striplen;
            }
        if (erased) memset(xp,' ',erased);
        /*...*/
		p+=n;
        /*... p+=endmatch; ...*/
        /*... n-=erased; ...*/
        }
#endif /* STRIPMARKUP */
/* CMD 'A' */
/* CMD '{' */
        else {
            c= *p++;
#if RUFxRACE
printf("+++c=%c\n",c);
#endif
            for (n=0; *p; p++, n++)
                if (*p == c) break;
#if RUFxRACE
else printf("+++%d  p=%c\n",n,*p);
printf("+++%d\n",n);
#endif
            if (*p != c) {
                freex(srcareap,dirarea);
                return(errp);
            }
        }
/* END CMD 'A' */
/* END CMD 'H' */
/* END CMD '<' */
/* END CMD '{' */

#if STRIPMARKUP
	    if (cc == '<' && n /*...*/ -erased /*...*/ < fldlenmin) {
        /*...*/ p+=endmatch; /*...*/
        }
        else {
#endif /* STRIPMARKUP */
        sp=srcareap+dirsleft;
	    for (reploop=1, dirp=dirarea+dirsleft; reploop-- ; dirp++, sp++) {
		if (dirsleft >= maxdirs) {
            freex(srcareap,dirarea);
		    return(errp);
		}
		dirp->tag = (UWORD)tag;
		dirp->pos = (FFI)(p-batchp)-n;                  /* v3.4 */
#if STRIPMARKUP
        /*...*/ p+=endmatch; /*...*/
        /*...*/ n-=erased; /*...*/
#endif /* STRIPMARKUP */
		dirp->len = (FFI)n;
		*sp = 'b';      /* source is batchp */          /* v3.4 */
		vlenadd+=dirp->len;
		dirsleft++;
		nadd++;
#if RUFTRACE
printf("fldupdat - A - tag=%d  dirsleft=%d  vlenadd=%d\n",
tag,dirsleft,vlenadd);
for (sxp=srcareap, dp=dirarea, loop=0; loop<dirsleft; loop++, dp++, sxp++)
 printf("fldupdat - dir[%d]: tag=%d  pos=%d(%c)  len=%d\n",
  loop,dp->tag,dp->pos,*sxp,dp->len);
#endif
	    }
#if STRIPMARKUP
        }
	    if (cc != 'H' && cc != '<')
#else /* STRIPMARKUP */
	    if (cc != 'H')
#endif /* STRIPMARKUP */
        p++;
	    break;

	default:
        freex(srcareap,dirarea);
	    return(errp);
	}
    }
#if BEFORE20000426
    if ((MFRmfrl+(nadd-ndel)*sizeof(DIRSTRU)-vlendel+vlenadd) > RECnbytes) {
#else
    if ((LONGX)(MFRmfrl+(nadd-ndel)*sizeof(DIRSTRU)-vlendel+vlenadd) >
        (LONGX)RECnbytes) { /* AOT/RP - 26/04/2000 - para ALPHACPU (IPEN) */
#endif
    freex(srcareap,dirarea);
	return(batchp);
    }

    if (ndel) {
	mfdirp=MFRdir;
	newp=MFX+MFRbase-(k=sizeof(DIRSTRU)*ndel); lastpos=0;
	sp=srcareap;
	for (dirp=dirarea, n=dirsleft; n--; dirp++, sp++)
	    if (*sp == 'r') {                                   /* v3.4 */
		mfdirp->tag = dirp->tag;
		mfdirp->pos = lastpos;
		mfdirp->len = dirp->len;
		p=MFX+MFRbase+dirp->pos;
		memmove(newp,p,dirp->len); newp+=dirp->len;
		lastpos+=dirp->len;
		mfdirp++;
	    }
	MFRbase-=k;
	MFRnvf-=ndel;
	MFRmfrl-=(k+vlendel);
    }

    if (nadd) {
	nxtpos=MFRmfrl-MFRbase;
	nxtp=MFX+MFRmfrl+(k=sizeof(DIRSTRU)*nadd);
	for (p=(mfp=nxtp-1)-k, mfdirp=MFRdir, n=MFRnvf; n--; ) {
	    for (loop=mfdirp->len; loop--; )
		    *mfp-- = *p--;
	    /* mfdirp->pos+=k;  MFRbase will be changed */
	    mfdirp++;
	}
	if (mfdirp != &MFRdir[MFRnvf])
	    fatal("fldupdat/bug");
	sp=srcareap;                                            /* v3.4 */
	for (mfp=nxtp, dirp=dirarea, n=dirsleft; n--; dirp++, sp++) {
	    if (*sp == 'r')                                     /* v3.4 */
		continue;
	    p=batchp+dirp->pos;                                 /* v3.4 */
	    memmove(mfp,p,dirp->len); mfp+=dirp->len;
	    mfdirp->tag = dirp->tag;
	    mfdirp->pos = nxtpos;
	    mfdirp->len = dirp->len;
	    mfdirp++;
	    nxtpos+=dirp->len;
	}
	MFRbase+=k;
	MFRnvf+=nadd;
	MFRmfrl+=(k+vlenadd);
    }

#if RUGTRACE
    printf("fldupdat - RECrc=%d  Leader: %"_LD_",%d,%"_LD_",%d,%d,%d,%d\n",
	RECrc,MFRmfn,MFRmfrl,MFRmfbwb,MFRmfbwp,MFRbase,MFRnvf,MFRstatus);
for (mfdirp=MFRdir, loop=0; loop<MFRnvf; loop++, mfdirp++) {
 printf("fldupdat - dir[%d]: tag=%d  pos=%d  len=%d",
  loop,mfdirp->tag,mfdirp->pos,mfdirp->len);
#if RUHTRACE
 printf("  ->");
 for (p=MFX+MFRbase+mfdirp->pos, n=mfdirp->len; n--; p++)
  printf("%c",*p);
 printf("<-");
#endif
printf("\n");
}
#endif

    freex(srcareap,dirarea);

    if (sort) {
#if CICPP
        try { srcareap = (char *) new char [MFRmfrl-MFRbase+1]; }
        catch (BAD_ALLOC) { srcareap = (char *)ALLONULL; }
#else /* CICPP */
        srcareap=(char *)ALLOC((ALLOPARM)(MFRmfrl-MFRbase+1));
#endif /* CICPP */
        if (srcareap == (char *)ALLONULL) fatal("fldupdat/ALLOC/sort");
        dirarea=NULL;
	dirp=MFRdir;
	sp=srcareap;
	lastpos=0;
	for (n=MFRnvf; n--; dirp++) {
	    p=MFX+MFRbase+dirp->pos;
	    memmove(sp,p,dirp->len); sp+=dirp->len;
	    dirp->pos = lastpos;
	    lastpos+=dirp->len;
	}
	memmove(MFX+MFRbase,srcareap,MFRmfrl-MFRbase);
    freex(srcareap,dirarea);
    }

    return(NULL);
#undef freex
}
