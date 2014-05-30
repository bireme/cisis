/*
 * <R>
 * cib71, b7_exp()
 * basado en b6 usando algo de b4
 * b4 - proximidad
 * b6 - prefix, multiples I/F, log format
 * </R>
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

/* ----------------------------- b60.c ------------------------------ */
/* ----------------------------- b61.c ------------------------------ */

#if CIAPI
#include "ciapi.h"
#if !TLS
#define ciapip cib7p->ciapip
#else
DWORD extern ciapiTlsIndex;
#endif
#else /* CIAPI */
#include "cisis.h"
#endif /* CIAPI */

#if !CICPP
#include "cib70.h"
#endif /* CICPP */

#if CICPP
char * CIB7::b7_exp(RECSTRU *recp,
                    char *dbnamp,       /* ptr nome base de dados a ler */
                    char *qryp,         /* ptr formulacao */
                    char *buffup,       /* area addr for fldupdat */
                    char *qrydbnp,      /* query data base */
                    RECSTRU *crecp,     /* registro de controle qrydbnp */
                    int *errnop)        /* addr para codigo de erro */

#else /* CICPP */
char *b7_exp(cib7p,irec,dbnamp,qryp,buffup,qrydbnp,crec,errnop)         /*

------------
                    ...
                    retorna NULL ou ptr erro

                                                                        */
b7_CIB7 *cib7p;         /* variables globales */
LONGX irec;     /* vrecp para store */
char *dbnamp;       /* ptr nome base de dados a ler */
char *qryp;         /* ptr formulacao */
char *buffup;       /* area addr for fldupdat */
char *qrydbnp;      /* query data base */
LONGX crec;     /* registro de controle qrydbnp */
int *errnop;        /* addr para codigo de erro */
#endif /* CICPP */
{
#if !CICPP
    RECSTRU *recp;
#endif /* CICPP */

    LONGX answer;
    char *b7buffup = NULL;
    char *trim;

#if DISTRACE
 printf("b7_exp - irec,dbnamp,qryp: %"_LD_",%s,'%s' [%s]\n",
  irec,dbnamp,qryp,qrydbnp);
#endif
#if BEFORE20010117
#if SHOWCORE
 if (b70trace) showcore("b7_exp");
#endif
#endif /* BEFORE */

    strcpy(b7errxy,"b7_exp");
#if FATRAP
    b71error = setjmp(b71jumper);
#else
    b7error = b71error = 0;
#endif /* FATRAP */
B7_EXPERROR:
    if (b71error != 0) {
#if ERRTRACE
    printf("setjmp - error=%d (%s)\n",b71error,b7errxy);
#endif
      *errnop=b71error;
#if CICPP
      if (b7buffup) delete[] b7buffup;
#else /* CICPP */
      if (b7buffup) FREE(b7buffup);
#endif /* CICPP */
      return(b7errxy);
    }

#if CICPP
    try { b7buffup = new char[((ALLOPARM)MAXMFRL+1)]; }
    catch (BAD_ALLOC) { b7buffup = (char *)ALLONULL; }
    if (b7buffup == (char *)ALLONULL){ b7_experr(B7_THISP -8,"buffup/ALLOC",0);
    goto B7_EXPERROR;
    }
#else /* CICPP */
    b7buffup = (char *)ALLOC((ALLOPARM)MAXMFRL+1);
    if (b7buffup == (char *)ALLONULL){ b7_experr(B7_THISP -8,"buffup/ALLOC",0);
    goto B7_EXPERROR;
    }

    if (!ndbxs) dbxinit();  /* init vdbxp/vrecp/vtrmp if not yet init - AOT, 28/10/2005 */

    recp=vrecp[irec];
#endif /* CICPP */

    if (!recp){
      b7_experr(B7_THISP B7EE_X2,"b7_exp/recallok/recp",0);
      goto B7_EXPERROR;
    }

#if CICPP
    if (!recp->recmfp){
      b7_experr(B7_THISP B7EE_X2,"b7_exp/recallok/recmfp",0);
      goto B7_EXPERROR;
    }
#endif /* CICPP */

    trim = qryp + ( strlen( qryp ) - 1 );
    for( ;isspace( *trim ); trim-- )
    	*trim='\0';

    b7recp=recp;

    RECtype = TYPEMFR;
    if (qrydbnp) {
        RECdbxp = dbxstorp(qrydbnp);
        if (RDBmsmfn <= 0L) {
            recisis0(qrydbnp);
            RDBmsmfn = 1L;
        }
        MFRmfn=RDBmsmfn;
#if CICPP
        crecp->xrecord(dbnamp,0); 
        crecp->xrecord(qrydbnp,0); 
#else /* CICPP */
        record(crec,qrydbnp,0);
#endif /* CICPP */
    }
    else {
        RECdbxp = NULL;
        if (MFRmfn == 0) MFRmfn=1; else MFRmfn=MFRmfn+1;
    }

    RECrc=RCNORMAL;

    MFRmfrl=LEADER;
    MFRmfbwb=MFRmfbwp=0;

    MFRbase=MFRmfrl;
    MFRnvf=0;
    MFRstatus=ACTIVE;


    /* store MFQTDBN0 */
    sprintf(b7buffup,"A%d|%s|",MFQTDBN0,dbnamp);
#if CICPP
    if (recp->xfldupdat(b7buffup))
#else /* CICPP */
    if (fldupdat(irec,b7buffup))
#endif /* CICPP */
    {
        b7_experr(B7_THISP -3,"b7_exp/MFQTDBN0",0);
        goto B7_EXPERROR;
    }


    /* store MFQTQRY0 */
    sprintf(b7buffup,"A%d|%s|",MFQTQRY0,qryp);
#if CICPP
    if (recp->xfldupdat(b7buffup))
#else /* CICPP */
    if (fldupdat(irec,b7buffup))
#endif /* CICPP */
    {
        b7_experr(B7_THISP -4,"b7_exp/MFQTQRY0",0);
        goto B7_EXPERROR;
    }


    /* setup MFQTOBJ0 fldupdat */
#if BOLTRACE
    b7objp=b7buffup;
#endif
    b7batchp=b7buffup;
    sprintf(b7batchp,"A%d\n",MFQTOBJ0); b7batchp+=strlen(b7batchp); /* |  AOT, 27/05/2004 */


    /* aponta expressao para gettoken */
    toknextp=qryp;

    /* analisa primeiro comando */
    nb7oprs=0;
    statement(B7_THISP &answer);

    /* analisa demais */
    do {
        if (*token) {
            statement(B7_THISP &answer);
        }
        else
            break;
    } while (1);

    if( b7error ) {
          b7_experr(B7_THISP b7error,"b7_exp/statement",0);
          goto B7_EXPERROR;
    }

    *b7batchp++ = OPRXEND;
#if TESTNotNULL
    strcpy(b7batchp,"Not NULL");   b7batchp+=strlen(b7batchp);
#endif
    *b7batchp++ = '\n';  /* '|';   AOT, 27/05/2004 */
    *b7batchp='\0';
#if CICPP
    if (recp->xfldupdat(b7buffup))
#else /* CICPP */
    if (fldupdat(irec,b7buffup))
#endif /* CICPP */
    {
        b7_experr(B7_THISP -5,"b7_exp/fldupdat",0);
        goto B7_EXPERROR;
    }

    if (qrydbnp) {
#if CICPP
        recp->xrecupdat();
#else /* CICPP */
        recupdat(crec,irec);
#endif /* CICPP */
    }


#if BOLTRACE
 display("end");
#endif


    RECtype = TYPEMFQ;

#if BEFORE20010117
#if SHOWCORE
 if (b70trace) showcore("b7_exp - exit");
#endif
#endif /* BEFORE20010117 */

    *errnop=0;
    sprintf( buffup, "%s", b7buffup);
#if CICPP
    delete[] b7buffup;
#else /* CICPP */
    FREE(b7buffup);
#endif /* CICPP */
    return(NULL);
}



