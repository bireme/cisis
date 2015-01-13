#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "cisis.h"

#if PC
#if !MSC
#include <dir.h>
#endif /* !MSC */
#endif

#if MSC
#include <share.h>
#include <sys\locking.h>
#endif

#if CICPP
#define CIDBX_SOURCE
#include <cidbx.hpp>
#include <cirun.hpp>

#ifdef USE_ERROR_SYS
void ISIS_OemToAnsi(char *in, char *out);
#endif

/* ------------------------- CIDBX.HPP --------------------------------- */
#if MULTI
/*#define dbxcinet  xdbxcinet    moved to CISISX */
#define dbxflock  xdbxflock   /* internal use */
#define dbxulock  xdbxulock   /* internal use */
/*#define dbxilock  xdbxilock    moved to CISISX */
#define dbxwlock  xdbxwlock   /* internal use */
#endif
/*#define dbxinit   xdbxinit     moved to CISISX */
/*#define dbxstorp  xdbxstorp    moved to CISISX */
/*#define dbxsrchp  xdbxsrchp    moved to CISISX */
/*#define dbxopen   xdbxopen     moved to CISISX */
/*#define dbxopenw  xdbxopenw    moved to CISISX */
/*#define dbxopenc  xdbxopenc    moved to CISISX */
/*#define fatal     xfatal       moved to CISISX */
#define dbxflush  xdbxflush   /* internal use */
#define mstclose  xmstclose   /* internal use */
#define mstflush  xmstflush   /* internal use */
/*#if RECGIZM */
/*#define gizflush  xgizflush    moved to CISISX */
/*#endif*/
/*#if RECDECO */
/*#define decflush  xdecflush    moved to CISISX */
/*#endif */
#define dbxinvmp  xdbxinvmp   /* internal use */
#define invclose  xinvclose   /* internal use */
#define invflush  xinvflush   /* internal use */
/*#define loadfile  xloadfile    moved to CISISX */
/*#define dbxciset  xdbxciset    moved to CISISX */
/*#define dbxcipar  xdbxcipar    moved to CISISX */
#if GIPAR
/*#define dbxgipar  xdbxgipar    moved to CISISX */
#endif
#define fpccreat  xfpccreat   /* internal use */
#define fpcwrite  xfpcwrite   /* internal use */
#define fpcclose  xfpcclose   /* internal use */
/*#define dbxtmpnm  xdbxtmpnm    moved to CISISX */
#define cicopyr   xcicopyr    /* internal use */
#if GEN_CORELEFT
#define coreleft  xcoreleft   /* internal use */
#endif
#if GEN_LABS
#define labs      DBXSTRU::xlabs       /* internal use */
#endif
#if GEN_STRSTR
#define strstr    DBXSTRU::xstrstr     /* internal use */
#endif
#if GEN_STRUPR
#define strupr    DBXSTRU::xstrupr     /* internal use */
#endif
#if GEN_STRREV
#define strrev    DBXSTRU::xstrrev     /* internal use */
#endif
#if GEN_MEMICMP
#define memicmp   DBXSTRU::xmemicmp    /* internal use */
#endif
/* --------------------------------------------------------------------- */
#endif /* CICPP */


#ifdef USE_ERROR_SYS	/* Peter Diry error window (compiler environment) */
#include <ui_win.hpp>
#include <errorsys.hpp>
extern MY_ERROR_SYSTEM * errsys;
#endif

/* ----------------------------- dbx.c ------------------------------ */
#define DBXTRACw    0
#define DBXTRAC0    0
#define DBXTRAC1    0
#define DBXTRAC2    0
#define CNVTRACE    0

#define DBXTRACE    1           /* dbxtrace */
#define DBGTRACE    1           /* dbxtrace */

#define SHTEST      0

#if !CICPP
#if SHTEST
static char shtest[CIMPL+1];
#endif /* SHTEST */

#if CNV_PCBINUM
char cnv_pcbuff[MAXMFRL];	/* PC data representation (swapped) */
#endif

#if CNV_PCFILES
UCHR nodeunibuff[N2BSIZ];       /* area para ler pagina de .n01/.n02 */
#endif
#if LEAFCNV_PCFILES
UCHR leafunibuff[L2BSIZ];       /* area para ler pagina de .l01/.l02 */
#endif

#endif /* !CICPP */

#if !CICPP
/* global */
int partrace=0;                 /* dbxopen/dbxcipar trace */
int dbxtrace=0;                 /* dbxopen/dbxcipar trace */
int rectrace=0;                 /* rec RESTRACE runtime switch */
int dectrace=0;                 /* decoread()/recdeco() runtime switch  */
int trmtrace=0;                 /* trm TRSTRACE runtime switch */
int b40trace=0;                 /* b40 RUXTRACE runtime switch */
int b50trace=0;                 /* b50 RUXTRACE runtime switch */
int b70trace=0;                 /* b70 RUXTRACE runtime switch */
int fmttrace=0;                 /* fmt runtime switch */
int fsttrace=0;                 /* fst FSSTRACE runtime switch */
int multrace=0;                 /* upd MULTRACE runtime switch */
int cgitrace=0;                 /* cicgi trace */
int bugadddel=1;                /* 13/03/94 */

int dbxopt_fatal = 1;           /* dbxopen() - fatal when file doesn't exist */
int dbxopt_errno = 1;           /* dbxopen() - dbname/errno msg before fatal */

int dbxopt_mflush = 0;          /* record() - flush lastrecread if != dbname */
int dbxopt_mclose = 0;          /* record() - close lastrecread if != dbname */
int dbxopt_iflush = 0;          /* term() - flush lasttrmread if != dbname */

LONGX rec_maxmfrl=MAXMFRL;	/* 25/02/97 */
LONGX rec_mstload=0L;		/* 28/03/97 */
LONGX trm_invload=0L;		/* 28/03/97 */

LONGX fmt_fsiz=MAXMFRL;               /* fmt_inter() - to set max field length */

#if MULTI
#if BEFOREISIS301
off_t dbxfloff=0L;		/* dbxflock()/dbxulock() - offset */
LONGX dbxflsiz=MSBSIZ;		/* dbxflock()/dbxulock() - length */
#else
off_t dbxfloff=MSBSIZ;		/* dbxflock()/dbxulock() - offset = blk #2 */
LONGX dbxflsiz=MSBSIZ;		/* dbxflock()/dbxulock() - length */
#endif
#if BEFORE990317
int dbxfloop = INT_MAX;         /* dbxflock() - #retries */
int dbxwloop = INT_MAX;         /* dbxwlock() - #retries */
int dbxiloop = INT_MAX;         /* dbxilock() - #retries */
#else
int dbxfloop = SHRT_MAX;         /* dbxflock() - #retries */
int dbxwloop = SHRT_MAX;         /* dbxwlock() - #retries */
int dbxiloop = SHRT_MAX;         /* dbxilock() - #retries */
#endif
int dbxuclos = 1;               /* dbxulock() - close/reopen */
int dbxewlrc = 0;               /* dbxflock()/dbxwlock() error action */
int dbxsleep = 0;               /* dbxilock() seconds to sleep */
#endif /* MULTI */

