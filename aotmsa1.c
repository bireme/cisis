/* =============================================================================
   AOTMSA1     Adalberto Otranto Tardelli / Meide da Silva Ancao
               Application 1
               by AOT, 05/01/2002

   aotmsa1.c   CI/tabula casos segundo variavel/variaveis de controle
   -----------------------------------------------------------------------------

   aotmsa1 <action>[,...] [<option> [...]]

      actions:   C  - get parameters from cgi
                 1  - display header MDL:
                TA  - search amdl
                      tabulate amdl (v351/)
                      store F tab and W tab
                DA  - read F tab and W tab
                      display formA results

      options:  bool=<expr>
                cip=<>                             c:/2001x300/usersaot/cip.cip
                decs=<>                            decs
                amdl=<>                            amdl351 | MEDLINE*.def
                atmdl=<>                           atmdl (000001..039999)
                name=<>                            c:\windows\temp\tmp1.$$$
                pft=<>                             aotmsa1A.pft
                atmdl99                            amdl351.maxmfn
                {from||loop|count|tell|setno}=<n>
   -----------------------------------------------------------------------------
============================================================================= */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "cisis.h"          /* CISIS Interface header file */
#include "cirun.h"          /* CISIS Interface runtime declarations */
#include "cib70.h"          /* CISIS Interface search engine */


#if 1  /* ANSITABS */
/* ---------------------------------------------------------------------- ansiuc.tab -- */
unsigned char ansiuctab[256]=
{
          0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
         16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
         32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
         48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
         64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
         80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
         96, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
         80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90,123,124,125,126,127,
        128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
        144,145,146,147,148,149,150,151,152,153,138,155,140,157,158,159,
        160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,
        176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
         65, 65, 65, 65, 65, 65, 65, 67, 69, 69, 69, 69, 73, 73, 73, 73,
         68, 78, 79, 79, 79, 79, 79,215,216, 85, 85, 85, 85, 89,222,223,
         65, 65, 65, 65, 65, 65, 65, 67, 69, 69, 69, 69, 73, 73, 73, 73,
        208, 78, 79, 79, 79, 79, 79,247,216, 85, 85, 85, 85,221,222,221
};
/* ---------------------------------------------------------------------- ansiac.tab -- */
unsigned char ansiactab[] = {
 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66,
 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 97, 98, 99,100,101,102,103,104,
 105,106,107,108,109,110,111,112,113,114,115,116,117,
118,119,120,121,122,128,129,130,131,132,133,134,135,136,137,138,139,140,141,
142,143,144,145,146,147,148,149,150,151,152,153,154,
160,161,162,163,164,165,168,192,193,194,195,196,199,200,201,202,203,204,205,
206,207,209,210,211,212,213,214,217,218,219,220,221,
224,225,226,227,228,231,232,233,234,235,236,237,238,239,241,242,243,244,245,
246,249,250,251,252,253,255
,000
};
/* ------------------------------------------------------------------------------------ */
#endif /* 0000 */


#define CLASSES   40000     /* default */

#define TAG351      351     /* DecsMfn tag - MH by serverw::\bases\mdl.006\aot\get*\ */
#define TAG020       20     /* DecsCateg tag - standard A00.000.000.000...000 - currently up to 39 chrs */

#define MAXCATLEN    47     /* 111.222.333.444.555.666.777.888.999.000.111.222 */

#define DECSTAGNAME(x)        (      \
  (toupper(*x) == 'I') ? 1 :         \
  (toupper(*x) == 'E') ? 2 :         \
  (toupper(*x) == 'P') ? 3 : 1 )

#define TAG022       22     /* DecsCDallCategs tag  */
#define TAG005        5     /* DecsCDCateg1MFN tag  */

#define ATMDLTAG999 999     /* atmdl/999999 tag 999 = amdl351.maxmfn */

typedef struct tabfreqf_entry {
    LONGX v351;
    LONGX freq;         /*           = docs de TT em xA */
} TABFREQF_ENTRY;

typedef struct tabfreqd_entry {
    LONGX v351;
//    LONGX delta;
    char delca[MAXCATLEN+1];
    LONGX gmfn;
} TABFREQD_ENTRY;

typedef struct tabfreqw_entry {
    LONGX v351;
    float peso;
    LONGX docsTTxA;       /*  v999[1] = docs de TT em xA */
    LONGX docsTTamdl;     /*  v999[2] = docs de TT em amdl = postings de TT em amdl */
//    LONGX docsTOTxA;    /* v9991[1] = docs de xA - nao vazios */
//    LONGX docsTOTamdl;  /* v9991[2] = docs de amdl */
    float frelTTxA;      /* v1991[1] = frelTTxA = docs de TT em xA / docs de xA */
    float frelTTamdl;    /* v1991[2] = frelTTamdl = docs de TT em amdl / docs de amdl */
//see peso    float pesoTTxA;      /* v2991[1] = frelTTxA / frelTTamdl */
//    char pesoTTxAout[4]; /* v2221[1] = 'out' se peso < 1 */
} TABFREQW_ENTRY;

/* prototypes */
#if ANSI
int rutfreqF(LONGX *tabcntA, int ntabs, LONGX drec, LONGX *tabcntArankF, TABFREQF_ENTRY **tabfreqFp);
int outfreqF(int ntabs, char *dbnoutp, LONGX *tabcntArankW, LONGX *tabcntArankD, TABFREQF_ENTRY *tabfreqFp);
int rutfreqW(TABFREQF_ENTRY *tabfreqF, int ntabs, char *amdlname, LONGX docsTOTxA, LONGX docsTOTamdl, LONGX *tabcntArankF, LONGX *tabcntArankW, TABFREQW_ENTRY **tabfreqWp, LONGX *tabcntMDL, LONGX *gtabcntA);
int outfreqW(int ntabs, char *dbnoutp, LONGX docsTOTxA, LONGX docsTOTamdl, LONGX *tabcntArankF, LONGX *tabcntArankD, TABFREQD_ENTRY *tabfreqD, TABFREQW_ENTRY *tabfreqW);
int rutfreqD(TABFREQF_ENTRY *tabfreqF, int ntabs, LONGX *tabcntArankF, LONGX *tabcntArankW, LONGX *tabcntArankD, TABFREQD_ENTRY **tabfreqDp, LONGX *tabcntGMDL, LONGX *gtabcntA);
int outfreqD(int ntabs, char *dbnoutp, LONGX *tabcntArankF, LONGX *tabcntArankW, TABFREQD_ENTRY *tabfreqD);

char *recsfldv(char *areap, char *fldp, FFI fldlen, char *defaultp);
#else
int rutfreqF();
int outfreqF();
int rutfreqW();
int outfreqW();
int rutfreqD();
int outfreqD();
char *recsfldv();
#endif

                                      /* <========================= */
LONGX ddrec;
char *ddecsdbnp;
                                      /* <========================= */

/* tabs here */
    int  ntabs=0;                  /* numero de entradas de tabcntA/tabcntArankx */
    LONGX tabcntA     [CLASSES+1];  /* indexed by DecsMfn */ /* avoid -1 */
    LONGX tabcntArankF[CLASSES+1];  /* indexed by DecsMfn */
    LONGX tabcntArankW[CLASSES+1];  /* indexed by DecsMfn */
    LONGX tabcntArankD[CLASSES+1];  /* indexed by DecsMfn */

    int  gntabs=0;                  /* numero de entradas de gtabcntA */
    LONGX gtabcntA    [CLASSES+1];   /* indexed by DecsMfn */

    LONGX tabcntMDL   [CLASSES+1];  /* indexed by DecsMfn */
    LONGX tabcntGMDL  [CLASSES+1];  /* indexed by DecsMfn */

#define MAXTABV05  42099
    LONGX vcategmfn[26][10][10];    /* indexed by [A][0][0] */  /* 10K */
    int ntabv05=0;                 /* avail */
    LONGX vtabv05[MAXTABV05];       /* indexed by vtabv05xrf[DecsMnf] */
    int  vtabv05xrf[CLASSES+1];    /* starting index 0..MAXTABV05-1 */
    int  vtabv05len[CLASSES+1];    /* number of entries */
/* tabs here */


