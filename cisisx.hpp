#ifndef CISISX_HPP
#define CISISX_HPP

#include <cisis.h>
#if IFUPDATE
#include <ciupi.h> // uses BOOLEAN + LKXONLY
#endif

#if CICPP

class ISO_2709;

class _YOURCLASS CISISX
{
 private:

  void cidbx_init(void);
  void cirec_init(void);
  void citrm_init(void);
  void ciifl_init(void);
  void ciutl_init(void);

 public:

//------------------------- CISIS.HPP ----------------------------------

  DBXSTRU      *vdbxp[MAXNDBX];          /* pointers to the entries   */
  LONGX          ndbxs;                   /* actual #entries           */
  LONGX          maxndbx;                 /* runtime value             */
#if FATRAP
  int           fatal_errcod;            /* cisis fatal() error code  */
  char          fatal_iomsg[MAXFATML+1]; /* cisis fatal() flag/msg    */
  jmp_buf       fatal_jumper;            /* cisis fatal() longjmp()   */
#endif
  static unsigned char init_isisuctab[256];
  static unsigned char init_isisactab[256];
  unsigned char isisuctab[256];
  int           isiswctot;
  unsigned char isiswctab[256];

//#ifdef CI_WINISIS
  unsigned char isisactab[256];
//#else
//  unsigned char isisactab[];
//#endif
  int           NXPAGES[MAXLIV];               /* [MAXLIV] */

#ifndef CISISX_SOURCE

#define vdbxp          cisisxp->vdbxp
#define ndbxs          cisisxp->ndbxs
#define maxndbx        cisisxp->maxndbx
#define fatal_errcod   cisisxp->fatal_errcod
#define fatal_iomsg    cisisxp->fatal_iomsg
#define fatal_jumper   cisisxp->fatal_jumper
#define isisuctab      cisisxp->isisuctab
#define isiswctot      cisisxp->isiswctot
#define isiswctab      cisisxp->isiswctab
#define isisactab      cisisxp->isisactab
#define NXPAGES        cisisxp->NXPAGES

#endif // CISISX_SOURCE

//------------------------- CIDBX.C ------------------------------------

#if SHTEST
  char shtest[CIMPL+1];
#endif /* SHTEST */

#if CNV_PCBINUM
  char cnv_pcbuff[MAXMFRL];	   /* PC data representation (swapped) */
#endif

#if CNV_PCFILES
  UCHR nodeunibuff[N2BSIZ];      /* area para ler pagina de .n01/.n02 */
#if ISISUNISYS
#define LEAFCNV_PCFILES 0
#else
#define LEAFCNV_PCFILES 1
#endif
#if LEAFCNV_PCFILES
  UCHR leafunibuff[L2BSIZ];       /* area para ler pagina de .l01/.l02 */
#endif
#endif

  int partrace;            /* dbxopen/dbxcipar trace */
  int dbxtrace;            /* dbxopen/dbxcipar trace */
  int rectrace;            /* rec RESTRACE runtime switch */
  int dectrace;            /* decoread()/recdeco() runtime switch  */
  int trmtrace;            /* trm TRSTRACE runtime switch */
  int b40trace;            /* b40 RUXTRACE runtime switch */
  int b50trace;            /* b50 RUXTRACE runtime switch */
  int b70trace;            /* b70 RUXTRACE runtime switch */
  int fmttrace;            /* fmt runtime switch */
  int fsttrace;            /* fst FSSTRACE runtime switch */
  int multrace;            /* upd MULTRACE runtime switch */
  int cgitrace;            /* cicgi trace */
  int bugadddel;           /* 13/03/94 */

  int dbxopt_fatal;        /* dbxopen() - fatal when file doesn't exist */
  int dbxopt_errno;        /* dbxopen() - dbname/errno msg before fatal */

  int dbxopt_mflush;       /* record() - flush lastrecread if != dbname */
  int dbxopt_mclose;       /* record() - close lastrecread if != dbname */
  int dbxopt_iflush;       /* term() - flush lasttrmread if != dbname */

  LONGX rec_maxmfrl;	   /* MAXMFRL/cipar maxmfrl entry */
  LONGX rec_mstload;	   /* 28/03/97 */
  LONGX trm_invload;	   /* 28/03/97 */