#if CICPP
void CIB7::statement(LONGX *result)
#else /*CICPP*/
void statement(cib7p,result)
b7_CIB7 *cib7p;
LONGX *result;
#endif /*CICPP*/
{

#if BOLTRACE
display("statement");
#endif

    get_token(B7_THIS );

    if (!*token){
        b7_experr(B7_THISP 1,"",0);
        return;
    }

    astatement(B7_THISP result);

    if (*token && *token != ';'){
        b7_experr(B7_THISP 2,token,toklen);
        return;
    }

    /*** nao gera fim de execucao */
    ;

#if BOLTRACE
printf("statement - fim:'%s'\n",b7objp);
#endif

}



#if CICPP
void CIB7::astatement(LONGX *result)
#else /*CICPP*/
void astatement(cib7p,result)
b7_CIB7* cib7p;
LONGX *result;
#endif /*CICPP*/
{
    char *p,*slotp;
    int n,slotl;
    char *acclevp;

#if BOLTRACE
display("astatement");
#endif

    acclevp=b7batchp;

    if (toktyp == TOKTERM && *token == '#') {        /* #{idx/filn} */

        slotp=token; slotl=toklen;

        p=tokbackp; get_token(B7_THIS );

        if (toktyp == TOKDELIM && *token == '=') {

#if BOLTRACE
printf("\n+++ %s",slotp);
#endif
            get_token(B7_THIS );
            expression(B7_THISP result);

            /*** gera assign para o slot */

            nb7oprs++;

            *b7batchp++ = ZASS;
            for (p=slotp, n=slotl; n--; )
                *b7batchp++ = *p++;
            *b7batchp++ = OPRNULL;

            *b7batchp = '\0';     /* tmp end */
        }

        else {

#if BOLTRACE
printf("putback - tokbackp=%s\n",tokbackp);
#endif
            toknextp=p; get_token(B7_THIS );

            expression(B7_THISP result);
        }
    }

    else {

        expression(B7_THISP result);
    }


    for (p=acclevp; p != b7batchp; p++)
#if FIXTRACE
        putchar(*p);
    putchar('\n');
#else
        ; /* next p */
#endif
	if( b7error )
   	return;

    fixup( B7_THISP acclevp,p-1);
#if FIXTRACE
puts(acclevp);
#endif

#if BOLTRACE
printf("astatement  - fim:'%s'\n",b7objp);
#endif

}

