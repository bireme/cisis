#ifndef CIUPI_H
#define CIUPI_H

#define TRACEhh        0
#define DEBIFUPD  0
#define IFUPDXUP  1
#define IFUPDXLD  2

#define LKXONLY 1			/* allow .lkx processing */


/* IFMAXTIV=127 numero de elementos do ifprec. O primeiro e' o numero
   do bloco. Sobram 126 posicoes p/ postings. O indice varia 0..125
*/
#define MAXIDXPST        IFMAXTIV-2  /* Indice maximo p/ um posting */

#if SINDO_OK
/* IFMAXTIV=127 numero de elementos do ifprec.  O indice varia 0..126
		#define IFMAXTIV    127         /* gdb */
		typedef struct ifpstru {
		    INFO     ifpblk;            /* gdb .ifp blk order */
		    INFO     ifprec[IFMAXTIV];  /* gdb .ifp data */
		} IFPSTRU;
*/
#define MAXIDXPST        (IFMAXTIV-1)  /* Indice maximo p/ um posting */
#endif /* SINDO_OK */

#define PSTSIZE          2
#define IDXHEADSIZE      5

#ifndef BOOLEAN
#define BOOLEAN         int
#endif  /* BOOLEAN */ /* HB - winnt.h */

#define FALSE           0
#define TRUE            1

#define UNDEF_OPER      ' '
#define INSERTION       'I'
#define DELETION        'D'

#define NOFIRST         FALSE
#define ISFIRST         TRUE
#define MAX_TREE_LEVEL  MAXLIV


#define MAXIFPMEM               2       /* Number of IFPREC in memory */
#define MAXPSTSSEG              32767   /* gdb ? */
#define KEEP_HEADERS            TRUE
#define DONT_KEEP_HEADERS       FALSE
#define BUFFER_FULL             gbuf>=MAXIFPMEM-1

typedef struct ifpavailpos {    /* First ifp record */
    INFO     ifpblk;            /* gdb .ifp blk order */
    INFO     nxtb;              /* ifprec[0] */
    INFO     nxtp;              /* ifprec[1] */
} IFPAVAILPOS;

typedef struct ifpbuffer {
    IFPSTRU buff[MAXIFPMEM];
} IFPBUFFER;

typedef struct ifpheaders {
   INFO ifpblk;
   INFO offset;
}IFPHEADERS;

typedef struct tree_path {
  int top;
  PUNT  stck[MAX_TREE_LEVEL];
 } TREE_PATH;

typedef struct n1struw {
    PUNT     pos               PACKED;  /* gdb .n01 pos */
    UWORD      ock               PACKED;  /* gdb .n01 ock */
    UWORD      it                PACKED;  /* gdb .n01 it */
    N1IDXE   idx[TWORDN+1]     PACKED;  /* gdb .n01 idx */
} N1STRUW;

typedef struct n2struw{
    PUNT     pos               PACKED;  /* gdb .n02 pos */
    UWORD      ock               PACKED;  /* gdb .n02 ock */
    UWORD      it                PACKED;  /* gdb .n02 it */
    N2IDXE   idx[TWORDN+1]     PACKED;  /* gdb .n02 idx */
} N2STRUW;

typedef struct l1struw {
    PUNT     pos               PACKED;  /* gdb .l01 pos */
    UWORD      ock               PACKED;  /* gdb .l01 ock */
    UWORD      it                PACKED;  /* gdb .l01 it */
    PUNT     ps                PACKED;  /* gdb .l01 ps */
#if LIND
    PUNT     psb               PACKED;  /* tlc gdb .l0x ps backward */
#endif
    L1IDXE   idx[TWORDF+1]     PACKED;  /* gdb .l01 idx */
} L1STRUW;

typedef struct l2struw {
    PUNT     pos               PACKED;  /* gdb .l02 pos */
    UWORD      ock               PACKED;  /* gdb .l02 ock */
    UWORD      it                PACKED;  /* gdb .l02 it */
    PUNT     ps                PACKED;  /* gdb .l02 ps */
#if LIND
    PUNT     psb               PACKED;  /* tlc gdb .l0x ps backward */
#endif
    L2IDXE   idx[TWORDF+1]     PACKED;  /* gdb .l02 idx */
} L2STRUW;

#define IFPmaxhd (IFMAXTIV+1)/(IDXHEADSIZE+PSTSIZE) /* 128/(5+2) */

#if ANSI
void ifp_init_hd(int *hdn);
void ifp_ins_new_hd(INFO blk,INFO off,
		    INFO hdblk[],INFO hdoff[],int hdmax,int *hdn);
#else
void ifp_init_hd();
void ifp_ins_new_hd();
#endif

#if DEBIFUPD
#if ANSI
void upif_chgtostr(UCHR *v,int n,UCHR *tkey);
void upif_gera_keys(INVMAP *invp,int ident,
                    int level, PUNT punt,int treecase,FILE *fkeys);
void upif_print_btree(INVMAP *invp,int ident,int level,
                      PUNT punt,int treecase,FILE *fout);
void upif_print_leaf(L0STRU *l0p);
void upif_print_node(N0STRU *n0p);
void lifp_print_cnt(INVMAP *invp);
#else
void upif_chgtostr();
void upif_gera_keys();
void upif_print_btree();
void upif_print_leaf();
void upif_print_node();
void lifp_print_cnt();
#endif
#endif /* DEBIFUPD */

#endif /* CIUPI_H */
