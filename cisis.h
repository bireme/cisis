/* ***********************************************************
CISIS.H

CISIS application programs and functions include CISIS.H
as their main header file.

CISIS application programs include CIRUN.H, providing
the CISIS Interface run time pointers, control variables
and other CISIS application mandatory areas.

Please replace ciaot.h to cirun.h in include statements
of existing CISIS Interface applications.

                                AOT, August 19, 1992.

CISIS Interface v3.20 supports MicroISIS dbn.par file.
It might be necessary to change existing CISIS applications
to add a data base name parameter - to get a dbn.par file -
in the following functions: dbxopen(), dbxcipar(),
loadfile() and loadstw().

                                AOT, March 6, 1996.

*********************************************************** */


#ifndef CISIS_H
#define CISIS_H

/* *****************************************************
CISIS environment
***************************************************** */

#define GCC         01
#if GCC
#ifndef PACKED
#define PACKED  __attribute__ ((aligned(2)))
#endif
#endif

#define PC          0
#define DOS32BITS   0
#define MSC         0
#define VAX         0
#define MPE         0
#define UNIX        01
#define SUN         0
#define UNISYS      0
#define UNISYSGDB   0
#define UNISYSCPU   0
#define INTELCPU    1
#define BRME        0
#define BRMXCPU     0
#define CDCS4320    0

#define CNV_PCFILES 0   /* Standard MicroISIS PC format conversion */
#define CNV_PCBINUM 0   /* PC data representation (swapped) conversion */

#define PCREADLINUX 0   /* Read Linux's M/F in PC */

#ifdef CIAPI_SOURCE
#define CICPP       1   /* CISIS CPP preprocessor */
#endif
#ifdef ISIS_DLL
#define CICPP       1   /* CISIS CPP preprocessor */
#endif
#ifdef CI_WINISIS
#define CICPP       1   /* CISIS CPP preprocessor */
#endif
#ifndef CICPP
#define CICPP       0   /* CISIS CPP preprocessor */
#endif

#if CICPP
#ifndef USE_GDBFMT
#ifdef CI_WINISIS
#define USE_GDBFMT  1   /* CICPP/FST+IFU with GDB New FORMAT Release 2 */
#else
#define USE_GDBFMT  0   /* CICPP/FST+IFU with CISIS FORMAT */
#endif
#endif
#endif /* CICPP */

#if CICPP
#ifndef MAXNDBX
#if BEFORE990318
#define MAXNDBX  256L   /* maximum #entries in vdbxp[] */
#else
#define MAXNDBX  2048L  /* maximum #entries in vdbxp[] */
#endif /* BEFORE990318 */
#endif
#endif /* CICPP */

#if UNISYS || UNISYSCPU
#define ISISUNISYS  1
#endif

#if PC || VAX || GCC || (UNISYS && !UNISYSGDB)
#define ANSI        1
#endif

#if PC /* || VAX */
#define CRLF        1
#else
#define CRLF        0
#endif

#if PC || VAX || UNISYSCPU || INTELCPU || SUN
#define SWAPPED     1
#endif

#ifdef PACKED
#define ISPACKED    1
#else
#if SWAPPED
#if !UNIX /*#if PC || VAX*/
#define ISPACKED    1
#endif /* !UNIX */
#endif
#define PACKED
#endif

#if MPE || VAX || UNIX
#define O_BINARY    0
#define O_DENYNONE  0
#endif

#if SUN
#define SUNBUG_UNSIGNED 1 /* cannot test if link records have sorted postings */
#if GCC
#define SUNBUG_GCC1     1 /* cannot access node just read in noderead() */
#endif
#endif /* SUN */


/* *****************************************************
CISIS configuration
***************************************************** */

#define UC_EXTENSION ISISUNISYS  /* ISIS/UNISYS upper case .extension */
#define LINK_7544KEY ISISUNISYS  /* ISIS/UNISYS fixed-length link */
#define MST0FILL0XFF ISISUNISYS  /* ISIS/UNISYS .mst 1st block filler */

#if !CICPP
#ifndef CIAPI
#define CIAPI         0                   /* para compilar aplicacoes em C que usem CIAPI */
#endif
#if CIAPI
#ifndef TLS
#define TLS        0                   /* setar 1 para compilar zserver em windows-95 */
#endif
#endif
#endif
#ifndef CIB71
#define CIB71       1                   /* CISIS Rosnier Mijares Valdes' search function */
#endif
#ifndef CIB64
#define CIB64       0                   /* cib62 using ISIS I/F */
#endif
#ifndef CDMDL
#define CDMDL       0                   /* BRM CD-ROM Retrieval Interface */
#endif
#ifndef WWWISIS
#define WWWISIS     00
#endif /* WWWISIS */
#define BIGREC      0                   /* BRM CD-ROM Retrieval Interface */
#ifndef  LINDLUX
#define LINDLUX     0                   /* ifupd2 */
#endif
#define DBXMSTXL    1                   /* extended .mst capacity */
#if !EXCFMCGI
#define CICGI       1                   /* cicgi0/cicgi1 */
#endif
#if CICPP
#define FATRAP      0                   /* fatal() trapping */
#else /* CICPP */
#define FATRAP      1                   /* fatal() trapping */
#endif /* CICPP */

#ifndef CIFFI
#define CIFFI       0                   /* size-t mfrl/base/pos/len */
#endif
#ifndef CIWTF
#if !CICPP
#if !CIAPI
#if GCC
#if CIFFI
#define CIWTF       1                    /* WTFUN support */
#else
#define CIWTF       0                    /* WTFUN support */
#endif
#endif /* GCC */
#endif /* CIAPI */
#endif /* CICPP */
#endif


#if !CDMDL

#ifndef LIND
#if CIFFI
#define LIND        1                   /* MY: must be off */
#else
#define LIND        0                   /* MY: must be off */
#endif
#endif
#ifndef ISISXL
#if CIFFI || LIND
#define ISISXL      1                   /* Large keys - standard for CIFFI */
#else
#define ISISXL      00                   /* Large keys */
#endif
#endif
#ifndef ISISXL512
#define ISISXL512   00                  /* very Large keys */
#endif
#ifndef CNLI
#define CNLI        1                   /* MY: must be off */
#endif
#define ZTREE       1                   /* MY: must be off */
#define SAMEL       0                   /* MY: must be off */

#ifndef CI_WINISIS
#define RECGIZM     1                   /* CIGIZ */
#define RECXPND     0                   /* MX: putdir/getdir */
#define RECDECO     1                   /* DEC */
#endif
#define GEN_ISI     1                   /* enable iso_open()/iso_read() */
#define GEN_UTL     1                   /* enable except loaductb() */

#define INVXBOOL    (LIND || CIB64 || CIB71) /* 0 */       /* Bx0 use several inverted files */
#define  TW_BOOL    0                   /* Bx0 multiple ifp's for text word */

#define CIPAR       1                   /* dbxcipfp & dbxcdcip processing */
#define CIPARIC     0                   /* CIPAR ignore case */
#ifdef ISIS_DLL
#define GIPAR       1                   /* dbn.par & dbxgicip processing */
#endif
#ifdef CI_WINISIS
#define GIPAR       1                   /* dbn.par & dbxgicip processing */
#endif
#ifndef GIPAR
#define GIPAR       0                   /* dbn.par & dbxgicip processing */
#endif

#define MULTI       1                   /* Multi-user operation */
#define IFUPDATE    1                   /* Inverted file update */
#define MFUPDATE    1                   /* Master file update */

#else /* CDMDL */

#define ISISXL      1                   /* Large keys */
#define LIND        1                   /* MY: must be off */
#define CNLI        1                   /* MY: must be off */
#define ZTREE       1                   /* MY: must be off */
#define SAMEL       0                   /* MY: must be off */

#define RECGIZM     1                   /* CIGIZ */
#define RECXPND     0                   /* MX: putdir/getdir */
#define RECDECO     1                   /* DEC */
#define GEN_ISI     1                   /* enable iso_open()/iso_read() */
#define GEN_UTL     1                   /* enable except loaductb() */

#define INVXBOOL    1                   /* Bx0 use several inverted files */
#define  TW_BOOL    0                   /* Bx0 multiple ifp's for text word */

#define CIPAR       1                   /* dbxcipfp & dbxcdcip processing */
#define CIPARIC     0                   /* CIPAR ignore case */
#define GIPAR       0                   /* dbn.par & dbxgicip processing */

#define MULTI       01                   /* Multi-user operation */
#define IFUPDATE    01                   /* Inverted file update */
#define MFUPDATE    1                   /* Master file update */

#endif /* CDMDL */

#if _LARGEFILE64_SOURCE
#if  (_FILE_OFFSET_BITS != 64)
#error "_FILE_OFFSET_BITS != 64"
#endif
#define LSEEK64 lseek64
#define P_OFF_T "lld"
#define LONG_LONG long long
#else /* _LARGEFILE64_SOURCE */
#if (_FILE_OFFSET_BITS == 64)
#error "_FILE_OFFSET_BITS == 64"
#endif
#define LSEEK64 lseek
#define P_OFF_T "ld"
#define LONG_LONG long
#endif /* _LARGEFILE64_SOURCE */


/* *****************************************************
CISIS/C++ DLL export
***************************************************** */
#if CICPP
/* DLL export */
#if defined (BUILDING_YOUR_DLL)
#define _YOURCLASS _export
#else
#if defined (USING_YOUR_DLL)
#define _YOURCLASS _import
#else
#define _YOURCLASS
#endif /* USING_YOUR_DLL */
#endif /* BUILDING_YOUR_DLL */
#endif /* CICPP */

/*
        Required CISIS.H header files
*/

#include <stdio.h>   /* FILE */

/*
        Compiler dependent header files
*/

#if PC
#include <fcntl.h>
#include <process.h>
#include <io.h>
#include <stdlib.h>
#include <limits.h>
#include <dos.h>
#include <sys/types.h>
#endif

#if VAX
#include <file.h>
#include <stdlib.h>
#endif

#if MPE
#include <fcntl.h>
#include <stdlib.h>
#include <limits.h>
#endif

#if UNIX
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#endif

#ifndef SIXTY_FOUR
#define SIXTY_FOUR  0
#endif /* SIXTY_FOUR */

#if SIXTY_FOUR
#define LONGX int
#define LONGX_MAX INT_MAX
#define _LD_ "d"
#else /* SIXTY_FOUR */
#define LONGX long
#define LONGX_MAX LONG_MAX
#define _LD_ "ld"
#endif /* SIXTY_FOUR */

#define UBYTE       unsigned char       /* UCHR */
#define UWORD       unsigned short      /* INT  */
#if BEFOREWINDEFH
#define ULONG       unsigned LONGX       /*      */
#endif
#if BEFOREWINNTH
#define CHAR        UBYTE
#else
#define UCHR        UBYTE
#endif
#if BEFOREWINDEFH
#define INT         UWORD
#endif

#if CIFFI
#define FFI         unsigned int /* size_t = bug no linux - RP, 27/05/2003 */
#define MFRL_MAX    INT_MAX
#else
#define FFI         UWORD /* UWORD = UWORD unsigned short */
#define MFRL_MAX    SHRT_MAX
#endif


/*
        Memory allocation
*/

#if CICPP           /* alloc substituido por new */

#ifndef CINEWCPP
#define CINEWCPP 1  // AOT,HB 31/07/2002 */
#endif

#if MSC || !CINEWCPP                 /* HB, 27/05/2002 */
#define BAD_ALLOC ...   /* catch qualquer coisa */
#else /* MSC || !CINEWCPP */
#if 0                   /* catch especifico */
#define BAD_ALLOC xalloc
#include <except.h>
#else /* 0 */
#include <new>
#define BAD_ALLOC std::bad_alloc
#endif /* 0 */
#endif /* MSC || !CINEWCPP */

#define ALLOMAXV    64000U /* <==== RPIVA VAI MUDAR 20010214*/
#define ALLOPARM    unsigned int
#define ALLONULL    NULL
//#define CORELEFT()  640000

#else /* CICPP */

#if PC
#if MSC
#include <malloc.h>
#define ALLOMAXV    64000U
#define ALLOC       malloc
#define ALLOPARM    unsigned int
#define ALLONULL    NULL
#define FREE        free
#define CORELEFT    coreleft
#else /* MSC */
#if DOS32BITS /* 20000518 */
#include <alloc.h>
#if BEFORE20010105
#define ALLOMAXV    UINT_MAX /* 640000L */ /* AOT/RP, 27/03/2001 */
#else
#define ALLOMAXV    INT_MAX  /* 640000L */ /* AOT/RP, 27/03/2001 */
#endif
#define ALLOC       malloc
#define ALLOPARM    size_t
#define ALLONULL    NULL
#define FREE        free
#define CORELEFT    coreleft
#else /* DOS32BITS 20000518 */
#include <alloc.h>
#define ALLOMAXV    64000L
#define ALLOC       farmalloc
#define ALLOPARM    unsigned LONGX
#define ALLONULL    NULL
#define FREE        farfree
#define CORELEFT    farcoreleft
#endif /* DOS32BITS 20000518 */
#endif
#endif /* PC */

#if VAX
#define ALLOC       malloc
#define ALLOPARM    size_t
#define ALLOMAXV    640000L
#define ALLONULL    NULL
#define FREE        free
#define CORELEFT    coreleft
#endif

#if MPE
#include <malloc.h>
#define ALLOC       malloc
#define ALLOPARM    unsigned int
#define ALLOMAXV    64000000L
#define ALLONULL    NULL
#define FREE        free
#define CORELEFT    coreleft
#endif

#if UNIX
#include <malloc.h>
#define ALLOC       malloc
#define ALLOPARM    unsigned
#define ALLOMAXV    INT_MAX /* 64000000L */ /* 64000L*/ /* 128000000L*/ /* UINT_MAX */
#define ALLONULL    NULL
#define FREE        free
#define CORELEFT    coreleft
#if !GCC
#define size_t      int
#endif
#endif

#endif /* CICPP */


/*
        MAX PATH NAME
*/
#if PC
#if DOS32BITS
#define CIMPL 512
#else /* DOS32BITS */
#define CIMPL (128+4)
#endif /* DOS32BITS */
#else
#define CIMPL 512
#endif /* PC */


/*
        Unbuffered low level I/O
*/
#if CICPP
#define PERMIS      0644
#define CREAT       creat
#define OPEN        open
#define CLOSE       close
#define CIREAD      read
#define CIWRITE     write
#else /* CICPP */
#if PC
#if MSC
#define PERMIS      0644
#define CREAT       creat
#if MULTI
#define OPEN        sopen
#else
#define OPEN        open
#endif
#define CLOSE       close
#define CIREAD      read
#define CIWRITE     write
#else /* MSC */
#define PERMIS      0000
#if DOS32BITS /* UINT_MAX -1 */
#define CREAT       _rtl_creat
#define OPEN        _rtl_open
#define CLOSE       _rtl_close
#define CIREAD      _rtl_read
#define CIWRITE     _rtl_write
#else /* DOS32BITS */
#define CREAT       _creat
#define OPEN        _open
#define CLOSE       _close
#define CIREAD      _read
#define CIWRITE     _write
#endif /* DOS32BITS */
#endif /* MSC */
#else
#if VAX
#define PERMIS      0644
#endif
#if MPE
#define PERMIS      0644
#endif
#if UNIX
/*#define PERMIS      0644*/
#define PERMIS      0664    /* Requested by Chico in apr 2012 */
#endif
#define CREAT       creat
#define OPEN        open
#define CLOSE       close
#define CIREAD      read
#define CIWRITE     write
#endif
#endif /* CICPP */

/*
        Standard Library extensions
*/


#define GEN_CORELEFT            MSC || MPE || VAX || UNIX  || DOS32BITS /* 20000518 */
#define GEN_LABS                MPE || VAX || (UNIX && !UNISYS)
#define GEN_STRSTR              MPE || VAX || (UNIX && !UNISYSGDB && !SUN && !BRME && !GCC)
#define GEN_STRUPR              MPE || VAX || UNIX
#define GEN_STRREV              MPE || VAX || UNIX
#define GEN_MEMICMP             /* CIPARIC && */ (MPE || VAX || (UNIX && !BRME))

#if CDCS4320 || MPE || (GCC && !CICPP) || BRME
#define CONST                   /* not supported */
#else
#define CONST const
#endif

#if GEN_CORELEFT
#if ANSI
unsigned LONGX coreleft(void);
#else
unsigned LONGX coreleft();
#endif
#endif

#if GEN_LABS
#if ANSI
long labs(long x);
#else
long labs();
#endif
#endif

