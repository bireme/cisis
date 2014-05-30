/* mxtb.c    -   CI/tabula casos segundo variavel/variaveis de controle

   uso: mxtb <dbn> <dbnout> {len1:fmt1} {len2:fmt2} ...
              			        [tab=<v>] [class=<n>]

        <dbn>:      data base de entrada

        <dbnout>:   arquivo de saida, contendo um registro para cada
                    combinacao de categorias das variaveis de controle
                    ocorrida em <dbnin>, como abaixo:

                        c1|c2|...|cn|frequencia

        <v1>:       tamanho/formato que sera' tomado como a primeira
                    variavel de controle (ou 't' para total)

        <v2>:       tamanho/formato que sera' tomado como a segunda
                    variavel de controle
           .
           .

        <vn>:       tamanho/formato que sera' tomado como a ultima
                    variavel de controle

        tab=<c>:    para somar o conteudo do campo <c> durante a
                    tabulacao

        class=<n>:  para alocar <n> casos na tabela de frequencias

	memos=<n>:  para alocar <n> modulos para a area de tabulacao

	tell=<n>:   para dar "tabulados" a cada <n> casos

	hash=<n>:   para usar a <n>-esima funcao de hash


    Autor:  tab0.c/AOT/ALP, 24/07/90

    Alter:  AOT, 22/01/91
	    1. parametro memos=
	    
	    AOT/RP, 14/12/93
	    1. alocacao para UNIX (!PM)
	    
	    AOT/MMB, 30/01/94
	    1. ALLO64V = USHRT_MAX (vide hash())
	    2. correcao hashx=2 (agora default if !PC)

	    AOT, 20/08/97
	    1. parametro bool=

	    AOT, 15/09/2002
	    1. ALLO64V = UINT_MAX (vide hash())

	    AOT, 17/05/2004
	    1. execucao do parametro tab= apos tabkeys
        2. fldupdat dos campos 32601, 32602, ...
        3. parametro mfnsum=<n> para forcar tabulacao de mfn nas <n> primeiras chaves
        4. parametros {min|max}{avg|freq}

	    AOT, 10/01/2006
	    1. mxtbofi.c source code

--------------------------------------------------------------------- */

#define xPC 0
#include <stdio.h>
#include <string.h>

#include "cisis.h"          /* CISIS Interface header file */
#include "cirun.h"          /* CISIS Interface runtime declarations */


#define MXTBOL 1
#if MXTBOL
#if CIB71
#include "cib70.h"
char *boolqryp=NULL;
LONGX boolridx=0;
LONGX boolhits=0,boolmfn,boolh=0;

char *booldbnp = NULL;          /* b4x query dbn */
LONGX booltell=1L;               /* 0L = no msg */
LONGX booltlir=0L;               /* b40 invx */

b7_CIB7 *cib7p=NULL;
char booltmpnam[CIMPL+1]={'\0'};    /* b72 MFQTHMEM tmp file */
#else /* CIB71 */
char *boolqryp=NULL;
LONGX boolhits=0L;
LONGX boolridx;
LONGX nbytes1,nbytes2;
LONGX boolh;
char *boolp;
int boolerr;
LONGX boolmfn;
#endif /* CIB71 */
#endif

#define CLASS   1000        /* default */

#define MAXFMTA MAXMFRL	    /* max lw */

int parmmfnsum=0;
LONGX parmmin996=0;
LONGX parmmin997=0;
LONGX parmmax996=0;
LONGX parmmax997=0;
int usecnt1=0;

LONGX count1;                /* registros lidos */
LONGX count2;                /* casos tabulados */

LONGX parmclass;             /* parametro <class> */
int parmhashx;              /* parametro <hash> */
int parmmemos;              /* parametro <memos> */

LONGX ntabs;                 /* numero de entradas de tabkey/tabcnt */
char *tabkey;               /* chaves das entradas, agora via ALLOC */
LONGX *tabcnt;               /* frequencias - agora pointer */
LONGX *tabcnt1;              /* frequencia1 */
LONGX maxhash;
int tabwidth;               /* tamanho de cada entrada de tabkey */
int tabwint;                /* tabwidth em 4 bytes inteiros */
char wkey[MAXFMTA+1];       /* chave para pesquisa */

char *gidbnp;
char *dbnoutp;

char *buffup=NULL;	        /* outfreq/fldupdat, now allocated - AOT, 28/01/2002 */
LONGX buffuplen=MAXMFRL+BUFSIZ;   /* ALLOMAXV? */

