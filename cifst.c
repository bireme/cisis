#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "cisis.h"

#if CICPP
#include <cifst.hpp>
#include <cirun.hpp>

//#include <ciutl.hpp>
#if USE_GDBFMT
#include <format.hpp>
#else
#include <cifmt.hpp>
#endif
#endif /* CICPP */

#if CICPP
#define FST351 0
#else
#define FST351 1
#endif

#define FSTBSIZ 0x7FFF

#if BEFORE20010604
#ifdef CI_WINISIS
#define ENTRY_DLM   0x01    /* no Ctrl C may be entered and therefore... */
#else
#define ENTRY_DLM   ';'     /* for inline fst's */
#endif
#endif
#define FST_EDLM    0x01    /* no Ctrl C may be entered and therefore... */

/* ------------------------- CIFST.HPP --------------------------------- */
#if CICPP
#define fst_gener   xfst_gener                           /* internal use */
#define fst_free    xfst_free                            /* internal use */
/* #define fst_inter xfst_inter                            no conversion */
#define fst_link    xfst_link                            /* internal use */
#define fst_writ    xfst_writ                            /* internal use */
#define fst_open    xfst_open                            /* internal use */
#define fst_clos    xfst_clos                            /* internal use */
#define pstdecod    xpstdecod                            /* internal use */
#define prtlink     xprtlink                             /* internal use */
#endif /* CICPP */
/* --------------------------------------------------------------------- */

#if !CICPP
int cifstfix=0;         /* key %8"_LD_" %5d %4d %4d */
int cifstfim=0;         /* key %8"_LD_" w/ cifstfix */
#endif /* CICPP */

/* ----------------------------- fst.c ------------------------------ */
#define FSTTRACE    0
#if !CICPP
#define FSSTRACE    1
#endif

#if !CICPP

LONGX fst_error;         /* gener - fst spec error */
LONGX fst_errl;          /* gener - fst spec line where error occured */
char *fst_errp;         /* gener - fst spec ptr where error occured */
char *fst_fmtap=NULL;   /* inter - default: no static format area */
LONGX fst_fmtby=MAXMFRL; /* inter - bytes for dynamic format area */
char fst_rdlm='%';      /* inter - new occ delimiter */
char *fst_batchup=NULL; /* inter - batchp for fldupdat() area */
char *fst_batchp=NULL;  /* inter - batchp for fldupdat() commands */
int fst_batch0=1;       /* inter - batchp ^mmfn^oocc^ccnt^l1/2 option */
char fst_a351='\0';     /* inter - 351 */
UCHR *fst_fbuff[2]={NULL,NULL};           /* writbsiz*/
UWORD fst_fbsiz=FSTBSIZ/*0x7FFF*/;
UWORD fst_fleft[2];
UWORD fst_ffree[2]={0,0};
int fst_fd[2]={0,0};    /* inter - file descriptor for .ln1/.ln2 */
LONGX fst_fx[2]={0,0};   /* inter - output records to .ln1/.ln2 */
char *fst_hdrp=NULL;    /* inter - hit file headers - area */
int fst_hdrx=0;         /* inter - hit file headers - #bytes allocated */
int fst_hdru=0;         /* inter - hit file headers - #bytes stored */
int fst_hdrt=0;         /* inter - hit file headers - for area+^m^t^o^c */
char *fst_wlupifnp=NULL;    /* inter - word lookup I/F for TW processing */
TRMSTRU *fst_wluptrmp=NULL;    /* inter - word lookup I/F for TW processing */
LONGX fst_wlupitrm=0L;    /* inter - word lookup I/F for TW processing */
#endif /* CICPP */

#if CICPP
#define NO_IREC   -1L
#endif /* CICPP */

#if CICPP
FSTSTRU :: FSTSTRU (CISISX *parm_cisisxp, char *gidbnp, char *fstfile, char *stwfile,
#if USE_GDBFMT
            MST_FILE *mst, BTREE *btree,
#endif
            /* LONGX maxlk1, LONGX maxlk2, int instream):*/
            LONGX maxlk1, LONGX maxlk2, int instream, int stw_instream):
            cisisxp(parm_cisisxp), cifstfix(0), cifstfim(0), fst_error(0L), fst_errl(0L),
            fst_errp(NULL), fst_fmtap (NULL), fst_fmtby(MAXMFRL),
            fst_rdlm('%') ,
            fst_batchup(NULL), fst_batchp(NULL), fst_batch0(1),
            fst_a351('\0'), fst_fbsiz(FSTBSIZ/*0x7FFF*/)
{
 fst_fbuff[0] = NULL;
 fst_fbuff[1] = NULL;
 fst_fleft[0] = 0;
 fst_fleft[1] = 0;
 fst_ffree[0] = 0;
 fst_ffree[1] = 0;
 fst_fd[0]    = 0;
 fst_fd[1]    = 0;
 fst_fx[0]    = 0;
 fst_fx[1]    = 0;

 fst_pgmp=NULL;
 fst_stwp=NULL;
 fst_nstws=0;
 fst_area1p=NULL;
 fst_maxlk1=maxlk1;
 fst_area2p=NULL;
 fst_maxlk2=maxlk2;
 fst_qtylk1=0;
 fst_qtylk2=0;

 fst_oarea1p=NULL;
 fst_oarea2p=NULL;
 fst_oqtylk1=0;
 fst_oqtylk2=0;

 fst_hdrp=NULL;
 fst_hdrx=0;
 fst_hdru=0;
 fst_hdrt=0;

 fst_wlupifnp=NULL;    /* inter - word lookup I/F for TW processing */
 fst_wluptrmp=NULL;    /* inter - word lookup I/F for TW processing */
 fst_wlupitrm=0L;    /* inter - word lookup I/F for TW processing */

#if USE_GDBFMT
 fst_mst=mst;
 fst_btree=btree;
#endif
 fst_recp=NULL;

 int nlins = 0;

#if !CIAPI_SOURCE

 char *specp=xfst_loadfst(gidbnp,fstfile,instream);
#ifdef CI_WINISIS
 // ---- DS 03/98
 if (!specp) specp=xfst_loadfst(NULL,fstfile,instream);
 // ---- DS 03/98 (end)
#endif
 if (!specp) fst_error+=1;

 /* fst_stwp=xfst_loadstw(gidbnp,stwfile,&fst_nstws); */
 fst_stwp=xfst_loadstw(gidbnp,stwfile,&fst_nstws, stw_instream);
 //if (!fst_stwp) fst_error+=2;

 if (specp) nlins=xfst_gener(specp);

 delete [] specp;
 if (nlins <= 0) fst_error+=4;
#endif /* CIAPI_SOURCE */
}

FSTSTRU :: ~FSTSTRU (void)
{
 fst_clos(0); fst_clos(1);
 xfst_free(); // delete each entry pointed to by fst_pgmp, including fmtp
}


char * FSTSTRU :: xfst_batch(LONGX nbytes, int option)
{
 delete [] fst_batchup;
 try {fst_batchup=new char [nbytes];}
 catch(BAD_ALLOC) {fst_batchup=(char *)NULL;}
 fst_batch0 = option;
 return(fst_batchup);
}


RECSTRU * FSTSTRU :: xfst_getrecp(void)
{
    if (!fst_recp) fst_recp = new RECSTRU(cisisxp);
    return(fst_recp);
}

void  FSTSTRU :: xfst_setrecp(RECSTRU *recp)
{
    fst_recp=recp;
}

#endif /* CICPP */

