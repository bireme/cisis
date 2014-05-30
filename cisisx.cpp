///******************************************************
//CISISX.CPP
//******************************************************/

#define CISISX_SOURCE

//#include <cisisx.hpp>
#include <ciiso.hpp>
#include <cirun.hpp>
#include <string.h>

#include <cisisx.c>

#if CICPP

void CISISX :: cidbx_init(void)
{
 partrace=0;                 /* dbxopen/dbxcipar trace */
 dbxtrace=0;                 /* dbxopen/dbxcipar trace */
 rectrace=0;                 /* rec RESTRACE runtime switch */
 dectrace=0;                 /* decoread()/recdeco() runtime switch  */
 trmtrace=0;                 /* trm TRSTRACE runtime switch */
 b40trace=0;                 /* b40 RUXTRACE runtime switch */
 b50trace=0;                 /* b50 RUXTRACE runtime switch */
 b70trace=0;                 /* b70 RUXTRACE runtime switch */
 fmttrace=0;                 /* fmt runtime switch */
 fsttrace=0;                 /* fst FSSTRACE runtime switch */
 multrace=0;                 /* upd MULTRACE runtime switch */
 cgitrace=0;                 /* cicgi trace */
 bugadddel=1;                /* 13/03/94 */

 dbxopt_fatal = 1;           /* dbxopen() - fatal when file doesn't exist */
 dbxopt_errno = 1;           /* dbxopen() - dbname/errno msg before fatal */

 dbxopt_mflush = 0;          /* record() - flush lastrecread if != dbname */
 dbxopt_mclose = 0;          /* record() - close lastrecread if != dbname */
 dbxopt_iflush = 0;          /* term() - flush lasttrmread if != dbname */

 rec_maxmfrl=MAXMFRL;	     /* 25/02/97 */
 rec_mstload=0L;	     /* 28/03/97 */
 trm_invload=0L;	     /* 28/03/97 */

#if MULTI
 dbxfloff=MSBSIZ;	     /* dbxflock()/dbxulock() - offset = blk #2 */
 dbxflsiz=MSBSIZ;	     /* dbxflock()/dbxulock() - length */
 dbxfloop = SHRT_MAX;        /* dbxflock() - #retries */
 dbxwloop = SHRT_MAX;        /* dbxwlock() - #retries */
 dbxiloop = SHRT_MAX;        /* dbxilock() - #retries */
 dbxuclos = 1;               /* dbxulock() - close/reopen */
 dbxewlrc = 0;               /* dbxflock()/dbxwlock() error action */
 dbxsleep = 0;               /* dbxilock() seconds to sleep */
#endif /* MULTI */

#if MULTI
 dbxopt_ordwr = O_RDONLY;    /* dbxopen() */
#else
 dbxopt_ordwr = O_RDONLY;    /* dbxopen() - O_RDWR / O_RDONLY */
#endif
 dbxordwr = 0;               /* dbxopen() - dbxopt_ordwr & O_RDWR */

 bitmask[0] = 0x80; bitmask[1] = 0x40; bitmask[2] = 0x20;
 bitmask[3] = 0x10; bitmask[4] = 0x08; bitmask[5] = 0x04;
 bitmask[6] = 0x02; bitmask[7] = 0x01;

 fpc_fd=0;                   /* fpccreat()/fpcwrite()/fpcclose() */
 fpc_left=FPCBSIZ;           /* fpccreat()/fpcwrite()/fpcclose() */
 fpc_buffer=NULL;            /* fpccreat()/fpcwrite()/fpcclose() */

 vlex[0] = LE1; vlex[1] = LE2;             /* use it */
 nxbsiz[0] = N1BSIZ; nxbsiz[1] = N2BSIZ;   /* use it */
 lxbsiz[0] = L1BSIZ; lxbsiz[1] = L2BSIZ;   /* use it */

 mx1extp = ".mst";                         /* mstsetup/recisis0 */
 xx1extp = ".xrf";                         /* mstsetup/recisis0 */

 cx1extp = ".cnt";                           /* invsetup/trmisis0 */
 nx12extp[0] = ".n01"; nx12extp[1] = ".n02"; /* invsetup/trmisis0 */
#if LIND
 lx12extp[0] = ".ly1"; lx12extp[1] = ".ly2"; /* invsetup/trmisis0 */
 ix1extp = ".iyp";                           /* invsetup/trmisis0 */
#else
 lx12extp[0] = ".l01"; lx12extp[1] = ".l02"; /* invsetup/trmisis0 */
 ix1extp = ".ifp";                           /* invsetup/trmisis0 */
#endif
#if CNLI
 iy0extp = ".iy0";           /* All IF into 1. To ALP, WL */
#endif

#if MULTI
 cipnetws=MONONETS;          /* MULTI default operation */
#endif

#if DBXMSTXL
#if CIFFI
 cipmstxl=4;                 /* extended .mst capacity */
#else
 cipmstxl=0;                 /* extended .mst capacity */
#endif
#endif

#if BEFORE20000323           /* because some BRM's GCC - ask rpiva */
 cistderr=stderr;            /* 20/05/98 (Asael!) */
#else
#define cistderr stderr
#endif

 dbxcipfp = NULL;            /* dbxcipar() input parameter file pointer */
 dbxcdcip = NULL;            /* dbxcipar() input data */
 dbxcipok = 0;               /* dbxcipar() found */

#if GIPAR
 dbxgiext[0] = ".xrf";
 dbxgiext[1] = ".mst";
 dbxgiext[2] = ".cnt";
 dbxgiext[3] = ".n01";
 dbxgiext[4] = ".n02";
#if LIND
 dbxgiext[5] = ".ly1";
 dbxgiext[6] = ".ly2";
 dbxgiext[7] = ".iyp";
#else /* LIND */
 dbxgiext[5] = ".l01";
 dbxgiext[6] = ".l02";
 dbxgiext[7] = ".ifp";
#endif /* LIND */
 dbxgiext[8] = ".any";
 dbxgiext[9] = ".fdt";
 dbxgiext[10] = ".fst";
 dbxgiext[11] = ".fmt";
 dbxgiext[12] = ".pft";
 dbxgiext[13] = ".stw";
 dbxgiext[14] = ".srt";
 dbxgiext[15] = ".wpr";
 dbxgiext[16] = ".val";
 dbxgiext[17] = NULL;

 dbxgiexn[0] = 1;
 dbxgiexn[1] = 2;
 dbxgiexn[2] = 3;
 dbxgiexn[3] = 4;
 dbxgiexn[4] = 5;
 dbxgiexn[5] = 6;
 dbxgiexn[6] = 7;
 dbxgiexn[7] = 8;
 dbxgiexn[8] = 9;
 dbxgiexn[9] = 10;
 dbxgiexn[10] = 10;
 dbxgiexn[11] = 10;
 dbxgiexn[12] = 10;
 dbxgiexn[13] = 10;
 dbxgiexn[14] = 10;
 dbxgiexn[15] = 10;
 dbxgiexn[16] = 10;
#endif /* GIPAR */

}