#if MULTI
int dbxopt_ordwr = O_RDONLY;    /* dbxopen() */
#else
int dbxopt_ordwr = O_RDONLY;    /* dbxopen() - O_RDWR / O_RDONLY */
#endif
int dbxordwr = 0;               /* dbxopen() - dbxopt_ordwr & O_RDWR */

unsigned char bitmask[8] = { 0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01 };

int fpc_fd=0;                   /* fpccreat()/fpcwrite()/fpcclose() */
FFI fpc_left=FPCBSIZ;           /* fpccreat()/fpcwrite()/fpcclose() */
char *fpc_buffer=NULL;          /* fpccreat()/fpcwrite()/fpcclose() */

int vlex[NTREE] = { LE1, LE2 };                 /* use it */
int nxbsiz[NTREE] = { N1BSIZ, N2BSIZ };         /* use it */
int lxbsiz[NTREE] = { L1BSIZ, L2BSIZ };         /* use it */

char *mx1extp = ".mst";                         /* mstsetup/recisis0 */
char *xx1extp = ".xrf";                         /* mstsetup/recisis0 */

char *cx1extp = ".cnt";                         /* invsetup/trmisis0 */
char *nx12extp[NTREE] = { ".n01", ".n02" };     /* invsetup/trmisis0 */
#if LIND
char *lx12extp[NTREE] = { ".ly1", ".ly2" };     /* invsetup/trmisis0 */
char *ix1extp = ".iyp";                         /* invsetup/trmisis0 */
#else
char *lx12extp[NTREE] = { ".l01", ".l02" };     /* invsetup/trmisis0 */
char *ix1extp = ".ifp";                         /* invsetup/trmisis0 */
#endif
#if CNLI
char *iy0extp = ".iy0";                         /* All IF into 1. To ALP, WL */
#endif

#if MULTI
int cipnetws=MONONETS;          /* MULTI default operation */
#endif

#if DBXMSTXL
#if ( LIND4 || SUPERISIS==1 ) && _LARGEFILE64_SOURCE
int cipmstxl=6;                 /* extended .mst capacity */
#elif CIFFI
int cipmstxl=4;                 /* extended .mst capacity */
#else
int cipmstxl=0;                 /* extended .mst capacity */
#endif
#endif

#if BEFORE20000323           /* because some BRM's GCC - ask rpiva */
FILE *cistderr=stderr;      /* 20/05/98 (Asael!) */
#else
#define cistderr stderr
#endif

/* ----------------------------- par.c ------------------------------ */
/* global */
FILE *dbxcipfp = NULL;          /* dbxcipar() input parameter file pointer */
char *dbxcdcip = NULL;          /* dbxcipar() input data */
char dbxcikey[BUFSIZ];          /* dbxcipar() actual file name */
int dbxcipok = 0;               /* dbxcipar() found */

#if GIPAR
char dbxgikey[BUFSIZ];         /* dbxgipar() actual file name */

char *dbxgiext[] = {
                     ".xrf",".mst",
                     ".cnt",".n01",".n02",
#if LIND
		     ".ly1",".ly2",".iyp",
#else /* LIND */
		     ".l01",".l02",".ifp",
#endif /* LIND */
                     /*
                     xx1extp, mx1extp,
                     cx1extp,
                     nx12extp[0], nx12extp[1],
                     lx12extp[0], lx12extp[1],
                     ix1extp,
                     */
		     ".any",
                     ".fdt",".fst",".fmt",".pft",".stw",".srt",".wpr",".val",
                     /*
                     iy0extp,
                     */
                     NULL }; /* bc4.5/C++ */
int   dbxgiexn[] = { 1,2,
                     3,
                     4,5,
                     6,7,
                     8,
		     9,
		     10,10,10,10,10,10,10,10
                     /*
                     ,11
                     */ };

#endif /* GIPAR */

#endif /* !CICPP */

#if !CICPP
#define CIDBX_CISISX_SOURCE 1
#include "cisisx.c"
#endif

#if CICPP
DBXSTRU :: DBXSTRU(CISISX *parm_cisisxp)
{
 memset(this,0x00,sizeof(DBXSTRU));
 cisisxp = parm_cisisxp;
 //try {
 //}
 //catch {
 //}
 //strncpy(dbxname,dbnamep,*highv);
}

#endif /* CICPP */


#if MULTI

#if CICPP
int DBXSTRU :: xdbxflock(DBXSTRU *dbxp,
                         char *typ)
#else /* CICPP */
int dbxflock(dbxp,typ)
DBXSTRU *dbxp;
char *typ;
#endif /* CICPP */
{
    int floop;					/* dbxflock() - #retries */
    int rc,fd=0,type=0;
    int opw,*opvp;

    if (!dbxp) fatal("dbxflock/dbxp");
    if (dbxtrace) printf("+++ dbxflock - %s / %s \n",DBXname,typ);

    if (strcmp(typ,"M") == 0) {
	type=1; fd=DBXmsopn; opw=DBXmsopw; opvp= &DBXmsopv;
    }
    if (strcmp(typ,"I") == 0) {
	type=2; fd=DBIifopn; opw=DBIifopw; opvp= &DBIifopv;
    }
    if (!type) fatal("dbxflock/type");

    if (fd <= 0) fatal("dbxflock/fd");
    if (!opw)   fatal("dbxflock/file is not opened for write");
    if (*opvp) fatal("dbxflock/file is locked");

    for (floop=dbxfloop; floop--; ) {		/* dbxflock() - #retries */
#if PC
#if MSC
	if (LSEEK64(fd,dbxfloff,SEEK_SET) != dbxfloff) fatal("dbxflock/lseek");
	rc=locking(fd,LK_LOCK,dbxflsiz);
#else
	rc=lock(fd,dbxfloff,dbxflsiz);
#endif
#else
	if (LSEEK64(fd,dbxfloff,SEEK_SET) != dbxfloff) fatal("dbxflock/lseek");
	rc=lockf(fd,F_TLOCK,dbxflsiz);
#endif
	if (rc == 0) { (*opvp)++; /* locked */   return(0); }

	if (floop == 0) {
	    if (multrace)
	        printf("<F> File %s (%s) is locked by another user\n",DBXname,typ);
#if SHTEST
	    printf(".."); if (*gets(shtest)) if (strcmp(shtest,"x")) system(shtest); else fatal("dbxflock/shtest");
#endif
	    return(RCLOCK);
	}
    }
    return(rc);
}


