/* dx0.c    -   converte arquivos B-3700 (sem lf's)
                para carga no unify (dlm's e lf's)
                (originalmente...)

   uso: dx0 <filin> <filout> <lrecl> [modo=dat]
            [pc1] [r1x]n1 [=s1] ... [pck] [rkx]nk [=sk]
            [numera[=n]] [duplo[=n]] [skip=[b=]n] [count=n]

        <filin>:    arquivo de entrada 

        <filout>:   arquivo de saida, com os dados de entrada
                    reformatados em arquivos tipo UNI (|,LF)
                    de acordo com os parametros de chamada

        <lrecl>:    tamanho do registro de entrada

        [modo]:     modo=dat para nao gravar tipo UNI e sim DAT
                    (sem delimitador entre campos, nem LF final)

        [pci]:      posicionamento de coluna para extracao:

                    c   -   numero da coluna

        [rix]ni:    especificacao dos campos que serao gravados:

                    r   -   numero de repeticoes (default=1)
                    n   -   numero de bytes do campo

                    ex: 1 1 2 35x1
                        especifica 38 campos consecutivos: dois
                        de um byte, um de dois bytes e trinta e
                        cinco de um byte, a partir da coluna 1
                    
                    Se r=0, entao os n bytes especificados em 0xn
                    serao apenas saltados, ie, nao regravados em
                    <filout>

        [=si]:      especificacao de literal a ser gravado como
                    continuacao do campo anterior (ie, sem dlm apos
                    campo anterior) e, ainda, sem o dlm apos a
                    movimentacao do literal:

                    s   -   string

                    ex: =1      para gravar o string 1
                        =abc    para gravar o string abc
                        =" "    para gravar um branco
                        ="|s|"  para gravar um campo .uni com s
                        =LF     para gravar um '\n'

        [numera]:   Gera, no final de cada registro, um contador
                    de registros em 6 bytes, a partir de 1

                    Se usado numera=n, entao o valor inicial do
                    contador sera' n

        [duplo]:    Para checar presenca de pares de registros de
                    de entrada de k=<lrecl>/2 bytes sendo o byte 0
                    numerico e o byte k nao numerico ou, se usado
                    duplo=n, os n bytes a partir do byte k nao nu-
                    mericos (uso: INC987)

        [skip=n]:   Para saltar n registros antes do processamento
                    ou, se skip=b=n, para saltar n bytes

        [count=n]:  Para processar no maximo n registros


    Autor:  datuni0.c02/AOT
            FCC, 12/08/87

    Alter:  AOT, 12/08/87
            1. opcao numera=n
            2. opcao duplo=n
    Alter:  AOT/YRA, 13/08/87
            1. opcao modo=dat
    Alter:  AOT/YRA, 12/01/88
            1. opcao =string
    Alter:  AOT/WA, 20/01/88
            1. opcao skip=n
            2. opcao count=n
    Alter:  AOT/WA, 17/05/88
            1. opcao skip=b=n
    Alter:  AOT, 30/11/88
            1. dx0
    Alter:  AOT/AARG, 23/04/99
            1. opcao tell=n
    Alter:  AOT, 14/08/2001
            1. cicopyr()
----------------------------------------------------------- */


#include <stdio.h>
#include <string.h>
#include "cisis.h"
#include "cirun.h"

#define CHAR UBYTE

#define MODOP 0644  /* dono: LG, grupo: L, outros: L */

#define MBUFSIZ 8192

CHAR bufin[MBUFSIZ];            /* readbsiz */
CHAR ibuf1[MBUFSIZ];
CHAR *ibuf = ibuf1;
int ibufsize;
int f1,n1left,i1left,n1recl;

CHAR bufout[MBUFSIZ];           /* writbsiz*/
CHAR obuf1[MBUFSIZ];
CHAR *obuf = obuf1;
int obufsize;
int f2=0,n2free,i2free=0; /* AOT, 14/08/2001 */

