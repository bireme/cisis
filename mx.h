/* ------------------------------ mx.h ------------------------------------- */

#if BEFORE20010221
#if VAX
#define MXMAIN          int
#else
#define MXMAIN          void
#endif
#endif /* BEFORE20010221 */

#define MJ_STWS         char

#if PC && !DOS32BITS
#define OPEN30 1
#endif

#define MAXNPRM         1024 /* 128 */  /* AOT, 22/06/2005 */
#define MAXNFPRM        16

#define JOINALL         1
#define MXGIZP          1
#define MXGIZHP         1

#define MAXNPPP         1024

#define MAXNJJJ         128
#if !CIAPI
#define MINBJJJ         (sizeof(((DBXSTRU *)NULL)->dbxname))
#else /* !CIAPI */
#define MINBJJJ         (CIMPL+1)
#endif /* !CIAPI */
#define MJASIZE         MAXMFRL
#define MAXVTAG         1000
#define MAXNSTW         800

#define MAXTBIN 32

#define MXFST 1
#define MXFULLINV   1

#if UNISYS || LIND
#define MXIFUPD 0
#else
#define MXIFUPD 1
#endif

typedef struct prmstru {
    char *p;
    int plen;
} PRMSTRU;

typedef struct mj_tagstru {
    UWORD tagv[MAXVTAG];
} MJ_TAGS;


#define MAXBOOL         (MAXMFRL)

#define LINSIZE         BUFSIZ
#define PRMSIZE         BUFSIZ

#define DUPPRIGHT       1
#define DUPPLEFT        2

#define MAXUPFTAG       1001

#define COPY            1
#define APPEND          2
#define MERGE           3
#define UPDATF          4

#define ISX(s) s?(strcmp(s,"x") == 0 || strcmp(s,"X") == 0 || strcmp(s,"/*") == 0):0

#if ANSI
int mj_scan(void *xmxp, char *gidbnp, char *dbname, char *specp, int parmjchk);
int mj_edit(void *xmxp, LONGX ridx, int i1, int i2, int tagoff, char **areapp);
int mj_join(void *xmxp, LONGX ridx, int ijjj, char *keyp, int tagoff, int l, int o, int c);
int mj_free(void *xmxp);
int mp_scan(void *xmxp, char *gidbnp, char *specp);
int mp_edit(void *xmxp, LONGX ridx, int i1, int i2);
int mp_free(void *xmxp);
int mxbool(void *xmxp, char *p, char *dbnp);
int mxexit(void *xmxp, int rc);
int mxtext(void *xmxp, LONGX irec, char *textp);
int mxinit(void **xmxpp);
#else
int mj_scan();
int mj_edit();
int mj_join();
int mj_free();
int mp_scan();
int mp_edit();
int mp_free();
int mxbool();
int mxexit();
int mxtext();
int mxinit();
#endif


/* ------------------------------ mx.c ------------------------------------- */

#if MXFUN
#if CIB71
#include "cib70.h"
#endif /* CIB71 */
#include "mxfun.h"
#include "mxrun.h"
#else /* MXFUN */

#if OPEN30
extern union REGS      d33_regs;
extern struct SREGS    d33_sregs;
extern unsigned short  newax;
#endif

extern char *parmpftoutp;
extern FILE *parmpftoutfp;
extern char *parmprologp;
extern char *parmepilogp;
extern LONGX  irecepilog;
#if CICGI
extern LONGX wwrec1;
extern LONGX wwrec2;
extern char *cgixpok;
#endif /* CICGI */

extern FMT_CODE *pgmp;
extern char *fmtarea;
extern LONGX fmtlw;
extern LONGX fmtlines;
extern LONGX fmtasize;
extern FILE *fp;

extern char *ptabtable;
extern int   ptabwidth;
extern LONGX  ptabcategories;
extern LONGX  ptabmaxprim;
extern LONGX *ptabCOUNT; 

extern LONGX parmtell;
extern LONGX parmload;
extern int parmtrace;
extern int parmcopy ;

