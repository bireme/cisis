/*
 * <R>
 * cib72, b7_run(), b7_hit()
 * basado en b6 usando algo de b4
 * b4 - proximidad
 * b6 - prefix, multiples I/F, log format
 * </R>
 */

/* Alteracoes BIREME:

   1. Arquivo temporario com lista de hits

      nao gravar arquivo com lista de hits se nome do arquivo = "null"
      (campo MFQTHMEM=35 do registro de b7_run - parametro irec)

      ver #if CIB7TMPNULL
      #define CIB7TMPNULL em cisis.h
      AOT, 14/01/2002

   2. B7BUFFER (lista de hits) com dois tipos de lista

      grava bit string ou lista de LONGX integer com os mfn's recuperados
      se for mais economico em termos de espaco

      ver #if CIB7HITLIST
      #define CIB7HITLIST em cisis.h
      AOT, 15/01/2002

   3. Correcao no processamento de '$'

      Rosnier deixou passar warning incorrect assign em "if( mkp[len-1]='$' )"

      ver #if BEFORE20020115
      AOT, 15/01/2002

   4. Correcao do tamanho maximo de CIWRITE

      define tamanho maximo UINT_MAX apenas se for PC 16 bits

      ver #if BEFORE20020115
      AOT, 15/01/2002

   5. Parametro btch0p

      chama fldupdat() com parametro btch0p, como cib62.c

      ver btch0p - AOT, 16/01/2002
      AOT, 16/01/2002

   6. Reprogracao de MFNSTRING na b7_hit()

      nao toca no registro do qrydb

      ver #if CIB7HITLIST
      AOT, 28/01/2002

   7. Correcao "chaves de 29 caracteres"

      testa existencia de aspas antes de mover '\0'

      ver AOT/RP 03/07/2002
      AOT, 03/07/2002

   8. Formato para mais hits nas msgs:
            1    MACACA/SU
           79  MACACA$
          106  Operation +
          106  Set #000000001
      Hits=106
      AOT, 22/12/2006

   9. Alteracao B72SIZE_MAX para B72B72SIZE_MAX

      ver: cib72.c:139:1: warning: "B72SIZE_MAX" redefined
           In file included from /usr/include/netinet/in.h:23,
                            from cisis.h:3526,
                            from cib72.c:107:
           /usr/include/stdint.h:268:1: warning: this is the location of the previous definition
      AOT, 12/01/2007


*/

/*
 * Alteraciones Rosnier
 * Definir BITSTRING, MFNSTRING e CNTSTRING para !LIND.
 * pasarlo para cisis.h o eliminar el uso en caso !LIND.
 */

#if !LIND
#define BITSTRING  (-1)
#define MFNSTRING  0
#define CNTSTRING  1
#endif

/*
 * Eliminar realloc.
 * No se porque esta "reallocando" en memoria que ya esta en uso.
 * ver #if BEFORE20020319
 */


#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

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

#define MIDDLEMSG       01
#define RUITRACE	0
#define RUATRACE	0

#define HITTRACE 0
#define HITTRACx 0


#if BEFORE20020115
#if PC
#define UINTIO		1
#else
#define UINTIO		0
#endif
#else /*BEFORE20020115 */
#if PC && !DOS32BITS  /* AOT, 15/01/2002 */
#define UINTIO		1
#else
#define UINTIO		0
#endif
#endif /*BEFORE20020115 */

#if UINTIO
#define B72SIZE_MAX	UINT_MAX
#else
#define B72SIZE_MAX	INT_MAX
#endif /* UINTIO */

/*
Rosnier: falta implementar esta parte (ver comigo e com Ricardo)
         como subcampo na tabela de prefixos (por exemplo, ^vTW^wMH)
         de maneira a poder usar a feature em otras aplicacoes alem do
         search em MedLine e, sobretudo, nao fazer a cagada de ir sempre
         pesquisar numa arvore MH quando uma chave de uma arvore chamada TW
         tem mais de uma palavra
AOT, 18/05/2001
*/
#define SOING           1       /* CDMDL - text word TI and AB are I
                                           text word MeSH are I/E/P

                                            if dbn="TW"
                                              if key has multiple words
                                                dbn="MH"
                                   (b6tw2mh)
                                */

/*
	definiciones sobre la pila de ejecuación.
*/
#define PDLlvel         pdlp->pdllvel
#define PDLleft         pdlp->pdlleft
#define PDLdocs         pdlp->pdldocs
#define PDLlmfn			pdlp->pdllmfn
#define PDLsize         pdlp->pdlsize
#define PDLisiz         pdlp->pdlisiz
#define PDLplen         pdlp->pdlplen
#define PDLnxtp         pdlp->pdlnxtp
#define VPDLlvel(i)     vpdlp[i]->pdllvel
#define VPDLleft(i)     vpdlp[i]->pdlleft
#define VPDLdocs(i)     vpdlp[i]->pdldocs
#define VPDLlmfn(i)     vpdlp[i]->pdllmfn
#define VPDLsize(i)     vpdlp[i]->pdlsize
#define VPDLisiz(i)     vpdlp[i]->pdlisiz
#define VPDLplen(i)     vpdlp[i]->pdlplen
#define VPDLnxtp(i)     vpdlp[i]->pdlnxtp
#define PDL0            vpdlp[0]
#define PDL0lvel        PDL0->pdllvel
#define PDL0left        PDL0->pdlleft
#define PDL0docs        PDL0->pdldocs
#define PDL0lmfn        PDL0->pdllmfn
#define PDL0size        PDL0->pdlsize
#define PDL0isiz        PDL0->pdlisiz
#define PDL0plen        PDL0->pdlplen
#define PDL0nxtp        PDL0->pdlnxtp

#define vpdltop				npdls-1


#if CICPP
void CIB7::b7_runerr(int error, char *errp, int errl)
#else /*CICPP*/
void b7_runerr(cib7p,error,errp,errl)
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
    if (b7fd) { CLOSE(b7fd); b7fd=0;} 	/* 06/03/95 */

    if (errl <= 0) errl=strlen(errp);
    if (errl > MAXERRL) errl=MAXERRL;
#if ERRTRACE
    printf("b7_runerr - error=%d (",error);
    for (p=errp,loop=errl;loop--;p++)
      putchar(*p);
    printf(")\n");
#endif
    memmove(b7errxy,errp,errl); b7errxy[errl]='\0';
#ifndef CICPP
#if !WWWISIS
    longjmp(b71jumper,error);
    fatal("b7_run/b7_runerr");
#endif /* !WWWISIS */
#endif /* CICPP */
    b7error = error?error:1;

}

/*
	eliminar un nivel de la pila.
*/

#if CICPP
int CIB7::b7_free(int ipdl)
#else /*CICPP*/
int b7_free(cib7p,ipdl)
b7_CIB7 *cib7p;
int ipdl;
#endif
{
	PDLSTRU *pdlp;
   pdlp = vpdlp[ipdl];

   if( pdlp == (PDLSTRU*)ALLONULL )
   	return 0;

#if CICPP
   if( PDLlvel )
   	delete[] PDLlvel;

   delete[] pdlp;
#else /* CICPP */
   if( PDLlvel )
   	FREE( PDLlvel );

   FREE( pdlp );
#endif /* CICPP */

   npdls--;
   return 0;
}

/*
	intercambia 2 niveles de la pila.
*/

#if CICPP
int CIB7::b7_xpdl(int ipdl1, int ipdl2 )
#else /*CICPP*/
int b7_xpdl( cib7p,ipdl1, ipdl2 )
b7_CIB7 *cib7p;
int ipdl1;
int ipdl2;
#endif
{

	PDLSTRU *tmp;

   tmp = vpdlp[ ipdl1 ];
   vpdlp[ ipdl1 ]=vpdlp[ ipdl2 ];
   vpdlp[ ipdl2 ] = tmp;

   return 1;

}

/*
	Inicializa un PDLSTRU.
*/

#if CICPP
char *CIB7::b7_initpdl(PDLSTRU *pdlp, char postlen, LONGX lastmfn)
#else /*CICPP*/
char *b7_initpdl(cib7p,pdlp, postlen, lastmfn)
b7_CIB7 *cib7p;
PDLSTRU *pdlp;
char postlen;	/* tamaño de los postings */
LONGX lastmfn;	/* último MFN de la base. */
#endif /*CICPP*/
{

   switch( postlen ){
	   case '1': PDLplen=0; break;	/* one bit per mfn */
	   case '2': PDLplen=5; break;   /* MFN+TAG */
       case '3': PDLplen=6; break;   /* MFN+TAG+OCC */
       case '4': PDLplen=8; break;   /* MFN+TAG+OCC+CNT */
       default : PDLplen=0;
   }

   if( !PDLplen )
   	PDLsize=lastmfn/8+1;	/*bitstring.*/
   else
   	PDLsize=lastmfn*PDLplen;

   PDLisiz = PDLsize;

#if CICPP
   if( PDLlvel ) delete[] PDLlvel;

   try {PDLlvel= (UCHR *)new char[(ALLOPARM)PDLsize]; }
   catch(BAD_ALLOC) { PDLlvel = (UCHR *)ALLONULL; }
#else /* CICPP */
   if( PDLlvel ) FREE(PDLlvel);

   PDLlvel= (char*)ALLOC((ALLOPARM)(PDLsize));
#endif /* CICPP */
   if (PDLlvel == (UCHR *)ALLONULL)
   {
   	b7_runerr(B7_THISP 3,"b7_ipdl/ALLOC/PDLlvel",0);
      return (char *)ALLONULL;
   }
   memset(PDLlvel,0x00,PDLsize);

   PDLleft=PDLsize;

   PDLdocs=0;
   PDLnxtp=(char *)PDLlvel;
   PDLlmfn=lastmfn;

   return (char *)PDLlvel;

}

/*
	adicionar PDLSTRU a la pila en el nivel ipdl.
*/

#if CICPP
PDLSTRU *CIB7::b7_ipdl(int ipdl)
#else /*CICPP*/
PDLSTRU *b7_ipdl(cib7p,ipdl)
b7_CIB7 *cib7p;
int ipdl;
#endif /*CICPP*/
{
	PDLSTRU *pdlp;
   if (ipdl >= npdls) {
		if (ipdl > npdls)
      	fatal("b7_ipdl/ipdl");
		if (npdls >= MAXPDLS)
		{
      	b7_runerr(B7_THISP 2,"b7_ipdl/MAXPDLS",0);
	    	return 0;
      }

#if CICPP
      try { pdlp=(PDLSTRU *)new char[(ALLOPARM)(sizeof(PDLSTRU))]; }
      catch(BAD_ALLOC) { pdlp=(PDLSTRU *)NULL; }
#else /* CICPP */
      pdlp=(PDLSTRU *)ALLOC((ALLOPARM)(sizeof(PDLSTRU)));
#endif /* CICPP */

      memset((char *)pdlp,0x00,sizeof(PDLSTRU));

      if (pdlp == (PDLSTRU *)ALLONULL)
		{
      	b7_runerr(B7_THISP 3,"b7_ipdl/ALLOC/pdlp",0);
      	return 0;
	 	}
#if RUATRACE
printf("ALLOC - ipdl=%d  pdlp=%p\n",ipdl,pdlp);
#endif
      vpdlp[npdls++]=pdlp;
	}
   else
   	pdlp=vpdlp[ipdl];

	return(pdlp);
}

/*
	guess!!!
*/
#if CICPP
PDLSTRU *CIB7::b7_push()
#else
PDLSTRU *b7_push(cib7p)
b7_CIB7 *cib7p;
#endif
{
	return b7_ipdl( B7_THISP npdls );
}