#if CICPP
int DBXSTRU :: xdbxulock(DBXSTRU *dbxp,
                         char *typ)
#else /* CICPP */
int dbxulock(dbxp,typ)
DBXSTRU *dbxp;
char *typ;
#endif /* CICPP */
{
    int rc,fd,type=0;
    char *extp;
    int *opnp,*opwp,*opvp;

    if (!dbxp) fatal("dbxulock/dbxp");
    if (dbxtrace) printf("+++ dbxulock - %s / %s \n",DBXname,typ);

    if (strcmp(typ,"M") == 0) {
	type=1;
	opnp= &DBXmsopn; extp=mx1extp; opwp= &DBXmsopw; opvp= &DBXmsopv;
    }
    if (strcmp(typ,"I") == 0) {
	type=2;
	opnp= &DBIifopn; extp=ix1extp; opwp= &DBIifopw; opvp= &DBIifopv;
    }
    if (!type) fatal("dbxulock/type");
    fd= *opnp;

    if (dbxtrace)
     printf("+++ dbxulock - fd=%d ext=%s opw=%d opv=%d\n",fd,extp,*opwp,*opvp);

    if (fd <= 0) fatal("dbxulock/fd");
    if (!*opwp) fatal("dbxulock/file is not opened for write");
    if (!*opvp) fatal("dbxulock/file is not locked");

#if PC
#if MSC
    if (LSEEK64(fd,dbxfloff,SEEK_SET) != dbxfloff) fatal("dbxulock/lseek");
    rc=locking(fd,LK_UNLCK,dbxflsiz);
#else
    rc=unlock(fd,dbxfloff,dbxflsiz);
#endif
#else
    if (LSEEK64(fd,dbxfloff,SEEK_SET) != dbxfloff) fatal("dbxulock/lseek");
    rc=lockf(fd,F_ULOCK,dbxflsiz);
#endif
    *opvp=0; /* unlocked */
    if (dbxuclos < 0) {
	if (dbxtrace) printf("dbxulock - will close %d \n",fd);
	CLOSE(fd); *opnp=0; *opwp=0;
    }
    else {
	if (dbxtrace) printf("dbxulock - will close/open %d \n",fd);
	*opwp=0; /* force close&open/w in dbxopenw */
	dbxopenw(DBXname,DBXname,extp,opnp,opwp,"dbxulock/reopn/w");
    }
    return(rc);
}

#if CICPP
int DBXSTRU :: xdbxwlock(DBXSTRU *dbxp,
                         char *m0p,
			 int   times)
#else /* CICPP */
int dbxwlock(dbxp,m0p,times)
DBXSTRU *dbxp;
char *m0p;
int times;
#endif /* CICPP */
{
    char m0area[sizeof(M0STRU)];
    int wloop,n;

    if (dbxtrace)
        printf("+++ dbxwlock - %s %p %d (%d)\n",DBXname,m0p,times,DBXewlxx);

    if (!m0p) m0p=m0area;
    for (wloop=times; wloop--; ) {
	/* get the control record */
	if (LSEEK64(DBXmsopn,0L,SEEK_SET) != 0) fatal("dbxwlock/lock/LSEEK64/ewl");
	n=CIREAD(DBXmsopn,m0p,sizeof(M0STRU));
#if CNV_PCBINUM
	ConvertMST_CTLSTRUCT(m0p);
#endif
	if (n != sizeof(M0STRU)) fatal("dbxwlock/lock/read/ewl");
#if DBXMSTXL /* AOT 18/06/2002 */
	((M0STRU *)m0p)->m0mftype = ((M0STRU *)m0p)->m0mftype & 0x00FF;
#endif
	if (((M0STRU *)m0p)->m0mfcxx3 == 0) break;
	if (multrace) printf("<W> %s has exclusive write lock\n",DBXname);
	if (DBXewlxx) break; /* user's ewl */
	/* .mst file lock: unlock */
	if (multrace) printf("<W> %s .mst unlock/ewl \n",DBXname);
	if (dbxulock(dbxp,"M")) fatal("dbxwlock/lock/file unlock/ewl");
	if (wloop == 0) {
	    if (multrace)
	        printf("<W> Database %s is locked by another user\n",DBXname);
#if SHTEST
	    printf(".."); if (*gets(shtest)) if (strcmp(shtest,"x")) system(shtest); else fatal("dbxwlock/shtest");
#endif
	    return(RCLOCK);
	}
	/* .mst file lock: lock */
	if (multrace) printf("<W> %s .mst lock/ewl \n",DBXname);
	if (dbxflock(dbxp,"M"))
#if BEFORE20000608
            fatal("dbxwlock/lock/file lock/ewl");
#else
	    return(RCLOCK);
#endif
    }

    return(0);
}

#endif /* MULTI */

#if CICPP
void DBXSTRU :: xdbxflush(char *dbnamp)
#else /* CICPP */
void dbxflush(dbnamp)                                                   /*
-------------
		    seta dbxp;
		    flush master e invertido;
		    libera dbxp;
		    reloca vdbxp[]
									*/
char *dbnamp;                   /* dbn to be flushed */
#endif /* CICPP */
{
    DBXSTRU *dbxp;
    LONGX idbx;
#if DBXTRACE
    char dbname[sizeof(dbxp->dbxname)];
#endif

    dbxp=dbxsrchp(dbnamp);
    if (dbxp == NULL)
	return;

    mstflush(dbnamp);
    invflush(dbnamp);

    for (idbx=ndbxs; idbx--; )
	if (strcmp(VDBXname(idbx),dbnamp) == 0) {
#if DBXTRACE
 if (dbxtrace) strcpy(dbname,dbnamp);
#endif

#if GIPAR
	    if (DBXgicip) {
#if CICPP
		delete [] DBXgicip;
#else /* CICPP */
		FREE(DBXgicip);
#endif /* CICPP */
	    }
#endif /* GIPAR */

#if BEFORE981202
#if CICPP
	    delete vdbxp[idbx];
#else /* CICPP */
	    FREE(vdbxp[idbx]);
#endif /* CICPP */
	    vdbxp[idbx]=(DBXSTRU *)NULL;
	    while (++idbx < ndbxs) vdbxp[idbx-1] = vdbxp[idbx];
	    ndbxs--;
#else /* BEFORE */
	    /* Agora mantem estrutura - apenas reinicializa */
#if CICPP
            CISISX *cp = dbxp->cisisxp;
            memset(dbxp,0x00,sizeof(DBXSTRU)); /* init everything */
            dbxp->cisisxp = cp;
#else /* CICPP */
            memset(dbxp,0x00,sizeof(DBXSTRU)); /* init everything */
#endif /* CICPP */
            strcpy(DBXname,dbnamp);            /* store dbnamp */
	    /* HB/ISIS_DLL precisa de ndbxs--; para nao cair */
	    /* rever dbxgipar: chamar dbxsrchp e nao dbxstorp */
#endif /* BEFORE */

#if DBXTRACE
 if (dbxtrace) printf("dbxflush - dbnamp='%s' \n",dbname);
#endif
#if DBXTRAC2
 for (idbx=0L; idbx < ndbxs; idbx++)
  printf("dbxflush - ndbxs=%"_LD_"  idbx=%"_LD_"=%s=%p \n",
   ndbxs,idbx,VDBXname(idbx),vdbxp[idbx]);
#endif
	    return;
	}

    fatal("dbxflush");
}

