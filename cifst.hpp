#ifndef CIFST_HPP
#define CIFST_HPP

#include <cirec.hpp>

#if USE_GDBFMT
#include <mffunc.hpp>
#include <iffunc.hpp>
#endif

class _YOURCLASS FSTSTRU
{
 public:

  int cifstfix;            /* key %8ld %5d %4d %4d */
  int cifstfim;            /* key %8ld w/ cifstfix */
  LONGX fst_error;          /* gener - fst spec error */
  LONGX fst_errl;           /* gener - fst spec line where error occured */
  char *fst_errp;          /* gener - fst spec ptr where error occured */
  char *fst_fmtap;         /* inter - default: no static format area */
  LONGX fst_fmtby;          /* inter - bytes for dynamic format area */
  char fst_rdlm;           /* inter - new occ delimiter */
  char *fst_batchup;       /* inter - batchp for fldupdat() area */
  char *fst_batchp;        /* inter - batchp for fldupdat() commands */
  int fst_batch0;          /* inter - batchp ^mmfn^oocc^ccnt^l1/2 option */
  char fst_a351;           /* inter - 351 */
  UCHR *fst_fbuff[2];      /* writbsiz */
  UWORD fst_fbsiz;
  UWORD fst_fleft[2];
  UWORD fst_ffree[2];
  int fst_fd[2];           /* inter - file descriptor for .ln1/.ln2 */
  LONGX fst_fx[2];          /* inter - output records to .ln1/.ln2 */

  char *fst_hdrp;          /* inter - hit file headers - area */
  int fst_hdrx;            /* inter - hit file headers - #bytes allocated */
  int fst_hdru;            /* inter - hit file headers - #bytes used */
  int fst_hdrt;            /* inter - hit file headers - for area+^m^t^o^c */

  char *fst_wlupifnp;      /* inter - word lookup I/F for TW processing */
  TRMSTRU *fst_wluptrmp;      /* inter - word lookup I/F for TW processing */
  LONGX *fst_wlupitrm;      /* inter - word lookup I/F for TW processing */

  FST_CODE *fst_pgmp;      // inter
  char     *fst_stwp;      // inter
  int       fst_nstws;     // inter
  char     *fst_area1p;    // inter
  LONGX      fst_maxlk1;    // inter
  char     *fst_area2p;    // inter
  LONGX      fst_maxlk2;    // inter
  LONGX      fst_qtylk1;    // inter
  LONGX      fst_qtylk2;    // inter

  char     *fst_oarea1p;   // inter
  char     *fst_oarea2p;   // inter
  LONGX      fst_oqtylk1;   // inter
  LONGX      fst_oqtylk2;   // inter

#if USE_GDBFMT
  MST_FILE *fst_mst;
  BTREE *fst_btree;
#endif
  RECSTRU *fst_recp;

  CISISX *cisisxp;

  FSTSTRU (CISISX *parm_cisisxp, char *gidbnp, char *fstfile, char *stwfile,
#if USE_GDBFMT
           MST_FILE *mst, BTREE *btree,
#endif
	   LONGX maxlk1,LONGX maxlk2,int instream=0, int stw_instream=0);

  ~FSTSTRU(void);
   
  char * xfst_loadfst(char *gidbnp,char *file,int instream=0);
  char * xfst_loadstw(char *gidbnp,char *file, int *nstwsp, int instream=0);

  RECSTRU * xfst_getrecp(void);
  void xfst_setrecp(RECSTRU *recp);

  char * xfst_batch(LONGX nbytes, int option);

  LONGX xfst_gener(char *fstspecp);

  LONGX xfst_inter(int bwcase=0);

  void xfst_free(void);

  int  xfst_link(char *pfxp,
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
                 LONGX *lft2);

  int  xfst_writ(int   treecase,
                 LONGX  mfn,
                 UWORD   tag,
                 UWORD   occ,
                 UWORD   cnt,
                 char *keyp,
                 int   keylen,
                 UCHR *uctabp);

  int  xfst_open(char *gidbnp, char *filnamp,
                 int   treecase);

  int xfst_clos(int treecase);

  void xpstdecod(POSTSTRU *pst,
                 LONGX     *mfn,
                 UWORD      *tag,
                 UWORD      *occ,
                 UWORD      *cnt);

  void xprtlink(char *stubp,
                LONGX  pmfn,
                UWORD   ptag,
                UWORD   pocc,
                UWORD   pcnt,
                UCHR *keyp,
                int   len);

  char * xloadmemstw(char *memp,
                     int  *nstws);

};

#endif // CIFST_HPP