extern char *parmdupp;
extern int parmdupx;

extern LONGX *tagfreq;

extern int parmiso1 ;
extern int parmiso2 ;
extern FILE *fpfix;             /* parmiso2=2 */

#if RECXPND
extern int parmxpn01;
extern int parmxpn02;
#endif
#if MXGIZP
extern char *parmgizp;
extern int parmgizhp;
#endif

extern char *prompt1p;
extern char *prompt2p;

extern char promptxx[];

extern char line[];             /* recfgets() + working */
extern char prmbuff[];          /* in= */

extern FILE *fpseq;
extern FILE *fpprm;

extern FILE *vfpprm[];
extern int nfpprms;

extern int tmpseq;              /* mxexit() */
extern char *mx_bup;            /* global */
extern LONGX mx_buplen;          /* global */

extern LONGX upcrec;             /* global */
extern LONGX upirec;             /* global */


#if MXFULLINV
extern char *fullinv_ifnp;
extern char *fullinv_ln1p;
extern char *fullinv_ln2p;
extern char *fullinv_lk1p;
extern char *fullinv_lk2p;
extern int fullinv_pstflag;
extern int fullinv_reset;
extern int fullinv_balan;
extern int fullinv_keep;
#if LIND
extern LONGX fullinv_maxmfn;
#endif
extern char *fullinv_master;
#endif

#if MXIFUPD
extern LONGX mstcrec;
extern LONGX mstirec;
extern char *ifnp;
extern int svdifupx;
extern int svdifupy;
#endif

#if MXIFUPD || MXFST
extern char *stwp;
extern FST_CODE *fstpgmp;
extern int pstflag;
extern int endup;
extern char *lnk1p;
extern char *lnk2p;
extern LONGX maxlk1;
extern LONGX maxlk2;
#endif


extern PRMSTRU *vprmp[];
extern LONGX nprms;


/* ------------------------------ mxbol.c --------------------------------- */

extern char *boolqryp;
extern LONGX boolridx;
extern LONGX boolhits,boolmfn,boolh;

extern char *booldbnp;          /* b4x query dbn */
extern LONGX booltell;           /* 0L = no msg */
extern LONGX booltlir;           /* b40 invx */
#if CIB71
#include "cib70.h"
extern b7_CIB7 *cib7p;
extern char booltmpnam[];       /* b72 MFQTHMEM tmp file */
#else /* CIB71 */
#if LIND || CIB64
extern LONGX boolnby1;           /* b62 nbytes1 */
extern LONGX boolnbyb;           /* b62 b6bufsiz */
extern LONGX boolnby0;           /* b62 b5irxbyts0 */
extern char booltmpnam[];       /* b62 MFQTHMEM tmp file */
#else
extern int b40trace;
extern char b4_tmpnam[];
extern LONGX boolnby2;           /* b42 nbytes2 */
#endif
#endif /* CIB71 */

/* ------------------------------ mxrel.c --------------------------------- */

/* ========================== mp_scan/mp_edit ============================= */

extern FMT_CODE *mp_pgmp[];
extern int mp_n;
extern char *mp_areap;

/* ====================== mj_scan/mj_edit/mj_join ========================= */

extern MJ_TAGS *mj_vtagp[];
extern MJ_STWS *mj_vstwp[];
extern FMT_CODE *mj_pgmp[];
extern LONGX mj_ridx[];
extern LONGX mj_plustag[]; /* AOT 23/04/2005 */
extern LONGX mj_tidx;
extern char *mj_areap;
extern char *mj_buffup;
extern int mj_pjchk[];
extern int mj_n;
extern LONGX mj_jmax;
extern UWORD mj_jtag;

/* ------------------------------ mxtxt.c --------------------------------- */

extern int  mxtext_found;
extern int  mxtext_fdir;
extern int  mxtext_foff;
extern int  mxtext_pshow;

#if CIAPI
#if !MXFUN
extern void* ciapip;
#endif /* MXFUN */
#endif /* CIAPI */

#endif /* MXFUN */
