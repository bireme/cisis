#ifndef CIDBX_HPP
#define CIDBX_HPP

//#include <cisis.hpp>
#include <cisisx.hpp>
#include <string.h>

#ifndef ErrorCode
#define ErrorCode       long
#endif // ErrorCode

#ifndef TRUE
# define TRUE           1
#endif // TRUE

#ifndef FALSE
# define FALSE          0
#endif // FALSE

#define ERRMESLEN       256     // Error message lenght.

class Fatal                     // Exception handling.
{
 public:

 ErrorCode Fa_ECode;                // Error code.
 LONGX      Fa_Abort;                // Abort the execution. (yes=1, no=0)
 char      Fa_EMess[ERRMESLEN];     // Error message.
 char      Fa_AuxEMess[ERRMESLEN];  // Auxiliary error message.

 Fatal (void)
 {
  Fa_ECode = Fa_Abort = 0;

  *Fa_EMess='\0';
  *Fa_AuxEMess='\0';
 }

 LONGX Fa_Fill (ErrorCode ecode,
	  char     *emess,
	  LONGX      abort = FALSE,
	  char     *auxemess = "")
 {
  if (!auxemess || *auxemess == '\0')
   strcpy (Fa_EMess, emess);
  else
  {
   sprintf (Fa_AuxEMess, " ->\n%s", auxemess);
   strcpy (Fa_EMess, emess);
   strcat (Fa_EMess, Fa_AuxEMess);
  }

  Fa_Abort = abort;
  *Fa_AuxEMess = '\0';
  return (Fa_ECode = ecode);
 }
};
/* bode da dll_utl.hpp ... */

class _YOURCLASS DBXSTRU //: public CISIS
{
 public:

    char       dbxname[CIMPL+1];  /* dbname + .ext + NULL */
    int        dbxxropn;          /* =dbxopen(.xrf) */
    int        dbxmsopn;          /* =dbxopen(.mst) */
    int        dbxxropw;          /* dbxopenw/w(.xrf) */
    int        dbxmsopw;          /* dbxopenw/w(.mst) */
    int        dbxmsopv;          /* dbxflock/dbxulock(.mst) */
    XRSTRU    *dbxxribp;          /* ptr .xrf ibuf */
    MSSTRU    *dbxmsibp;          /* ptr .mst ibuf */
#if GIPAR
    char      *dbxgicip;          /* .par contents */
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
    void      *dbxifupp;          /* CIIFU_PROCX_SOURCE */
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
    CISISX    *cisisxp;

    DBXSTRU(CISISX *parm_cisixp);

#if MULTI
  //int xdbxcinet(char *dbnamp);

  int xdbxflock(DBXSTRU *dbxp,
                char *typ);

  int xdbxulock(DBXSTRU *dbxp,
                char *typ);

  /* int xdbxilock(DBXSTRU *dbxp,
                  char *mdbnp,
                  char *typ); 
  */
  int xdbxwlock(DBXSTRU *dbxp,
                char *m0p,
                int   times);
#endif

  //void xdbxinit(void);

  //DBXSTRU *xdbxstorp(char *dbnamp);

  //DBXSTRU *xdbxsrchp(char *dbnamp);

  //int xdbxopen(char *gidbnp,
  //             char *dbname,
  //             char *extp);

  //int xdbxopenw(char *gidbnp,
  //              char *dbname,
  //              char *extp,
  //              int  *opnp,
  //              int  *opwp,
  //              char *errmsgp);

  //char *xdbxopenc(char *gidbnp,
  //                char *filnamp,
  //                int *opnp,
  //                int *opwp,
  //                char *errmsgp, int xcreate, int xappend);

//  static void xfatal(char *msg);

  void xdbxflush(char *dbnamp);

  void xmstflush(char *dbnamp);

  void xmstclose(DBXSTRU *dbxp);

//#if RECGIZM
//  void xgizflush(VGIZPSTRU *vgizmap);
//#endif

//#if RECDECO
//  void xdecflush(VDECPSTRU *vdecmap);
//#endif

  INVMAP *xdbxinvmp(DBXSTRU *dbxp);

  void xinvclose(DBXSTRU *dbxp);

  void xinvflush(char *dbnamp);

  //char *xloadfile(char *gidbnp,
  //	       	  char  at,
  //	          char *atp,
  //                char *areap,
  //	          LONGX asize,
  //	          char lf2x);

  //char *xloadstw(char *gidbnp,
  //	           char *atp,
  //	           char *areap,
  //	           LONGX  asize,
  //	           int  *nstws);

  //void xdbxciset(void);

  //char *xdbxcipar(char *gidbnp,
  //                char *argkey,
  //                char argchar);

//#if GIPAR
  //char *xdbxgipar(char *gidbnp,
  //                char *gikeyp,
  //                int *namlenp,
  //                int *extlenp);
//#endif

#if GEN_CORELEFT
  unsigned LONGX xcoreleft(void);
#endif

#if GEN_LABS
  static long xlabs(long x);
#endif

#if GEN_STRSTR
  static char *xstrstr (CONST char *p1,
                        CONST char *p2);
#endif

#if GEN_STRUPR
  static char *xstrupr (char* p);
#endif

#if GEN_STRREV
  static char *xstrrev (char *p);
#endif

#if GEN_MEMICMP
  static int xmemicmp(CONST void   *s1,
		      CONST void   *s2,
		      size_t  n);
#endif

  int xfpccreat(char *gidbnp,
                char *namp,
		char *extp,
		int   lrecl);

  void xfpcwrite(char *recbufp,
                 FFI   reclen);

  void xfpcclose(void);

  //char *xdbxtmpnm(char *dirtmp,
  //                int strip,
  //                char *filnamp);

  char *xcicopyr(char *namep);

};

#endif //CIDBX_HPP