#if GEN_STRSTR
#if ANSI
char *strstr(CONST char *s1, CONST char *s2);
#else
char *strstr();
#endif
#endif

#if GEN_STRUPR
#if ANSI
char *strupr(char *p);
#else
char *strupr();
#endif
#endif

#if GEN_STRREV
#if ANSI
char *strrev(char *p);
#else
char *strrev();
#endif
#endif

#if GEN_MEMICMP
#if ANSI
int memicmp(CONST void *s1, CONST void *s2, size_t n);
#else
int memicmp();
#endif
#endif


#if CICPP
class _YOURCLASS CISISX;
#endif /* CICPP */

/*
        FATRAP support
*/

#if FATRAP
#include <setjmp.h>
#endif

/* *****************************************************
CISIS/C++ extern "C"
***************************************************** */
#if CICPP
#ifdef __cplusplus
extern "C" {
#endif
#endif /* CICPP */


/* *****************************************************
CIDBX.H
***************************************************** */

#if MAXMFRL==0
#if CIFFI
#define MAXMFRL    1048576L     /* 1024*1024 ! */
#else /* CIFFI */
#if UNIX || WWWISIS || USE_CIREC || DOS32BITS
#if BEFORE20010109
#define MAXMFRL    30000L       /*     */
#else
#if MULTI
#define MAXMFRL    32767L       /*     */
#else /* MULTI */
#define MAXMFRL    63002L       /*     */
#endif /* MULTI */
#endif
#else /* UNIX || WWWISIS || USE_CIREC || DOS32BITS */
#define MAXMFRL     8192L       /* gdb */
#endif /* UNIX || WWWISIS || USE_CIREC || DOS32BITS */
#endif /* CIFFI */
#endif /* MAXMFRL */	

#if ISISXL
#define LE1         16          /* gdb ISISXL */
#if SUPERISIS
#define LE2         256
#else
#if ISISXL512
#define LE2         512         /* gdb ISISXL */
#else
#define LE2         60          /* gdb ISISXL */
#endif
#endif /* SUPERISIS */
#else /* ISISXL */
#define LE1         10          /* gdb */
#define LE2         30          /* gdb */
#endif /* ISISXL */


#define XRBSIZ      512         /* gdb */
#define XRSHIFT     9           /*     */

#define XRMAXTIV    127         /* gdb */
#define XRMAXTV1    126         /*     */

#define MSBSIZ      512         /* gdb */
#define MSBSHIFT    9           /*     */

#define XRXDIVIDE   2048        /* gdb */
#define XRXSHIFT    11          /*     */
#define XRXMASK     0x000001FF  /* gdb no IF pending */
#define XRXMASKN    0x00000400  /* gdb ++ IF pending */
#define XRXMASKU    0x00000200  /* gdb ++ IF pending */

#if LIND
#ifndef LIND4
#define LIND4       1 /*0*/
#endif /* LIND4 */
#endif /* LIND */

#if LIND4
#define MAXUPDMFN   0x7FFFFFFFL /* gdb posting */
#else /* LIND4 */
#define MAXUPDMFN   0x00FFFFFFL /* gdb posting */
#endif /* LIND4 */

#define MSMFTUSR    0           /* gdb non system */
#define MSMFTMSG    1           /* gdb msg data base */

#define XRPOS       LONGX
#define XRPTR       /*off_t*/ LONGX
#define XRPOSSIZ    sizeof(XRPOS)
#define XRPTRSIZ    sizeof(XRPTR)

typedef struct xrstru {
    XRPOS     xrxrpos;           /* gdb .xrf blk order */
    XRPTR     xrmfptr[XRMAXTIV]; /* gdb mfr ptrs */
} XRSTRU;

typedef struct msstru {
    char     msbuff[MSBSIZ];    /* gdb .mst blk */
    LONGX     msbufn;            /* gdb .mst blk order - simulated */
} MSSTRU;


#define IFBSIZ      512         /* gdb */
#define IFSHIFT     9           /*     */

#define IFMAXTIV    127         /* gdb */

#define PUNT        LONGX        /* gdb paho 1988 */

#define INFO        LONGX        /* gdb */

#if LIND
#ifndef INFO8
#define INFO8       1            /* 8-byte ptr and size (info1/info3) in .ly1/.ly2 IDXE - AOT/HB 14/03/2008 */
#endif /* INFO8 */

#if INFO8
#if _LARGEFILE64_SOURCE
#define INFX        off_t
#else /* LARGEFILE64_SOURCE */
#define INFX        LONGX        /* gdb */
#endif /* LARGEFILE64_SOURCE */
#else  /* INFO8 */
#define INFX        LONGX        /* gdb */
#endif /* INFO8 */
#else /* LIND */
#define INFX        LONGX        /* gdb */
#endif /* LIND */

#define INFOSIZE    sizeof(INFO)
#define IFHDRINFOS  5           /* check in trminit() */
#define IFPSTINFOS  2           /*                    */
#define IFNVSPLT    28          /* gdb .ifp never split - header+posting */

#define MAXLIV      8           /* INVMAP.nx */
#define NEGLIV      (-1)        /* INVMAP.cn[treecase].liv - gdb */

#define ORDN        5           /* gdb */
#define TWORDN      10          /*     */

#define ORDF        5           /* gdb */
#define TWORDF      10          /*     */

#define CNTN        15          /* gdb */
#define CNTK        5           /* gdb */

#if SAMEL
#define MAXSAMEL    8           /* max no. of .ifp on same leaf */
#endif


typedef struct cnstru {
    UWORD      idtype      PACKED;/* gdb .cnt idtype */
    UWORD      ordn        PACKED;/* gdb .cnt ordn */
    UWORD      ordf        PACKED;/* gdb .cnt ordf */
    UWORD      n           PACKED;/* gdb .cnt n */
    UWORD      k           PACKED;/* gdb .cnt k */
    short    liv         PACKED;/* gdb .cnt liv = no of index levels */
/* 6*2=12 */
    PUNT     posrx       PACKED;/* gdb .cnt posrx = pos of root node */
    PUNT     nmaxpos     PACKED;/* gdb .cnt nmaxpos */
    PUNT     fmaxpos     PACKED;/* gdb .cnt fmaxpos */
    UWORD      abnormal    PACKED;/* gdb .cnt abnormal */
/*  CNBUNI   +2 */
} CNSTRU;

typedef struct n0stru {
    PUNT     pos         PACKED;/* gdb .n0x pos */
    UWORD      ock         PACKED;/* gdb .n0x ock */
    UWORD      it          PACKED;/* gdb .n0x it */
    char     idxchars[1] PACKED;/* gdb .n01 and .n02 idx */
} N0STRU;

typedef struct l0stru {
    PUNT     pos         PACKED;/* gdb .l0x pos */
    UWORD      ock         PACKED;/* gdb .l0x ock */
    UWORD      it          PACKED;/* gdb .l0x it */
    PUNT     ps          PACKED;/* gdb .l0x ps */
#if LIND
    PUNT     psb         PACKED;/* tlc gdb .l0x ps backward */
#endif
    char     idxchars[1] PACKED;/* gdb .l01 and .l02 idx */
} L0STRU;


typedef struct nxstru {
    int      pages;             /* no of memo buffers alloced */
    int      left;              /* no of memo buffers still free */
    char    *basep;             /* area addr */
} NXSTRU;

#define NTREE           2
#define LTXPAGES        128L
#define LUXPAGES        1024L
#define LVXPAGES        1024L /* lvxmxmfn/8+1 */
#define LVXIYPBS        8192L /* ou INT_MAX */

typedef struct invmap {
#if LINDLUX
    LONGX ltxthresh;
    LONGX ltxpages[NTREE];
    N0STRU *ltxvpagp[NTREE][1+LTXPAGES];
    LONGX luxthresh;
    LONGX luxpages[NTREE];
    L0STRU *luxvpagp[NTREE][1+LUXPAGES];
    int  lvxiflind;
    LONGX lvxmaxmfn;
    LONGX lvxpages;
    char *lvxvpagp[1+LVXPAGES];
    FILE *lvxfplog;
#endif /* LINDLUX */
    char    *ifl1p;             /* .l01 block */
    char    *ifl2p;             /* .l02 block */
#if SAMEL
    int      iflxn;             /* #total */
#endif
#if ZTREE
    int      iflzx;             /* ztree */
#endif
#if 1 /* IFUPDATE */
    int      cnopn;             /* =dbxopen(.cnt) */
#endif
    int      n1opn;             /* =dbxopen(.n01) */
    int      n2opn;             /* =dbxopen(.n02) */
    int      l1opn;             /* =dbxopen(.l01) */
    int      l2opn;             /* =dbxopen(.l02) */
#if SAMEL
    int      ifopn[MAXSAMEL];   /* =dbxopen(.ifp) */
#else
    int      ifopn;             /* =dbxopen(.ifp) */
#endif
#if 1 /* IFUPDATE */
    int      cnopw;             /* dbxopenw/w(.cnt) */
#endif
    int      n1opw;             /* dbxopenw/w(.n01) */
    int      n2opw;             /* dbxopenw/w(.n02) */
    int      l1opw;             /* dbxopenw/w(.l01) */
    int      l2opw;             /* dbxopenw/w(.l02) */
    int      ifopw;             /* dbxopenw/w(.ifp) */
    int      ifopv;             /* dbxflock/dbxulock(.ifp) */
    CNSTRU   cn[2];             /* .cnt recs */
    NXSTRU   nx[2][MAXLIV];     /* .n01 and .n02 pages */
    char    *nybasep[2];        /* all .n01 and .n02 pages */
    char    *lybasep[2];        /* all .l01 and .l02 pages */
    char    *iybasep;           /* all .ifp blocks */
    off_t    cc_offset;         /* .cnt SEEK_SET offset */
#if CNLI
    off_t    cn_offset[2];      /* .n0x SEEK_SET offset */
    off_t    cl_offset[2];      /* .l0x SEEK_SET offset */
    off_t    ci_offset;         /* .ifp SEEK_SET offset */
#endif
} INVMAP;


#if RECGIZM
typedef struct gizmstru {
    short isize;
    short osize;
    unsigned char *ipatt;
    unsigned char *opatt;
    LONGX nused;
    struct gizmstru *nextp;
} GIZMSTRU;

#define MAXGIZR 16

typedef struct vgizpstru {
    GIZMSTRU *ghdrp[256];
    int grngs;
    UWORD grng1[MAXGIZR];
    UWORD grng2[MAXGIZR];
    unsigned char *gdbnp;
    struct vgizpstru *nextp;
} VGIZPSTRU;

#endif


#if RECDECO
#define MAXDECR 16
#define MAXDECX 16

typedef struct vdecpstru {
    unsigned char *ddbnp;
    int drngs;
    UWORD drng1[MAXDECR];
    UWORD drng2[MAXDECR];
    unsigned char drdlm;
    int dsflds;
    unsigned char dsfld[MAXDECX];
    unsigned char dsfldx[MAXDECX];
    LONGX dsfldv[MAXDECX];
    struct vdecpstru *nextp;
} VDECPSTRU;
#endif

#if CIAPI
typedef void DBXSTRU;
#else /* CIAPI */
#if CICPP
class _YOURCLASS DBXSTRU;       /* Para substituir def da struct */
#else
typedef struct dbxstru {
    char       dbxname[CIMPL+1];  /* dbname + .ext + NULL */
    int        dbxxropn;          /* =dbxopen(.xrf) */
    int        dbxmsopn;          /* =dbxopen(.mst) */
    int        dbxxropw;          /* dbxopenw/w(.xrf) */
    int        dbxmsopw;          /* dbxopenw/w(.mst) */
    int        dbxmsopv;          /* dbxflock/dbxulock(.mst) */
    XRSTRU    *dbxxribp;          /* ptr .xrf ibuf */
    MSSTRU    *dbxmsibp;          /* ptr .mst ibuf */
#if GIPAR
    char      *dbxgicip;          /* extended .mst capacity */
#endif
#if DBXMSTXL
    int        dbxmstxl;          /* extended .mst capacity */
#endif
    int        dbxmflush;         /* mstflush() flag */
    int        dbxmclose;         /* mstclose() flag */
    int        dbxiflush;         /* invflush() flag */
#if MULTI
    int        dbxnetws;          /* type of network support */
    int        dbxdelxx;          /* flag data entry lock */
    int        dbxewlxx;          /* flag exclusive write lock */
    int        dbxmxtmp;          /* mx() tmpseq master file */
#endif
    LONGX       dbxmsmfn;          /* mstsetup() .mst nxtmfn */
    INVMAP    *dbxifmap;          /* .cnt, .n0x, .l0x, .ifp */
#if 1 /* SAMEL */
    int        dbxiflxx;          /* dbx samel */
#endif
    char      *dbxxryyp;          /* all .xrf blocks */
    char      *dbxmsyyp;          /* all .mst blocks */
#if RECGIZM
    VGIZPSTRU *dbxvgzrp;          /* gizmo processing */
#endif
#if RECXPND
    int        dbxxpn01;          /* expand processing */
    int        dbxxpn02;          /* expand processing */
#endif
#if RECDECO
    VDECPSTRU *dbxvderp;          /* decod processing */
#endif
#if IFUPDATE
    int        dbxiinit;          /* ifupdat() init */
    int        dbxitrac;          /* ifupdat() trace parameter */
    LONGX       dbxitell;          /* ifupdat() tell parameter */
    LONGX       dbxirang;          /* ifupdat() tell parameter */
    LONGX       dbxirecs;          /* ifupdat() records updated */
    LONGX       dbxipadd[2];       /* ifupdat() added lk1/lk2 posts */
    LONGX       dbxipdel[2];       /* ifupdat() deleted lk1/lk2 posts */
#endif
} DBXSTRU;
#endif /* CICPP */

#define DBXname         dbxp->dbxname
#define DBXxropn        dbxp->dbxxropn
#define DBXmsopn        dbxp->dbxmsopn
#define DBXxropw        dbxp->dbxxropw
#define DBXmsopw        dbxp->dbxmsopw
#define DBXmsopv        dbxp->dbxmsopv
#define DBXxribp        dbxp->dbxxribp
#define DBXmsibp        dbxp->dbxmsibp
#if GIPAR
#define DBXgicip        dbxp->dbxgicip
#endif
#if DBXMSTXL
#define DBXmstxl        dbxp->dbxmstxl
#endif
#define DBXmflush       dbxp->dbxmflush
#define DBXmclose       dbxp->dbxmclose
#define DBXiflush       dbxp->dbxiflush
#if MULTI
#define DBXnetws        dbxp->dbxnetws
#define DBXdelxx        dbxp->dbxdelxx
#define DBXewlxx        dbxp->dbxewlxx
#define DBXmxtmp        dbxp->dbxmxtmp
#endif
#define DBXmsmfn        dbxp->dbxmsmfn
#define DBXifmap        dbxp->dbxifmap
#define DBXiflxx        dbxp->dbxiflxx
#define DBXxryyp        dbxp->dbxxryyp
#define DBXmsyyp        dbxp->dbxmsyyp
#if RECGIZM
#define DBXvgzrp        dbxp->dbxvgzrp
#endif
#if RECXPND
#define DBXxpn01        dbxp->dbxxpn01
#define DBXxpn02        dbxp->dbxxpn02
#endif
#if RECDECO
#define DBXvderp        dbxp->dbxvderp
#endif
#if IFUPDATE
#define DBXiinit        dbxp->dbxiinit
#define DBXitrac        dbxp->dbxitrac
#define DBXitell        dbxp->dbxitell
#define DBXirang        dbxp->dbxirang
#define DBXirecs        dbxp->dbxirecs
#define DBXipadd        dbxp->dbxipadd
#define DBXipdel        dbxp->dbxipdel
#endif

#define VDBXname(i)     vdbxp[i]->dbxname
#define VDBXxropn(i)    vdbxp[i]->dbxxropn
#define VDBXmsopn(i)    vdbxp[i]->dbxmsopn
#define VDBXxropw(i)    vdbxp[i]->dbxxropw
#define VDBXmsopw(i)    vdbxp[i]->dbxmsopw
#define VDBXmsopv(i)    vdbxp[i]->dbxmsopv
#define VDBXxribp(i)    vdbxp[i]->dbxxribp
#define VDBXmsibp(i)    vdbxp[i]->dbxmsibp
#if GIPAR
#define VDBXgicip(i)    vdbxp[i]->dbxgicip
#endif
#if DBXMSTXL
#define VDBXmstxl(i)    vdbxp[i]->dbxmstxl
#endif
#define VDBXmflush(i)   vdbxp[i]->dbxmflush
#define VDBXmclose(i)   vdbxp[i]->dbxmclose
#define VDBXiflush(i)   vdbxp[i]->dbxiflush
#if MULTI
#define VDBXnetws(i)    vdbxp[i]->dbxnetws
#define VDBXdelxx(i)    vdbxp[i]->dbxdelxx
#define VDBXewlxx(i)    vdbxp[i]->dbxewlxx
#define VDBXmxtmp(i)    vdbxp[i]->dbxmxtmp
#endif
#define VDBXmsmfn(i)    vdbxp[i]->dbxmsmfn
#define VDBXifmap(i)    vdbxp[i]->dbxifmap
#define VDBXiflxx(i)    vdbxp[i]->dbxiflxx
#define VDBXxryyp(i)    vdbxp[i]->dbxxryyp
#define VDBXmsyyp(i)    vdbxp[i]->dbxmsyyp
#if RECGIZM
#define VDBXvgzrp(i)    vdbxp[i]->dbxvgzrp
#endif
#if RECXPND
#define VDBXxpn01(i)    vdbxp[i]->dbxxpn01
#define VDBXxpn02(i)    vdbxp[i]->dbxxpn02
#endif
#if RECDECO
#define VDBXvderp(i)    vdbxp[i]->dbxvderp
#endif
#if IFUPDATE
#define VDBXiinit(i)    vdbxp[i]->dbxiinit
#define VDBXitrac(i)    vdbxp[i]->dbxitrac
#define VDBXitell(i)    vdbxp[i]->dbxitell
#define VDBXirang(i)    vdbxp[i]->dbxirang
#define VDBXirecs(i)    vdbxp[i]->dbxirecs
#define VDBXipadd(i)    vdbxp[i]->dbxipadd
#define VDBXipdel(i)    vdbxp[i]->dbxipdel
#endif

#define DBIifl1p        dbxp->dbxifmap->ifl1p
#define DBIifl2p        dbxp->dbxifmap->ifl2p
#define DBIiflxn        dbxp->dbxifmap->iflxn
#define DBIiflzx        dbxp->dbxifmap->iflzx
#define DBIcnopn        dbxp->dbxifmap->cnopn
#define DBIn1opn        dbxp->dbxifmap->n1opn
#define DBIn2opn        dbxp->dbxifmap->n2opn
#define DBIl1opn        dbxp->dbxifmap->l1opn
#define DBIl2opn        dbxp->dbxifmap->l2opn
#if SAMEL
#define DBIifopn(lxx)   dbxp->dbxifmap->ifopn[lxx]
#else
#define DBIifopn        dbxp->dbxifmap->ifopn
#endif
#define DBIcnopw        dbxp->dbxifmap->cnopw
#define DBIn1opw        dbxp->dbxifmap->n1opw
#define DBIn2opw        dbxp->dbxifmap->n2opw
#define DBIl1opw        dbxp->dbxifmap->l1opw
#define DBIl2opw        dbxp->dbxifmap->l2opw
#define DBIifopw        dbxp->dbxifmap->ifopw
#define DBIifopv        dbxp->dbxifmap->ifopv
#define DBIcn(i)        dbxp->dbxifmap->cn[i]
#define DBInx(i,j)      dbxp->dbxifmap->nx[i][j]
#define DBInybasep(i)   dbxp->dbxifmap->nybasep[i]
#define DBIlybasep(i)   dbxp->dbxifmap->lybasep[i]
#define DBIiybasep      dbxp->dbxifmap->iybasep
#if CNLI
#define DBIcn_offset(i) dbxp->dbxifmap->cn_offset[i]
#define DBIcl_offset(i) dbxp->dbxifmap->cl_offset[i]
#define DBIci_offset    dbxp->dbxifmap->ci_offset
#endif

#endif /* CIAPI */

#if !CIAPI
#if !CICPP      /* Deslocados para classe DBXSTRU  */
/* CIDBX Interface */
#if ANSI
#if MULTI
int dbxcinet(char *dbnamp);
int dbxflock(DBXSTRU *dbxp, char *type);
int dbxulock(DBXSTRU *dbxp, char *type);
int dbxilock(DBXSTRU *dbxp, char *mdbnp, char *type);
int dbxwlock(DBXSTRU *dbxp, char *m0p, int times);
#endif  /* MULTI */
void dbxflush(char *dbnamp);
DBXSTRU *dbxsrchp(char *dbnamp);
DBXSTRU *dbxstorp(char *dbnamp);
int dbxopen(char *gidbnp, char *dbnamp, char *extp);
#if CLOSEX
int CLOSE(int fd);
#endif
int dbxopenw(char *gidbnp, char *dbnamp,char *extp,int *opnp,int *opwp,char *errmsgp);
char *dbxopenc(char *gidbnp, char *filnamp,int *opnp,int *opwp,char *errmsgp, int xcreate, int xappend);
void dbxinit(void);
void fatal(char *msg);
int fpccreat(char *gidbnp, char *namp,char *extp,int lrecl);
void fpcwrite(char *recbufp,FFI reclen);
void fpcclose(void);
char *dbxtmpnm(char *dirtmp, int strip, char *filnamp);
char *cicopyr(char *namep);
#else
#if MULTI
int dbxcinet();
int dbxflock();
int dbxulock();
int dbxilock();
int dbxwlock();
#endif /* MULTI */
void dbxflush();
DBXSTRU *dbxsrchp();
DBXSTRU *dbxstorp();
int dbxopen();
int CLOSE();
int dbxopenw();
char *dbxopenc();
void dbxinit();
void fatal();
int fpccreat();
void fpcwrite();
void fpcclose();
char *dbxtmpnm();
char *cicopyr();
#endif /* ANSI */
#endif /* CICPP */
#endif /* CIAPI */
/* global */

#if !CICPP                      /* Deslocados para classe CISIS */
extern DBXSTRU *vdbxp[];        /* pointers to the entries      */
extern LONGX ndbxs;              /* actual #entries              */
extern LONGX maxndbx;            /* runtime value                */
#endif /* CICPP */

#if !CICPP
extern int dbxtrace;            /* dbxopen/dbxcipar trace */

extern int dbxopt_fatal;        /* dbxopen() - fatal when file doesn't exist */
extern int dbxopt_errno;        /* dbxopen() - dbname/errno msg before fatal */

extern int dbxopt_mflush;       /* record() - flush lastrecread if != dbname */
extern int dbxopt_mclose;       /* record() - close lastrecread if != dbname */
extern int dbxopt_iflush;       /* term() - flush lasttrmread if != dbname */

extern int dbxopt_ordwr;        /* dbxopen() - O_RDWR / O_RDONLY (LM/CD-ROM) */
extern int dbxordwr;            /* dbxopen() - dbxopt_ordwr & O_RDWR */

extern off_t dbxfloff;          /* dbxflock()/dbxulock() - offset */
extern LONGX dbxflsiz;          /* dbxflock()/dbxulock() - length */
extern int dbxfloop;            /* dbxflock() - #retries */
extern int dbxwloop;            /* dbxwlock() - #retries */
extern int dbxiloop;            /* dbxilock() - #retries */
extern int dbxuclos;            /* dbxulock() - close/reopen */
extern int dbxewlrc;            /* dbxflock()/dbxwlock() error action */

extern LONGX rec_maxmfrl;   /* MAXMFRL/cipar maxmfrl entry */
extern LONGX rec_mstload;      /* 28/03/97 */
extern LONGX trm_invload;      /* 28/03/97 */

#if CNV_PCBINUM
extern char cnv_pcbuff[];  /* PC data representation (swapped) */
#endif

#if CNV_PCFILES
extern UCHR nodeunibuff[];      /* area para ler pagina de .n01/.n02 */
#if ISISUNISYS
#define LEAFCNV_PCFILES 0
#else
#define LEAFCNV_PCFILES 1
#endif
#if LEAFCNV_PCFILES
extern UCHR leafunibuff[];      /* area para ler pagina de .l01/.l02 */
#endif
#endif
#endif /* CICPP */

#ifndef ON
#define ON       1
#endif
#ifndef OFF
#define OFF      0
#endif
#define BY8SHIFT 3
#define BY8RMASK 0x07
#if !CICPP
extern unsigned char bitmask[]; /* 0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01 */
#endif /* CICPP */

#define FPCBSIZ 0x4000  /*16K*/ /* fpccreat()/fpcwrite()/fpcclose() */
#if !CICPP
extern int fpc_fd;              /* fpccreat()/fpcwrite()/fpcclose() */
extern FFI fpc_left;            /* fpccreat()/fpcwrite()/fpcclose() */
extern char *fpc_buffer;        /* fpccreat()/fpcwrite()/fpcclose() */

extern int vlex[];              /* use it */
extern int nxbsiz[];            /* use it */
extern int lxbsiz[];            /* use it */

extern char *mx1extp;           /* mstsetup/recisis0 */
extern char *xx1extp;           /* mstsetup/recisis0 */

extern char *cx1extp;           /* invsetup/trmisis0 */
extern char *nx12extp[];        /* invsetup/trmisis0 */
extern char *lx12extp[];        /* invsetup/trmisis0 */
extern char *ix1extp;           /* invsetup/trmisis0 */
#if CNLI
extern char *iy0extp;           /* All IF into one. To ALP, WL */
#endif

#if MULTI
extern int cipnetws;            /* MULTI default operation */
#endif

#if DBXMSTXL
extern int cipmstxl;            /* extended .mst capacity */
#endif
#endif /* CICPP */

/* FATRAP Interface */
#define MAXFATML        80 /* for fatal_iomsg or equivalent */
#if FATRAP
#if !CICPP      /* Deslocados para a classe CISIS em cisis.hpp */
/* global */
extern int     fatal_errcod;    /* cisis fatal() error code */
extern char    fatal_iomsg[];   /* cisis fatal() flag/msg */
extern jmp_buf fatal_jumper;    /* cisis fatal() longjmp() */
#endif /* CICPP */
#endif

#if !CICPP      /* Deslocados para a classe DBXSTRU em cidbx.cpp */
/* CIPAR Interface */
#if ANSI
int dbxciset(char *cipfile);
char *dbxcipar(char *gidbnp, char argkey[], char argchar);
#if GIPAR
char *dbxgipar(char *gidbnp, char *gikeyp, int *namlenp, int *extlenp);
#endif
#else
int dbxciset();
char *dbxcipar();
#if GIPAR
int dbxgipar();
#endif
#endif
#endif /* CICPP */

/* global */
#if !CICPP
extern FILE *dbxcipfp;          /* dbxcipar() input parameter file pointer */
extern char *dbxcdcip;          /* dbxcipar() input data */
extern char dbxcikey[];         /* dbxcipar() actual file name */
extern int dbxcipok;            /* dbxcipar() found */

#if GIPAR
extern char dbxgikey[];         /* dbxgipar() actual file name */
extern char *dbxgiext[];        /* dbxgipar() dbn.par .extensions */
extern int dbxgiexn[];          /* dbxgipar() dbn.par numbers */
#endif
#endif /* CICPP */


/* *****************************************************
CIREC.H
***************************************************** */

typedef struct m0stru {
    LONGX     m0ctlmfn    PACKED;/* gdb ctlmfn */
    LONGX     m0nxtmfn    PACKED;/* gdb nxtmfn */
    LONGX     m0nxtmfb    PACKED;/* gdb nxtmfb */
    UWORD      m0nxtmfp    PACKED;/* gdb nxtmfp - offset */
    UWORD      m0mftype    PACKED;/* gdb mftype */
#if PCREADLINUX
    UWORD      fillerXm0mftype;
#endif /* PCREADLINUX */
    LONGX     m0reccnt    PACKED;/* gdb reccnt */
    LONGX     m0mfcxx1    PACKED;/* gdb mfcxx1 */
    LONGX     m0mfcxx2    PACKED;/* gdb mfcxx2 - MULTI: Data entry lock */
    LONGX     m0mfcxx3    PACKED;/* gdb mfcxx3 - MULTI: Exclusive write lock */
} M0STRU;

typedef struct dir_entry {
    UWORD    tag         PACKED;/* gdb dir.tag */
    FFI      pos         PACKED;/* gdb dir.pos */
    FFI      len         PACKED;/* gdb dir.len */
} DIRSTRU;

#if CNV_PCFILES
typedef struct m1stru {    /* Conversion is done during I/O */
    LONGX     m1mfn;             /* gdb mfn */
    LONGX     m1mfbwb;           /* gdb mfbwb */
    FFI      m1mfrl;            /* gdb mfrl - MULTI: record being updated */
    UWORD      m1mfbwp;           /* gdb mfbwp - offset */
    FFI      m1base;            /* gdb base */
    UWORD      m1nvf;             /* gdb nvf */
    UWORD      m1status;          /* gdb status */
    DIRSTRU  m1dir[1];          /* gdb dir_entries */
} M1STRU;
#else
typedef struct m1stru {
/* PACKED CIFFI */
/* 4 */      LONGX     m1mfn       PACKED;/* gdb mfn */
/* 2 4 */    FFI      m1mfrl      PACKED;/* gdb mfrl - MULTI: record being updated */
#if PCREADLINUX
             UWORD      fillerXm1mfrl;
#endif /* PCREADLINUX */
/* 4 */      LONGX     m1mfbwb     PACKED;/* gdb mfbwb */
/* 2 */      UWORD      m1mfbwp     PACKED;/* gdb mfbwp - offset */
/* 2 4 */    FFI      m1base      PACKED;/* gdb base (MSNVSPLT) */
/* 14 18 */
/*  ? 20 */
/* 2 */      UWORD      m1nvf       PACKED;/* gdb nvf */
/* 2    */   UWORD      m1status    PACKED;/* gdb status */
/* 18 22 */
/* 20 24 align4 */
/* 6 10 */   DIRSTRU  m1dir[1]    PACKED;/* gdb dir_entries */
/* ?  ?  align4 */
} M1STRU;
#endif

#if CIFFI || PCREADLINUX
#if 0
#define LEADER      22          /* gdb */
#define MSNVSPLT    18          /* gdb .mst never split - 'til base */
#endif /* 0 */
#define LEADER      (sizeof(M1STRU)-sizeof(DIRSTRU))       /* pc=18,hp=20;ff=22/24 */
#define MSNVSPLT    (LEADER-sizeof(UWORD)-sizeof(UWORD))   /* pc=14,hp=16;ff=18/20  */
#else /* CIFFI */
#if (ISPACKED && !UNIX) || CNV_PCFILES     /* PC || VAX || CNV_PCFILES */
#define LEADER      18          /* gdb */
#define MSNVSPLT    14          /* gdb .mst never split - 'til base */
#else /* ISPACKED || CNV_PCFILES */
#define LEADER      20          /* gdb */
#define MSNVSPLT    16          /* gdb .mst never split - 'til base */
#endif /* ISPACKED || CNV_PCFILES */
#endif /* CIFFI */


typedef union mfunion {
    M0STRU   m0;
    M1STRU   m1;
    char     mx[1];             /* gdb flds */
} MFUNION;

#if CIAPI
typedef void RECSTRU;
#else /* CIAPI */
#if CICPP
class _YOURCLASS RECSTRU;       /* Para substituir def da struct */
#endif /* CICPP */

#if !CICPP                      /* Declarada como classe em cirec.hpp */
typedef struct recstru {
    LONGX     recnbytes;         /* nbytes allocated (RECHSIZE not included) */
    int      rectype;           /* layout indicator */
    DBXSTRU *recdbxp;           /* ptr dbx */
    int      recrc;             /* record rc */
#if MULTI
    int      reclock;           /* mfr read/lock option */
    int      recwlock;          /* mfr write/lock option */
#endif
    int      recgdbl;           /* mfr read/mfrl lock granted */
    int      recgdbw;           /* mfr read/mfrl flag */
    MFUNION  recmf;             /* master_recs and .mx unions */
} RECSTRU;

#define RECHSIZE        sizeof(RECSTRU)-sizeof(MFUNION)
#endif /* CICPP */

#define RECnbytes       recp->recnbytes
#define RECtype         recp->rectype

#define RECdbxp         recp->recdbxp

#define RDBname         recp->recdbxp->dbxname
#define RDBxropn        recp->recdbxp->dbxxropn
#define RDBmsopn        recp->recdbxp->dbxmsopn
#define RDBxropw        recp->recdbxp->dbxxropw
#define RDBmsopw        recp->recdbxp->dbxmsopw
#define RDBmsopv        recp->recdbxp->dbxmsopv
#define RDBxribp        recp->recdbxp->dbxxribp
#define RDBmsibp        recp->recdbxp->dbxmsibp
#if GIPAR
#define RDBgicip        recp->recdbxp->dbxgicip
#endif
#if DBXMSTXL
#define RDBmstxl        recp->recdbxp->dbxmstxl
#endif
#define RDBmflush       recp->recdbxp->dbxmflush
#define RDBmclose       recp->recdbxp->dbxmclose
#if MULTI
#define RDBnetws        recp->recdbxp->dbxnetws
#define RDBdelxx        recp->recdbxp->dbxdelxx
#define RDBewlxx        recp->recdbxp->dbxewlxx
#define RDBmxtmp        recp->recdbxp->dbxmxtmp
#endif
#define RDBmsmfn        recp->recdbxp->dbxmsmfn
#define RDBifmap        recp->recdbxp->dbxifmap
#define RDBiflxx        recp->recdbxp->dbxiflxx
#define RDBxryyp        recp->recdbxp->dbxxryyp
#define RDBmsyyp        recp->recdbxp->dbxmsyyp
#if RECGIZM
#define RDBvgzrp        recp->recdbxp->dbxvgzrp
#endif
#if RECXPND
#define RDBxpn01        recp->recdbxp->dbxxpn01
#define RDBxpn02        recp->recdbxp->dbxxpn02
#endif
#if RECDECO
#define RDBvderp        recp->recdbxp->dbxvderp
#endif

#define RECrc           recp->recrc
#if MULTI
#define REClock         recp->reclock
#define RECwlock        recp->recwlock
#endif
#define RECgdbl         recp->recgdbl
#define RECgdbw         recp->recgdbw

#if CICPP
#define MF0ctlmfn       recp->recmfp->m0.m0ctlmfn
#define MF0nxtmfn       recp->recmfp->m0.m0nxtmfn
#define MF0nxtmfb       recp->recmfp->m0.m0nxtmfb
#define MF0nxtmfp       recp->recmfp->m0.m0nxtmfp
#define MF0mftype       recp->recmfp->m0.m0mftype
#define MF0reccnt       recp->recmfp->m0.m0reccnt
#define MF0mfcxx1       recp->recmfp->m0.m0mfcxx1
#define MF0mfcxx2       recp->recmfp->m0.m0mfcxx2
#define MF0mfcxx3       recp->recmfp->m0.m0mfcxx3

#define MFRmfn          recp->recmfp->m1.m1mfn
#define MFRmfrl         recp->recmfp->m1.m1mfrl
#define MFRmfbwb        recp->recmfp->m1.m1mfbwb
#define MFRmfbwp        recp->recmfp->m1.m1mfbwp
#define MFRbase         recp->recmfp->m1.m1base
#define MFRnvf          recp->recmfp->m1.m1nvf
#define MFRstatus       recp->recmfp->m1.m1status
#define MFRdir          recp->recmfp->m1.m1dir

#define DIRtag(x)       recp->recmfp->m1.m1dir[x].tag
#define DIRpos(x)       recp->recmfp->m1.m1dir[x].pos
#define DIRlen(x)       recp->recmfp->m1.m1dir[x].len

#define MFX             recp->recmfp->mx
#else /* CICPP */
#define MF0ctlmfn       recp->recmf.m0.m0ctlmfn
#define MF0nxtmfn       recp->recmf.m0.m0nxtmfn
#define MF0nxtmfb       recp->recmf.m0.m0nxtmfb
#define MF0nxtmfp       recp->recmf.m0.m0nxtmfp
#define MF0mftype       recp->recmf.m0.m0mftype
#define MF0reccnt       recp->recmf.m0.m0reccnt
#define MF0mfcxx1       recp->recmf.m0.m0mfcxx1
#define MF0mfcxx2       recp->recmf.m0.m0mfcxx2
#define MF0mfcxx3       recp->recmf.m0.m0mfcxx3

#define MFRmfn          recp->recmf.m1.m1mfn
#define MFRmfrl         recp->recmf.m1.m1mfrl
#define MFRmfbwb        recp->recmf.m1.m1mfbwb
#define MFRmfbwp        recp->recmf.m1.m1mfbwp
#define MFRbase         recp->recmf.m1.m1base
#define MFRnvf          recp->recmf.m1.m1nvf
#define MFRstatus       recp->recmf.m1.m1status
#define MFRdir          recp->recmf.m1.m1dir

#define DIRtag(x)       recp->recmf.m1.m1dir[x].tag
#define DIRpos(x)       recp->recmf.m1.m1dir[x].pos
#define DIRlen(x)       recp->recmf.m1.m1dir[x].len

#define MFX             recp->recmf.mx
#endif /* CICPP */

#define VRECnbytes(r)   vrecp[r]->recnbytes
#define VRECtype(r)     vrecp[r]->rectype

#define VRECdbxp(r)     vrecp[r]->recdbxp

#define VRDBname(r)     vrecp[r]->recdbxp->dbxname
#define VRDBxropn(r)    vrecp[r]->recdbxp->dbxxropn
#define VRDBmsopn(r)    vrecp[r]->recdbxp->dbxmsopn
#define VRDBxropw(r)    vrecp[r]->recdbxp->dbxxropw
#define VRDBmsopw(r)    vrecp[r]->recdbxp->dbxmsopw
#define VRDBmsopv(r)    vrecp[r]->recdbxp->dbxmsopv
#define VRDBxribp(r)    vrecp[r]->recdbxp->dbxxribp
#define VRDBmsibp(r)    vrecp[r]->recdbxp->dbxmsibp
#if GIPAR
#define VRDBgicip(r)    vrecp[r]->recdbxp->dbxgicip
#endif
#if DBXMSTXL
#define VRDBmstxl(r)    vrecp[r]->recdbxp->dbxmstxl
#endif
#define VRDBmflush(r)   vrecp[r]->recdbxp->dbxmflush
#define VRDBmclose(r)   vrecp[r]->recdbxp->dbxmclose
#if MULTI
#define VRDBnetws(r)    vrecp[r]->recdbxp->dbxnetws
#define VRDBdelxx(r)    vrecp[r]->recdbxp->dbxdelxx
#define VRDBewlxx(r)    vrecp[r]->recdbxp->dbxewlxx
#define VRDBmxtmp(r)    vrecp[r]->recdbxp->dbxmxtmp
#endif
#define VRDBmsmfn(r)    vrecp[r]->recdbxp->dbxmsmfn
#define VRDBifmap(r)    vrecp[r]->recdbxp->dbxifmap
#define VRDBiflxx(r)    vrecp[r]->recdbxp->dbxiflxx
#define VRDBxryyp(r)    vrecp[r]->recdbxp->dbxxryyp
#define VRDBmsyyp(r)    vrecp[r]->recdbxp->dbxmsyyp
#if RECGIZM
#define VRDBvgzrp(r)    vrecp[r]->recdbxp->dbxvgzrp
#endif
#if RECXPND
#define VRDBxpn01(r)    vrecp[r]->recdbxp->dbxxpn01
#define VRDBxpn02(r)    vrecp[r]->recdbxp->dbxxpn02
#endif
#if RECDECO
#define VRDBvderp(r)    vrecp[r]->recdbxp->dbxvderp
#endif

#define VRECrc(r)       vrecp[r]->recrc
#if MULTI
#define VREClock(r)     vrecp[r]->reclock
#define VRECwlock(r)    vrecp[r]->recwlock
#endif
#define VRECgdbl(r)     vrecp[r]->recgdbl
#define VRECgdbw(r)     vrecp[r]->recgdbw

#if CICPP
#define VMF0ctlmfn(r)   vrecp[r]->recmfp->m0.m0ctlmfn
#define VMF0nxtmfn(r)   vrecp[r]->recmfp->m0.m0nxtmfn
#define VMF0nxtmfb(r)   vrecp[r]->recmfp->m0.m0nxtmfb
#define VMF0nxtmfp(r)   vrecp[r]->recmfp->m0.m0nxtmfp
#define VMF0mftype(r)   vrecp[r]->recmfp->m0.m0mftype
#define VMF0reccnt(r)   vrecp[r]->recmfp->m0.m0reccnt
#define VMF0mfcxx1(r)   vrecp[r]->recmfp->m0.m0mfcxx1
#define VMF0mfcxx2(r)   vrecp[r]->recmfp->m0.m0mfcxx2
#define VMF0mfcxx3(r)   vrecp[r]->recmfp->m0.m0mfcxx3

#define VMFRmfn(r)      vrecp[r]->recmfp->m1.m1mfn
#define VMFRmfrl(r)     vrecp[r]->recmfp->m1.m1mfrl
#define VMFRmfbwb(r)    vrecp[r]->recmfp->m1.m1mfbwb
#define VMFRmfbwp(r)    vrecp[r]->recmfp->m1.m1mfbwp
#define VMFRbase(r)     vrecp[r]->recmfp->m1.m1base
#define VMFRnvf(r)      vrecp[r]->recmfp->m1.m1nvf
#define VMFRstatus(r)   vrecp[r]->recmfp->m1.m1status
#define VMFRdir(r)      vrecp[r]->recmfp->m1.m1dir

#define VDIRtag(r,x)    vrecp[r]->recmfp->m1.m1dir[x].tag
#define VDIRpos(r,x)    vrecp[r]->recmfp->m1.m1dir[x].pos
#define VDIRlen(r,x)    vrecp[r]->recmfp->m1.m1dir[x].len

#define VMFX(r)         vrecp[r]->recmfp->mx
#else /* CICPP */
#define VMF0ctlmfn(r)   vrecp[r]->recmf.m0.m0ctlmfn
#define VMF0nxtmfn(r)   vrecp[r]->recmf.m0.m0nxtmfn
#define VMF0nxtmfb(r)   vrecp[r]->recmf.m0.m0nxtmfb
#define VMF0nxtmfp(r)   vrecp[r]->recmf.m0.m0nxtmfp
#define VMF0mftype(r)   vrecp[r]->recmf.m0.m0mftype
#define VMF0reccnt(r)   vrecp[r]->recmf.m0.m0reccnt
#define VMF0mfcxx1(r)   vrecp[r]->recmf.m0.m0mfcxx1
#define VMF0mfcxx2(r)   vrecp[r]->recmf.m0.m0mfcxx2
#define VMF0mfcxx3(r)   vrecp[r]->recmf.m0.m0mfcxx3

#define VMFRmfn(r)      vrecp[r]->recmf.m1.m1mfn
#define VMFRmfrl(r)     vrecp[r]->recmf.m1.m1mfrl
#define VMFRmfbwb(r)    vrecp[r]->recmf.m1.m1mfbwb
#define VMFRmfbwp(r)    vrecp[r]->recmf.m1.m1mfbwp
#define VMFRbase(r)     vrecp[r]->recmf.m1.m1base
#define VMFRnvf(r)      vrecp[r]->recmf.m1.m1nvf
#define VMFRstatus(r)   vrecp[r]->recmf.m1.m1status
#define VMFRdir(r)      vrecp[r]->recmf.m1.m1dir

#define VDIRtag(r,x)    vrecp[r]->recmf.m1.m1dir[x].tag
#define VDIRpos(r,x)    vrecp[r]->recmf.m1.m1dir[x].pos
#define VDIRlen(r,x)    vrecp[r]->recmf.m1.m1dir[x].len

#define VMFX(r)         vrecp[r]->recmf.mx
#endif /* CICPP */

#define FIELDP(xdir)            MFX+(MFRbase+DIRpos(xdir))

#define VFIELDP(irec,xdir)      VMFX(irec)+(VMFRbase(irec)+VDIRpos(irec,xdir))

#endif /* CIAPI */

#define TYPEMF0     11
#define TYPEMFR     12
#define TYPEX       13
#define TYPEMFQ     TYPEMFR             /* 31 */

#if MULTI
#define NOLOCK      0           /* recread() unlocked option */
#define RLOCK       1           /* recread() record lock */
#define DELOCK      2           /* recread() data entry lock */
#define RDELOCK     3           /* recread() record or data entry lock */
#define EWLOCK      4           /* recread() exclusive write lock */
#define FORCE       8           /* recunlck() force unlock */
#define WUNLOCK     1           /* recwrite() unlock */
#define WLOCK       2           /* recwrite() keep lock */
#define NEWREC      4           /* recwrite() new record mask */
#endif

#define RCLOCK      (-2)        /* gdb - Lock refused */
#define RCEOF       (-1)        /* gdb */
#define RCNORMAL    0           /* gdb */
#define RCLDEL      1           /* gdb */
#define RCPDEL      2           /* gdb */

#define ACTIVE      0           /* gdb - mfr.leader.status */
#define DELETED     1           /* gdb - mfr.leader.status */

/* CIREC Interface */
#if !CICPP                      /* transferidas para a classe RECSTRU */
#if ANSI
LONGX recallok(LONGX irec, LONGX nbytes);
LONGX record(LONGX irec, char *dbnamp, LONGX mfn);
int nocc(LONGX irec, int tag);
int fieldx(LONGX irec, int tag, int occ);
int fieldn(LONGX irec, int tag, int occ);
int recread(RECSTRU *recp, LONGX mfn);
int recxref(RECSTRU *recp, LONGX mfn, LONGX *comb, int *comp);
void recinit(void);
void mstsetup(char *dbnamp, LONGX loadxrf, LONGX loadmst);
void mstflush(char *dbnamp);
void mstclose(DBXSTRU *dbxp);
#else
LONGX recallok();
LONGX record();
int nocc();
int fieldx();
int fieldn();
int recread();
int recxref();
void recinit();
void mstsetup();
void mstflush();
void mstclose();
#endif
#endif /* CICPP */

/* global */
#if !CICPP
extern RECSTRU *vrecp[];        /* pointers to the entries      */
extern LONGX nrecs;              /* actual #entries              */
extern LONGX maxnrec;            /* runtime value                */
#endif /* CICPP */

#if !CICPP
extern int rectrace;            /* rec RESTRACE runtime switch  */

extern LONGX recxrefb;           /* recxref() comb */
extern int  recxrefp;           /* recxref() comp */
extern int  recxrefn;           /* recxref() flagnew */
extern int  recxrefm;           /* recxref() flagmod */
extern FFI  recreadl;           /* force recread() mfrl */
#if IFUPDATE
extern LONGX recreadb;           /* force recread() comb = mfbwb */
extern int  recreadp;           /* force recread() comp = mfbwp */
#endif
#endif /* CICPP */



/* *****************************************************
CIGIZ.H
***************************************************** */

#if RECGIZM
#define GIZMTAG1        1
#define GIZMTAG2        2
#define GIZMTAG11       11
#define GIZMTAG21       21

#if !CIAPI

/* CIGIZ Interface */
#if ANSI
unsigned char *pattalloc(int n);
GIZMSTRU *gizmalloc(int n);
#if CICPP
int gizmread(CISISX *cisisxp, char *gizdbnp, VGIZPSTRU **vgizmpp);
LONGX recgizmo(CISISX *cisisxp, RECSTRU *recp, VGIZPSTRU *vgizmap);
#else /* CICPP */
int gizmread(char *gizdbnp, VGIZPSTRU **vgizpp, LONGX irec);
LONGX recgizmo(LONGX irec, VGIZPSTRU *vgizmap);
#endif /* CICPP */
void gizflush(VGIZPSTRU *vgizmap);
#else
unsigned char *pattalloc();
GIZMSTRU *gizmalloc();
int gizmread();
LONGX recgizmo();
void gizflush();
#endif /* ANSI */

#endif /* !CIAPI */


#endif /* RECGIZM */

#if RECXPND
#if ANSI
#if CICPP
LONGX recxpnmo(RECSTRU *recp);
LONGX recxpn00 (RECSTRU *recp, int xpn01, int xpn02);
#else /* CICPP */
LONGX recxpnmo(LONGX irec);
LONGX recxpn00(LONGX irec, int xpn01, int xpn02);
#endif /* CICPP */
#else
LONGX recxpnmo();
LONGX recxpn00();
#endif /* ANSI */
#endif /* RECXPND */


#if RECDECO
#define DECOTAG1        1
#define DECOTAG2        2
#define DECOTAG3        3
#define DECOTAG4        4

/* CIDEC Interface */
#if ANSI
#if CICPP
int recdecod(CISISX * cisisxp, RECSTRU *recp, char *decdbnp);
LONGX recdecex (CISISX * cisisxp, RECSTRU *recp);
#else /* CICPP */
int recdecod(LONGX irec, char *decdbnp, LONGX wrec);
LONGX recdecex(LONGX irec);
#endif /* CICPP */
void decflush(VDECPSTRU *vdecmap);
#else
int recdecod();
LONGX recdecex();
void decflush();
#endif /* ANSI */
#endif /* RECDECO */

/* global */
#if !CICPP
extern int dectrace;            /* decoread()/recdeco() runtime switch  */
#endif /* CICPP */



/* *****************************************************
CITRM.H
***************************************************** */

typedef struct n1idxe {
    UCHR     key[LE1] PACKED;          /* gdb .n01 idx key */
    PUNT     punt PACKED;              /* gdb .n01 idx punt */
} N1IDXE;

typedef struct n2idxe {
    UCHR     key[LE2] PACKED;          /* gdb .n02 idx key */
    PUNT     punt PACKED;              /* gdb .n02 idx punt */
} N2IDXE;

typedef struct n1stru {
    PUNT     pos PACKED;               /* gdb .n01 pos */
    UWORD    ock PACKED;               /* gdb .n01 ock */
    UWORD    it PACKED;                /* gdb .n01 it */
    N1IDXE   idx[TWORDN] PACKED;       /* gdb .n01 idx */
} N1STRU;

typedef struct n2stru {
    PUNT     pos PACKED;               /* gdb .n02 pos */
    UWORD    ock PACKED;               /* gdb .n02 ock */
    UWORD    it PACKED;                /* gdb .n02 it */
    N2IDXE   idx[TWORDN] PACKED;       /* gdb .n02 idx */
} N2STRU;


typedef struct l1idxe {
    UCHR     key[LE1] PACKED;          /* gdb .l01 idx key */
#if ISISUNISYS
    UCHR     keyalign[2] PACKED; /* see ISIS.PAS for UNISYS */
#endif
    INFX     info1 PACKED;             /* gdb .l01 idx info1 */
    INFO     info2 PACKED;             /* gdb .l01 idx info2 */
#if LIND
    union {
       INFX    info3 PACKED;          /* tlc info3 - tot no of docs */
       INFX    info4 PACKED;          /* tlc info4 - posting type and etc */
    } info3info4;
#endif
} L1IDXE;

typedef struct l2idxe {
    UCHR     key[LE2] PACKED;          /* gdb .l02 idx key */
#if ISISUNISYS
    UCHR     keyalign[2] PACKED; /* see ISIS.PAS for UNISYS */
#endif
    INFX     info1 PACKED;             /* gdb .l02 idx info1 */
    INFO     info2 PACKED;             /* gdb .l02 idx info2 */
#if LIND
    union {
        INFX info3 PACKED;             /* tlc info3 - tot no of docs */
        INFX info4 PACKED;             /* tlc info4 - posting type and etc */
    } info3info4;
#endif
} L2IDXE;

typedef struct l1stru {
    PUNT     pos PACKED;               /* gdb .l01 pos */
    UWORD    ock PACKED;               /* gdb .l01 ock */
    UWORD    it PACKED;                /* gdb .l01 it */
    PUNT     ps PACKED;                /* gdb .l01 ps */
#if LIND
    PUNT     psb PACKED;               /* tlc gdb .l0x ps backward */
#endif
    L1IDXE   idx[TWORDF] PACKED;       /* gdb .l01 idx */
} L1STRU;

typedef struct l2stru {
    PUNT     pos PACKED;               /* gdb .l02 pos */
    UWORD    ock PACKED;               /* gdb .l02 ock */
    UWORD    it PACKED;                /* gdb .l02 it */
    PUNT     ps PACKED;                /* gdb .l02 ps */
#if LIND
    PUNT     psb PACKED;                       /* tlc gdb .l0x ps backward */
#endif
    L2IDXE   idx[TWORDF] PACKED;       /* gdb .l02 idx */
} L2STRU;


#if SAMEL
                                /* *** SAMEL *** */
typedef struct lxidxe {
    INFX     info1;             /* gdb .l0x idx info1 */
    INFO     info2;             /* gdb .l0x idx info2 */
#if LIND
    union {
        INFX info3;             /* tlc info3 - tot no of docs */
        INFX info4;             /* tlc info4 - posting type and etc */
    } info3info4;
#endif
} LXIDXE;

typedef struct lxl1stru {
    PUNT     pos;               /* gdb .l01 pos */
    UWORD    ock;               /* gdb .l01 ock */
    UWORD    it;                /* gdb .l01 it */
    PUNT     ps;                /* gdb .l01 ps */
#if LIND
    PUNT     psb;               /* tlc gdb .l0x ps backward */
#endif
    UCHR     key[TWORDF][LE1];  /* gdb .l01 idx key */
    LXIDXE   idx[1][TWORDF];    /* gdb .l01 idx - samel */
} LXL1STRU;

typedef struct lxl2stru {
    PUNT     pos;               /* gdb .l02 pos */
    UWORD    ock;               /* gdb .l02 ock */
    UWORD    it;                /* gdb .l02 it */
    PUNT     ps;                /* gdb .l02 ps */
#if LIND
    PUNT     psb;                       /* tlc gdb .l0x ps backward */
#endif
    UCHR     key[TWORDF][LE2];  /* gdb .l02 idx key */
    LXIDXE   idx[1][TWORDF];    /* gdb .l02 idx - samel */
} LXL2STRU;

#endif /* SAMEL */


#if ZTREE

#define LZKASZ1 (TWORDF*(2+LE1))
#define LZKASZ2 (TWORDF*(2+LE2))

typedef struct lzstru {
    PUNT     pos PACKED;               /* gdb .l0x pos */
    UWORD    ock PACKED;               /* gdb .l0x ock */
    UWORD    it PACKED;                /* gdb .l0x it */
    PUNT     ps PACKED;                /* gdb .l0x ps */
#if LIND
    PUNT     psb PACKED;               /* tlc gdb .l0x ps backward */
#endif
    INFX     info1[TWORDF] PACKED;     /* tlc gdb .l0x idx info1 */
    INFO     info2[TWORDF] PACKED;     /* tlc gdb .l0x idx info2 */
#if LIND
    INFX     info3[TWORDF] PACKED;     /* tlc gdb .l0x idx info3 */
#endif
    char     keys[LZKASZ2] PACKED;     /* compressed keys */
} LZSTRU;

#define LZHSIZE (sizeof(LZSTRU)-LZKASZ2)

#endif /* ZTREE */

#define MINLCHR     ' '

#if LIND4
#define PMFNSIZ     4           /* gdb */
#else /* LIND4 */
#define PMFNSIZ     3           /* gdb */
#endif /* LIND4 */

#define PTAGSIZ     2           /* gdb */
#define POCCSIZ     1           /* gdb */
#define PCNTSIZ     2           /* gdb */

#if SWAPPED
#if LIND4
#define M1      3
#define M2      2
#define M3      1
#define M4      0
#else /* LIND4 */
#define M1      2
#define M2      1
#define M3      0
#define M4      3
#endif /* LIND4 */
#define T1      1
#define T2      0
#define O1      0
#define O2      1
#define C1      1
#define C2      0
#else /* SWAPPED */
#if LIND4
#define M1      0
#define M2      1
#define M3      2
#define M4      3
#else /* LIND4 */
#define M1      1
#define M2      2
#define M3      3
#define M4      0
#endif /* LIND4 */
#define T1      0
#define T2      1
#define O1      1
#define O2      0
#define C1      0
#define C2      1
#endif /* SWAPPED */

#define BITSTRING   (-1)        /* xxxSTRING also used for CIB7HITLIST */
#define MFNSTRING   0
#define CNTSTRING   1

#if LIND
typedef struct ifpstru {
    UCHR     ifpx[IFBSIZ];      /* tlc .ifp data */
} IFPSTRU;
#else
typedef struct ifpstru {
    INFO     ifpblk;            /* gdb .ifp blk order */
    INFO     ifprec[IFMAXTIV];  /* gdb .ifp data */
} IFPSTRU;

typedef struct ifpctrl {
    INFO     ifpblk;            /* gdb .ifp blk order */
    INFO     ifprec1;           /* gdb .ifp ifprec(1) */
    INFO     ifprec2;           /* gdb .ifp ifprec(2) */
} IFPCTRL;

typedef struct ifphead {
    INFO     ifpnxtb;           /* gdb .ifp ifpnxtb */
    INFO     ifpnxtp;           /* gdb .ifp ifpnxtp */
    INFO     ifptotp;           /* gdb .ifp ifptotp */
    INFO     ifpsegp;           /* gdb .ifp ifpsegp */
    INFO     ifpsegc;           /* gdb .ifp ifpsegc */
} IFPHEAD;

#define IFHDRSIZ    sizeof(IFPHEAD)
#endif

typedef struct poststru {                               /* CIFST */
    UCHR pmfn[PMFNSIZ];         /* gdb .ifp pmfn */
    UCHR ptag[PTAGSIZ];         /* gdb .ifp ptag */
    UCHR pocc[POCCSIZ];         /* gdb .ifp pocc */
    UCHR pcnt[PCNTSIZ];         /* gdb .ifp pcnt */
} POSTSTRU;

#define IFPSTSIZ    sizeof(POSTSTRU)

#define CNBSIZ      sizeof(CNSTRU)
#define N1BSIZ      sizeof(N1STRU)
#define N2BSIZ      sizeof(N2STRU)
#define L1BSIZ      sizeof(L1STRU)
#define L2BSIZ      sizeof(L2STRU)

#if CNV_PCFILES
#if PC || GCC
#define CNBUNI      0
#define N1BUNI      0
#define N2BUNI      0
#define L1BUNI      0
#define L2BUNI      0
#else
#define CNBUNI      2
#define N1BUNI      2
#define N2BUNI      2
#define L1BUNI      2
#define L2BUNI      2
#endif
#endif

#if CIAPI
typedef void TRMSTRU;
#else /* CIAPI */
#if CICPP
class _YOURCLASS TRMSTRU;       /* Para substituir def da struct */
#endif /* CICPP */

#if !CICPP                      /* Declarada como classe em citrm.hpp */
typedef struct trmstru {
    LONGX    trmnbytes;         /* nbytes allocated */
    int      trmtype;           /* layout indicator */
    DBXSTRU *trmdbxp;
    int      trmrc;             /* term/nxterm rc */
    UCHR     trmkey[LE2+1];     /* key padded with blanks and null */
    LONGX    trmpost;           /* current posting order */
    LONGX    trmpmfn;           /* current posting pmfn */
    UWORD    trmptag;           /* current posting ptag */
    UWORD    trmpocc;           /* current posting pocc */
    UWORD    trmpcnt;           /* current posting pcnt */
    L1STRU   trml1buf;          /* .l01 input buffer */
    UCHR     trml1nul;          /* .l01 input buffer idx delimiter */
    L2STRU   trml2buf;          /* .l02 input buffer */
    UCHR     trml2nul;          /* .l02 input buffer idx delimiter */
    int      trmlcase;          /* last term/nxterm tree used */
    int      trmrcase;          /* trmread() treecase+1 selection  */
    off_t    trmrseek[2];       /* last term/nxterm leafread() lseek addr */
    int      trmrsize[2];       /* last term/nxterm leafread() read size */
    int      trmliock[2];       /* last trmread() ock */
    UCHR     *trmlidxp[2];      /* last term/nxterm idxchars ptr */
    LONGX    trmpleft;          /* postings left in segment */
    UCHR     *trmpcurr;         /* trmpchar not yet incremented */
    UCHR     *trmpchar;         /* ptr to next posting in trmifbuf */
#if LIND
    INFX      trmyaddr;          /* byte address */
    INFO      trmypsts;          /* total no of psts */
    int       trmycase;          /* postings type */
    int       trmysize;          /* postings length */
    INFO      trmydocs;          /* total no of docs */
    INFX      trmydisk;          /* total no of bytes */
    INFX      trmyread;          /* total no of bytes already read */
    INFX      trmyleft;          /* total no of bytes left on ifbuf */
#else
    INFO     trmpcblk;          /* info1 for next posting() */
    INFO     trmpcpos;          /* info2 for next posting() */
    IFPHEAD  trmifseg;          /* .ifp current segm header */
    IFPHEAD  trmifhdr;          /* .ifp 1st segm header - total psts here */
#endif
    UCHR     trmifbuf[1];       /* .ifp input buffer */
} TRMSTRU;
#endif /* CICPP */

#if LIND
#define TRMxytotp       TRMypsts
#else
#define TRMxytotp       TRMifhdr.ifptotp
#endif

#if !CICPP
#define TRMHSIZE        sizeof(TRMSTRU)                 /* one byte more */
#endif /* CICPP */

#define TRMnbytes       trmp->trmnbytes
#define TRMtype         trmp->trmtype

#define TRMdbxp         trmp->trmdbxp

#define TDBname         trmp->trmdbxp->dbxname
#define TDBifmap        trmp->trmdbxp->dbxifmap
#define TDBiflxx        trmp->trmdbxp->dbxiflxx
#define TDBiflush       trmp->trmdbxp->dbxiflush

#define TRMrc           trmp->trmrc
#define TRMkey          trmp->trmkey

#define TRMpost         trmp->trmpost
#define TRMpmfn         trmp->trmpmfn
#define TRMptag         trmp->trmptag
#define TRMpocc         trmp->trmpocc
#define TRMpcnt         trmp->trmpcnt

#define TRMl1buf        trmp->trml1buf
#define TRMl2buf        trmp->trml2buf
#define TRMlcase        trmp->trmlcase
#define TRMrcase        trmp->trmrcase
#define TRMrseek(i)     trmp->trmrseek[i]
#define TRMrsize(i)     trmp->trmrsize[i]
#define TRMliock(i)     trmp->trmliock[i]
#define TRMlidxp(i)     trmp->trmlidxp[i]

#define TRMpleft        trmp->trmpleft
#define TRMpcurr        trmp->trmpcurr
#define TRMpchar        trmp->trmpchar

#if LIND
#define TRMyaddr        trmp->trmyaddr
#define TRMypsts        trmp->trmypsts
#define TRMycase        trmp->trmycase
#define TRMysize        trmp->trmysize
#define TRMydocs        trmp->trmydocs
#define TRMydisk        trmp->trmydisk
#define TRMyread        trmp->trmyread
#define TRMyleft        trmp->trmyleft
#else
#define TRMpcblk        trmp->trmpcblk
#define TRMpcpos        trmp->trmpcpos
#define TRMifseg        trmp->trmifseg
#define TRMifhdr        trmp->trmifhdr
#endif

#define TRMifbuf        trmp->trmifbuf

#define VTRMnbytes(r)   vtrmp[r]->trmnbytes
#define VTRMtype(r)     vtrmp[r]->trmtype

#define VTRMdbxp(r)     vtrmp[r]->trmdbxp

#define VTDBname(r)     vtrmp[r]->trmdbxp->dbxname
#define VTDBifmap(r)    vtrmp[r]->trmdbxp->dbxifmap
#define VTDBiflxx(r)    vtrmp[r]->trmdbxp->dbxiflxx
#define VTDBiflush(r)   vtrmp[r]->trmdbxp->dbxiflush

#define VTRMrc(r)       vtrmp[r]->trmrc
#define VTRMkey(r)      vtrmp[r]->trmkey

#define VTRMpost(r)     vtrmp[r]->trmpost
#define VTRMpmfn(r)     vtrmp[r]->trmpmfn
#define VTRMptag(r)     vtrmp[r]->trmptag
#define VTRMpocc(r)     vtrmp[r]->trmpocc
#define VTRMpcnt(r)     vtrmp[r]->trmpcnt

#define VTRMl1buf(r)    vtrmp[r]->trml1buf
#define VTRMl2buf(r)    vtrmp[r]->trml2buf
#define VTRMlcase(r)    vtrmp[r]->trmlcase
#define VTRMrcase(r)    vtrmp[r]->trmrcase
#define VTRMrseek(r,i)  vtrmp[r]->trmrseek[i]
#define VTRMrsize(r,i)  vtrmp[r]->trmrsize[i]
#define VTRMliock(r,i)  vtrmp[r]->trmliock[i]
#define VTRMlidxp(r,i)  vtrmp[r]->trmlidxp[i]

#define VTRMpleft(r)    vtrmp[r]->trmpleft
#define VTRMpcurr(r)    vtrmp[r]->trmpcurr
#define VTRMpchar(r)    vtrmp[r]->trmpchar

#if LIND
#define VTRMyaddr(r)    vtrmp[r]->trmyaddr
#define VTRMypsts(r)    vtrmp[r]->trmypsts
#define VTRMycase(r)    vtrmp[r]->trmycase
#define VTRMysize(r)    vtrmp[r]->trmysize
#define VTRMydocs(r)    vtrmp[r]->trmydocs
#define VTRMydisk(r)    vtrmp[r]->trmydisk
#define VTRMyread(r)    vtrmp[r]->trmyread
#define VTRMyleft(r)    vtrmp[r]->trmyleft
#else
#define VTRMpcblk(r)    vtrmp[r]->trmpcblk
#define VTRMpcpos(r)    vtrmp[r]->trmpcpos
#define VTRMifseg(r)    vtrmp[r]->trmifseg
#define VTRMifhdr(r)    vtrmp[r]->trmifhdr
#endif

#if LIND
#define VTRMxytotp(r)   vtrmp[r]->trmypsts
#else
#define VTRMxytotp(r)   vtrmp[r]->trmifhdr.ifptotp
#endif

#define VTRMifbuf(r)    vtrmp[r]->trmifbuf

#endif /* CIAPI */

#define TYPETRM     21

#define RCEOF       (-1)        /*     */
#define RCNORMAL    0           /*     */
#define RCNEXT      1           /*     */

#define ABNORMAL    0           /* gdb */

/* CITRM Interface */
#if !CIAPI
#if !CICPP                      /* transferidas para a classe TRMSTRU */
#if ANSI
LONGX trmalloc(LONGX itrm, LONGX nbytes);
LONGX term(LONGX itrm, UCHR *dbnamp, UCHR *keyp);
LONGX nxterm(LONGX itrm);
LONGX posting(LONGX itrm, LONGX n);
int trmread(TRMSTRU *trmp, int treecase, UCHR **retkeyp);
N0STRU *noderead(INVMAP *invp, int treecase, int level, PUNT punt);
L0STRU *leafread(UCHR *lbufp, INVMAP *invp, int treecase, PUNT punt, int lxx);
#if !LIND
IFPSTRU *postread(UCHR *xbufp, INVMAP *invp, PUNT xblk, int lxx);
#else
IFPSTRU *postread(UCHR *xbufp, INVMAP *invp, off_t addr, unsigned int toread,
                                                                int lxx);
#endif
int cntread(DBXSTRU *dbxp, int *ifpopen, char *ifpname, int *firstcnli);
void invsetup(UCHR *dbnamp, LONGX loadn0x,LONGX loadl0x,LONGX loadpst);
void posthdr1(TRMSTRU *trmp);
void postpost(TRMSTRU *trmp);
void trminit(void);
void invflush(char *dbnamp);
INVMAP *dbxinvmp(DBXSTRU *dbxp);
void invclose(DBXSTRU *dbxp);
#else
LONGX trmalloc();
LONGX term();
LONGX nxterm();
LONGX posting();
int trmread();
N0STRU *noderead();
L0STRU *leafread();
IFPSTRU *postread();
int cntread();
void invsetup();
void posthdr1();
void postpost();
void trminit();
void invflush();
INVMAP *dbxinvmp();
void invclose();
#endif
#endif /* CICPP */
#endif /* CIAPI */

/* global */
#if !CIAPI
#if !CICPP
extern TRMSTRU *vtrmp[];        /* pointers to the entries      */
extern LONGX ntrms;              /* actual #entries              */
extern LONGX maxntrm;            /* runtime value                */
#endif /* CICPP */
#endif /* CIAPI */

#if !CICPP
extern int trmtrace;            /* trm TRSTRACE runtime switch  */
extern int trmifupd;            /* trm IFUPDATE operation */
extern unsigned char highv[];   /* trm high values              */
#endif /* CICPP */



/* *****************************************************
CIUPD.H
***************************************************** */

#if 1 /* MULTI */
#define MONONETS        0       /* single user (non network version) */
#define FULLNETS        1       /* full network version */
#define MASTNETS        2       /* restricted network support */
#endif

#define BUFFUPL         (MAXMFRL+MAXMFRL)

#define MS0ctlmfn       ms0p->m0ctlmfn
#define MS0nxtmfn       ms0p->m0nxtmfn
#define MS0nxtmfb       ms0p->m0nxtmfb
#define MS0nxtmfp       ms0p->m0nxtmfp
#define MS0mftype       ms0p->m0mftype
#define MS0reccnt       ms0p->m0reccnt
#define MS0mfcxx1       ms0p->m0mfcxx1
#define MS0mfcxx2       ms0p->m0mfcxx2
#define MS0mfcxx3       ms0p->m0mfcxx3

#define MSHmfn          mshp->m1mfn
#define MSHmfrl         mshp->m1mfrl
#define MSHmfbwb        mshp->m1mfbwb
#define MSHmfbwp        mshp->m1mfbwp
#define MSHbase         mshp->m1base
#define MSHnvf          mshp->m1nvf
#define MSHstatus       mshp->m1status

#define MS1mfn          ms1p->m1mfn
#define MS1mfrl         ms1p->m1mfrl
#define MS1mfbwb        ms1p->m1mfbwb
#define MS1mfbwp        ms1p->m1mfbwp
#define MS1base         ms1p->m1base
#define MS1nvf          ms1p->m1nvf
#define MS1status       ms1p->m1status
#define MS1dir          ms1p->m1dir

#define MSDtag(x)       ms1p->m1dir[x].tag
#define MSDpos(x)       ms1p->m1dir[x].pos
#define MSDlen(x)       ms1p->m1dir[x].len


/* CIUPD Interface */
#if !CICPP                      /* transferidas para a classe RECSTRU */
#if ANSI
#if MULTI
int recunlck(LONGX irec, int option);
#endif
int recisis0(char *dbnamp);
int recupdat(LONGX crec, LONGX irec);
int recwrite(RECSTRU *crecp, RECSTRU *recp);
int recwmast(RECSTRU *crecp, RECSTRU *recp, LONGX b, int p, int nblk, FFI wlen);
int recwxref(RECSTRU *recp, XRPTR pointer, LONGX lastmfn);
char *fldupdat(LONGX irec, char *batchp);
void recreset(char *dbnamp, LONGX mfn1, LONGX mfn2, LONGX xrec, LONGX parmtell);
#else
#if MULTI
int recunlck();
#endif
int recisis0();
int recupdat();
int recwrite();
int recwmast();
int recwxref();
char *fldupdat();
void recreset();
#endif
#endif /* CICPP */

/* global */
#if !CICPP
extern int multrace;                    /* upd MULTRACE runtime switch */
extern int recisis0_m;                  /* recisis0() init .mst */
#endif /* CICPP */


/* *****************************************************
CIUPI.H
***************************************************** */
/* CIUPI Interface */
#if ANSI
#if CICPP
int trmisis0(CISISX *cisisxp,char *dbnamp);
#else
int trmisis0(char *dbnamp);
#endif /* CICPP */
int cntwrit(DBXSTRU *dbxp);
int nodewrit(DBXSTRU *dbxp,N0STRU *n0p,int level,int isroot);
int leafwrit(DBXSTRU *dbxp,L0STRU *l0p);
#if CNV_PCBINUM
int ifpwrit(DBXSTRU *dbxp, char *buffer, LONGX nbytes, INFO hdblk[], INFO hdoff[], int hdn);
#else /* CNV_PCBINUM */
int ifpwrit(DBXSTRU *dbxp,char *buffer,LONGX nbytes);
#endif /* CNV_PCBINUM */
#else
int trmisis0();
int cntwrit();
int nodewrit();
int leafwrit();
int ifpwrit();
#endif


/* *****************************************************
CIUTL.H
***************************************************** */

#define SFLDCHR '^'

#if PC || MPE
#define OPENFCHR        '®'
#define CLOSFCHR        '¯'
#endif
#if VAX
#define OPENFCHR        '"'
#define CLOSFCHR        '"'
#endif
#if UNIX
#define OPENFCHR        '"'
#define CLOSFCHR        '"'
#endif

/* CIUTL Interface */
#if !CIAPI
#if CICPP
UBYTE *subfldp(UBYTE *fldp, UBYTE dlm, FFI *lenp);
FFI subfldn(UBYTE *sfldp, FFI len);
FFI subfield(UBYTE *fldp, FFI fldl, UBYTE dlm, UBYTE *areap);
char *loadfile(char *gidbnp, char at, char *atp, char *areap, LONGX asize, char lf2x);
char *loadstw(char *gidbnp, char *atp,char *areap,LONGX asize,int *nstws);
char *loaductb(char *gidbnp, char *uctbp, char *ucfilp);
char *loadactb(char *gidbnp, char *actbp, char *acfilp);
void decodepst(POSTSTRU *pstp, LONGX *mfnp, unsigned short *tagp,
          unsigned short *occp, unsigned short *cntp);
void encodepst(POSTSTRU *pstp, LONGX mfn, unsigned short tag,
          unsigned short occ, unsigned short cnt );
FFI bobkey(char *keyp,FFI klen,char *areap,FFI maxlen,int l,int m,int t);
#if CIAPI_SOURCE
LONGX prtifcnt  (TRMSTRU *trmp, char dbname[]);
LONGX prtcontrol(RECSTRU *recp, char dbname[]);
LONGX prtleader(RECSTRU *recp, LONGX mfn);
LONGX prtfields(RECSTRU *recp, LONGX mfn);
LONGX prtxref(CISISX *cisisxp, RECSTRU *recp, LONGX mfn);
LONGX prtdir(RECSTRU *recp, LONGX mfn);
int recfgets(CISISX *cisisxp, RECSTRU *crec, RECSTRU *irec, char line[], FFI linsiz, FILE *fpconv);
int reccopy(CISISX *cisisxp, RECSTRU *upirec,RECSTRU *upcrec,RECSTRU *recp,LONGX mfn);
unsigned LONGX showcore(char *msg);
char *recfield(CISISX *cisisxp, char *areap, RECSTRU *recp, int tag, int iocc, char *defaultp);
int fldocc(CISISX *cisisxp, RECSTRU *recp, int diridx);
#endif /* CIAPI_SOURCE */
#else /* CICPP */
#if ANSI
UBYTE *subfldp(UBYTE *fldp, UBYTE dlm, FFI *lenp);
FFI subfldn(UBYTE *sfldp, FFI len);
FFI subfield(UBYTE *fldp, FFI fldl, UBYTE dlm, UBYTE *areap);
LONGX prtifcnt  (TRMSTRU *trmp, char dbname[]);
LONGX prtcontrol(RECSTRU *recp, char dbname[]);
LONGX prtleader(RECSTRU *recp, LONGX mfn);
LONGX prtfields(RECSTRU *recp, LONGX mfn);
LONGX prtxref(RECSTRU *recp, LONGX mfn);
LONGX prtdir(RECSTRU *recp, LONGX mfn);
int recfgets(LONGX crec, LONGX irec, char line[], FFI linsiz, FILE *fpconv);
int reccopy(LONGX upirec, LONGX upcrec, RECSTRU *recp, LONGX mfn);
unsigned LONGX showcore(char *msg);
char *recfield(char *areap, LONGX ridx, int tag, int iocc, char *defaultp);
int fldocc(LONGX irec, int diridx);
char *loadfile(char *gidbnp, char at, char *atp, char *areap, LONGX asize, char lf2x);
char *loadstw(char *gidbnp, char *atp,char *areap,LONGX asize,int *nstws);
char *loaductb(char *gidbnp, char *uctbp, char *ucfilp);
char *loadactb(char *gidbnp, char *actbp, char *acfilp);
void decodepst(POSTSTRU *pstp, LONGX *mfnp, unsigned short *tagp,
                    unsigned short *occp, unsigned short *cntp);
void encodepst(POSTSTRU *pstp, LONGX mfn, unsigned short tag,
                    unsigned short occ, unsigned short cnt );
FFI bobkey(char *keyp,FFI klen,char *areap,FFI maxlen,int l,int m,int t);
#else
UBYTE *subfldp();
FFI subfldn();
FFI subfield();
LONGX prtcontrol();
LONGX prtleader();
LONGX prtfields();
LONGX prtxref();
LONGX prtdir();
int recfgets();
int reccopy();
unsigned LONGX showcore();
char *recfield();
int fldocc();
char *loadfile();
char *loadstw();
char *loaductb();
char *loadactb();
void decodepst();
void encodepst();
FFI bobkey();
#endif
#endif /* CICPP */
#endif /* CIAPI */



/* *****************************************************
CIISO.H
***************************************************** */

#define ISOBSIZ         99999L          /* wrn: is LONGX */    /* AOT, 06/06/2005 - length[5] */

#define ISOLLEF         4
#define ISOLSCP         5
#define ISOXFS          '#'             /* 0x1E = 30 para Minisis */
#define ISOXGS          '#'             /* 0x1D = 29 para Minisis */
#define ISOXFSMARC      0x1E            /* 0x1E = 30 para Minisis and Marc */   /* AOT, 10/06/2005 */
#define ISOXGSMARC      0x1D            /* 0x1D = 29 para Minisis and Marc */   /* AOT, 10/06/2005 */
#define ISOXXSMARC      0x23            /* subfield  para Minisis and Marc */   /* AOT, 10/06/2005 */

#if CICPP
class _YOURCLASS ISO_2709;       /* Para substituir def da struct */
#endif /* CICPP */

typedef struct isohstru {
    unsigned char length[5];
    unsigned char status[1];
    unsigned char icodes[4];
    unsigned char indlen[1];
    unsigned char sublen[1];
    unsigned char basead[5];
    unsigned char forusr[3];
    unsigned char lenlef[1];
    unsigned char lenscp[1];
    unsigned char forfut[2];
} ISOHSTRU;

typedef struct isodstru {
    unsigned char tagfld[3];
    unsigned char lenfld[ISOLLEF];
    unsigned char locfld[ISOLSCP];
} ISODSTRU;

#define ISOHSIZE       sizeof(ISOHSTRU)
#define ISODSIZE       sizeof(ISODSTRU)

#if !CICPP
extern int iso_ismarc;
#endif /* CICPP */

/* CIISO Interface */
#if !CICPP
#if ANSI
int iso_open(char *gidbnp, unsigned char filnam[], int lrecb, int lrfix, int isfix);
int iso_read(LONGX crec, LONGX irec, unsigned int ldrtag);
int iso_creat(char *gidbnp, unsigned char filnam[], int lrecb, int lrfox, int isfox);
int iso_write(RECSTRU *recp, unsigned char isoxfs, unsigned char isoxgs, unsigned int ldrtag);
int iso_close(void);
int iso_rclos(void);
#else
int iso_open();
int iso_read();
int iso_creat();
int iso_write();
int iso_close();
int iso_rclos();
#endif
#endif /* CICPP */



/* *****************************************************
CIB7.H
***************************************************** */

#if CIB71

#ifndef CIB7HITLIST
#define CIB7HITLIST     1
#endif
#ifndef CIB7TMPNULL
#define CIB7TMPNULL     1
#endif

#if BEFORE20020127
#define MAXVOPR         512
#else                         /* 16K for MeSH/Drug category */
#define MAXVOPR         16384 /* 16K */  /* AOT, 29/01/2002 */
#endif                        /* 16K x sizeof(char)+sizeof(char)+sizeof(char*) */
#define MAXQUAL         20
#define MAXPDLS         32
#define MAXERRL         60

#define ZORX  '+'
#define ZAND  '*'
#define ZANN  '^'
#if !LIND
#define ZANG  'G'
#define ZANF  'F'
#define ZANP  '.'
#define ZANE  '$'
#endif
#define ZASS  '='
#define ZLDV  '_'
#define ZPFX  '['
#define ZQUA  '/'

#define TOKNULL         0
#define TOKDELIM        1
#define TOKTERM         2

#define TBOOL           1

#define MFQTDBN0        11
#define MFQTQRY0        12
#define MFQTOBJ0        13

#define MFQTXCTS        20

#define MFQSWTIM        't'
#define MFQSWDBN        'd'
#define MFQSWQTY        'q'

#define MFQTASET        21
#define MFQTALCC        22

#define MFQTHITS        31
#define MFQTHCUR        32
#define MFQTHMFN        33
#define MFQTHSIZ        34
#define MFQTHMEM        35

#define OPRNULL         0x01
#define OPRXEND         0x02
#define PSTLEVEL        0x03
#define ACCLEVEL        0x04

#define B7UPLEN         9               /* length for b40 numeric fields */
#define B7DELADD        "D%dA%d|%09ld|" /* sprintf format for dir/field */
#define B7EDIT1         "%09ld"         /* sprintf format for field edit */

#define MFQTXTAG        101             /* inverted files prefixes */

#define B7RE_X2         1
#define B7RE_X3         2
#define B7RE_X1         3
#define B7RE_53         4
#define B7RE_1          5
#define B7RE_2          6
#define B7RE_3          7
#define B7RE_4          8
#define B7RE_10         9
#define B7RE_18         10
#define B7RE_28         11
#define B7RE_PFX        12
#define B7RE_SET        13

#define B7EE_X2         2
#define B7EE_PFX        11


/* estructura de la pila de ejecución */
typedef struct pdlstru {
   UCHR        *pdllvel;      /* bitstring, contiene la informacion de los postings. */
   LONGX        pdlleft;    /* cuanto queda del lvel */
   LONGX        pdldocs;    /* cantidad de documentos almacenados */
   LONGX        pdllmfn;    /* valor del último MFN de la BD */
   LONGX        pdlsize;    /* tamaño actual de lvel */
   LONGX        pdlisiz;    /* tamaño inicial de lvel, also growing step */
   short int   pdlplen;    /* tamaño de los postings */
   char        *pdlnxtp;      /* proximo slot libre en lvel */
} PDLSTRU;
/*
   si plen == 0
      se usa un bit per MFN. * + ^
   sino
      3 bytes para MFN
      2 bytes para TAG  ==> 5 oper (G)
      1 byte  para OCC  ==> 6 oper (F)
      2 bytes para CNT  ==> 8 oper . $

*/


#if CIB7HITLIST /* AOT 15/01/2002 */

#define B7HITLISTLONG   1
#define B7HITLISTSHORT  2

typedef struct b7hitbuf {
    LONGX setno;                         /* query number */
    LONGX bufsiz;                        /* buffer size */
    LONGX ndocs;                         /* number of docs */
    int  hcase;                         /* BITSTRING or MFNSTRING (4 bytes) */
    int  htype;                         /* list of LONGX or unsigned short mfn's */
    LONGX align4;                        /* 4-byte alignment */
    UBYTE area[1];                      /* buffer - via ALLOC() */
} B7HITBUF;

typedef struct b7hitlist {
    LONGX mfnlist[1];                    /* array of mfn */
} B7HITLIST;

typedef struct b7hitlistshort {
    unsigned short int mfnlist[1];      /* array of mfn - short */
} B7HITLISTSH;

#else /* CIB7HITLIST */

typedef struct b7hitbuf {
    LONGX setno;                         /* query number */
#if BEFORE20010607
    LONGX block;                         /* block number */
#endif
    LONGX bufsiz;                        /* buffer size */
    UBYTE area[1];                      /* buffer - via ALLOC() */
} B7HITBUF;
#endif /* CIB7HITLIST */


typedef struct
{
/*variables globales ... */

/*cib71.c */
#if BOLTRACE
#define DISTRACE 1
   char *b7objp;
#endif

   int and2gf;
   char *subst_and;
   LONGX b7setno1;
   LONGX b7setno;
   char *b7batchp;                 /* fldupdat */
   RECSTRU *b7recp;                /* current recp */

#if FATRAP
/* error */
   jmp_buf b71jumper;
#endif

   int b71error;
   char b7errxy[MAXERRL+1];       /* static */

/* gettoken */
   char *tokbackp;
   char *toknextp;
   char *token;
   char toktyp;
   char tokopr;
   int tokqty;
   int toklen;

   char *tokqualp;                 /* qualifiers */
   int tokquall;                   /* length */
   int tokqualn;                   /* no of elements in the list */

   char *tokmassp;                 /* toknextp after b7_massp() */

/* expressoes */
   int nb7oprs;

/*cib72 */
   unsigned char mask;
   char *b7_gidbnp;
   PDLSTRU *vpdlp[MAXPDLS];
   int npdls;
   LONGX b7tell;
   LONGX b7itrm;
   int b7fd;
   int b7error;
   B7HITBUF *b7bufferp;

   char b7_pfxdbn[CIMPL+1];
   char b7_pfxopx[LE2+LE2+1];
   FFI b7_pfxlen;
   char b7_pfxmsg[LE2+1];
   char b7_pfxtwx[CIMPL+1];
   FFI b7_pfxmdl;

#if FIXTRACE
   int xtrac;
#endif

   char vetopr[MAXVOPR],vetcas[MAXVOPR],*vetopp[MAXVOPR];
   int nop;

#if !TLS
   void *ciapip;
#endif
   LONGX smfn;
   LONGX sirec;

   short unsigned int nqualts,vqualt[MAXQUAL],*qualtp;

   int b7rootmsg;

   int b7tw2mh;

} b7_CIB7;

#if !CIAPI && !CIAPI_SOURCE
extern int b70trace;            /* b70 RUXTRACE runtime switch */
#endif /* !CIAPI && !CIAPI_SOURCE */


/* --------------------- cib70.h --------------------- */

/* CIB71 Interface */
#if ANSI
char *b7_exp(b7_CIB7 *cib7p, LONGX irec, char *dbnamp, char *qryp,
               char *buffup, char *qrydbnp, LONGX crec, int *errnop);
char *b7_run(b7_CIB7 *cib7p, LONGX irec,char *dbnamp,char *buffup, int buffuplen,
            LONGX tell,unsigned char uctab[],char *btch0p,
            LONGX tlirecp,LONGX crec,int *errnop);
LONGX b7_hit(b7_CIB7 *cib7p, LONGX irec, LONGX nord, LONGX setno);
int b7_hballoc(b7_CIB7 *cib7p, LONGX bufsiz, B7HITBUF **bufferpp);
int b7_hbinit(b7_CIB7 *cib7p, LONGX setno, B7HITBUF *bufferp);
int b7_hbfree(b7_CIB7 *cib7p, B7HITBUF **bufferpp);
#else /* ANSI */
char *b7_exp();
char *b7_run();
LONGX b7_hit();
int b7_hballoc();
int b7_hbinit();
int b7_hbfree();
#endif /* ANSI */
#else /* CIB71 */


/* *****************************************************
CIB40.H
***************************************************** */

#if !LIND && !CIB64

#define BX0CDROM        1       /* Bx0 is a bit more user friendly */
#define SHOWCORE        1       /* 1 to show coreleft if b40trace is on */

#define MAXVOPR         512
#define MAXQUAL         20
#define MAXPDLS         32
#define MAXERRL         60

#define ZORX  '+'
#define ZAND  '*'
#define ZANN  '^'
#if !LIND
#define ZANG  'G'
#define ZANF  'F'
#endif
#define ZANP  '.'
#define ZANE  '$'
#define ZASS  '='
#define ZLDV  '_'
#define ZQUA  '/'

#define TOKNULL         0
#define TOKDELIM        1
#define TOKTERM         2

#define TBOOL           1

#define OPRNULL         0x01
#define OPRXEND         0x02
#define PSTLEVEL        0x03
#define ACCLEVEL        0x04

#define MFQTDBN0        11
#define MFQTQRY0        12
#define MFQTOBJ0        13

#define MFQTXCTS        20

#define MFQSWTIM        't'
#define MFQSWDBN        'd'
#define MFQSWQTY        'q'

#define MFQTASET        21
#define MFQTALCC        22

#define MFQTHITS        31
#define MFQTHCUR        32
#define MFQTHMFN        33
#define MFQTHSIZ        34
#define MFQTHMEM        35

#define MFQXHMFNx       'C'
#define MFQXHTAGx       'A'
#define MFQXHOCCx       'S'
#define MFQXHCNTx       'E'


#define B4UPLEN         6               /* length for b40 numeric fields */
#define B4DELADD        "D%dA%d|%06ld|" /* sprintf format for dir/field */
#define B4EDIT1         "%06ld"         /* sprintf format for field edit */

#define MFQTXTAG        101             /* inverted files prefixes */

typedef struct irxstru {
    LONGX     irxbytes;          /* irx bytes */
    LONGX     irxhits;
    char     irxarea[1];        /* irx area  */
} IRXSTRU;


/* CIB40 Interface */
#if ANSI
char *b4_exp(LONGX irec, char *dbnamp, char *qryp,char *qrydbnp, int *errnop);
char *b4_run(LONGX irec, char *dbnamp, LONGX nbytes1, LONGX nbytes2, LONGX tell,
                unsigned char uctab[], LONGX tlirec, int *errnop);
LONGX b4_hit(LONGX irec, LONGX nhit);
int b4_msg(int tline, char *msg, LONGX psts, LONGX docs, int tmsg);
int b4_kbh();
char *b4_massp(char *tken, int tklen,
                                char *tkqualp, int tkquall, char *tknextp);
char *b4_xlkey(char *opx, char *bufp, unsigned char uctab[], char *xlfilp,
                                                                int *opxlp);
#else
char *b4_exp();
char *b4_run();
LONGX b4_hit();
int b4_msg();
int b4_kbh();
char *b4_massp();
char *b4_xlkey();
#endif

/* global */
#if !CICPP
extern int b40trace;
extern char b4_tmpnam[];
extern LONGX b4setno;
/* extern ...;  */
#endif /* CICPP */

#endif /* CIB40 */


/* *****************************************************
CIB50.H
***************************************************** */

#if LIND || CIB64

#define ERRTRACE        0
#define BX0CDROM        1       /* Bx0 is a bit more user friendly */
#if CICPP
#define SHOWCORE        0       /* 1 to show coreleft if b50trace is on */
#else /* CICPP */
#define SHOWCORE        1       /* 1 to show coreleft if b50trace is on */
#endif /* CICPP */
#define MAXPDLS         32
#define MAXERRL         81

#define ZORX  '+'
#define ZAND  '*'
#define ZANN  '^'
#define ZLDV  '_'
#define ZPFX  '['
#define ZPDL  '{'

#define TOKNULL         0
#define TOKDELIM        1
#define TOKTERM         2

#define TBOOL           1

#define OPRNULL         0x01
#define OPRXEND         0x02
#define PSTLEVEL        '1'
#define ACCLEVEL        '1'
#define FLDUPXB5        0x05

#define MFQTDBN0        11
#define MFQTQRY0        12
#define MFQTOBJ0        13

#define MFQTXCTS        20

#define MFQSWTIM        't'
#define MFQSWDBN        'd'
#define MFQSWQTY        'q'

#define MFQTASET        21
#define MFQTALCC        22

#define MFQTHITS        31
#define MFQTHCUR        32
#define MFQTHMFN        33
#define MFQTHSIZ        34
#define MFQTHMEM        35

#define MFQTSNAM        36

#define MFQTHITX        41              /* CDUSP */

#define B5UPLEN         8               /* length for b50 numeric fields */
#define B5DELADD        "D%dA%d|%08ld|" /* sprintf format for dir/field */
#define B5EDIT1         "%08ld"         /* sprintf format for field edit */

#define MFQTXTAG        101             /* inverted files prefixes */

#define B5RE_X2         1
#define B5RE_X3         2
#define B5RE_X1         3
#define B5RE_53         4
#define B5RE_1          5
#define B5RE_2          6
#define B5RE_3          7
#define B5RE_4          8
#define B5RE_10         9
#define B5RE_18         10
#define B5RE_28         11
#define B5RE_PFX        12
#define B5RE_SET        13

#if ! CICPP
#if BIGREC
#else /* BIGREC */
typedef struct b6hitbuf {
    LONGX setno;                         /* query number */
    LONGX block;                         /* block number */
    LONGX bufsiz;                        /* buffer size */
    UBYTE area[1];                      /* buffer - via ALLOC() */
} B6HITBUF;
#endif /* BIGREC */
#endif /* CICPP */

/* CIB50 Interface */
#if !CICPP
#if ANSI
char *b5_exp(LONGX irec, char *dbnamp, char *qryp,
                char *buffup, char *qrydbnp, LONGX crec, int *errnop);
#if BIGREC
char *b5_run(LONGX irec, char *dbnamp,
                char *buffup, LONGX nbytes1, LONGX tell,
                unsigned char uctab[], char *btch0p, LONGX tlirec, LONGX crec,
                int *errnop);
LONGX b5_hit(LONGX irec, LONGX nhit, char *buffup);
#else /* BIGREC */
char *b6_run(LONGX irec, char *dbnamp,
                char *buffup, int buffuplen, LONGX nbytes1, LONGX tell,
                unsigned char uctab[], char *btch0p, LONGX tlirec,
      LONGX crec, int *errnop);
LONGX b6_hit(LONGX irec, LONGX nhit, LONGX setno, B6HITBUF *bufferp);
int b6_hballoc(LONGX bufsiz, B6HITBUF **bufferpp);
int b6_hbinit(LONGX setno, B6HITBUF *bufferp);
int b6_hbfree(B6HITBUF **bufferpp);
#endif /* BIGREC */
int b5_msg(int tline, char *msg, LONGX docs, int tmsg);
int b5_kbh(void);
char *b5_massp(char *tken, int tklen, int b5ee_pfx);
void b5_experr(int error, char *errp, int errl);
int b50read(DBXSTRU *dbxp,LONGX mfn,int tag,int occ,char *areap,LONGX *areasizp);
int b5_pfx(LONGX irec, UWORD invxtag, char *dp, char *op, int b5re_pfx);
int b5_nxtoken(void); /* now return toktype */
#else /* ANSI */
char *b5_exp();
#if BIGREC
char *b5_run();
LONGX b5_hit();
#else /* BIGREC */
char *b6_run();
LONGX b6_hit();
int b6_hballoc();
int b6_hbinit();
int b6_hbfree();
#endif /* BIGREC */
int b5_msg();
int b5_kbh();
char *b5_massp();
void b5_experr();
int b50read();
int b5_pfx();
int b5_nxtoken();
#endif /* ANSI */
#endif /* CICPP */

/* global */
#if !CICPP
extern char *toknextp;          /* b5_nxtoken input data */
extern int toklen;              /* token length */
#if BEFORE970507
extern char *token;             /* token addr */
extern int toktyp;              /* token type - return by b5_nxtoken() */
#endif

extern LONGX b5setno;            /* query set number */
extern char b53_tmpnam[];       /* tmp file name for b5_run() */
extern int b50trace;            /* b50 RUXTRACE runtime switch */
extern DBXSTRU *b5_savdbxp;     /* '@' processing for b5_run() */
extern LONGX b5_isn1;            /* query #0 lower limit */
extern LONGX b5_isn2;            /* query #0 upper limit */

#if BIGREC
#else /* BIGREC */
extern B6HITBUF *b6bufferp;     /* b6_run() processing buffer */
#endif /* BIGREC */

#endif /* CIB50 */
#endif /* CICPP */

#endif /* CIB71 */


/* *****************************************************
Current master file record & inverted file term
***************************************************** */

/* CIAOT Definitions */

/* Current master file record */

#if !CIAPI
#if !CICPP                      /* nao existe mais prateleira de registros */
#define RECORD(irec,dbn,mfn)    recp=vrecp[(unsigned)record(irec,dbn,mfn)]
#define RECALLOC(irec,n)        recp=vrecp[(unsigned)recallok(irec,n)]
#endif /* CICPP */
#endif /* CIAPI */

/* Current inverted file term (and postings) */

#if !CIAPI
#if !CICPP                      /* nao existe mais prateleira de termos */
#define TERM(itrm,dbn,key)      trmp=vtrmp[(unsigned)term(itrm,dbn,key)]
#define NXTERM(itrm)            trmp=vtrmp[(unsigned)nxterm(itrm)]
#define TRMALLOC(itrm,n)        trmp=vtrmp[(unsigned)trmalloc(itrm,n)]
#endif /* CICPP */
#endif /* CIAPI */



/* *****************************************************
CIFMT.H
***************************************************** */

/* CDS/ISIS Formatting Language interface */

#if !EXCFMXML
#ifndef CI_XMLELEM
#define CI_XMLELEM 0 //1   v5.2 - AOT, 28/12/2005
#endif
#endif

typedef enum ins {
   dummy, jumpf, jumpt, jump, pre_cond, pre_r_lit, pre_r_lit_plus,
   suf_cond, suf_cond_null, suf_r_lit, suf_r_lit_plus,
   set_true_rep, set_false_rep, test_rep, begin, end,

   beg_init_not_rep,
   end_init_not_rep,
   beg_init_rep,
   end_init_rep,

   begin_rep_gr, end_rep_gr, test_occ, print_field, d_dummy, n_dummy,
   load_field_all, load_field_occ, load_number, load_string,
   x_spac, c_spac, slash_spac, n_sigx_spac, n_sign_spac, percent_spac,
   mpl_par, mpu_par, mhl_par, mhu_par, mdl_par, mdu_par,
   escape_seq, u_cond, print_mfn, load_mfn, absent, present, contains,
   neq_op, lss_op, gtr_op, leq_op, geq_op, eql_op, or_op, and_op, not_op,
   plus_op, minus_op, times_op, divide_op,
   init_if, end_then, end_else, fi, val_beg, val_end,
   print_core, print_maxmfn, load_core, load_maxmfn,
   rupx_beg, rupx_end, rupd_beg, rupd_end, syst_beg, syst_end,
   date_mktime_beg, date_mktime_end, /* AOT */
   rsum_beg, rsum_end, rmax_beg, rmax_end,
   rmin_beg, rmin_end, ravr_beg, ravr_end,
   l_beg, l_end, s_beg, s_end, fmt_beg, fmt_end, f_beg, f_end, str_cat,
   getenv_beg, getenv_end,
   putenv_beg, putenv_end,
   prt_str_f,
   ref_beg, ref_end,
   refu_beg,refu_end, /* Ref aparecendo com funcao */
   read_mfn,
   update_fld, input_fld, lw_beg, lw_end,
   dbname_beg, dbname_end,load_float,
   noccins,
   np_beg,np_end,
   ioccins,
   continueins,breakins,
   size_beg, size_end,
   type_beg, type_end,
   load_mstnam, /* print_mstnam, */
   load_date, /* print_date, */
   duptop,  /* duplicar topo pilha */
   end_select,
   datex_beg,datex_end, /* AOT */
   instr_beg, instr_end,
   left_beg,left_end,
   right_beg,right_end,
   mid_beg,mid_end,
   cat_beg, cat_end,
        replac_beg,replac_end,
#if CI_XMLELEM
        xmlelem_beg,xmlelem_end,
#endif
   nl_beg,nl_end,
        ign_cond, /*12-03-99 ignore conditional lits without field selector */
      while_beg,while_end,
      occins,
      eattrib_beg,eattrib_end,sattrib_beg,sattrib_end,
      intvvalue,strvvalue,
      ss_beg,ss_end,
      citype_beg,citype_end,
   zzzzzz
} instruction_code;

typedef int label;

typedef struct intermediate_code {
   label  lab;
   instruction_code  instr;
   LONGX  add;
} pgm_node;

#if CICPP
class _YOURCLASS FMTSTRU;       /* Para substituir def da struct */
#endif /* CICPP */

#if CIAPI
typedef void FMT_CODE;
#else /* CIAPI */
typedef struct lista {
    pgm_node info;
    LONGX  m_add;
    LONGX  next;
} FMT_CODE;
#endif /* CIAPI */


/* CIFMT Interface */

#if !CIAPI

#if CIAPI_SOURCE
LONGX recfmt( RECSTRU* recp, LONGX lw, char *fmt, char *area, LONGX asize);
LONGX recfmtcmp(RECSTRU* recp, LONGX lw, char *fmt, char *area, LONGX asize,
                    FMTSTRU **ptpgm, int flagfree);
#endif /* CIAPI_SOURCE */

#if !CICPP
#if ANSI
LONGX recfmt(   LONGX irec, LONGX lw, char *fmtp, char *areap, LONGX asize);
LONGX recfmtcmp(LONGX irec, LONGX lw, char *fmtp, char *areap, LONGX asize,
                    FMT_CODE **pgmpp, int flagfree);
LONGX fmt_gener(FMT_CODE **pgmpp, char *fmtp);
LONGX fmt_inter(FMT_CODE  *pgmp, LONGX irec, LONGX lw, char *areap, LONGX asize);
void fmt_free( FMT_CODE  *pgmp);
#else
LONGX recfmt();
LONGX recfmtcmp();
LONGX fmt_gener();
LONGX fmt_inter();
void fmt_free();
#endif
#endif /* CICPP */

/* global */
#if !CICPP
extern int fmttrace;
extern LONGX fmt_error; /* error - code */
extern LONGX fmt_errof; /* error - fmt spec offset to error */
extern LONGX fmt_fsiz;  /* fmt_inter() - to set max field length */
#endif /* CICPP */

#endif /* !CIAPI */


/* *****************************************************
CIFST.H
***************************************************** */

#if CICPP
#if USE_GDBFMT
#define FMT_CODX char
#else /* USE_GDBFMT */
#define FMT_CODX char /*FMTSTRU */
#endif /* USE_GDBFMT */
#else
#define FMT_CODX FMT_CODE
#endif /* CICPP */

#if CICPP
class _YOURCLASS FSTSTRU;       /* Para substituir def da struct */
#endif /* CICPP */

typedef struct fst_code {
    UWORD tag;
    int it;
    FMT_CODX *fmtp;
    struct fst_code *nextp;
} FST_CODE;

typedef struct link1 {
    UCHR key[LE1];
    POSTSTRU post;
} LINK1;

typedef struct link2 {
    UCHR key[LE2];
    POSTSTRU post;
} LINK2;

#define FSTERR1 1
#define FSTERR2 2
#define FSTERR3 3
#define FSTERRF 4

/* CIFST Interface */

#if !CICPP
#if ANSI
LONGX recfst(   LONGX irec, char *fstspecp,
                             char *area1p, LONGX max1,
                             char *area2p, LONGX max2,
                             LONGX *qty1, LONGX *qty2);
LONGX recfstcmp(LONGX irec, char *fstspecp,
                             char *area1p, LONGX max1, char *area2p,
                             LONGX max2, LONGX *qty1, LONGX *qty2,
                             FST_CODE **pgmpp, int flagfree);
LONGX fst_gener(FST_CODE **pgmpp, char *fstspecp);
LONGX fst_inter(FST_CODE  *pgmp, LONGX irec, char *stwp,
         char **area1pp, LONGX max1,
         char **area2pp, LONGX max2,
         LONGX *qty1p, LONGX *qty2p);
void fst_free( FST_CODE  *pgmp);
int fst_link(char *pfxp, int plen, char *keyp, int klen,
         LONGX mfn, UWORD tag, UWORD occ, UWORD cnt,
         char *link1, char *link2, LONGX *lft1, LONGX *lft2);
int fst_writ(int treecase, LONGX mfn, UWORD tag, UWORD occ, UWORD cnt,
         char *kp, int kl, UCHR *uctabp);
int fst_open(char *gidbnp, char *filnamp, int treecase);
int fst_clos(int treecase);
void pstdecod(POSTSTRU *pst, LONGX *mfn, UWORD *tag, UWORD *occ, UWORD *cnt);
void prtlink(char *sp, LONGX mfn, UWORD tag,UWORD occ,UWORD cnt, UCHR *keyp, int len);
#else
LONGX recfst();
LONGX recfstcmp();
LONGX fst_gener();
LONGX fst_inter();
void fst_free();
int fst_link();
int fst_writ();
int fst_open();
int fst_clos();
void pstdecod();
void prtlink();
#endif

#endif /* CICPP */

/* global */
#if !CICPP
extern int fsttrace;    /* fst FSSTRACE runtime switch */
#endif /* CICPP */
#if !CICPP
extern LONGX fst_error;  /* gener - fst spec error */
extern LONGX fst_errl;   /* gener - fst spec line where error occured */
extern char *fst_errp;  /* gener - fst spec ptr where error occured */
extern char *fst_fmtap; /* inter - static format area */
extern LONGX fst_fmtby;  /* inter - bytes for dynamic format area */
extern char fst_rdlm;   /* inter - new occ delimiter */
extern int fst_fd[];    /* inter - file descriptor for .ln1/.ln2 */
extern LONGX fst_fx[];   /* inter - output records to .ln1/.ln2 */
extern int cifstfix;    /* inter - key %8ld %5d %4d %4d */
extern int cifstfim;    /* inter - key %8ld w/ cifstfix */
extern char *fst_batchup; /* inter - batchp for fldupdat() area */
extern char *fst_batchp;/* inter - batchp for fldupdat() commands */
extern int fst_batch0;  /* inter - batchp ^mmfn^oocc^ccnt^l1/2 option */
extern char *fst_hdrp;  /* inter - hit file headers - area */
extern int fst_hdrx;    /* inter - hit file headers - #bytes allocated */
extern int fst_hdru;    /* inter - hit file headers - #bytes stored */
extern int fst_hdrt;    /* inter - hit file headers - for area+^m^t^o^c */
extern char *fst_wlupifnp; /* inter - word lookup I/F for TW processing */
extern TRMSTRU *fst_wluptrmp; /* inter - word lookup I/F for TW processing */
extern LONGX fst_wlupitrm; /* inter - word lookup I/F for TW processing */
#endif /* CICPP */


/* *****************************************************
CIIFx.H
***************************************************** */

#if IFUPDATE
#define IFUPDICT     0
#define IFUPISIS     1

#define IFUPCORE     0
#define IFUPWRIT     1
#define IFUPCLOS     2

#define IFUPDXUP     1
#define IFUPDXLD     2

#define MAXIFMRG     1024 /* svdifmerg() */  /* AOT 22/06/2001 */
#endif

/* CIIFx Interface */

#if CICPP
class _YOURCLASS CIIFU;       /* Para substituir def da struct */
class CIIFL;       /* Para substituir def da struct */
#endif /* CICPP */

#if !CICPP
#if ANSI
#if  BEFORE20000926
int upif_end(int pstflag);
#else /* BEFORE20000926 */
int upif_end(char *dbnp, int pstflag);
void upif_save(char *dbnp);
void upif_restore(char *dbnp);
#endif /* BEFORE20000926 */
void upif_init(char *dbnp);
#if LIND
LONGX svdiflind(char *dbnp,char *filekeys_1,char *filekeys_2,
                  LONGX maxmfn,char *logp,
                  int pstflag,LONGX tell);
#else
LONGX svdifload(char *dbnp,char *filekeys_1,char *filekeys_2,
                  int pstflag,LONGX tell);
#endif
LONGX svdifmerg(char *dbnp,char *vifnamp[],LONGX vifmfns[],
               LONGX parmxmfn,char *parmlogp,int pstflag,LONGX tell);
int ifupdat(char *mdbnp,LONGX mfn1,LONGX mfn2,char *idbnp,FST_CODE *fstp,
         char *stwp,LONGX maxlk1,LONGX maxlk2,int pstflag,int endup);
void svdifupd(char *lnk1p,LONGX qtylk1,char *lnk2p,LONGX qtylk2,
         char *olnk1p,LONGX oqtylk1,char *olnk2p,LONGX oqtylk2,
         int pstflag,LONGX tell);
#else
int upif_end();
void upif_init();
#if LIND
LONGX svdiflind();
#else
LONGX svdifload();
#endif
LONGX svdifmerg();
void svdifupd();
int ifupdat();
#endif /* ANSI */
#endif /* CICPP */

/* global */
#define CIIFLFIX        1
#if !CICPP
#if CIIFLFIX
extern int ciiflfix;    /* svdifload/lind() - key %8ld %5d %4d %4d */
extern int ciiflfim;    /* svdifload/lind() - key %8ld w/ ciiflfix */
#endif

extern int ifupd_reset;    /* ifupdat() reset IfUpdPend flags */
extern int ifupd_wrnmsg;   /* ifupdat() warning messages */
#endif /* CICPP */

/* see DBXi.... */



/* *****************************************************
ISISUCTAB & others
***************************************************** */

#if CICPP
#else /* CICPP */
#if CIAPI
#else /* CIAPI */
/* tabelas de mxaot.c
*/
extern unsigned char isisuctab[];
extern int isiswctot;
extern unsigned char isiswctab[];
extern unsigned char isisactab[];
extern unsigned char ansimctab[]; /* convert=ansi */ /* asc2ansi */
extern unsigned char arabmctab[]; /* convert=arab */ /* 708_1256 */
extern unsigned char ansiuctab[];
extern unsigned char ansiactab[];

extern int NXPAGES[];           /* AOT, 21/02/2001 */
#endif /* CIAPI */
#endif /* CICPP */



/* *****************************************************
MXAOT & others utilities
***************************************************** */
#if 1 /* !CIAPI_SOURCE */
#ifndef ISIS_DLL
#ifndef MXFUN
#define MXFUN       1              /* AOT, 21/02/2001 */
#endif
#ifndef IFLOADFUN
#define IFLOADFUN   1              /* AOT, 21/02/2001 */
#endif
#endif /* ISIS_DLL */
#if PC
#ifndef MYSFUN
#define MYSFUN      1              /* AOT, 21/05/2001 */
#endif
#else
#ifndef MYSFUN
#define MYSFUN      0              /*                 */
#endif
#endif
#endif /* !CIAPI_SOURCE */

/* MXAOT Interface */
int mxexec(int argc,char *argv[]);
#if MXFUN
int cisis_mx(char *stringp);
#endif /* MXFUN */

/* IFLOADAOT Interface */
int ifloadexec(int argc,char *argv[]);
#if IFLOADFUN
int cisis_ifload(char *stringp);
#endif /* MXFUN */


/* MYSAOT Interface */
#if MYSFUN
int cisis_mysfunc(char *gidbnp, int parmlen, char *parmin, char *parmout, LONGX parmtell);
#endif /* MYSFUN */


/* *****************************************************
   Funzioni di Conversione
   Original code by Beppe Romano (quadri.nis.garr.it)
***************************************************** */

#if CNV_PCBINUM
#if ANSI
int ConvertMST_CTLSTRUCT(char *buffer);

int ConvertMST_LEADER(char *buffer, int skipval, int breakval);
int ConvertMST_DIR(char *buffer, int nvf);

int ConvertXRF_REC(char *buffer);
int ConvertXRF_PTR(char *buffer);

int ConvertCNT_REC(char *buffer);      /* changed (1 .cnt rec) */
int ConvertN01_REC(char *buffer);
int ConvertN02_REC(char *buffer);
int ConvertL01_REC(char *buffer);
int ConvertL02_REC(char *buffer);
int ConvertIFP_BLKCTL(char *buffer, int blk1);
int ConvertIFP_PSTHDR(char *buffer);
int ConvertBuffer(char *buffer, int fieldlength);
#else
int ConvertL02_REC();
int ConvertN02_REC();
int ConvertL01_REC();
int ConvertN01_REC();
int ConvertMST_CTLSTRUCT();
int ConvertMST_LEADER();
int ConvertMST_DIR();
int ConvertCNT_REC();
int ConvertXRF_PTR();
int ConvertXRF_REC();
int ConvertIFP_BLKCTL();
int ConvertIFP_PSTHDR();
int ConvertBuffer();
#endif
#endif /* CNV_PCBINUM */


#if CICGI

#if PC
#define CICGI_VALSIZE           (MAXMFRL/8)
#define CICGI_NAMESIZE          (MAXMFRL/8)
#define CICGI_MAXENTRIES        256 /* 64 */
#define CICGI_WORDSIZE          (MAXMFRL/8)
#else
#define CICGI_VALSIZE           MAXMFRL
#define CICGI_NAMESIZE          MAXMFRL
#define CICGI_MAXENTRIES        4096 /* 1024 */
#define CICGI_WORDSIZE          MAXMFRL
#endif

typedef struct {
    char *namp;
    char *valp;
} CICGI_ENTRY;

#if CICPP
class _YOURCLASS CGIClass;       /* Para substituir def da struct */
#endif /* CICPP */

#if !CICPP
#if ANSI
int cicgi0(int *argc, char *argv[], char **wwwp, UWORD taghxsub, char *taghpfxp);
int cicgi1(int *argc, char *argv[], CICGI_ENTRY entries[], int nentries, char *cicgi_p);
#else
int cicgi0();
int cicgi1();
#endif

extern int cgitrace; /* extern */
#endif /* CICPP */

#endif /* CICGI */



#include "cihsh.h"      /* CISIS Interface hashing & binary table search */
#if CIWTF                                /* WTFUN support */
/* header files */
#include <math.h>
#include <time.h>
//#include "cihsh.h"      /* CISIS Interface hashing & binary table search */
#include "wt.h"                 /* //MAXNREC; typedef VECTEL; typedef LISTA */
#include "wtfun.h"              /* //typedef WTFUN_XWT / WTFUN_ARRAY */
/* global */
extern WTFUN_ARRAY *ciawtfp;    /* //global WTFUN_ARRAY */
extern char *ciahcgi;           /* //global H2000 cgi data */
#endif /* CIWTF */

#if GCC
#define PGCC  1
     #include <sys/socket.h>
     #include <netinet/in.h>
     #include <netdb.h>
     #include <arpa/inet.h>  /* inet_ntoa */
#else
#define uint16_t unsigned short
//   #include <wsksock.h>
#endif

/* ciupdsocy.c / ciupdsocx.c prototypes */
int mainfile   (int cmd, char *filnamp, int parm0, int parm1, char *buffer, int buffersize);                                         // in ciupdsocy.c
int mainclient (int cmd, char *protocol, char *serverhost, uint16_t port, char *message, char *buffer, int buffersize, int maxrds);  // in ciupdsocx.c



/* *****************************************************
CISIS/C++ extern "C"
***************************************************** */
#if CICPP
#ifdef __cplusplus
}
#endif
#endif /* CICPP */

#endif /* CISIS_H */