void main(argc,argv_main)
int argc;
char *argv_main[];
{
#define MAXARGS 512
    char *argv[MAXARGS+1];
    int i,argnext=1;

    void *ciapip = NULL;

    RECSTRU *recp;
    LONGX irec=0L;
    unsigned char *p,*q;

    LONGX   vtime[100];   /* watch */
    char *vtimid[100];   /* watch */
    int      ntimes=0;   /* watch */

    int parmtime=0;
    int parmloadmdl=1;

    int parmgroup=0;
    LONGX mydrec=0L;

    char parmsortx='\0'; /* actual sort parameter */
    int parmshowgrp1=0;  /* show group01.w information */       /* 1 */
    int parmshowcat1=0;  /* show first cat1 - name */           /* c */
    int parmshowtree=0;  /* show hierarchy?? lines */           /* t */
    int parmshowzero=0;  /* don't show condition lines */       /* z */
    
       
#define ENVASIZE  8000
    char envarea[ENVASIZE];

    char *usernamp="aot";
    char *parmaid=NULL;

    /* ---------------------------------------------------------------------- */
    LONGX boolridx=0L;
    LONGX booldbcrec=0L; /* b5_exp usa booldbcrec condicionado a namedbnp */

    LONGX boolhits=0;
    LONGX boolh;
    LONGX booltell=0L;               /* 0L = no msg; 1L = tell; 2L = tell+root */
    LONGX booltlir=0L;               /* b70 invx */
    b7_CIB7 *cib7p=NULL;
    B7HITLIST *hitlistp;            /* hit processing */
    LONGX *mfnlistp;                 /* hit processing */

    char booltmpnam[128]={'\0'};    /* b72 MFQTHMEM tmp file */

    /* ---------------------------------------------------------------------- */
/* tabs here */
    TABFREQF_ENTRY *tabfreqF=NULL; /* indexed by rankF */
    TABFREQW_ENTRY *tabfreqW=NULL; /* indexed by rankW */
    TABFREQD_ENTRY *tabfreqD=NULL; /* indexed by rankD */

    LONGX docsTOTxA;    /* v9991[1] = docs de xA - nao vazios */
    LONGX docsTOTamdl;  /* v9991[2] = docs de amdl */

    LONGX count1=0L;                /* registros lidos */
    LONGX count2=0L;                /* casos tabulados */

    LONGX drec=0L;
    char *gidbnp=NULL;

    int parmtrace=0;
/*nu     LONGX parmfrom=1,parmto=999999999L,parmcount=0; */

/* -------------------------------------------------------------------------- */
    char *deffnamp=NULL;
    char *deffmhxp=NULL;
    char deffbuffup[ENVASIZE];

    char *cipfnamp=NULL;
    char *decsdbnp=NULL;
    char *amdldbnp=NULL;
    char *atmdldbnp=NULL;
    char *tmp1dbnp=NULL;
    char *pftfnamp=NULL;
    char *langp=NULL;
    LONGX  parmloopTA=0;
    LONGX  parmtellTA=0;
    LONGX  parmfromDA=0;
    LONGX  parmcountDA=0;
    char *parmsortp=NULL;
    char *parmdownp=NULL;
    char *namedbnp=NULL;     /* b7x query dbn */
    char *boolqryp=NULL;     /* bool= */
    LONGX  boolsetno=0;       /* b7x set no. */
    int   parmatmdl99=0;

    char *prmloopTA =NULL;
    char *prmfromDA =NULL;
    char *prmcountDA=NULL;
    char *prmboolset=NULL;
    char *prmatmdl99=NULL;

    char *actionp=NULL;

    int   actionCG=0;
    int   actionDD=0;
    int   actionTA=0;
    int   actionDA=0;

    int   parmdown=0;

/* -------------------------------------------------------------------------- */
    char namedbnarea[BUFSIZ];

    char dbnoutF[BUFSIZ];
    char dbnoutW[BUFSIZ];
    char dbnoutD[BUFSIZ];
/* -------------------------------------------------------------------------- */

#if CICGI
    char *cgipfxp="tag";
    UWORD cgitag=2000;
    char *wwwp=NULL;
    LONGX wrec;
#endif

    /* here we go..
    */
#if CIAPI
    ciapip = cisisApplication( MAXNREC,MAXNTRM );/* mandatory for CIAPI definitions */
#endif /* CIAPI */

    /* get calling arguments
    */
    for (i=0; i < argc; i++) argv[i]=argv_main[i];

    /* set watch
    */
    vtimid[ntimes]="start"; time(&vtime[ntimes++]); /* watch */

    /* set ansiuc & ansiac
    */
#if 1  /* ANSITABS */
    memmove(isisuctab,ansiuctab,sizeof(ansiuctab));
    memset(isiswctab,0x00,256);
    for (isiswctot=0; ansiactab[isiswctot]; isiswctot=isiswctot+1) isiswctab[ansiactab[isiswctot]]=1;
#endif /* 0000 */

    /* check if trace is present */
    for (i=0; i < argc; i++) {
        p=argv[i];
        if (strncmp(p,"trace?",6) == 0 || strncmp(p,"trace=",6) == 0) {
            if (sscanf(p+6,"%d",&parmtrace) != 1) fatal(p);
              if (parmtrace) {
                  printf("+++trace: trace='%.11s\n'",p);
                  printf("Content-type: text/plain\n\n");
                  printf("+++trace: \n");
              }
            break;
        }
    }

#ifndef FORCECGI
#define FORCECGI 0
#endif
#if FORCECGI
    putenv("REQUEST_METHOD=GET");
/*
    putenv("QUERY_STRING=lang=p&action=C%2CTA%2CDA&amdl=amdl351&bool=009068&bool=034268&bool=034269&loop=100&sort=f&from=1&count=20&x=39&y=3");
*/
    putenv("QUERY_STRING=lang=p&action=C,TA,DA&amdl=MEDLINE_6687.def&loop=1&bool=006621&sort=f&from=1&count=20");
#endif


    /* get CGI data
    */
#if CICGI
    RECORD(wrec=nrecs,"null",1L); RECrc=RCNORMAL; MFRstatus=ACTIVE;
    wwwp=NULL; cicgi0(&argc, argv, &wwwp, cgitag, cgipfxp);

    if (wwwp) {

      DIRSTRU *dirp;
      int xdir,iocc,loop;
      if (fldupdat(wrec,wwwp)) fatal(wwwp);

      /* check if trace is present */
      for (parmtrace=0, dirp=MFRdir, xdir=0, loop=MFRnvf; loop--; dirp++, xdir++)
        if (dirp->tag == cgitag) {
          p=FIELDP(xdir);
          if (strncmp(p,"^ntrace^v",2+5+2) == 0) {
              parmtrace=1;
              if (parmtrace) {
                  printf("+++trace: trace='%.11s\n'",p);
                  printf("Content-type: text/plain\n\n");
                  printf("+++trace: \n");
                  break;
              }
          }
        }

      /* check if cgi is enabled */
      for (actionCG=0, dirp=MFRdir, xdir=0, loop=MFRnvf; loop--; dirp++, xdir++)
        if (dirp->tag == cgitag) {
          p=FIELDP(xdir);
          if (strncmp(p,"^naction^vC",2+6+2+1) == 0) {
              actionCG=1;
              if (parmtrace) printf("+++trace: action='%.11s'\n",p);
              break;
          }
        }

      if (actionCG) { /* cgi is enabled */

      for (dirp=MFRdir, xdir=iocc=0, loop=MFRnvf; loop--; dirp++, xdir++) {
        if (dirp->tag == cgitag) {
          FFI plen;
          p=FIELDP(xdir); plen=dirp->len; iocc++;
          /* get CGI by name
          */
          if (strncmp(p,"^ncip^v"    , 3+4) == 0)   cipfnamp  =recsfldv(NULL,p,plen,"");
          if (strncmp(p,"^ndecs^v"   , 4+4) == 0)   decsdbnp  =recsfldv(NULL,p,plen,"");
          if (strncmp(p,"^namdl^v"   , 4+4) == 0)   amdldbnp  =recsfldv(NULL,p,plen,"");
          if (strncmp(p,"^natmdl^v"  , 5+4) == 0)   atmdldbnp =recsfldv(NULL,p,plen,"");
          if (strncmp(p,"^ntmp1^v"   , 4+4) == 0)   tmp1dbnp  =recsfldv(NULL,p,plen,"");
          if (strncmp(p,"^npft^v"    , 3+4) == 0)   pftfnamp  =recsfldv(NULL,p,plen,"");
          if (strncmp(p,"^nlang^v"   , 4+4) == 0)   langp     =recsfldv(NULL,p,plen,"");
          if (strncmp(p,"^nloop^v"   , 4+4) == 0) { prmloopTA =recsfldv(NULL,p,plen,""); parmloopTA =atol(prmloopTA ); }
          if (strncmp(p,"^nfrom^v"   , 4+4) == 0) { prmfromDA =recsfldv(NULL,p,plen,""); parmfromDA =atol(prmfromDA ); }
          if (strncmp(p,"^ncount^v"  , 5+4) == 0) { prmcountDA=recsfldv(NULL,p,plen,""); parmcountDA=atol(prmcountDA); }
          if (strncmp(p,"^nsort^v"   , 4+4) == 0)   parmsortp =recsfldv(NULL,p,plen,"");
          if (strncmp(p,"^ndown^v"   , 4+4) == 0)   parmdownp =recsfldv(NULL,p,plen,"");
          q=NULL;
          if (strncmp(p,"^nbool^v"   , 4+4) == 0)   q         =recsfldv(NULL,p,plen,"");
          if (q) {
              if (boolqryp) {
                  sprintf(envarea,"%s or %s",boolqryp,q);
                  boolqryp=strdup(envarea);
              }
              else boolqryp=q;
          }
          if (strncmp(p,"^nname^v"   , 4+4) == 0)   namedbnp  =recsfldv(NULL,p,plen,"");
          if (strncmp(p,"^natmdl99^v", 7+4) == 0) { prmatmdl99=recsfldv(NULL,p,plen,""); parmatmdl99=atoi(prmatmdl99); }
          if (strncmp(p,"^naction^v" , 6+4) == 0)   actionp   =recsfldv(NULL,p,plen,"");
          if (strncmp(p,"^nsetno^v"   , 5+4) == 0) { prmboolset=recsfldv(NULL,p,plen,""); boolsetno=atol(prmboolset); }

        } /* end if (dirp->tag == cgitag) */
      } /* end for (dirp=MFRdir) */
    } /* end if (cgi is enabled */
    } /* end if (wwwp) */
#endif /* CICGI */


    /* set default values
    */
#if 0
PATH_DATA=/iah/online/
PATH_CGI-BIN=/users/www/cgi-bin/iah/
#endif

#if UNIX
    if (!cipfnamp)    cipfnamp    = "/users/www/aot/aotmsa1/cip.cip";
    if (!decsdbnp)    decsdbnp    = "decs";
    if (!amdldbnp)    amdldbnp    = "/users/www/data/iah/online/MEDLINE_2001.def";
#else
    if (!cipfnamp)    cipfnamp    = "c:/2001x300/usersaot/cip.cip";
    if (!decsdbnp)    decsdbnp    = "decs";
    if (!amdldbnp)    amdldbnp    = "amdl351";
#endif
    if (!atmdldbnp)   atmdldbnp   = "atmdl";
    if (!tmp1dbnp)    tmp1dbnp    = NULL;
    if (!pftfnamp)    pftfnamp    = "aotmsa1A.pft";
    if (!langp)       langp       = "p";
    if (!parmloopTA)  parmloopTA  = 1;
    if (!parmtellTA)  parmtellTA  = 0; /* */
    if (!parmfromDA)  parmfromDA  = 1;
    if (!parmcountDA) parmcountDA = 0;
    if (!parmsortp)   parmsortp   = "f";
    if (!parmdownp)   parmdownp   = "0";
    if (!parmatmdl99) parmatmdl99 = 0;
    if (!actionp)    {actionCG    = 0;
                      actionDD    = 0;
                      actionTA    = 0;
                      actionDA    = 0; }
    if (!boolsetno)   boolsetno   = 0;

    dbnoutF[0]='\0';   /* .f */
    dbnoutW[0]='\0';   /* .w */
    dbnoutD[0]='\0';   /* .w */


    if (argc < 2 && !wwwp) {
        printf("%s",cicopyr("Application AOTMSA1"));
        printf(" \n");
        printf("aotmsa1 <action>[,...] [<option> [...]] \n");
        printf(" \n");
        printf("actions:  C   - get parameters from cgi \n");
        printf("          TA  - search amdl \n");
        printf("                tabulate amdl (v351/) \n");
        printf("                store tableF and tableW \n");
        printf("                DA  - read tableF or tableW \n");
        printf("                      display formA results \n");
        printf(" \n");
        printf("options: loop=<n>       \n");
        printf("         from|count=<n> \n");
        printf("         sort={f|w} \n");
        printf("         btell|tell=<n> \n");
        printf("         bool=<expr> [setno=<n>]  \n");
        printf("         atmdl99    \n");
        printf(" \n");
        printf("         cip    ,                              %s \n",  cipfnamp    );
        printf("         decs   ,                              %s \n",  decsdbnp    );
        printf("         amdl   ,                              %s \n",  amdldbnp    );
        printf("         atmdl  ,                              %s \n",  atmdldbnp   );
        printf("         tmp1   ,                              %s \n",  tmp1dbnp    );
        printf("         pft    ,                              %s \n",  pftfnamp    );
        printf("         lang   ,                              %s \n",  langp       );
        printf("         loop   ,                              %"_LD_" \n",  parmloopTA  );
        printf("         from   ,                              %"_LD_" \n",  parmfromDA  );
        printf("         count  ,                              %"_LD_" \n",  parmcountDA );
        printf("         sort   ,                              %s \n",  parmsortp   );
        printf("         down   ,                              %s \n",  parmdownp   );
        printf("         bool   ,                              %s \n",  boolqryp    );
        printf("         name   ,                              %s \n",  namedbnp    );
        printf("         atmdl99,                              %d \n",  parmatmdl99 );
        printf("         action ,                              %s \n",  actionp     );
        printf("         setno  ,                              %"_LD_" \n",  boolsetno );
        printf(" \n");
        exit(1);
        }


    /*
        get options
    */
    for (; argnext < argc; argnext++) {
        p=argv[argnext];

        if (strncmp(p,"cip?",4) == 0 || strncmp(p,"cip=",4) == 0) {
            cipfnamp=p+4;
            continue;
        }
        if (strncmp(p,"decs?",5) == 0 || strncmp(p,"decs=",5) == 0) {
            decsdbnp=p+5;
            continue;
        }
        if (strncmp(p,"amdl?",5) == 0 || strncmp(p,"amdl=",5) == 0) {
            amdldbnp=p+5;
            continue;
        }
        if (strncmp(p,"atmdl?",6) == 0 || strncmp(p,"atmdl=",6) == 0) {
            atmdldbnp=p+6;
            continue;
        }
        if (strncmp(p,"tmp1?",5) == 0 || strncmp(p,"tmp1=",5) == 0) {
            tmp1dbnp=p+5;
            continue;
        }
        if (strncmp(p,"pft?",4) == 0 || strncmp(p,"pft=",4) == 0) {
            pftfnamp=p+4;
            continue;
        }
        if (strncmp(p,"loop?",5) == 0 || strncmp(p,"loop=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&parmloopTA) != 1) fatal(p);
            continue;
        }
        if (strncmp(p,"from?",5) == 0 || strncmp(p,"from=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&parmfromDA) != 1) fatal(p);
            continue;
        }
        if (strncmp(p,"count?",6) == 0 || strncmp(p,"count=",6) == 0) {
            if (sscanf(p+6,"%"_LD_,&parmcountDA) != 1) fatal(p);
            continue;
        }
        if (strncmp(p,"sort?",5) == 0 || strncmp(p,"sort=",5) == 0) {
            parmsortp=p+5;
            continue;
        }
        if (strncmp(p,"down?",5) == 0 || strncmp(p,"down=",5) == 0) {
            parmdownp=p+5;
            if (sscanf(parmdownp,"%d",&parmdown) != 1) fatal(p);
            continue;
        }
        if (!strncmp(p,"bool=",5)) {
            q=loadfile(gidbnp,'@',p+5,NULL,0L,'\n');
            if (!q) fatal(p);
            if (boolqryp) {
                  sprintf(envarea,"%s or %s",boolqryp,q);
                  boolqryp=strdup(envarea);
            }
            else boolqryp=q;
            continue;
        }
        if (strncmp(p,"name?",5) == 0 || strncmp(p,"name=",5) == 0) {
            namedbnp=p+5;
            strcpy(namedbnarea,namedbnp);
            continue;
        }
        if (strcmp(p,"atmdl99") == 0) {
            parmatmdl99=1;
            continue;
        }
        if (strncmp(p,"action?",7) == 0 || strncmp(p,"action=",7) == 0) {
            actionp=q=p+7;
            if (!*q) fatal(p);
            continue;
        }
        if (strncmp(p,"setno?",6) == 0 || strncmp(p,"setno=",6) == 0) {
            if (sscanf(p+6,"%"_LD_,&boolsetno) != 1) fatal(p);
            continue;
        }
        /* offer */
        if (strncmp(p,"tell?",5) == 0 || strncmp(p,"tell=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&parmtellTA) != 1) fatal(p);
            continue;
        }
        if (strncmp(p,"btell?",6) == 0 || strncmp(p,"btell=",6) == 0) {
            if (sscanf(p+6,"%"_LD_,&booltell) != 1) fatal(p);
            continue;
        }
        if (strcmp(p,"trace") == 0) {
            parmtrace=1;
            continue;
        }

        fatal(p);
    }


    /* actions */
    if (actionp) {
      for (q=actionp; *q; ) {
      
         if (*q == 'C' && strncmp(q,"C" ,1) == 0)
                   { /*actionCG=1;*/                  if (*(q+=1) == ',') q++; continue; }
                   
         if (isdigit(*q))
                   {   actionDD=(int)(*q)-(int)'0';   if (*(q+=1) == ',') q++; continue; }
                   
         if (*q == 'T' && strncmp(q,"TA",2) == 0)
                   {   actionTA=1;                    if (*(q+=2) == ',') q++; continue; }
                   
         if (*q == 'D' && strncmp(q,"DA",2) == 0)
                   {   actionDA=1;                    if (*(q+=2) == ',') q++; continue; }

         if (*q) fatal(p);
      }
    }

    if (parmsortp) {
        q=parmsortp;
        if (*q == 'g') { parmgroup=1;    q++; parmsortp=q; }
        if (*q == '1') { parmshowgrp1=1; q++; }
        if (*q == 'c') { parmshowcat1=1; q++; }
        if (*q == 't') { parmshowtree=1; q++; }
        if (*q == 'z') { parmshowzero=1; q++; }
        if (strcmp(q,"f") && strcmp(q,"w") && strcmp(q,"d")) fatal(parmsortp);
        parmsortx= *q;
    }
#if 0
    for (q=parmsortp; *q; q++) {
        if (*q == 'g') { parmgroup=1; parmsortp=q+1; }
        if (*q == '1') { parmshowgrp1=1; }
        if (*q == 'c') { parmshowcat1=1; }
        if (*q == 't') { parmshowtree=1; }
        if (*q == 'z') { parmshowzero=1; }
        if (*q == 'f' || *q == 'w' || *q == 'd') { parmsortx= *q; }
    }
#endif

if (parmtrace) {
  if (amdldbnp) printf("+++trace: amdldbnp=%s\n",amdldbnp);
  if (tmp1dbnp) printf("+++trace: tmp1dbnp=%s\n",tmp1dbnp);
  printf("+++trace: actionp=%s\n",actionp);
  printf("+++trace: actionDD=%d\n",actionDD);
  printf("+++trace: actionTA=%d\n",actionTA);
  printf("+++trace: actionDA=%d\n",actionDA);
  printf("+++trace:     sort=%s\n",parmsortp);
  printf("+++trace:    group=%d\n",parmgroup);
}


    /* Common setup
    */

    /* get MEDLINE*.def parameters
    */
    if (strlen(amdldbnp) > 4) if (strcmp(amdldbnp+strlen(amdldbnp)-4,".def") == 0) {
        char loadname[1+BUFSIZ];
        char *defarea;
        deffnamp=amdldbnp;
        sprintf(loadname,"@%s",deffnamp);
        defarea=loadfile(gidbnp,'@',loadname,NULL,0L,'\n');
        if (!defarea) {
                printf("Content-type: text/plain\n\n");
                printf("file %s not found \n",deffnamp);
                exit(120);
        }
/*
FILE DATABASE.*=/base2/mdl/back3/mdl8590.*
FILE MH.*=/base1/mdl/back3/mdlmh%lang%.*
FILE M.*=/base1/mdl/back3/mdlmh.*
*/
        p=strstr(defarea,"FILE DATABASE.*=");
        if (p) if (strstr((p+=strlen("FILE DATABASE.*="))+1,".*")) {
            *(strstr(p+1,".*"))='\0';
            amdldbnp=strdup(p);
            if (parmtrace) printf("+++trace: amdldbnp=%s\n",amdldbnp);
        }
        p=strstr(defarea,"FILE MH.*=");
        if (p) if (strstr((p+=strlen("FILE MH.*="))+1,"%lang%.*")) {
            *(strstr(p+1,"%lang%.*"))='\0'; strcat(p,langp);
            deffmhxp=strdup(p);
            if (parmtrace) printf("+++trace: deffmhxp=%s\n",deffmhxp);
        }
        /* free(defarea); */
    }

    /* set cipar */
    if ((dbxcipfp=fopen(cipfnamp,"r")) == NULL) fatal(cipfnamp);
    /* rec_maxmfrl */
    p=dbxcipar(NULL,"maxmfrl",'='); if (dbxcipok) { sscanf(p,"%"_LD_,&rec_maxmfrl); }

    /* set tempfile */
    if (tmp1dbnp) strcpy(booltmpnam,tmp1dbnp);

    /* set query db */
    if (namedbnp) {
        if (strncmp(namedbnp,"create=",7) == 0) {
            namedbnp+=7;
            recisis0(namedbnp);
        } else {
            int xfd;
            dbxopt_fatal=0; xfd=dbxopen("",namedbnp,xx1extp);
            if (xfd > 0) CLOSE(xfd); else recisis0(namedbnp);
        }
        RECORD(boolridx=nrecs,namedbnp,0L);
        if (boolsetno < 1 || boolsetno > MF0nxtmfn-1) boolsetno=MF0nxtmfn-1; /* last set */
    }
    else {
        if (!dbxtmpnm("CI_TEMPDIR", 0, namedbnarea)) fatal("aotmsa1/dbxtmpnm");
        namedbnp=namedbnarea;
        if (parmtrace) printf("+++trace: namedbnp=%s\n",namedbnp);
        recisis0(namedbnp);
        boolsetno=1L; /* set#1 */
    }

    /* make output file names
    */
    sprintf(dbnoutF,"%s%s",namedbnp,".f");
    sprintf(dbnoutW,"%s%s",namedbnp,".w");
    sprintf(dbnoutD,"%s%s",namedbnp,".d");

    parmdown=atoi(parmdownp);
    if (parmdown < 0x00 || parmdown > 0xFF) parmdown=0;
    

/* =======================         =============          ================== */
/* =======================         =============          ================== */
if (actionTA) {

    /* Tabulate A
    */

    /* init docsTOTamdl - number of docs in Cluster MDL */
    /* init reading */
    if (!irec) irec=nrecs;
    RECORD(irec,amdldbnp,0L);
    docsTOTamdl=MF0nxtmfn-1;
    if (parmatmdl99) {
        RECSTRU *recp;
        TRMSTRU *trmp;
        LONGX /*nu irec,*/itrm;
        char *p;
        TERM(itrm=ntrms,atmdldbnp,"999999"); if (TRMrc != RCNORMAL) fatal("aotmsa1/atmdl/TERM");
        RECORD(/*nu irec=*/nrecs,atmdldbnp,posting(itrm,1));  if (TRMrc != RCNORMAL) fatal("aotmsa1/atmdl/RECORD");
        p=recfield(NULL,boolridx,ATMDLTAG999,1,"");
        docsTOTamdl = atol(p); if (docsTOTamdl < 1) fatal("aotmsa1/atmdl/999999");
    }

    /* init docsTOTxa - number of docs in Cluster A */
    docsTOTxA=docsTOTamdl;

    vtimid[ntimes]="search"; time(&vtime[ntimes++]); /* watch */
    /*
        Search "bool=" parameter
    */
    if (boolqryp) { /* mxbool/boolh */

        RECSTRU *recp;
        char *p;
        LONGX mfn;
        int errno;

        LONGX asize=strlen(amdldbnp) +                 /* field MFQTDBN0=11 (may be longer than 128+4+1) */
                   strlen(boolqryp) +                 /* the expression */
                   2*strlen(boolqryp) +               /* the terms + pseudo operators _1termX+11X */
                   BUFSIZ+strlen(amdldbnp) +          /* field MFQSWDBN=20 + a diferent db */
                   24*(strlen(B7DELADD)+3*B7UPLEN) +  /* 2 duzias de control fields */  /* DxxAxx|xxx| */ 
                   strlen(boolqryp)+4 +               /* tmpfile .%03x */
                   BUFSIZ;                            /* por si las moscas */
                   
        char *buffup=loadfile(NULL,'@',"",NULL,asize,'\0');

        cib7p=b7_cib7(cib7p,ciapip);

        /* initialize ridx and b7_xxx() parameters and run b7_run()
        */
        for (boolridx=nrecs, mfn=1; ; mfn++) { /* get actual maxmfn */
            RECORD(boolridx,amdldbnp,mfn);
            if (RECrc == RCEOF) fatal(amdldbnp);
            if (RECrc == RCNORMAL) break;
        }

        if (namedbnp) {
            if (!booldbcrec) booldbcrec=nrecs;
            /*RECORD(booldbcrec,namedbnp,0L); */ /* b7_exp faz "record(crec,qrydbnp,0L)"
                                                          (se houver "qrydbnp")
                                                       */
/*<R>
            vrecp[boolridx]->recdbxp=RECdbxp;
*/          /* VRECdbxp(boolridx)=RECdbxp; */ /* b7_exp faz "RECdbxp = dbxstorp(qrydbnp)" */
        }
        else {
            VRECdbxp(boolridx)=NULL; /* nao usa base de log */
            VMFRmfn(boolridx)=0L;
        }

        /* run b7_exp() / b7_run() / b7_hit(0)
        */
        p=b7_exp(cib7p,boolridx,amdldbnp,boolqryp,buffup,namedbnp,booldbcrec,&errno);
        if (p || errno) printf("Expression syntax error %d: '%s'\n",errno,p);
        else {
            LONGX bufsiz;
            /* .. sprintf(buffup,"%s.%03x",RDBname,(int)b7setno) .. */
            char *hitfnamp="null";
            if (booltmpnam[0]) hitfnamp=booltmpnam;
            sprintf(buffup,"D%dA%d|%s|",MFQTHMEM,MFQTHMEM,hitfnamp);
            if (fldupdat(boolridx,buffup) != NULL) fatal("aotmsa1/fldupdat/boolridx");

            /* b7_run faz "trirecp=vrep[tlirec]" */
            if (!booltlir) record(booltlir=nrecs,"null",1L);
            
            /* set default index to actual INDEX MH= */
            if (deffmhxp) {
                char buffup[BUFSIZ+80];
                sprintf(buffup,"A101|^p*^y%s|",deffmhxp);
                if (fldupdat(booltlir,buffup)) fatal("aotmsa1/booltlir/fldupdat");
            }

            /* alloc buffer for 20000 hits = 80Kbytes */
            bufsiz=20000*sizeof(LONGX);
            if (b7bufferp) b7_hbfree(cib7p,&b7bufferp);
            b7_hballoc(cib7p,bufsiz,&b7bufferp);

#if 1
            /* translate DecsMfn to MH
            */
            if (deffmhxp) {
                char *batchp;
                int xdir;
                if ((xdir=fieldx(boolridx,MFQTOBJ0,1)) >= 0) {
                    char *b7p;
                    LONGX v351;
                    sprintf(batchp,"D%dA%d|",MFQTOBJ0,MFQTOBJ0); batchp+=strlen(batchp);
                    for (b7p=(unsigned char *)FIELDP(xdir); *b7p; ) {
                        if (*b7p == OPRXEND) break;
                        if (*b7p != ZLDV) {
                            *batchp++ = *b7p++; /* move other */
                            continue;
                        }
                        *batchp++ = *b7p++; /* move b7opr */
                        *batchp++ = *b7p++; /* move postlen */
                        v351=atol(b7p);
                        while (*b7p != OPRNULL) b7p++; /* skip DecsMfn */
                        if (v351) {
                            if (!drec) drec=nrecs;
                            record(drec,decsdbnp,v351);
                            recfield(batchp,drec,DECSTAGNAME(langp),1,"");  batchp[LE2]='\0';  /* move MH */
                            while (*batchp) { *batchp=isisuctab[*batchp]; batchp++; }
                        }
                        *batchp++ = *b7p++; /* move OPRNULL */
                    } /* end for (b7p) */
                    *batchp++ = *b7p++; /* move OPRXEND */
                } /* end if (xdir) */
                sprintf(batchp,"|"                           ); /*nu batchp+=strlen(batchp); */
                batchp=deffbuffup; *batchp='\0';
            } /* end if (deffmhxp) */
#endif
            
            /* run search */
            p=b7_run(cib7p,boolridx,amdldbnp,buffup,asize,
                booltell,isisuctab,deffbuffup/*btch0p*/,booltlir,booldbcrec,&errno);
            if (p || errno) printf("Execution error %d: '%s'\n",errno,p);
            else {
                boolhits=b7bufferp->ndocs; /* b7_hit(cib7p,boolridx,0L,b7setno); */
                if (booltell) printf("Hits=%"_LD_"\n",boolhits);
            }
        }
        if (p || errno) fatal("Expression execution error");

        /* set# */
        boolsetno=b7setno;  /* last mfn */

        /* set docsTOTxa - number of docs in Cluster A */
        docsTOTxA=boolhits;

    } /* mxbool/boolh */


    vtimid[ntimes]="set"; time(&vtime[ntimes++]); /* watch */

    /* init tabcntMDL
    */
    if (parmloadmdl) {
        /* loadcnt(amdldbnp,treecase,tabcntMDL,CLASSES);
AMDL351  CNT            52  12-21-01  9:54p amdl351.cnt
AMDL351  N01        21,216  12-21-01 10:00p amdl351.n01
AMDL351  N02             0  12-21-01 10:00p amdl351.n02
AMDL351  LY1       269,064  12-21-01  9:55p amdl351.ly1
AMDL351  LY2             0  12-21-01  9:55p amdl351.ly2
AMDL351  IYP    48,611,328  12-21-01  9:55p amdl351.iyp
        */
        int treecase=0;
        INVMAP *invp;
        LONGX punt,punt2;
        char lparea[sizeof(L1STRU)];
        L1STRU *l1p=(L1STRU *)lparea;
        int loop;
        char *keyp;
        int v351;
        LONGX ndocs;
        LONGX tell=0;

        memset(tabcntMDL,0x00,sizeof(tabcntMDL));
        invsetup(amdldbnp,0L,0L,0L);
        invp=dbxstorp(amdldbnp)->dbxifmap;
        if (parmtellTA) tell=(punt2=invp->cn[treecase].fmaxpos)/10+1;
        if (parmtrace) fprintf(stderr,"+++ punt2=%"_LD_"\n",punt2);
        /* for each leaf */
        for (punt=1; punt; punt=l1p->ps) {
            leafread(lparea,invp,treecase,punt,0); /* read a leaf */
            /* for each term */
            for (loop=0; loop < l1p->ock; loop++) {
                keyp=l1p->idx[loop].key;
                v351=atoi(keyp); /* LE1 */
                ndocs=l1p->idx[loop].info2;
                tabcntMDL[v351]=ndocs;
            }
            if (parmtrace) if (tell) if (punt%tell==0) fprintf(stderr,"+++ punt=%"_LD_"\n",punt);
        }

        memset(tabcntGMDL,0x00,sizeof(tabcntGMDL));
        if (parmgroup) {
            RECSTRU *recp;
            LONGX irec=nrecs;
            LONGX mfn;
            char x351[6+1];
            LONGX v351;
            DIRSTRU *dirp;
            int xdir,loop;

            for (mfn=1; ; mfn++) {
                RECORD(irec,"group01.f",mfn);
                if (RECrc == RCEOF) break;
                if (RECrc != RCNORMAL) continue;
                /* get total records for each group */
#define G01TAG999 999
#define G01TAG001   1
                for (dirp=MFRdir, xdir=0, loop=MFRnvf; loop--; dirp++, xdir++) {
                    if (dirp->tag == G01TAG999) {
                       LONGX ndocs=0;
                       char *p=FIELDP(xdir);
                       int loop;
                       for (loop=dirp->len; loop--; p++) ndocs=ndocs*10+((int)(*p)-(int)'0');
                       recfield(x351,irec,G01TAG001,1,""); v351=atol(x351);
                       tabcntGMDL[v351]=ndocs;
#if 0
printf("+++tabcntGMDL[%"_LD_"]=%"_LD_"\n",v351,tabcntGMDL[v351]);
getchar();
#endif
                       break;
                    } /* end tag == 999 */
                } /* end for dirp */
            } /* end for mfn */
        }

        /* VTABV05*/
        memset(vcategmfn,0x00,sizeof(vcategmfn));
        memset(vtabv05len,0x00,sizeof(vcategmfn)); ntabv05=0;

    }


    /* init mxtb
    */
    memset(tabcntA,0x00,sizeof(tabcntA));
    memset(gtabcntA,0x00,sizeof(gtabcntA));

    /* create output
    */
    recisis0(dbnoutF);
    recisis0(dbnoutW);
    recisis0(dbnoutD);


    vtimid[ntimes]="loop"; time(&vtime[ntimes++]); /* watch */

    /*
        loop principal
    */

#define MAXN20 30
#define MAXZ20 12 /* hoje=10 (1*3+9*4=39) */
#define MAXL20 (MAXZ20*4)

#define MAXNVG 30
#define MAXN05 30

    for (boolh=1; boolh <= boolhits; boolh+=parmloopTA) {

        LONGX mfn;
        int v351,v351nocc,xdir,loop;
        DIRSTRU *dirp;
        char area[BUFSIZ];

        LONGX vgmfns[MAXNVG];
        int nvgs=0;
        int ivg;

        /* hit processing */

        if (boolh == 1) {
            hitlistp=(B7HITLIST *)b7bufferp->area;
            mfnlistp=hitlistp->mfnlist;
        }

        /* get next hit */
        if (b7bufferp->hcase == BITSTRING)
            mfn=b7_hit(cib7p,boolridx,boolh,b7setno);
        else
            mfn = *mfnlistp; mfnlistp+=parmloopTA;

        if (mfn <= 0) fatal("aotmsa1/TA/amdldbnp/hit");

        if (parmtrace) printf("+++ hit#%"_LD_" mfn=%"_LD_"\n",boolh,mfn);

        RECORD(irec,amdldbnp,mfn);
        if (RECrc != RCNORMAL) fatal("aotmsa1/TA/amdldbnp/RCNORMAL");

        /* tabulate (v351/) in tabcntA, exploding in gtabcntA
        */
        /* get a subject descriptor */
        v351nocc=0;
        for (dirp=MFRdir, xdir=0, loop=MFRnvf; loop--; dirp++, xdir++) {
            if (dirp->tag == TAG351) {

                /* tabulate it */
                memmove(area,FIELDP(xdir),dirp->len); area[dirp->len]='\0';
                v351=atoi(area);
                if (1/*parmgroup*/)  {
#if 1 /*V02BAK9J*/ /* v0.2bak9j */
                    RECSTRU *recp; /* local */
                    int xdir,loop;
                    DIRSTRU *dirp;
#if 0
                    int parmgrouplen=4;
                    LONGX mfn;
                    char *p;
                    char v20[MAXN20][MAXL20+1];
                    int n20=0;
                    char x20[MAXL20+1];
                    int len,found;
                    int glen=(parmgroup*parmgrouplen)-1;
#else
                    LONGX v05[MAXN05];
                    int n05=0;
#endif

                    if (!mydrec) {
                        LONGX loadxrf=200000;
                        LONGX loadmst=4000000;
                        mstsetup("mydecs",loadxrf,loadmst);
                        mydrec=nrecs;
                    }
                    RECORD(mydrec,"mydecs",v351);
                    if (RECrc != RCNORMAL) fatal("aotmsa1/TA/mydecs/RCNORMAL");
#if 0
                    /* build v351groups in v20[] from mydecs (v20/) */
                    for (dirp=MFRdir, xdir=0, loop=MFRnvf; loop--; dirp++, xdir++) {
                        if (dirp->tag == TAG020) {
                            p=FIELDP(xdir);
                            if (*p == 'C' or *p == 'D') { /* ... */
                                if ((len=dirp->len) > glen) len=glen;
                                memmove(x20,p,len);
                                if (len < glen) memset(x20+len,' ',glen-len)
                                for (found=0, i=0; i<n20; i++)
                                    if (memcmp(x20,v20[i],parmgrouplen1) == 0) { found=1; break; }
                                if (!found) {
                                    if (n20 >= MAXN20) fatal("aotmsa1/TA/mydecs/MAXN20");
                                    memmove(v20[n20++],x20,parmgrouplen);
                                }
                            } /* end 'C' or 'D' */
                        } /* end tag == TAG020 */
                    } /* end for dirp */
#else
                    /* store v351groups in v05[] from mydecs (v5/) */
                    for (dirp=MFRdir, xdir=0, loop=MFRnvf; loop--; dirp++, xdir++) {
                        if (dirp->tag == TAG005) {
                           LONGX mfn=0;
                           char *p=FIELDP(xdir);
                           int loop;
                           for (loop=dirp->len; loop--; p++) mfn=mfn*10+((int)(*p)-(int)'0');
                           if (n05 >= MAXN05) fatal("aotmsa1/TA/mydecs/MAXN05");
                           v05[n05++]=mfn;
                        } /* end tag == TAG005 */
                    } /* end for dirp */
#endif
                    /* store v05groups in vgmfns[] */
                    for (i=0; i<n05; i++) {
                        LONGX mfn=v05[i];
                        int found;
                        for (found=0, ivg=0; ivg < nvgs; ivg++)
                            if (mfn == vgmfns[ivg]) { found=1; break; }
                        if (!found) {
                            if (nvgs >= MAXNVG) fatal("aotmsa1/TA/mydecs/MAXNVG");
                            vgmfns[nvgs++]=mfn;
                        }
                    } /* end for v05[] */
#else /* v0.2bak9j */
#endif /* v0.2bak9j */
                } /* end if (1 parmgroup) */

                if (v351 >= CLASSES) fatal("aotmsa1/CLASSES overflow/v351");
                if (!tabcntA[v351]) ntabs++;
                tabcntA[v351]++;
                v351nocc++;

            } /* end if (dirp->tag == TAG351) */
        } /* end for (dirp=MFRdir, ) */

        if (v351nocc) count2++;
        
        /*
           tabulate groups
        */
        for (ivg=0; ivg < nvgs; ivg++) {
            LONGX gmfn=vgmfns[ivg];
            if (gmfn >= CLASSES) fatal("aotmsa1/CLASSES overflow/gmfn");
            if (!gtabcntA[gmfn]) gntabs++;
            gtabcntA[gmfn]++;
        } /* end for vgmfns[] */


        /* .... */

        count1++;
        if (parmtellTA) if ((count1 % parmtellTA) == 0)
          fprintf(stderr,"+++ %"_LD_": %"_LD_"/%d \n",count1,count2,ntabs);
    
    } /* end for (boolh=1; boolh <= boolhits;  ) */

    /*
    if (booltmpnam[0]) remove(booltmpnam);   / * b72 MFQTHMEM tmp file * /
    */
    


    /* run tabfreqF, tabfreqW and tabfreqD
    */
    if (ntabs) {
#if DCTFILTER
        /* set docsTOTxa - number of docs in Cluster A */
        docsTOTxA=count2_filtered;
#endif

        /* run tabfreqF
        */
        vtimid[ntimes]="freqF"; time(&vtime[ntimes++]); /* watch */
        if (parmtrace) fprintf(stderr,"+++ ntabs=%d ..\n",ntabs);
        if (parmtrace) fprintf(stderr,"+++ running %s ..\n",dbnoutF);
        if (!drec) record(drec=nrecs,decsdbnp,0L);
ddrec=drec;
ddecsdbnp=decsdbnp;
        if (rutfreqF(tabcntA,ntabs,drec,tabcntArankF,&tabfreqF)) exit(3);

        /* run tabfreqW
        */
        vtimid[ntimes]="freqW"; time(&vtime[ntimes++]); /* watch */
        if (parmtrace) fprintf(stderr,"+++ runing %s ..\n",dbnoutW);
        if (rutfreqW(tabfreqF,ntabs,(deffmhxp?deffmhxp:amdldbnp),docsTOTxA,docsTOTamdl,tabcntArankF,tabcntArankW,&tabfreqW,(parmloadmdl?tabcntMDL:NULL),gtabcntA)) exit(4);

        /* run tabfreqD
        */
        vtimid[ntimes]="freqD"; time(&vtime[ntimes++]); /* watch */
        if (parmtrace) fprintf(stderr,"+++ runing %s ..\n",dbnoutD);
        if (rutfreqD(tabfreqF,ntabs,tabcntArankF,tabcntArankW,tabcntArankD,&tabfreqD,tabcntGMDL, gtabcntA)) exit(5);

        /* output tabfreqD
        */
        vtimid[ntimes]="outD"; time(&vtime[ntimes++]); /* watch */
        if (parmtrace) fprintf(stderr,"+++ updating %s ..\n",dbnoutD);
        if (outfreqD(ntabs,dbnoutD,tabcntArankF,tabcntArankW,tabfreqD)) exit(6);
        /* output tabfreqF
        */
        vtimid[ntimes]="outF"; time(&vtime[ntimes++]); /* watch */
        if (parmtrace) fprintf(stderr,"+++ updating %s ..\n",dbnoutF);
        if (outfreqF(ntabs,dbnoutF,tabcntArankW,tabcntArankD,tabfreqF)) exit(7);
        /* output tabfreqW
        */
        vtimid[ntimes]="outW"; time(&vtime[ntimes++]); /* watch */
        if (parmtrace) fprintf(stderr,"+++ updating %s ..\n",dbnoutW);
        if (outfreqW(ntabs,dbnoutW,docsTOTxA,docsTOTamdl,tabcntArankF,tabcntArankD,tabfreqD,tabfreqW)) exit(8);
    }

if (ntabs) {

    /* log A1
    */
    if (!parmaid) parmaid="A1";
    if (strcmp(parmaid,"A1") == 0) {
        char *batchp=envarea;
    /* environment */
    if (usernamp   ) { sprintf(batchp,"a3300|%s|" , usernamp    ); batchp+=strlen(batchp); }
    if (cipfnamp   ) { sprintf(batchp,"a3000|%s|" , cipfnamp    ); batchp+=strlen(batchp); }
    if (decsdbnp   ) { sprintf(batchp,"a3001|%s|" , decsdbnp    ); batchp+=strlen(batchp); }
    if (amdldbnp   ) { sprintf(batchp,"a3002|%s|" , amdldbnp/**/); batchp+=strlen(batchp); }
    if (atmdldbnp  ) { sprintf(batchp,"a3003|%s|" , atmdldbnp   ); batchp+=strlen(batchp); }
    if (tmp1dbnp   ) { sprintf(batchp,"a3004|%s|" , tmp1dbnp    ); batchp+=strlen(batchp); }
    if (pftfnamp   ) { sprintf(batchp,"a3005|%s|" , pftfnamp    ); batchp+=strlen(batchp); }
    if (langp      ) { sprintf(batchp,"a3006|%s|" , langp       ); batchp+=strlen(batchp); }
    if (parmloopTA ) { sprintf(batchp,"a3007|%"_LD_"|", parmloopTA  ); batchp+=strlen(batchp); }
    if (parmfromDA ) { sprintf(batchp,"a3008|%"_LD_"|", parmfromDA  ); batchp+=strlen(batchp); }
    if (parmcountDA) { sprintf(batchp,"a3009|%"_LD_"|", parmcountDA ); batchp+=strlen(batchp); }
    if (parmsortp  ) { sprintf(batchp,"a3010|%s|" , parmsortp   ); batchp+=strlen(batchp); }
    if (boolqryp   ) { sprintf(batchp,"a3011|%s|" , boolqryp    ); batchp+=strlen(batchp); }
    if (namedbnp   ) { sprintf(batchp,"a3012|%s|" , namedbnp    ); batchp+=strlen(batchp); }
    if (parmatmdl99) { sprintf(batchp,"a3013|%d|" , parmatmdl99 ); batchp+=strlen(batchp); }
    if (actionp    ) { sprintf(batchp,"a3014|%s|" , actionp     ); batchp+=strlen(batchp); }
/**/
                       sprintf(batchp,"a3015|%"_LD_"|", boolsetno   ); batchp+=strlen(batchp);
                       sprintf(batchp,"a4001|%d|" , ntabs       ); batchp+=strlen(batchp);
/**/
        if (1) {
        /* add watch */
            int hours,mins,secs;
            int i;
            vtimid[ntimes]="end1"; time(&vtime[ntimes++]); /* watch */
            //vtimid[ntimes]="xT";   time(&vtime[ntimes++]); /* watch */
            secs=vtime[ntimes-1]-vtime[0];
            hours=secs/3600; secs-=hours*3600;
            mins=secs/60; secs-=mins*60;
            sprintf(batchp,"a3216|%02d:%02d:%02d|",hours,mins,secs); batchp+=strlen(batchp);
            for (i=0; i<ntimes-1; i++) {
                sprintf(batchp,"a3214|%s|",  vtimid[i]);             batchp+=strlen(batchp);
                sprintf(batchp,"a3215|%"_LD_"|", vtime[i+1]-vtime[i]);   batchp+=strlen(batchp);
            }
            //ntimes--;
        }

        if (fldupdat(boolridx,envarea)) fatal("aotmsa1/TA/env/fldupdat");
        if (recupdat(booldbcrec,boolridx)) fatal("aotmsa1/TA/env/fldupdat");
    }

    /* done
    */
    if (parmtellTA)
        fprintf(stderr,"aotmsa1: %"_LD_" recs in | %"_LD_" tabs | %d recs out\n",count1,count2,ntabs);

} else { /* else if (ntabs) */
    actionDA=0;
} /* end if (ntabs) */

} /* end actionTA */


/* =======================         =============          ================== */
/* =======================         =============          ================== */


if (actionDA) {

    /* List tableF/tableW
    */

#define MFQTAENV   102

#define FMTASIZE 32000

    FMT_CODE *tabfmtp=NULL;           /* formato para tab= */
    LONGX ntabfmts=0;
    char fmtspec[BUFSIZ];
    char *batchp;
    LONGX parmfrom,parmto;
    LONGX parmtell=/*0*/parmtellTA;
    LONGX mfn1;
    char *file1p;
    DBXSTRU *dbx1p;
    
    char fmtarea[FMTASIZE+1];
    LONGX fmtasize=FMTASIZE;

    char *vMFQTQRY0=NULL;
    char *vMFQTHITS=NULL;
/*nu     char *vMFQTAENV; */
    char *p;

    char vbreak[2][3];
    int nbreak;
    char v2992[MAXCATLEN+1];
    LONGX mfn351;

    LONGX setno=boolsetno;

    int ndownf=parmdown;
    int ndownw=parmdown;
    int ncolors=5;
    int kdownf=ndownf/ncolors;
    int kdownw=ndownw/ncolors;
    float rangef,stepf;
    float rangew,stepw;
    LONGX freq1,freq2,freq;
    float peso1,peso2,peso;
    int fcolor=0;
    char x999[20];
    int wcolor=0;
    char x2991[20];

    if (parmtrace) printf("+++trace: DA/namedbnp=%s\n",namedbnp);

    vtimid[ntimes]="fmt1"; time(&vtime[ntimes++]); /* watch */

    if (!booldbcrec) RECORD(booldbcrec=nrecs,namedbnp,0L);
    if (!boolridx) boolridx=nrecs;

    RECORD(boolridx,namedbnp,setno);

    vMFQTQRY0 = recfield(NULL,boolridx,MFQTQRY0,1,"");
    if (!boolqryp) boolqryp=vMFQTQRY0;

    vMFQTHITS = recfield(NULL,boolridx,MFQTHITS,1,"");

    /* get booldbn environment  */
/*nu     vMFQTAENV = recfield(NULL,boolridx,MFQTAENV,1,""); */
    /* p=vMFQTAENV; ... */

    /* set master file */
    if (parmsortx == 'f') file1p=dbnoutF; /* alternate file is dbnoutW */
    if (parmsortx == 'w') file1p=dbnoutW;
    if (parmsortx == 'd') file1p=dbnoutD; /* alternate file is dbnoutW */
    dbx1p=dbxstorp(file1p);

    /* init reading */
    if (!irec) irec=nrecs;
    RECORD(irec,file1p,0L);
    parmto=MF0nxtmfn-1; if (!ntabs) ntabs=parmto;
    count1=0;
    if (parmsortx == 'd') parmfrom=1;
    else {
        parmfrom=parmfromDA;
        if (parmcountDA) parmto=parmfromDA+parmcountDA-1;
        if (parmto > MF0nxtmfn-1) parmto=MF0nxtmfn-1;
        count2=parmto-parmfromDA+1;
    }

    /* environment */
    batchp=envarea;
    if (cipfnamp   ) { sprintf(batchp,"a3000|%s|" , cipfnamp    ); batchp+=strlen(batchp); }
    if (decsdbnp   ) { sprintf(batchp,"a3001|%s|" , decsdbnp    ); batchp+=strlen(batchp); }
    if (amdldbnp   ) { sprintf(batchp,"a3002|%s|" , amdldbnp    ); batchp+=strlen(batchp); }
    if (atmdldbnp  ) { sprintf(batchp,"a3003|%s|" , atmdldbnp   ); batchp+=strlen(batchp); }
    if (tmp1dbnp   ) { sprintf(batchp,"a3004|%s|" , tmp1dbnp    ); batchp+=strlen(batchp); }
    if (pftfnamp   ) { sprintf(batchp,"a3005|%s|" , pftfnamp    ); batchp+=strlen(batchp); }
    if (langp      ) { sprintf(batchp,"a3006|%s|" , langp       ); batchp+=strlen(batchp); }
    if (parmloopTA ) { sprintf(batchp,"a3007|%"_LD_"|", parmloopTA  ); batchp+=strlen(batchp); }
    if (parmfromDA ) { sprintf(batchp,"a3008|%"_LD_"|", parmfromDA  ); batchp+=strlen(batchp); }
    if (parmcountDA) { sprintf(batchp,"a3009|%"_LD_"|", parmcountDA ); batchp+=strlen(batchp); }
    if (parmsortp  ) { sprintf(batchp,"a3010|%s|" , parmsortp   ); batchp+=strlen(batchp); }
    if (boolqryp   ) { sprintf(batchp,"a3011|%s|" , boolqryp    ); batchp+=strlen(batchp); }
    if (namedbnp   ) { sprintf(batchp,"a3012|%s|" , namedbnp    ); batchp+=strlen(batchp); }
    if (parmatmdl99) { sprintf(batchp,"a3013|%d|" , parmatmdl99 ); batchp+=strlen(batchp); }
    if (actionp    ) { sprintf(batchp,"a3014|%s|" , actionp     ); batchp+=strlen(batchp); }
/**/
                       sprintf(batchp,"a3015|%"_LD_"|", setno       ); batchp+=strlen(batchp);
                       sprintf(batchp,"a4001|%d|" , ntabs       ); batchp+=strlen(batchp);
/**/


/* -----------------------------------------------------------------------------
v4001 = ntabs

v3210[1] = 'A'
v3211[1] = Aboolqryp
v3212 = hits
v3213[1] = Aboolqrypdecs

p/ v1001='1'
  v3214[] = time id
  v3215[] = seconds
  v3216   = elapsed "%02d:%02d:%02d",hours,mins,secs

decs data fields
  v3203 = v3
*/

    if (vMFQTQRY0) {
        int v351;
        RECSTRU *recp;
        sprintf(batchp,"a3210|%s|" , "A"             ); batchp+=strlen(batchp);
        sprintf(batchp,"a3211|%s|" , vMFQTQRY0       ); batchp+=strlen(batchp);
        sprintf(batchp,"a3212|%"_LD_"|", atol(vMFQTHITS) ); batchp+=strlen(batchp);
        sprintf(batchp,"a3213|"                      ); batchp+=strlen(batchp);
        for (p=vMFQTQRY0; *p; ) {
            for (v351=0; isdigit(*p); p++) v351=v351*10+((int)(*p)-(int)'0');
            if (v351) {
                if (!drec) drec=nrecs;
                RECORD(drec,decsdbnp,v351);
                recfield(batchp,drec,DECSTAGNAME(langp),1,"");    batchp+=strlen(batchp);
            }
            else { *batchp++ = *p; if (*p) p++; }
        }
        sprintf(batchp,"|"                           ); /*nu batchp+=strlen(batchp); */
    }
    /* end of envarea */

    /* init summary */
    memset(vbreak,0x00,sizeof(vbreak));

    /* do it */
    for (mfn1=parmfrom/*DA*/; mfn1 <= parmto; mfn1++) {

        /* compile strip CR if CRLF + display format */
        if (!tabfmtp) {
            /* use fmtarea for actual format specification */
            sprintf(fmtspec,"@%s",pftfnamp);
            if (!loadfile(NULL,'@',fmtspec,fmtarea,fmtasize,'\0')) {
                printf("Content-type: text/plain\n\n");
                printf("format error: file %s not found \n",pftfnamp);
                exit(12);
            }
            if (fmt_gener(&tabfmtp,fmtarea)) {
                printf("Content-type: text/plain\n\n");
                printf("format error: file %s error#%d offset %"_LD_" [%.512s%s]\n",
                        pftfnamp,fmt_error,fmt_errof,fmtarea+fmt_errof,"...");
                exit(12);
            }
        }

        /* get color limits */
        if (parmdown) if ((count1+1) == 1) {
            RECORD(irec,dbnoutF,1L);
            freq1=atol(recfield(x999,irec,999,1,""));
            RECORD(irec,dbnoutF,0L);
            freq2=freq1;
            if (MF0nxtmfn-1 > 1) {
                RECORD(irec,dbnoutF,MF0nxtmfn-1);
                freq2=atol(recfield(x999,irec,999,1,""));
            }
            rangef=freq1-freq2; if (rangef < 0) rangef = -rangef; /* ok */
            stepf=(rangef+1)/(float)ncolors;
            if (stepf <= 0) { stepf=1; kdownf=1; ndownf=0; }

            RECORD(irec,dbnoutW,1L);
            peso1=atof(recfield(x2991,irec,2991,1,""));
            RECORD(irec,dbnoutW,0L);
            peso2=peso1;
            if (MF0nxtmfn-1 > 1) {
                RECORD(irec,dbnoutW,MF0nxtmfn-1);
                peso2=atof(recfield(x2991,irec,2991,1,""));
            }
            rangew=peso1-peso2; if (rangew < 0) rangew = -rangew; /* ok */
            stepw=(rangew+1)/(float)ncolors;
            if (stepw <= 0) { stepw=1; kdownw=1; ndownw=0; }
        }

        /* get record */
        RECORD(irec,file1p,mfn1);
        if (RECrc != RCNORMAL) fatal("aotmsa1/DA/RCNORMAL");
        count1++;
        if (parmtrace) printf("+++ mfnDA=%"_LD_"\n",mfn1);

        /* process parmsort
        */
        batchp=fmtarea;
        if (file1p == dbnoutW /* parmsortx=='w' */) {
/*
            LONGX rankf=atol(recfield(fmtarea,irec,2220,1,""));
            LONGX rankd=atol(recfield(fmtarea,irec,2220,2,""));
            sprintf(batchp,"D2220A2220/%"_LD_"/A2220/%"_LD_"/A2220/%"_LD_"/",rankf,MFRmfn,rankd); batchp+=strlen(batchp);
*/
        }
        else {
/*
            LONGX rank1=atol(recfield(fmtarea,irec,2220,1,""));
*/
            LONGX rankw=atol(recfield(fmtarea,irec,2220,2,""));
            char *v1f=recfield(fmtarea,irec,1,1,"");
            char *v1w=fmtarea+strlen(fmtarea)+1;
            RECORD(irec,dbnoutW,rankw); RECdbxp=dbx1p;
            recfield(v1w,irec,1,1,"");
            if (!*v1f || !*v1w || strcmp(v1f,v1w)) fatal("aotmsa1/DA/join");
/*
            sprintf(batchp,"D2220A2220/%"_LD_"/A2220/%"_LD_"/",mfn1,MFRmfn); batchp+=strlen(batchp);
*/
        }

        /* process summary */
        memset(v2992,0x00,sizeof(v2992));
        nbreak=0;
        p=recfield(v2992,irec,2992,1,""); /* 1st mesh C/D */
        if (memcmp(vbreak[0],p+0,3)) {
            memmove(vbreak[0],p+0,3); nbreak+=1; *vbreak[1]='\0';
        }
        if (memcmp(vbreak[1],p+4,3)) {
            memmove(vbreak[1],p+4,3); nbreak+=2;
        }

        if (nbreak) {

            if (nbreak & 0x01) {
                sprintf(batchp,"A3994/%d/"  ,nbreak&0x01);  batchp+=strlen(batchp);
                sprintf(batchp,"A3995/%.3s/",v2992      );  batchp+=strlen(batchp);
                if (nbreak & 0x02) {
                sprintf(batchp,"A3995/%.7s/",v2992      );  batchp+=strlen(batchp);
                }
                nbreak=1;
            }
            else {
                nbreak=2;
                sprintf(batchp,"A3994/%d/",nbreak       ); batchp+=strlen(batchp);
                sprintf(batchp,"A3995//"                ); batchp+=strlen(batchp);
                sprintf(batchp,"A3995/%.7s/",v2992      ); batchp+=strlen(batchp);
            }
    
        } /* end if (nbreak) */

        /* skip records if parmshowzero 
        */
        if (parmshowzero)
            if (!nbreak)
                if (count1 > 1)
                    if (count1 < count2)
                        continue;

        /* add CGI and +hits data fields
        */
        sprintf(batchp,"A1001/%"_LD_"/A1002/%"_LD_"/",count1,count2); batchp+=strlen(batchp);

        /* add decs data fields
        */
        mfn351=atol(recfield(batchp,irec,1,1,"")); /* the condition */
        sprintf(batchp,"a3203|"                      ); batchp+=strlen(batchp);
        if (!drec) drec=nrecs;
        RECORD(drec,"mydecs",mfn351);
        recfield(batchp,drec,DECSTAGNAME(langp),1,"");            batchp+=strlen(batchp);
        sprintf(batchp,"|"                           ); batchp+=strlen(batchp);

        /* add watch */
        if (count1 == 1) {
            int hours,mins,secs;
            int i;
            vtimid[ntimes]="fmt2"; time(&vtime[ntimes++]); /* watch */
            secs=vtime[ntimes-1]-vtime[0];
            hours=secs/3600; secs-=hours*3600;
            mins=secs/60; secs-=mins*60;
            sprintf(batchp,"a3216|%02d:%02d:%02d|",hours,mins,secs); batchp+=strlen(batchp);
            for (i=0; i<ntimes-1; i++) {
                sprintf(batchp,"a3214|%s|",  vtimid[i]);             batchp+=strlen(batchp);
                sprintf(batchp,"a3215|%"_LD_"|", vtime[i+1]-vtime[i]);   batchp+=strlen(batchp);
            }
        }

        /* get color */
        if (parmdown) {
            freq=atol(recfield(x999,irec,999,1,""));
            fcolor=(int)(0xFF-ndownf+(freq/stepf)*kdownf);
            peso=atof(recfield(x2991,irec,2991,1,""));
            wcolor=(int)(0xFF-ndownw+(peso/stepw)*kdownw);
            sprintf(batchp,"a1515/%02x/",fcolor);   batchp+=strlen(batchp);
            sprintf(batchp,"a1515/%02x/",wcolor);   batchp+=strlen(batchp);
        }


        /* add/del fields */
        if (fldupdat(irec,fmtarea)) fatal("aotmsa1/DA/fldupdat/fmt");
        if (fldupdat(irec,envarea)) fatal("aotmsa1/DA/fldupdat/env");

        /* apply display format */
        if (fmt_inter(tabfmtp,irec,fmtasize,fmtarea,fmtasize) < 0) {
            printf("Content-type: text/plain\n\n");
            printf("error: %s [format execution error %d]\n",pftfnamp,fmt_error);
            exit(21);
        }
        /* close fmt_ every each MAXTABFMTS calls */
        if (++ntabfmts == 1000 /*MAXTABFMTS*/) {
            fmt_free(tabfmtp); tabfmtp=(FMT_CODE *)NULL;
            ntabfmts=0;
        }

        /* output format results
        */
        puts(fmtarea); /* for (left=strlen(fmtarea), p=fmtarea; left; ) .s{ int len=BUFSIZ/2; char c;
                            if (len > left) len=left; c=p[len]; p[len]='\0'; puts(p); p[len]=c;
                            p+=len; left-=len; } */

        if (parmtell) if ((count1 % parmtell) == 0)
            fprintf(stderr,"+++ %"_LD_"/%"_LD_" \n",count1,count2);
        if (parmtrace)
            fprintf(stderr,"+++ %"_LD_"/%"_LD_" \n",count1,count2);

    }

    vtimid[ntimes]="end2"; time(&vtime[ntimes++]); /* watch */

} /* actionDA */


/* =======================         =============          ================== */
/* =======================         =============          ================== */
if (actionDD) {

    int errno1=0;
    int errno2=0;
    char *p1=NULL;
    char *p2=NULL;
    LONGX decsnxtmfn;
    char *decsdbp=decsdbnp;

    /* Search DECS
    */
    boolsetno=0;
    boolhits=0;

    /* init reading */
    if (!drec) drec=nrecs;
    RECORD(drec,decsdbnp,0L);
    decsnxtmfn=MF0nxtmfn;
    
    /* will call format even on error and/or expression missing */
    count2=1;

    /* display MeSH Diseases (cat C$) 
    */
    if (actionDD == 2) if (!boolqryp) {
        boolqryp="CAT1=C$";
        decsdbp="mydecs";
    }
    
    /*
        Search "bool=" parameter
    */
    if (boolqryp) { /* boolqryp */

        LONGX mfn;

        LONGX asize=strlen(decsdbp) +                  /* field MFQTDBN0=11 (may be longer than 128+4+1) */
                   strlen(boolqryp) +                 /* the expression */
                   2*strlen(boolqryp) +               /* the terms + pseudo operators _1termX+11X */
                   BUFSIZ+strlen(amdldbnp) +          /* field MFQSWDBN=20 + a diferent db */
                   24*(strlen(B7DELADD)+3*B7UPLEN) +  /* 2 duzias de control fields */  /* DxxAxx|xxx| */ 
                   strlen(boolqryp)+4 +               /* tmpfile .%03x */
                   BUFSIZ;                            /* por si las moscas */
                   
        char *buffup=loadfile(NULL,'@',"",NULL,asize,'\0');

        if (!cib7p) cib7p=b7_cib7(cib7p,ciapip);

        /* initialize ridx and b7_xxx() parameters and run b7_run()
        */
        for (boolridx=nrecs, mfn=1; ; mfn++) { /* get actual maxmfn */
            RECORD(boolridx,decsdbp,mfn);
            if (RECrc == RCEOF) fatal(decsdbp);
            if (RECrc == RCNORMAL) break;
        }
        RECdbxp=NULL;                /* nao usa base de log */
        MFRmfn=0L;

        /* run b7_exp() / b7_run() / b7_hit(0)
        */
        p1=b7_exp(cib7p,boolridx,decsdbp,boolqryp,buffup,NULL,0L,&errno1);
        if (p1 || errno1) ; /* printf("Expression syntax error %d: '%s'\n",errno1,p1); */
        else {
            LONGX bufsiz;
            char *hitfnamp="null";
            if (booltmpnam[0]) hitfnamp=booltmpnam;
            sprintf(buffup,"D%dA%d|%s|",MFQTHMEM,MFQTHMEM,hitfnamp);
            if (fldupdat(boolridx,buffup) != NULL) fatal("aotmsa1/DD/fldupdat/boolridx");

            /* b7_run faz "trirecp=vrep[tlirec]" */
            if (!booltlir) record(booltlir=nrecs,"null",1L);

            /* alloc buffer for 20000 hits = 80Kbytes */
            bufsiz=20000*sizeof(LONGX);
            if (b7bufferp) b7_hbfree(cib7p,&b7bufferp);
            b7_hballoc(cib7p,bufsiz,&b7bufferp);

            /* run search */
            p2=b7_run(cib7p,boolridx,decsdbp,buffup,sizeof(buffup),
                booltell,isisuctab,""/*btch0p*/,booltlir,0L,&errno2);
            if (p2 || errno2) ; /* printf("Execution error %d: '%s'\n",errno2,p2); */
            else {
                boolsetno=b7setno;  /* =1 ??? */
                boolhits=b7bufferp->ndocs; /* b7_hit(cib7p,boolridx,0L,b7setno); */
                if (booltell) printf("Hits=%"_LD_"\n",boolhits);
            }
        }
        if (p2 || errno2) fatal("Expression execution error");

    } /* boolqryp */

    /* set number of times format is called */
    if (boolhits) count2=boolhits;

    if (p1 || errno1 /* || p2 || errno2*/) { ; }

    if (1) { /* errno */
//#define FMTASIZE 32000
        FMT_CODE *tabfmtp=NULL;           /* formato para tab= */
        char fmtspec[BUFSIZ];
        char *batchp;
        char fmtarea[FMTASIZE+1];
        LONGX fmtasize=FMTASIZE;

        count1=0;

        /* environment */
        batchp=envarea;
        if (cipfnamp   ) { sprintf(batchp,"a3000|%s|" , cipfnamp    ); batchp+=strlen(batchp); }
        if (decsdbnp   ) { sprintf(batchp,"a3001|%s|" , decsdbnp    ); batchp+=strlen(batchp); }
        if (amdldbnp   ) { sprintf(batchp,"a3002|%s|" , amdldbnp    ); batchp+=strlen(batchp); }
        if (atmdldbnp  ) { sprintf(batchp,"a3003|%s|" , atmdldbnp   ); batchp+=strlen(batchp); }
        if (tmp1dbnp   ) { sprintf(batchp,"a3004|%s|" , tmp1dbnp    ); batchp+=strlen(batchp); }
        if (pftfnamp   ) { sprintf(batchp,"a3005|%s|" , pftfnamp    ); batchp+=strlen(batchp); }
        if (langp      ) { sprintf(batchp,"a3006|%s|" , langp       ); batchp+=strlen(batchp); }
        if (parmloopTA ) { sprintf(batchp,"a3007|%"_LD_"|", parmloopTA  ); batchp+=strlen(batchp); }
        if (parmfromDA ) { sprintf(batchp,"a3008|%"_LD_"|", parmfromDA  ); batchp+=strlen(batchp); }
        if (parmcountDA) { sprintf(batchp,"a3009|%"_LD_"|", parmcountDA ); batchp+=strlen(batchp); }
        if (parmsortp  ) { sprintf(batchp,"a3010|%s|" , parmsortp   ); batchp+=strlen(batchp); }
//        if (boolqryp   ) { sprintf(batchp,"a3011|%s|" , boolqryp    ); batchp+=strlen(batchp); }
        if (namedbnp   ) { sprintf(batchp,"a3012|%s|" , namedbnp    ); batchp+=strlen(batchp); }
        if (parmatmdl99) { sprintf(batchp,"a3013|%d|" , parmatmdl99 ); batchp+=strlen(batchp); }
        if (actionp    ) { sprintf(batchp,"a3014|%s|" , actionp     ); batchp+=strlen(batchp); }
/**/
//                           sprintf(batchp,"a3015|%"_LD_"|", setno       ); batchp+=strlen(batchp);
//                           sprintf(batchp,"a4001|%d|" , ntabs       ); batchp+=strlen(batchp);
/**/
        /* end of envarea */

        /* compile strip CR if CRLF + display format */
        pftfnamp="aotmsa1D.pft";
        sprintf(fmtspec,"%s,@%s","if size(s(#))=2 then newline(s(#)*1) fi",pftfnamp);
        if (fmt_gener(&tabfmtp,fmtspec)) {
            printf("Content-type: text/plain\n\n");
            printf("error: %s [format error %d:%s]\n",pftfnamp,fmt_error,fmtspec+fmt_errof);
            exit(12);
        }


        /*
            loop decs
        */

        for (boolh=1; ; boolh++) {

            LONGX mfn;

            /* hit processing */
            if (boolhits) {

                if (boolh > boolhits) break;
                
                if (boolh == 1) {
                    hitlistp=(B7HITLIST *)b7bufferp->area;
                    mfnlistp=hitlistp->mfnlist;
                }
                if (b7bufferp->hcase == BITSTRING)
                    mfn=b7_hit(cib7p,boolridx,boolh,b7setno);
                else
                    mfn = *mfnlistp; mfnlistp++;

                if (mfn <= 0) fatal("aotmsa1/DD/decsdbnp/hit");
    
                if (parmtrace) printf("+++ hit#%"_LD_" mfn=%"_LD_"\n",boolh,mfn);
    
                RECORD(drec,decsdbnp,mfn);
                if (RECrc != RCNORMAL) fatal("aotmsa1/DD/decsdbnp/RCNORMAL");

            }
            else {
                RECORD(drec,decsdbnp,mfn=decsnxtmfn);
                RECrc=RCNORMAL; MFRstatus=ACTIVE;
//                sprintf(fmtarea,"a%"_LD_"|x|",(LONGX)DECSTAGP);
//                if (fldupdat(drec,fmtarea)) fatal("aotmsa1/DD/fldupdat/x");
            }

            count1++;
            if (parmtrace) printf("+++ mfnDD=%"_LD_"\n",mfn);
    
            /* process record
            */
            batchp=fmtarea;
    
            /* add CGI and +hits data fields
            */
            sprintf(batchp,"A1001/%"_LD_"/A1002/%"_LD_"/",count1,count2); batchp+=strlen(batchp);
    
            /* add decs data fields
            */
            sprintf(batchp,"a3203|"                      ); batchp+=strlen(batchp);
            recfield(batchp,drec,DECSTAGNAME(langp),1,"");            batchp+=strlen(batchp);
            sprintf(batchp,"|"                           ); batchp+=strlen(batchp);

            if (fldupdat(drec,fmtarea)) fatal("aotmsa1/DD/fldupdat/fmt");
            if (fldupdat(drec,envarea)) fatal("aotmsa1/DD/fldupdat/env");

            if (fmt_inter(tabfmtp,drec,fmtasize,fmtarea,fmtasize) < 0) {
                printf("Content-type: text/plain\n\n");
                printf("error: %s [format execution error %d]\n",pftfnamp,fmt_error);
                exit(21);
            }

            /* output format results
            */
            puts(fmtarea);
            
            if (!boolhits) break;

        } /* end for (boolh=1; ;  ) */

    } /* errno */

} /* end actionDD */

/* =======================         =============          ================== */
/* =======================         =============          ================== */

    /* watch */
    time(&vtime[ntimes++]);
    if (parmtime) {
        struct tm *tp;
        char fmt_time[21+1]; /* aaaammdd hhmmss WDAY YDAY */
                             /* 1234567890123456 7  8 901 */
        int hours,mins,secs;
        LONGX time_start=vtime[0];
        LONGX time_elapsed;
        vtimid[ntimes]="exit"; time(&vtime[ntimes++]); /* watch */
        time_elapsed=vtime[ntimes-1];
        tp=localtime(&time_elapsed);
        sprintf(fmt_time,"%02d/%02d/%02d %02d:%02d:%02d",
                           tp->tm_mday,tp->tm_mon+1,tp->tm_year%100,
                           tp->tm_hour,tp->tm_min,tp->tm_sec);
        fprintf(stderr,"+++ localtime %s \n",fmt_time);
        secs=time_elapsed - time_start;
        hours=secs/3600; secs-=hours*3600;
        mins=secs/60; secs-=mins*60;
        sprintf(fmt_time,"%02d:%02d:%02d",hours,mins,secs);
        fprintf(stderr,"+++ elapsed %s \n",fmt_time);
    }

    /* exit
    */
    if (cib7p) cib7p=b7_cib7_delete(cib7p);
#if CIAPI
    cisisApplicationDelete(ciapip);
    ciapip=NULL;
#endif /* CIAPI */

    exit(0);
}


/* =======================         =============          ================== */
/* =======================         =============          ================== */

#if ANSI
int rutfreqF(LONGX *tabcntA, int ntabs, LONGX drec, LONGX *tabcntArankF, TABFREQF_ENTRY **tabfreqFp)
#else
int rutfreqF(tabcntA, ntabs, dbnoutp, drec, tabcntArankF, tabfreqFp)
LONGX *tabcntA;
int ntabs;
LONGX drec;
LONGX *tabcntArankF;
TABFREQF_ENTRY **tabfreqFp;
#endif
{
    LONGX mfn;
    char buffup[BUFSIZ];
    int v351;

    LONGX nbytes;
    char *p;

    TABFREQF_ENTRY *tabfreqF,*tabF;

typedef struct outfreqF_sort_entry {
    char x999[10];
    char categ[39];
    char null[1];
    char align[2];
    LONGX v351;
    LONGX freq;
} OUTFREQF_SORT_ENTRY;

    OUTFREQF_SORT_ENTRY *sort_tab,*sort_tmp,*sortp,*ip,*jp;
    int catlen;
    int i,j,k;

    int keylen=sizeof(sortp->x999)+sizeof(sortp->categ);


    /* init tabcntArankF (indexed by DecsMfn)
    */
    memset(tabcntArankF,0,sizeof(LONGX)*CLASSES);

    *tabfreqFp=NULL;
    if (!ntabs) return(0);

    /* alloc tabfreqF (indexed by rankF)
    */
    nbytes=sizeof(TABFREQF_ENTRY)*ntabs;
    if (nbytes >= (LONGX)ALLOMAXV) fatal("aotmsa1/rutfreqF/tabfreqF/ALLOMAXV");
    p=(char *)ALLOC((ALLOPARM)(nbytes));
    if (p == ALLONULL) fatal("aotmsa1/rutfreqF/tabfreqF/ALLOC");
    memset(p,0,nbytes); /* init */
    tabfreqF=(TABFREQF_ENTRY *)p;
    *tabfreqFp=tabfreqF;

    nbytes=sizeof(OUTFREQF_SORT_ENTRY)*ntabs+sizeof(OUTFREQF_SORT_ENTRY);
    if (nbytes >= (LONGX)ALLOMAXV) fatal("aotmsa1/rutfreqF/sort_tab/ALLOMAXV");
    p=(char *)ALLOC((ALLOPARM)(nbytes));
    if (p == ALLONULL) fatal("aotmsa1/rutfreqF/sort_tab/ALLOC");
    sort_tab=(OUTFREQF_SORT_ENTRY *)p;
    sort_tmp=sort_tab+ntabs;

    for (mfn=0L, v351=1; mfn<ntabs; v351++) {
        if (!tabcntA[v351]) continue;
        sortp=sort_tab+mfn;
        sprintf(sortp->x999,"%10"_LD_,999999999L-tabcntA[v351]);
        if (drec) { /* use decs/v20 as 2nd sort key for sortF */
            record(drec,ddecsdbnp,(LONGX)v351);
            recfield(buffup,drec,TAG020,1,""); if (!*buffup) fatal("aotmsa1/rutfreqF/decs/TAG020");
            catlen=strlen(buffup); if (catlen>sizeof(sortp->categ)) buffup[catlen=sizeof(sortp->categ)]='\0';
            memset(buffup+catlen,(int)' ',sizeof(sortp->categ)-catlen);
            memmove(sortp->categ,buffup,sizeof(sortp->categ));
        } else memset(sortp->categ,(int)' ',sizeof(sortp->categ));
        sortp->null[0]='\0';
        sortp->v351=v351;
        sortp->freq=tabcntA[v351];
        mfn++;
    }

    /* sort F
    */
    for (k=ntabs-1, i=0; i<k; i++) {
        ip=sort_tab+i;
        for (j=i+1; j<ntabs; j++) {
            jp=sort_tab+j;
            if (memcmp((void *)ip,(void *)jp,keylen) > 0) {
                memmove((void *)sort_tmp,(void *)ip,      sizeof(OUTFREQF_SORT_ENTRY));
                memmove((void *)ip      ,(void *)jp,      sizeof(OUTFREQF_SORT_ENTRY));
                memmove((void *)jp      ,(void *)sort_tmp,sizeof(OUTFREQF_SORT_ENTRY));
            }
        }
    }

    /* store tabcntArankF (indexed by DecsMfn) and tabfreqF (indexed by rankF)
    */
    for (sortp=sort_tab, tabF=tabfreqF, mfn=1L; mfn<=ntabs; mfn++, sortp++, tabF++) {
        tabF->v351=sortp->v351;
        tabF->freq=sortp->freq;
        tabcntArankF[sortp->v351]=mfn;
    }

    return(0);
}


#if ANSI
int outfreqF(int ntabs, char *dbnoutp, LONGX *tabcntArankW, LONGX *tabcntArankD, TABFREQF_ENTRY *tabfreqF)
#else
int outfreqF(ntabs, dbnoutp, tabcntArankW, tabcntArankD, tabfreqF)
int ntabs;
char *dbnoutp;
LONGX *tabcntArankW;
LONGX *tabcntArankD;
TABFREQF_ENTRY *tabfreqF;
#endif
{
    RECSTRU *recp;
    LONGX mfn;
    char buffup[BUFSIZ],*batchp;
    LONGX upirec;
    LONGX upcrec;
    TABFREQF_ENTRY *tabF;

    if (!ntabs) return(0);

    /* output tabfreqF (indexed by rankF) and tabcntArankW (indexed by DecsMfn)
    */
    record(upcrec=nrecs,dbnoutp,0L);
    record(upirec=nrecs,dbnoutp,1L);
    for (tabF=tabfreqF, mfn=1L; mfn<=ntabs; mfn++, tabF++) {
        batchp=buffup;
        sprintf(batchp,"A1/%06"_LD_"/A999/%"_LD_"/",tabF->v351,tabF->freq); batchp+=strlen(batchp);
        sprintf(batchp,"A2220/%"_LD_"/"    ,mfn                     ); batchp+=strlen(batchp);
        sprintf(batchp,"A2220/%"_LD_"/"    ,tabcntArankW[tabF->v351]); batchp+=strlen(batchp);
        sprintf(batchp,"A2220/%"_LD_"/"    ,tabcntArankD[tabF->v351]); batchp+=strlen(batchp);
        *batchp='\0';
        RECORD(upirec,dbnoutp,mfn); MFRstatus=ACTIVE;
        if (fldupdat(upirec,buffup)) return(mfn);
        recupdat(upcrec,upirec);
    }

    return(0);
}



#if ANSI
int rutfreqW(TABFREQF_ENTRY *tabfreqF, int ntabs, char *amdlname, LONGX docsTOTxA, LONGX docsTOTamdl, LONGX *tabcntArankF, LONGX *tabcntArankW, TABFREQW_ENTRY **tabfreqWp, LONGX *tabcntMDL, LONGX *gtabcntA)
#else
int rutfreqW(tabfreqF, ntabs, amdlname, docsTOTxA, docsTOTamdl, tabcntArankF, tabcntArankW, tabfreqWp, tabcntMDL, gtabcntA)
TABFREQF_ENTRY *tabfreqF;
int ntabs;
char *amdlname;
LONGX docsTOTxA;    /* v9991[1] = docs de xA - nao vazios */
LONGX docsTOTamdl;  /* v9991[2] = docs de amdl */
LONGX *tabcntArankF;
LONGX *tabcntArankW;
TABFREQW_ENTRY **tabfreqWp;
LONGX *tabcntMDL;
LONGX *gtabcntA;
#endif
{
    LONGX mfn;
    int v351;

    LONGX nbytes;
    char *p;

    TABFREQW_ENTRY *tabfreqW,*tabW;
    TABFREQF_ENTRY *tabF;

typedef struct outfreqW_sort_entry {
    char x2991[10];
    char xrankF[6];
    char null[1];
    char align[3];
    LONGX v351;
    LONGX docsTTxA;         /*  v999[1] = docs de TT em xA */
    LONGX docsTTamdl;       /*  v999[2] = docs de TT em amdl = postings de TT em amdl */
    float frelTTxA;        /* v1991[1] = frelTTxA = docs de TT em xA / docs de xA */
    float frelTTamdl;      /* v1991[2] = frelTTamdl = docs de TT em amdl / docs de amdl */
    float pesoTTxA;        /* v2991[1] = frelTTxA / frelTTamdl */
//    char pesoTTxAout[4]; /* v2221[1] = 'out' se peso < 1 */
} OUTFREQW_SORT_ENTRY;

    OUTFREQW_SORT_ENTRY *sort_tab,*sort_tmp,*sortp,*ip,*jp;
    int i,j,k;

    TRMSTRU *trmp;
    LONGX itrm;
    char x351[6+1];

    int keylen=sizeof(sortp->x2991)+sizeof(sortp->xrankF);

#if XPROCIN
proc= /* v999[1] = docs de TT em xA */
proc='a999|',f(npost(['c:\2001x300\usersaot\amdl351']v1),1,0),'|'/* docs de TT em amdl = postings de TT em amdl */
proc='a9991|',ref([getenv('T999999')]1,v999),'|',   /* docs de xA - nao vazios */
proc='a9991|',ref(['c:\2001x300\usersaot\atmdl']l(['c:\2001x300\usersaot\atmdl']'999999'),v999),'|'  /* docs de amdl */
proc='a1991|',if val(v9991[1])> 0 then f(val(v999[1])/val(v9991[1]),10,8) else f(0,10,8) fi,'|' /* frelTTxA = docs de TT em xA / docs de xA */
proc='a1991|',if val(v9991[2])> 0 then f(val(v999[2])/val(v9991[2]),10,8) else f(0,10,8) fi,'|' /* frelTTamdl = docs de TT em amdl / docs de amdl */
proc='a2991|',if val(v1991[2])> 0 then f(val(v1991[1])/val(v1991[2]),5,3) else f(0,5,3) fi,'|'  /* frelTTxA / frelTTamdl */
proc=/* if v1991[1]<v1991[2] then 'a2221|out|' fi */
#endif

    /* init tabcntArankW (indexed by DecsMfn)
    */
    memset(tabcntArankW,0,sizeof(LONGX)*CLASSES);

    *tabfreqWp=NULL;
    if (!ntabs) return(0);

    /* alloc tabfreqW (indexed by rankW)
    */
    nbytes=sizeof(TABFREQW_ENTRY)*ntabs;
    if (nbytes >= (LONGX)ALLOMAXV) fatal("aotmsa1/rutfreqW/tabfreqW/ALLOMAXV");
    p=(char *)ALLOC((ALLOPARM)(nbytes));
    if (p == ALLONULL) fatal("aotmsa1/rutfreqW/tabfreqW/ALLOC");
    memset(p,0,nbytes); /* init */
    tabfreqW=(TABFREQW_ENTRY *)p;
    *tabfreqWp=tabfreqW;

    nbytes=sizeof(OUTFREQW_SORT_ENTRY)*ntabs+sizeof(OUTFREQW_SORT_ENTRY);
    if (nbytes >= (LONGX)ALLOMAXV) fatal("aotmsa1/rutfreqW/sort_tab/ALLOMAXV");
    p=(char *)ALLOC((ALLOPARM)(nbytes));
    if (p == ALLONULL) fatal("aotmsa1/rutfreqW/sort_tab/ALLOC");
    sort_tab=(OUTFREQW_SORT_ENTRY *)p;
    sort_tmp=sort_tab+ntabs;


    if (!tabcntMDL) TRMALLOC((itrm=1L),0L); /* do not load .iyp */

    for (sortp=sort_tab, tabF=tabfreqF, mfn=1L; mfn<=ntabs; mfn++, tabF++, sortp++) {
        v351=tabF->v351;
        /* */
        sortp->docsTTxA   = tabF->freq;     /* v999[1] = docs de TT em xA */
        if (tabcntMDL)
            sortp->docsTTamdl = tabcntMDL[v351];   /*  v999[2] = docs de TT em amdl = postings de TT em amdl */
        else {
            sprintf(x351,"%06"_LD_,v351);
            TERM(itrm,amdlname,x351); if (TRMrc != RCNORMAL) fatal("aotmsa1/rutfreqW/TERM/amdl");
            sortp->docsTTamdl = TRMxytotp;         /*  v999[2] = docs de TT em amdl = postings de TT em amdl */
        }
        sortp->frelTTxA   = (docsTOTxA   > 0) ? sortp->docsTTxA   / (float)docsTOTxA   : 0;   /* v1991[1] = frelTTxA = docs de TT em xA / docs de xA */
        sortp->frelTTamdl = (docsTOTamdl > 0) ? sortp->docsTTamdl / (float)docsTOTamdl : 0;   /* v1991[2] = frelTTamdl = docs de TT em amdl / docs de amdl */
        sortp->pesoTTxA   = (sortp->frelTTamdl > 0)  ? sortp->frelTTxA   / sortp->frelTTamdl  : 0;   /* v2991[1] = frelTTxA / frelTTamdl */
        //memmove(sortp->pesoTTxAout,(frelTTxA < frelTTamdl)?"out":"  ",sizeof(sortp->pesoTTxA)); /* if v1991[1]<v1991[2] then 'a2221|out|' fi */
        /* */
        sprintf(sortp->x2991,"%10.3f",999999.999-sortp->pesoTTxA);
        sprintf(sortp->xrankF,"%6"_LD_,tabcntArankF[v351]); /* key */
        sortp->null[0]='\0';
        memset(sortp->align,(int)' ',sizeof(sortp->align));
        sortp->v351=v351;
    }

    /* sort W
    */
    for (k=ntabs-1, i=0; i<k; i++) {
        ip=sort_tab+i;
        for (j=i+1; j<ntabs; j++) {
            jp=sort_tab+j;
            if (memcmp((void *)ip,(void *)jp,keylen) > 0) {
                memmove((void *)sort_tmp,(void *)ip,      sizeof(OUTFREQW_SORT_ENTRY));
                memmove((void *)ip      ,(void *)jp,      sizeof(OUTFREQW_SORT_ENTRY));
                memmove((void *)jp      ,(void *)sort_tmp,sizeof(OUTFREQW_SORT_ENTRY));
            }
        }
    }


    /* store tabcntArankW (indexed by DecsMfn) and tabfreqW (indexed by rankW)
    */
    for (sortp=sort_tab, tabW=tabfreqW, mfn=1L; mfn<=ntabs; mfn++, sortp++, tabW++) {
        tabW->v351=sortp->v351;
        tabW->docsTTxA=sortp->docsTTxA;       /*  v999[1] = docs de TT em xA */
        tabW->docsTTamdl=sortp->docsTTamdl;   /*  v999[2] = docs de TT em amdl = postings de TT em amdl */
        tabW->frelTTxA=sortp->frelTTxA;       /* v1991[1] = frelTTxA = docs de TT em xA / docs de xA */
        tabW->frelTTamdl=sortp->frelTTamdl;   /* v1991[2] = frelTTamdl = docs de TT em amdl / docs de amdl */
        tabW->peso=sortp->pesoTTxA;           /* v2991    = frelTTxA / frelTTamdl */
//        strcpy(tabW->pesoTTxAout,(sortp->frelTTxA < sortp->frelTTamdl)?"out":""); /* if v1991[1]<v1991[2] then 'a2221|out|' fi */
        tabcntArankW[sortp->v351]=mfn;
    }


    return(0);
}


#if ANSI
int outfreqW(int ntabs, char *dbnoutp, LONGX docsTOTxA, LONGX docsTOTamdl, LONGX *tabcntArankF, LONGX *tabcntArankD, TABFREQD_ENTRY *tabfreqD, TABFREQW_ENTRY *tabfreqW)
#else
int outfreqW(ntabs, dbnoutp, docsTOTxA, docsTOTamdl, tabcntArankF, tabcntArankD, tabfreqD, tabfreqW)
int ntabs;
char *dbnoutp;
LONGX docsTOTxA;    /* v9991[1] = docs de xA - nao vazios */
LONGX docsTOTamdl;  /* v9991[2] = docs de amdl */
LONGX *tabcntArankF;
LONGX *tabcntArankD;
TABFREQD_ENTRY *tabfreqD;
TABFREQW_ENTRY *tabfreqW;
#endif
{
    RECSTRU *recp;
    LONGX mfn;
    char buffup[BUFSIZ],*batchp;

    LONGX upirec;
    LONGX upcrec;

    TABFREQD_ENTRY *tabD;
    LONGX xrankD;

    TABFREQW_ENTRY *tabW;

    if (!ntabs) return(0);

    /* output
    */
    record(upcrec=nrecs,dbnoutp,0L);
    record(upirec=nrecs,dbnoutp,1L);
    for (tabW=tabfreqW, mfn=1L; mfn<=ntabs; mfn++, tabW++) {
        batchp=buffup;

        sprintf(batchp,"A1/%06"_LD_"/"     ,tabW->v351              ); batchp+=strlen(batchp);
        sprintf(batchp,"A999/%"_LD_"/"     ,tabW->docsTTxA          ); batchp+=strlen(batchp);
        sprintf(batchp,"A999/%"_LD_"/"     ,tabW->docsTTamdl        ); batchp+=strlen(batchp);

        sprintf(batchp,"A9991/%"_LD_"/"    ,      docsTOTxA         ); batchp+=strlen(batchp);
        sprintf(batchp,"A9991/%"_LD_"/"    ,      docsTOTamdl       ); batchp+=strlen(batchp);

        sprintf(batchp,"A1991/%10.8f/" ,tabW->frelTTxA          ); batchp+=strlen(batchp);
        sprintf(batchp,"A1991/%10.8f/" ,tabW->frelTTamdl        ); batchp+=strlen(batchp);
        sprintf(batchp,"A2991/%5.3f/"  ,tabW->peso              ); batchp+=strlen(batchp);

//        if (tabW->pesoTTxAout[0])
//      sprintf(batchp,"A2221/%s/"     ,tabW->pesoTTxAout       ); batchp+=strlen(batchp);

        sprintf(batchp,"A2220/%"_LD_"/"    ,tabcntArankF[tabW->v351]); batchp+=strlen(batchp);
        sprintf(batchp,"A2220/%"_LD_"/"    ,mfn                     ); batchp+=strlen(batchp);
        sprintf(batchp,"A2220/%"_LD_"/"    ,tabcntArankD[tabW->v351]); batchp+=strlen(batchp);

        xrankD=tabcntArankD[tabW->v351];
        tabD=tabfreqD; tabD+=(xrankD-1);
        sprintf(batchp,"A2992/%s/"     ,tabD->delca             ); batchp+=strlen(batchp);
        sprintf(batchp,"A2993/%"_LD_"/"    ,gtabcntA[tabD->gmfn]    ); batchp+=strlen(batchp);
        sprintf(batchp,"A2994/%06"_LD_"/"  ,tabD->gmfn              ); batchp+=strlen(batchp);

        *batchp='\0';
        RECORD(upirec,dbnoutp,mfn); MFRstatus=ACTIVE;
        if (fldupdat(upirec,buffup)) return(mfn);
        recupdat(upcrec,upirec);
    }

    return(0);
}



#if ANSI
int rutfreqD(TABFREQF_ENTRY *tabfreqF, int ntabs, LONGX *tabcntArankF, LONGX *tabcntArankW, LONGX *tabcntArankD, TABFREQD_ENTRY **tabfreqDp, LONGX *tabcntGMDL, LONGX *gtabcntA)
#else
int rutfreqD(tabfreqF, ntabs,tabcntArankF, tabcntArankW, tabcntArankD, tabfreqDp, gtabcntA)
TABFREQF_ENTRY *tabfreqF;
int ntabs;
LONGX *tabcntArankF;
LONGX *tabcntArankW;
LONGX *tabcntArankD;
TABFREQD_ENTRY **tabfreqDp;
LONGX *tabcntGMDL;
LONGX *gtabcntA;
#endif
{
    LONGX mfn;
    int v351;

    LONGX nbytes;
    char *p;

    TABFREQD_ENTRY *tabfreqD,*tabD;
    TABFREQF_ENTRY *tabF;

typedef struct outfreqD_sort_entry {
//    char x2992[10];
    char x2992_cat[1]; /* memmove(,,1) */
    char x2993[10];
    char x2992[MAXCATLEN+1];
    char xrankF[6];
    LONGX v351;
    LONGX gmfn;
//    LONGX delta;
} OUTFREQD_SORT_ENTRY;

    OUTFREQD_SORT_ENTRY *sort_tab,*sort_tmp,*sortp,*ip,*jp;
    int i,j,k;

    int keylen=sizeof(sortp->x2992_cat)+sizeof(sortp->x2993)+sizeof(sortp->x2992)+sizeof(sortp->xrankF);


    /* init tabcntArankD (indexed by DecsMfn)
    */
    memset(tabcntArankD,0,sizeof(LONGX)*CLASSES);

    *tabfreqDp=NULL;
    if (!ntabs) return(0);

    /* alloc tabfreqD (indexed by rankD)
    */
    nbytes=sizeof(TABFREQD_ENTRY)*ntabs;
    if (nbytes >= (LONGX)ALLOMAXV) fatal("aotmsa1/rutfreqD/tabfreqD/ALLOMAXV");
    p=(char *)ALLOC((ALLOPARM)(nbytes));
    if (p == ALLONULL) fatal("aotmsa1/rutfreqD/tabfreqD/ALLOC");
    memset(p,0,nbytes); /* init */
    tabfreqD=(TABFREQD_ENTRY *)p;
    *tabfreqDp=tabfreqD;

    nbytes=sizeof(OUTFREQD_SORT_ENTRY)*ntabs+sizeof(OUTFREQD_SORT_ENTRY);
    if (nbytes >= (LONGX)ALLOMAXV) fatal("aotmsa1/rutfreqD/sort_tab/ALLOMAXV");
    p=(char *)ALLOC((ALLOPARM)(nbytes));
    if (p == ALLONULL) fatal("aotmsa1/rutfreqD/sort_tab/ALLOC");
    sort_tab=(OUTFREQD_SORT_ENTRY *)p;
    sort_tmp=sort_tab+ntabs;

    for (sortp=sort_tab, tabF=tabfreqF, mfn=1L; mfn<=ntabs; mfn++, tabF++, sortp++) {
        v351=tabF->v351;
////        sortp->delta=tabcntArankW[v351]-tabcntArankF[v351];
////        sprintf(sortp->x2992,"%10"_LD_,555555555-sortp->delta);
//        sortp->delta=tabcntGMDL[v351];
//        sprintf(sortp->x2992,"%10"_LD_,999999999-sortp->delta);


#if 0
            (if v20.1='C' or v20.1='D'
                then v20,break
                else if iocc=1 then v20'<br>' fi
             fi
            )
#endif
          if (1) {
              DIRSTRU *dirp;
              int len,xdir;
              RECSTRU *recp;
        
              RECORD(ddrec,"mydecs",v351); //        recfield(sortp->x2992,ddrec,20,1,"");
        
              for (dirp=MFRdir, xdir=0; xdir < MFRnvf; dirp++, xdir++)
                if (dirp->tag == TAG005) {
                    int loop;
                    LONGX mfn=0;
                    p=FIELDP(xdir);
                    for (loop=dirp->len; loop--; p++) mfn=mfn*10+((int)(*p)-(int)'0');
                    if (mfn > CLASSES) fatal("aotmsa1/rutfreqD/TAG005/CLASSES");
                    sprintf(sortp->x2993,"%10"_LD_,999999999L-gtabcntA[mfn]);
                    sortp->gmfn=mfn;
                    break;
                }
              for (dirp=MFRdir, xdir=0; xdir < MFRnvf; dirp++, xdir++)
                if (dirp->tag == TAG022) {
                    p=FIELDP(xdir);
                    if (*p == 'C' || *p == 'D') ; else fatal("aotmsa1/rutfreqD/TAG022/CD");
                    len=dirp->len; if (len>=sizeof(sortp->x2992)) len=sizeof(sortp->x2992)-1;
                    strncpy(sortp->x2992,p,len); sortp->x2992[len]='\0';
                    memmove(sortp->x2992_cat,p,1); /* memmove(,,1) */
                    break;
                }
          }
          sprintf(sortp->xrankF,"%6"_LD_,tabcntArankF[v351]); /* key */
          sortp->v351=v351;
          
    } /* end for (sortp=sort_tab) */

    /* sort D
    */
    for (k=ntabs-1, i=0; i<k; i++) {
        ip=sort_tab+i;
        for (j=i+1; j<ntabs; j++) {
            jp=sort_tab+j;
            if (memcmp((void *)ip,(void *)jp,keylen) > 0) {
                memmove((void *)sort_tmp,(void *)ip,      sizeof(OUTFREQD_SORT_ENTRY));
                memmove((void *)ip      ,(void *)jp,      sizeof(OUTFREQD_SORT_ENTRY));
                memmove((void *)jp      ,(void *)sort_tmp,sizeof(OUTFREQD_SORT_ENTRY));
            }
        }
    }


    /* store tabcntArankD (indexed by DecsMfn) and tabfreqD (indexed by rankD)
    */
    for (sortp=sort_tab, tabD=tabfreqD, mfn=1L; mfn<=ntabs; mfn++, sortp++, tabD++) {
        tabD->v351=sortp->v351;
//        tabD->delta=sortp->delta;
        memmove(tabD->delca,sortp->x2992,MAXCATLEN);
        tabD->gmfn=sortp->gmfn;
        tabcntArankD[sortp->v351]=mfn;
    }

    return(0);
}

#if ANSI
int outfreqD(int ntabs, char *dbnoutp, LONGX *tabcntArankF, LONGX *tabcntArankW, TABFREQD_ENTRY *tabfreqD)
#else
int outfreqD(ntabs, dbnoutp, tabcntArankF, tabcntArankW, tabfreqD)
int ntabs;
char *dbnoutp;
LONGX *tabcntArankF;
LONGX *tabcntArankW;
TABFREQD_ENTRY *tabfreqD;
#endif
{
    RECSTRU *recp;
    LONGX mfn;
    char buffup[BUFSIZ],*batchp;

    LONGX upirec;
    LONGX upcrec;

    TABFREQD_ENTRY *tabD;

    if (!ntabs) return(0);

    /* output
    */
    record(upcrec=nrecs,dbnoutp,0L);
    record(upirec=nrecs,dbnoutp,1L);
    for (tabD=tabfreqD, mfn=1L; mfn<=ntabs; mfn++, tabD++) {
        batchp=buffup;

        sprintf(batchp,"A1/%06"_LD_"/"     ,tabD->v351              ); batchp+=strlen(batchp);

        sprintf(batchp,"A2220/%"_LD_"/"    ,tabcntArankF[tabD->v351]); batchp+=strlen(batchp);
        sprintf(batchp,"A2220/%"_LD_"/"    ,tabcntArankW[tabD->v351]); batchp+=strlen(batchp);
        sprintf(batchp,"A2220/%"_LD_"/"    ,mfn                     ); batchp+=strlen(batchp);

        *batchp='\0';
        RECORD(upirec,dbnoutp,mfn); MFRstatus=ACTIVE;
        if (fldupdat(upirec,buffup)) return(mfn);
        recupdat(upcrec,upirec);
    }

    return(0);
}


#if 0
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
#define B7DELADD        "D%dA%d|%09"_LD_"|" /* sprintf format for dir/field */
#define B7EDIT1         "%09"_LD_         /* sprintf format for field edit */

#define MFQTXTAG        101             /* inverted files prefixes */

#endif


#if ANSI
char *recsfldv(char *areap, char *fldp, FFI fldlen, char *defaultp)
#else
char *recsfldv(areap,fldp,fldlen,defaultp)
char *areap;
char *fldp;
FFI fldlen;
char *defaultp;
#endif /* CIAPI_SOURCE */
{
    FFI n;
    unsigned char *p;

    p=subfldp(fldp,'v',&fldlen);

    if (p) n=fldlen; /* ja' decrementado */
    else
        if (defaultp)
            n=(FFI)strlen((char*)(p=(unsigned char*)defaultp));
        else
            n=(FFI)strlen((char*)(p=(unsigned char*)"\0"));
    if (!areap)
        if ((areap=(char *)ALLOC((ALLOPARM)n+1)) == (char *)ALLONULL)
            fatal("aotmsa1/recsfldv/ALLOC");
    memmove(areap,p,n); areap[n]='\0';
    return(areap);
}


#if 0
		for (tag=0; isdigit(*p); p++) tag=tag*10+(*p-'0');
	    if (*p) {
            freex(srcareap,dirarea);
		    return(errp);
	    }
        if (tag) {
    		for (i=0; i+1 < MFRnvf; i++) {
    		    if (DIRtag(i) != tag) continue;
    		    for (j=i+1; j < MFRnvf; j++) {
    		        if (DIRtag(j) != tag) continue;
        		    if (memcmp(FIELDP(i),FIELDP(j),DIRlen(i)) > 0) {
            			memmove(tempdir,&MFRdir[i],sizeof(DIRSTRU));
            			memmove(&MFRdir[i],&MFRdir[j],sizeof(DIRSTRU));
            			memmove(&MFRdir[j],tempdir,sizeof(DIRSTRU));
            		}
                }
        	}
	        break;
        }
#endif
