#ifndef CIIFL_HPP
#define CIIFL_HPP

#include <cisisx.hpp>
//#include <cifst.hpp>

class CIIFL
//class _YOURCLASS CIIFL
{
 public:

  CISISX *cisisxp;

  char *ifllk_gidbnp;		/* dbn.par */

  typedef struct tree_nodes {
   int top;
   N2STRU  idx_node[MAX_TREE_LEVEL];
  } TREE_NODES;


  TREE_NODES load_idx[2];
  PUNT last_root;  /* Guarda a raiz atual antes de criar uma nova raiz*/
  L0STRU *l0p;
  L1STRU *l1p, l1node;
  L2STRU *l2p, l2node;
  L1IDXE init_leaf_el1;
  L2IDXE init_leaf_el2;
  N1IDXE init_node_el1;
  N2IDXE init_node_el2;
  LONGX  print_step;
#if TRACELOAD
  UCHR tkey[LE2+1], *ptkey; /* AOT 23/10/93 */
  int  ti;
#endif
  POSTSTRU pst,pst_ant;
  UCHR  *keyp;
  LONGX  nlido;
  PUNT p_b_punt,
             key_punt;
  INVMAP *invp;
  DBXSTRU *dbxp;
  N0STRU *n0p;
  UWORD isroot,i;
  UCHR  *p_b_key,
              a_p_b_key[LE2+1];
  BOOLEAN promoted;
  int topo,
      level,
      keysize,
      treecase,
      treecase_ant;
  UCHR key_ant[LE2+1],
       key[LE2+1];
  IFPBUFFER *ifpbuff; /* AOT - 16/09/93 */
#if !LIND
  INFO key_blk,
       key_pos;
 /*  int tag,occ,cnt; */   /* AOT - 26/10/93 */
  IFPAVAILPOS pn;
  IFPSTRU  *pf,
           *ph,
           *pb;
  INFO next_blk,
       next_pos;
  INFO totp,
       nxtb,
       nxtp,
       segp,
       segc;
  INFO off_f,
       off_h,
       npst;
  int gbuf;
  IFPSTRU current_seghead,
          first_seghead;
  IFPHEAD *currenthead,
          *firsthead;
  BOOLEAN flag_first_seghead,
          flag_current_seghead;
  UWORD idxpst;
#endif /* !LIND */
#if LIND
#include "ciiflh.c"
#endif /* LIND */
#if LKXONLY
  int lkxonly;    /* AOT - 14/05/98 */
#endif

/*25/07/97*/
/* Variaveis para guardar a localizacao ds headers de um segmento */
/* 128/(5+2) */
#define IFLmaxhd  IFPmaxhd*MAXIFPMEM + 1   /* headers em memoria */
#if CNV_PCBINUM
 int hdn=-1;       /* numero headers de um ifp */
 INFO hdblk[IFLmaxhd];
 INFO hdoff[IFLmaxhd];
#endif

#if LIND
 INFX tlcpaddr;  /* info1 */
#if MY
 LONGX tlcppsts;  /* info2 */
#endif
 INFX tlcpdocs;  /* info3 */
 LONGX tlcinfo3;  /* info3 for ciifl.c */
 UBYTE *areabits=NULL;
 LONGX threshold;
 LONGX tlcmxmfn;  /* current */
 FILE *fplog = NULL;
 char *iyp_buff=NULL;
 unsigned int iyp_left;
#define IYPBSIZ 8192L /* ou INT_MAX */

 UBYTE mask;
 UBYTE *bp;
 LONGX tlputby;
 int tlcpcase;
 LONGX tlcloop,tlcmfn;
 UBYTE *tlcp,*tlcq,tlcqmfn[PMFNSIZ];
 int tlcloop2;
#define XFD 0
#if XFD
 int xfd;
#endif

#endif /* LIND */
/* -----------------------------------------------------------------------*/

  CIIFL(CISISX *parm_cisisxp);
  void lifp_init_counters(void);
  void lifp_init_leaf(L0STRU *lp,int treecase);
  void lifp_init_node(N0STRU *n0p,int treecase);
  BOOLEAN lifp_insert_leaf(DBXSTRU *dbxp,char *key,char *p_b_key,
                         PUNT *p_b_punt, int treecase,INFX info1,
                         INFO  info2);
  BOOLEAN lifp_insert_node (DBXSTRU *dbxp,N0STRU *n0p,int treecase,
                          int level,
                          int isroot,UCHR *b_key,PUNT b_punt,
                          UCHR *p_key,PUNT *p_punt);
  void lifp_create_root(INVMAP *invp,N0STRU *n0p,PUNT esq,char *key,
		      PUNT dir,int treecase,BOOLEAN first);

  void determina_nova_qtda(int ordem,int a,int b,int *n_a,int *n_b);
  void init_invp_roots_nodes_leaves(INVMAP *invp);
  void balance_nodes(DBXSTRU *dbxp,INVMAP *invp,int treecase,
                 int level,PUNT idxpenultimo,PUNT idxultimo,N0STRU *np);
  void balance_btree(DBXSTRU *dbxp,INVMAP *invp,
                          int level,PUNT punt,int treecase);
  void balance_leaves(DBXSTRU *dbxp,INVMAP *invp,int treecase,
			   PUNT idxpenultimo,PUNT idxultimo,N0STRU *np);
  void lifp_store_btree(UCHR *key,INFX blk,INFO off,int treecase);
  IFPBUFFER *lifp_init(INVMAP *invp); /* antes de LKXONLY: (void) */
  void lifp_close_tree(void);
#if !LIND
  void lifp_grava_ifp(void);
  void lifp_storepst(void);
  void lifp_init_buff(IFPBUFFER  *p,INFO blk);
  void write_buffers(BOOLEAN keep_head,BOOLEAN reestrut_buff);
  void get_room_for (int qt,BOOLEAN opt_keep_head);
  void lifp_upd_pst_header(IFPSTRU *p,INFO offset,INFO nxtb,INFO nxtp,
                         INFO totp,INFO segp,INFO segc);
/* void lifp_upd_first_rec(IFPAVAILPOS *pn,INFO next_blk,UWORD idxpst); */
  void lifp_upd_first_rec(IFPAVAILPOS *pn,INFO next_blk,INFO next_post);
 /*19-02-95*/
  void lifp_init_ifprec(IFPSTRU *p,INFO blk);
#endif /* !LIND */
  int lifp_trans_key(UCHR *keyp, int len, int *treecase);

#if LIND
  LONGX svdiflind(char *dbnp,char *filekeys_1,char *filekeys_2,LONGX parmxmfn,char *parmlogp,int pstflag,LONGX tell);
#else
  LONGX svdifload(char *dbnp,char *filekeys_1,char *filekeys_2,int pstflag,LONGX tell);
#endif /* LIND */

#if LIND
  LONGX tlputifp(char *areap, LONGX nbytes);
  void iyp_storinit(LONGX parmxmfn, char *parmlogp);
  void iyp_storlast(int storcond, char *storkeyp, int pstflag);
  void iyp_storthis(LONGX mfn,LONGX parmxmfn,
                        char *keyp,LONGX count,int tag,int occ,int cnt);
  void iyp_storclos(void);
#endif /* LIND */
};

#endif // CIIFL_HPP
