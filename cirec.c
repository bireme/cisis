#include <stdio.h>
#include <string.h>
#include "cisis.h"

#if CICPP
#include <cirec.hpp>
#define CIREC_SOURCE
#include <cirun.hpp>
/* ------------------------- CIREC.HPP --------------------------------- */
#define recallok(irec,nbytes)   xrecalloc(nbytes)   /* internal use */
#define record(irec,dbnamp,mfn) xrecord(dbnamp,mfn) /* internal use */
#define recread(recp,mfn)       xrecread(mfn)       /* internal use */
#define recxref(recp,mfn,comb,comp) xrecxref(mfn,comb,comp) /* internal use */
#define nocc(irec,tag)          xnocc(tag)          /* internal use */
#define fieldn(irec,tag,occ)    xfieldn(tag,occ)    /* internal use */
#define fieldx(irec,tag,occ)    xfieldx(tag,occ)    /* internal use */
#define mstsetup                xmstsetup           /* internal use */
#if MULTI
#define recunlck(irec,option)   xrecunlck(option)   /* internal use */
#endif
/*#define recisis0                xrecisis0          moved to CISISX */
#define recupdat(crec,irec)     xrecupdat()         /* internal use */
#define recwrite                xrecwrite           /* internal use */
#define recwmast                xrecwmast           /* internal use */
#define recwxref                xrecwxref           /* internal use */
#define fldupdat(irec,batchp)   xfldupdat(batchp)   /* internal use */
//#if IFUPDATE
/*#define recreset                xrecreset         moved to CISISX */
//#endif
//#define recfield                xrecfield           /* internal use */
/* ---------------------------------------------------------------------- */
#endif /* CICPP */

/* ----------------------------- rec.c ------------------------------ */

#define RECTRACE    0
#define REATRACE    0
#define RERTRACE    0
#define REXTRACE    0
#define REYTRACE    0
#define RESXRACE    02
#define RESTRACE    01111

#if !CICPP

char lastrecread[CIMPL+1] = "";     /* 950724 */

LONGX recxrefb=0;                /* recxref() comb */
int  recxrefp=0;                /* recxref() comp */
int  recxrefn=0;                /* recxref() flagnew */
int  recxrefm=0;                /* recxref() flagmod */
FFI  recreadl=0;                /* force recread() mfrl */

#if IFUPDATE
LONGX recreadb=0;                /* force recread() comb = mfbwb */
int  recreadp=0;                /* force recread() comp = mfbwp */
#endif

#endif /* CICPP */

#if CICPP
#define NO_IREC  -1L
#endif /* CICPP */


#if CICPP

RECSTRU :: RECSTRU (CISISX *cxp): cisisxp(cxp),
                           recnbytes(0L), rectype(0), recdbxp(NULL), recrc(0),
#if MULTI
			   reclock(0),    recwlock(0),
#endif
			   recgdbl(0),    recgdbw(0), recmfp(NULL)
{
 if (cisisxp == NULL)
  fatal("RECSTRU/cisisxp==NULL");
};

RECSTRU :: RECSTRU (CISISX *cxp,
                    CONST RECSTRU &other)
{
 recnbytes = other.recnbytes;
 rectype   = other.rectype;
 recdbxp   = other.recdbxp;
#if MULTI
 reclock   = other.reclock;
 recwlock  = other.recwlock;
#endif // MULTI
 recgdbl   = other.recgdbl;
 recgdbw   = other.recgdbw;

 try {recmfp = (MFUNION  *) new char [recnbytes];}
 catch (BAD_ALLOC) {fatal("RECSTRU/ALLOC");}
 memmove (recmfp,other.recmfp,recnbytes);
 cisisxp = cxp;
 if (cisisxp == NULL)
  fatal("RECSTRU/cisisxp==NULL");
}

RECSTRU :: ~RECSTRU(void)
{
 if (recmfp)
  delete [] recmfp;
}
#endif /* CICPP */


#if !CICPP
#if ANSI
void recinit(void)                                                    /*
------------                                                          */
#else
void recinit()                                                        /*
-------                                                               */
#endif
								      /*
		    inicializa vrecp
								      */
{
    for (nrecs=maxnrec;nrecs--;)
	vrecp[nrecs]=NULL;

    nrecs=0;
}
#endif /* CICPP */


#if CICPP
LONGX RECSTRU :: xrecalloc(LONGX nbytes)
#else
LONGX recallok(irec,nbytes)
					   /*
-------------
		    abenda se nbytes menor do que primeiro move;
		    abenda se ja' alocado;
		    aloca RECHSIZE+nbytes bytes;
		    aponta por vrecp[irec];
		    garante inicializacao com zeros;
		    armazena vrecp[irec]->recnbytes;
		    retorna irec
								      */
LONGX irec;          /* indice de vrecp, para ALLOC */
LONGX nbytes;        /* tamanho da area util em bytes */
#endif /* CICPP */
{
    RECSTRU *recp;

#if CICPP
    LONGX irec = NO_IREC;
    recp=this;
#else /* CICPP */

    if (!nrecs)
	recinit();

    if (irec < 0 || irec >= maxnrec)
	fatal("recallok/index");

    if (nrecs >= maxnrec)
	fatal("recallok/overflow");
#endif /* CICPP */

    if (nbytes < MSNVSPLT)
	fatal("recallok/MSNVSPLT");

#if CICPP
    if (!recmfp || nbytes != RECnbytes) {
        if (recmfp) delete [] recmfp;
	try { recmfp = (MFUNION *) new char [nbytes];}
    catch (BAD_ALLOC) {recmfp = (MFUNION *)ALLONULL; }
	if (recmfp == (MFUNION *)ALLONULL)
	    fatal("recallok/ALLOC");
    }
    memset(recmfp,0x00,(size_t)nbytes);

#else /* CICPP */

    if (vrecp[irec]) fatal("recallok/again");

    if ((RECHSIZE+nbytes) > ALLOMAXV)
	fatal("recallok/ALLOMAXV/RECHSIZE+nbytes");

    recp=(RECSTRU *)ALLOC((ALLOPARM)(RECHSIZE+nbytes));
    if (recp == (RECSTRU *)ALLONULL)
	fatal("recallok/ALLOC");

    vrecp[irec]=recp;
    nrecs++;
    memset(recp,0x00,(size_t)(RECHSIZE+nbytes));

#endif /* CICPP */

    RECnbytes=nbytes;
    RECtype=0; /* init */

#if MULTI
    RECwlock=WUNLOCK;	/* 28/07/94 */
#endif

#if REATRACE
#if CICPP
 printf("recallok - recp=%p  RECtype=%d RECnbytes=%"_LD_"\n",
    recp,RECtype,RECnbytes);
#else
 printf("recallok - vrecp[%"_LD_"]=%p nrecs=%"_LD_" RECtype=%d RECnbytes=%"_LD_"\n",
    irec,vrecp[irec],nrecs,RECtype,RECnbytes);
#endif /* CICPP */
#endif

    return(irec);
}