#if CICPP
char * FSTSTRU::xfst_loadfst(char *gidbnp, char *file, int instream)
{
    char *fstspecp = NULL; // = "24 4 v24\n 70 0 (v70/)\n 69 2 v69";
    if (instream) {
      try {fstspecp=new char [strlen(file)+1];}
      catch(BAD_ALLOC) {fstspecp=(char *)NULL;}
      if (fstspecp == (char *)NULL) return(NULL);
      strcpy(fstspecp,file);
    }
    else fstspecp=loadfile(gidbnp,'\0',file,NULL,0L,'\n'); /* ok */
    return fstspecp;
}

char * FSTSTRU::xfst_loadstw(char *gidbnp, char *file, int *nstwsp, int stw_instream)
{
    char *stwp=NULL;
    int nstws=0;
    //if (file) if (file[0]) stwp=loadstw(gidbnp, file,NULL,0L,&nstws);
    if (file)
        if (file[0])
            if (stw_instream) {
                stwp=xloadmemstw(file,&nstws);
            } else {
                stwp=loadstw(gidbnp, file,NULL,0L,&nstws);
            }
    if (nstwsp) *nstwsp = nstws;
    return stwp;
}

LONGX FSTSTRU :: xfst_gener(char *fstspecp)
#else
LONGX fst_gener(pgmpp,fstspecp)                      /*
--------------
            aloca e compila uma fst
                                    */
FST_CODE **pgmpp;
char *fstspecp;
#endif /* CICPP */
{
#if CICPP
    FST_CODE **pgmpp = &fst_pgmp;
#endif
    FST_CODE *pgmp,*lastp=NULL;
    LONGX nfsts=0L;
    UWORD tag;
    int itx;
    FMT_CODX *fmtp;
    char *p,*q,c;
    int n;
    LONGX fmtrc;
    char cifstdlm=FST_EDLM;     /* multiple fst delimiter */ /* 20010604 */
#if FSTTRACE
 printf("fst_gener - '%s'\n",fstspecp);
#endif
    if (rectrace) fsttrace=1;
    /* init pgmpp */
    *pgmpp=NULL;
    nfsts=0;
    /* point to fst specification and do it */
    p=fstspecp;
    fst_errl=1;
    fst_errp=p;
    if (*p) if (!isdigit(*p)) cifstdlm = *p++;
    for (fst_error=0; *p; ) {
    /* ignore leading spaces ( /ht/cr/lf/vt/ff) */
    while (isspace(*p) || *p == cifstdlm) p++;
    if (!*p) break;
    /* new fst entry */
#if CICPP
    try { pgmp=(FST_CODE *)new char[sizeof(FST_CODE)];}
    catch (BAD_ALLOC){pgmp=(FST_CODE *)ALLONULL;}
#else
    pgmp=(FST_CODE *)ALLOC((ALLOPARM)sizeof(FST_CODE));
#endif /* CICPP */
    if (pgmp == (FST_CODE *)ALLONULL) fatal("fst_gener/ALLOC");
    memset(pgmp,0x00,sizeof(FST_CODE));
    if (lastp) lastp->nextp=pgmp; else *pgmpp=pgmp;
    lastp=pgmp;
    /* parse */
    fst_errp=p;
    if (!isdigit(*p)) {fst_error=FSTERR1; break;}
    for (tag=0; isdigit(*p); p++) tag=tag*10+((int)(*p)-(int)'0');
    while (*p == ' ' || *p == '\t') p++;
    if (!isdigit(*p)) {fst_error=FSTERR2; break;}
    for (itx=0; isdigit(*p); p++) itx=itx*10+((int)(*p)-(int)'0');
    while (*p == ' ' || *p == '\t') p++;
    if (itx != 351)
        if (itx < 0 || itx > 8)
            if (itx < 1000 || itx > 1008)
                if (itx < 2000 || itx > 2008)
                {fst_error=FSTERR2; break;}
#if BEFORE20010604
    if (isspace(*p) || *p == ENTRY_DLM) {fst_error=FSTERR3; break;}
#else
    if (isspace(*p) || *p == cifstdlm)  {fst_error=FSTERR3; break;}
#endif
    for (n=0, q=p; ; q++, n++)
#if BEFORE20010604
        if (*q == '\n' || *q == ENTRY_DLM || !*q) {c= *q; break;}
#else
        if (*q == '\n' || *q == cifstdlm  || !*q) {c= *q; break;}
#endif
    if (!n) {fst_error=FSTERR3; break;}
    *q='\0';
    if (fsttrace) printf("fst_gener - tag=%d it=%d pft='%s'\n",tag,itx,p);
#if CICPP
#if USE_GDBFMT
    fmtrc=0;
    fmtp=strdup(p); // RELEASE
#else
        fmtp=(char *)new FMTSTRU(cisisxp);
        fmtrc = ((FMTSTRU *)fmtp)->xfmt_gener(p);
#endif /* USE_GDBFMT */
#else
    fmtrc=fmt_gener(&fmtp,p);
#endif /* CICPP */
    if (fmtrc) {fst_error=FSTERRF; break;}
    pgmp->tag=tag;
    pgmp->it=itx;
    pgmp->fmtp=fmtp;
    /* restore last chr */
    *q=c;
    /* next line */
    p=q;
    fst_errl++;
    nfsts++;
    /* next line */
    fst_errl++;
    }
    if (fst_error) {
    pgmp= *pgmpp;
#if CICPP
    xfst_free();
#else /* CICPP */
    fst_free(pgmp);
#endif /* CICPP */
    *pgmpp=NULL;
    return(fst_error);
    }
    else {
    fst_errl=0;
    fst_errp=NULL;
    return(nfsts);
    }
}

#if CICPP
void FSTSTRU :: xfst_free(void)
#else
void fst_free(pgmp)                                                     /*
--------------
            libera area de uma fst
         inclui delete's de FSTSTRU
                                    */
FST_CODE *pgmp;
#endif /* CICPP */
{
#if CICPP
    FST_CODE *pgmp = fst_pgmp;
#endif
    FST_CODE *nextp;
    for (; pgmp; pgmp=nextp) {
        if (pgmp->fmtp)
#if USE_GDBFMT
            free (pgmp->fmtp); // because has been allocated via strdup()
//          delete [] pgmp->fmtp; // RELEASE
#else
#if CICPP
            //((FMTSTRU *)pgmp->fmtp)->xfmt_free();
            delete (FMTSTRU *)pgmp->fmtp;
#else
            fmt_free(pgmp->fmtp); /* fmt_free nao testava */
#endif /* CICPP */
#endif
        nextp=pgmp->nextp;

#if CICPP
        delete [] (char *)pgmp;
#else
        FREE(pgmp);
#endif /* CICPP */
    }
#if CICPP
    delete [] fst_stwp; fst_stwp = NULL;
    delete [] fst_area1p; fst_area1p = NULL;
    delete [] fst_area2p; fst_area2p = NULL;
    delete [] fst_oarea1p; fst_oarea1p = NULL;
    delete [] fst_oarea2p; fst_oarea2p = NULL;
    delete fst_recp; fst_recp = NULL;
    delete [] fst_batchup; fst_batchup = NULL;
    delete fst_wluptrmp; fst_wluptrmp = NULL;
#else
/*
    if (fst_stwp) FREE(fst_stwp);
    if (fst_area1p) FREE(fst_area1p);
    if (fst_area2p) FREE(fst_area2p);
    if (fst_oarea1p) FREE(fst_oarea1p);
    if (fst_oarea2p) FREE(fst_oarea2p);
    if (fst_recp) FREE(fst_recp);
    if (fst_batchup) FREE(fst_batchup);
*/
    if (fst_wluptrmp) if (fst_wlupitrm) {
        FREE(vtrmp[fst_wlupitrm]); vtrmp[fst_wlupitrm]=NULL; ntrms--;
        fst_wlupitrm=0L;
    }
#endif /* CICPP */

/*
    fst_stwp=NULL;
    fst_area1p=NULL;
    fst_area2p=NULL;
    fst_oarea1p=NULL;
    fst_oarea2p=NULL;
    fst_recp=NULL;
    fst_batchup=NULL;
*/
    fst_wluptrmp=NULL;
}