#if CLOSEX
int CLOSE(fd)
int fd;
{
#define XTC 0
#if XTC
    unsigned int eof;
#endif /* CICPP */
#if 0
int n;
for (n=0; n<HANDLE_MAX; n++)
 printf("openfd[%d] = %04x\n",n,_openfd[n]);
#endif
    if (dbxtrace) printf("+++ CLOSE/closing %d\n",fd);

#if XTC
    if (fd < 0 || fd >= HANDLE_MAX) fatal("CLOSE/fd");
    eof=(unsigned int)EOF;
    if (_openfd[fd] == eof) fatal("CLOSE/openfd");
#endif
    return(CLOSEX(fd));
}
#endif

/* ----------------------------- std.c ------------------------------ */


/* ----------------------------- flush.c ------------------------------ */

#if CICPP
void DBXSTRU :: xmstclose(DBXSTRU *dbxp)
#else /* CICPP */
void mstclose(dbxp)                                                     /*
-------------
		    faz close dos arquivos abertos do master file
									*/
DBXSTRU *dbxp;
#endif /* CICPP */
{
    if (dbxp == NULL) return;

    if (DBXmsopn) {
	CLOSE(DBXmsopn); DBXmsopn=DBXmsopw=DBXmsopv=0;
    }
    if (DBXxropn) {
	CLOSE(DBXxropn); DBXxropn=DBXxropw=0;
    }
}


#if CICPP
void DBXSTRU :: xmstflush(char *dbnamp)
#else /* CICPP */
void mstflush(dbnamp)                                                   /*
-------------
		    seta dbxp;
		    faz close dos arquivos abertos do master file
		    flush gizmo,etc;
									*/
char *dbnamp;                   /* dbn to be flushed */
#endif /* CICPP */
{
    DBXSTRU *dbxp;

    dbxp=dbxsrchp(dbnamp);
    if (dbxp == NULL) return;

#if RECGIZM
    if (DBXvgzrp) { gizflush(DBXvgzrp); DBXvgzrp=NULL; }
#endif
#if RECDECO
    if (DBXvderp) { decflush(DBXvderp); DBXvderp=NULL; }
#endif

    if (DBXxribp != NULL) {
#if CICPP
	delete [] DBXxribp;
#else /* CICPP */
	FREE(DBXxribp);
#endif /* CICPP */
	DBXxribp=NULL;
    }

    if (DBXmsibp != NULL) {
#if CICPP
	delete [] DBXmsibp;
#else /* CICPP */
	FREE(DBXmsibp);
#endif /* CICPP */
	DBXmsibp=NULL;
    }

    if (DBXxryyp != NULL) {
#if CICPP
	delete [] DBXxryyp;
#else /* CICPP */
	FREE(DBXxryyp);
#endif /* CICPP */
	DBXxryyp=NULL;
    }

    if (DBXmsyyp != NULL) {
#if CICPP
	delete [] DBXmsyyp;
#else /* CICPP */
	FREE(DBXmsyyp);
#endif /* CICPP */
	DBXmsyyp=NULL;
    }

    mstclose(dbxp);
}



#if CICPP
INVMAP * DBXSTRU :: xdbxinvmp(DBXSTRU *dbxp)
#else /* CICPP */
INVMAP *dbxinvmp(dbxp)                                                  /*
-----------------
		    aloca/seta dbxifmap
									*/
DBXSTRU *dbxp;
#endif /* CICPP */
{
    INVMAP *invp;
#if !CICPP
    LONGX lvar;
#endif /* CICPP */

    if (!DBXifmap) { /* funtion */
#if CICPP
	try { invp=(INVMAP *) new char [(sizeof(INVMAP))]; }
	catch (BAD_ALLOC) { invp = (INVMAP *)ALLONULL; }
#else /* CICPP */
	lvar=sizeof(INVMAP);
	if ((lvar) > ALLOMAXV)
	    fatal("dbxinvmp/ALLOMAXV/sizeof(INVMAP)");
	invp=(INVMAP *)ALLOC((ALLOPARM)(sizeof(INVMAP)));
#endif /* CICPP */
	if (invp == (INVMAP *)ALLONULL) fatal("dbxinvmp/ALLOC/invmap");
	memset(invp,0x00,sizeof(INVMAP));
	DBXifmap=invp;
    }
    if (dbxtrace) printf("+++ dbxinvmp - %s = %p\n",DBXname,DBXifmap);

    return(DBXifmap);
}

#if CICPP
void DBXSTRU :: xinvclose(DBXSTRU *dbxp)
#else /* CICPP */
void invclose(dbxp)                                                     /*
-------------
		    faz close dos arquivos abertos do inverted file
									*/
DBXSTRU *dbxp;
#endif /* CICPP */
{
    INVMAP *invp;
#if SAMEL
    int lxx;
#endif

    if (dbxp == NULL) return;

    if (DBXifmap) {
	invp=DBXifmap;
#if SAMEL
	for (lxx=0; lxx < MAXSAMEL; )
	    if (invp->ifopn[lxx]) CLOSE(invp->ifopn[lxx]);
#else
	if (invp->ifopn) CLOSE(invp->ifopn); /* 1st */
#endif
	if (invp->cnopn) CLOSE(invp->cnopn);
	if (invp->n1opn) CLOSE(invp->n1opn);
	if (invp->n2opn) CLOSE(invp->n2opn);
	if (invp->l1opn) CLOSE(invp->l1opn);
	if (invp->l2opn) CLOSE(invp->l2opn);

	invp->cnopn=invp->cnopw=0;
	invp->n1opn=invp->n1opw=0;
	invp->n2opn=invp->n2opw=0;
	invp->l1opn=invp->l1opw=0;
	invp->l2opn=invp->l2opw=0;
	invp->ifopn=invp->ifopw=0;
    }
}

