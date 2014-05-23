#ifndef CIBX_HPP
#define CIBX_HPP

#include <cisisx.hpp>

class _YOURCLASS CIBX
{
 public:

 CISISX * cisisxp;

//---------------------- cib51.c ------------------------------------
  /* b5_() global */
  LONGX b5setno;                   /* current query set no */
  char *b5batchp;                 /* fldupdat */
  RECSTRU *b5recp;                /* current recp */

#if FATRAP
#if !WWWISIS
  jmp_buf b51jumper;
#endif /* !WWWISIS */
#endif /* FATRAP */
  int b51error;
  char b51errxy[MAXERRL+1];       /*  */

  /* gettoken */
  char *toknextp;
  char *token;
  int toktyp;
  char tokopr;
  int tokqty;
  int toklen;

  char *tokmassp;                 /* toknextp after b5_massp() */

  /* expressoes */
  int nb5oprs;
  int b5highnow;
  int b5highmax;

//---------------------- cib62.c ------------------------------------
  int b6tw2mh;        /* SOING */
  char *b6_gidbnp;	/* dbn.par */

  typedef struct b6hitbuf {
    LONGX setno;                         /* query number */
    LONGX block;                         /* block number */
    LONGX bufsiz;                        /* buffer size */
    UBYTE area[1];                      /* buffer - via ALLOC() */
  } B6HITBUF;

  typedef struct pdlstru {
    LONGX         pdlirx1;       /* offset - not used */
    LONGX         pdlirxi;       /* index - not used */
    LONGX         pdlirxs;       /* b53_seek or EOF */
    LONGX         pdlleft;       /* bytes still free - not used */
    char         pdlirxa[1];    /* tmp area for working storage */
  } PDLSTRU;
#define PDLirx1         pdlp->pdlirx1
#define PDLirxi         pdlp->pdlirxi
#define PDLirxs         pdlp->pdlirxs
#define PDLirxa         pdlp->pdlirxa
#define PDLleft         pdlp->pdlleft
#define VPDLirx1(i)     vpdlp[i]->pdlirx1
#define VPDLirxi(i)     vpdlp[i]->pdlirxi
#define VPDLirxs(i)     vpdlp[i]->pdlirxs
#define VPDLirxa(i)     vpdlp[i]->pdlirxa
#define VPDLleft(i)     vpdlp[i]->pdlleft

  /* pilha de execucao */
  char *irx0p;                    /* result addr */
  PDLSTRU *vpdlp[MAXPDLS];        /* ptr pilha de execucao */
  int npdls;                      /* no entrs */

  TRMSTRU *b5trmp;

  /* temporary storage */
  LONGX b5irxbytes;                /* parm nbytes1 */
  LONGX b5maxbyts;                 /* max bytes in bitstring */
  LONGX b5maxbyts0;                /* max bytes in bitstring - to override */
  int b5prodtag;                  /* product tag on qrydbnp - via btch0p */
  char *b5prodp;                  /* product's name */
  char b53_tmpnam[CIMPL+1];           /* file name for temporary storage */
  int b53_fd;			  /* open() */
  LONGX b53_seek;		  /* before b53_yopen() */
  DBXSTRU *b5_savdbxp;		  /* '@' processing */
  int b6fd;			  /* open() - 06/03/95 */
  /* b5_msg call */
  LONGX b5tell;
  int b5rootf;
  int b5rootmsg;
  /* error */
#if FATRAP
#if !WWWISIS
  jmp_buf b52jumper;
#endif /* !WWWISIS */
#endif /* FATRAP */
  int b52error;
  char b52errxy[MAXERRL+1];	  /*  */
#if BIGREC
#else /* BIGREC */
  B6HITBUF *b6bufferp;		  /* b6_run() processing buffer */
#endif /* BIGREC */
  LONGX b5_isn1;			  /* query #0 lower limit */
  LONGX b5_isn2;			  /* query #0 upper limit */
  LONGX keepndbx;
  unsigned char b5_pfxdbn[CIMPL+1];
  unsigned char b5_pfxopx[LE2+LE2+1];
  FFI b5_pfxlen;
  unsigned char b5_pfxmsg[LE2+1];
  unsigned char b5_pfxtwx[CIMPL+1];
  FFI b5_pfxmdl;

//---------------------- member functions ----------------------------

  void cib51_init();
  void cib62_init();

  CIBX(CISISX *parm_cisisxp);

//---------------------- cib50m.c ------------------------------------
  char *b5_massp(char *token,
                 int   tokenlen,
	         int   b5ee_pfx);

//---------------------- cib50r.c ------------------------------------
  int b50read(DBXSTRU *dbxp,
              LONGX     mfn,
              int      tag,
              int      occ,
              char    *areap,
              LONGX    *areasizp);

//---------------------- cib51.c ------------------------------------
  char *b5_exp(RECSTRU *irecp,
               char *dbnamp,
               char *qryp,
               char *buffup,
               char *qrydbnp,
               RECSTRU *crecp,
               int *errnop);

  void statement(LONGX *result);

  char expression(LONGX *result);

  char conjunction(LONGX *result);

  char primitive(LONGX *result);

  void semaction(int   type,
    	         char  op,
	         int   opl,
	         LONGX *r,
	         LONGX *h);

  int b5_nxtoken();

  void gettoken();

  void b5_experr(int   error,
	         char *errp,
	         int   errl);

  void display(char *s);

//---------------------- cib50t.c -----------------------------------

  int b5_msg(int tline,
             char *msg,
             LONGX docs,
             int tmsg);

  int b5_kbh(void);

//---------------------- cib62.c ------------------------------------

  void b5_keepdbx(void);

  int b5_deltmp(int   fd,
                char *tmpnamp);

  void b5_runerr(int   error,
    	         char *errp,
	         int   errl);

  void b5_free(TRMSTRU *trmp);

  int b5_ipdl(int ipdl);

  void b53_yopen();

  int b53_writx(PDLSTRU     *pdlp,
	        char        *buff,
                unsigned int nw);

  int b53_readx(int          ipdl,
	        LONGX         offset,
	        unsigned int nr);

  int b5_savpdl(int ipdl);

  int b5_pfx(RECSTRU *irecp,
             UWORD invxtag,
             char *dp,
             char *op,
             int b5re_pfx);

  char *b6_run(RECSTRU *irecp,
               char *dbnamp,
               char *buffup,
               int buffuplen,
               LONGX nbytes1,
               LONGX tell,
               unsigned char uctab[],
               char *btch0p,
               RECSTRU *tlirecp,
               RECSTRU *crecp,
               int *errnop);

  LONGX b5_hsizh(char *irxp,
    	        LONGX nbytes,
	        LONGX *hsizp,
	        LONGX *basep);

  int b6_hballoc(LONGX       bufsiz,
	         B6HITBUF **bufferpp);

  int b6_hbinit(LONGX      setno,
	        B6HITBUF *bufferp);

  int b6_hbfree(B6HITBUF **bufferpp);

  LONGX b6_hit(RECSTRU *irecp,
              LONGX nord,
              LONGX setno,
              B6HITBUF *bufferp);
};

#endif // CIBX_HLL