#if CICPP
LONGX FSTSTRU :: xfst_inter(int bwcase)
#else
LONGX fst_inter(pgmp,irec,stwp,area1pp,max1,area2pp,max2,qty1p,qty2p)    /*
--------------
            se area1p = NULL entao aloca max1 x LINK1;
            se area2p = NULL entao aloca max2 x LINK2;
            executa pgmp;
            retorna qty1+qty2
                                    */
FST_CODE *pgmp;
LONGX irec;
char *stwp;
char **area1pp;
LONGX max1;
char **area2pp;
LONGX max2;
LONGX *qty1p;
LONGX *qty2p;
#endif /* CICPP */
{
#if CICPP
#if USE_GDBFMT
    MST_FILE *mst=fst_mst;
    BTREE *btree=fst_btree;
#endif
    FST_CODE *pgmp = fst_pgmp;
    char     *stwp = fst_stwp;
    char    **area1pp = (!bwcase) ? &fst_area1p : &fst_oarea1p;
    LONGX      max1 = fst_maxlk1;
    char    **area2pp = (!bwcase) ? &fst_area2p : &fst_oarea2p;
    LONGX      max2 = fst_maxlk2;
    LONGX     *qty1p = (!bwcase) ? &fst_qtylk1 : &fst_oqtylk1;
    LONGX     *qty2p = (!bwcase) ? &fst_qtylk2 : &fst_oqtylk2;
    LONGX irec = NO_IREC;
#else
    LONGX xqty1,xqty2;
#endif /* CICPP */

    RECSTRU *recp;
    LONGX mfn;
    UWORD tag,occ,cnt;
    int itx,ity,itz,nwo,nwc;
    FMT_CODX *fmtp;
    UCHR word[LE2+LE2+LE2+1],*p,*q,*wp,*sp,c1,c2;
    LINK1 *link1p;
    LINK2 *link2p;
    LONGX x1,x2;
    char *fmtareap,*fmtap;
    char *pfxp,*keyp,*hdrp;
    int pfxl,n,wlen,isstw,ifst;
#if FST351
    int n351;
#endif
    int error=0,hlen;
    LONGX fmtrc,nbytes;

#define ITMARKUP 1
#if ITMARKUP
    int itm;
    char *itmp;
    int itmn,itmx;
    int itmalen,itmyplen,itmelen,itmstriplen,itmyok,itmymatch;
    int itmfldleft,itmfldqleft;
    char *itmyp,*itmfldp,*itmfldq;
#define MAXITMARKUPLEN 100
    char itmend[MAXITMARKUPLEN+1];
    char itmuc[MAXITMARKUPLEN+1];
    char *itmxp,*itmxq;
    int itmxx;
#endif /* ITMARKUP */

#define STRIPMARKUP 1
#if STRIPMARKUP /* STRIPMARKUP */
    char *smuenvp=NULL;
    int smumaxlen=0;
    int fldleft,qleft,striplen;
    int xdir,erased,match;
    char *fldp,*fldq;
#define STRIPMAXTAGS 16
    UWORD striptags[STRIPMAXTAGS+1];
    int nstrips=0;
    LONGX ltag;
    UWORD xtag;
    int xloop,xxtag;
#endif

#if FSTTRACE
 printf("fst_inter - pgmp=%p  irec=%"_LD_"   area1=%p/%"_LD_"  area2=%p/%"_LD_"\n",
  pgmp,irec,*area1pp,max1,*area2pp,max2);
#endif
    /* check fst */
#if !CICPP
    if (!qty1p) qty1p= &xqty1;
    if (!qty2p) qty2p= &xqty2;
#endif /* CICPP */
    *qty1p=0L;
    *qty2p=0L;
    if (!pgmp) return(-1L);
#if CICPP
    //recp must be set via setrecp (or mst is provided)
    recp=fst_recp;
#else /* CICPP */
    /* check irec */
    if (!nrecs) fatal("fst_inter/nrecs");
    if (irec < 0 || irec >= maxnrec) fatal("fst_inter/irec");
    if ((recp=vrecp[irec]) == NULL) fatal("fst_inter/recp");
#endif /* CICPP */
#if USE_GDBFMT
    if (recp) mfn=MFRmfn;       /* recp is provided */
    else mfn=mst->getmfn();
#else
    if (RECtype != TYPEMFR) fatal("fst_inter/TYPEMFR");
    mfn=MFRmfn;
#endif

#if STRIPMARKUP /* STRIPMARKUP */
    p=(unsigned char *)dbxcipar(NULL,"ci_fststrip",'=');
    if (dbxcipok) smuenvp=(char *)p;
#if PC || UNIX
    if (!smuenvp)
        if ((smuenvp=getenv("CI_FSTSTRIP")) == NULL)
            smuenvp=getenv("ci_fststrip");
#endif
    if (smuenvp) if (sscanf(smuenvp,"%d",&smumaxlen) != 1) fatal("fst_inter/CI_FSTSTRIP/maxlen");
    if (smumaxlen > 0) { /* strip <.smumaxlen> from data segment */
    /*...
    UWORD striptags[STRIPMAXTAGS+1];
    ...*/
    nstrips=0; p=(unsigned char *)smuenvp;
    while (isspace(*p)) p++; while (isdigit(*p)) p++; /* smumaxlen */
    while (isspace(*p)) p++;
    while (*p++ == ',') {
        while (isspace(*p)) p++;
        for (ltag=0; isdigit(*p); ) { ltag=ltag*10+(*p-'0'); p++; }
        if (ltag <= 0) fatal("fst_inter/CI_FSTSTRIP/tag");
        striptags[nstrips++]=(UWORD)ltag;
        if (nstrips >= STRIPMAXTAGS) fatal("fst_inter/CI_FSTSTRIP/MAXTAGS");
    }
    striptags[nstrips]=(UWORD)0;
    /*...*/
        for (xdir=0; xdir < MFRnvf; xdir++) {
            if (nstrips) {
                xtag=DIRtag(xdir);
                for (xxtag=0, xloop=0; striptags[xloop]; xloop++)
                    if (xtag == striptags[xloop]) { xxtag=1; break; }
                if (!xxtag) continue;
            }
            fldp=FIELDP(xdir);
            fldleft=DIRlen(xdir);
            for (erased=0, p=(UCHR *)fldp; fldleft > 0; ) {
                if (*fldp != '<') {
                    if (erased) *p = *fldp; p++; fldp++; fldleft--; continue;
                }
                for (striplen=1, match=0, fldq=fldp, qleft=fldleft; qleft > 0; ) {
                    if (striplen > smumaxlen) break;
                    if (*fldq == '>') { match=1; break; }
                    fldq++; qleft--; striplen++; continue;
                }
                if (!match) {
                    if (erased) *p = *fldp; p++; fldp++; fldleft--; continue;
                }
                /*...*/
                fldp+=striplen; fldleft-=striplen; erased+=striplen;
            }
            if (erased) memset(p,' ',erased);
        }
    }
#endif /* STRIPMARKUP */




    /* setup fst_batchp for links in fldupdat() format */
    if (fst_batchup) {
        *fst_batchup='\0';
        fst_batchp=fst_batchup;
    }

    /* alloc area for links */
    if (!fst_fd[0]) /* 21/11/94 */
    if (!*area1pp) {
        nbytes=sizeof(LINK1)*max1;
        if (nbytes > ALLOMAXV) fatal("fst_inter/max1");
#if CICPP
        try {*area1pp=new char [nbytes];}
        catch(BAD_ALLOC) {*area1pp=(char *)ALLONULL;}
#else
        *area1pp=(char *)ALLOC((ALLOPARM)nbytes);
#endif /* CICPP */
        if (*area1pp == (char *)ALLONULL) fatal("fst_inter/ALLOC/area1");
    }
    if (!fst_fd[1]) /* 21/11/94 */
    if (!*area2pp) {
        nbytes=sizeof(LINK2)*max2;
        if (nbytes > ALLOMAXV) fatal("fst_inter/max2");
#if CICPP
        try { *area2pp=new char[nbytes];}
        catch (BAD_ALLOC){ *area2pp=(char *)ALLONULL;}
#else
        *area2pp=(char *)ALLOC((ALLOPARM)nbytes);
#endif /* CICPP */

        if (*area2pp == (char *)ALLONULL) fatal("fst_inter/ALLOC/area2");
    }
    /* alloc format area */
    if (fst_fmtby < MAXMFRL)
    if (fsttrace) printf("+++ fst_fmtby=%"_LD_"\n",fst_fmtby);
    fmtareap=fst_fmtap;
    if (!fst_fmtap) {
    nbytes=fst_fmtby;
    if (nbytes > ALLOMAXV) fatal("fst_inter/fst_fmtby");
#if SHOW
    if (fsttrace) showcore("+++ fstareap");
#endif
#if CICPP
    try {fmtareap=new char [nbytes];}
    catch (BAD_ALLOC){ fmtareap=(char *)ALLONULL;}
#else
    fmtareap=(char *)ALLOC((ALLOPARM)nbytes);
#endif /* CICPP */
    if (fmtareap == (char *)ALLONULL) fatal("fst_inter/ALLOC/fmtarea");
    }
    /* setup link1/link2 */
    if (!fst_fd[0]) /* 21/11/94 */
        link1p=(LINK1 *)*area1pp;
    x1=max1;
    if (!fst_fd[1]) /* 21/11/94 */
     link2p=(LINK2 *)*area2pp;
    x2=max2;
    /* check stwp */
    if (!stwp) stwp="";

    fst_hdru=0; /* 05/10/95 */
    if (fst_hdrp) *fst_hdrp='\0';

    /* format and process each fst entry */
/* new */
    for (ifst=1; pgmp; pgmp=pgmp->nextp, ifst++) {
    tag=pgmp->tag;
    itx=pgmp->it;
    ity=itz=0;
#if ITMARKUP
    itm=0;
    if (itx >= 2000) {itx-=2000; itm=1;}
#endif /* ITMARKUP */
    if (itx >= 1000) {itx-=1000; itz=1;}
    if (itx == 351) itx=351;
    else {
        if (itx < 0 || itx > 8) /*fatal("fst_inter/it");*/ {error=1; break;}
        if (itx > 4) {itx-=4; ity=1;}
    }
    /* format */
    fmtp=pgmp->fmtp;
    *fmtareap='\0';
#if USE_GDBFMT
//#if BEFORE950113
//  char * recbuf=mst->getrecbuf();
//#else
//  char * recbuf=recp->recmfp->mx; // MFX
//#endif
    char *recbuf;
    if (recp) recbuf=MFX;
    else recbuf=mst->getrecbuf();

    // .. if (mst->getrec(mfn,&recbuf)!=0)
    FORMAT *f = new FORMAT(cisisxp, mst,btree,1); // ascii_output
    fmtrc = - f->Format(fmtp,       /* format */ /* HB 06/10/99 */
        recbuf,     // char *rec,   /* MF record */
        fmtareap,   // char *wka,   /* format work area */
        fst_fmtby); // LONGX la,     /* length of work area */
    delete f;
    /* fmtrc=0;*/                        /* HB 06/10/99 */
#else /* USE_GDBFMT */
#if CICPP
    fmtrc=((FMTSTRU *)fmtp)->xfmt_inter(recp,fst_fmtby,fmtareap,fst_fmtby);
#else
    fmtrc=fmt_inter(fmtp,irec,fst_fmtby,fmtareap,fst_fmtby);
#endif /* CICPP */
#endif /* USE_GDBFMT */

    if (fsttrace)
        printf("+++ fst %d = %"_LD_" %d+%d\n+++ fmt=%"_LD_"\n%s.\n",
        ifst,(LONGX)tag,itx,ity,fmtrc,fmtareap);
    if (fmtrc < 0) /*fatal("fst_inter/fmtrc");*/ {error=2; break;}

#if BEFORE20000831
#else
#if USE_GDBFMT
#else
    mfn=MFRmfn;
#endif
#endif

    fmtap=fmtareap; pfxp=""; pfxl=0;
#if ITMARKUP
    itmp=""; itmn=0;
#endif /* ITMARKUP */
    if (itm)
        if (*(p=(UCHR *)fmtap) != '\0')
        if (*(p+1))
            for (c1= *p++; *p; p++)
                if (*p == c1) {
                    fmtap++; *p='\0';
                    for (itmn=1, itmp=fmtap; *fmtap++; )
                        if (*fmtap == ',') { fmtap='\0'; itmn++; }
                        else *fmtap=isisuctab[*fmtap];
                    fmtap=(char *)(p+1);
                    break;
                }
    if (*itmp) {
        itmfldp=fmtap; itmfldleft=strlen(fmtap); itmalen=0;
        for (; itmfldleft > 0; ) {
            if (*itmfldp != '<') { itmfldp++; itmfldleft--; continue; }
            for (itmyok=0, itmyp=itmp, itmx=itmn; itmx--; itmyp+=(itmyplen+1)) {
                itmyplen=strlen(itmyp);
                if (itmyplen >= MAXITMARKUPLEN)  { itmfldp++; itmfldleft--; continue; } /* return fatal error */
                for (itmxq=itmuc, itmxp=itmfldp, itmxx=itmyplen; itmxx--; )
                    *itmxq++=isisuctab[*itmxp++];
                *itmxq='\0';
                if (strncmp(itmuc,itmyp,itmyplen) == 0) { itmyok=1; break; }
            }
            if (!itmyok) { itmfldp++; itmfldleft--; continue; }
            if (itmyplen+1 >= MAXITMARKUPLEN)  { itmfldp++; itmfldleft--; continue; } /* return fatal error */
            itmend[0]='<'; itmend[1]='/'; memmove(itmend+2,itmyp+1,itmyplen-1);
            itmend[itmelen=itmyplen+1]='\0';
            if (itmend[itmelen-1] == ' ') {
                itmend[itmelen-1]='>';  /* permite <XX> ou <XXb - AOT, 25/09/2002 */
                /* .. falta saltar os atributos */
            }
            itmymatch=0, itmstriplen=0;
            for (itmfldq=itmfldp+itmyplen, itmfldqleft=itmfldleft-itmyplen; itmfldqleft > 0; ) {
                if (*itmfldq != '>') { itmfldq++; itmfldqleft--; continue; }
                if (itmelen >= MAXITMARKUPLEN)  { itmfldq++; itmfldqleft--; continue; } /* return fatal error */
                if (itmfldleft-itmfldqleft >= itmelen) {
                    for (itmxq=itmuc, itmxp=itmfldq-itmelen+1, itmxx=itmelen; itmxx--; )
                        *itmxq++=isisuctab[*itmxp++];
                    if (strncmp(itmuc,itmend,itmelen) == 0)
                        { itmymatch=1; break; }
                }
                itmfldq++; itmfldqleft--;
            }
            if (itmymatch) {
                itmfldq++; itmfldqleft--;
                itmstriplen=itmfldleft-itmfldqleft-itmyplen-itmelen;
                memmove(fmtap+itmalen,itmfldp+itmyplen,itmstriplen); /* for */
                itmalen+=itmstriplen;
                itmfldp=itmfldq+1; itmfldleft-=(itmyplen+itmstriplen+itmelen);
            }
            else { itmfldp++; itmfldleft--; }
        }
        fmtap[itmalen]='\0';
    }
    if (itz)
        if (*(p=(UCHR *)fmtap) != '\0')
        if (*(p+1))
            for (c1= *p++; *p; p++)
            if (*p == c1) {
                fmtap++; *p='\0';
                sscanf(fmtap,"%"_LD_,&mfn);
                fmtap=(char *)(p+1);
                break;
            }
    if (ity)
        if (*(p=(UCHR *)fmtap) != '\0')
        if (*(p+1))
            for (c1= *p++; *p; p++)
            if (*p == c1) {
                pfxp=fmtap+1; *p='\0';
                fmtap=(char *)(p+1);
                for (p=(UCHR *)pfxp; *p; p++, pfxl++)
                if (*p == '\r' || *p == '\n') {
                    *p='\0'; break;
                }
                if (!pfxl) pfxp="";
                break;
            }
    for (nwo=nwc=0, occ=cnt=1, p=(UCHR *)(keyp=fmtap); *keyp; ) {
        if (*p == fst_rdlm) {nwo=1; *p='\n';}
        if (itx == 351) if (*p == ';') {nwo=1; *p='\n';} /* ENTRY_DLM ? */
        if (*p != '\r') if (*p != '\n') if (*p) {p++; continue;}
        while (*p == '\r' || *p == '\n') *p++='\0';
#if FSSTRACE
        if (fsttrace) printf("+++ keyp=%s\n",keyp);
#endif
        switch (itx) {

        case 0:
        if (*keyp) {
            n=fst_link(pfxp,pfxl,keyp,strlen(keyp),mfn,tag,occ,cnt,
                (char *)link1p,(char *)link2p,&x1,&x2);
          if (n < 0) return(n);
            if (n) if (n <= LE1) link1p++; else link2p++;
            cnt++;
        }
        break;

        case 1:
        if (*keyp == SFLDCHR) if (*++keyp) keyp++;
        for (q=(UCHR *)keyp; ; ) {
            nwc=0;
            hlen=0; hdrp=(char *)q;
            while (*q) {
            if (*q == SFLDCHR) {nwc=1; if (*++q) q++; break;}
            hlen++; q++;
            }
            if (hlen == 0) {
            if (nwc) cnt++;
            if (*q) continue; else break;
            }
#if FSSTRACE
            if (fsttrace) {
                printf("+++ %d:%d,%d=",itx,occ,cnt);
            for (n=0; n < hlen; n++) printf("%c",hdrp[n]);
            printf("\n");
            }
#endif
            n=fst_link(pfxp,pfxl,hdrp,hlen,mfn,tag,occ,cnt,
                (char *)link1p,(char *)link2p,&x1,&x2);
          if (n < 0) return(n);
            if (n) if (n <= LE1) link1p++; else link2p++;
            cnt++; if (!*q) break;
        }
        break;

        case 2:
        c1='<'; c2='>';
        case 3:
        if (itx == 3) c1=c2='/';
        for (q=(UCHR *)keyp; ; ) {
            nwc=0;
            hlen=0;
            while (*q != c1) if (*q) q++; else break;
            if (*q == c1) {
#if FST2TAGS
                tagop=strstr(q,"<I>");
                if (tagop) {

                }
#endif /* FST2TAGS */
                hdrp=(char *)(q+1);
                for (nwc=1; *++q; )
                    if (*q == c2) {
#if FST2TAGS
                        .
#endif /* FST2TAGS */
                        q++; break;
                    }
                    else hlen++;
            }
            if (hlen == 0) {
                if (nwc) cnt++;
                if (*q) continue; else break;
            }
#if FSSTRACE
            if (fsttrace) {
                printf("+++ %d:%d,%d=",itx,occ,cnt);
                for (n=0; n < hlen; n++) printf("%c",hdrp[n]);
                printf("\n");
            }
#endif
            n=fst_link(pfxp,pfxl,hdrp,hlen,mfn,tag,occ,cnt,
                (char *)link1p,(char *)link2p,&x1,&x2);
          if (n < 0) return(n);
            if (n) if (n <= LE1) link1p++; else link2p++;
            cnt++; if (!*q) break;
        }
        break;

        case 4:
        for (q=(UCHR *)keyp, wlen=0; ; ) {
            hdrp=(char *)q; hlen=0;
            while (isiswctab[*q]) {
            if (wlen < sizeof(word)) word[wlen++] = *q;
            hlen++;
            if (!*++q) break;
            }
            while (!isiswctab[*q]) if (*q) q++; else break;
            if (wlen == 0) if (*q) continue; else break;
            word[wlen] = '\0';
#if FSSTRACE
            if (fsttrace) {
                printf("+++ %d:%d,%d=%s=",itx,occ,cnt,word);
            for (n=0; n < hlen; n++) printf("%c",hdrp[n]);
            printf("\n");
            }
#endif
            for (wp=word; *wp; wp++) *wp = isisuctab[*wp];
            isstw=0;
            if (wlen <= LE1) {
#ifdef CI_WINISIS
            for (sp=stwp; *sp; sp+=(LE1+1)*2) {
                 n=memcmp(sp,word,wlen+1);
                 if (n > 0) {
                    if (memcmp(sp-(LE1+1),word,wlen+1)==0)
                            isstw=1;
                    break;
                    }
                 if (n < 0) continue;
                 isstw=1; break;
            }
#else
            for (sp=(UCHR *)stwp; *sp; sp+=LE1+1) {
                n=memcmp(sp,word,wlen+1);
                if (n > 0) break;
                if (n < 0) continue;
                isstw=1; break;
            }
#endif /* CI_WINISIS */
            }

#if BEFORE20030113
#else
            if (fst_wlupifnp) {
              if (!fst_wluptrmp) {
#if CICPP
     try { fst_wluptrmp=new TRMSTRU(cisisxp); }
     catch (BAD_ALLOC) { fatal("cifst/lup_trm/next"); }
     fst_wluptrmp->xtrmalloc(0L);
     fst_wluptrmp->xterm((unsigned char *)fst_wlupifnp,(unsigned char *)"!");
#else /* CICPP */
     for (fst_wlupitrm=maxntrm; fst_wlupitrm--; ){
        if (!vtrmp[fst_wlupitrm]) /* ja' decrementado */
            break;
     }
     if (!fst_wlupitrm) fatal("cifst/lup_trm/next");
     trmalloc(fst_wlupitrm,0L);
     term(fst_wlupitrm,fst_wlupifnp,"!");
     fst_wluptrmp=vtrmp[fst_wlupitrm];
#endif /* CICPP */
              }
#if CICPP
              fst_wluptrmp->xterm((unsigned char *)fst_wlupifnp,word);
#else /* CICPP */
              term(fst_wlupitrm,fst_wlupifnp,word);
#endif /* CICPP */
              if (fst_wluptrmp->trmrc == RCNORMAL) isstw=1; /* force */
          }
#endif
          if (!isstw) {
            n=fst_link(pfxp,pfxl,hdrp,hlen,mfn,tag,occ,cnt,
                (char *)link1p,(char *)link2p,&x1,&x2);
          if (n < 0) return(n);
            if (n) if (n <= LE1) link1p++; else link2p++;
            }
            cnt++; wlen=0; if (!*q) break;
        }
        break;

#if FST351
        case 351:
        for (; *keyp == ' '; ) keyp++;
        q=keyp;
        pfxp=q; pfxl=0;
        for (; *q && *q != '/'; q++) {
            *q=isisuctab[(UCHR)*q]; if (pfxl < LE2) pfxl++;
        }
        for (; pfxl; ) {
            n351=nwc=wlen=0; fst_a351='\0';
            if (*q == '/') {
            word[wlen++]='/';
            if (*++q == '*') {
                fst_a351='.'; q++; if (*q == '/') n351=1;
                /* printf("+++ n351=%d\n",n351); */
            }
            }
            for (; *q && *q != '/'; q++)
            if (wlen < sizeof(word)) word[wlen++] = *q;
            if (wlen == 1) wlen=0;
            if (!wlen) if (!pfxl) break;
            word[wlen] = '\0';
            pfxp[pfxl] = '\0';
#if FSSTRACE
            if (fsttrace) printf("+++ %d:%d,%d=%c,%s(%d),%s(%d)\n",
                itx,occ,cnt,fst_a351,pfxp,pfxl,word,wlen);
#endif
            n=fst_link(pfxp,-pfxl,"",0,mfn,tag,occ,cnt,
                (char *)link1p,(char *)link2p,&x1,&x2);
          if (n < 0) return(n);
            if (n) if (n <= LE1) link1p++; else link2p++;
            n=fst_link(pfxp,-pfxl,(wlen)?(char *)word:"/",
                0,mfn,tag,occ,cnt,
                (char *)link1p,(char *)link2p,&x1,&x2);
          if (n < 0) return(n);
            if (n) if (n <= LE1) link1p++; else link2p++;
            if (wlen) {
            n=fst_link("",0,word,0,mfn,tag,occ,cnt,
                (char *)link1p,(char *)link2p,&x1,&x2);
          if (n < 0) return(n);
            if (n) if (n <= LE1) link1p++; else link2p++;
            }
            if (fst_a351) {
            fst_a351='\0';
            n=fst_link(pfxp,-pfxl,"",0,mfn,tag,occ,cnt,
                (char *)link1p,(char *)link2p,&x1,&x2);
          if (n < 0) return(n);
            if (n) if (n <= LE1) link1p++; else link2p++;
            n=fst_link(pfxp,-pfxl,(wlen)?(char *)word:"/",
                0,mfn,tag,occ,cnt,
                (char *)link1p,(char *)link2p,&x1,&x2);
          if (n < 0) return(n);
            if (n) if (n <= LE1) link1p++; else link2p++;
            if (wlen) {
                n=fst_link("",0,word,0,mfn,tag,occ,cnt,
                (char *)link1p,(char *)link2p,&x1,&x2);
          if (n < 0) return(n);
                if (n) if (n <= LE1) link1p++; else link2p++;
            }
            fst_a351='.';
            }
            if (n351) continue;
            cnt++; if (!*q) break;
            if (wlen == 0) {
            if (nwc) cnt++;
            if (*q) continue; else break;
            }
        } /* end for pfxl */
        break;
#endif /* FST351 */

        } /* end of switch itx */

        if (nwo) {occ++; cnt=1;}
        nwo=0;
        keyp=(char *)p;

    } /* end of for fmtap */

    } /* end of for pgmp */

    /* release local format area */
    if (!fst_fmtap)
#if CICPP
     delete [] fmtareap;
#else
     FREE(fmtareap);
#endif /* CICPP */

    /* update qty1/qty2 and return to caller */
    *qty1p = max1 - x1;
    *qty2p = max2 - x2;

    if (error) return(-2);

    return( *qty1p + *qty2p );
}


