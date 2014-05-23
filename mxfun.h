/* ==----------------------------- mxfun.h ------------------------------== */

typedef struct mxfun_xmx {

#if OPEN30
union REGS      d33_regs;
struct SREGS    d33_sregs;
unsigned short  newax;
#endif

char *parmpftoutp;
FILE *parmpftoutfp;
char *parmprologp;
char *parmepilogp;
LONGX  irecepilog;
#if CICGI
LONGX wwrec1;
LONGX wwrec2;
char *cgixpok;
#endif /* CICGI */

FMT_CODE *pgmp;
char *fmtarea;
LONGX fmtlw;
LONGX fmtlines;
LONGX fmtasize;
FILE *fp;

char *ptabtable;
int   ptabwidth;
LONGX  ptabcategories;
LONGX  ptabmaxprim;
LONGX *ptabCOUNT; 

LONGX parmtell;
LONGX parmload;
int parmtrace;
int parmcopy;

char *parmdupp;
int parmdupx;

LONGX *tagfreq;

int parmiso1;
int parmiso2;
FILE *fpfix;    /*parmiso2=2 */

#if RECXPND
int parmxpn01;
int parmxpn02;
#endif
#if MXGIZP
char *parmgizp;
int parmgizhp;
#endif

char *prompt1p;
char *prompt2p;

char promptxx[2];

char line[LINSIZE];              /* recfgets() + working */
char prmbuff[PRMSIZE];           /* in= */

FILE *fpseq;
FILE *fpprm;

FILE *vfpprm[MAXNFPRM];
int nfpprms;

int tmpseq;                   /* mxexit() */
char *mx_bup;                   /* global */
LONGX mx_buplen;              /* global */

LONGX upcrec;                 /* global */
LONGX upirec;                 /* global */

#if MXFULLINV
char *fullinv_ifnp;
char *fullinv_ln1p;
char *fullinv_ln2p;
char *fullinv_lk1p;
char *fullinv_lk2p;
int fullinv_pstflag;
int fullinv_reset;
int fullinv_balan;
int fullinv_keep;
#if LIND
LONGX fullinv_maxmfn;
#endif
char *fullinv_master;
#endif

#if MXIFUPD
LONGX mstcrec;
LONGX mstirec;
char *ifnp;
int svdifupx;
int svdifupy;
#endif

#if MXIFUPD
int pstflag;
int endup;
#endif

#if MXIFUPD || MXFST
char *stwp;
FST_CODE *fstpgmp;
char *lnk1p;
char *lnk2p;
LONGX maxlk1;
LONGX maxlk2;
#endif

PRMSTRU *vprmp[MAXNPRM];        /* pointers to the entries      */
LONGX nprms;                   /* actual #entries              */
LONGX maxnprm;           /* runtime value                */


/* ------------------------------ mx.h ------------------------------------- */

#if RECXPND
#endif

#if RECGIZM
#endif




/* ------------------------------ mxbol.c --------------------------------- */

char *boolqryp;
LONGX boolridx;
LONGX boolhits;
LONGX boolh;

char *booldbnp;          /* b4x query dbn */
LONGX booltell;               /* 0L = no msg */
LONGX booltlir;               /* b40 invx */

#if CIB71
b7_CIB7 *cib7p;
char booltmpnam[CIMPL+1];    /* b72 MFQTHMEM tmp file */
#else /* CIB71 */
#if LIND || CIB64
LONGX boolnby1;               /* b62 nbytes1 */
LONGX boolnbyb;               /* b62 b6bufsiz */
LONGX boolnby0;               /* b62 b5irxbyts0 */
char booltmpnam[CIMPL+1];    /* b62 MFQTHMEM tmp file */
int boolerrno;                /* b62 errno */
#else
LONGX boolnby2;          /* b42 nbytes2 */
#endif
#endif /* CIB71 */

/* ------------------------------ mxrel.c --------------------------------- */

/* ========================== mp_scan/mp_edit ============================= */

FMT_CODE *mp_pgmp[MAXNPPP];
int mp_n;
char *mp_areap;

/* ====================== mj_scan/mj_edit/mj_join ========================= */

MJ_TAGS *mj_vtagp[MAXNJJJ];
MJ_STWS *mj_vstwp[MAXNJJJ];
FMT_CODE *mj_pgmp[MAXNJJJ];
LONGX mj_ridx[MAXNJJJ];
LONGX mj_plustag[MAXNJJJ]; /* AOT 23/04/2005 */
LONGX mj_tidx;
char *mj_areap;
char *mj_buffup;
int mj_pjchk[MAXNJJJ];
int mj_n;
LONGX mj_jmax;
UWORD mj_jtag;

/* ------------------------------ mxtxt.c --------------------------------- */

int  mxtext_found;
int  mxtext_fdir;
int  mxtext_foff;
int  mxtext_pshow;

/* ------------------------------ ciapip --------------------------------- */
void *mx_ciapip;
} MXFUN_XMX;

/* ==----------------------------- mxfun.h ------------------------------== */