#if CICPP
char CIB7::expression(LONGX *result)
#else /*CICPP*/
char expression(cib7p,result)
b7_CIB7 *cib7p;
LONGX *result;
#endif /*CICPP*/
{
    char op;
    int opl;
    LONGX hold;

#if BOLTRACE
display("expression");
#endif

    conjunction(B7_THISP result);
    op=tokopr; opl=tokqty;
    while (op  == ZORX) {
        get_token(B7_THIS );
        conjunction(B7_THISP &hold);
        semaction(B7_THISP TBOOL,op,opl,result,&hold);
        op=tokopr; opl=tokqty;
    }

#if BOLTRACE
printf("expression  - fim - '%s'\n",b7objp);
#endif

    return(PSTLEVEL);

}

#if CICPP
char CIB7::conjunction(LONGX *result)
#else /*CICPP*/
char conjunction(cib7p,result)
b7_CIB7 *cib7p;
LONGX *result;
#endif /*CICPP*/
{
    char op;
    int opl;
    LONGX hold;

#if BOLTRACE
display("conjunction");
#endif

#if !LIND
    conjgop(B7_THISP &hold);
#else
    primitive(B7_THISP &hold);
#endif
    op=tokopr; opl=tokqty;
    while (op == ZAND || op == ZANN) {
        get_token(B7_THIS );
#if !LIND
        conjgop(B7_THISP &hold);
#else
        primitive(B7_THISP &hold);
#endif
        semaction(B7_THISP TBOOL,op,opl,result,&hold);
        op=tokopr; opl=tokqty;
    }

#if BOLTRACE
printf("conjunction  - fim - '%s'\n",b7objp);
#endif

    return(PSTLEVEL);
}



#if !LIND
#if CICPP
char CIB7::conjgop(LONGX *result)
#else /*CICPP*/
char conjgop(cib7p,result)
b7_CIB7 *cib7p;
LONGX *result;
#endif /*CICPP*/
{
    char op;
    int opl;
    LONGX hold;

#if BOLTRACE
display("conjgop");
#endif

    conjfop(B7_THISP result);
    op=tokopr; opl=tokqty;
    while (op == ZANG) {
        get_token(B7_THIS );
        conjfop(B7_THISP &hold);
        semaction(B7_THISP TBOOL,op,opl,result,&hold);
        op=tokopr; opl=tokqty;
    }

#if BOLTRACE
printf("conjgop - fim - '%s'\n",b7objp);
#endif

    return(PSTLEVEL);
}


#if CICPP
char CIB7::conjfop(LONGX *result)
#else /*CICPP*/
char conjfop(cib7p,result)
b7_CIB7 *cib7p;
LONGX *result;
#endif /*CICPP*/
{
    char op;
    int opl;
    LONGX hold;

#if BOLTRACE
display("conjfop");
#endif

    conjpop(B7_THISP result);
    op=tokopr; opl=tokqty;
    while (op == ZANF) {
        get_token(B7_THIS );
        conjpop(B7_THISP &hold);
        semaction(B7_THISP TBOOL,op,opl,result,&hold);
        op=tokopr; opl=tokqty;
    }

#if BOLTRACE
printf("conjfop - fim - '%s'\n",b7objp);
#endif

    return(PSTLEVEL);
}

#if CICPP
char CIB7::conjpop(LONGX *result)
#else /*CICPP*/
char conjpop(cib7p,result)
b7_CIB7 *cib7p;
LONGX *result;
#endif /*CICPP*/
{
    char op;
    int opl;
    LONGX hold;

#if BOLTRACE
display("conjpop");
#endif

    conjeop(B7_THISP result);
    op=tokopr; opl=tokqty;
    while (op == ZANP) {
        get_token(B7_THIS );
        conjeop(B7_THISP &hold);
        semaction(B7_THISP TBOOL,op,opl,result,&hold);
        op=tokopr; opl=tokqty;
    }

#if BOLTRACE
printf("conjpop - fim - '%s'\n",b7objp);
#endif

    return(PSTLEVEL);
}


