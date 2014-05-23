#ifndef CIRUN_HPP
#define CIRUN_HPP

//  ------------------------- CIDBX.HPP ----------------------------------
#ifndef CIDBX_SOURCE
#if MULTI
//#define dbxcinet                DBXSTRU::xdbxcinet              // moved to CISISX
#define dbxflock(dbxp,typ)        dbxp->xdbxflock(dbxp,typ)       // external use
#define dbxulock(dbxp,typ)        dbxp->xdbxulock(dbxp,typ)       // external use
//#define dbxilock(dbxp,mdbnp,typ)  dbxp->xdbxilock(dbxp,mdbnp,typ) // moved to CISISX
#define dbxwlock(dbxp,m0p,times)  dbxp->xdbxwlock(dbxp,m0p,times) // external use
#endif
//#define dbxinit                 DBXSTRU::xdbxinit         // moved to CISISX
//#define dbxstorp                DBXSTRU::xdbxstorp        // moved to CISISX
//#define dbxsrchp                DBXSTRU::xdbxsrchp        // moved to CISISX
//#define dbxopen                 DBXSTRU::xdbxopen         // moved to CISISX
//#define dbxopenw                DBXSTRU::xdbxopenw        // moved to CISISX
//#define dbxopenc                DBXSTRU::xdbxopenc        // moved to CISISX

//#define fatal                   DBXSTRU::xfatal           // moved to CISISX

#ifndef CISISX_SOURCE
//#define fatal(msg)                cisisxp->getDBXSTRU(NULL)->xfatal(msg)
#define dbxflush(dbnamp)          cisisxp->getDBXSTRU(dbnamp)->xdbxflush(dbnamp) // external use
#define mstflush(dbnamp)          cisisxp->getDBXSTRU(dbnamp)->xmstflush(dbnamp) // external use
#define invflush(dbnamp)          cisisxp->getDBXSTRU(dbnamp)->xinvflush(dbnamp) // external use
#define cicopyr                   cisisxp->getDBXSTRU(NULL)->xcicopyr            // external use
#else
//#define fatal(msg)                getDBXSTRU(NULL)->xfatal(msg)
#define dbxflush(dbnamp)          getDBXSTRU(dbnamp)->xdbxflush(dbnamp) // external use
#define mstflush(dbnamp)          getDBXSTRU(dbnamp)->xmstflush(dbnamp) // external use
#define invflush(dbnamp)          getDBXSTRU(dbnamp)->xinvflush(dbnamp) // external use
#define cicopyr                   getDBXSTRU(NULL)->xcicopyr           // external use
#endif // CISISX_SOURCE

#define mstclose(dbxp)            dbxp->xmstclose(dbxp)       // external use
#define dbxinvmp(dbxp)            dbxp->xdbxinvmp(dbxp)       // external use
#define invclose(dbxp)            dbxp->xinvclose(dbxp)       // external use
//#define dbxciset                DBXSTRU::xdbxciset        // moved to CISISX
//#define dbxcipar                DBXSTRU::xdbxcipar        // moved to CISISX
//#if GIPAR
//#define dbxgipar                DBXSTRU::xdbxgipar        // moved to CISISX
//#endif
#define fpccreat                  dbxp->xfpccreat             // external use
#define fpcwrite                  dbxp->xfpcwrite             // external use
#define fpcclose                  dbxp->xfpcclose             // external use
//#define dbxtmpnm		  dbxp->xdbxtmpnm             // moved to CISISX
#if GEN_CORELEFT
#define coreleft                  dbxp->xcoreleft             // external use
#endif
#if GEN_LABS
#define labs                      DBXSTRU::xlabs              // external use
#endif
#if GEN_STRSTR
#define strstr                    DBXSTRU::xstrstr            // external use
#endif
#if GEN_STRUPR
#define strupr                    DBXSTRU::xstrupr            // external use
#endif
#if GEN_STRREV
#define strrev                    DBXSTRU::xstrrev            // external use
#endif
#if GEN_MEMICMP
#define memicmp                   DBXSTRU::xmemicmp           // external use
#endif
#endif // CIDBX_SOURCE
//  ----------------------------------------------------------------------