CHAR sbuf[MBUFSIZ];
CHAR *sptr = sbuf;
int lastdlm;
int fdup;

LONGX count,countx;

                                  /* "0123456789abcdef" */
unsigned char ebctoasc[16][16+1] = { "!!!!!!!!! !!!!!!",
                                     "!!!!!!!!!!!!!##!",
                                     "!!!!!!!!!!!!!!!!",
                                     "!!!!!!!!!!!!!!!!",
                                     " !!!!!!!!![.<(+!",
                                     "&^^^!!!!!!!$*);^",
                                     "-/!!!!!!!!!,%_>?",
                                     "!!!!!!!!!`:#@'=\"",
                                     "^abcdefghi!!!!!!",
                                     "!jklmnopqr!!!\"!!",
                                     "!~stuvwxyz!!![!!",
                                     "!!!!!!!!!!!!!]!!",
                                     "{ABCDEFGHI!!!!!!",
                                     "}JKLMNOPQR!!!!!!",
                                     "\\!STUVWXYZ!!!!!!",
                                     "0123456789!!!!!!" };
int parmconv;

LONGX parmtell=1000L;

#if ANSI
void erro(CHAR *s1, CHAR *s2);
int readbsiz(void);
void writbsiz(void);
void breakout(void);
#else
void erro();
int readbsiz();
void writbsiz();
void breakout();
#endif

main(argc,argv)

int argc;
char *argv[];

