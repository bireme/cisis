#ifndef CIB7_HPP
#define CIB7_HPP

#include <cirec.hpp>
#include <citrm.hpp>

/*
	CIB7 *cib7 como primer parametro de las funciones, algo asi como el this de C++
 */

#define B7_THIS
#define B7_THISP

#define DISTRACE 0
#define TOKTRACE 0
#define BOLTRACE 0
#define RECTRACE 0

#if RECTRACE
#define DISTRACE 1
#endif

#define B7EE_X2          2
#define B7EE_PFX         11


class _YOURCLASS CIB7
{
 public:

    CISISX * cisisxp;

/*     int b70trace; atualizado em cisisx.?pp - AOT/RM, 23/02/2001 */


//cib71.c
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

/* error */
#if FATRAP
#if !WWWISIS
    jmp_buf b71jumper;
#endif /* !WWWISIS */
#endif /* FATRAP */

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

//cib72
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

    void *ciapip;
    LONGX smfn;
    RECSTRU *srecp;

    short unsigned int nqualts,vqualt[MAXQUAL],*qualtp;

    int b7rootmsg;

   //prototipos de las funciones

    LONGX b7_z_search(char* path, char* query, LONGX *num_records );
    char *b7_z_record(RECSTRU *lrecp, LONGX lmfn, LONGX nord );
    b7_CIB7 *b7_cib7();
    b7_CIB7 *b7_cib7_delete();
    char *b7_exp(RECSTRU *irecp, char *dbnamp, char *qryp,
               char *buffup, char *qrydbnp, RECSTRU *crecp, int *errnop);
    char *b7_run(RECSTRU *irecp,char *dbnamp,char *buffup, int buffuplen,
			        LONGX tell,unsigned char uctab[],char *btch0p,
                 LONGX tlirecp,LONGX crec,int *errnop);
    int b7_hballoc(LONGX bufsiz, B7HITBUF **bufferpp);
    int b7_hbinit(LONGX setno, B7HITBUF *bufferp);
    int b7_hbfree(B7HITBUF **bufferpp);                         
    LONGX b7_hit(RECSTRU *irecp, LONGX nord, LONGX setno);
    int b7_msg(int tline,char *msg,LONGX psts,LONGX docs,int tmsg);
    void statement(LONGX *result);
    void astatement(LONGX *result);
    char expression(LONGX *result);
    char conjunction(LONGX *result);
    char conjgop(LONGX *result);
    char conjfop(LONGX *result);
    char conjpop(LONGX *result);
    char conjeop(LONGX *result);
    char primitive(LONGX *result);
    void semaction(int type, char op, int opl, LONGX *r, LONGX *h);
    void get_token();
    void gettoken();
    void b7_experr(int error, char *errp, int errl);
    void fixup(char *p1, char *p2);
    int fixlev(int i2, char level);
    char *b7_massp(char *tokenmassp, int   tokenlen, int   b7ee_pfx);
    int b7_free(int ipdl);
    int b7_xpdl(int ipdl1, int ipdl2 );
    void b7_runerr(int error, char *errp, int errl);
    char *b7_initpdl(PDLSTRU *pdl, char postlen, LONGX lastmfn);
    PDLSTRU *b7_ipdl(int ipdl);
    int b7_pop();
    int b7_pfx(RECSTRU *irecp, UWORD invxtag, char *dp, char *op, int b7re_pfx);
    char *b7_run(RECSTRU *irecp, char *dbnamp, char *buffup, int buffuplen,
                 LONGX tell, unsigned char uctab[], char *btch0p,
                 RECSTRU *tlirecp, RECSTRU *crecp, int *errnop);
    PDLSTRU *b7_push();
    PDLSTRU *b7_top();
    LONGX b7_hsizh(PDLSTRU *pdlp,LONGX *hsizp);
    int b7_cmp( char *op1, char *op2, int oplen );
    int b7_prox( char *op1, char* op2, short andl, LONGX dist );
    LONGX b7_gmfn( char *str );        
    void b7_zorx();
    void b7_zand(short andl );
    void b7_zann();
    void b7_zanp(short andl, unsigned char b7qty );
    void b7_copy(PDLSTRU *pdl, char *src );
    void b7_inst(PDLSTRU *pdl, char *src );
    void b7_addk(PDLSTRU *pdl, TRMSTRU *trmp );
    void b7_print( char *str, short int len );    
    void b7_showpdl();
    char *b7_xlkey(char *opx,char *bufp,unsigned char uctab[],char *xlfilp,int *opxlp);
    LONGX b7_hsizh(PDLSTRU *pdlp,LONGX hsizp);    
    void display(char *s);

    CIB7(CISISX *parm_cisisxp);
};

#endif /* CIB7_HPP */