#if CICPP
void DBXSTRU :: xinvflush(char *dbnamp)
#else /* CICPP */
void invflush(dbnamp)                                                   /*
-------------
		    seta dbxp;
		    faz close dos arquivos abertos do inverted file
									*/
char *dbnamp;                   /* dbn to be flushed */
#endif /* CICPP */
{
    DBXSTRU *dbxp;
    INVMAP *invp;
    int treecase;
    NXSTRU *nxp;
    int level;

    dbxp=dbxsrchp(dbnamp);
    if (dbxp == NULL) return;

    if (DBXifmap) {

	invclose(dbxp);
	invp=DBXifmap;

	for (treecase=0; treecase<2; treecase++) {
	    for (level=invp->cn[treecase].liv; level>=0; level--) {
		if ((nxp= &invp->nx[treecase][level]) != NULL)
#if CICPP
		    delete [] nxp->basep;
#else /* CICPP */
		    FREE(nxp->basep);
#endif /* CICPP */
	    }
	}
	for (treecase=0; treecase<2; treecase++) {
	    if (invp->nybasep[treecase] != NULL)
#if CICPP
		delete [] invp->nybasep[treecase];
#else /* CICPP */
		FREE(invp->nybasep[treecase]);
#endif /* CICPP */
	}
	for (treecase=0; treecase<2; treecase++) {
	    if (invp->lybasep[treecase] != NULL)
#if CICPP
		delete [] invp->lybasep[treecase];
#else /* CICPP */
		FREE(invp->lybasep[treecase]);
#endif /* CICPP */
	}
	if (invp->iybasep != NULL)
#if CICPP
	    delete [] invp->iybasep;
#else /* CICPP */
	    FREE(invp->iybasep);
#endif /* CICPP */

	if (invp->ifl1p != NULL)
#if CICPP
	    delete [] invp->ifl1p;
#else /* CICPP */
	    FREE(invp->ifl1p);
#endif /* CICPP */
	if (invp->ifl2p != NULL)
#if CICPP
	    delete [] invp->ifl2p;
#else /* CICPP */
	    FREE(invp->ifl2p);
#endif /* CICPP */

#if CICPP
	delete [] invp;
#else /* CICPP */
	FREE(invp);
#endif /* CICPP */
	DBXifmap=NULL;
    }
}


/* ***************** Standard Library extensions **************** */

#if GEN_CORELEFT
#if CICPP
unsigned LONGX DBXSTRU :: xcoreleft(void)
#else /* CICPP */
unsigned LONGX coreleft()
#endif /* CICPP */
{
#if UNIX && !GCC
    LONGX bytes;
    bytes=ulimit(3,0L)-(LONGX)sbrk(0);
    if (dbxtrace) printf("+++ coreleft=%"_LD_"\n",bytes);
    return((unsigned LONGX)bytes);
#else
    return(ALLOMAXV);
#endif
}
#endif /* GEN_CORELEFT */


#if GEN_LABS
#if CICPP
long DBXSTRU :: xlabs(long x)
#else /* CICPP */
long labs(x)
long x;
#endif /* CICPP */
{
    if (x < 0) return(-x); else return(x);
}
#endif /* GEN_LABS */


#if GEN_STRSTR
#if CICPP
char * DBXSTRU :: xstrstr (CONST char *p1,
			   CONST char *p2)
#else /* CICPP */
char *strstr(p1, p2)
CONST char *p1;
CONST char *p2;
#endif /* CICPP */
{
    char *p3, *p4;
    do {
	for ( ; *p1 != *p2; p1++) if (*p1 == '\0') return (NULL);
	for (p3 = p1, p4 = p2; ; p3++, p4++) {
	    if (*p4 == '\0') return (p1);
	    if (*p3 != *p4) break;
	}
	p1++;
    } while (*p1 != '\0');
    return (NULL);
}
#endif /* GEN_STRSTR */


#if GEN_STRUPR
#if CICPP
char * DBXSTRU :: xstrupr (char* p)
#else /* CICPP */
char *strupr(p)
char *p;
#endif /* CICPP */
{
    char *p1;
    for (p1 = p; *p1; p1++) *p1 = toupper (*p1);
    return (p);
}
#endif /* GEN_STRUPR */


#if GEN_STRREV
#if CICPP
char * DBXSTRU :: xstrrev (char *p)
#else /* CICPP */
char *strrev(p)
char *p;
#endif /* CICPP */
{
    char *p1, *p2, c;
    int lp;
    lp = strlen(p);
    p2 = p+lp-1;
    lp = lp/2;
    for (p1 = p; lp; lp--, p1++, p2--) {
	c = *p1;
	*p1 = *p2;
	*p2 = c;
    }
    return (p);
}
#endif /* GEN_STRREV */


#if GEN_MEMICMP
#if CICPP
int DBXSTRU :: xmemicmp(CONST void *s1,
			CONST void *s2,
			size_t      n)
#else /* CICPP */
int memicmp(s1, s2, n)
CONST void *s1;
CONST void *s2;
size_t n;
#endif /* CICPP */
{
    size_t loop;
    int rc;
    unsigned char *p1,*p2;
    for (rc=0, p1=(unsigned char *)s1, p2=(unsigned char *)s2, loop=n; loop--; p1++, p2++) {
	rc=(int)*p1-(int)*p2;
	if (rc) break;
    }
    return(rc);
}
#endif /* GEN_MEMICMP */



/* *********************** fpc_ extension ******************** */
#if CICPP
void DBXSTRU :: xfpcwrite(char *recbufp,
			  FFI   reclen)