{
    int n1;

    int parmlrecl;

    LONGX parmskip,parmskib,parmcount;

    static int ndats;
    static CHAR *vdatptr[256];
    static int  vdatsiz[256];
    static CHAR *voutptr[256];

    static int nstrs;
    static CHAR *sdatptr[256];
    static int  sdatsiz[256];
    static CHAR *soutptr[256];

    CHAR *idatptr,*odatptr,*pnumer,*pfinal,*p,*q;

    int i,r,n,l,nd;
    int k;

    int znumer,zduplo,zmodat;
    int dupnn;
    LONGX cnumer;
    CHAR *dup1,*dup2;


    if (argc < 5) {
        printf("%s",cicopyr("Utility AOT/DX0"));
        printf("\n");
        printf("dx0 <filin> <filout> <lrecl> [modo=dat] [pc1] [r1x]n1 [=s] .. \n");
        printf("    [numera[=n]] [ebcdic] [skip=[b=]n] [count=n] [tell=n] \n");
        printf("\n");
        exit(1);
    }

#if PC
    if ((f1=open(argv[1],O_RDONLY | O_BINARY)) == -1)
#else
    if ((f1=open(argv[1],O_RDONLY           )) == -1)
#endif
        erro("impossivel abrir o arquivo ",argv[1]);

#if PC
    if ((f2=open(argv[2],O_WRONLY|O_BINARY|O_CREAT|O_TRUNC,MODOP)) == -1)
#else
    if ((f2=creat(argv[2],MODOP)) == -1)
#endif
        erro("impossivel criar o arquivo ",argv[2]);

    if ((sscanf(argv[3],"%d",&parmlrecl)) != 1)
        erro("parametro lrecl invalido",NULL);
    if (parmlrecl > MBUFSIZ)
        erro("parametro lrecl maior do que o previsto",NULL);


    ndats=0;
    nstrs=0; lastdlm=0;
    obufsize=0;
    idatptr=ibuf;
    zmodat=0;
    znumer=0;
    zduplo=0;

    parmskip=0; parmskib=0; parmcount=999999999; parmconv=0;

    for (i=4;i<argc;i++) {
        if (!strcmp(argv[i],"ebcdic")) {
            parmconv=1;
            printf("+++conversao ebcdic/ascii\n");
        }
        else
        if (!strcmp(argv[i],"modo=dat")) {
            zmodat=1;
            if (i > 4)
                erro("esperava modo=dat como 4. argumento",NULL);
            printf("+++gravacao da saida em modo DAT\n");
        }
        else
        if (!strncmp(argv[i],"numera",6)) {
            znumer=1;
            cnumer=1;
            p=argv[i]+6;
            if (*p) {
                if (*p != '=')
                    erro("esperava 'numera='",NULL);
                if ((sscanf(++p,"%"_LD_,&cnumer)) != 1)
                    erro("valor inicial de numera invalido",NULL);
            }
            printf("+++saida sera' numerada a partir de %"_LD_"\n",cnumer);
        }
        else
        if (!strncmp(argv[i],"duplo",5)) {
            if (parmlrecl%2)
                erro("opcao 'duplo' incompativel com <lrecl>",NULL);
            zduplo=parmlrecl/2;
            dupnn=1;
            p=argv[i]+5;
            if (*p) {
                if (*p != '=')
                    erro("esperava 'duplo='",NULL);
                if ((sscanf(++p,"%d",&dupnn)) != 1)
                    erro("valor de duplo= nao numerico",NULL);
                if (dupnn > zduplo)
                    erro("valor de duplo= invalido",NULL);
            }
            printf("+++saida dupla de %d bytes (1+%d)\n",zduplo,dupnn);
        }
        else
        if (!strncmp(argv[i],"skip",4)) {
            p=argv[i]+4;
            if (*p) {
                if (*p != '=')
                    erro("esperava 'skip='",NULL);
                q=p;
                if (*++q == 'b' && *++q == '=') {
                    if ((sscanf(++q,"%"_LD_,&parmskib)) != 1)
                        erro("valor de skip=b invalido",NULL);
                }
                else
                    if ((sscanf(++p,"%"_LD_,&parmskip)) != 1)
                        erro("valor de skip invalido",NULL);
            }
            if (parmskip)
                printf("+++skip=%"_LD_" registros de %d bytes\n",parmskip,parmlrecl);
            if (parmskib)
                printf("+++skip=%"_LD_" bytes\n",parmskib);
        }
        else
        if (!strncmp(argv[i],"count",5)) {
            p=argv[i]+5;
            if (*p) {
                if (*p != '=')
                    erro("esperava 'count='",NULL);
                if ((sscanf(++p,"%"_LD_,&parmcount)) != 1)
                    erro("valor de count invalido",NULL);
            }
            printf("+++count=%"_LD_" registros de %d bytes\n",parmcount,parmlrecl);
        }
        else
        if (!strncmp(argv[i],"tell",4)) {
            p=argv[i]+4;
            if (*p) {
                if (*p != '=')
                    erro("esperava 'tell='",NULL);
                if ((sscanf(++p,"%"_LD_,&parmtell)) != 1)
                    erro("valor de tell invalido",NULL);
            }
        }
        else

            if (*argv[i] == 'p') {
                if ((sscanf(argv[i]+1,"%d",&k)) != 1)
                    erro("parametro invalido ",argv[i]);
                idatptr = ibuf+k-1;
            }
            else 
            if (*argv[i] == '=') {
                p=argv[i]+1;
                if (!strcmp(p,"LF"))
                    strcpy(sptr,"\n");
                else
                    strcpy(sptr,p);
                p=sptr; n=strlen(p); sptr+=n; sptr++; /* NULL */
                if (lastdlm)
                    obufsize--; /* p/ mover sobre dlm anterior */
                printf("+++string '%s' em +%d(%d)\n",p,obufsize,n);
                sdatptr[nstrs]=p;
                soutptr[nstrs]=obuf+obufsize;
                sdatsiz[nstrs]=n;
                nstrs++; obufsize+=n; lastdlm=0;
            }
            else {
                r=1;
                if ((sscanf(argv[i],"%dx%d",&r,&n)) != 2) {
                    r=1;
                    if ((sscanf(argv[i],"%d",&n)) != 1) 
                        erro("parametro invalido ",argv[i]);
                    }
                if (r == 0)
                    idatptr+=n;
                else
                    for (l=1;l<=r;l++) {
                        if (idatptr+n > &ibuf[parmlrecl])
                            erro("parametro <lrecl> foi ultrapassado por ",argv[i]);
                        vdatptr[ndats]=idatptr;
                        vdatsiz[ndats]=n;
                        voutptr[ndats]=obuf+obufsize;
                        ndats++;
                        idatptr+=n;
                        obufsize+=n;
                        if (zmodat) {
                            lastdlm=0;  /* false */
                        }
                        else {
                            obufsize++; /* | ou LF */
                            lastdlm=1;  /* true */
                        }
                    }
            }
    }


    /*
        processa skip
    */
    LSEEK64(f1,parmskib+parmskip*(LONGX)parmlrecl,0);

    /*
        inicializa readbsiz e faz primeira leitura
    */
    n1left=0;
    ibufsize=parmlrecl;
    n1=readbsiz();
    if (n1 != EOF)
        count++;

    /*
        inicializa writbufz
    */
    n2free=MBUFSIZ;

    /*
        trata znumer
    */
    pnumer=NULL;
    if (znumer) {
        pnumer=obuf+obufsize;
        obufsize+=6;
        if (!zmodat)
            if (lastdlm)
                obufsize++;
    }

    /*
        trata LF final
    */
    pfinal=NULL;
    if (!zmodat)
        if (lastdlm)
            pfinal=obuf+obufsize-1;
        else {
            pfinal=obuf+obufsize;
            obufsize++;
        }


    /*
        trata zduplo
    */
    if (zduplo) {
        dup1=ibuf;
        dup2=ibuf+zduplo;
        if ((fdup=creat("dx0.dup",MODOP)) == -1)
            erro("impossivel criar o arquivo ","dx0.dup");
        count++;
    }


    /*
        loop principal
    */
    while (n1 != EOF && count <= parmcount) {

        if (zmodat) 
            for (nd=0;nd<ndats;nd++) {
                idatptr=vdatptr[nd];
                odatptr=voutptr[nd];
                n=vdatsiz[nd];
                while (n--)
                    *odatptr++ = *idatptr++;
            }
        else
            for (nd=0;nd<ndats;nd++) {
                idatptr=vdatptr[nd];
                odatptr=voutptr[nd];
                n=vdatsiz[nd];
                while (n--)
                    *odatptr++ = *idatptr++;
                *odatptr++ = '|';
            }

        for (nd=0;nd<nstrs;nd++) {
            idatptr=sdatptr[nd];
            odatptr=soutptr[nd];
            n=sdatsiz[nd];
            while (n--)
                *odatptr++ = *idatptr++;
        }

        if (pnumer)
            sprintf(pnumer,"%06"_LD_,cnumer);

        if (pfinal) 
            *pfinal='\n';


        if (zduplo) {

            p=dup2;
            i=dupnn;
            while (i--)
                if (*p < '0' || *p > '9') {
                    p=0;
                    break;
                }
                else
                    p++;
            
            if (*dup1 >= '0' && *dup1 <= '9' && p == 0) {
                writbsiz();
                n1=readbsiz();
                if (n1 > 0) {
                    count+=2;
                    cnumer++;
                }
            }
            else {
                write(fdup,obuf,obufsize);
                countx++;
                strncpy(dup1,dup2,zduplo);
                ibuf=dup2; ibufsize=zduplo;
                n1=readbsiz();
                if (n1 > 0) {
                    n1+=zduplo;
                    count++;
                }
                ibuf=dup1; ibufsize=zduplo+zduplo;
            }
        }

        else {
            writbsiz();
            n1=readbsiz();
            if (n1 > 0) {
                count++;
                cnumer++;
            }
        }

        if (parmtell) if (!(count % parmtell))
            fprintf(stdout,
                "+++processados: %"_LD_" registros de %d bytes\n",
                ((zduplo)?count>>1:count),parmlrecl);
    }

    breakout();

    if (count > parmcount)
        count--;

    fprintf(stdout,
        "dx0: %"_LD_" registros processados\n",count);

    if (zduplo)
        fprintf(stdout,
            "     %"_LD_" registros nao duplos\n",countx);

    if (znumer)
        fprintf(stdout,
            "     registros numerados ate' %06"_LD_"\n",cnumer);

    exit(0);
}