  off_t dbxfloff;           /* dbxflock()/dbxulock() - offset */
  LONGX dbxflsiz;           /* dbxflock()/dbxulock() - length */
  int dbxfloop;            /* dbxflock() - #retries */
  int dbxwloop;            /* dbxwlock() - #retries */
  int dbxiloop;            /* dbxilock() - #retries */
  int dbxuclos;            /* dbxulock() - close/reopen */
  int dbxewlrc;            /* dbxflock()/dbxwlock() error action */
  int dbxsleep;            /* dbxilock() seconds to sleep */

  int dbxopt_ordwr;        /* dbxopen() - O_RDWR / O_RDONLY (LM/CD-ROM) */
  int dbxordwr;            /* dbxopen() - dbxopt_ordwr & O_RDWR */

  unsigned char bitmask[8]; /* 0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01 */

  int fpc_fd;              /* fpccreat()/fpcwrite()/fpcclose() */
  FFI fpc_left;            /* fpccreat()/fpcwrite()/fpcclose() */
  char *fpc_buffer;        /* fpccreat()/fpcwrite()/fpcclose() */

  int vlex[NTREE];              /* use it */
  int nxbsiz[NTREE];            /* use it */
  int lxbsiz[NTREE];            /* use it */

  char *mx1extp;           /* mstsetup/recisis0 */
  char *xx1extp;           /* mstsetup/recisis0 */

  char *cx1extp;           /* invsetup/trmisis0 */
  char *nx12extp[NTREE];        /* invsetup/trmisis0 */
  char *lx12extp[NTREE];        /* invsetup/trmisis0 */
  char *ix1extp;           /* invsetup/trmisis0 */
#if CNLI
  char *iy0extp;           /* All IF into one. To ALP, WL */
#endif

#if MULTI
  int cipnetws;            /* MULTI default operation */
#endif

#if DBXMSTXL
  int cipmstxl;            /* extended .mst capacity */
#endif

#if BEFORE20000323           /* because some BRM's GCC - ask rpiva */
  FILE *cistderr;          /* 20/05/98 (Asael!) */
#endif

  FILE *dbxcipfp;          /* dbxcipar() input parameter file pointer */
  char *dbxcdcip;          /* dbxcipar() input data */
  char dbxcikey[BUFSIZ];   /* dbxcipar() actual file name */
  int dbxcipok;            /* dbxcipar() found */

#if GIPAR
  char dbxgikey[BUFSIZ];   /* dbxgipar() actual file name */
  char *dbxgiext[32];      /* dbxgipar() dbn.par .extensions */
  int dbxgiexn[32];        /* dbxgipar() dbn.par numbers */
#endif

#ifndef CISISX_SOURCE

#define shtest         cisisxp->shtest
#define cnv_pcbuff     cisisxp->cnv_pcbuff
#define nodeunibuff    cisisxp->nodeunibuff
#define leafunibuff    cisisxp->leafunibuff
#define partrace       cisisxp->partrace
#define dbxtrace       cisisxp->dbxtrace
#define rectrace       cisisxp->rectrace
#define dectrace       cisisxp->dectrace
#define trmtrace       cisisxp->trmtrace
#define b40trace       cisisxp->b40trace
#define b50trace       cisisxp->b50trace
#define b70trace       cisisxp->b70trace
#define fmttrace       cisisxp->fmttrace
#define fsttrace       cisisxp->fsttrace
#define multrace       cisisxp->multrace
#define cgitrace       cisisxp->cgitrace
#define bugadddel      cisisxp->bugadddel
#define dbxopt_fatal   cisisxp->dbxopt_fatal
#define dbxopt_errno   cisisxp->dbxopt_errno
#define dbxopt_mflush  cisisxp->dbxopt_mflush
#define dbxopt_mclose  cisisxp->dbxopt_mclose
#define dbxopt_iflush  cisisxp->dbxopt_iflush
#define rec_maxmfrl    cisisxp->rec_maxmfrl
#define rec_mstload    cisisxp->rec_mstload
#define trm_invload    cisisxp->trm_invload
#define dbxfloff       cisisxp->dbxfloff
#define dbxflsiz       cisisxp->dbxflsiz
#define dbxfloop       cisisxp->dbxfloop
#define dbxwloop       cisisxp->dbxwloop
#define dbxiloop       cisisxp->dbxiloop
#define dbxuclos       cisisxp->dbxuclos
#define dbxewlrc       cisisxp->dbxewlrc
#define dbxsleep       cisisxp->dbxsleep
#define dbxopt_ordwr   cisisxp->dbxopt_ordwr
#define dbxordwr       cisisxp->dbxordwr
#define bitmask        cisisxp->bitmask
#define fpc_fd         cisisxp->fpc_fd
#define fpc_left       cisisxp->fpc_left
#define fpc_buffer     cisisxp->fpc_buffer
#define vlex           cisisxp->vlex
#define nxbsiz         cisisxp->nxbsiz
#define lxbsiz         cisisxp->lxbsiz
#define mx1extp        cisisxp->mx1extp
#define xx1extp        cisisxp->xx1extp
#define cx1extp        cisisxp->cx1extp
#define nx12extp       cisisxp->nx12extp
#define lx12extp       cisisxp->lx12extp
#define ix1extp        cisisxp->ix1extp
#define iy0extp        cisisxp->iy0extp
#define cipnetws       cisisxp->cipnetws
#define cipmstxl       cisisxp->cipmstxl
#if BEFORE20000323           /* because some BRM's GCC - ask rpiva */
#define cistderr       cisisxp->cistderr
#endif
#define dbxcipfp       cisisxp->dbxcipfp
#define dbxcdcip       cisisxp->dbxcdcip
#define dbxcikey       cisisxp->dbxcikey
#define dbxcipok       cisisxp->dbxcipok
#define dbxgikey       cisisxp->dbxgikey
#define dbxgiext       cisisxp->dbxgiext
#define dbxgiexn       cisisxp->dbxgiexn

#endif // CISISX_SOURCE

//------------------------- CIREC.C ------------------------------------