void CISISX :: cirec_init(void)
{
 memset(lastrecread, 0x00, CIMPL+1);   /* 950724 */

 recxrefb=0;                /* recxref() comb */
 recxrefp=0;                /* recxref() comp */
 recxrefn=0;                /* recxref() flagnew */
 recxrefm=0;                /* recxref() flagmod */
 recreadl=0;                /* force recread() mfrl */

#if IFUPDATE
 recreadb=0;                /* force recread() comb = mfbwb */
 recreadp=0;                /* force recread() comp = mfbwp */
#endif

 recisis0_m=1;              /* recisis0() init .mst */
 recisis0_s=0;              /* recisis0() system file type */
}

void CISISX :: citrm_init(void)
{
 trmifupd=0;                        /* trm IFUPDATE operation */

 lrlrseek=0L;
 lrlrsize=0;

 memset(lasttrmread, 0x00, CIMPL+1);    /* 950724 */

#if PC
 highv[0]=0xFF; highv[1]='\0';      /* manca controllo.. if signed */
#else
 highv[0]=0x7F; highv[1]='\0';      /* manca controllo.. if signed */
#endif
}

void CISISX :: ciifl_init(void)
{
 ciiflfix = 0;
 ciiflfim = 0;
 ifl_balan = 1;
}

void CISISX :: ciutl_init(void)
{
 sfldchr = SFLDCHR;
 loaduct0 = 0;
}