#else /* CICPP */
void fpcwrite(recbufp,reclen)
char *recbufp;
FFI reclen;
#endif /* CICPP */
{
    char *p;

#if fpc_TRACE
    FFI loop;
    for (p=recbufp, loop=0; loop<reclen; p++) {
	/* printf("%02x ",*p); */
	printf("%02x%c ",*p,isprint(*p)?*p:' ');
	if (++loop%16 == 0) printf("\n");
    }
    printf("\n");
#endif

    /* allocate fpcwrite() output buffer */
    if (!fpc_buffer) {
#if CICPP
	try { fpc_buffer= new char [FPCBSIZ]; }
	catch (BAD_ALLOC) { fpc_buffer=(char *)NULL; }
#else /* CICPP */
	fpc_buffer=(char *)ALLOC((ALLOPARM)FPCBSIZ);
#endif /* CICPP */
	if (fpc_buffer == (char *)ALLONULL)
		fatal("fpcwrite/ALLOC/fpc_buffer");
#if fpc_TRACE
	if (dbxtrace) printf("fpc_buffer: coreleft=%"_LD_"\n",CORELEFT());
#endif
    }

    if (!fpc_fd) fatal("fpcwrite/fd");
    for (p=recbufp; reclen; ) {
	if (reclen <= fpc_left) {
	    memmove(&fpc_buffer[FPCBSIZ-fpc_left],p,(size_t)reclen);
	    fpc_left-=reclen;
	    break;
	}
	memmove(&fpc_buffer[FPCBSIZ-fpc_left],p,fpc_left);
	if (CIWRITE(fpc_fd,fpc_buffer,FPCBSIZ) != FPCBSIZ)
	    fatal("fpcwrite/write");
	reclen-=fpc_left;
	p+=fpc_left;
	fpc_left=FPCBSIZ;
    }
}


#if CICPP
void DBXSTRU :: xfpcclose(void)
#else /* CICPP */
void fpcclose()
#endif /* CICPP */
{
    FFI n;

    if (fpc_left < FPCBSIZ) {
	if (/* fpc_left < 0 || */ fpc_left > FPCBSIZ) fatal("fpcclose/left");
	if (!fpc_fd) fatal("fpcclose/fd");
	if (!fpc_buffer) fatal("fpcclose/buffer");
	n=FPCBSIZ-fpc_left;
	if (CIWRITE(fpc_fd,fpc_buffer,n) != n) fatal("fpcclose/write");
    }
    fpc_left=FPCBSIZ;

    CLOSE(fpc_fd); fpc_fd=0;

    if (fpc_buffer) {
#if CICPP
	delete [] fpc_buffer;
#else /* CICPP */
	FREE(fpc_buffer);
#endif /* CICPP */
	fpc_buffer=NULL;
    }
}


#if CICPP
int DBXSTRU :: xfpccreat(char *gidbnp,
                         char *namp,
			 char *extp,
			 int   lrecl)
#else /* CICPP */
int fpccreat(gidbnp,namp,extp,lrecl)
char *gidbnp;			/* dbn.par */
char *namp;
char *extp;
int lrecl;
#endif /* CICPP */
{
    char line[BUFSIZ];
#if MPE
    char dcb[80];
#endif

    strcpy(line,namp);
    strcat(line,extp);
    strcpy(line,dbxcipar(gidbnp,line,'='));

#if MPE
    sprintf(dcb,"b R%d",lrecl);
    fpc_fd=OPEN(line,O_WRONLY|O_CREAT|O_TRUNC|O_MPEOPTS,PERMIS,dcb);
#else
    fpc_fd=CREAT(line,PERMIS);
    if (lrecl) fatal("fpccreat/lrecl");
#endif
    if (fpc_fd <= 0) fatal(line);
    if (dbxtrace) printf("+++ %s=%d\n",line,fpc_fd);

    return(fpc_fd);
}

#if CICPP
char * DBXSTRU :: xcicopyr(char *namep)
#else /* CICPP */
char *cicopyr(namep)
char *namep;
#endif /* CICPP */
{
    //static char cicopyr_area[3*100+9];
    static char cicopyr_area[5*100+9];
    //char *p,*q;
    char *p,*q,*q2,*q3;
    char vci[100],vwi[100],*vp;

    vci[0]='\0';
    vwi[0]='\0';
#if GCC
    strcat(vci,"/G");
#endif
#if ISPACKED || CNV_PCFILES
    strcat(vci,"/P");
#endif
#if PCREADLINUX
    strcat(vci,"X");
#endif
#if SWAPPED || CNV_PCBINUM
    strcat(vci,"C");
#endif
#if DOS32BITS
    strcat(vci,"32");
#endif
#if _LARGEFILE64_SOURCE
      /*sprintf(vwi,"/%dG",2<<cipmstxl);
      strcat(vci,vwi);*/
      strcat(vci,"/512G");
#endif
#if ISISUNISYS
    strcat(vci,"/X");
#endif
#if CIWTF
    strcat(vci,"/W");
#endif
#if CIFFI
    strcat(vci,"/F");
#endif
#if LIND
    strcat(vci,"/L");
#if LIND4
    strcat(vci,"4");
#endif
#endif
#if MULTI
    strcat(vci,"/M");
#endif
    vp=vci+strlen(vci);
    sprintf(vp,"/%"_LD_"/%d/%d",MAXMFRL,LE1,LE2);
    vp+=strlen(vp);
#if IFUPDATE                       /* Inverted file update */
    sprintf(vp,"/I");
    vp+=strlen(vp);
#endif
#if SIXTY_FOUR
    sprintf(vp,"/64bits");
    vp+=strlen(vp);
#endif

    if (!namep) namep="";
    if (namep[0] == '?') {
        vp=vwi;
#if CNLI
	sprintf(vp,"/%s",iy0extp); vp+=strlen(vp);
#endif
#if ZTREE                          /* MY: must be off */
	sprintf(vp,"/Z"); vp+=strlen(vp);
#endif
#if DBXMSTXL                       /* extended .mst capacity */
	/* strcat(vp,"/xGB"); vp+=strlen(vp); */
#endif
/* #if !CICPP - AOT 27/03/2001 */
#if RECGIZM                        /* CIGIZ */
	sprintf(vp,"/GIZ"); vp+=strlen(vp);
#endif
#if RECXPND                        /* MX: putdir/getdir */
	sprintf(vp,"/XPND"); vp+=strlen(vp);
#endif
#if RECDECO                        /* DEC */
	sprintf(vp,"/DEC"); vp+=strlen(vp);
#endif
/* #endif  CICPP   AOT 27/03/2001 */

#if GEN_ISI                        /* enable iso_open()/iso_read() */
	sprintf(vp,"/ISI"); vp+=strlen(vp);
#endif
#if GEN_UTL                        /* enable except loaductb() */
	sprintf(vp,"/UTL"); vp+=strlen(vp);
#endif
#if INVXBOOL                       /* Bx0 use several inverted files */
	sprintf(vp,"/INVX"); vp+=strlen(vp);
#endif
#if CIB64                          /* cib62 using ISIS I/F */
	sprintf(vp,"/B6"); vp+=strlen(vp);
#endif
#if CIB71                          /* cib71/cib72 */
	sprintf(vp,"/B7"); vp+=strlen(vp);
#endif
#if  TW_BOOL                       /* Bx0 multiple ifp's for text word */
	sprintf(vp,"/TW"); vp+=strlen(vp);
#endif
#if FATRAP                         /* fatal() trapping */
	sprintf(vp,"/FAT"); vp+=strlen(vp);
#endif
#if GIPAR                          /* dbn.par & dbxgicip processing */
	sprintf(vp,"/PAR"); vp+=strlen(vp);
#endif
#if CIPAR                          /* dbxcipfp & dbxcdcip processing */
	sprintf(vp,"/CIP"); vp+=strlen(vp);
#endif
#if CIPARIC                        /* CIPAR ignore case */
	sprintf(vp,"/IC"); vp+=strlen(vp);
#endif
#if CICGI
	sprintf(vp,"/CGI"); vp+=strlen(vp);
#endif
#if MXFUN || IFLOADFUN
    strcat(vp,"/MX");
#endif
#if CIAPI || CIAPI_SOURCE
    strcat(vp,"/A");
#endif
#if CI_XMLELEM
    strcat(vp,"/X");
#endif
#if CIWTF
    strcat(vp,"/W");
#endif
#if PROCXSLT
    strcat(vp,"/XSL");
#endif
    }

    p="CISIS Interface v5.7f";
    q="Copyright (c)BIREME/PAHO 2010. [http://reddes.bvsalud.org/projects/cisis]";
  //q="Copyright (c)BIREME/PAHO 2010. [http://www.bireme.br/products/cisis]";
  //q2="Copyright (c)BIREME/PAHO 2010. [http://bvsmodelo.bvsalud.org/php/index.php?lang=pt]";
    q3="Copyright (c)BIREME/PAHO 2010. [http://bvsmodelo.bvsalud.org/php/level.php?lang=pt&component=28&item=1]";

    if (namep[0] == '?')
      sprintf(cicopyr_area,"%s%s - %s\n%s%s\n%s\n",p,vci,namep+1,p,vwi,q);
      //sprintf(cicopyr_area,"%s%s - %s\n%s%s\n%s\n%s\n",p,vci,namep+1,p,vwi,q,q3);
    else
      sprintf(cicopyr_area,"%s%s - %s\n%s\n",p,vci,namep,q);
      
    return(cicopyr_area);
}