int readbsiz()                                                  /*
--- ---------                                                   */

{
    int n1,i,j;
    CHAR *p,*q;
    int loop;

    if (n1left >= ibufsize) {
        n1=ibufsize;
/*0     strncpy(ibuf,&bufin[i1left],ibufsize); */
	for (p=ibuf, q=bufin+i1left, loop=ibufsize; loop--; )  *p++ = *q++;
        n1left-=ibufsize;
        i1left+=ibufsize;
    }
    else {
        n1=n1left;
/*0     strncpy(ibuf,&bufin[i1left],n1); */
	for (p=ibuf, q=bufin+i1left, loop=n1; loop--; )  *p++ = *q++;
        n1left=read(f1,bufin,MBUFSIZ);
        i1left=0;
        if (n1+n1left) {
            n1recl=ibufsize-n1;
            if (n1left < n1recl) {
                fprintf(stdout,"count: %"_LD_"\n",count);
                fprintf(stdout,"ibufsize: %d\n",ibufsize);
                fprintf(stdout,"n1: %d\n",n1);
                erro("esperava mais bytes no arquivo de entrada",NULL);
                }
/*0         strncpy(&ibuf[n1],bufin,n1recl); */
	    for (p=ibuf+n1, q=bufin, loop=n1recl; loop--; )  *p++ = *q++;
            n1+=n1recl;
            n1left-=n1recl;
            i1left+=n1recl;
        }
        else
            n1=EOF;
    }

    if (parmconv) {
/* for (q=ibuf, loop=ibufsize; loop--; q++)
putchar(*q); */
        for (q=ibuf, loop=ibufsize; loop--; q++) {
            i=((*q)>>4);
            j=((*q)&0x0F);
            *q = ebctoasc[i][j];
        }
/* for (q=ibuf, loop=ibufsize; loop--; q++)
putchar(*q);
getchar(); */
    }

    return(n1);
}