  char lastrecread[CIMPL+1];   /* 950724 */

  LONGX recxrefb;           /* recxref() comb */
  int  recxrefp;           /* recxref() comp */
  int  recxrefn;           /* recxref() flagnew */
  int  recxrefm;           /* recxref() flagmod */
  FFI  recreadl;           /* force recread() mfrl */

#if IFUPDATE
  LONGX recreadb;           /* force recread() comb = mfbwb */
  int  recreadp;           /* force recread() comp = mfbwp */
#endif

  int  recisis0_m;         /* recisis0() init .mst */
  int  recisis0_s;         /* recisis0() system file type */

#ifndef CISISX_SOURCE

#define lastrecread cisisxp->lastrecread
#define recxrefb    cisisxp->recxrefb
#define recxrefp    cisisxp->recxrefp
#define recxrefn    cisisxp->recxrefn
#define recxrefm    cisisxp->recxrefm
#define recreadl    cisisxp->recreadl
#define recreadb    cisisxp->recreadb
#define recreadp    cisisxp->recreadp
#define recisis0_m  cisisxp->recisis0_m
#define recisis0_s  cisisxp->recisis0_s

#endif // CISISX_SOURCE

//------------------------- CITRM.C ------------------------------------

  int trmifupd;            /* trm IFUPDATE operation */

  off_t lrlrseek;
  int lrlrsize;

  char lasttrmread[CIMPL+1];   /* 950724 */

#if PC
  UCHR highv[LE2+1];       /* manca controllo.. if signed */
#else
  UCHR highv[LE2+1];       /* manca controllo.. if signed */
#endif

#ifndef CISISX_SOURCE

#define trmifupd    cisisxp->trmifupd
#define lrlrseek    cisisxp->lrlrseek
#define lrlrsize    cisisxp->lrlrsize
#define lasttrmread cisisxp->lasttrmread
#define highv       cisisxp->highv

#define cntread     cisisxp->xcntread
#define invsetup    cisisxp->xinvsetup
#define noderead    cisisxp->xnoderead
#define leafread    cisisxp->xleafread

#endif // CISISX_SOURCE

//------------------------- CIIFL.C ------------------------------------

  int ciiflfix;
  int ciiflfim;                 /* key %8ld w/ ciiflfix */
  int ifl_balan;   /* Faz balanceamento da arvore */

#ifndef CISISX_SOURCE

#define ciiflfix cisisxp->ciiflfix
#define ciiflfim cisisxp->ciiflfim
#define ifl_balan cisisxp->ifl_balan

#endif // CISISX_SOURCE

//------------------------- CIUTL.C ------------------------------------

  UBYTE sfldchr;
  int loaduct0;            /* last char (zero) is not loaded */

#ifndef CISISX_SOURCE

#define sfldchr  cisisxp->sfldchr
#define loaduct0 cisisxp->loaduct0

#endif // CISISX_SOURCE

/* --------------------------------------------------------------------- */

  CISISX (unsigned char *parm_isisactab=(unsigned char *)NULL,
          unsigned char *parm_isisuctab=(unsigned char *)NULL,
          int *parm_NXPAGESp=(int *)NULL);