#if CICPP
LONGX RECSTRU :: xrecord(char *dbnamp,
			LONGX  mfn)
#else /* CICPP */
LONGX record(irec,dbnamp,mfn)                                          /*
-----------
		    inicializa vrecp, se necessario;
		    aloca o registro irec, se ainda nao alocado;
		    seta dbxp com o resultado de dbxstorp(dbnamp);
		    le o registro mfn da base de dados descrita em dbxp;
		    armazena retorno de recread() em RECrc;
		    retorna irec
								      */
LONGX irec;          /* indice de vrecp, para store */
char *dbnamp;       /* ptr nome base de dados a ler */
LONGX mfn;           /* numero do registro a ler */
#endif /* CICPP */
{
    RECSTRU *recp;
    DBXSTRU *dbxp;
#if RECGIZM
    VGIZPSTRU *vgizmap;
#endif

#if RECTRACE
    FFI i,n;
#endif
    char *p;

#if CICPP
    LONGX irec = NO_IREC;
#endif

    /* forca carga de rec_maxmfrl - AOT, 02/06/99 */
    p=dbxcipar(NULL,"maxmfrl",'='); if (dbxcipok) sscanf(p,"%"_LD_,&rec_maxmfrl);

#if CICPP
    recp=this;
    if (!recmfp) xrecalloc(rec_maxmfrl);
#else /* CICPP */
    if (!nrecs)
	recinit();
    recp=vrecp[irec];
    if (!recp)
	recp=vrecp[recallok(irec,rec_maxmfrl)];
#endif /* CICPP */

    dbxp=dbxstorp(dbnamp);

#if BEFORE950724
#else
    if (strcmp(dbnamp,lastrecread)) {
        if (lastrecread[0]) {
            DBXSTRU *lastdbxp=dbxsrchp(lastrecread); /* "" - 22/01/97 */
	    if (lastdbxp) {
                if (lastdbxp->dbxmflush) mstflush(lastrecread); else
                if (lastdbxp->dbxmclose) mstclose(lastdbxp);
	    }
	}
        strcpy(lastrecread,dbnamp);
    }
#endif

#if BEFOREGIZMO
    if (RECdbxp == dbxp)
	if (MFRmfn == mfn)
	    return(irec);
#endif

    RECdbxp=dbxp;

#if RECTRACE
printf("record - irec=%"_LD_"  recp=%p  dbxp=%p=%s  mfn=%"_LD_"\n",
irec,recp,dbxp,dbnamp,mfn);
#endif

    recread(recp,mfn);

    if (RECtype == TYPEMFR) {
#if RECXPND
	if (RDBxpn01)
#if CICPP
	    recxpnmo(recp);
#else
	    recxpnmo(irec);                                         /* v4.2 */
#endif /* CICPP */
#endif

#if RECGIZM
	for (vgizmap=RDBvgzrp; vgizmap; vgizmap=vgizmap->nextp)
#if CICPP
	    recgizmo(cisisxp,recp,vgizmap);
#else
	    recgizmo(irec,vgizmap);                                 /* v3.4 */
#endif /* CICPP */
#endif

#if RECDECO
	if (RDBvderp) {
#if CICPP
	    recdecex(cisisxp, recp);
#else
	    recdecex(irec);                                        /* decod */
#endif /* CICPP */
	}
#endif
    }

#if RECTRACE
if (RECtype == TYPEMF0) {
    printf("record - RECrc=%d  Ctlmfr: %"_LD_"%"_LD_"%"_LD_"%d,%d,%"_LD_"%"_LD_"%"_LD_"%"_LD_"\n",
	RECrc,
	MF0ctlmfn,MF0nxtmfn,MF0nxtmfb,MF0nxtmfp,
	MF0mftype,MF0reccnt,MF0mfcxx1,MF0mfcxx2,MF0mfcxx3);
}
else {
    printf("record - RECrc=%d  Leader: %"_LD_",%d,%"_LD_",%d,%d,%d,%d\n",
	RECrc,MFRmfn,MFRmfrl,MFRmfbwb,MFRmfbwp,MFRbase,MFRnvf,MFRstatus);
#if RECTRACE
    if (RECrc == RCNORMAL || RECrc == RCLDEL)
	for (i=0;i<MFRnvf;i++) {
	    printf("record - tag[%d],pos[%d],len[%d]=%d,%d,%d\n",
		i,i,i,MFRdir[i].tag,MFRdir[i].pos,MFRdir[i].len);
	    for (p=FIELDP(i), n=DIRlen(i); n--;)
		putchar(*p++);
	    putchar('\n');
	}
#endif
}
#endif

#define RECMSTFLUSH 0
#if RECMSTFLUSH /* 950724 */
    mstflush(DBXname);
#endif

    return(irec);
}