LONGX cq1,cq2,cq3,cq4,cqt;   /* teste */


#define UINT unsigned int
#if PC && !DOS32BITS
#define MAX64	200
#define ALLO64V	SHRT_MAX
#else
#define MAX64	2000
#define ALLO64V	UINT_MAX
#endif

int memoidx;		    /* indice do modulo corrente */
UINT vntabs[MAX64];         /* numero de entradas de vtabkey[i]/vtabcnt[i] */
char *vtabkey[MAX64];       /* chaves das entradas, agora via ALLOC */
LONGX *vtabcnt[MAX64];       /* frequencias - agora pointer */
LONGX *vtabcnt1[MAX64];      /* frequencia1 */

FMT_CODE *tabfmtp=NULL;	    /* formato para tab= */
char *tabfmta=NULL;	    /* area para tab= */

LONGX upirec,upcrec;

#define MAXTABX 8
int ntabx=0;
char *areatabx=NULL;
LONGX lintabx[MAXTABX];
LONGX lentabx[MAXTABX];
char *atabx[MAXTABX];
FMT_CODE *ptabx[MAXTABX];
#define MAXTABXL 32
LONGX  linxtabx[MAXTABX];
LONGX  xxlltabx[MAXTABX][MAXTABXL];
char *xxlptabx[MAXTABX][MAXTABXL];
int itabx;
int ilin;
LONGX  xxlftabx[MAXTABX][MAXTABXL];
LONGX  vinxtabx[MAXTABX];
int level;
unsigned char ctabx;

/* prototypes */
#if ANSI
int hash(void);
void outfreq(UINT n);
#else
int hash();
void outfreq();
#endif