#if CICPP
char CIB7::conjeop(LONGX *result)
#else /*CICPP*/
char conjeop(cib7p,result)
b7_CIB7 *cib7p;
LONGX *result;
#endif /*CICPP*/
{
    char op;
    int opl;
    LONGX hold;

#if BOLTRACE
display("conjeop");
#endif

    primitive(B7_THISP result);
    op=tokopr; opl=tokqty;
    while (op == ZANE) {
        get_token(B7_THIS );
        primitive(B7_THISP &hold);
        semaction(B7_THISP TBOOL,op,opl,result,&hold);
        op=tokopr; opl=tokqty;
    }

#if BOLTRACE
printf("conjeop - fim - '%s'\n",b7objp);
#endif

    return(PSTLEVEL);
}
#endif /* LIND */


#if CICPP
char CIB7::primitive(LONGX *result)
#else /*CICPP*/
char primitive(cib7p,result)
b7_CIB7 *cib7p;
LONGX *result;
#endif /*CICPP*/
{
    char *p;
    int n;
    int aspc;

#if BOLTRACE
printf /* display() */ ("primitive -  '%s'\n",b7objp);
#endif

    switch (toktyp) {

    case TOKNULL:
        b7_experr(B7_THISP 3,"",0);
        return ' ';

    case TOKDELIM:

        if (tokopr == '(') {
            get_token(B7_THIS );
            expression(B7_THISP result);
            if (tokopr != ')'){
                b7_experr(B7_THISP 4,token,toklen);
                return ' ';
	         }
        }
        else{
            b7_experr(B7_THISP 5,token,toklen);
            return ' ';
	     }

#if BOLTRACE
display("primitive.D");
#endif
        break;


    case TOKTERM:

        /*** gera carga do termo */

        nb7oprs++;

        if (tokqualp) {
            if (tokqualn <= 0)
                fatal("b7_primitive/tokqualn");
            *b7batchp++ = ZQUA;
            for (p=tokqualp, n=tokquall; n--; )
                *b7batchp++ = *p++;
            *b7batchp++ = OPRNULL;
            nb7oprs++;
        }

        *b7batchp++ = ZLDV;
        *b7batchp++ = PSTLEVEL;                 /* room for pstlevel */
        for (aspc=0,p=token, n=toklen; n--; ){
/*
 <R>
   Eliminar los carascteres de escape.
 */
            if( *p == '\\' ){
               memmove( p, p+1, strlen(p) );
               *b7batchp++ = *p++;
               continue;
            }

/* ASPAS */
            if( *p == '"' )
               aspc++;
            *b7batchp++ = *p++;
        }
        if( aspc )
            if( aspc%2 ){
               b7_experr( B7_THISP 10, token, toklen );
               return ' ';
	         }

/* </R> */
        *b7batchp++ = OPRNULL;

        *b7batchp = '\0';     /* tmp end */


#if BOLTRACE
display("primitive.T");
#endif
        break;


    default:

        b7_experr(B7_THISP 6,token,toklen);
        return ' ';
    }


    get_token(B7_THIS );

#if BOLTRACE
printf("primitive - fim - '%s'\n",b7objp);
#endif

    return(PSTLEVEL);
}


#if CICPP
void CIB7::semaction(int   type,
                     char  op,
                     int   opl,
                     LONGX *r,
                     LONGX *h)
#else /*CICPP*/
void semaction(cib7p,type,op,opl,r,h)
b7_CIB7* cib7p;
int type;
char op;
int opl;
LONGX *r,*h;
#endif /*CICPP*/
{
#if BOLTRACE
    printf("semaction - op=%c\n",op);
#endif


    /*** gera operacao com o topo-1 e topo */


    nb7oprs++;

    *b7batchp++ = op;
    *b7batchp++ = opl + '0';
    *b7batchp++ = ACCLEVEL;                     /* room for acclevel */
    *b7batchp++ = OPRNULL;

    *b7batchp = '\0'; /* tmp end */

    op=type+(*r)+(*h); /* no wrn */
}