/*
	guess!!!
*/
#if CICPP
int CIB7::b7_pop()
#else
int b7_pop(cib7p)
b7_CIB7 *cib7p;
#endif
{
	b7_free( B7_THISP npdls-1);
   return 1;
}

#if CICPP
int CIB7 :: b7_pfx(RECSTRU *recp, UWORD invxtag, char *dp, char *op, int b7re_pfx)
#else /* CICPP */
int b7_pfx(cib7p,irec, invxtag, dp, op, b7re_pfx)
b7_CIB7 *cib7p;
LONGX irec;
UWORD invxtag;
char *dp;
char *op;
int b7re_pfx;
#endif /* CICPP */
{
    int found=0;
    unsigned char *fldp,*pfxp,*dbnp,*usep,*kp,*msgp;
    int xdir;
    FFI pfxl,dbnl,usel,deflen,opxlen,kl,n,msgl;
    char *tmp;

    int aspas;
    
#if !CICPP
    RECSTRU *recp;
    recp=vrecp[irec];
#endif
    if (!recp) fatal("b7_pfx/irec");

#if CICPP
    if (!recp->recmfp) fatal("b7_pfx/recmfp");
#endif /* CICPP */

    b7_pfxtwx[0]='\0';
    b7_pfxlen=0;
    *b7_pfxmsg='\0';
    *b7_pfxdbn='\0';
    strcpy(b7_pfxopx,op);
    opxlen=kl=strlen((const char *)(kp=(unsigned char *)op));
    deflen=strlen(dp);
#if INVXTRAC
printf("0. dbname=%s,opx=%s<== \n",b7_pfxdbn,b7_pfxopx);
#endif
#if CICPP
    if (opxlen > 0) {
#else /* CICPP */
    if (irec && opxlen > 0) {
#endif /* CICPP */
        for (xdir=0; xdir < MFRnvf; xdir++) {
#if INVXTRAC
 printf("xdir,tag: %d,%d \n",xdir,DIRtag(xdir));
#endif
            if (DIRtag(xdir) == invxtag) {
                fldp=(unsigned char *)FIELDP(xdir);
                pfxl=DIRlen(xdir);
                pfxp=subfldp(fldp,'p',&pfxl);
                pfxl=subfldn(pfxp,pfxl);
                if (!pfxl) fatal("b7_pfx/invxtag/^p");
                if (pfxl == 1 && *pfxp == '*') continue;
                dbnl=DIRlen(xdir);
                dbnp=subfldp(fldp,'y',&dbnl);
                dbnl=subfldn(dbnp,dbnl);
                if (!dbnl) fatal("b7_pfx/invxtag/^y");
                usel=DIRlen(xdir);
                usep=subfldp(fldp,'u',&usel);
                usel=subfldn(usep,usel);
                if (usel > LE2) fatal("b7_pfx/invxtag/^u");
                if (opxlen >= pfxl) {
                	if (strncmp(b7_pfxopx,(const char *)pfxp,pfxl) == 0) {
                        memmove(b7_pfxdbn,dbnp,dbnl);
                        b7_pfxdbn[dbnl]='\0';
                        if (usel) memmove(b7_pfxopx,usep,usel);

/* <R>  */
                        if( op[pfxl] == '"' ){
                        	aspas=1;
                           tmp = strrchr((char *)op+pfxl+1, (int)'"' );
                           if (tmp) /* AOT/RP 03/07/2002 */ *tmp = '\0';
                        }else{
                        	aspas=0;
                        }
/* </R> */
								strcpy(b7_pfxopx+usel,op+pfxl+aspas);
								found=1;
#if INVXTRAC
 printf("1. dbname=%s,opx=%s<== \n",b7_pfxdbn,b7_pfxopx);
#endif
                        break;
                    }
                }
            }
        }
    }
#if INVXTRAC
 printf("1. defpfx=%s(%d),op,opx: %s,%s[%s] (%d)\n",
  dp,deflen,op,b7_pfxopx,b7_pfxdbn,found);
#endif
#if CICPP
    if (found == 0) {
#else /* CICPP */
    if (irec && found == 0) {
#endif /* CICPP */
        if (strlen(dp)) {
            for (xdir=0; xdir < MFRnvf; xdir++) {
#if INVXTRAC
 printf("1. xdir,tag: %d,%d \n",xdir,DIRtag(xdir));
#endif
                if (DIRtag(xdir) == invxtag) {
                    fldp=(unsigned char *)FIELDP(xdir);
		              pfxl=DIRlen(xdir);
                    pfxp=subfldp(fldp,'p',&pfxl);
                    pfxl=subfldn(pfxp,pfxl);
                    if (pfxl == 1 && *pfxp == '*') continue;
		    if (pfxl < deflen) continue;
		    n=pfxl-deflen;
		    if (strncmp((const char *)pfxp,dp,deflen) != 0) continue;
		    if (strncmp((const char *)(pfxp+deflen),(const char *)kp,n) != 0) continue;
                    dbnl=DIRlen(xdir);
                    dbnp=subfldp(fldp,'y',&dbnl);
                    dbnl=subfldn(dbnp,dbnl);
                    usel=DIRlen(xdir);
                    usep=subfldp(fldp,'u',&usel);
                    usel=subfldn(usep,usel);
                    memmove(b7_pfxdbn,dbnp,dbnl); b7_pfxdbn[dbnl]='\0';
                    if (usel) memmove(b7_pfxopx,usep,usel);
/* <R>  */
                    if( kp[n] == '"' ){
                    		aspas=1;
                     	tmp = strrchr((char *)kp+n+1, (int)'"' );
                     	if (tmp) /* AOT/RP 03/07/2002 */ *tmp = '\0';
                    }else{
                    		aspas=0;
                    }
/* </R> */
                    memmove(b7_pfxopx+usel,kp+n+aspas,kl-n);
                    b7_pfxopx[usel+kl-n]='\0';
                    found=2;
#if INVXTRAC
 printf("2. dbname=%s,opx=%s<== \n",b7_pfxdbn,b7_pfxopx);
#endif
		    break;
		}
	    }
	    if (!found)
	    {
		b7_runerr(B7_THISP b7re_pfx,dp,0);
                return(0);
		/*goto B7_RUNERROR;*/
	    }
	}
    }
#if INVXTRAC
 printf("2. defpfx=%s(%d),op,opx: %s,%s[%s] (%d)\n",
  dp,deflen,op,opx,b7_pfxdbn,found);
#endif
#if !CICPP
    if (irec) {
#endif /* !CICPP */
	if (found == 0) {
	    for (xdir=0; xdir < MFRnvf; xdir++) {
#if INVXTRAC
 printf("2. xdir,tag: %d,%d \n",xdir,DIRtag(xdir));
#endif
		if (DIRtag(xdir) == invxtag) {
		    fldp=(unsigned char *)FIELDP(xdir);
		    pfxl=DIRlen(xdir);
		    pfxp=subfldp(fldp,'p',&pfxl);
		    pfxl=subfldn(pfxp,pfxl);
		    if (pfxl != 1) continue;
		    if (*pfxp != '*') continue;
		    dbnl=DIRlen(xdir);
		    dbnp=subfldp(fldp,'y',&dbnl);
		    dbnl=subfldn(dbnp,dbnl);
		    usel=DIRlen(xdir);
		    usep=subfldp(fldp,'u',&usel);
		    usel=subfldn(usep,usel);
		    memmove(b7_pfxdbn,dbnp,dbnl);
		    b7_pfxdbn[dbnl]='\0';
		    if (usel) memmove(b7_pfxopx,usep,usel);
/* <R>  */
          if( op[0] == '"' ){
          		aspas=1;
               tmp = strrchr((char *)op+1, (int)'"' );
               if (tmp) /* AOT/RP 03/07/2002 */ *tmp = '\0';
          }else{
               aspas=0;
          }
/* </R> */

		    strcpy(b7_pfxopx+usel,op+aspas);
		    found=3;
#if INVXTRAC
 printf("3. dbname=%s,opx=%s<== \n",b7_pfxdbn,b7_pfxopx);
#endif
		    break;
		}
	    }
	}
#if !CICPP
    }
#endif /* !CICPP */
    if (found) {
	b7_pfxlen=usel;
	msgl=DIRlen(xdir);
	msgp=subfldp((unsigned char *)FIELDP(xdir),'m',&msgl);
	msgl=subfldn(msgp,msgl);
	if (msgl >= sizeof(b7_pfxmsg)) fatal("b7_pfx/invxtag/^m");
	memmove(b7_pfxmsg,msgp,msgl); b7_pfxmsg[msgl]='\0';
	msgl=DIRlen(xdir);
	msgp=subfldp((unsigned char *)FIELDP(xdir),'w',&msgl);
	msgl=subfldn(msgp,msgl);
	if (msgl >= sizeof(b7_pfxtwx)) fatal("b7_pfx/invxtag/^w");
	memmove(b7_pfxtwx,msgp,msgl); b7_pfxtwx[msgl]='\0';
	msgl=DIRlen(xdir);
	msgp=subfldp((unsigned char *)FIELDP(xdir),'x',&msgl);
	b7_pfxmdl = (msgp) ? 1 : 0;
    }
#if INVXTRAx
 printf("3. defpfx=%s(%d),op,opx: %s,%s[%s] (%d)\n",
  dp,deflen,op,b7_pfxopx,b7_pfxdbn,found);
#endif

	/*	27.Feb.2003 RP - begin */
	if ( aspas )
	{
   	char *auxString = strdup(b7_pfxopx);

		sprintf(b7_pfxopx,"\"%s\"",auxString);
#if CICPP 
      free(auxString);
#else /* CICPP */
      FREE(auxString);
#endif /* CICPP */
	}
	/*	27.Feb.2003 RP - end */
    return(found);
}


/*
	b7_run()
	ejecuta la búsqueda :-)
 */
#if CICPP
char *CIB7::b7_run(RECSTRU *irecp, char *dbnamp,
                char *buffup, int buffuplen, LONGX tell,
                unsigned char uctab[], char *btch0p, RECSTRU *tlirecp,
		RECSTRU *crecp, int *errnop)
#else /* CICPP */
char *b7_run(cib7p,irec,dbnamp,buffup,buffuplen,tell,uctab,
                        btch0p,tlirec,crec,errnop)           /*

------------
                    executa query em dbnamp;
                    retorna NULL ou ptr erro
                                                                        */
b7_CIB7 *cib7p;
LONGX irec;				/* recp, para store */
char *dbnamp;				/* ptr nome base de dados a ler */
char *buffup;				/* area addr for fldupdat */
int buffuplen;				/* buffup length */
LONGX tell;          		/* b7_msg call */
unsigned char uctab[];		/* uctab */
char *btch0p;       		/* initial fldupdat */
LONGX tlirec;			/* invx definition */
LONGX crec;				/* registro de controle de qrydbnp */
int *errnop;        		/* addr para codigo de erro */
#endif /* CICPP */
{
   RECSTRU *recp;
   TRMSTRU *trmp;
   PDLSTRU *pdlp;						/* pilha de execucao */

   unsigned char *b7p;

   unsigned char *q, *p, *op1, *res, *batchp, *p0p, *mtp, *mkp;

   char postlen;

   int b7opxlen;

/* <R>
	Pasadas a b7_CIB7.
   short unsigned int nqualts,vqualt[MAXQUAL],*qualtp;
 </R>
 */

   int b7rootf;
   LONGX b7rootloop;

   unsigned char b7opr;
   unsigned char b7qty;
   unsigned char b7pst;
   unsigned char b7acc;
   unsigned char b7opx[2+LE2+1];
   LONGX lastmfn;
   LONGX offset, loop;

   int xdir_obj0,xdir_dbn0;

   int xdir_hmem;
   int xdir_xcts,n;
   FFI nffi;

   LONGX nord,mfn,left;
   LONGX hits,hsiz,lastcc,bytes;

   time_t wtime;

   int b7fd1;

   LONGX npdlsb;

   char buffup1[1024];

#if INVXBOOL
    unsigned char /*tl_dbname[CIMPL+1],*/tl_opx[LE2+LE2+1];
#endif

   int tl_deflen;
   unsigned char tl_defpfx[LE2+1];

   DBXSTRU *dbxp;

#if MIDDLEMSG
   LONGX count;
#endif

#if CDMDL
    int dctlen,qlflen;
#endif

//#if CIB7HITLIST  /* AOT, 15/01/2002 */
//    LONGX bytsneed=LONGX_MAX;
//    int hcase=BITSTRING;
//#endif

#if !CICPP
   RECSTRU *tlirecp;
#endif /* CICPP */

   *errnop=0;

   if (!ndbxs) dbxinit();  /* init vdbxp/vrecp/vtrmp if not yet init - AOT, 28/10/2005 */
                
   tl_defpfx[0] = '\0';
   nqualts=0;

#if CICPP
   recp = irecp;
#else /* CICPP */
   recp=vrecp[irec];
#endif /* CICPP */
   if (!recp)
		fatal("b7_run/irec");

#if !CICPP
   tlirecp=vrecp[tlirec];
#endif /* CICPP */
   if (!tlirecp)
		fatal("b7_run/tlirec");

   npdls=0;
   b7itrm=0;
   lastcc=0;

#if CICPP
   trmp = new TRMSTRU(cisisxp);
   trmp->xtrmalloc(IFBSIZ);
#else /* CICPP */
   for (b7itrm=maxntrm; --b7itrm > 0; )
   	if (!vtrmp[b7itrm]) {
      	trmalloc(b7itrm,IFBSIZ);
         break;
      }
   if (b7itrm == 0)
   	b7_runerr(B7_THISP 8,"b7_run/itrm",0);
#endif /* CICPP */

B7_RUNERROR:
	if (b7error != 0) {
#if ERRTRACE
		printf("setjmp - error=%d (%s)\n",b7error,b7errxy);
#endif
		sprintf(buffup,B7DELADD,MFQTALCC,MFQTALCC,(LONGX)b7error);
#if CICPP
		recp->xfldupdat(buffup); /* try it */
#else /* CICPP */
		fldupdat(irec,buffup); /* try it */
#endif /* CICPP */

		if (RECdbxp) {
      	RECtype=TYPEMFR;

#if CICPP
      	recp->xrecupdat();   /* put it on disk */
#else /* CICPP */
      	recupdat(crec,irec);   /* put it on disk */
#endif /* CICPP */

      	RECtype=TYPEMFQ;
		}
		if (b7error == 222) {
      	b7error=0;
		}

		*errnop=b7error;

		return(b7errxy);
	}


#if CICPP
   recp = irecp;
#else /* CICPP */
	recp = vrecp[irec];
#endif /* CICPP */
   b7tell = tell;
   b7setno=MFRmfn;

   /* setup MFQTXCTS and MFQTAxxx and MFQTHxxx */
   RECtype=TYPEMFR;
   batchp=(unsigned char *)buffup;
   sprintf((char *)batchp,"A%d|^%c%"_LD_,MFQTXCTS,MFQSWTIM,time(&wtime));
   batchp+=strlen((const char *)batchp);

#if CICPP
   if ((xdir_dbn0=recp->xfieldx(MFQTDBN0,1)) < 0){
#else /* CICPP */
   if ((xdir_dbn0=fieldx(irec,MFQTDBN0,1)) < 0){
#endif /* CICPP */
		b7_runerr(B7_THISP B7RE_1,"b7_run/xdir_dbn0",0);
		goto B7_RUNERROR;
   }

   if (strlen(dbnamp) != DIRlen(xdir_dbn0) || strncmp(dbnamp,FIELDP(xdir_dbn0),DIRlen(xdir_dbn0))) {
		sprintf((char *)batchp,"^%c%s",MFQSWDBN,dbnamp);
		batchp+=strlen((const char *)batchp);
   }

   sprintf((char *)batchp,"^%c",MFQSWQTY);
   batchp+=strlen((const char *)batchp);
   sprintf((char *)batchp,B7EDIT1,0L);
   batchp+=strlen((const char *)batchp);
   *batchp++ = '|';
   sprintf((char *)batchp,B7DELADD,MFQTASET,MFQTASET,0L);
   batchp+=strlen((const char *)batchp);
   sprintf((char *)batchp,B7DELADD,MFQTALCC,MFQTALCC,lastcc);
   batchp+=strlen((const char *)batchp);
   sprintf((char *)batchp,B7DELADD,MFQTHITS,MFQTHITS,0L);
   batchp+=strlen((const char *)batchp);
   sprintf((char *)batchp,B7DELADD,MFQTHCUR,MFQTHCUR,0L);
   batchp+=strlen((const char *)batchp);
   sprintf((char *)batchp,B7DELADD,MFQTHMFN,MFQTHMFN,0L);
   batchp+=strlen((const char *)batchp);
   sprintf((char *)batchp,B7DELADD,MFQTHSIZ,MFQTHSIZ,0L);
 /*  batchp+=strlen((const char *)batchp); */

#if CICPP
   if ((p=(unsigned char *)recp->xfldupdat(buffup)) != NULL)
   	b7_runerr(B7_THISP B7RE_2,(char *)p,0);
   if (btch0p)
   if ((p=(unsigned char *)recp->xfldupdat(btch0p)) != NULL)         /* btch0p - AOT, 16/01/2002 */
   	b7_runerr(B7_THISP B7RE_2,(char *)p,0);

   if ((xdir_hmem=recp->xfieldx(MFQTHMEM,1)) < 0) {
   	if (!RECdbxp)
      	fatal("b7_run/RECdbxp");
   	sprintf(buffup,"A%d|%s.%03x|",MFQTHMEM,RDBname,(int)b7setno);
   	if ((p=(unsigned char *)recp->xfldupdat(buffup)) != NULL)
      	b7_runerr(B7_THISP B7RE_2,(char *)p,0);
   	sprintf(buffup,"%s.%03x",RDBname,(int)b7setno);
#else /* CICPP */
   if ((p=fldupdat(irec,buffup)) != NULL)
   	b7_runerr(B7_THISP B7RE_2,(char *)p,0);
   if (btch0p)
   if ((p=fldupdat(irec,btch0p)) != NULL)                            /* btch0p - AOT, 16/01/2002 */
   	b7_runerr(B7_THISP B7RE_2,(char *)p,0);

   if ((xdir_hmem=fieldx(irec,MFQTHMEM,1)) < 0) {
   	if (!RECdbxp)
      	fatal("b7_run/RECdbxp");
   	sprintf(buffup,"A%d|%s.%03x|",MFQTHMEM,RDBname,(int)b7setno);
   	if ((p=fldupdat(irec,buffup)) != NULL)
      	b7_runerr(B7_THISP B7RE_2,(char *)p,0);
   	sprintf(buffup,"%s.%03x",RDBname,(int)b7setno);         /* build file name for results in buffup */
#endif /* CICPP */
   }else {
   	memmove(buffup,FIELDP(xdir_hmem),DIRlen(xdir_hmem));     /* or get it from irec into buffup */
   	buffup[DIRlen(xdir_hmem)]='\0';
   }

   
#if CIB7TMPNULL
   if (strcmp(buffup,"null")) {   /* do not write hits (bit string) to disk
                                     note: b7_run() alloc b7buffer if needed and 
                                           stores hits in b7buffer at end of execution
                                           therefore, b7buffer still can be used by b7_hit()
                                           (or used directly by the caller..)
                                     AOT - 14/01/2002
                                  */
#endif /* CIB7TMPNULL */

       b7fd=CREAT(dbxcipar(b7_gidbnp,buffup,'='),PERMIS);
       if (b7fd > 0)
   	    CLOSE(b7fd);

       n=dbxopt_ordwr; dbxopt_ordwr=O_RDWR;
       b7fd=dbxopen(b7_gidbnp,buffup,"");
       dbxopt_ordwr=n;

#if CIB7TMPNULL
   }                              /* AOT - 14/01/2002 */
#endif /* CIB7TMPNULL */


#if CICPP
   if ((xdir_obj0=recp->xfieldx(MFQTOBJ0,1)) < 0){
#else /* CICPP */
   if ((xdir_obj0=fieldx(irec,MFQTOBJ0,1)) < 0){
#endif /* CICPP */
   	    b7_runerr(B7_THISP B7RE_3,"b7_run/xdir_obj0",0);
		goto B7_RUNERROR;
   }

	dbxp = dbxstorp(dbnamp);
    lastmfn = DBXmsmfn;

#if CICPP
   if ((xdir_obj0=recp->xfieldx(MFQTOBJ0,1)) < 0)
#else /* CICPP */
   if ((xdir_obj0=fieldx(irec,MFQTOBJ0,1)) < 0)
#endif /* CICPP */
   	fatal("b7_run/fieldx");

   b7p=(unsigned char *)FIELDP(xdir_obj0);

   while (*b7p != OPRXEND) {
#if RUITRACE
   	b7_showpdl( B7_THIS );
#endif
   	b7opr = *b7p++;
      postlen = *b7p;
      switch (b7opr) {
      	case ZQUA:
            for (nqualts=0; *b7p; ) {
                if (*b7p == OPRNULL)
                    break;
                for( ;isspace(*b7p); b7p++) ;
                for (n=0; isdigit(*b7p); b7p++)         /* convert tag */
                    n = 10 * n + (*b7p - '0');
                if (nqualts >= MAXQUAL)
                    fatal("b7_run/MAXQUAL");
                vqualt[nqualts++]=n;
                if (*b7p == ',')
                    b7p++;
            }
            vqualt[nqualts]=0;                          /* end */
            b7pst = '0';
            b7qty = '0';
            b7acc = '0';
         	break;
			case ZPFX:
	    		for (tl_deflen=0; *b7p != OPRNULL; )
					tl_defpfx[tl_deflen++] = uctab[*b7p++];
            if (tl_deflen)
            	if (isalpha(tl_defpfx[tl_deflen-1]))
               	tl_defpfx[tl_deflen++] = ' ';
            tl_defpfx[tl_deflen]='\0';
            b7pst = '0';
	    		b7qty = '0';
	    		b7acc = '0';
	    		break;

      	case ZLDV:
         	b7pst = *b7p++;
            b7qty = '0';
            b7acc = '0';
            break;
      	default:
            b7pst = '0';
            b7qty = *b7p++;
            b7acc = *b7p++;
      }


      q=(unsigned char *)buffup; /* buffup is available */
      for (b7opxlen=0; *b7p; ) {
      	if (*b7p == OPRNULL)
         	break;
         *q++ = *b7p++;
         b7opxlen++;
      }
      *q='\0';
      b7rootf=OFF;
      b7rootloop=0;
   	*b7opx='\0';

      if (b7opxlen) {
      	if (buffup[strlen(buffup)-1] == '$') {
         	b7rootf=ON;
            b7opxlen--;
         }
         if (b7_xlkey(B7_THISP (char *)b7opx,buffup,uctab,NULL,&b7opxlen) == NULL)
         	b7_runerr(B7_THISP 10,"b7_xlkey",0);
      }

      if (*b7p != OPRNULL)
      	fatal("b7_run/OPRNULL");
      b7p++;

      switch (b7opr) {
      	case ZQUA:
#if RUXTRACE
				if (b70trace) {
					printf("->");
					for (qualtp=vqualt; *qualtp; qualtp++)
						printf("%d,",*qualtp);
					printf("nq=%d\n",nqualts);
				}
#endif
            break;
			case ZPFX:
#if RUXTRACE
				if (b70trace) {
					printf("->");
				}
#endif
	    		break;

/*
	<R>
		Aca empiezan los cambios :-)
	</R>
 */
      	case ZLDV:
		/* oper _ */

         	npdlsb = npdls;

			/*Cargar resultado de búsqueda anterior almacenado en HD */
         	if (RECdbxp && *b7opx == '#' && b7opxlen > 1) {
            	mfn=atol((const char *)b7opx+1);
               for (p=b7opx+1; *p; p++)
		    		if (!isdigit(*p)) {
               	mfn= -1;
                  break;
               }
               if( mfn<0 )
               	break;

               pdlp=b7_push(B7_THIS);
               b7_initpdl(B7_THISP pdlp, 0, lastmfn);

               if( mfn == 0 ){ /* Marcar todos los registros ... */
                  /* 1er bit=0 */
                  *PDLlvel=0x7F;
                  /* resto de los bits a 1 */
                  memset( PDLlvel+1, 0xFF, PDLsize-1);
                  /* 0s a partir de lmfn */
                  p0p=PDLlvel+PDLsize-1; /*last byte. */
                  for( mfn=PDLsize*8-1;mfn>PDLlmfn;mfn--)
                  	*p0p &= ~bitmask[mfn&BY8RMASK];
               }else{

                  /* should be read from qrydbnp, field MFQTHMEM - AOT, 28/01/2002 */
                  sprintf(buffup1,"%s.%03x",RDBname,(int)mfn);
   					b7fd1=dbxopen(b7_gidbnp,buffup1,""); /* result log */

                  if( ( CIREAD( b7fd1, PDLlvel, PDLsize ) )!= PDLsize )
                  	break;

                  if (b7fd1 > 0) CLOSE(b7fd1);

               }
               PDLdocs = b7_hsizh( B7_THISP pdlp, &hsiz );
            }else{

#if CICPP
               int AuxError = b7_pfx(tlirecp,MFQTXTAG,(char *)tl_defpfx,(char *)b7opx,B7RE_PFX);
               if (b7error) goto B7_RUNERROR;
               if (AuxError) {
#else /* CICPP */
               if (b7_pfx(B7_THISP tlirec,MFQTXTAG,(char *)tl_defpfx,(char *)b7opx,B7RE_PFX)) {
#endif /* CICPP */
                  mtp = (unsigned char *)b7_pfxdbn;
                  mkp = (unsigned char *)b7_pfxopx;
#if CDMDL
                /* MDL Medical Subject Heading is [.]mesh[/[xx]] */
                	if (b7_pfxmdl && b7opxlen > LE2) {
                  	dctlen=LE2-1-3;
                    	if (tl_opx[0] == '.') dctlen++;
                    	qlflen=0;
                    	if (tl_opx[b7opxlen-1] == '/') qlflen=1;
                    	if (tl_opx[b7opxlen-3] == '/') qlflen=3;
                    	memmove(buffup,tl_opx,dctlen);
                    	memmove(buffup+dctlen,tl_opx+b7opxlen-qlflen,qlflen);
                    	b7opxlen=dctlen+qlflen;
                    	memmove(tl_opx,buffup,b7opxlen);
                    	tl_opx[b7opxlen]='\0';
                	}
#endif
               }else{
                  mtp = (unsigned char *)dbnamp;
                  mkp = (unsigned char *)b7opx;
               }
               if( (mkp[0] == '"') && (mkp[strlen((const char *)mkp)-1] == '"' ) ){
                  int len;
               	mkp++;
                  len = strlen((const char *)mkp)-1;
#if BEFORE20020115 /* BC5: Possibly incorrect assignment... */
                  if( mkp[len-1]='$' ){
#else
                  if( mkp[len-1]=='$' ){ /* AOT 15/01/2002 */
#endif
                  	mkp[len-1] = '\0';
                     b7rootf=ON;
                  }else{
                  	mkp[len] = '\0';
                  }
               }

#if CDMDL /* D-Day */
#if SOING /* if ^y == TW and key has multiple words then search on MH */
            	if (b7tw2mh) {
               	if (strcmp(mtp,"TW") == 0) {                      /* tree */
                  	for (n=0, p=mkp; *p; p++)                       /* key */
                     	if (!isiswctab[*p]) { n++; break; }
                		if (n) mtp="MH";
              		}
            	}
#endif /* SOING */
#endif /* CDMDL */

#if CICPP
               trmp->xterm(mtp,mkp);
#else /* CICPP */
               TERM( b7itrm, mtp,mkp);
#endif /* CICPP */

               pdlp=b7_push(B7_THIS);
               b7_initpdl(B7_THISP pdlp, postlen, lastmfn);

               hits=0; hsiz=0;
               for(;;){
				   	if( b7rootf ){ /* right truncate ... */
               		if (TRMrc == RCEOF) break;
#if BEFORE20010827
                     	if( strncmp((const char *) TRMkey, b7_pfxopx, strlen( b7_pfxopx ) ) ){
#endif
                     	if( strncmp((const char *) TRMkey, (const char *)mkp, strlen( (const char *)mkp ) ) ){
                        	break;
                     }
                    	if (b7tell >= 2 ) {
                    		count=TRMxytotp;
#if !CICPP
								b7_msg(B7_THISP 1,TRMkey,0,count,b7rootmsg ? 1 : 4); /* 1st_msg */
#endif /* CICPP */
							}

							if( PDLdocs && PDLplen ){
                        pdlp=b7_push(B7_THIS);
                        b7_initpdl(B7_THISP pdlp, postlen, lastmfn);
							}

                  }else if (TRMrc != RCNORMAL){
                     break;
                  }

                  for( nord=0;; ){
#if CICPP
                     mfn=trmp->xposting(++nord);
#else /* CICPP */
                     mfn=posting(b7itrm,++nord);
#endif /* CICPP */		     
                     if( mfn <= 0 )
                        break;

                     if (nqualts) {
                        for (n=0, qualtp=vqualt; *qualtp; qualtp++)
                           if (TRMptag == *qualtp) {
                              n=1; break;
                           }
                        if (n)
                            ;           /* qualifiers list */
                        else
                            continue;   /* goto next posting */
                     }	     
#if !LIND
                     if( !PDLplen ){
#endif /* !LIND */
                     	mask =bitmask[mfn&BY8RMASK];
                        offset = (mfn/8);
                        if( !( PDLlvel[offset] & mask ) ){
                        	PDLdocs++;
                           PDLlvel[offset] |= mask;
                        }
#if!LIND
                     }else{
                     	if( mfn != hsiz ){
                        	hsiz = mfn;
                           PDLdocs++;
//                           b7_addk( B7_THISP pdlp, trmp );
                        }
                        b7_addk( B7_THISP pdlp, trmp );
                     }
#endif /* !LIND */
                  }

                  if( b7rootf ){
#if CICPP
                     trmp->xnxterm();
#else /* CICPP */
                     NXTERM(b7itrm);
#endif /* CICPP */
                  }else
                     break;

                  if( b7rootloop ){ /* hacer or entre los resultados. */
#if !LIND
							if( PDLplen ){
                     	if( npdls > 1 ){
                           pdlp = b7_push(B7_THIS); /*donde dejar el resultado. */
                           b7_initpdl( B7_THISP pdlp, postlen, lastmfn );
                        	b7_zorx(B7_THIS);
                        }
                     }
#else
							;
#endif /* !LIND */
                  }else
                     b7rootloop++;
               }

            }

/*GOT_ZLDV:*/

			if( npdls == npdlsb ){
               pdlp = b7_push(B7_THIS);
               b7_initpdl( B7_THISP pdlp, postlen, lastmfn );
            }

         	if (b7tell) {
               sprintf(buffup,"%s%c",mkp,(b7rootf)? '$' : '\0');
            	b7_msg(B7_THISP 0,buffup,0,PDLdocs,1);
            }
            nqualts=0;
         	break;

            default:
#if !LIND
            	if( !VPDLplen( vpdltop ) ){	/* top tiene nivel 1 */
#endif /* !LIND */
                  pdlp = vpdlp[ vpdltop ];
                  res = VPDLlvel( vpdltop - 1 );
                  op1 = PDLlvel;
#if !LIND
               }else{
               	pdlp=b7_push(B7_THIS);
                  b7_initpdl( B7_THISP pdlp, b7acc, lastmfn );
               }
#endif /* !LIND */
               loop=PDLsize;
               switch( b7opr ){
               	case ZORX:
				/* oper + */
#if !LIND
                  	if( b7acc == '1' ){
#endif /* !LIND */
               			for( ;loop--; )
               				*res++ |= *op1++;
                        b7_pop(B7_THIS);
#if !LIND
                     }else
                     	b7_zorx(B7_THIS);
#endif /* !LIND */
                     break;
               	case ZAND:
				/* oper * */
#if !LIND
                  	if( b7acc == '1' ){
#endif /* !LIND */
               			for( ;loop--; )
               				*res++ &= *op1++;
                        b7_pop(B7_THIS);
#if !LIND
                     }else
                     	b7_zand( B7_THISP 1 );
#endif /* !LIND */
                     break;
               	case ZANN:
				/* oper ^ */
#if !LIND
                  	if( b7acc == '1' ){
#endif /* !LIND */
               			for( ;loop--; )
               				*res++ &= ~(*op1++);
                        b7_pop(B7_THIS);
#if !LIND
                     }else
                     	b7_zann(B7_THIS);
#endif /* !LIND */
                     break;
#if !LIND
               	case ZANG:
				/* oper (G) */
                  	b7_zand( B7_THISP 2 );
                     break;
               	case ZANF:
				/* oper (F) */
                  	b7_zand( B7_THISP 3 );
                     break;
                case ZANP:
				/* oper . */
                  	b7_zanp( B7_THISP 4, b7qty );
                     break;
                case ZANE:
				/* oper $ */
                  	b7_zanp( B7_THISP 5, b7qty );
                     break;
#endif /* !LIND */
               }
               pdlp = vpdlp[vpdltop];
               if( !PDLplen )
               	PDLdocs = b7_hsizh( B7_THISP pdlp, &hsiz );

               if (b7tell) {
               	sprintf(buffup,"%c",b7opr);
               	b7_msg(B7_THISP 0,buffup,0,PDLdocs,2);
               }
      }
   }

   PDLdocs = b7_hsizh( B7_THISP pdlp, &hsiz );

	sprintf(buffup,B7EDIT1,b7setno);
   if (b7tell)
   	b7_msg(B7_THISP 1,buffup,0,PDLdocs,3);                            /* end_msg */

#if RUITRACE
   b7_showpdl( B7_THIS );
#endif

	pdlp = vpdlp[vpdltop];

    /* store actual MFQTASET */
    sprintf(buffup,B7EDIT1,b7setno);
#if CICPP
    memmove(FIELDP(irecp->xfieldx(MFQTASET,1)),buffup,B7UPLEN);
#else /* CICPP */
    memmove(FIELDP(fieldx(irec,MFQTASET,1)),buffup,B7UPLEN);
#endif /* CICPP */

    /* store actual MFQTALCC */
    sprintf(buffup,B7EDIT1,lastcc);
#if CICPP
    memmove(FIELDP(irecp->xfieldx(MFQTALCC,1)),buffup,B7UPLEN);
#else /* CICPP */
    memmove(FIELDP(fieldx(irec,MFQTALCC,1)),buffup,B7UPLEN);
#endif /* CICPP */

    /* store actual MFQTHITS */
    sprintf(buffup,B7EDIT1,PDLdocs);
#if CICPP
    memmove(FIELDP(irecp->xfieldx(MFQTHITS,1)),buffup,B7UPLEN);
#else /* CICPP */
    memmove(FIELDP(fieldx(irec,MFQTHITS,1)),buffup,B7UPLEN);
#endif /* CICPP */

/*..*/
	/* actual #bytes of hit list */
   hsiz = hsiz/8+1;

    /* store actual MFQTHSIZ */
    sprintf(buffup,B7EDIT1,hsiz);
#if CICPP
    memmove(FIELDP(irecp->xfieldx(MFQTHSIZ,1)),buffup,B7UPLEN);
#else /* CICPP */
    memmove(FIELDP(fieldx(irec,MFQTHSIZ,1)),buffup,B7UPLEN);
#endif /* CICPP */

/*..*/
    /* update stored MFQTHMEM */
    if (b7fd) {

    	for (p0p=PDLlvel, left=hsiz/*, bytes=0*/; left; left-=bytes ) {
			bytes=hsiz;
			if (bytes > B72SIZE_MAX)
         	bytes=B72SIZE_MAX;	/* < U */
			if (bytes > left)
         	bytes=left;
			if ((unsigned)CIWRITE(b7fd,p0p,(unsigned)bytes) != bytes){
				fatal("b73_run/write/6");
			}

         p0p+=bytes;
      }
      CLOSE(b7fd);
      b7fd=0; /* 06/03/95 */
	}


    /* update MFQTXCTS^MFQSWQTY */
    sprintf(buffup,B7EDIT1,PDLdocs);
#if CICPP
    if ((xdir_xcts=irecp->xfieldx(MFQTXCTS,irecp->xnocc(MFQTXCTS))) < 0)
#else /* CICPP */
    if ((xdir_xcts=fieldx(irec,MFQTXCTS,nocc(irec,MFQTXCTS))) < 0)
#endif /* CICPP */
        fatal("b7_run/xdir_xcts");
    n=DIRlen(xdir_xcts); p=(unsigned char *)FIELDP(xdir_xcts);
    nffi=n;
    p=subfldp(p,MFQSWQTY,&nffi);
    memmove(p,buffup,B7UPLEN);

    /* put it on disk */
    if (RECdbxp) {
#if CICPP
        recp->xrecupdat();
#else /* CICPP */
        recupdat(crec,irec);
#endif /* CICPP */
    }
    RECtype=TYPEMFQ;


#if CIB7HITLIST
    /* alloc b7bufferp if needed */
    if (!b7error) {

        LONGX bytsneed;         //
        int hcase;             //
        LONGX mfnvar;
        LONGX xbytes=PDLdocs*sizeof(mfnvar);

        if (xbytes < hsiz) {
            bytsneed=xbytes;
            hcase=MFNSTRING; /* a binary 4-byte integer list ready to use */
        }
        else {
            bytsneed=hsiz;     //
            hcase=BITSTRING;   //
        }

        if (b7bufferp) if (b7bufferp->bufsiz < bytsneed) b7_hbfree(B7_THISP &b7bufferp);
        if (!b7bufferp) b7_hballoc(B7_THISP (bytsneed),&b7bufferp);
        b7bufferp->setno = b7setno;

        b7bufferp->hcase = hcase;
        b7bufferp->ndocs = PDLdocs;

        if (hcase == MFNSTRING) {

    unsigned char *hp=PDLlvel;
	LONGX hsizleft=hsiz;
    LONGX hitsleft=PDLdocs;

    LONGX hcur=0;
    LONGX hmfn=0;
    int bitloop,by8masked;

    B7HITLIST *hitlistp=(B7HITLIST *)b7bufferp->area;
    LONGX *mfnlistp=hitlistp->mfnlist;

          for (; hitsleft; ) {
            /* check bytes left */ if (!hsizleft) fatal("b7_run/aotmsa1/hsizleft");
            /* skip null bytes */
            while (!*hp) {
              hp++; hmfn+=8; hsizleft--;
            }
            /* scan max 8 bits */
            bitloop=8-(hmfn&BY8RMASK);
            for (; bitloop--; hmfn++) {
                if (*hp&bitmask[by8masked=hmfn&BY8RMASK]) {
                    hcur++; hitsleft--;
					/* store hit hcur/hmfn and count it */
                    *mfnlistp++ = hmfn;
                } /* end if (*hp & bitmask[by8masked]) */
            } /* end for (; bitloop--; hmfn++) */
			/* increment byte ptr every 8 bits */
            if (by8masked == BY8RMASK) {
                hp++; hsizleft--;
            }
          } /* end for (; hitsleft; ) */
      } /* end if (hcase == MFNSTRING) */
      else { /* else if (hcase == MFNSTRING) */
        memmove(b7bufferp->area,PDLlvel,hsiz);
        memset(b7bufferp->area+hsiz,0x00,b7bufferp->bufsiz-hsiz);
      } /* end if (hcase == MFNSTRING) */
    } /* end if (!b7error) */

//B7HITLISTLONG   1
//B7HITLISTSHORT  2

#else /* CIB7HITLIST */

    /* alloc b7bufferp if needed */
    if (!b7error) {
        if (b7bufferp) if (b7bufferp->bufsiz < (lastmfn/8+1)) b7_hbfree(B7_THISP &b7bufferp);
        if (!b7bufferp) b7_hballoc(B7_THISP (lastmfn/8+1),&b7bufferp);
        b7bufferp->setno = b7setno;

        memmove(b7bufferp->area,PDLlvel,hsiz);
        memset(b7bufferp->area+hsiz,0x00,b7bufferp->bufsiz-hsiz);
    }
#endif /* CIB7HITLIST */

    /* release tmp area */
	 b7_pop(B7_THIS);

#if CICPP
    delete trmp;
#else
    free(vtrmp[b7itrm]);    /* Chico 03/07/2007 */
    vtrmp[b7itrm] = NULL;
    ntrms--;    
#endif /* CICPP */

   *errnop=b7error;
 return 0;

}
                                  
#if CICPP
int CIB7::b7_hballoc(LONGX bufsiz, B7HITBUF **bufferpp)
#else /*CICPP*/
int b7_hballoc(cib7p,bufsiz,bufferpp)						/*
------------
				allocate b7_hit() processing buffer
									*/
b7_CIB7 *cib7p;
LONGX bufsiz;		/* buffer size */
B7HITBUF **bufferpp;	/* buffer addr */
#endif /*CICPP*/
{
    if (!*bufferpp) {
#if CICPP
        try { *bufferpp=(B7HITBUF *)new char [sizeof(B7HITBUF)+bufsiz]; }
        catch (BAD_ALLOC) { *bufferpp = (B7HITBUF *)ALLONULL; }
#else /* CICPP */
        *bufferpp=(B7HITBUF *)ALLOC((ALLOPARM)(sizeof(B7HITBUF)+bufsiz));
#endif /* CICPP */
	if (*bufferpp == (B7HITBUF *)ALLONULL) fatal("b7_hballoc/ALLOC");
	(*bufferpp)->bufsiz=bufsiz;
    }
    else
	if ((*bufferpp)->bufsiz != bufsiz) fatal("b7_hballoc/bufsiz");
/*    (*bufferpp)->block=(-1L);*/
    (*bufferpp)->setno=(-1L);
    return(0);
}


#if CICPP
int CIB7::b7_hbinit(LONGX setno, B7HITBUF *bufferp)
#else /*CICPP*/
int b7_hbinit(cib7p,setno,bufferp)						/*
------------
				initialize b7_hit() processing buffer
									*/
b7_CIB7 *cib7p;
LONGX setno;		/* query number */
B7HITBUF *bufferp;	/* buffer */
#endif /*CICPP*/
{
    if (!bufferp) fatal("b7_hbinit/NULL");
/*    bufferp->block=(-1L);*/
    bufferp->setno=setno;
    return(0);
}

#if CICPP
int CIB7::b7_hbfree(B7HITBUF **bufferpp)
#else /*CICPP*/
int b7_hbfree(cib7p,bufferpp)							/*
------------
				release b7_hit() processing buffer
									*/
b7_CIB7 *cib7p;
B7HITBUF **bufferpp;	/* buffer addr */
#endif /*CICPP*/
{
    if (*bufferpp) {
#if CICPP
	delete [] (char *)*bufferpp;
#else /* CICPP */
        FREE(*bufferpp);
#endif /* CICPP */
	*bufferpp=NULL;
    }
    return(0);
}

#if CICPP
LONGX CIB7::b7_hit(RECSTRU *irecp, LONGX nord, LONGX setno)
#else /* CICPP */
LONGX b7_hit(cib7p,irec,nord,setno)                                   /*
-----------
                    abenda se vrecp nao inicializado;
                    abenda se registro irec nao alocado;
                    abenda se registro irec invalido;
                    retorna total de hits se nord = 0;
                    se nord > total de hits
                        zera hit e mfn correntes e retorna -1;
                    mantem controles para chamadas subsequentes;
                    retorna o mfn correspondente ao hit nord
                    (retorna -2 .. -n se algum erro)
                                                                      */
b7_CIB7 *cib7p;
LONGX irec;		/* indice de vrecp, para info/store */
LONGX nord;		/* numero de ordem do hit a analisar */
LONGX setno;		/* query number */
#endif /* CICPP */
{
    char buffup[B7UPLEN+1];       /* area for fldupdat */
    RECSTRU *recp;
    LONGX hits,hcur,hsiz,hmfn;
    int xdir_hits,xdir_hcur,xdir_hsiz,xdir_hmfn,xdir_hmem;
    unsigned char *p;
    LONGX offset,mfn;
    LONGX loop;
    int loop2,n,delta8,delta1;
#if CHKFOUND
    int found;
#endif
/* Leer de disco */
    int fd;
    char filname[CIMPL+1];
    LONGX rslen;
#if HITTRACE
 printf("b7_hit - irec,nord: %"_LD_"%"_LD_" \n",irec,nord);
#endif
#if SHOWCORE
 if (b70trace) showcore("b7_hit");
#endif

    /* Process hit MFNSTRING/LONGX
    */
#if CIB7HITLIST                     /* AOT, 28/01/2002 */
    if (!b7bufferp) fatal("b7_hit/MFNSTRING/b7bufferp");
    if (b7bufferp->setno != setno) fatal("b7_hit/MFNSTRING/setno");
    if (b7bufferp->hcase == MFNSTRING) {
        hits=b7bufferp->ndocs;
        if (nord == 0 || nord > hits) {
            if (nord == 0)
                mfn=hits;           /* returning value */
            else
                mfn=(-1L);
        }
        else { /* 1 >= nord <= hits */
            B7HITLIST *hitlistp;            /* hit processing */
            LONGX *mfnlistp;                 /* hit processing */
            hitlistp=(B7HITLIST *)b7bufferp->area;
            mfnlistp=hitlistp->mfnlist;
            mfnlistp+=(nord-1);
            mfn = *mfnlistp;
        } /* end if (nord == 0 || nord > hits) */
        
        return(mfn);
            
    } /* if (b7bufferp->hcase == MFNSTRING) */
#endif /* CIB7HITLIST */
    
    
    /* BTW, b7errxy nao deveria ser resetado antes de return??? - AOT, 28/01/2002 */
    strcpy(b7errxy,"b7_hit");          /* b7_hiterr() */

#if !CICPP
    if (!nrecs)
#if CDROM
        return(-2L);
#else
        fatal("b7_hit/nrecs");
#endif
    recp=vrecp[irec];

#else /* CICPP */
    recp=irecp;
#endif /* CICPP */

    if (!recp)
#if CDROM
        return(-3L);
#else
        fatal("b7_hit/irec");
#endif
    if (RECtype != TYPEMFQ || RECrc != RCNORMAL)
#if CDROM
        return(-4L);
#else
        fatal("b7_hit/check");
#endif
#if CICPP
    if ((xdir_hits=recp->xfieldx(MFQTHITS,1)) < 0)
#else /* CICPP */
    if ((xdir_hits=fieldx(irec,MFQTHITS,1)) < 0)
#endif /* CICPP */

#if CDROM
        return(-5L);
#else
        fatal("b7_hit/xdir_hits");
#endif

		for (hits=0, p=(unsigned char *)FIELDP(xdir_hits), loop=DIRlen(xdir_hits); loop--; p++)
        hits = hits * 10 + (*p - '0');
    if (nord < 0 || nord > (hits+1))
#if CDROM
        return(-6L);
#else
        fatal("b7_hit/nord");
#endif
#if CICPP
    if ((xdir_hcur=recp->xfieldx(MFQTHCUR,1)) < 0)
#else /* CICPP */
    if ((xdir_hcur=fieldx(irec,MFQTHCUR,1)) < 0)
#endif /* CICPP */
#if CDROM
        return(-7L);
#else
        fatal("b7_hit/xdir_hcur");
#endif
#if CICPP
    if ((xdir_hmfn=recp->xfieldx(MFQTHMFN,1)) < 0)
#else /* CICPP */
    if ((xdir_hmfn=fieldx(irec,MFQTHMFN,1)) < 0)
#endif /* CICPP */
#if CDROM
        return(-8L);
#else
        fatal("b7_hit/xdir_hmfn");
#endif
#if CICPP
    if ((xdir_hsiz=recp->xfieldx(MFQTHSIZ,1)) < 0)
#else /* CICPP */
    if ((xdir_hsiz=fieldx(irec,MFQTHSIZ,1)) < 0)
#endif /* CICPP */
#if CDROM
        return(-9L);
#else
        fatal("b7_hit/xdir_hsiz");
#endif
    if (nord == 0 || nord > hits) {
        hcur=hmfn=0;
        if (nord == 0)
            mfn=hits;           /* returning value */
        else
            mfn=(-1L);
    }
    else {
        for (hcur=0, p=(unsigned char *)FIELDP(xdir_hcur), loop=DIRlen(xdir_hcur); loop--; p++)
            hcur = hcur * 10 + (*p - '0');
        for (hmfn=0, p=(unsigned char *)FIELDP(xdir_hmfn), loop=DIRlen(xdir_hmfn); loop--; p++)
            hmfn = hmfn * 10 + (*p - '0');
        for (hsiz=0, p=(unsigned char *)FIELDP(xdir_hsiz), loop=DIRlen(xdir_hsiz); loop--; p++)
            hsiz = hsiz * 10 + (*p - '0');
#if HITTRACx
 printf("b7_hit - rec - hits=%"_LD_" hcur=%"_LD_" hsiz=%"_LD_" hmfn=%"_LD_"\n",
  hits,hcur,hsiz,hmfn);
#endif
        if (hcur == nord)
            mfn=hmfn;
        else {
#if CICPP
            if ((xdir_hmem=recp->xfieldx(MFQTHMEM,1)) < 0)
#else /* CICPP */
            if ((xdir_hmem=fieldx(irec,MFQTHMEM,1)) < 0)
#endif /* CICPP */
#if CDROM
                return(-10L);
#else
                fatal("b7_hit/xdir_hmen");
#endif
            if (nord > hcur) {
#if UNIX
                delta8 =  8; delta1 =  1;
#else
                delta8 = +8; delta1 = +1;
#endif
            }
            else {
                delta8 = -8; delta1 = -1;
            }
#if CDUSP
            mfn = (hcur == 0) ? 0 : hmfn+delta1;
#else
            mfn = (hmfn == 0) ? 0 : hmfn+delta1;
#endif
            offset=(mfn/8);
/* Leer de disco */
            if (!b7bufferp) fatal("b7_hit/buffer");
            if (b7bufferp->bufsiz < hsiz) fatal("b7_hit/bufsiz");
            if (b7bufferp->setno != setno) {
				    memmove(filname,FIELDP(xdir_hmem),n=DIRlen(xdir_hmem));
				    filname[n]='\0';
                dbxopt_fatal=0; fd=dbxopen(b7_gidbnp,filname,"");
				    if (fd <= 0) fatal("b7_hit/open/1");
				    rslen=(unsigned)CIREAD(fd,b7bufferp->area,hsiz);
				    if (rslen != hsiz) fatal("b7_hit/read/1");
                CLOSE(fd);
                b7bufferp->setno = setno;
            }

/*            for (p=FIELDP(xdir_hmem)+offset; hcur != nord; ) {*/
            for (p=b7bufferp->area+offset; hcur != nord; ) {
                loop=(nord > hcur) ? hsiz-(mfn/8) : (mfn/8);
#if CHKFOUND
                found=0;
#endif
#if HITTRACx
 printf("b7_hit - hcur=%"_LD_" nord=%"_LD_" mfn=%"_LD_" *p=%02x loop=%"_LD_" - hmfn=%"_LD_"\n",
  hcur,nord,mfn,*p,loop,hmfn);
#endif
                do {
                    if (!*p) {
                        p+=delta1; mfn+=delta8;
#if HITTRACx
 printf("b7_hit - hcur=%"_LD_" nord=%"_LD_" mfn=%"_LD_" *p=%02x loop=%"_LD_" - continue\n",
  hcur,nord,mfn,*p,loop);
#endif
                        continue;
                    }
                    if (nord > hcur)
                        loop2 = 8 - (mfn&BY8RMASK);
                    else
                        loop2 = (mfn&BY8RMASK) + 1;
                    for (; loop2--; mfn+=delta1) {
                        n=mfn&BY8RMASK;
#if HITTRACx
 printf("b7_hit - hcur=%"_LD_" nord=%"_LD_" mfn=%"_LD_" *p=%02x loop=%"_LD_" loop2=%d n=%d\n",
  hcur,nord,mfn,*p,loop,loop2,n);
#endif
                        if (*p & bitmask[n]) {
#if CHKFOUND
                            found=1;
#endif
                            hmfn=mfn; hcur+=delta1;
                            if (nord == hcur)
                                break;
                        }
                    }
#if CHKFOUND
                    if (found) {
#endif
                        if (nord > hcur) {
                            if (n == BY8RMASK)
                                p++;
                        }
                        else
                            if (n == 0)
                                p--;
                        break;
#if CHKFOUND
                    }
                    else
                        fatal("b7_hit/bug1");
#endif
                } while (--loop > 0);
#if CHKFOUND
                if (!found)
                     fatal("b7_hit/bug2");
#endif
            } /* end for hcur != nord */
        } /* end else hcur == nord */
    } /* end else nord == 0 || nord > hits */
    /* store */
    sprintf(buffup,B7EDIT1,hcur);
    memmove(FIELDP(xdir_hcur),buffup,B7UPLEN);
    sprintf(buffup,B7EDIT1,hmfn);
    memmove(FIELDP(xdir_hmfn),buffup,B7UPLEN);
#if HITTRACE
 printf("b7_hit - mfn: %"_LD_"\n",mfn);
#endif
#if SHOWCORE
 if (b70trace) showcore("b7_hit - exit");
#endif
    return(mfn);
}

#if !WWWISIS
#if CICPP
int CIB7::b7_msg(int tline,char *msg,LONGX psts,LONGX docs,int tmsg)
#else
int b7_msg(cib7p,tline,msg,psts,docs,tmsg)
b7_CIB7 *cib7p;
int tline;
char *msg;
LONGX psts;
LONGX docs;
int tmsg;
#endif
{
	char *q;

   q = msg+strlen( msg );
   if (nqualts) {
      *q++ = '/';  *q++ = '(';
      for (qualtp=vqualt; *qualtp; qualtp++) {
          if (*(qualtp+1))
              sprintf(q,"%d,",*qualtp);
          else
              sprintf(q,"%d",*qualtp);
          while (*q) q++;
      }
      *q++ = ')';  *q = '\0';
   }


    if (docs) {
        printf("%8"_LD_" ",docs); //AOT, 22/12/2006
    }
    else {    /*12345678 */
        printf("         ");  //AOT, 22/12/2006
    }
    if (msg)
        switch (tmsg) {
        case 1:
            printf(" %s",msg);
            break;
        case 2:
            printf(" Operation %s",msg);
            break;
        case 3:
            printf(" Set #%s",msg);
            break;
        case 4:
            printf("   %s",msg);
            break;
        default:
            fatal("b7_msg/tmsg");
        }
#if TOSEETLINE
    if (tline)
        printf(" [%d]\n",tline);
    else
#else
    if (tline == 0 || tline == 1)       /* no wrn */
#endif
        printf("\n");


    return(0);
}
#endif /* WWWISIS */

#if CICPP
char * CIB7 :: b7_xlkey(char *opx,char *bufp,unsigned char uctab[],char *xlfilp,int *opxlp)
#else
char *b7_xlkey(cib7p,opx,bufp,uctab,xlfilp,opxlp)
b7_CIB7 *cib7p;
char *opx;
char *bufp;
unsigned char uctab[];
char *xlfilp;
int *opxlp;
#endif
{
    unsigned char *p;
    int loop,newlen;
#if TRACEXK
printf("b7_xlkey - buf=%s (%d)\n",bufp,*opxlp);
#endif
    if (xlfilp) fatal("b7_xlkey/xlfilp");
    for (p=(unsigned char *)bufp, loop= *opxlp; loop--; p++)
        *p = uctab[*p];
    newlen= *opxlp;
    if (newlen > LE2) if (newlen != LE2+2 || (*bufp != '\"' && bufp[newlen-1] != '\"'))
        newlen=LE2;
    memmove(opx,bufp,newlen);
    opx[newlen]='\0';
    *opxlp = newlen;
    return(opx);
}

#if CICPP
LONGX CIB7::b7_hsizh(PDLSTRU *pdlp,LONGX *hsizp)
#else
LONGX b7_hsizh(cib7p,pdlp,hsizp)
b7_CIB7 *cib7p;
PDLSTRU *pdlp;
LONGX *hsizp;
#endif
{
    LONGX hits,hsiz,loop,mfn,offset;
    int loop2;
    unsigned char *p1p;
    hits=0; hsiz=0;
    p1p=PDLlvel;
    for (mfn=0, loop=PDLsize; loop--; ) {
        offset=(mfn/8);
        if (!*(PDLlvel+offset)) {
            mfn+=8;
            continue;
        }
        for (loop2=8; loop2--; mfn++){
        		if( mfn > PDLlmfn )
            	break;
            if (*(p1p+offset) & bitmask[mfn&BY8RMASK]) {
                hits++;
                hsiz=mfn;
#if MFNTRACE
 printf("%"_LD_" ",mfn);
#endif
            }
		  }
    }
#if MFNTRACE
 printf("\n");
#endif
    *hsizp = hsiz;
    return(hits);
}


/*
 * <R>
 * funciones utilizadas desde _run()
 * </R>
 */

#if !LIND

#if CICPP
int CIB7::b7_cmp( char *op1, char *op2, int oplen )
#else
int b7_cmp( cib7p,op1,op2,oplen )
b7_CIB7 *cib7p;
char *op1;		/* posting 1 */
char *op2;		/* posting 2 */
int oplen;		/* tamaño de los posting */
#endif
{
/*
	Return value
	>0	=> op1 > op2
	<0	=> op1 < op2 
	 0	=> op1 = op2 
 */
	int res;
	int i;

   oplen = (oplen>3)?oplen:3;

   if (!op1 || !op2) fatal("cib72/b7_cmp/op1Xop2");

   for( i = 0; i < oplen; i++ ){
   	res = ((int)(unsigned char)*(op1+i) - (unsigned char)*(op2+i));
      if( res != 0 )	return res;
   }
    return 0;
}

#if CICPP
int CIB7::b7_prox( char *op1, char* op2, short andl, LONGX dist )
#else
int b7_prox( cib7p,op1,op2,andl,dist )
b7_CIB7 *cib7p;
char *op1;
char *op2;
short andl;		/* 4: oper . ;  5:oper $ */
LONGX dist;		/* cant. de operadores. */
#endif
{

/*
	retorna 0 si no cumple la condicion de proximidad,
	dist de 0 en caso contrario.
 */

	int res;
   short int cnt1, cnt2;

	if( b7_cmp( B7_THISP op1, op2, 6 ) )
   	return 0;

   cnt1 = (unsigned)*(op1+6);
   cnt1 = (cnt1<<8) + ((unsigned)*(op1+7));

   cnt2 = (unsigned)*(op2+6);
   cnt2 = (cnt2<<8) + ((unsigned)*(op2+7));

   switch( andl ){
   	case 4:
      	res = ( abs( cnt1 - cnt2 ) <= dist );
         break;
   	case 5:
      	res = ( abs( cnt1 - cnt2 ) == dist );
         break;
      default:
      	res = 0;
   }

   return res;

}

#if CICPP
LONGX CIB7::b7_gmfn( char *str )
#else
LONGX b7_gmfn( cib7p,str )
b7_CIB7 *cib7p;
char *str;		/*posting */
#endif
{
/*
	Calcula el MFN del posting almacenado.
 */
	LONGX res;
	unsigned char mfn1, mfn2, mfn3;

	mfn1 = *str;
	mfn2 = *(str+1);
	mfn3 = *(str+2);

	res = (LONGX)(mfn1<<16) + (LONGX)(mfn2<<8) + (LONGX) mfn3;

   return res;
}

/*
	Funciones sobre la pila de ejecución, trabajan sobre las listas
	del tope y subtope de la pila
*/


/* logical or. */
#if CICPP
void CIB7 :: b7_zorx()
#else
void b7_zorx(cib7p)
b7_CIB7 *cib7p;
#endif
{
/*
	recorrer las 2 listas de posting y crear una nueva
	almacenando ordenadamente los postings distintos entre si
	comparandolos en el nivel del resultado.
 */
	char *op1, *op2, *cur, *lp;
   int reslen, oplen;
   int iop1, iop2;
   int cmpf;
   PDLSTRU *pdlp;
   LONGX offset;

   LONGX mfnx, lmfn;

   iop1 = vpdltop - 2;
   iop2 = vpdltop - 1;

   pdlp = vpdlp[ vpdltop ];

   op1 = (char *)VPDLlvel( iop1 );
   op2 = (char *)VPDLlvel( iop2 );

   reslen = PDLplen;
   oplen = VPDLplen( iop1 );

   lmfn = 0;

   lp=NULL;
   while( op1 != VPDLnxtp( iop1 ) || op2 != VPDLnxtp( iop2 ) ){

   	if( op1 == VPDLnxtp( iop1 ) ){
      	cur = op2;
         op2 += oplen;
      }else if( op2 == VPDLnxtp( iop2 ) ){
      	cur = op1;
         op1 += oplen;
      }else{
         cmpf = b7_cmp( B7_THISP op1, op2, oplen );

         if( cmpf <= 0 ){
            cur = op1;
            op1 += oplen;
            if( !cmpf ){
               op2 += oplen;
            }
         }else{
            cur = op2;
            op2 += oplen;
         }
      }

	  /*cur esta en el menor de los posting restantes. */
      mfnx = b7_gmfn( B7_THISP cur );

      if( ( !PDLdocs || ( b7_cmp( B7_THISP cur, lp, reslen ) ) != 0  ) ){
		  /*no hemos almacenado ninguno o cur apunta a un posting */
		  /*distinto del último almacenado. */

         if( PDLplen ){
            b7_copy( B7_THISP pdlp, cur );
         }else{
            offset = (mfnx/8);
            mask=bitmask[mfnx&BY8RMASK];
            PDLlvel[offset] |= mask;
         }
         lp = cur;
      }

      if( !lmfn || lmfn != mfnx ){
		  /*es el primer posting o */
		  /*tiene mfn distinta del último almacenado. */

      	 PDLdocs++;
         lmfn = mfnx;
      }
   }

   /*cambiar el tope de la pila con el 3er nivel. */
   b7_xpdl( B7_THISP npdls-1, npdls-3);
   b7_pop(B7_THIS);
   b7_pop(B7_THIS);

}

/*logical and, oper (G) and oper (F) */
#if CICPP
void CIB7::b7_zand( short andl )
#else
void b7_zand( cib7p, andl )
b7_CIB7 *cib7p;
short andl;	/* 1: *; 2: (G); 3: (F) */
#endif
{
/*
	recorrer las 2 listas hasta qeu una de ellas termine,
	creanndo una nueva con los posting qeu se encuentran en ambas.
 */
	char *op1, *op2, *cur, *lp;
   int reslen, oplen, andlen;
   int iop1, iop2;
   int cmpf;
   PDLSTRU *pdlp;
   LONGX offset;

   LONGX mfnx, lmfn;

   iop1 = vpdltop - 2;
   iop2 = vpdltop - 1;

   pdlp = vpdlp[ vpdltop ];

   op1 = (char *)VPDLlvel( iop1 );
   op2 = (char *)VPDLlvel( iop2 );

   reslen = PDLplen;
   oplen = VPDLplen( iop1 );

   switch( andl ){
   	case 2: andlen = 5; break;
   	case 3: andlen = 6; break;
      default: andlen = 3;
   }

   lmfn = 0;
   lp = 0;


   while( op1 != VPDLnxtp( iop1 ) && op2 != VPDLnxtp( iop2 ) ){

   	cur = 0;
   	cmpf = b7_cmp( B7_THISP op1, op2, andlen );
      if( cmpf < 0 ){
      	if( lp && !b7_cmp( B7_THISP op1, lp, andlen ) ){
         	cur = op1;
         }
      	op1 += oplen;
      }else if( cmpf > 0 ){
      	if( lp && !b7_cmp( B7_THISP op2, lp, andlen ) ){
         	cur = op2;
         }
      	op2 += oplen;
      }else{
         cmpf = b7_cmp( B7_THISP op1, op2, oplen );
         if( cmpf <= 0 ){
         	cur = op1;
            op1 += oplen;
            if( !cmpf )
            	op2 += oplen;
         }else{
         	cur = op2;
            op2 += oplen;
         }
      }

      if( cur ){
         mfnx = b7_gmfn( B7_THISP cur );

         if( !PDLdocs || ( b7_cmp( B7_THISP cur, lp, reslen ) ) != 0 ){

            if( PDLplen ){
               b7_copy( B7_THISP pdlp, cur );
            }else{
               offset = (mfnx/8);
               mask=bitmask[mfnx&BY8RMASK];
               PDLlvel[offset] |= mask;
            }
            lp = cur;
         }

         if( !lmfn || lmfn != mfnx ){
            PDLdocs++;
            lmfn = mfnx;
         }
      }
   }

   b7_xpdl( B7_THISP npdls-1, npdls-3);
   b7_pop(B7_THIS);
   b7_pop(B7_THIS);


}

/*logical not. */
#if CICPP
void CIB7::b7_zann()
#else
void b7_zann( cib7p )
b7_CIB7 *cib7p;
#endif
{

/*
	Por cada elemento de la lista 1, incluirlo en la lista de resultados
	sino se encuentra en la lista 2.
 */

	char *op1, *op2, *cur, *lp, *tmp;
   int reslen, oplen;
   int iop1, iop2;
   int cmpf;
   PDLSTRU *pdlp;
   LONGX offset;

   LONGX mfnx, lmfn;

   iop1 = vpdltop - 2;
   iop2 = vpdltop - 1;

   pdlp = vpdlp[ vpdltop ];

   op1 = (char *)VPDLlvel( iop1 );
   op2 = (char *)VPDLlvel( iop2 );

   reslen = PDLplen;
   oplen = VPDLplen( iop1 );


   lp=NULL; lmfn=0;
   while( op1 != VPDLnxtp( iop1 ) ){
   	cur = 0;
   	cmpf = b7_cmp( B7_THISP op1, op2, 3 );
      if( cmpf < 0 ){
      	cur = op1;
         op1 += oplen;
      }else if( cmpf > 0 ){
      	op2 += oplen;
      }else{
      	tmp = op1;
         while( !b7_cmp( B7_THISP tmp, op1, 3 ) )
         	op1 += oplen;

         tmp = op2;
         while( !b7_cmp( B7_THISP tmp, op2, 3 ) )
         	op2 += oplen;
      }

      if( cur ){
         mfnx = b7_gmfn( B7_THISP cur );

         if( !PDLdocs || ( b7_cmp( B7_THISP cur, lp, reslen ) ) != 0 ){

            if( PDLplen ){
               b7_copy( B7_THISP pdlp, cur );
            }else{
               offset = (mfnx/8);
               mask=bitmask[mfnx&BY8RMASK];
               PDLlvel[offset] |= mask;
            }
            lp = cur;
         }

         if( !lmfn || lmfn != mfnx ){
            PDLdocs++;
            lmfn = mfnx;
         }
      }

   }


   b7_xpdl( B7_THISP npdls-1, npdls-3);
   b7_pop(B7_THIS);
   b7_pop(B7_THIS);


}

/*proximidad. */
#if CICPP
void CIB7::b7_zanp( short andl, unsigned char b7qty )
#else
void b7_zanp( cib7p,andl,b7qty )
b7_CIB7 *cib7p;
short andl;	/*4: .; 5: $ */	
unsigned char b7qty;
#endif
{
/*
	Por cada elemento de la lista 1
		si existe elemento en la lista 2 que cumpla condicion de proximidad
			adicionarlos a ambos en el resultado ( si son distintos en el nivel del resultado )
 */
	char *op1, *op2, *lp;
   int oplen;
   int iop1, iop2;
   int cmpf;
   PDLSTRU *pdlp;
   LONGX offset;

   LONGX mfnx, lmfn;
   short dist;

//HB   dist = atoi((const char *) &b7qty );
   dist = (short)b7qty;

   iop1 = vpdltop - 2;
   iop2 = vpdltop - 1;

   pdlp = vpdlp[ vpdltop ];

   op1 = (char *)VPDLlvel( iop1 );
   op2 = (char *)VPDLlvel( iop2 );

   oplen = VPDLplen( iop1 );

   lmfn = 0;
   lp = 0;

   while( op1 != VPDLnxtp( iop1 ) ){
   	cmpf = b7_cmp( B7_THISP op1, op2, 6 );
      if( cmpf < 0 ){
      	op1 += oplen;
      	op2 = (char *)VPDLlvel( iop2 );
      }else if( cmpf > 0 ){
      	op2 += oplen;
      }else{
         if( b7_prox( B7_THISP op1, op2, andl, dist) ){
         	mfnx = b7_gmfn( B7_THISP op1 );
         	if( !PDLplen ){
               offset = (mfnx/8);
               mask=bitmask[mfnx&BY8RMASK];
               PDLlvel[offset] |= mask;
            }else if( PDLplen < 7 ){
               if( !lp || b7_cmp( B7_THISP op1, lp, PDLplen ) ){
                  b7_copy( B7_THISP pdlp, op1 );
                  lp = op1;
               }
            }else{
          		cmpf = b7_cmp( B7_THISP op1, op2, PDLplen );
               if( cmpf <= 0 ){
                  b7_inst( B7_THISP pdlp, op1 );
                  b7_inst( B7_THISP pdlp, op2 );
                  if( !lp || b7_cmp( B7_THISP op2, lp, PDLplen ) > 0 )
                  	lp = op2;
               }else{
                  b7_inst( B7_THISP pdlp, op2 );
                  b7_inst( B7_THISP pdlp, op1 );
                  if( !lp || b7_cmp( B7_THISP op1, lp, PDLplen ) > 0 )
                  	lp = op1;
               }
            }
            if( !lmfn || lmfn != mfnx ){
               PDLdocs++;
               lmfn = mfnx;
            }
         }
			op2 += oplen;
      }
   }
   b7_xpdl( B7_THISP npdls-1, npdls-3);
   b7_pop(B7_THIS);
   b7_pop(B7_THIS);

}


#if CICPP
void CIB7::b7_copy( PDLSTRU *pdlp, char *src )
#else
void b7_copy(cib7p,pdlp,src )
b7_CIB7 *cib7p;
PDLSTRU *pdlp;
char *src;
#endif
{
/*
	adicionar un posting en el pdllvel del pdlp, siempre al final.
 */

	LONGX offset;
   if( !PDLleft ){
#if BEFORE20020319	   
   	  offset = (UCHR *)PDLnxtp - PDLlvel;
      PDLlvel = (unsigned char *)realloc( PDLlvel, PDLsize+PDLisiz );
      if( !PDLlvel )
         fatal( "cib72/b7_copy/realloc" );

      PDLsize += PDLisiz;
      PDLnxtp = (char *)(PDLlvel+offset);
      memset( PDLnxtp, 0x00, PDLisiz );
#else
	UCHR *newlvel;

/*
	offset = (UCHR *)PDLnxtp - PDLlvel;
*/
        offset = PDLsize;

#if CICPP
	newlvel = (UCHR*)new char[(ALLOPARM)(PDLsize+PDLisiz)];
#else /* CICPP */
	newlvel = (UCHR*)ALLOC((ALLOPARM)(PDLsize+PDLisiz));
#endif /* CICPP */
	if( !newlvel )
		fatal( "cib72/b7_addk/realloc" );
	memset( newlvel, 0x00, PDLsize+PDLisiz );
	memmove( newlvel, PDLlvel, PDLsize );
#if CICPP
    delete[] PDLlvel;
#else /* CICPP */
	FREE( PDLlvel );
#endif /* CICPP */
	PDLlvel = newlvel;
	PDLsize += PDLisiz;
	PDLnxtp = (char *)(PDLlvel+offset);
        PDLleft = PDLisiz;
#endif
   }

   memmove( PDLnxtp, src, PDLplen );

   PDLleft-= PDLplen;
   PDLnxtp += PDLplen;
}

#if CICPP
void CIB7::b7_inst( PDLSTRU *pdlp, char *src )
#else
void b7_inst( cib7p,pdlp,src )
b7_CIB7 *cib7p;
PDLSTRU *pdlp;
char *src;
#endif
{
/*
	Adicionar un posting en el pdllvel del pdlp, pero ordenado.
 */

	LONGX offset;
   char *cur;

   if( PDLlvel == (unsigned char *)PDLnxtp ){
   	b7_copy( B7_THISP pdlp, src );
      return;
   }

   if( !PDLleft ){
#if BEFORE20020319
   	  offset = (UCHR *)PDLnxtp - PDLlvel;
      PDLlvel = (unsigned char *)realloc( PDLlvel, PDLsize+PDLisiz );
      if( !PDLlvel )
         fatal( "cib72/b7_copy/realloc" );

      PDLsize += PDLisiz;
      PDLnxtp = (char *)(PDLlvel+offset);
      memset( PDLnxtp, 0x00, PDLisiz );
#else
	UCHR *newlvel;

/*
	offset = (UCHR *)PDLnxtp - PDLlvel;
*/
        offset = PDLsize;

#if CICPP
	newlvel = (UCHR*)new char[(ALLOPARM)(PDLsize+PDLisiz)];
#else /* CICPP */
	newlvel = (UCHR*)ALLOC((ALLOPARM)(PDLsize+PDLisiz));
#endif /* CICPP */
	if( !newlvel )
		fatal( "cib72/b7_addk/realloc" );
	memset( newlvel, 0x00, PDLsize+PDLisiz );
	memmove( newlvel, PDLlvel, PDLsize );
#if CICPP
    delete[] PDLlvel;
#else /* CICPP */
	FREE( PDLlvel );
#endif /* CICPP */
	PDLlvel = newlvel;
	PDLsize += PDLisiz;
	PDLnxtp = (char *)(PDLlvel+offset);
        PDLleft = PDLisiz;
#endif
   }

   cur = PDLnxtp;

   while( cur-PDLplen != (char *)PDLlvel && b7_cmp( B7_THISP cur-PDLplen, src, PDLplen ) > 0 ){
      cur -= PDLplen;
   }

   if( !b7_cmp( B7_THISP cur-PDLplen, src, PDLplen ) )
      return;

   if( cur != PDLnxtp )
   	memmove( cur, cur+PDLplen, PDLnxtp-cur );

   memmove( cur, src, PDLplen );

   PDLleft-=PDLplen;
   PDLnxtp += PDLplen;
}

#if CICPP
void CIB7::b7_addk( PDLSTRU *pdlp, TRMSTRU *trmp )
#else
void b7_addk( cib7p,pdlp,trmp )
b7_CIB7 *cib7p;
PDLSTRU *pdlp;
TRMSTRU *trmp;
#endif
{
/*
	Adicionar un posting al final del pdllvel del pdlp,
	partiendo de la información del trmp.
 */

	LONGX offset;
   char *p;
   LONGX pmfn;
   UWORD ptag, pocc, pcnt;

   if( !PDLplen ) return;

   if( !PDLleft ){
#if BEFORE20020319
   	  offset = (UCHR *)PDLnxtp - PDLlvel;
      PDLlvel = (unsigned char *)realloc( PDLlvel, PDLsize+PDLisiz );
      if( !PDLlvel )
         fatal( "cib72/b7_copy/realloc" );

      PDLsize += PDLisiz;
      PDLnxtp = (char *)(PDLlvel+offset);
      memset( PDLnxtp, 0x00, PDLisiz );
#else
	UCHR *newlvel;

/*
	offset = (UCHR *)PDLnxtp - PDLlvel;
*/
        offset = PDLsize;

#if CICPP
	newlvel = (UCHR*)new char[(ALLOPARM)(PDLsize+PDLisiz)];
#else /* CICPP */
	newlvel = (UCHR*)ALLOC((ALLOPARM)(PDLsize+PDLisiz));
#endif /* CICPP */
	if( !newlvel )
		fatal( "cib72/b7_addk/realloc" );
	memset( newlvel, 0x00, PDLsize+PDLisiz );
	memmove( newlvel, PDLlvel, PDLsize );
#if CICPP
    delete[] PDLlvel;
#else /* CICPP */
	FREE( PDLlvel );
#endif /* CICPP */
	PDLlvel = newlvel;
	PDLsize += PDLisiz;
	PDLnxtp = (char *)(PDLlvel+offset);
        PDLleft = PDLisiz;
#endif
   }

   p = PDLnxtp;

   pmfn = TRMpmfn;

   *p= (pmfn>>16)&0xFF;
   *(p+1)=(pmfn>>8)&0xFF;
   *(p+2)= (pmfn)&0xFF;


   ptag = TRMptag;

   p += 3;
   *p = (ptag>>8)&0xFF;
   *(p+1)=ptag & 0xFF;


   if( PDLplen > 5 ){
	   pocc = TRMpocc;
	   p+=2;
      *p = pocc;
   }

   if( PDLplen > 6 ){
	   pcnt = TRMpcnt;
      p+=1;
      *p = (pcnt>>8)&0xFF;
      *(p+1) = pcnt & 0xFF;
   }

   PDLleft-=PDLplen;
   PDLnxtp += PDLplen;

}

#endif /* !LIND */

/*utilidades para mostrar el contenido de la pila de ejecución */
#if CICPP
void CIB7::b7_print( char *str, short int len )
#else
void b7_print( cib7p,str, len )
b7_CIB7 *cib7p;
char *str;
short int len;
#endif
{
   LONGX val;

   val=(unsigned char)*str;
   val=(val<<16)+(*(str+1)<<8)+(unsigned char)(*(str+2));

   printf( "mfn: %u", val );

   val=(unsigned char)*(str+3);
   val=(val<<8)+(unsigned char)(*(str+4));
   printf( ", tag: %u", val );

   if( len > 5 ){
   	val = (unsigned char)*(str+5);
   	printf( ", occ: %u", val );
   }

   if( len > 6 ){
      val=(unsigned char)*(str+6);
      val=(val<<8)+(unsigned char)(*(str+7));
      printf( ", cnt: %u", val );
   }

}

#if CICPP
void CIB7::b7_showpdl()
#else
void b7_showpdl( cib7p )
b7_CIB7 *cib7p;
#endif
{
	LONGX i;
   PDLSTRU *pdlp;
   char *str;
   LONGX mfn, offset, hits;

   printf( "%d levels\n", npdls );

   for( i = 0; i < npdls; i++ ){
   	pdlp = vpdlp[i];
   	printf( "\tlevel:%i\n", i );
      printf( "lvel: %d, nxtp: %d\n", (int)PDLlvel, PDLnxtp );
      printf( "size: %d, plen: %d,docs: %d, lvel:\n", PDLsize, PDLplen, PDLdocs );
      if( PDLplen ){
         str = (char *)PDLlvel;
         while( str != PDLnxtp ){
            b7_print( B7_THISP str, PDLplen );
            str += PDLplen;
            printf( "\n" );
         }
      }else{
      	for( mfn=1, hits=0; mfn<=PDLlmfn && hits < PDLdocs; mfn++ ){
            offset = (mfn/8);
            mask=bitmask[mfn&BY8RMASK];
            if( PDLlvel[offset] & mask ){
               printf( "%d | ", mfn );
               hits++;
            }
         }
      }
      printf( "\n" );
   }
   fflush( stdout );

}

#if CICPP
CIB7::CIB7(CISISX *parm_cisisxp) {
   cisisxp = parm_cisisxp;

   b70trace=0;        /* ja' esta' inicializada em cidbx/cisisx */
   and2gf=0;          /* AOT&RP 28/05/98 */
   subst_and=NULL; /* RP 29/05/98 */
   b7_gidbnp=NULL;   /* dbn.par */
   b7fd=0;        /* open() - 06/03/95 */
   b7bufferp = NULL;    /* b7_run() processing buffer */
   b7_pfxmdl=0;
   b7error=0;
   b7rootmsg=OFF;
}
#endif