/* *****************************************************
   Funzioni di Conversione
   Original code by Beppe Romano (quadri.nis.garr.it)
***************************************************** */

#if CNV_PCBINUM

#if CICPP
int ConvertMST_CTLSTRUCT(char *buffer)
#else /*CICPP*/
int ConvertMST_CTLSTRUCT(buffer)
char *buffer;
#endif /*CICPP*/
{
  static int fieldlength[9]={4,4,4,2,2,4,4,4,4};
  int x,y;
#if CNVTRACE
  printf ("Converting MST_CTLSTRUCT\n");
#endif
  y=0;
  for (x=0;x<9;x++) {
    ConvertBuffer(buffer+y,fieldlength[x]);
    y+=fieldlength[x];
  }
  return (1);
}

#if CICPP
int ConvertMST_LEADER(char *buffer,
		      int   skipval,
		      int   breakval)
#else /*CICPP*/
int ConvertMST_LEADER(buffer,skipval,breakval)
char *buffer;
int skipval;
int breakval;
#endif /*CICPP*/
{
  static int fieldlength[7]={4,2,4,2,2,2,2};
  int x,y;
#if CNVTRACE
  LONGX *w;
  printf ("Converting MST_LEADER (skip=%d break=%d)\n",skipval,breakval);
for (x=0; x<LEADER; x++) printf("%02x ",buffer[x]);
printf("\n");
#endif
  y=0;
  for (x=0;x<7;x++) {
    if (y >= skipval) ConvertBuffer(buffer+y,fieldlength[x]);
    y+=fieldlength[x];
    if (y >= breakval) break;
  }
#if CNVTRACE
for (x=0; x<LEADER; x++) printf("%02x ",buffer[x]);
printf("\n");
  w=(LONGX *)buffer;
  printf("MFN converted:%d\n",*w);
#endif
  return (1);
}

#if CICPP
int ConvertMST_DIR(char *buffer,
		   int   nvf)
#else /*CICPP*/
int ConvertMST_DIR(buffer,nvf)
char *buffer;
int nvf;
#endif /*CICPP*/
{
  int x,y,z;
  char *c;
#if CNVTRACE
  printf ("Converting MST_DIR\n");
for (x=0; x<LEADER+6*nvf; x++) printf("%02x ",buffer[x]);
printf("\n");
#endif
  c=buffer+LEADER;
  y=0;
  for (x=nvf;x--; ){
     for (z=0;z<3;z++){
       ConvertBuffer(c+y,2);
       y+=2;
     }
  }
#if CNVTRACE
for (x=0; x<LEADER+6*nvf; x++) printf("%02x ",buffer[x]);
printf("\n");
#endif
  return (1);
}


#if CICPP
int ConvertXRF_PTR(char *buffer)
#else /*CICPP*/
int ConvertXRF_PTR(buffer)
char *buffer;
#endif /*CICPP*/
{
#if CNVTRACEx
    printf ("Converting XRF_PTR\n");
#endif
    ConvertBuffer(buffer,XRPTRSIZ);
    return(1);
}

#if CICPP
int ConvertXRF_REC(char *buffer)
#else /*CICPP*/
int ConvertXRF_REC(buffer)
char *buffer;
#endif /*CICPP*/
{
    XRSTRU *xrp;
    int n;
#if CNVTRACE
    printf ("Converting XRF_REC\n");
#endif
    xrp=(XRSTRU *)buffer;
    ConvertBuffer((char *)&xrp->xrxrpos,XRPOSSIZ);
    for (n=0; n < XRMAXTIV; n++)
	ConvertXRF_PTR((char *)&xrp->xrmfptr[n]);
    return(1);
}


#if CICPP
int ConvertL02_REC(char *buffer)
#else /*CICPP*/
int ConvertL02_REC(buffer)
char *buffer;
#endif /*CICPP*/
{
  static int fieldlength[34]={4,2,2,4,30,4,4,30,4,4,30,4,4,30,4,4,30,4,4,30,4,4,30,4,4,30,4,4,30,4,4,30,4,4};
  static int yes[34]={1,1,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1};
  int x,y;
#if CNVTRACE
  UCHR *c;
  LONGX v;
  printf ("Converting L02_REC\n");
#endif
  y=0;
  for (x=0;x<34;x++) {
	 if (yes[x]) ConvertBuffer(buffer+y,fieldlength[x]);
	 y+=fieldlength[x];
  }
#if CNVTRACE
  printf(" -  Number of active key: %d\n",256*(int)buffer[4]+(int)buffer[5]);
  c=buffer;
  c+=12;
  for (x=0;x<TWORDF;x++) {
    printf ("K: ");
    for (y=0;y<30;y++,c++) printf ("%c",*c);
    for (v=y=0;y<4;y++,c++) v=v*256+(UWORD)*c;
    printf (" info1: %"_LD_"\n",v);
    for (v=y=0;y<4;y++,c++) v=v*256+(UWORD)*c;
    printf (" info2: %"_LD_"\n",v);
  }
#endif
  return (1);
}

