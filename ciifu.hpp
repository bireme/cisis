#ifndef CIIFU_HPP
#define CIIFU_HPP

#include <cisisx.hpp>
#include <cifst.hpp>

class _YOURCLASS CIIFU //: public CISIS
{
 public:

  CISISX *cisisxp;

  int ifupd_reset;
  int ifupd_wrnmsg;

  UCHR key[LE2+1],*keyp;
  INVMAP *invp;
  DBXSTRU *dbxp;

#if TRACEUPIF
  UCHR tkey[LE2+1]; /* AOT 11/09/2000 */
#endif /* TRACEUPIF */

  TREE_PATH lfpath;
  PUNT puntlf;

#if LINDLUX /* 2 */
  int luxtree,luxn;
  LONGX luxpunt,luxpos,luxx,luxaddr;
  L0STRU *luxpagp;
  L1STRU *luxp1;
  L2STRU *luxp2;
  char *luxp;
/* <============================ */
#else /* LINDLUX 2 */
#if !LIND /* 8x2 */
  typedef struct ifpkeydata {
        BOOLEAN  st;            /* record modified */
        IFPSTRU  reg;
  } IFPKEYDATA;

#define  UNDEFINED   -1
#define  CURR_INP    0
#define  SEGH        1
#define  FIRST       2
#define  CURR_OUT    3
#define  PREG(x)     (IFPSTRU *)&(v[x].reg)
#define  BLKX(x)     BLK( (PREG(x)) )
#define  STATUS(x)   v[x].st

  IFPKEYDATA v[4];
  int idx_curr_inp,idx_segh,idx_first,idx_curr_out;
  BOOLEAN dupl_msg,ja_inseriu;
  POSTSTRU *pinitpst,initpst;
  UCHR buf[IFBSIZ],*pbuf;
  IFPAVAILPOS pn;
  INFO totp_inp,nxtb_inp,nxtp_inp,segp_inp,segc_inp,
	    accurate_totp,nxtb_out,nxtp_out,npst_read_in_seg,
	    off_fh_inp,off_ch_inp,blk_inp,off_inp,
	    idxpst_inp,idxpst_out;
  IFPSTRU *pcur_blk,*pcur_blk_out;
#endif /* !LIND 8x2 */
#endif /* LINDLUX 2 */
  LONGX nlidos;

#if CNV_PCBINUM
  int hdn;       /* numero headers de um ifp */
  INFO hdblk[IFLmaxhd];
  INFO hdoff[IFLmaxhd];
#endif /*CNV_PCBINUM */

  CIIFU(CISISX *parm_cisisxp);

#if TRACEUPIF
  void upif_print_lfitem(int   treecase,
                         UCHR *lfitem);
#endif
  void upif_print_msg(POSTSTRU *pst,
                      UCHR     *msg);

#if !LIND /* 3 */
  void verif_grava_rec(DBXSTRU *dbxp,
                       int      idx);

  void update_frst_curr_headers(INFO nelem);
  void read_ifpcntrl();
#endif

#if BEFORE20000926
  int upif_end(int pstflag);
#else /* BEFORE20000926 */
  int upif_end(char *dbnp,
               int pstflag);
#endif /* BEFORE20000926 */
  void upif_init(char *dbnp);

#if !LIND /* 7 */
  void update_ifpcntrl(INFO nxtb,
                       INFO nxtp);
#endif /* !LIND 7 */

  void print_post(POSTSTRU *pstp);

#if !LIND /* 8 */
  void next_output_blk();
  void next_input_blk();
  void next_input_seg();
  void next_input_element();
  void next_output_element();

  void copia_posting(POSTSTRU *ppst,
                     int      *ja_inseriu);

  void insere_posting(int idx_ifprec,
                      int idx,
                      POSTSTRU *ppst);

  void gethead_counters(IFPSTRU *p,
                      INFO   off,
                      INFO  *nxtb,
                      INFO  *nxtp,
                      INFO  *totp,
                      INFO  *segp,
                      INFO  *segc);

  void update_header(IFPSTRU *pifp,
                     INFO off,
                     INFO  nxtb,
                     INFO  nxtp,
                     INFO  totp,
                     INFO  segp,
                     INFO  segc);

  void create_new_segment(INFO segp,
                          INFO segc);

  void posiciona_inicio_seg();

  void upif_split_segment(POSTSTRU *ppst);
  void upif_find_segment(POSTSTRU *ppst);
  void upif_insert_into_seg(POSTSTRU *ppst);

  void upif_process_posting(POSTSTRU *ppst,
                            INFO blk,
                            INFO off,
                            UCHR operation);

  void upif_process_new_key(POSTSTRU *ppst,
                            INFO     *blk,
                            INFO     *off);
#endif /* !LIND 8 */

  void trace_records(LONGX tell);

  void optlnk2(char *lnk2p,
               LONGX qtylk2,
               char *olnk2p,
               LONGX oqtylk2);

  void optlnk1(char *lnk1p,
               LONGX  qtylk1,
               char *olnk1p,
               LONGX oqtylk1);

  void svdifupd(char *lnk1p,
                LONGX qtylk1,
                char *lnk2p,
                LONGX qtylk2,
                char *olnk1p,
                LONGX oqtylk1,
                char *olnk2p,
                LONGX oqtylk2,
                int  pstflag,
                LONGX tell);

#if !LIND /* 9 */
  void upif_delete_elem();
  void upif_delete_posting(POSTSTRU *ppst);
#endif /* !LIND 9 */

#if !INPUTFILE
#if USE_GDBFMT
  int ifupdat(FSTSTRU *fst,
              LONGX mfn1,
              LONGX mfn2,
              int pstflag,
              int endup);
#else
   int ifupdat(
	char *mdbnp,
	LONGX mfn1,
	LONGX mfn2,
	char *idbnp,
	FSTSTRU *fst,
	int pstflag = IFUPISIS,
	int endup = IFUPWRIT);
#endif /* USE_GDBFMT */
#endif /* INPUTFILE */

  BOOLEAN upif_find_key_node(N0STRU *np,
			     UCHR   *key,
			     int     treecase,
			     int    *pos);

  BOOLEAN upif_find_key_leaf(L0STRU *lp,
			    UCHR   *key,
			    int     treecase,
			    int    *pos);

  PUNT  upif_create_root(DBXSTRU *dbxp,
		         PUNT     esq,
		         UCHR    *key,
		         PUNT     dir,
		         int      treecase,
		         BOOLEAN  first);

  BOOLEAN upif_insert_index (DBXSTRU *dbxp,
			     PUNT  punt,
			     int   treecase,
			     int   level,
			     int   isroot,
			     UCHR *b_key,
			     PUNT  b_punt,
			     UCHR *p_key,
			     PUNT *p_punt);

  void upif_branqueia(UCHR *str,
		      int   size);

  BOOLEAN upif_insert_leaf(DBXSTRU *dbxp,
			   L0STRU  *lp,
			   UCHR    *key,
			   int      treecase,
			   UCHR    *p_b_key,
			   PUNT    *p_b_punt,
			   int      pos,
			   INFO     TSTblk,
			   INFO     TSToff);

  PUNT upif_init_index(DBXSTRU *dbxp,
		       int treecase,
		       UCHR *key,
		       INFO TSTblk,
		       INFO TSToff);

  PUNT upif_search_btree(INVMAP *invp,
    		         int     treecase,
		         UCHR   *key);

  void upif_fnd_pifp(DBXSTRU  *dbxp,
		     UCHR     *key,
		     int       treecase,
		     POSTSTRU *pst,
		     UCHR      oper,
		     int       pstflag);
};
#endif // CIIFU_HPP