#if CICPP
void CIB7::fixup(char *p1, char* p2)
#else /* CICPP */
void fixup(cib7p,p1,p2)
b7_CIB7 *cib7p;
char *p1;
char *p2;
#endif /* CICPP */
{
    char *p,c;
    int x;

#if FIXTRACE
printf("fixup "); for (p=p1; p!=p2; p++) putchar(*p);
getchar();
#endif

    if (*p2 != OPRNULL) fatal("*p2 != OPRNULL");

    for (nop=0, p=p1; p != p2; p++) {
        x=0;
        switch (*p) {
                case PSTLEVEL:  c= *(p-1);
                                switch (c) {
                                        case ZLDV:      x='0'; break;
                                        default: fatal("b7_exp/PSTLEVEL");
                                }
                                break;
                case ACCLEVEL:  c= *(p-2);
                                switch (c) {
                                        case ZORX:
                                        case ZAND:
                                        case ZANN:      x='1'; break;
#if !LIND
                                        case ZANG:      x='2'; break;
                                        case ZANF:      x='3'; break;
                                        case ZANP:
                                        case ZANE:      x='4'; break;
#endif /* !LIND */
                                        default: fatal("b7_exp/ACCLEVEL");
                                }
                                break;
        }
        if (x) {
            if (nop >= MAXVOPR){
                b7_experr(B7_THISP -7,"fixup/MAXVOPR",0);
                return;
	         }
            vetopr[nop]=c; vetcas[nop]=x; vetopp[nop]=p;
#if FIXTRACE
printf("#%2d  opr=%c  cas=%c  opp=%.3s\n",
 nop,vetopr[nop],vetcas[nop],vetopp[nop]);
#endif
            nop++;
        }
    }
    if (nop == 0)
        fatal("fixup/nop");
#if FIXTRACE
xtrac=0;
#endif
    fixlev(B7_THISP nop-1,'1');

    /* check '#' */
    for (p=p1; p != p2; p++) {
        if (*p == ZLDV)
            if (*(p+1) >= '1' && *(p+1) <= '4')
                if (*(p+2) == '#')
                    if (*(p+1) != '1'){
                        b7_experr(B7_THISP 22,"#",1);
        		            return;
		    				}
    }
}


#if CICPP
int CIB7::fixlev(int i2, char level)
#else
int fixlev(cib7p,i2,level)
b7_CIB7 *cib7p;
int i2;
char level;
#endif
{
#if FIXTRACE
int n;
for (n=xtrac; n--; ) printf("  ");
printf("[%d] i2=%d level=%c ",xtrac,i2,level);
#endif

    if (i2 < 0) return(i2);

#if FIXTRACE
printf("%.9s\n",vetopp[i2]);
#endif

    *vetopp[i2]=level;

    if (vetopr[i2] != ZLDV) {                   /* operation */
#if FIXTRACE
xtrac++;
#endif
        if (vetcas[i2] > level)
            level=vetcas[i2];

        if (i2 < 2)
            fatal("fixlev/i2<2");
        i2=fixlev(B7_THISP i2-1,level);

        if (i2 < 0)
            fatal("fixlev/i2<0");
        i2=fixlev(B7_THISP i2-1,level);
#if FIXTRACE
xtrac--;
#endif
    }

    return(i2);
}

#define iswhite(c)  isspace(c)
#if CICPP
void CIB7::get_token()
#else
void get_token(cib7p)
b7_CIB7 *cib7p;
#endif
{
    char *toknp,*toknxp,toknx;

    char *tkkbackp;
    char *tkknextp;
    char *tkken;
    char  tkktyp;
    char  tkkopr;
    int   tkkqty;
    int   tkklen;

    char *p;
    int loop;


NEWTOKEN:
    tokmassp=NULL;
    gettoken(B7_THIS);
    tokqualp=NULL;
    tokquall=tokqualn=0;

    switch (toktyp) {

    case TOKDELIM:
            break;

    case TOKTERM:
            if (!*token) {
                toktyp=TOKNULL;
                break;
            }

            for (toknxp=toknextp; iswhite(*toknxp); toknxp++);
            toknx = (*toknxp == '/') ? '/' : '\0';

            while (toklen && iswhite(*(toknextp-1))) {
                toklen--;
                toknextp--;
            }

            toknp=token+toklen;
            while (toklen && iswhite(*(toknp-1))) {
                toklen--;
                toknp--;
            }

            if (b7batchp)                               /* intbrm - 06/01/93 */
                tokmassp=b7_massp(B7_THISP token,toklen,B7EE_PFX);


            if (*(toknp-1) == '/' || toknx == '/') {
                tkkbackp=tokbackp;
                tkknextp=toknextp;
                tkken=token;
                tkktyp=toktyp;
                tkkopr=tokopr;
                tkkqty=tokqty;
                tkklen=toklen;

                if (toknx == '/')
                    toknextp=toknxp+1;
                gettoken(B7_THIS);             /* '(' */
                if (tokopr == '(') {
                    tokqualp=toknextp;
                    gettoken(B7_THIS);         /* 10,20,30,...,qualifier_n */
                    tokquall=toklen;
                    gettoken(B7_THIS);         /* ')' */
                    if (tokopr == ')') {
                        tokqualn=1;
                        for (p=tokqualp, loop=tokquall; loop--; p++) {
                            if (iswhite(*p))
                                continue;
                            if (*p == ',')
                                tokqualn++;
                            if (tokqualn >= MAXQUAL){
                                b7_experr(B7_THISP 7,tokqualp,tokquall);
        		        					return;
			    						}
                        }
                    }
                    else{
                        b7_experr(B7_THISP 8,tokqualp,tokquall);
        					   return;
		    			  }
                    token=tkken;
                    toktyp=tkktyp;
                    tokopr=tkkopr;
                    tokqty=tkkqty;
                    toklen=tkklen;

                    if (toknx != '/')
                        do {
                            toklen--;
                            toknp--;
                        } while (toklen && iswhite(*(toknp-1)));
                }
                else {
                    tokbackp=tkkbackp;
                    toknextp=tkknextp;
                    token=tkken;
                    toktyp=tkktyp;
                    tokopr=tkkopr;
                    tokqty=tkkqty;
                    toklen=tkklen;
                }
            }
            ;
#if MASSAGE
            tokmassp=b7_massp(token,toklen,tokqualp,tokquall,toknextp);
#endif
            ;
            break;

    default:
            fatal("b7_expr/get_token");
    }

#if TOKTRACE
#if !BOLTRACE
    display("get_token");
#endif
#endif

    if (tokmassp) {
        toknextp=tokmassp;
        goto NEWTOKEN;
    }
}