#if CICPP
int ConvertN02_REC(char *buffer)
#else /*CICPP*/
int ConvertN02_REC(buffer)
char *buffer;
#endif /*CICPP*/
{
  static int fieldlength[23]={4,2,2,30,4,30,4,30,4,30,4,30,4,30,4,30,4,30,4,30,4,30,4,};
  static int yes[23]={1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1};
  int x,y;
#if CNVTRACE
  UCHR *c;
  LONGX v;
  printf ("Converting N02_REC\n");
#endif
  y=0;
  for (x=0;x<23;x++) {
    if (yes[x]) ConvertBuffer(buffer+y,fieldlength[x]);
    y+=fieldlength[x];
  }
#if CNVTRACE
  printf(" -  Number of active key: %d\n",256*(int)buffer[4]+(int)buffer[5]);
  c=buffer;
  c+=8;
  for (x=0;x<TWORDN;x++) {
    printf ("K: ");
    for (y=0;y<30;y++,c++) printf ("%c",*c);
    for (v=y=0;y<4;y++,c++) v=v*256+(UWORD)*c;
    printf (" punt: %"_LD_"\n",v);
  }
#endif
  return (1);
}

#if CICPP
int ConvertL01_REC(char *buffer)
#else /*CICPP*/
int ConvertL01_REC(buffer)
char *buffer;
#endif /*CICPP*/
{
  static int fieldlength[34]={4,2,2,4,10,4,4,10,4,4,10,4,4,10,4,4,10,4,4,10,4,4,10,4,4,10,4,4,10,4,4,10,4,4,};
  static int yes[34]={1,1,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1};
  int x,y;
#if CNVTRACE
  UCHR *c;
  LONGX v;
  printf ("Converting L01_REC\n");
#endif
  y=0;
  for (x=0;x<34;x++) {
    if (yes[x]) ConvertBuffer(buffer+y,fieldlength[x]);
    y+=fieldlength[x];
  }
#if CNVTRACE
  printf(" -  Number of active key: %d\n",256*(int)buffer[4]+(int)buffer[5]);
  c=buffer;
  c+=12;
  for (x=0;x<TWORDF;x++) {
    printf ("K: ");
    for (y=0;y<10;y++,c++) printf ("%c",*c);
    for (v=y=0;y<4;y++,c++) v=v*256+(UWORD)*c;
    printf (" info1: %"_LD_"\n",v);
    for (v=y=0;y<4;y++,c++) v=v*256+(UWORD)*c;
    printf (" info2: %"_LD_"\n",v);
  }
#endif
  return (1);
}

#if CICPP
int ConvertN01_REC(char *buffer)
#else /*CICPP*/
int ConvertN01_REC(buffer)
char *buffer;
#endif /*CICPP*/
{
  static int fieldlength[23]={4,2,2,10,4,10,4,10,4,10,4,10,4,10,4,10,4,10,4,10,4,10,4,};
  static int yes[23]={1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1};
  int x,y;
#if CNVTRACE
  UCHR *c;
  LONGX v;
  printf ("Converting N01_REC\n");
#endif
  y=0;
  for (x=0;x<23;x++) {
    if (yes[x]) ConvertBuffer(buffer+y,fieldlength[x]);
    y+=fieldlength[x];
  }
#if CNVTRACE
  printf(" -  Number of active key: %d\n",256*(int)buffer[4]+(int)buffer[5]);
  c=buffer;
  c+=8;
  for (x=0;x<TWORDN;x++) {
    printf ("K: ");
    for (y=0;y<10;y++,c++) printf ("%c",*c);
    for (v=y=0;y<4;y++,c++) v=v*256+(UWORD)*c;
    printf (" punt: %"_LD_"\n",v);
  }
#endif
  return (1);
}


#if CICPP
int ConvertCNT_REC(char *buffer)
#else /*CICPP*/
int ConvertCNT_REC(buffer)
char *buffer;
#endif /*CICPP*/
{
  static int fieldlength[10]={2,2,2,2,2,2,4,4,4,2};
  int x,y;
  y=0;
#if CNVTRACE
  printf ("Converting CNT_REC\n");
#endif
  for (x=0;x<10;x++) {
    ConvertBuffer(buffer+y,fieldlength[x]);
    y+=fieldlength[x];
  }
  return (1);
}


#if CICPP
int ConvertBuffer(char *buffer,
		  int   fieldlength)
#else /*CICPP*/
int ConvertBuffer(buffer,fieldlength)
char *buffer;
int fieldlength;
#endif /*CICPP*/
{
  char cc[4];
  int x,y;
  y=fieldlength-1;
  for (x=0;x<fieldlength;x++) cc[y--]=buffer[x];
  for (x=0;x<fieldlength;x++) buffer[x]=cc[x];
  return 1;
}



#if CICPP
int ConvertIFP_BLKCTL(char *buffer,
		      int   blk1)
#else /*CICPP*/
int ConvertIFP_BLKCTL(buffer,blk1)
char *buffer;
int blk1;
#endif /*CICPP*/
{
  int y;
#if CNVTRACE
  LONGX *v;
  printf ("Converting IFP_BLKCTL blk1=%d\n",blk1);
#endif
  y=0;
  ConvertBuffer(buffer,4);
  y+=4;
#if CNVTRACE
  v=(LONGX *)buffer;
  printf("IFPRECNUM=%"_LD_"\n",*v);
#endif
  if (blk1) {
    ConvertBuffer(buffer+y,4);
    y+=4;
    ConvertBuffer(buffer+y,4);
  }
  return (1);
}


#if CICPP
int ConvertIFP_PSTHDR(char *buffer)
#else /*CICPP*/
int ConvertIFP_PSTHDR(buffer)
char *buffer;
#endif /*CICPP*/
{
  int x,y;
#if CNVTRACE
  printf ("Converting IFP_PSTHDR\n");
#endif
  y=0;
  for (x=0;x<5;x++) {
    ConvertBuffer(buffer+y,4);
    y+=4;
  }
  return (1);
}

#endif /* CNV_PCBINUM */

