#ifndef CIREC_HPP
#define CIREC_HPP

#include <cidbx.hpp>

class _YOURCLASS RECSTRU // : public CISIS
{
 public:

  LONGX     recnbytes;         /* nbytes allocated (RECHSIZE not included) */
  int      rectype;           /* layout indicator */
  DBXSTRU *recdbxp;           /* ptr dbx */
  int      recrc;             /* record rc */
#if MULTI
  int      reclock;           /* mfr read/lock option */
  int      recwlock;          /* mfr write/lock option */
#endif /* MULTI */
  int      recgdbl;           /* mfr read/mfrl lock granted */
  int      recgdbw;           /* mfr read/mfrl flag */
 MFUNION  *recmfp;            /* ptr to master_recs and .mx unions */
 CISISX   *cisisxp;

 RECSTRU  (CISISX *cxp);

 RECSTRU  (CISISX *cxp,
           const RECSTRU &other);  // copy constructor

 ~RECSTRU (void);

 LONGX     xrecalloc(LONGX nbytes);

 LONGX     xrecord(char *dbnamp,
		  LONGX  mfn);

 int      xrecread(LONGX mfn);

 int      xrecxref(LONGX     mfn,
		   LONGX    *comb,
		   int     *comp);

 int      xnocc(int tag);

 int      xfieldn(int tag,
		  int occ);

 int      xfieldx(int tag,
		  int occ);

 void     xmstsetup(char *dbnamp,
		    LONGX  loadxrf,
		    LONGX  loadmst);

#if MULTI
 int      xrecunlck(int option);
#endif

 //int      xrecisis0(char *dbnamp);   /* moved to CISISX */

 int      xrecupdat(void);

 int      xrecwrite(RECSTRU *crecp);

 int      xrecwmast(RECSTRU *crecp,
		    LONGX     comb,
		    int      comp,
		    int      newblk,
		    FFI      wlen);

 int      xrecwxref(XRPTR    pointer,
		    LONGX     lastmfn);

 char    *xfldupdat(char *batchp);

// char    *xrecfield(char *areap,
//		    LONGX  ridx,
//		    int   tag,
//		    int   iocc,
//		    char *defaultp);

#if HBARB
 RECSTRU *xRecord(char *dbname,
		  LONGX  mfn);

 RECSTRU *xRecAlloc(LONGX nbytes);
#endif /* HBARB */
};

#endif // CIREC_HPP