#if CICPP
void CIB7::gettoken()
#else
void gettoken(cib7p)
b7_CIB7 *cib7p;
#endif
                                                                   /*
-------------                                                      */
{
    int n;
    char c,*p;
#if !LIND
    char uc;
#endif /* !LIND */
    int checkpos;     /* RP 29/05/98 */
    int aspc;

    aspc = 0;


    tokbackp=toknextp;    /* para putback */



    /* space and proximity operators */

    if (iswhite(*toknextp)) {

        while (iswhite(*toknextp)) {
            toknextp++;
        }

        p=token=toknextp; c= *p;

        switch (c) {

        case '.':
        case '$':
            if (iswhite(*(p+1))) {
                n=0;
                do {
                    n++; p+=2;
                } while (*p == c && iswhite(*(p+1)));
                toktyp=TOKDELIM;
                tokopr = c;
                tokqty=n;
                toklen=n+n-1;
                toknextp=p;
                return;
            }
            break;
        }
    }


    p=token=toknextp; c= *p;


    /* others delimiters */

    tokqty=1; toktyp=TOKDELIM;

    switch (c) {

        case 'a':
        case 'A':
            if (iswhite(*(p-1)))
                if (toupper(*(p+1)) == 'N')
                    if (toupper(*(p+2)) == 'D')
                        if (iswhite(*(p+3)))
                            if (toupper(*(p+4)) == 'N')
                                if (toupper(*(p+5)) == 'O')
                                    if (toupper(*(p+6)) == 'T')
                                        if (iswhite(*(p+7))) {
                                            toknextp+=7; toklen=7;
                                            c = '^';
                                            tokopr=c; return;
                                        }
            if (iswhite(*(p-1)))
                if (toupper(*(p+1)) == 'N')
                    if (toupper(*(p+2)) == 'D')
                        if (iswhite(*(p+3))) {
                            toknextp+=3; toklen=3;
                            c = '*';
             tokopr=c;
#if !LIND
             if (and2gf == 1) tokopr=ZANG; /* AOT&RP 28/05/98 */
             if (and2gf == 2) tokopr=ZANF; /* AOT&RP 28/05/98 */
#endif /* !LIND */
             return;
                        }
            break;
        case 'o':
        case 'O':
            if (iswhite(*(p-1)))
                if (toupper(*(p+1)) == 'R')
                    if (iswhite(*(p+2))) {
                        toknextp+=2; toklen=2;
                        c = '+';
                        tokopr=c; return;
                    }
            break;
#if !LIND
        case 'w':
        case 'W':
            if (iswhite(*(p-1)))
                if (toupper(*(p+1)) == 'I')
                    if (toupper(*(p+2)) == 'T')
                        if (toupper(*(p+3)) == 'H')
                            if (iswhite(*(p+4))) {
                                toknextp+=4; toklen=4;
                                c = ZANF;
                                tokopr=c; return;
                            }
            break;
#endif

        case '=':
        case '+':
        case '*':
        case '^':
            tokopr = c;
            toklen=1;
            toknextp+=1;
#if !LIND
       if (tokopr == '*') { /* AOT&RP 28/05/98 */
             if (and2gf == 1) tokopr=ZANG; /* AOT&RP 28/05/98 */
             if (and2gf == 2) tokopr=ZANF; /* AOT&RP 28/05/98 */
       }
#endif /* !LIND */
            return;

        case '(':
        case ')':
#if !LIND
            if (c == '(')
                if ((uc = toupper(*(p+1))) == ZANG || uc == ZANF)
                    if (*(p+2) == ')') {
                        tokopr = uc;
                        toklen=3;
                        toknextp+=3;
                        return;
                    }
#endif
            tokopr = c;
            toklen=1;
            toknextp+=1;
            return;

        case '"':
            tokopr='\0';
            toktyp=TOKTERM;
            toklen=tokqty=0;
            toknextp++;
            toklen++;
/*
<R>
ASPAS
 */
             for (; *toknextp; toknextp++, toklen++)
                if (*toknextp == c) {
                    toknextp++;
                    toklen++;
                    return;
                }
                b7_experr(B7_THISP 2,token,toklen);
                return;
/*
</R>
 */

            break;
        default :                          /* RP 29/05/98 begin */
            if (!subst_and) break;
            if (!iswhite(*(p-1))) break;
            for (checkpos = 0; *(subst_and+checkpos); checkpos++)
                if (toupper(*(p+checkpos)) != toupper(*(subst_and+checkpos))) {
                   checkpos = 0;
                   break;
                }
            if (!checkpos) break;
            if (!iswhite(*(p+checkpos))) break;
            toknextp+=checkpos; toklen=checkpos;
            c = '*';
            tokopr=c;
            return;                       /* RP 29/05/98 end */

    }


    /* default */

    tokopr='\0'; toklen=tokqty=0; toktyp=TOKTERM;

    while (*p) {
        toklen++;
        toknextp++;

        switch (*toknextp) {

        case '.':
        case '$':
            if (iswhite(*(toknextp-1)))
                if (iswhite(*(toknextp+1)))
                    return;
            break;
        case '\\':
            toknextp+=2;
            toklen++;
            break;
        case '\0':
        case '+':
        case '*':
        case '^':
        case '(':
        case ')':
/*
   <R> ASPAS
*/
            if( !aspc )
               return;
/* </R> */
        case 'a':
        case 'A':
            if (iswhite(*(toknextp-1)))
                if (toupper(*(toknextp+1)) == 'N')
                    if (toupper(*(toknextp+2)) == 'D')
                        if (iswhite(*(toknextp+3)))
                            return;
            break;
        case 'o':
        case 'O':
            if (iswhite(*(toknextp-1)))
                if (toupper(*(toknextp+1)) == 'R')
                        if (iswhite(*(toknextp+2)))
                            return;
            break;
#if !LIND
        case 'w':
        case 'W':
            if (iswhite(*(toknextp-1)))
                if (toupper(*(toknextp+1)) == 'I')
                    if (toupper(*(toknextp+2)) == 'T')
                        if (toupper(*(toknextp+3)) == 'H')
                            if (iswhite(*(toknextp+4)))
                                return;
            break;
#endif
        default :                          /* RP 29/05/98 begin */
            if (!subst_and) break;
            if (!iswhite(*(toknextp-1))) break;
            for (checkpos = 0; *(subst_and+checkpos); checkpos++)
                if (toupper(*(toknextp+checkpos)) != toupper(*(subst_and+checkpos))) {
                   checkpos = 0;
                   break;
                }
            if (!checkpos) break;
            if (!iswhite(*(toknextp+checkpos))) break;
            return;                       /* RP 29/05/98 end */
        }
    }

    if (*p)
        fatal("gettoken");
}