void main(argc,argv)
int argc;
char *argv[];
{
    RECSTRU *recp;
    LONGX irec,mfn;

    int widx;
    unsigned char *wptr;
    int l;

    LONGX maxprim,maxdiv,bytskey,bytscnt,divid;
#if xPC
    LONGX parmc;
#endif
    LONGX tabv;
    unsigned char *p,*q;

    int parmtrace=0;
    int parmtrac2=0;
    int parmtrac3=0;
    LONGX parmfrom,parmto,parmloop,parmcount,parmtell;
    int argnext;

    if (argc < 4) {
        printf("%s",cicopyr("Utility MXTB"));
	printf(" \n");
	printf("mxtb <dbn> [create=|append=]<dbnout> <key> [<key> ...] ");
	printf("[<option> [...]] \n");
	printf(" \n");
	printf("keys:    keylen:key_fmtspec \n");
	printf(" \n");
	printf("options: {from|to|loop|count|tell|btell}=<n> \n");
	printf("         tab=<tab_val_fmt> \n");
	printf("         mfnsum=<#levels> [{min|max}{avg|freq}=<n>] \n");
#if PM
	printf("         {class|memos}=<n> \n");
#else
	printf("         class=%"_LD_" \n",(LONGX)CLASS);
#endif
#if MXTBOL
	printf("         bool=<expr> \n");
#endif
	printf("         uctab={<file>|ansi} \n");
	printf(" \n");
	printf("Ex: mxtb in out len1:fmt1 len2:fmt2 len3:fmt3 \n");
	printf(" \n");
	printf("     out = 1  key/key1_value (max len1 chars)\n");
	printf("           2  key/key2_value (max len2 chars)\n");
	printf("           3  key/key3_value (max len3 chars)\n");
	printf("         998  999999999 - key_frequency \n");
	printf("         999  key_frequency \n");
	printf(" \n");
	printf("Ex: mxtb in out len:fmt tab=Vtag \n");
	printf(" \n");
	printf("     out = 1  key_value (max len chars)\n");
	printf("         998  999999999 - Vtag_subtotal \n");
	printf("         999  Vtag_subtotal \n");
	printf(" \n");
	exit(1);
    }

    argnext=1;
    RECORD(irec=nrecs,gidbnp=argv[argnext++],0L);

    dbnoutp=argv[argnext++];

    /*
        pre-processa variaveis de controle
    */
    parmfrom=1; parmto=999999999L; parmloop=1; parmcount=0; parmtell=0;

    tabwidth=0; parmclass=CLASS; 
#if PC && !DOS32BITS
    parmhashx=1;
#else
    parmhashx=2;
#endif
#if PM
    parmmemos=1;
#endif

    for (; argnext < argc; argnext++) {
        p=argv[argnext];
        if (parmtrac2) printf("+++ parm=%s\n",p);
        if (strncmp(p,"from?",5) == 0 || strncmp(p,"from=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&parmfrom) != 1)
                fatal(p);
            continue;
        }
        if (strncmp(p,"to?",3) == 0 || strncmp(p,"to=",3) == 0) {
            if (sscanf(p+3,"%"_LD_,&parmto) != 1)
                fatal(p);
            continue;
        }
        if (strncmp(p,"loop?",5) == 0 || strncmp(p,"loop=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&parmloop) != 1)
                fatal(p);
            continue;
        }
        if (strncmp(p,"count?",6) == 0 || strncmp(p,"count=",6) == 0) {
            if (sscanf(p+6,"%"_LD_,&parmcount) != 1)
                fatal(p);
            continue;
        }
        if (strncmp(p,"tell?",5) == 0 || strncmp(p,"tell=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&parmtell) != 1)
                fatal(p);
            continue;
        }

	if (strcmp(p,"trace") == 0) {
	    parmtrace=1;
            continue;
        }
	if (strcmp(p,"trac2") == 0) {
            parmtrac2=1;
            continue;
        }
	if (strcmp(p,"trac3") == 0) {
            parmtrac3=1;
            continue;
        }


        if (!strncmp(p,"class=",6)) {
	    if ((sscanf(p+6,"%"_LD_,&parmclass)) != 1) fatal(p);
	    if (parmclass < 2) fatal(p);
            continue;
        }
#if PM
	if (!strncmp(p,"memos=",6)) {
	    if ((sscanf(p+6,"%d",&parmmemos)) != 1) fatal(p);
	    if (parmmemos < 1 || parmmemos >= MAX64) fatal(p);
            continue;
        }
#endif
	if (!strncmp(p,"hash=",5)) {
	    if ((sscanf(p+5,"%d",&parmhashx)) != 1) fatal(p);
            if (parmhashx < 1 || parmhashx > 2) fatal(p);
            continue;
        }
        if (!strncmp(p,"tab=",4)) {
	    if (tabfmta) fatal(p);
	    tabfmta=loadfile(gidbnp,'@',q=p+4,tabfmta,MAXFMTA+1,'\0');
	    if (!tabfmta) fatal(q);
	    if (fmt_gener(&tabfmtp,q)) fatal(q+fmt_errof);
            continue;
        }
        if (!strncmp(p,"mfnsum=",7)) {
	        if ((sscanf(p+7,"%d",&parmmfnsum)) != 1) fatal(p);
	        if (parmmfnsum != 1) fatal(p);
            continue;
        }
        if (!strncmp(p,"minavg=",7)) {
	        if ((sscanf(p+7,"%"_LD_,&parmmin996)) != 1) fatal(p);
	        if (parmmax996) if(parmmin996>parmmax996) fatal(p);
            continue;
        }
        if (!strncmp(p,"maxavg=",7)) {
	        if ((sscanf(p+7,"%"_LD_,&parmmax996)) != 1) fatal(p);
	        if (parmmin996) if(parmmin996>parmmax996) fatal(p);
            continue;
        }
        if (!strncmp(p,"minfreq=",8)) {
	        if ((sscanf(p+8,"%"_LD_,&parmmin997)) != 1) fatal(p);
	        if (parmmax997) if(parmmin997>parmmax997) fatal(p);
            continue;
        }
        if (!strncmp(p,"maxfreq=",8)) {
	        if ((sscanf(p+8,"%"_LD_,&parmmax997)) != 1) fatal(p);
	        if (parmmin997) if(parmmin997>parmmax997) fatal(p);
            continue;
        }

        if (!strncmp(p,"bool=",5)) {
	    if (boolqryp) fatal(p); q=p+5;
	    boolqryp=loadfile(gidbnp,'@',q,NULL,0L,'\n');
	    if (!boolqryp) fatal(p);
            continue;
        }
        if (strncmp(p,"uctab?",6) == 0 || strncmp(p,"uctab=",6) == 0) {
		            if (strcmp(p+6,"ansi") == 0) {
				                    memmove(isisuctab,ansiuctab,256/*sizeof(ansiuctab)*/);
						                } else {
									                /* loaductb(mi_gidbnp,isisuctab,p+6); */
									                if (!loaductb(gidbnp,isisuctab,p+6)) fatal(p); /* AOT, 02/04/2001 */
											            }
			                continue;
					        }

        if (strncmp(p,"btell?",6) == 0 || strncmp(p,"btell=",6) == 0) {
            if (sscanf(p+6,"%"_LD_,&booltell) != 1)
                fatal(p);
            continue;
        }

	if ((sscanf(p=p,"%d:",&l)) == 1) {
	    if (ntabx >= MAXTABX) fatal(p);
	    if (l > MAXFMTA) fatal(p);
	    lentabx[ntabx]=l;
	    for (q=p; *q; q++) if (*q == ':') break;
	    if (*q != ':') fatal(p);
	    atabx[ntabx]=loadfile(gidbnp,'@',q+1,NULL,MAXFMTA+1,'\0');
	    if (!atabx[ntabx]) fatal(q+1);
	    q=atabx[ntabx];
	    if (fmt_gener(&ptabx[ntabx],q)) fatal(q+fmt_errof);
	    lintabx[ntabx]=MAXTABXL;
	    if (!ntabx) {
	        areatabx=loadfile(NULL,'@',"",NULL,MAXFMTA+1,'\0');
		if (!areatabx) fatal("ALLOC/areatabx");
	    }
	    ntabx++;
	    tabwidth+=l;	/* '\0' */
	    continue;
	}

	fatal(p);
    }

    if (parmmin996 || parmmax996 || parmmin997 || parmmax997) usecnt1=1;

    if (!ntabx) fatal("no tabulation key specified");

    tabwidth++;			/* '\0' final */
    tabwint=(tabwidth+1)/4;     /* XOR a cada 4 bytes */

    /* setup parmclass/parmmemos (bytskey) */
#if !PM
    parmmemos=1;
#endif

    maxprim=parmclass;          /* contagem em tabkey */
#if PC
    if ((unsigned LONGX)maxprim > UINT_MAX)
	fatal("class overflow");
#endif
    maxhash=0;
    while (!maxhash) {
        maxdiv=maxprim/2;
        for (divid=2;divid<maxdiv;divid++)
            if (!(maxprim % divid)) {
                if (parmtrac3)
		    printf("+++ %"_LD_" divide by %"_LD_"\n",maxprim,divid);
                maxprim--;
                break;
            }
        if (divid >= maxdiv) {
            maxhash=maxprim;
	    if (parmtrace) printf("+++ hash=%"_LD_"\n",maxhash);
        }
    }


    bytskey=maxhash*tabwidth;
    bytscnt=maxhash*sizeof(LONGX);
    if (bytskey >= ALLOMAXV)
	fatal("bytskey/ALLOMAXV");
    if (bytscnt >= ALLOMAXV)
	fatal("bytscnt/ALLOMAXV");

#if xPC
    if (parmtrace) printf("+++ bytes available: %7"_LD_"\n",(LONGX)coreleft());
#endif

for (memoidx=0; memoidx < parmmemos; vntabs[memoidx]=0, memoidx++) {

    if (parmtrace) printf("+++ bytes for keys:  %7"_LD_" (%"_LD_" x %d)\n",
				bytskey,maxhash,tabwidth);

    vtabkey[memoidx]=tabkey=(char *)ALLOC((ALLOPARM)(bytskey));
    if (tabkey == ALLONULL) fatal("ALLOC/tabkey");
    memset(tabkey,0x00,bytskey);

    if (parmtrace) printf("+++ bytes for cnts:  %7"_LD_" (%"_LD_" x %d)\n",
				bytscnt,maxhash,sizeof(LONGX));

    vtabcnt[memoidx]=tabcnt=(LONGX *)ALLOC((ALLOPARM)(bytscnt));
    if (tabcnt == ALLONULL) fatal("ALLOC/tabcnt");
    memset(tabcnt,0x00,bytscnt);

    vtabcnt1[memoidx]=tabcnt1=(LONGX *)ALLOC((ALLOPARM)(bytscnt));
    if (tabcnt1 == ALLONULL) fatal("ALLOC/tabcnt1");
    memset(tabcnt1,0x00,bytscnt);

}

    /* allocate buffup
    */
    if (!buffup) {
        char *dbnp=gidbnp; 
        LONGX asize=0;
        if (boolqryp) asize=
               strlen(dbnp) +                     /* field MFQTDBN0=11 (may be longer than CIMPL+1) */
               strlen(boolqryp) +                 /* the expression */
               2*strlen(boolqryp) +               /* the terms + pseudo operators _1termX+11X */
               BUFSIZ+strlen(dbnp) +              /* field MFQSWDBN=20 + a diferent db */
               24*(strlen(B7DELADD)+3*B7UPLEN) +  /* 2 duzias de control fields */  /* DxxAxx|xxx| */
               strlen(boolqryp)+4 +               /* tmpfile .%03x */
               BUFSIZ;                            /* por si las moscas */
        if (asize > buffuplen) buffuplen=asize;
        buffup=loadfile(NULL,'@',"",NULL,buffuplen,'\0');
    }
    
#if xPC
    if (parmtrace) printf("+++ unused bytes:    %7"_LD_"\n",(LONGX)coreleft());
#endif

#if MXTBOL
    /* initialize ridx and b4_xxx() parameters */

    if (boolqryp) { /* mxbool/boolh */
#if CIB71
    RECSTRU *recp;
    char *p;
    LONGX mfn;
    LONGX crec=0L; /* b5_exp usa crec condicionado a booldbnp */
    int errno,xfd;

    char *dbnp=gidbnp;

    void *ciapip = NULL;
#if CIAPI
    ciapip = cisisApplication( MAXNREC,MAXNTRM );/* mandatory for CIAPI definitions */
#endif /* CIAPI */

    cib7p=b7_cib7(cib7p,ciapip);

    for (boolridx=nrecs, mfn=1; ; mfn++) { /* get actual maxmfn */
        RECORD(boolridx,dbnp,mfn);
    	if (RECrc == RCEOF) fatal(dbnp);
    	if (RECrc == RCNORMAL) break;
    }

    if (booldbnp) {
        dbxopt_fatal=0; xfd=dbxopen("",booldbnp,xx1extp);
        if (xfd > 0) CLOSE(xfd); else recisis0(booldbnp);
        RECORD(crec=nrecs,booldbnp,0L);
        b7setno=MF0nxtmfn-1L;
/*
<R>
        vrecp[boolridx]->recdbxp=RECdbxp;
*/
	VRECdbxp(boolridx)=RECdbxp;
    }
    else {
        VRECdbxp(boolridx)=NULL; /* nao usa base de log */
        VMFRmfn(boolridx)=0L;
    }

    p=b7_exp(cib7p,boolridx,dbnp,boolqryp,buffup,booldbnp,crec,&errno);
    if (p || errno) printf("Expression syntax error %d: '%s'\n",errno,p);
    else {
        /* set hit file name to "null=don't write" or as provided by "b70x=" */
        if (!booldbnp || booltmpnam[0]) {
            char *hitfnamp="null";
            if (booltmpnam[0]) hitfnamp=booltmpnam;
    	    sprintf(buffup,"D%dA%d|%s|",MFQTHMEM,MFQTHMEM,hitfnamp);
    	    if (fldupdat(boolridx,buffup) != NULL) fatal("mxtb/boolridx/fldupdat");
    	    if (b70trace) prtfields(vrecp[boolridx],VMFRmfn(boolridx));
    	}
        /* b7_run faz "trirecp=vrep[tlirec]" */
        if (!booltlir) record(booltlir=nrecs,"null",1L);

    	p=b7_run(cib7p,boolridx,dbnp,buffup,buffuplen,booltell,isisuctab,"",booltlir,crec,&errno);
    	if (p || errno) printf("Execution error %d: '%s'\n",errno,p);
    	else {
    	    boolhits=b7_hit(cib7p,boolridx,0L,b7setno);
    	    if (booltell) printf("Hits=%"_LD_"\n",boolhits);
    	}
    }
    if (p || errno) fatal("Expression execution error");
#else /* CIB71 */
	/* get maxmfn */
	nbytes1=MF0nxtmfn/8+1;	/* one bit per mfn for b4_run bit strings */
	recallok(boolridx=nrecs,BUFSIZ+nbytes1);
	nbytes2=4096L; /* buffer size for b4_run() tmp file */

	/* compile/execute boolean expression */
	boolp=b4_exp(boolridx,gidbnp,boolqryp,NULL,&boolerr);
	if (boolp) {
	  printf("Error #%d (%s)\n",boolerr,boolp);
	  fatal("Expression syntax error");
	}
	fldupdat(boolridx,"D12"); /* delete MFQTQRY0 */
	boolp=b4_run(boolridx,gidbnp,nbytes1,nbytes2,parmtrace,isisuctab,0L,
								&boolerr);
	if (boolp) {
	  printf("Error #%d (%s)\n",boolerr,boolp);
	  fatal("Expression execution error");
	}
	boolhits=b4_hit(boolridx,0L);
#endif /* CIB71 */
    } /* mxbool/boolh */
#endif

    count2=0; count1=0;

    if (!tabfmtp) tabv=1;                         /* tab field default */

    cq1=cq2=cq3=cq4=cqt=0;

    if (!strncmp(dbnoutp,"create=",7)) {
	dbnoutp+=7;
	recisis0(dbnoutp);
    }
    if (!strncmp(dbnoutp,"append=",7)) {
	dbnoutp+=7;
    }

    if (parmtrace) printf("+++ maxmfn=%"_LD_"\n",MF0nxtmfn-1);

    record(upcrec=nrecs,dbnoutp,0L);
    record(upirec=nrecs,dbnoutp,1L);


    /*
        loop principal
    */

    memoidx=0;

#if MXTBOL
    for (mfn=parmfrom, boolh=1L; ; mfn+=parmloop,
                                   boolh = ((boolh+=parmloop) > boolhits) ?
                                                                boolhits+1:
                                                                boolh      ) {

        if (boolqryp) { /* mxbool/boolh */
#if CIB71
	    boolmfn=b7_hit(cib7p,boolridx,boolh,b7setno);
#else /* CIB71 */
            boolmfn=b4_hit(boolridx,boolh);
#endif /* CIB71 */
            if (parmtrace) printf("+++ boolmfn=%"_LD_"\n",boolmfn);
            if (boolmfn <= 0L)      break;
            if (boolmfn < parmfrom) continue;
            mfn=boolmfn;
        }
	RECORD(irec,RDBname,mfn);
	if (RECrc == RCEOF) break;
#else
    for (RECORD(irec,RDBname,mfn=parmfrom);
		RECrc != RCEOF && mfn <= parmto;
			mfn+=parmloop, RECORD(irec,RDBname,mfn)) {
#endif

	if (mfn > parmto) break;

        if (parmcount)
	    if (count1 >= parmcount) break;

        if (RECrc != RCNORMAL)
	    continue;

#if BEFORE20040517
	if (tabfmtp) {
	    fmt_inter(tabfmtp,irec,(LONGX)MAXFMTA,tabfmta,(LONGX)MAXFMTA);
	    tabv=atol(tabfmta);
	}
#endif

	for (itabx=0; itabx < ntabx; itabx++) {
	    p=atabx[itabx]; *p='\0';
	    fmt_inter(ptabx[itabx],irec,(LONGX)MAXFMTA,p,(LONGX)MAXFMTA);
	    linxtabx[itabx]=0;
	    xxlltabx[itabx][0]=0;
	    xxlptabx[itabx][0]=p;
	    ctabx='\0';
	    while (*p) {
		xxlltabx[itabx][linxtabx[itabx]]=0;
		xxlptabx[itabx][linxtabx[itabx]]=p;
		for (; *p; p++) {
		    ctabx = *p;
		    if (ctabx == '\n' || ctabx == '\r') {
			p++;
			if (*p == '\n' || *p == '\r') p++;
			break;
		    }
		    xxlltabx[itabx][linxtabx[itabx]]++;
		}
		if (++linxtabx[itabx] >= lintabx[itabx]) break;
	    }
#if 0
	    if (ctabx != '\n')
		if (ctabx != '\r')
		    if (atabx[itabx][0]) linxtabx[itabx]++;
#endif
	}
	for (itabx=0; itabx < ntabx; itabx++) {
	    xxlftabx[itabx][0]=lentabx[itabx]; /* or 0 */
	    for (ilin=0; ilin < linxtabx[itabx]; ilin++)
		if (xxlltabx[itabx][ilin] > lentabx[itabx]) {
		    xxlltabx[itabx][ilin] = lentabx[itabx];
		    xxlftabx[itabx][ilin] = 0;
		}
		else
		    xxlftabx[itabx][ilin] = lentabx[itabx] -
					    xxlltabx[itabx][ilin];
	}

	for (itabx=0; itabx < ntabx; itabx++) vinxtabx[itabx]=0;
	for (level=ntabx-1; ; ) {

	    /* process itabx = 0 .. ntabx-1 */
	    wptr=wkey;
        p=tabfmta;
	    for (itabx=0; itabx < ntabx; itabx++) {
		ilin=vinxtabx[itabx];
		memmove(wptr,xxlptabx[itabx][ilin],xxlltabx[itabx][ilin]);
        if (tabfmtp) {
            sprintf(p,"H%5d %5d ",32601+itabx,xxlltabx[itabx][ilin]); p+=13;
		    memmove(p,   xxlptabx[itabx][ilin],xxlltabx[itabx][ilin]); p+=xxlltabx[itabx][ilin]; *p='\0';
        }
		wptr+=xxlltabx[itabx][ilin];
		memset(wptr,0x00,xxlftabx[itabx][ilin]);
		wptr+=xxlftabx[itabx][ilin];
	    }
	    *wptr='\0';

	    if (*wkey) { /* if *wkey */
#if BEFORE20040517
#else
	if (tabfmtp) {
        if (parmtrac2) printf("\n+++%s\n",tabfmta);
        if (fldupdat(irec,tabfmta)) fatal("mxtb/32601/H");
        if (parmtrac2) prtfields(recp,mfn);
	    fmt_inter(tabfmtp,irec,(LONGX)MAXFMTA,tabfmta,(LONGX)MAXFMTA);
	    tabv=atol(tabfmta);
        p=tabfmta;
	    for (itabx=0; itabx < ntabx; itabx++) {
            sprintf(p,"D%5d",32601+itabx); p+=6;
        }
        if (fldupdat(irec,tabfmta)) fatal("mxtb/32601/D");
	}
#endif
		if (parmmfnsum) {
            memset(wkey,0x00,lentabx[0]);
            sprintf(wkey,"%"_LD_,mfn);
        }
        if (parmtrac2) {
            char *p;
            int n=tabwidth;
		    printf("+++"); getchar();
		    printf("+++count,wkey,tabv: %"_LD_",",count2);
            for (p=wkey; n--; p++) if (*p) putchar(*p); else putchar(';');
		    printf(",%"_LD_"\n",tabv);
		}
        if (tabv) { /* if tabv */
		widx=hash();
		tabcnt[widx]+=tabv;
        if (usecnt1) tabcnt1[widx]+=1;
		if (parmtrac3) {
		    printf("+++cq1/4: %"_LD_",%"_LD_",%"_LD_",%"_LD_"\n",cq1,cq2,cq3,cq4);
		    printf("+++widx,tabcnt: %d,%"_LD_"\n",widx,tabcnt[widx]);
		}
		count2++;
        } /* end if tabv */
	    } else { /* else if *wkey */
		if (parmtrac2) {
		    printf("+++"); getchar();
		    printf("+++count,,tabv: %"_LD_",,%"_LD_"\n",count2,tabv);
		}
	    } /* end if *wkey */

	    /* get next */
	    while (++vinxtabx[level] >= linxtabx[level])
		if (level) vinxtabx[level--]=0; else {level= -1; break;}
	    if (level < 0) break;
	    level=ntabx-1;
	}

	cqt+=cq3; count1++;
	if (parmtell)
	    if ((count1 % parmtell) == 0) {
		fprintf(stderr,"+++ %"_LD_": %"_LD_"/%"_LD_"/%"_LD_"\n",
		    count1,count2,ntabs,cqt);
		cq1=cq2=cq3=cq4=0;
	    }

    }

    if (parmtrace) fprintf(stderr,"+++ updating %s ..\n",dbnoutp);
    for (memoidx=0; memoidx < parmmemos; memoidx++) {
	tabkey=vtabkey[memoidx];
	tabcnt=vtabcnt[memoidx];
	tabcnt1=vtabcnt1[memoidx];
	outfreq(vntabs[memoidx]);
    }

    if (parmtell) {
	fprintf(stdout,"mxtb: %"_LD_" recs in\n",count1);
	fprintf(stdout,"      %"_LD_" tabs\n",count2);
	fprintf(stdout,"      %"_LD_" recs out\n",ntabs);
    }

#if CIB71
    if (cib7p) cib7p=b7_cib7_delete(cib7p);
#if CIAPI
    cisisApplicationDelete(ciapip);
    ciapip=NULL;
#endif /* CIAPI */
#endif

    exit(0);
}


int hash()                                                          /*
--- ----                                                            */

{
    unsigned int hashv,*hptr;
    int i,hidx,widx;
    char *wptr;

    unsigned LONGX hashv2,hv2;
    int k;
    unsigned char *p;


    switch (parmhashx) {
    case 1:
            hashv=0;
	    hptr=(unsigned int *)wkey;
	    for (i=tabwint; i > 0 ; i--)    /* XOR a cada 4 bytes */
		hashv ^= *hptr++;
            hidx=hashv%maxhash;             /* modulo maxhash */
            break;
    case 2:
            hashv2=0L;
            p=(unsigned char *)wkey;
            k=0;
            for (i=tabwidth-1; i--;) {     /* XOR byte a byte */
		hv2 = *p++;
#if 0
		hv2 <<= k;
		hashv2 ^= hv2;
                if (++k == sizeof(LONGX))
                    k=0;
#else
                if (*p == ' ') continue;
		hv2 <<= k*8;
		hashv2 ^= hv2;
                if (++k == sizeof(LONGX))
                    k=0;
#endif
            }
            hidx=hashv2%maxhash;            /* modulo maxhash */
            break;
    default:
	    fatal("mxtb/parmhashx");
    }

    widx=hidx;

    memoidx=widx%parmmemos;
    tabkey=vtabkey[memoidx];
    tabcnt=vtabcnt[memoidx];
    tabcnt1=vtabcnt1[memoidx];

#if 0
printf("+0+ key=%s idx=%d memo=%d\n",wkey,widx,memoidx);
#endif

    wptr = &tabkey[widx * tabwidth];

    if (*wptr == NULL) {
        memmove(wptr,wkey,tabwidth);
	ntabs++; vntabs[memoidx]++;
        cq1++;
    }
    else {
        while (1) {
            if (strncmp(wptr,wkey,tabwidth) == 0) {
                cq2++;
                break;
            }
            else {
                if (++widx == hidx) {
                    printf("wkey='%s'\n",wkey);
		    /* outfreq(vntabs[memoidx]); */
		    fatal("mxtb/maxhash overflow");
                }
                if (widx == maxhash) {
                    widx=0;
                    wptr=tabkey;
                    cq4++;
                }
                else {
                    wptr+=tabwidth;
                    cq3++;
                }
                if (*wptr == NULL) {
                    memmove(wptr,wkey,tabwidth);
                    ntabs++; vntabs[memoidx]++;
                    break;
                }
            }
        }
    }

    return(widx);
}


void outfreq(n)
UINT n;
{
    int widx,wlen;
    char *wptr;
    char *batchp,*p;
    int print;

    widx=0;
    wptr=tabkey;
    do {
        if (widx == maxhash)
	    fatal("mxtb/outfreq/widx == maxhash");
        if (*wptr) {
	    batchp=buffup;
	    for (itabx=0;itabx<ntabx;itabx++) {
		for (wlen=0, p=wptr; *p; p++)
		    if (wlen < lentabx[itabx]) wlen++;
		    else break;
		sprintf(batchp,"H%d %d ",itabx+1,wlen); batchp+=strlen(batchp);
		memmove(batchp,wptr,wlen);
		batchp+=wlen;
                wptr+=lentabx[itabx];

            }
            wptr++; /* NULL */

        *batchp='\0';
        print=1;
        if (tabcnt1[widx]) {
            LONGX  frq=tabcnt1[widx];
            float avg=(float)tabcnt[widx]/(float)tabcnt1[widx];
            if (parmmin996) if (avg < parmmin996) print=0;
            if (parmmax996) if (avg > parmmax996) print=0;
            if (parmmin997) if (frq < parmmin997) print=0;
            if (parmmax997) if (frq > parmmax997) print=0;
            if (print) {
/*
                sprintf(batchp,"A998/%09"_LD_"/A999/%"_LD_"/A997/%"_LD_"/A996/%f/A995/%09"_LD_"/",
                    999999999L-tabcnt[widx],tabcnt[widx],frq,avg,999999999L-(LONGX)avg);
*/
                sprintf(batchp,"A998/%09"_LD_"/A999/%"_LD_"/",999999999L-tabcnt[widx],tabcnt[widx]);
                batchp+=strlen(batchp);
                sprintf(batchp,"A997/%"_LD_"/A996/%f/A995/%09"_LD_"/",frq,avg,999999999L-(LONGX)avg);
            }
        }
        else {
                sprintf(batchp,"A998/%09"_LD_"/A999/%"_LD_"/",999999999L-tabcnt[widx],tabcnt[widx]);
        }
	    batchp+=strlen(batchp);
	    *batchp='\0';
    if (print) {
	    record(upirec,dbnoutp,VMF0nxtmfn(upcrec));
	    VMFRstatus(upirec)=ACTIVE;
	    if (fldupdat(upirec,buffup)) fatal(fldupdat(upirec,buffup));
	    recupdat(upcrec,upirec);
    }
            n--;
        }
        else
            wptr+=tabwidth;
        widx++;
    } while (n);

}