#if CICPP
int FSTSTRU :: xfst_link(char *pfxp,
             int   plen,
             char *keyp,
             int   klen,
             LONGX  mfn,
             UWORD   tag,
             UWORD   occ,
             UWORD   cnt,
             char *lk1p,
             char *lk2p,
             LONGX *lft1,
             LONGX *lft2)
#else
int fst_link(pfxp,plen,keyp,klen,mfn,tag,occ,cnt,lk1p,lk2p,lft1,lft2)     /*
------------
            monta chave (em upper case se len > 0);
            monta posting;
            mantem left (lft1/lft2);
            se fst_fdx, chama fst_writ e retorna;
            move posting (para link1/link2);
            retorna 0 ou tamanho da chave
                                    */
char *pfxp;
int plen;
char *keyp;
int klen;
LONGX mfn;
UWORD tag;
UWORD occ;
UWORD cnt;
char *lk1p;
char *lk2p;
LONGX *lft1;
LONGX *lft2;
#endif /* CICPP */
{
    LINK1 *link1p;
    LINK2 *link2p;
#ifdef CI_WINISIS
    UCHR *p,*line,*a351,*bpfx,*bkey;
#else
    UCHR line[LE2+1],a351[LE2+1],*p,bpfx[LE2+1],bkey[LE2+1];
#endif
    int len,n,bplen,bklen,left,plen0,klen0;
    int xrc;

#ifdef CI_WINISIS
#define LE2X    (LE2*(*lft2))
    try { line=(UCHR *)new UCHR[LE2X];}
    catch (BAD_ALLOC){line=(UCHR *)ALLONULL;}
    try { a351=(UCHR *)new UCHR[LE2X];}
    catch (BAD_ALLOC){a351=(UCHR *)ALLONULL;}
    try { bpfx=(UCHR *)new UCHR[LE2X];}
    catch (BAD_ALLOC){bpfx=(UCHR *)ALLONULL;}
    try { bkey=(UCHR *)new UCHR[LE2X];}
    catch (BAD_ALLOC){bkey=(UCHR *)ALLONULL;}
#else
#define LE2X    LE2
#endif

    plen0=plen; plen=abs(plen);
    klen0=klen; klen=abs(klen);

    /* store as heading */
    if (fst_hdrp) { /* 05/10/95 */
    p=(unsigned char *)fst_hdrp+fst_hdru;
    left=fst_hdrx-fst_hdru; if (left < 2) return(-1);
        bplen=bobkey(pfxp,plen,(char *)p,left-2,1,1,0);
    fst_hdru+=bplen;
    left-=bplen; if (left < 2) { *p='\0'; return(-2); }
    p+=bplen;
    bklen=bobkey((char *)keyp,klen,(char *)p,left-2,1,1,1);
    fst_hdru+=bklen;
    left-=bklen;
        p+=bklen;
    if (fst_hdrt) {
      sprintf((char *)line,"^m%"_LD_"^t%u^o%u^c%u",mfn,tag,occ,cnt);
      if (left > (len=strlen((char *)line))) {
          memmove(p,line,len); p+=len; fst_hdru+=len;
      }
    }
    *p++='\n'; *p='\0';
    fst_hdru+=1;
#ifdef CI_WINISIS
    if (line) delete [] line;
    if (a351) delete [] a351;
    if (bpfx) delete [] bpfx;
    if (bkey) delete [] bkey;
#endif
    return(0);
    }

    /* setup pfx+key for regular fst */
    bklen=bobkey((char *)keyp,klen,(char *)bkey,LE2X,1,1,1);

    if (fst_a351) {
    a351[0]=fst_a351;
    memmove(a351+1,pfxp,(plen+1 > LE2X) ? LE2X-1 : plen);
    pfxp=(char *)a351; plen++;
    }

    bplen=bobkey((char *)pfxp,plen,(char *)bpfx,LE2X,1,1,0);

#ifdef CI_WINISIS
    if (plen0 <= 0 || LE2X > LE2) {
#else
    if (plen0 <= 0) {
#endif
    len=bplen;
    memmove(line,bpfx,len); p=line+len;
    }
    else
    for (p=line, len=bplen, n=0; n < bplen; n++)
        *p++ = isisuctab[bpfx[n]];

#ifdef CI_WINISIS
    if (klen0 <= 0 || *lft2==(MAXMFRL/LE2)) {  // poor solution
#else
    if (klen0 <= 0) {
#endif
    n=(len+bklen > LE2X) ? LE2X-len : bklen;
    memmove(p,bkey,n); p+=n; len+=n;
    }
    else
    for (n=0; n < bklen; n++, len++)
        if (len < LE2X) *p++ = isisuctab[bkey[n]]; else break;

    line[len]='\0';

#if FSSTRACE
    if (fsttrace) printf("+++ %s=%s[%d]%s[%d]\n",line,bpfx,bplen,bkey,bklen);
#endif

    if (!len) {
#if FSTTRACE
#ifndef USE_ERROR_SYS
    printf("REJECTED LINK: %"_LD_"/%d/%d/%d %c%s%c\n",
        mfn,tag,occ,cnt,OPENFCHR,line,CLOSFCHR);
#endif
#endif /* FSTTRACE */
#ifdef CI_WINISIS
    if (line) delete [] line;
    if (a351) delete [] a351;
    if (bpfx) delete [] bpfx;
    if (bkey) delete [] bkey;
#endif
    return(0);
    }

    /* write a link record */
    if (len <= LE1) {
    if (fst_fd[0] > 0) {
        if ((xrc=fst_writ(0,mfn,tag,occ,cnt,(char *)line,len,NULL)) < 0)
            return(10+xrc);
#ifdef CI_WINISIS
    if (line) delete [] line;
    if (a351) delete [] a351;
    if (bpfx) delete [] bpfx;
    if (bkey) delete [] bkey;
#endif
        return(0);
    }
    }
    else {
    if (fst_fd[1] > 0) {
        if ((xrc=fst_writ(1,mfn,tag,occ,cnt,(char *)line,len,NULL)) < 0)
            return(20+xrc);
#ifdef CI_WINISIS
    if (line) delete [] line;
    if (a351) delete [] a351;
    if (bpfx) delete [] bpfx;
    if (bkey) delete [] bkey;
#endif
        return(0);
    }
    }

    /* store as fldupdat() command */
    if (fst_batchp) { /* see mx.c */
    sprintf(fst_batchp,"A%u",tag); fst_batchp+=strlen(fst_batchp);
    *fst_batchp++ = '\n';
    memmove(fst_batchp,line,len); fst_batchp+=len;
    if (fst_batch0) {
      sprintf(fst_batchp,"^m%"_LD_"^o%u^c%u^l%d",mfn,occ,cnt,(len <= LE1)?1:2);
      fst_batchp+=strlen(fst_batchp);
    }
    *fst_batchp++ = '\n';
    *fst_batchp = '\0';
#ifdef CI_WINISIS
    if (line) delete [] line;
    if (a351) delete [] a351;
    if (bpfx) delete [] bpfx;
    if (bkey) delete [] bkey;
#endif
    return(0);
    }

    /* store as LINK1/LINK2 */

    if (len <= LE1) {
    if (*lft1 <= 0) /*fatal("fst_link/left/link1");*/ {
#ifdef CI_WINISIS
    if (line) delete [] line;
    if (a351) delete [] a351;
    if (bpfx) delete [] bpfx;
    if (bkey) delete [] bkey;
#endif
        return(-31);
    }
    link1p=(LINK1 *)lk1p;
    memmove(link1p->key,line,len);
    if (len < LE1) memset(link1p->key+len,' ',LE1-len);
    encodepst(&link1p->post,mfn,tag,occ,cnt);
    (*lft1)--;
    }
    else {
    if (*lft2 <= 0) /*fatal("fst_link/left/link2");*/ {
#ifdef CI_WINISIS
    if (line) delete [] line;
    if (a351) delete [] a351;
    if (bpfx) delete [] bpfx;
    if (bkey) delete [] bkey;
#endif
        return(-41);
    }
    link2p=(LINK2 *)lk2p;
    memmove(link2p->key,line,len);
    if (len < LE2) memset(link2p->key+len,' ',LE2-len);
    encodepst(&link2p->post,mfn,tag,occ,cnt);
    (*lft2)--;
    }

#ifdef CI_WINISIS
    if (line) delete [] line;
    if (a351) delete [] a351;
    if (bpfx) delete [] bpfx;
    if (bkey) delete [] bkey;
#endif
    return(len);
}


#if CICPP
int FSTSTRU :: xfst_writ(int   treecase,
             LONGX  mfn,
             UWORD   tag,
             UWORD   occ,
             UWORD   cnt,
             char *keyp,
             int   keylen,
             UCHR *uctabp)
#else
int fst_writ(treecase,mfn,tag,occ,cnt,keyp,keylen,uctabp)
int treecase;
LONGX mfn;
UWORD tag;
UWORD occ;
UWORD cnt;
char *keyp;
int keylen;
UCHR *uctabp;
#endif /* CICPP */
{
    UCHR line[1+25+LE2+1],*p,*q;
    int x,lrecl,loop;
    x=treecase;
#if FSSTRACE
    if (fsttrace) {
    printf("fst_writ - [%d] %8"_LD_"/%5d/%4d/%4d ",x+1,mfn,tag,occ,cnt);
    for (q=keyp, loop=keylen; loop--; q++) printf("%c",*q);
    printf("\n");
    }
#endif
    /* setup link record */
    if (keylen < 1 || keylen > LE2) fatal("fst_writ/keylen");
    p=line; lrecl=0;
#if MPE
    *p++='\0'; lrecl++;
    sprintf(p,"%8"_LD_" %5d %4d %4d ",mfn,tag,occ,cnt); p+=25; lrecl+=25;
    if (uctabp) for (q=keyp, loop=keylen; loop--; ) *p++ = uctabp[*q++];
    else {memmove(p,keyp,keylen); p+=keylen;}
    lrecl+=keylen;
    loop = ((keylen <= LE1) ? LE1 : LE2) - keylen;
    memset(p,' ',loop); p+=loop; lrecl+=loop;
#else
    if (cifstfix) {
    if (uctabp) for (q=(UCHR *)keyp, loop=keylen; loop--; ) *p++ = uctabp[*q++];
    else {memmove(p,keyp,keylen); p+=keylen;}
    lrecl+=keylen;
    loop = ((keylen <= LE1) ? LE1 : LE2) - keylen;
    memset(p,' ',loop); p+=loop; lrecl+=loop;
    if (cifstfim) {
        sprintf((char *)p," %8"_LD_,mfn);                         p+=9;  lrecl+=9;
    } else {
        sprintf((char *)p," %8"_LD_" %5d %4d %4d",mfn,tag,occ,cnt); p+=25; lrecl+=25;
    }
    }
    else {
#if LINK_7544KEY
    sprintf(p,"%8"_LD_" %5d %4d %4d ",mfn,tag,occ,cnt); p+=25; lrecl+=25;
#else
    sprintf((char *)p,"%1"_LD_" %1d %1d %1d ",mfn,tag,occ,cnt);
        lrecl+=strlen((CONST char *)p); p= &line[lrecl];
#endif
    if (uctabp) for (q=(UCHR *)keyp, loop=keylen; loop--; ) *p++ = uctabp[*q++];
    else {memmove(p,keyp,keylen); p+=keylen;}
    lrecl+=keylen;
#if LINK_7544KEY
    loop = ((keylen <= LE1) ? LE1 : LE2) - keylen;
    memset(p,' ',loop); p+=loop; lrecl+=loop;
#endif
    }
#if PC
    *p++='\r';  /* CIWRITE */
    lrecl++;
#endif
#endif /* !MPE */
    *p='\n';
    lrecl++;
    /* write it */
    if (!fst_fd[treecase]) /*fatal("fst_writ/fst_open");*/ return(-1);
    if (fst_fleft[x] >= lrecl) {
    memmove(&fst_fbuff[x][fst_ffree[x]],line,lrecl);
    fst_fleft[x]-=lrecl;
    fst_ffree[x]+=lrecl;
    }
    else {
    memmove(&fst_fbuff[x][fst_ffree[x]],line,fst_fleft[x]);
    if (CIWRITE(fst_fd[x],fst_fbuff[x],fst_fbsiz) != fst_fbsiz) {
#ifndef USE_ERROR_SYS
        printf("mfn/tag/occ/cnt: %"_LD_"/%d/%d/%d \n",mfn,tag,occ,cnt);
        fatal("writbsiz/write");
#endif
        return(-2);
    }
    fst_ffree[x]=lrecl-fst_fleft[x];
    memmove(fst_fbuff[x],&line[fst_fleft[x]],fst_ffree[x]);
    fst_fleft[x]=fst_fbsiz-fst_ffree[x];
    }
    fst_fx[x]++;
    return(keylen);
}

#if CICPP
int FSTSTRU :: xfst_open(char *gidbnp,
             char *filnamp,
             int   treecase)
#else
int fst_open(gidbnp,filnamp,treecase)
char *gidbnp;
char *filnamp;
int treecase;
#endif /* CICPP */
{
    int fd;
#if MPE
    int lrecl;
    char dcb[32];
#endif
    if (treecase != 0 && treecase != 1) fatal("fst_open/treecase");

    /* create link file */
    filnamp=dbxcipar(gidbnp,filnamp,'=');
#if MPE
    lrecl=25+((treecase)?LE2:LE1); /* ccxl ! */
    sprintf(dcb,"R%d Bl%d",lrecl,8192/lrecl);
    fd=OPEN(filnamp,O_WRONLY|O_CREAT|O_TRUNC|O_MPEOPTS,PERMIS,dcb);
#else
#if CICPP
#if !GCC // na verdade #if PC, pois PC no CISIS.h significa DOS ou Windows
         // em UNIX, fmode e' sempre BINARY (vide #define O_BINARY em CISIS.H)
    unsigned int mode;
    mode=_fmode; _fmode=O_BINARY;
#endif // !GCC
    fd=CREAT(filnamp,PERMIS);        /* BINARY */
#if !GCC
    _fmode=mode;
#endif // !GCC
#else
    fd=CREAT(filnamp,PERMIS);        /* BINARY */
#endif /* CICPP */
#endif
    if (fd == EOF) /*fatal(filnamp);*/ return(EOF);
    if (!fst_fbuff[treecase]) {
    /*if (fst_fbsiz > (UWORD)ALLOMAXV) fatal("fst_open/ALLOMAXV"); HB 28/03/2003 -> FFI */
    if (fst_fbsiz > ALLOMAXV) fatal("fst_open/ALLOMAXV");
#if CICPP
    try {fst_fbuff[treecase]=new unsigned char [fst_fbsiz];}
    catch (BAD_ALLOC) {fst_fbuff[treecase]=(unsigned char *)ALLONULL;}
#else
    fst_fbuff[treecase]=(char *)ALLOC((ALLOPARM)fst_fbsiz);
#endif /* CICPP */
    if (fst_fbuff[treecase] == (unsigned char *)ALLONULL) {
        CLOSE(fd); fatal("fst_open/ALLOC");
        return(EOF);
    }
    }

    fst_fleft[treecase]=fst_fbsiz;
    fst_ffree[treecase]=0;
    fst_fx[treecase]=0;

    return(fst_fd[treecase]=fd);
}

#if CICPP
int FSTSTRU :: fst_clos(int treecase)
#else
int fst_clos(treecase)                                                 /*
-------------                                                           */
int treecase;
#endif /* CICPP */
{
    int x;
    if (fst_fd[x=treecase] > 0) {
    if (fst_ffree[x])
        if (CIWRITE(fst_fd[x],fst_fbuff[x],fst_ffree[x]) != fst_ffree[x])
        /*fatal("fst_clos/write");*/ return(EOF);
#if CICPP
    delete [] fst_fbuff[x];
#else
    FREE(fst_fbuff[x]);
#endif /* CICPP */
    CLOSE(fst_fd[x]);
    }
    return(0);
}

/* *************************** pstdecod  ******************************** */
#if CICPP
void FSTSTRU :: xpstdecod(POSTSTRU *pst,
              LONGX     *mfn,
              UWORD      *tag,
              UWORD      *occ,
              UWORD      *cnt)
#else
void pstdecod(pst,mfn,tag,occ,cnt)
POSTSTRU *pst;
LONGX *mfn;
UWORD *tag;
UWORD *occ;
UWORD *cnt;
#endif /* CICPP */
{
  LONGX xmfn;
  UCHR *p,byt1,byt2,byt3;

  p=(UCHR *)pst;
  byt1= *p++; byt2= *p++; byt3= *p++;
  xmfn = (unsigned LONGX )byt1;
  xmfn=(unsigned LONGX )(xmfn << 16)+
       (unsigned LONGX)(byt2<<8)+
       (unsigned LONGX)byt3;
  *mfn=xmfn;
  byt1= *p++; byt2= *p++;
  *tag=(byt1<<8)+byt2;
  byt1= *p++;
  *occ=byt1;
  byt1= *p++; byt2= *p++;
  *cnt=(byt1<<8)+byt2;
}
/* *************************** prtlink   ******************************** */
#if CICPP
void FSTSTRU :: xprtlink(char *stubp,
             LONGX  pmfn,
             UWORD   ptag,
             UWORD   pocc,
             UWORD   pcnt,
             UCHR *keyp,
             int   len)
#else
void prtlink(stubp,pmfn,ptag,pocc,pcnt,keyp,len)
char *stubp;
LONGX pmfn;
UWORD ptag,pocc,pcnt;
UCHR *keyp;
int len;
#endif /* CICPP */
{
    int loop;
    char *p;

    if (stubp) printf("%s",stubp);
    printf("%8"_LD_" %5u %4u %4u ",pmfn,ptag,pocc,pcnt);
    for (p=(char *)keyp, loop=len; loop--; p++) printf("%c",*p);
    printf("\n");
}


#if CICPP
char * FSTSTRU :: xloadmemstw(char *memp,
                              int  *nstws) {
    char* areap = NULL;
    char* aposp = NULL;
    char* mposp = memp;
    int asize = 0;
    int mlen = 0;
    int klen = 0;
    int num = 0;
    bool added = false;

    if (memp != NULL) {
        mlen = strlen(memp);

        for (int counter = 0; counter < mlen; counter++) {
            if (isspace(*mposp)) {
                added = false;
            } else {
                if (!added) {
                    num++;
                    added = true;
                }
            }
            mposp++;
        }

        asize = (num * (LE1 + 1)) + 1;

        try {
            aposp = areap = new char[asize];
        } catch (BAD_ALLOC) {
            fatal("xloadmemstw/ALLOC");
        }

        memset(areap, 0x00, asize);
        mposp = memp;

        for (int pos = 0; pos < mlen; pos++) {
            if (isspace(*mposp) == 0) {
                if (klen <= LE1) {
                    *aposp = *mposp;
                    if (klen == LE1) {
                        aposp++;
                    }
                }
                klen++;
                aposp++;
            } else {
                if (klen > 0) {
                    aposp += (LE1 - klen + 1);
                    klen = 0;
                }
            }
            mposp++;
        }
    }
    *nstws = num;

    return areap;
}
#endif /* CICPP */