#if CICPP
void CIB7::b7_experr(int   error,
                     char *errp,
                     int   errl)
#else /*CICPP*/
void b7_experr(cib7p,error,errp,errl)
b7_CIB7 *cib7p;
int error;
char *errp;
int errl;
#endif /*CICPP*/
{
#if ERRTRACE
    char *p;
    int loop;
#endif

    if (errl <= 0) errl=strlen(errp);
    if (errl >= MAXERRL) errl=MAXERRL;

#if ERRTRACE
    printf("b7_experr - error=%d (",error);
    for (p=errp,loop=errl;loop--;p++)
      putchar(*p);
    printf(")\n");
#endif

    strncpy(b7errxy,errp,errl); b7errxy[errl]='\0';

#if FATRAP
    longjmp(b71jumper,error);
#endif /* FATRAP */

    b7error=error?error:1;
    b71error=b7error;

}



#if DISTRACE
#if CICPP
void display(char *s)
#else /*CICPP*/
void display(s)
char *s;
#endif /*CICPP*/
{
#if RECTRACE
    RECSTRU *recp;
    char c;
#endif
    int i,n;
    char *p;

#if BOLTRACE
    printf("%s - token=",s);
    for (p=token, n=toklen; n--; p++) putchar(*p);
    printf(",len=%d,typ=%d",toklen,toktyp);
    printf(",opr=%c,qty=%d",tokopr,tokqty);
    printf(",next=%c",*toknextp);
    if (tokqualp) {
        for (p=s; *p; p++) putchar(' ');
        printf(" - qualp=");
        for (p=tokqualp, n=tokquall; n--; p++) putchar(*p);
        printf(",l=%d,n=%d",tokqualp,tokquall,tokqualn);
    }
    putchar('\n');
}
#endif

#if RECTRACE
 recp=b7recp;
 printf("mfn,mfrl,mfbwb,mfbwp,base,nvf,status: %"_LD_,%d,%"_LD_,%d,%d,%d,%d\n",
  MFRmfn,MFRmfrl,MFRmfbwb,MFRmfbwp,MFRbase,MFRnvf,MFRstatus);
 for (i=0; i<MFRnvf; i++) {
  printf("b7 - i,tag,pos,len=%d,%d,%d,%d (",
   i,DIRtag(i),DIRpos(i),DIRlen(i));
  for (p=FIELDP(i), n=DIRlen(i); n--; )
   putchar(((c = *p++) == NULL) ? '0' : c);
  printf(")\n");
 }
