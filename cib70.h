/*	 --------------------- cib70.h --------------------- */

#if CIAPI
#include "ciapi.h"
#endif

/*
   b7_CIB7 *cib7 como primer parametro de las funciones, algo asi como el this de C++
 */
#define B7_THIS cib7p
#define B7_THISP cib7p,

#define DISTRACE 0
#define TOKTRACE 0
#define BOLTRACE 0
#define RECTRACE 0

#if RECTRACE
#define DISTRACE 1
#endif

/* prototipos de las funciones */

#if ANSI
b7_CIB7 *b7_cib7(b7_CIB7 *cib7p, void *xciapip);
b7_CIB7 *b7_cib7_delete(b7_CIB7 *cib7p);
int b7_msg(b7_CIB7 *cib7,int tline,char *msg,LONGX psts,LONGX docs,int tmsg);
void statement(b7_CIB7 *cib7, LONGX *result);
void astatement(b7_CIB7 *cib7, LONGX *result);
char expression(b7_CIB7 *cib7, LONGX *result);
char conjunction(b7_CIB7 *cib7, LONGX *result);
char conjgop(b7_CIB7 *cib7, LONGX *result);
char conjfop(b7_CIB7 *cib7, LONGX *result);
char conjpop(b7_CIB7 *cib7, LONGX *result);
char conjeop(b7_CIB7 *cib7, LONGX *result);
char primitive(b7_CIB7 *cib7, LONGX *result);
void semaction(b7_CIB7 *cib7, int type, char op, int opl, LONGX *r, LONGX *h);
void get_token(b7_CIB7 *cib7);
void gettoken(b7_CIB7 *cib7);
void b7_experr(b7_CIB7 *cib7, int error, char *errp, int errl);
void fixup(b7_CIB7 *cib7, char *p1, char *p2);
int fixlev(b7_CIB7 *cib7, int i2, char level);
char *b7_massp(b7_CIB7 *cib7, char *tokenmassp, int   tokenlen, int   b7ee_pfx);
int b7_free(b7_CIB7 *cib7, int ipdl);
int b7_xpdl(b7_CIB7* cib7,int ipdl1, int ipdl2 );
void b7_runerr(b7_CIB7* cib7, int error, char *errp, int errl);
char *b7_initpdl(b7_CIB7* cib7, PDLSTRU *pdl, char postlen, LONGX lastmfn);
PDLSTRU *b7_ipdl(b7_CIB7* cib7, int ipdl);
int b7_pop(b7_CIB7* cib7);
PDLSTRU *b7_push(b7_CIB7* cib7);
PDLSTRU *b7_top(b7_CIB7* cib7);
LONGX b7_hsizh(b7_CIB7* cib7, PDLSTRU *pdlp,LONGX *hsizp);
void b7_zorx(b7_CIB7* cib7);
void b7_zand(b7_CIB7* cib7,  short andl );
void b7_zann(b7_CIB7* cib7);
void b7_zanp(b7_CIB7* cib7,  short andl, unsigned char b7qty );
void b7_copy(b7_CIB7* cib7,  PDLSTRU *pdl, char *src );
void b7_inst(b7_CIB7* cib7,  PDLSTRU *pdl, char *src );
void b7_addk(b7_CIB7* cib7,  PDLSTRU *pdl, TRMSTRU *trmp );
void b7_showpdl(b7_CIB7* cib7);
char *b7_xlkey(b7_CIB7 *cib7, char *opx,char *bufp,unsigned char uctab[],char *xlfilp,int *opxlp);
int b7_pfx(b7_CIB7 *cib7, LONGX irec, UWORD invxtag, char *dp, char *op, int b7re_pfx);
int b7_cmp(b7_CIB7 *cib7p, char *op1,char *op2,int oplen);
LONGX b7_gmfn(b7_CIB7 *cib7p, char *str);
int b7_prox(b7_CIB7 *cib7p,char *op1,char *op2,short andl,LONGX dist);
void b7_print(b7_CIB7 *cib7p,char *str,short int len);
#else
b7_CIB7 *b7_cib7();
b7_CIB7 *b7_cib7_delete();
int b7_msg();
void statement();
void astatement();
char expression();
char conjunction();
char conjgop();
char conjfop();
char conjpop();
char conjeop();
char primitive();
void semaction();
void get_token();
void gettoken();
void b7_experr();
void fixup();
int fixlev();
char *b7_massp();
int b7_free();
int b7_xpdl();
void b7_runerr();
char *b7_initpdl();
PDLSTRU *b7_ipdl();
int b7_pop();
PDLSTRU *b7_push();
PDLSTRU *b7_top();
LONGX b7_hsizh();
void b7_zorx();
void b7_zand();
void b7_zann();
void b7_zanp();
void b7_showpdl();
void b7_copy();
void b7_inst();
void b7_addk();
char *b7_xlkey();
int b7_pfx();
int b7_cmp();
LONGX b7_gmfn();
int b7_prox();
void b7_print();
#endif /* ANSI */


/* uso de los componenter de cib7 como si fueran "globales" */
/* tratando de no modificar mucho los fuentes... */
#define b7objp cib7p->b7objp
#define and2gf cib7p->and2gf
#define subst_and cib7p->subst_and
#define b7setno1 cib7p->b7setno1
#define b7setno cib7p->b7setno
#define b7batchp cib7p->b7batchp
#define b7recp cib7p->b7recp
#define b71jumper cib7p->b71jumper
#define b71error cib7p->b71error
#define b7errxy cib7p->b7errxy
#define tokbackp cib7p->tokbackp
#define toknextp cib7p->toknextp
#define token cib7p->token
#define toktyp cib7p->toktyp
#define tokopr cib7p->tokopr
#define tokqty cib7p->tokqty
#define toklen cib7p->toklen
#define tokqualp cib7p->tokqualp
#define tokquall cib7p->tokquall
#define tokqualn cib7p->tokqualn
#define tokmassp cib7p->tokmassp
#define nb7oprs cib7p->nb7oprs
#define mask cib7p->mask
#define b7_gidbnp cib7p->b7_gidbnp
#define vpdlp cib7p->vpdlp
#define npdls cib7p->npdls
#define b7tell cib7p->b7tell
#define b7itrm cib7p->b7itrm
#define b7fd cib7p->b7fd
#define b7error cib7p->b7error
#define b7bufferp cib7p->b7bufferp
#define b7_pfxdbn cib7p->b7_pfxdbn
#define b7_pfxopx cib7p->b7_pfxopx
#define b7_pfxlen cib7p->b7_pfxlen
#define b7_pfxmsg cib7p->b7_pfxmsg
#define b7_pfxtwx cib7p->b7_pfxtwx
#define b7_pfxmdl cib7p->b7_pfxmdl
#define xtrac cib7p->xtrac
#define vetopr cib7p->vetopr
#define vetcas cib7p->vetcas
#define vetopp cib7p->vetopp
#define nop cib7p->nop
#define nqualts cib7p->nqualts
#define vqualt cib7p->vqualt
#define qualtp cib7p->qualtp
#define b7rootmsg cib7p->b7rootmsg
#define b7tw2mh cib7p->b7tw2mh

#if ANSI
void display(char *s);
#else
void display();
#endif