  ~CISISX (void);


  void xfatal(char *msg);

#if MULTI
  int xdbxcinet(char *dbnamp);
#endif

  int xdbxilock(DBXSTRU *dbxp,
                char *mdbnp,
                char *typ);

  void xdbxinit(void);

  DBXSTRU *xdbxstorp(char *dbnamp);

  DBXSTRU *xdbxsrchp(char *dbnamp);

  int xdbxopen(char *gidbnp,
               char *dbname,
               char *extp);

  int xdbxopenw(char *gidbnp,
  		char *dbname,
                char *extp,
                int  *opnp,
                int  *opwp,
                char *errmsgp);

  char *xdbxopenc(char *gidbnp,
                  char *filnamp,
                  int *opnp,
                  int *opwp,
                  char *errmsgp,
                  int xcreate = 0,
                  int xappend = 0);

  void xdbxciset(void);

  char *xdbxcipar(char *gidbnp,
                  char *argkey,
                  char argchar);

#if GIPAR
  char *xdbxgipar(char *gidbnp,
                  char *gikeyp,
                  int *namlenp,
                  int *extlenp);
#endif

#if RECGIZM
  void xgizflush(VGIZPSTRU *vgizmap);
#endif

#if RECDECO
  void xdecflush(VDECPSTRU *vdecmap);
#endif

  char *xloadfile(char *gidbnp,
	       	  char  at,
	          char *atp,
                  char *areap,
	          LONGX asize,
	          char lf2x);

  char *xloadstw(char *gidbnp,
                 char *atp,
	         char *areap,
	         LONGX  asize,
	         int  *nstws);

  char *xdbxtmpnm(char *dirtmp,
                  int strip,
                  char *filnamp);

  int xrecisis0(char *dbnamp);

#if IFUPDATE
 void xrecreset(char *dbnp,
		LONGX  mfn1,
		LONGX  mfn2,
		RECSTRU *xrecp,
		LONGX  parmtell);
#endif


  int     xcntread(DBXSTRU *dbxp,
		   int *ifpopen,
		   char *ifpname,
		   int *firstcnli);

  void    xinvsetup(UCHR *dbnamp,
		    LONGX  loadn0x,
		    LONGX  loadl0x,
		    LONGX  loadpst);

  N0STRU *xnoderead(INVMAP *invp,
		    int     treecase,
		    int     level,
		    PUNT    punt);

  L0STRU *xleafread(UCHR   *lbufp,
		    INVMAP *invp,
		    int     treecase,
		    PUNT    punt,
		    int     lxx);

#if !LIND
  IFPSTRU *xpostread(UCHR   *xbufp,
		     INVMAP *invp,
		     PUNT    xblk,
		     int     lxx);
#else
  IFPSTRU *xpostread(UCHR   *xbufp,
		     INVMAP *invp,
		     off_t   addr,
		     unsigned int toread,
		     int     lxx);
#endif


  ISO_2709 * getISO_2709(void);
  DBXSTRU  * getDBXSTRU(char *dbnamep);

#ifndef CISISX_SOURCE
#define dbxcinet                cisisxp->xdbxcinet        // external use
#define dbxilock                cisisxp->xdbxilock        // external use
#define dbxinit                 cisisxp->xdbxinit         // external use
#define dbxstorp                cisisxp->xdbxstorp        // external use
#define dbxsrchp                cisisxp->xdbxsrchp        // external use
#define dbxopen                 cisisxp->xdbxopen         // external use
#define dbxopenw                cisisxp->xdbxopenw        // external use
#define dbxopenc                cisisxp->xdbxopenc        // external use

#define fatal                   cisisxp->xfatal           // external use

#define dbxciset                cisisxp->xdbxciset        // external use
#define dbxcipar                cisisxp->xdbxcipar        // external use
#define dbxgipar                cisisxp->xdbxgipar        // external use
#define gizflush                cisisxp->xgizflush        // external use
#define decflush                cisisxp->xdecflush        // external use
#define loadfile                cisisxp->xloadfile        // external use
#define loadstw                 cisisxp->xloadstw         // external use
#define dbxtmpnm                cisisxp->xdbxtmpnm        // external use
#define recisis0                cisisxp->xrecisis0        // external use
#define recreset                cisisxp->xrecreset        // external use
#define postread                cisisxp->xpostread        // external use

#endif // CISISX_SOURCE
};


#endif // CICPP

#endif  // CISISX_HPP