#endif

#if RECTRACE
 recp=b7recp;
 printf("mfn,mfrl,mfbwb,mfbwp,base,nvf,status: %"_LD_,%d,%"_LD_,%d,%d,%d,%d\n",
  MFRmfn,MFRmfrl,MFRmfbwb,MFRmfbwp,MFRbase,MFRnvf,MFRstatus);
 for (i=0; i<MFRnvf; i++) {
  printf("b7 - i,tag,pos,len=%d,%d,%d,%d (",
   i,DIRtag(i),DIRpos(i),DIRlen(i));
  for (p=FIELDP(i), n=DIRlen(i); n--; )
   putchar(((c = *p++) == NULL) ? '0' : c);
  printf(")\n");
 }
#endif

}
#endif

#if CICPP
char *CIB7::b7_massp(char *tokenmassp,
                     int   tokenmassplen,
                     int   b7ee_pfx)
#else /*CICPP*/
char *b7_massp(cib7p,tokenmassp, tokenmassplen, b7ee_pfx)
b7_CIB7 *cib7p;
char *tokenmassp;
int tokenmassplen;
int b7ee_pfx;
#endif /*CICPP*/
{
    char *p;
    int deflen,found;

    if (*tokenmassp == '[') {
   found=0;
   for (p=tokenmassp+1, deflen=0; deflen+1 < tokenmassplen ; p++, deflen++) {
       if (*p == ']') {
      found=1;
      break;
       }
   }


   if (!found) b7_experr(B7_THISP b7ee_pfx,tokenmassp,tokenmassplen);

   /*** gera [XX] */
   nb7oprs++;
   *b7batchp++ = ZPFX;
#if 0
   if (deflen > 9) deflen=9;
   *b7batchp++ = deflen + '0';
#endif
        memmove(b7batchp,tokenmassp+1,deflen); b7batchp+=deflen;
        *b7batchp++ = OPRNULL;

        return(p+1);
    }

    return(NULL);
}


#if !CICPP
/*
  cib7.c - estrutura com o environment de compilacao, execucao e recuperacao dos hits
*/

/*
   Constructor y destructor :-)
 */

b7_CIB7 *b7_cib7(b7_CIB7 *cib7p, void *xciapip) {

   if (!cib7p) {
#if CICPP
      cib7p=(b7_CIB7*)new char[sizeof(b7_CIB7)];
#else
      cib7p=(b7_CIB7*)ALLOC((ALLOPARM)sizeof(b7_CIB7));
#endif
      if (!cib7p) fatal("b7_cib7/ALLOC");
      memset(cib7p,0x00,sizeof(b7_CIB7));  /* AOT, 24/02/2002,
      despues de mucho trabajo en debugar la aplicacion, porque jamais podria
      creer que el projecto B7 podiera haber sido "liberado" con un bug tan
      primitivo: llamar a b7_exp para una expression con error de sintaxis,
                 atrapar el error code y obviamente *no llamar a b7_run* y
                 llamar a b7_cib7_delete()
      que ocurre?  b7_free sera' llamada al azar, porque, npdls no
                   esta' inicializado y solo es setado en b7_run,
                   aunque b7_cib7_delete() obviamente sea llamada si hubo o
                   no hubo error de sintaxis en la expression de busqueda

      peor: el bug fue introduzido durante el desarrollo del zserver

      BTW: Rosnier, que significa su comentario "gress!!! "en el fuente cib72.c?
      */
   }

#if CIAPI
#if !TLS
   ciapip = xciapip;
#endif
#endif

   b70trace=0;
   and2gf=0;          /* AOT&RP 28/05/98 */
   subst_and=NULL; /* RP 29/05/98 */
   b7_gidbnp=NULL;   /* dbn.par */
   b7fd=0;        /* open() - 06/03/95 */
   b7bufferp = NULL;    /* b7_run() processing buffer */
   b7_pfxmdl=0;
   b7error=0;
   b7rootmsg=OFF;
   return (cib7p);
}

b7_CIB7 *b7_cib7_delete(b7_CIB7 *cib7p ) {

   if (!cib7p) return((b7_CIB7 *)NULL);

   for (; npdls; npdls--)
#if CICPP
      b7_free(npdls-1);
#else
      b7_free(cib7p,npdls-1);
#endif

   if (b7bufferp)
#if CICPP
      delete [] b7bufferp;
#else
      FREE(b7bufferp);
#endif

#if CICPP
   delete [] (char *)cib7p;
#else
   FREE((char *)cib7p);
#endif

   return((b7_CIB7 *)NULL);
}
#endif /* CICPP */