void writbsiz()                                                          /*
---------                                                           */

{
    int n2;
    CHAR *p,*q;
    int loop;

/*+++
    write(f2,obuf,obufsize);
    return;
+++*/

    if (n2free >= obufsize) {
/*0     strncpy(&bufout[i2free],obuf,obufsize); */
	for (p=bufout+i2free, q=obuf, loop=obufsize; loop--; )  *p++ = *q++;
        n2free-=obufsize;
        i2free+=obufsize;
    }
    else {
/*0     strncpy(&bufout[i2free],obuf,n2free); */
	for (p=bufout+i2free, q=obuf, loop=n2free; loop--; )  *p++ = *q++;
        n2=write(f2,bufout,MBUFSIZ);
        if (n2 < MBUFSIZ)
            erro("writbsiz - ","erro na gravacao");
        i2free=obufsize-n2free;
/*0     strncpy(bufout,&obuf[n2free],i2free); */
	for (p=bufout, q=obuf+n2free, loop=i2free; loop--; )  *p++ = *q++;
        n2free=MBUFSIZ-i2free;
    }
}


void breakout()                                                      /*
---------                                                       */

{
    int n2;

/*+++
    return;
+++*/

    if (i2free) if (f2) { /* AOT, 14/08/2001 */
        n2=write(f2,bufout,i2free);
        if (n2 < i2free)
            erro("breakout - ","erro na gravacao");
    }
}


void erro(s1,s2)     /* imprime mensagem de erro e morre */
CHAR *s1,*s2;
{
    fprintf(stderr,
        "dx0: %s%s\n",s1,s2);

    breakout();

    exit(1);
}