#if CICPP
int RECSTRU :: xrecread(LONGX mfn)
#else /* CICPP */
int recread(recp,mfn)                                                 /*
-----------
		    seta dbxp;
		    obtem ptr para .mst e seu (mfn) status em .xrf;
		    se Active / Logical deleted:
			chama dbxopen para dbxp->dbxname+.mst, se necessario;
			aloca dbxp->dbxmsibp, se ainda nao alocado;
			posiciona e le .mst blk;
			move mfr neversplit para recp->recmf.mx;
			move possivel para recp->recmf.mx[MSNVSPLT];
			le restante ja' em recp->recmf.mx[MSNVSPLT+i];
		    retorna gdb record rc ou aborta
								      */
RECSTRU *recp;      /* elemento de vrecp, para store */
LONGX mfn;           /* numero do registro a ler */
#endif /* CICPP */
{
    DBXSTRU *dbxp;

    FFI n,n1,n2,nr,moveok;
#if VAX
    FFI left;
    int d;
#endif

    LONGX comb;      /* gdb getmfr comb */
    int comp;       /* gdb getmfr comp */

    char *p,*inp,*msyyp;                                        /* v3.3 */
    off_t xseek;
    int fd,rc;
#if CNV_PCBINUM
    FFI wlen;
#endif

#if MULTI
#if CNV_PCFILES
    char unibuff[MSNVSPLT]; /* CNV_PCFILES/recread - EWL/DE/RL */
#endif
#endif

#if CICPP
    RECSTRU *recp = this;
#endif /* CICPP */

    dbxp=RECdbxp;

#if RERTRACE
printf("recread - recp=%p  dbxp=%p  mfn=%"_LD_"\n",recp,dbxp,mfn);
#endif

#if CICPP
    recp->xmstsetup(DBXname,0L,0L);
#else
    mstsetup(DBXname,0L,0L);
#endif /* CICPP */

    if (strcmp(DBXname,"null") == 0) {
	if (mfn == 0) {
	    memset(MFX,0x00,sizeof(M0STRU));
	    MF0nxtmfn=1L;
	    RECtype=TYPEMF0;
	}
	else {
	    memset(MFX,0x00,LEADER);
	    MFRmfn=mfn; MFRmfrl=MFRbase=LEADER;
	    RECtype=TYPEMFR;
	}
	return(RECrc=RCNORMAL);
    }

    RECgdbl=RECgdbw=0;
#if MULTI
#if RERTRACE
printf("recread - %s/%"_LD_" lock=%d\n",DBXname,mfn,REClock);
#endif
    /* if operation is single user then reset locking option */
    if (DBXnetws == MONONETS) REClock=NOLOCK;
    rc=RCNORMAL;
    if (REClock) {
	dbxopenw(DBXname,DBXname,xx1extp,&DBXxropn,&DBXxropw,"recread/xropn/w");
	dbxopenw(DBXname,DBXname,mx1extp,&DBXmsopn,&DBXmsopw,"recread/msopn/w");
    }
#endif

    if (mfn == 0) {
	RECtype=TYPEMF0;
	comb=1; comp=0;
	rc=RCNORMAL;
    }
    else {
	RECtype=TYPEMFR;
#if IFUPDATE
	if (recreadb) {
	    rc=RECrc=RCNORMAL;
	    comb=recreadb; recreadb=0;
	    comp=recreadp; recreadp=0;
	}
	else
#endif
	if ((rc=recxref(recp,mfn,&comb,&comp)) == RCEOF || rc == RCPDEL) {
	    MFRmfn=mfn;
	    MFRmfrl=LEADER;
	    MFRmfbwb=0; MFRmfbwp=0;
	    MFRbase=LEADER;
	    MFRnvf=0;
	    MFRstatus=DELETED;
#if MULTI
	    if (REClock) {
		if (multrace)
		    printf("<r> %s/%"_LD_" is inexistent or beyond EOF\n",
			DBXname,mfn);
	    }
#endif
	    return(RECrc=rc);
	}
    }

#if RERTRACE
printf("recread - recp=%p  dbxp=%p  mfn=%"_LD_"  comb=%"_LD_"  comp=%d  rc=%d\n",
    recp,dbxp,mfn,comb,comp,rc);
#endif

    if ((msyyp=DBXmsyyp) == NULL)                               /* v3.3 */
	if ((fd=DBXmsopn) <= 0)
	    fatal("recread/DBXmsopn");

    if (!DBXmsibp)
	fatal("recread/DBXmsibp");

#if MULTI
    /* step 1: lock the .mst file */
    if (REClock) {
	/* initial check */
	if (mfn == 0) {
	    if (REClock == EWLOCK) { /* Great Richard! */
	        /* user's exclusive write lock must be off */
	        if (DBXewlxx) fatal("recread/lock/exclusive write lock is on");
	    }
	    else
	        /* user's data entry lock must be off */
	        if (DBXdelxx) fatal("recread/lock/data entry lock is on");
	}
	else {
	    /* user's data entry lock must be on */
	    if (!DBXdelxx) fatal("recread/lock/data entry lock is off");
	}
	/* .mst file lock: lock */
	if (multrace) printf("<r> %s .mst lock \n",DBXname);
	if (dbxflock(dbxp,"M")) {
	    if (multrace) printf("<r> %s .mst lock/denied \n",DBXname);
	    if (dbxewlrc) return(RECrc=RCLOCK);
	    fatal("recread/lock/file lock");
	}
	/* wait another's exclusive write lock */
	if (dbxwlock(dbxp,MFX,dbxwloop)) {
	    if (multrace) printf("<r> %s .mst ewl is on !!!\n",DBXname);
	    if (dbxewlrc) return(RECrc=RCLOCK);
	    fatal("recread/lock/must wait ewl");
	}
	if (REClock == EWLOCK)
	    /* check another's DEL (allow user's DEL) */
	    if (MF0mfcxx2 > 1 || (MF0mfcxx2 == 1 && !DBXdelxx)) {
		if (multrace) printf("<r> %s .mst del is on !!!\n",DBXname);
	        if (dbxulock(dbxp,"M")) fatal("recread/lock/file unlock/del");
		return(RECrc=RCLOCK);
	    }
    }
#endif

    n1=MSBSIZ;  /* ok */

#if MULTI
    if (DBXnetws != MONONETS || comb != DBXmsibp->msbufn) {
#else
    if (comb != DBXmsibp->msbufn) {
#endif
	xseek=(((off_t)(comb-1))<<MSBSHIFT);
	if (msyyp == NULL) {                                    /* v3.3 */
#if RERTRACE
printf("recread - fd,lseek=%"_LD_",%"P_OFF_T"\n",fd,(LONG_LONG)xseek);
#endif
	    if (LSEEK64(fd,xseek,SEEK_SET) != xseek)
		fatal("recread/lseek");
	    if ((n1=CIREAD(fd,DBXmsibp->msbuff,MSBSIZ)) < MSBSIZ)
		fatal("recread/read");
	}
	else {
#if RERTRACE
printf("recread - msyyp=%p fd,lseek=%"P_OFF_T",%"_LD_"\n",msyyp,fd,(LONG_LONG)xseek);
#endif
	    memmove(DBXmsibp,msyyp+xseek,MSBSIZ);
	}
        DBXmsibp->msbufn=comb;
    }

    inp=DBXmsibp->msbuff+comp;

    if (RECtype == TYPEMF0) {

#if RERTRACE
printf("recread- RECtype=%d  n1=%d  n=%d  MFX=%p  inp=%p\n",
    RECtype,n1,sizeof(M0STRU),MFX,inp);
#endif
	if (n1 < (n=sizeof(M0STRU))) fatal("recread/block");
	memmove(MFX,inp,n); /* ok */
#if CNV_PCBINUM
	ConvertMST_CTLSTRUCT(MFX);
	if (rectrace) prtcontrol(recp,DBXname);
#endif
	if (MF0ctlmfn != mfn) fatal("recread/check/ctlmfn");
	DBXmsmfn=MF0nxtmfn;                             /* 31/12/90 */

#if MULTI
	/* step 2: perform a DEL or an EWL lock (MF0) */
	if (REClock) {
	    if (REClock == EWLOCK) {    /* EWL */
		MF0mfcxx3++;
		DBXewlxx++;
	    }
	    else {                      /* DEL */	/* is DELOCK ? */
		MF0mfcxx2++;
		DBXdelxx++;
	    }
	    if (LSEEK64(fd,0L,SEEK_SET) != 0) fatal("recread/lock/lseek/ctl");
#if CNV_PCBINUM
	    memmove(cnv_pcbuff,MFX,sizeof(M0STRU));
	    ConvertMST_CTLSTRUCT(cnv_pcbuff);
	    if (CIWRITE(fd,cnv_pcbuff,sizeof(M0STRU)) != sizeof(M0STRU))
#else
	    if (CIWRITE(fd,MFX,sizeof(M0STRU)) != sizeof(M0STRU))
#endif
		fatal("recread/lock/write/ctl");
	    /* update input buffer */
	    ((M0STRU *)DBXmsibp)->m0mfcxx2=MF0mfcxx2;
#if BEFORE20000413
#else
	    ((M0STRU *)DBXmsibp)->m0mfcxx3=MF0mfcxx3;
#endif
	    /* .mst file lock: unlock */
	    if (multrace) printf("<r> %s .mst unlock [%s on] \n",
			DBXname,(REClock == EWLOCK)?"EWL":"DEL");
	    if (dbxulock(dbxp,"M")) fatal("recread/lock/file unlock/ctl");
	}
#endif
#if DBXMSTXL /* reseta apos fazer o DE/EWL, AOT 18/06/2002 */
	/* DBXmstxl is assigned by mstsetup() */
	MF0mftype = MF0mftype & 0x00FF;
	if (rectrace) printf("+++ recread - %s/mstxl=%d\n",DBXname,DBXmstxl);
#endif
	return(RECrc=RCNORMAL);       /* MULTI ok */
    }

#if CNV_PCBINUM
    memmove(cnv_pcbuff,inp,MSNVSPLT);
    ConvertMST_LEADER(cnv_pcbuff,0,MSNVSPLT);
    p=inp; /* save inp */
    inp=cnv_pcbuff;
#endif

#if CNV_PCFILES
    memmove(MFX+0,inp+0,4); /* mfn= */		/* unibuff */
    memmove(MFX+4,inp+6,4); /* mfbwb= */		/* unibuff */
    memmove(MFX+8,inp+4,2); /* mfrl= */		/* unibuff */
    memmove(MFX+10,inp+10,MSNVSPLT-10);		/* unibuff */
    if (rectrace) prtleader(recp,mfn);
#else
    memmove(MFX,inp,MSNVSPLT);
#endif

#if CNV_PCBINUM
    inp=p; /* restore inp */
#endif

    p=MFX+MSNVSPLT; inp+=MSNVSPLT; moveok=MSNVSPLT;

#if MULTI
    /* step 3: process existing record lock */
    if (MFRmfrl > (FFI)MFRL_MAX) {
	if (multrace)
	    printf("<r> %s/%"_LD_" mfrl=%04x=%d \n",DBXname,mfn,MFRmfrl,MFRmfrl);
	RECgdbl=0; RECgdbw=MFRmfrl;
	MFRmfrl=(FFI)0-MFRmfrl;
	if (multrace)
	    printf("<r> %s/%"_LD_" mfrl=%04x=+%d\n",DBXname,mfn,RECgdbl,MFRmfrl);
	if (REClock) rc=RCLOCK;
    }
#endif

    n=MFRmfrl;						/* to read */
    if (recreadl) {
	if (recreadl == 1) n=LEADER;		/* leader */
	else if (recreadl == 2) n=MFRbase;	/*  + dir */
	else n=recreadl;			/* waall! */
#if CNV_PCBINUM
	wlen=recreadl;
#endif
	recreadl=0;
    }

    if (n < (FFI)LEADER || n > (FFI)RECnbytes) {
#if RERTRACE
	if (rectrace) {
	    memset(p,0x00,LEADER-MSNVSPLT);
	    prtleader(recp,mfn);
	}
#endif
	fatal("recread/check/mfrl");
    }

    n-=moveok;						/* more */
    n2=n1-comp-moveok;					/* left in buffer */
    if (n < n2) n2=n;					/* this */
    n-=n2;                                              /* next */

#if RERTRACE
printf("recread - moveok=%d  n=%d  n2=%d  p=%p \n",moveok,n,n2,p);
#endif

    if (n2) {
	if (n2 <= 0) fatal("recread/n2");
	memmove(p,inp,n2); p+=n2; moveok+=n2;
#if RERTRACE
printf("recread - moveok=%d  n=%d  n2=%d  p=%p \n",moveok,n,n2,p);
#endif
    }

    if (n) {
	if (n <= 0) fatal("recread/n");
	xseek=(((off_t)(comb-0))<<MSBSHIFT);
#if RERTRACE
printf("recread - fd,lseek2=%"_LD_",%"P_OFF_T"\n",fd,(LONG_LONG)xseek);
#endif
	if (msyyp == NULL) {                                    /* v3.3 */
	    if (LSEEK64(fd,xseek,SEEK_SET) != xseek) fatal("recread/lseek2");
	    nr=n;
#if VAX
	    for (left=nr; left > 0; ) {
		nr = (left > MSBSIZ) ? MSBSIZ : left;
#endif
		if ((n2=CIREAD(fd,p,nr)) != nr) {
		    if (rectrace)
			printf("\n*** mfn=%"_LD_" seek=%"_LD_"/%d=%"P_OFF_T" read=%d/%d\n",
			    mfn,comb,comp,(LONG_LONG)xseek,nr,n2);
		    fatal("recread/read2");
		}
#if VAX
		left-=n2; p+=n2;
	    }
#endif
	}
	else {
#if RERTRACE
printf("recread - p=%p msyyp=%p fd,lseek2=%"_LD_",%"P_OFF_T"\n",p,msyyp,fd,(LONG_LONG)xseek);
#endif
	    memmove(p,msyyp+xseek,n);
	}
    }

#if CNV_PCBINUM
    ConvertMST_LEADER(MFX,MSNVSPLT,LEADER);	/* CNV_PCFILES ok */
#endif

#if VAX
    d = MFRbase - (LEADER + MFRnvf*sizeof(DIRSTRU));
    if (d) if (d < 0 || d > sizeof(int)) {
	if (rectrace) prtleader(recp,mfn);
#else
    if (MFRbase != LEADER + MFRnvf*sizeof(DIRSTRU)) {
#endif
#if RERTRACE
	if (rectrace) prtleader(recp,mfn);
#endif
	fatal("recread/check/base");
    }

#if CNV_PCBINUM
    if (wlen != 1) ConvertMST_DIR(MFX,MFRnvf);
#endif

#if MULTI
    /* step 4: perform a record lock (MFR) */
    if (REClock) {
	if (rc != RCLOCK) {
	    /* mfr lock */
	    MFRmfrl=(FFI)0-MFRmfrl; /* set */
	    xseek=(((off_t)(comb-1))<<MSBSHIFT); xseek+=comp;
	    if (LSEEK64(fd,xseek,SEEK_SET) != xseek)
		fatal("recread/lock/lseek/mfr");
#if CNV_PCFILES
	    memmove(unibuff,MFX,MSNVSPLT);
	    memmove(MFX+0,unibuff+0,4); /* mfn= */
	    memmove(MFX+6,unibuff+4,4); /* mfbwb= */
	    memmove(MFX+4,unibuff+8,2); /* mfrl= */
#endif
#if CNV_PCBINUM
	    memmove(cnv_pcbuff,MFX,MSNVSPLT);
	    ConvertMST_LEADER(cnv_pcbuff,0,MSNVSPLT);
            if (CIWRITE(fd,cnv_pcbuff,MSNVSPLT) != MSNVSPLT)
#else
	    if (CIWRITE(fd,MFX,MSNVSPLT) != MSNVSPLT)
#endif
		fatal("recread/lock/write/mfr");
#if CNV_PCFILES
	    memmove(MFX,unibuff,MSNVSPLT);
#endif
	    RECgdbl=RECgdbw=MFRmfrl;
	    MFRmfrl=(FFI)0-MFRmfrl; /* reset */
	    /* update input buffer */
	    if (comb != DBXmsibp->msbufn) fatal("recread/lock/msbufn");
	    p=(char *)DBXmsibp; p+=comp;
	    ((M1STRU *)p)->m1mfrl=RECgdbl;
	}
	/* .mst file lock: unlock */
	if (multrace)
	    if (rc == RCLOCK)
		printf("<r> %s .mst unlock [RL denied] \n",DBXname);
	    else
		printf("<r> %s .mst unlock [RL on] \n",DBXname);
	if (dbxulock(dbxp,"M")) fatal("recread/lock/file unlock/mfr");
    }
#endif

    return(RECrc=rc);
}


#if CICPP
int RECSTRU :: xrecxref(LONGX     mfn,
			LONGX    *comb,
			int     *comp)
#else /* CICPP */
int recxref(recp,mfn,comb,comp)                                       /*
-----------
		    seta dbxp;
		    chama dbxopen para dbxp->dbxname+.xrf se necessario;
		    aloca dbxp->dbxxribp, se ainda nao alocado;
		    abenda se mfn < 1;
		    calcula .xrf blk = f(mfn);
		    posiciona e le .xrf blk, se nao o carregado;
		    seta *comb e *comp;
		    retorna gdb record rc
								      */
RECSTRU *recp;      /* elemento de vrecp, para info */
LONGX mfn;           /* numero do registro a ler */
LONGX *comb;         /* block number of MF block containing the rec */
int  *comp;         /* offset in block of 1st char position of MFR */
#endif /* CICPP */
{
    DBXSTRU *dbxp;

    int rc;
    FFI n;
    LONGX newpos;
    LONGX xrftiv;
    LONGX xrftiva;

    off_t xbyte;
    char *xryyp;                                                /* v3.3 */

#if CICPP
    RECSTRU *recp = this;
#endif /* CICPP */

    dbxp=RECdbxp;

    if ((xryyp=DBXxryyp) == NULL)                               /* v3.3 */
	if (!DBXxropn)
	    fatal("recxref/DBXxropn");

    if (!DBXxribp)
	fatal("recxref/DBXxribp");

#if REXTRACE
printf("recxref - DBXxribp->xrxrpos=%"_LD_"\n",DBXxribp->xrxrpos);
#endif

    if (mfn < 1)
	fatal("recxref/mfn");

    if (DBXmsmfn <= 0)                                          /* v3.0 */
	fatal("recxref/DBXmsmfn");

    newpos=(mfn+XRMAXTV1)/XRMAXTIV;
#if MULTI
    if (DBXnetws != MONONETS || newpos != labs(DBXxribp->xrxrpos)) {
#else
    if (newpos != labs(DBXxribp->xrxrpos)) {
#endif
	if (mfn > DBXmsmfn) {                           /* 127 ok */
	    DBXxribp->xrxrpos = -newpos;
	    xrftiv = (0)*XRXDIVIDE + (0);   /* inexistent records */
	    for (n=0; n < XRMAXTIV; )
		DBXxribp->xrmfptr[n++]=xrftiv;
#if REXTRACE
printf("recxref - para mfn=%"_LD_"  xrpos=%"_LD_"  xrftiv=%"_LD_"\n",mfn,
DBXxribp->xrxrpos,xrftiv);
#endif
	}
	else {
	    xbyte=((off_t)(newpos-1))<<XRSHIFT;
	    if (xryyp == NULL) {
		if (LSEEK64(DBXxropn,xbyte,SEEK_SET) != xbyte)
		    fatal("recxref/lseek");
#if REXTRACE
printf("recxref - mfn=%"_LD_"  newpos=%"_LD_"  lseek=%"P_OFF_T"  DBXmsmfn=%"_LD_"\n",
 mfn,newpos,(LONG_LONG)xbyte,DBXmsmfn);
#endif
		if ((n=CIREAD(DBXxropn,(char *)DBXxribp,XRBSIZ)) != XRBSIZ)
		    fatal("recxref/read");
	    }
	    else {
#if REXTRACE
printf("recxref - xryyp=%p mfn=%"_LD_"  newpos=%"_LD_"  lseek=%"P_OFF_T"  DBXmsmfn=%"_LD_"\n",
 xryyp,mfn,newpos,(LONG_LONG)xbyte,DBXmsmfn);
#endif
		xryyp+=(xbyte);
		n=XRBSIZ;
                memmove((char *)DBXxribp,xryyp,(size_t)n);
	    }
#if CNV_PCBINUM
            ConvertXRF_REC((char *)DBXxribp);
#endif
	}

	if (labs(DBXxribp->xrxrpos) != newpos)
	    fatal("recxref/xrfpos");
    }

    xrftiv=DBXxribp->xrmfptr[(mfn-1)%XRMAXTIV];
    xrftiva=labs(xrftiv);
#if DBXMSTXL
    *comb = xrftiv / (XRXDIVIDE>>DBXmstxl);
    *comp = xrftiva & (XRXMASK>>DBXmstxl);
    *comp = *comp << DBXmstxl;
    recxrefn = xrftiva & (XRXMASKN>>DBXmstxl);
    recxrefm = xrftiva & (XRXMASKU>>DBXmstxl);
    recxrefn = recxrefn<<DBXmstxl; /* 1024 */
    recxrefm = recxrefm<<DBXmstxl; /*  512 */
#else
    *comb = xrftiv / XRXDIVIDE;
    *comp = xrftiva & XRXMASK;
    recxrefn = xrftiva & XRXMASKN;
    recxrefm = xrftiva & XRXMASKU;
#endif

#if 1 /* REXTRACE */
if (rectrace)
printf("recxref - mfn=%"_LD_"  xrftiv=%"_LD_",%08lx  comb=%"_LD_" comp=%d (n=%d m=%d)\n",
    mfn,xrftiv,xrftiv,*comb,*comp,recxrefn,recxrefm);
#endif

    if (*comp+MSNVSPLT > MSBSIZ)
	fatal("recxref/comp+MSNVSPLT");

    if (*comb == 0)
	if (*comp != 0)
	    fatal("recxref/comb=0/comp");

    if (*comb != 0)
	if (*comb > 0)
	    rc=RCNORMAL;                /* Record Active */
	else {
	    rc=RCLDEL;                  /* Record Logical Deleted */
	    if (*comb == -1)
		if (*comp == 0)
		    rc=RCPDEL;          /* Record Physical Deleted */
	    *comb= -(*comb);
	}
    else
	rc=RCEOF;                       /* End of File */

#if REXTRACE
printf("recxref - xrftiv=%"_LD_,"%08lx  comb=%"_LD_"  comp=%d  rc=%d\n",
    xrftiv,xrftiv,*comb,*comp,rc);
printf("recxref - flagnew=%d flagmod=%d\n",recxrefn,recxrefm);
#endif

    recxrefb = *comb;
    recxrefp = *comp;

    return(rc);
}



/* ----------------------------- fld.c ------------------------------ */

#define FLDTRACE    0

#if CICPP
int RECSTRU :: xnocc(int tag)
#else /* CICPP */
int nocc(irec,tag)                                                    /*
--------
		    seta recp=vrec[irec];
		    aponta inicio do diretorio do registro recp;
		    pesquisa sequencialmente todas as entradas;
		    retorna o numero de ocorrencias desse tag
								      */
LONGX irec;          /* indice de vrecp */
int tag;            /* tag field */
#endif /* CICPP */
{
    RECSTRU *recp;      /* elemento de vrecp */

    int loop,n;
    DIRSTRU *dirp;

#if CICPP
    recp=this;
#else /* CICPP */
    recp=vrecp[irec];
#endif /* CICPP */

    for (n=0, dirp=MFRdir, loop=MFRnvf; loop--; dirp++) {

	if (dirp->tag == tag) n++;

#if FLDTRACE
printf("nocc - recp=%p  tag=%d  loop=%d  dirp=%p  dirp->tag=%d  n=%d\n",
    recp,tag,loop,dirp,dirp->tag,n);
#endif
    }

#if FLDTRACE
printf("nocc - recp=%p  tag=%d  n=%d\n",
    recp,tag,n);
#endif

    return(n);
}


#if CICPP
int RECSTRU :: xfieldn(int tag,
		       int occ)
#else /* CICPP */
int fieldn(irec,tag,occ)                                              /*
----------
		    seta recp=vrec[irec];
		    aponta inicio do diretorio do registro recp;
		    pesquisa sequencialmente as entradas;
		    retorna zero ou o numero de ordem dessa ocorrencia
								      */
LONGX irec;          /* indice de vrecp */
int tag;            /* tag field */
int occ;            /* ocorrencia */
#endif /* CICPP */
{
    RECSTRU *recp;      /* elemento de vrecp */

    int loop,n,fn;
    DIRSTRU *dirp;

#if CICPP
    recp=this;
#else /* CICPP */
    recp=vrecp[irec];
#endif /* CICPP */

    for (fn=n=0, dirp=MFRdir, loop=MFRnvf; loop--; dirp++) {

	if (dirp->tag == tag) n++;

#if FLDTRACE
printf("fieldn - recp=%p  tag=%d  occ=%d  loop=%d  dirp->tag=%d  n=%d\n",
    recp,tag,occ,loop,dirp->tag,n);
#endif
	if (n == occ) {
	    fn=MFRnvf-loop; /* already decremented */
	    break;
	}
    }

#if FLDTRACE
printf("fieldn - recp=%p  tag=%d  occ=%d  n=%d  fn=%d\n",
    recp,tag,occ,n,fn);
#endif

    return(fn);
}


#if CICPP
int RECSTRU :: xfieldx(int tag,
		       int occ)
#else /* CICPP */
int fieldx(irec,tag,occ)                                              /*
----------
		    seta recp=vrec[irec];
		    aponta inicio do diretorio do registro recp;
		    pesquisa sequencialmente as entradas;
		    retorna RCEOF ou o indice no diretorio dessa ocorrencia
								      */
LONGX irec;          /* indice de vrecp */
int tag;            /* tag field */
int occ;            /* ocorrencia */
#endif /* CICPP */
{
    RECSTRU *recp;      /* elemento de vrecp */

    int loop,n,fx;
    DIRSTRU *dirp;

#if CICPP
    recp=this;
#else /* CICPP */
    recp=vrecp[irec];
#endif /* CICPP */

    for (fx=RCEOF, n=0, dirp=MFRdir, loop=MFRnvf; loop--; dirp++) {

	if (dirp->tag == tag) n++;

#if FLDTRACE
printf("fieldx - recp=%p  tag=%d  occ=%d  loop=%d  dirp->tag=%d  n=%d\n"
    recp,tag,occ,loop,dirp->tag,n);
#endif
	if (n == occ) {
	    fx=MFRnvf-loop-1; /* already decremented */
	    break;
	}
    }

#if FLDTRACE
printf("fieldx - recp=%p  tag=%d  occ=%d  n=%d  fx=%d\n",
    recp,tag,occ,n,fx);
#endif

    return(fx);
}


#if CICPP
void RECSTRU :: xmstsetup(char *dbnamp,
			  LONGX  loadxrf,
			  LONGX  loadmst)
#else /* CICPP */
void mstsetup(dbnamp,loadxrf,loadmst)                                 /*
-------------
		    seta dbxp;
		    abre .xrf;
		    se loadxrf inicializa estrutura xy e carrega e fecha .xrf;
		    abre .mst
		    se loadmst inicializa estrutura my e carrega e fecha .mst;
								      */
char *dbnamp;       /* ptr nome base de dados a ler */               /* v3.3 */
LONGX loadxrf;       /* carga total de .xrf */                        /* v3.3 */
LONGX loadmst;       /* carga total de .mst */                        /* v3.3 */
#endif /* CICPP */
{
    DBXSTRU *dbxp;
    int fd;
    char *p;
    FFI pagesize;
    off_t npages,nbytes,loop;

    if (rec_mstload) loadxrf=loadmst=rec_mstload;		/* 28/03/97 */

    dbxp=dbxstorp(dbnamp);

#if RESXRACE
if (rectrace) {
 if (!DBXxropn && DBXxryyp == NULL ||
     !DBXmsopn && DBXmsyyp == NULL)
printf("mstsetup - %s [%p]\n",dbnamp,dbxp);
}
#endif

#if BEFORE950102
    DBXxropn=DBXxropw=0;
    DBXxryyp=NULL;
#endif

    if (strcmp(DBXname,"null") != 0 && !DBXxropn && DBXxryyp == NULL) {
	fd=DBXxropn=dbxopen(DBXname,DBXname,xx1extp); if (dbxordwr) DBXxropw++;
	pagesize=XRBSIZ;
	if (loadxrf) {
#if MULTI
	    if (DBXnetws != MONONETS) fatal("mstsetup/loadxrf/MULTI");
#endif
	    nbytes=LSEEK64(fd,0L,SEEK_END); LSEEK64(fd,0L,SEEK_SET);
	    npages=nbytes/pagesize;
	    if (loadxrf > ALLOMAXV)
		fatal("mstsetup/ALLOMAXV/.xrf");
	    if (nbytes <= loadxrf) {
#if CICPP
		try { DBXxryyp= new char [nbytes]; }
        catch (BAD_ALLOC) { DBXxryyp = (char *)ALLONULL; }
#else /* CICPP */
		DBXxryyp=(char *)ALLOC((ALLOPARM)(nbytes));
#endif /* CICPP */
		if (DBXxryyp == (char *)ALLONULL)
		    fatal("mstsetup/ALLOC/xypages");
#if RESTRACE
if (rectrace) {
printf("mstsetup - DBXxryyp  =%p %6Ld/%6Ld=%6Ld bytes/pages  fd=%d\n",
 DBXxryyp,(double)nbytes,(double)npages,(double)nbytes/npages,fd);
}
#endif
		for (p=DBXxryyp, loop=npages; loop--; p+=pagesize)
		    if (CIREAD(fd,p,pagesize) != pagesize)
			fatal("mstsetup/read/xypages");
		CLOSE(fd); DBXxropn=DBXxropw=0;
	    }
	}
    }

    if (!DBXxribp) {
#if CICPP
	try { DBXxribp=(XRSTRU *) new char [XRBSIZ]; }
    catch (BAD_ALLOC) { DBXxribp = (XRSTRU *)ALLONULL; }
#else /* CICPP */
	DBXxribp=(XRSTRU *)ALLOC((ALLOPARM)(XRBSIZ));
#endif /* CICPP */
	if (DBXxribp == (XRSTRU *)ALLONULL) fatal("mstsetup/ALLOC/xribp");
	memset(DBXxribp,0x00,(size_t)XRBSIZ),
	DBXxribp->xrxrpos=0;
#if REYTRACE
printf("mstsetup - DBXxryyp  =%p  DBXxropn=%d  DBXxribp=%p [%"_LD_"]\n",
 DBXxryyp,DBXxropn,DBXxribp,DBXmsmfn);
#endif
    }

#if BEFORE950102
    DBXmsopn=DBXmsopw=DBXmsopv=0;
    DBXmsyyp=NULL;
#endif

    if (strcmp(DBXname,"null") != 0 && !DBXmsopn && DBXmsyyp == NULL) {
	fd=DBXmsopn=dbxopen(DBXname,DBXname,mx1extp); if (dbxordwr) DBXmsopw++;
	pagesize=MSBSIZ;
	if (loadmst) {
#if MULTI
	    if (DBXnetws != MONONETS) fatal("mstsetup/loadmst/MULTI");
#endif
	    nbytes=LSEEK64(fd,0L,SEEK_END); LSEEK64(fd,0L,SEEK_SET);
	    npages=nbytes/pagesize;
	    if (loadmst > ALLOMAXV)
		fatal("mstsetup/ALLOMAXV/.mst");
	    if (nbytes <= loadmst) {
#if CICPP
		try { DBXmsyyp= new char [nbytes]; }
        catch (BAD_ALLOC) { DBXmsyyp = (char *)ALLONULL; }
#else /* CICPP */
		DBXmsyyp=(char *)ALLOC((ALLOPARM)(nbytes));
#endif /* CICPP */
		if (DBXmsyyp == (char *)ALLONULL)
		    fatal("mstsetup/ALLOC/mypages");
#if RESTRACE
if (rectrace) {
printf("mstsetup - DBXmsyyp  =%p %6"_LD_"/%6"_LD_"=%6"_LD_" bytes/pages  fd=%d\n",
 DBXmsyyp,nbytes,npages,nbytes/npages,fd);
}
#endif
		for (p=DBXmsyyp, loop=npages; loop--; p+=pagesize)
		    if (CIREAD(fd,p,pagesize) != pagesize)
			fatal("mstsetup/read/mypages");
	    }
	}
    }

    if (!DBXmsibp) {
#if CICPP
	try { DBXmsibp=(MSSTRU *)new char [sizeof(MSSTRU)]; }
    catch (BAD_ALLOC) { DBXmsibp = (MSSTRU *)ALLONULL; }
#else /* CICPP */
	DBXmsibp=(MSSTRU *)ALLOC((ALLOPARM)sizeof(MSSTRU));
#endif /* CICPP */
	if (DBXmsibp == (MSSTRU *)ALLONULL) fatal("mstsetup/ALLOC/msibp");
	memset(DBXmsibp,0x00,sizeof(MSSTRU));

	if (strcmp(DBXname,"null") != 0) {
	    if (!DBXmsopn) fatal("mstsetup/DBXmsopn");
	    if (LSEEK64(DBXmsopn,SEEK_SET,0L)) fatal("mstsetup/seek/msibp");
	    if (CIREAD(DBXmsopn,DBXmsibp,MSBSIZ) != MSBSIZ)
		fatal("mstsetup/read/msibp");
	    DBXmsibp->msbufn=1;
	    DBXmsmfn=((M0STRU *)DBXmsibp)->m0nxtmfn;
#if DBXMSTXL
            /* DBXmstxl = ((M0STRU *)DBXmsibp)->m0mftype / 0x00FF; NAO 255*/
            DBXmstxl = ((M0STRU *)DBXmsibp)->m0mftype / 256; /* AOT/AARG 05/01/99 */
	    if (rectrace) printf("+++ recread - %s/mstxl=%d\n",DBXname,DBXmstxl);
	    // if (DBXmstxl > 3) fatal("mstsetup/DBXmstxl");
#endif
	}
#if REYTRACE
printf("mstsetup - DBXmsyyp  =%p  DBXmsopn=%d  DBXmsibp=%p [%"_LD_"]\n",
 DBXmsyyp,DBXmsopn,DBXmsibp,DBXmsmfn);
#endif
    }
    if (DBXmsopn)
        if (DBXmsyyp) {CLOSE(DBXmsopn); DBXmsopn=DBXmsopw=DBXmsopv=0; }

    return;
}

/* ------------ Substitutes the macros RECORD and RECALLOC ----------------*/
#if HBARB
#if CICPP

RECSTRU * RECSTRU :: xRecord (char *dbname,
			      LONGX  mfn)
{
 xrecord(dbname,mfn);
 return this;
}

RECSTRU * RECSTRU :: xRecAlloc (LONGX nbytes)
{
 xrecalloc(nbytes);
 return this;
}

#endif /* CICPP */
#endif /* HBARB */
/* ------------------------------------------------------------------------*/