// ------------------------- CIREC.HPP -----------------------------------
#ifndef CIREC_SOURCE
//#define recalloc(irec,nbytes) xrecalloc(nbytes)           no conversion
//#define record(irec,dbnamp,mfn) xrecord(dbnamp,mfn)       no conversion
#define recread(recp, mfn)      recp->xrecread(mfn)         // external use
#define recxref(recp,mfn,comb,comp) recp->xrecxref(mfn,comb,comp)   // external use
//#define nocc(irec,tag)        xnocc(tag)                  no conversion
//#define fieldn(irec,tag,occ)  xfieldn(tag,occ)            no conversion
//#define fieldx(irec,tag,occ)  xfieldx(tag,occ)            no conversion
#define mstsetup                recp->xmstsetup             // external use

#if MULTI
//#define recunlck(irec,option) xrecunlck(option)           no conversion
#endif

//#define recisis0                recp->xrecisis0           // moved to CISISX
//#define recupdat(crec,irec)   xrecupdat()                 no conversion
#define recwrite(crecp,recp)    recp->xrecwrite(crecp)      // external use
#define recwmast(crecp,recp,comb,comp,newblk,wlen)  recp->xrecwmast(crecp,comb,comp,newblk,wlen)  // external use
#define recwxref(recp,pointer,last) recp->xrecwxref(pointer,last)  // external use
//#define fldupdat(irec,batchp) xfldupdat(batchp)           no conversion

//#if IFUPDATE
//#define recreset                recp->xrecreset             // moved to CISISX
//#endif

//#define recfield                recp->xrecfield             // external use
#endif // CIREC_SOURCE
// -----------------------------------------------------------------------

// ------------------------- CITRM.HPP -----------------------------------
#ifndef CITRM_SOURCE
//#define trminit                 trmp->xtrminit           // no external use
//#define trmalloc(itrm,nbytes) xtrmalloc(nbytes)           no conversion
//#define term(itrm,dbnamp,keyp)xterm(dbnamp,keyp)          no conversion
//#define trmread                 trmp->xtrmread           // external use
//#define cntread                 trmp->xcntread           // moved to CISISX
//#define invsetup                trmp->xinvsetup          // moved to CISISX
//#define noderead                trmp->xnoderead          // moved to CISISX
//#define leafread                trmp->xleafread          // moved to CISISX
//#define nxterm                xnxterm                     no conversion
#define posthdr1                trmp->xposthdr1          // external use
//#define posting(itrm,nord)    xposting(nord)              no conversion
#define postpost                trmp->xpostpost          // external use
//#define postread                trmp->xpostread          // moved to CISISX
#endif // CITRM_SOURCE
// -----------------------------------------------------------------------

// ------------------------- CIISO.HPP -----------------------------------
#ifndef CIISO_SOURCE
#define iso_creat     xiso_creat                            /* internal use */
#define iso_write     xiso_write                            /* internal use */
#define iso_bmove     xiso_bmove                            /* internal use */
#define iso_break     xiso_break                            /* internal use */
#define iso_close     xiso_close                            /* internal use */
#if GEN_ISI
#define iso_open      xiso_open                             /* internal use */
/* #define iso_read    xiso_read                            no conversion */
#define iso_loadbuff  xiso_loadbuff                         /* internal use */
#define iso_getval    xiso_getval                           /* internal use */
#endif
#endif // CIISO_SOURCE
// -----------------------------------------------------------------------

// ------------------------- CIFST.HPP -----------------------------------
#ifndef CIFST_SOURCE
//#define fst_gener               fst->xfst_gener           // external use
//#define fst_free                fst->xfst_free            // external use
//#define fst_inter               fst->xfst_inter           // external use
#define fst_link                  fst->xfst_link            // external use
#define fst_writ                  fst->xfst_writ            // external use
#define fst_open                  fst->xfst_open            // external use
#define fst_clos                  fst->xfst_clos            // external use
#define pstdecod                  fst->xpstdecod            // external use
#define prtlink                   fst->xprtlink             // external use
#endif // CIFST_SOURCE
// -----------------------------------------------------------------------

// ------------------------- CIUTL.HPP -----------------------------------
#ifndef CIUTL_SOURCE
#endif // CIUTL_SOURCE
// -----------------------------------------------------------------------
#endif // CIRUN_HPP