unsigned char CISISX :: init_isisuctab[256]=
{
#if PC || UNIX
	  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
	 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
	 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
	 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
	 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
	 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
	 96, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
	 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90,123,124,125,126,127,
	 67, 85, 69, 65, 65, 65, 65, 67, 69, 69, 69, 73, 73, 73, 65, 65,
	 69, 69, 69, 79, 79, 79, 85, 85, 89, 79, 85,155,156,157,158,159,
	 65, 73, 79, 85, 78, 78,166,167,168,169,170,171,172,173,174,175,
	176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
	192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,
	208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,
	224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
	240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255
#endif
#if MPE
	  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
	 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 28, 30, 31,
	 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
	 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
	 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
	 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
	 96, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
	 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90,123,124,125,126,127,
	128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
	144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
	160, 65, 65, 69, 69, 69, 73, 73,168,169,170,171,172, 85, 85,175,
	176, 89, 89,179, 67, 67, 78, 78,184,185,186,187,188,189,190,191,
	 65, 69, 79, 85, 65, 69, 79, 85, 65, 69, 79, 85, 65, 69, 79, 85,
	 65,209, 79,211, 65, 73, 79,215, 65, 73, 79, 85, 69,221,222, 79,
	 65, 65, 65, 68, 68, 73, 73, 79, 79, 79, 79,235,236, 85, 89, 89,
	240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255
#endif
#if VAX
	  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
	 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
	 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
	 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
	 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
	 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
	 96, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
	 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90,123,124,125,126,127,
	128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
	144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
	160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,
	176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
	192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,
	208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,
	224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
	240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255
#endif
};

#ifdef CI_WINISIS
unsigned char CISISX :: init_isisactab[256] = {
#else
unsigned char CISISX :: init_isisactab[] = {
#if PC || UNIX
 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83,
 84, 85, 86, 87, 88, 89, 90, 97, 98, 99,100,101,102,
103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,
122,128,129,130,131,132,133,134,135,136,137,138,139,
140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,160,161,162,163,
164,165
#endif
#if MPE
 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83,
 84, 85, 86, 87, 88, 89, 90, 97, 98, 99,100,101,102,
103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,
122,161,162,163,164,165,166,167,173,174,177,178,180,
181,182,183,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,
208,210,212,213,214,216,217,218,219,220,220,220,220,
223,224,225,226,227,228,229,230,231,232,233,239,234,237,238,239
#endif
#if VAX
 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83,
 84, 85, 86, 87, 88, 89, 90, 97, 98, 99,100,101,102,
103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,
122
#endif
,000
  };
#endif /* CI_WINISIS */

CISISX :: CISISX (unsigned char *parm_isisactab,
                  unsigned char *parm_isisuctab,
                  int *parm_NXPAGESp)
{
 maxndbx = MAXNDBX;         /* runtime value                */

 // initialize vdbxp[]
 for (ndbxs=maxndbx;ndbxs--;)
  vdbxp[(unsigned)ndbxs]=(DBXSTRU *)NULL;
 ndbxs=0;                   /* actual #entries              */

#if FATRAP
 fatal_errcod = 1;              /* cisis fatal() error code */
 fatal_iomsg[0] = '\0';         /* cisis fatal() flag/msg */
 memset(&fatal_jumper,0x00,sizeof(fatal_jumper));
#endif

 // set up the upper case charaters table
 if (parm_isisuctab)
  memmove(isisuctab,parm_isisuctab,sizeof(isisuctab));
 else
  memmove(isisuctab,init_isisuctab,sizeof(isisuctab));

 isiswctot=0;

 // set up the alphabetic charaters table
 if (parm_isisactab)
  memmove(isisactab,parm_isisactab,sizeof(isisactab));
 else
  memmove(isisactab,init_isisactab,sizeof(isisactab));
 memset(isiswctab,0x00,sizeof(isiswctab));
 for (isiswctot=0; isisactab[isiswctot]; isiswctot++)
  isiswctab[isisactab[isiswctot]]=1;

 // set up NXPAGES
 if (parm_NXPAGESp)
  for (int i=0; i <= MAXLIV; i++) NXPAGES[i]=parm_NXPAGESp[i];
 else {
  for (int i=0; i <= MAXLIV; i++) NXPAGES[i]=1;
  NXPAGES[1]=TWORDN;
 }

 cidbx_init();
 cirec_init();
 citrm_init();
 ciifl_init();
 ciutl_init();
} // end of constructor


CISISX :: ~CISISX (void)
{
}

DBXSTRU * CISISX :: getDBXSTRU(char *dbnamep)
{
 char *dbnp = (char *)((dbnamep == NULL) ? "_" : dbnamep);

 return xdbxstorp(dbnp);
}
#endif // CICPP

