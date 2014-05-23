/*-----------------------------------------------------------
   fxl0.c - regrava arquivo em registros de tamanho fixo

   uso: fxl0 <leng> <filin> <filout> [opc1]

        <leng>:     numero de bytes de dados que os registros
                    terao em <filout>

        <filin>:    arquivo de entrada

        <filout>:   arquivo de saida contendo os registros li-
                    dos mas preenchidos com brancos 'a direita
                    de forma que todos os registros tenham uma
                    largura de <leng> bytes, alem do LF

	<opc1>:	    para gravar <leng> - numero de brancos finais
		    em cada registro de saida

    Autor:  AOT/LK
            FCC, 02/04/87

    Alter:  AOT, 21/03/89
	    1. parametro <opc1>
	    2. writbsiz

----------------------------------------------------------- */


#include <stdio.h>
#include <string.h>

#include "cisis.h"   /* CISIS Interface header file */
#include "cirun.h"   /* CISIS Interface runtime declarations */


#define LF 012      /* line feed */
#define SP 040      /* space */

#define NULX 0xFF

#define MODOP 0644  /* dono: LG, grupo: L, outros: L */

#define MBUFSIZ 8192

unsigned char ibuf[MBUFSIZ+1];
unsigned char *iptr;

unsigned char bufout[MBUFSIZ];           /* writbsiz*/
unsigned char obuf1[MBUFSIZ];
unsigned char *obuf = obuf1;
int obufsize;
int f2=0,n2free,i2free=0;


main(argc,argv)

int argc;
char *argv[];

{
    int f1;
    int j;  /* indice em obuf */
    int n1;

    int parmleng;

    int i,n,parmopc1,parmopc2;
    unsigned char *p;

    LONGX count;


    if (argc < 4) {
        printf("%s",cicopyr("Utility AOT/FXL0"));
        printf("\n");
        printf("fxl0 <leng> <filin> <filout> [opc1] \n");
        printf("\n");
        exit(1);
    }

    if(sscanf(argv[1],"%d",&parmleng) != 1)
        erro("parametro <leng> invalido: ",argv[1]);

    if ((f1=open(argv[2],O_RDONLY)) == -1)
        erro("impossivel abrir o arquivo ",argv[2]);

    if ((f2=creat(argv[3],MODOP)) == -1)
        erro("impossivel criar o arquivo ",argv[3]);

    parmopc1=parmopc2=0;

    for (i=4; i < argc; i++) {
	if (!strcmp(argv[i],"opc1")) {
	    parmopc1=1;
	    printf("+++opc1 on\n");
	    continue;
	}
	if (!strcmp(argv[i],"opc2")) {
	    parmopc2=1;
	    printf("+++opc2 on\n");
	    continue;
	}
	erro("parametro invalido: ",argv[i]);
    }

    n1=read(f1,ibuf,MBUFSIZ);
    ibuf[n1]=NULX;
    iptr=ibuf;
    j=0;
    count=0;


    /*
        inicializa writbsiz
    */
    i2free=0; n2free=MBUFSIZ;
    obufsize=parmleng+1;        /* obufsize = entrada + SP's + LF */

    if (parmopc1)
	obufsize+=(1+4);


    /*
        loop principal
    */
    while (n1 > 0) {    /* enquanto nao for fim de arquivo */

        while (*iptr != NULX && *iptr != LF && j < parmleng)
            obuf[j++] = *iptr++;

        if (*iptr == NULX) {    /* rele buffer e continua o "move" */
            n1=read(f1,ibuf,MBUFSIZ);
            ibuf[n1]=NULX;
            iptr=ibuf;
        }
        else {
	    if (parmopc2)
		;
	    else
                while (n1 > 0 && *iptr != LF) {
                    while (*iptr != NULX && *iptr != LF)
                        iptr++;
                    if (*iptr == NULX) {
                        n1=read(f1,ibuf,MBUFSIZ);
                        ibuf[n1]=NULX;
                        iptr=ibuf;
                    }
                }
            if (*iptr == LF) {
                iptr++;
                while (j < parmleng)
                    obuf[j++]=SP;
		if (parmopc1) {
		    for (p=obuf+parmleng, n=parmleng; n; n--)
                        if (*--p != SP)
			    break;
		    sprintf(obuf+parmleng,"|%04d",n);
		}
                obuf[obufsize-1]=LF;
                writbsiz();
                count++;
                if (!(count % 1000))
                    fprintf(stderr,
                        "+++ %"_LD_" (%d bytes)\n",count,parmleng);
                j=0;
            }
        }
    }

    if (j != 0) {
        fprintf(stderr,"count:%"_LD_"\n",count);
        erro("ultimo registro nao tem LF em ",argv[2]);
    }

    breakout();

    fprintf(stderr,
        "fxl0: %"_LD_" lines fixed\n",count);

    exit(0);
}


erro(s1,s2)     /* imprime mensagem de erro e morre */
char *s1,*s2;
{
    fprintf(stderr,
        "fxl0: %s%s\n",s1,s2);
    breakout();
    exit(1);
}



writbsiz()                                                          /*
---------                                                           */

{
    int n2;
    unsigned char *p,*q;
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


breakout()                                                      /*
---------                                                       */

{
    int n2;

/*+++
    return;
+++*/

    if (i2free) if (f2) {
        n2=write(f2,bufout,i2free);
        if (n2 < i2free) {
            fprintf(stderr,"breakout - erro na gravacao\n");
	    exit(1);
	}
    }
}

