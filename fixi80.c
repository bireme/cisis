/*------------------------------------------------------------------------
   fixi80.c - deblock arquivos ISO/SERLINE em registros de 80 com [CR]LF

   uso: fixi80 <filin> <filout>

	<filin>:    arquivo de entrada
        <filout>:   arquivo de saida

   AOT, 27/04/99

----------------------------------------------------------------------- */

#include <stdio.h>
#include <fcntl.h>

#define PC    0

#define MODOP 0644  /* dono: LG, grupo: L, outros: L */
#if !PC
#define O_BINARY 0
#endif

#define IBUFSIZ 0x1FFF /* 8K */
#define OBUFSIZ 0x3FFF /* 16K */

unsigned char ibuf[IBUFSIZ+1];
unsigned char *iptr;

unsigned char bufout[OBUFSIZ];           /* writbsiz*/
unsigned char obuf1[OBUFSIZ];
unsigned char *obuf = obuf1;
int obufsize;
int f2,n2free,i2free=0;


main(argc,argv)

int argc;
char *argv[];

{
    int f1;
    int n1;

    int n,reclen;

    LONGX count,totbytes=0L,parmtell=1000L;


    if (argc != 3)
        erro("use 'fixi80 <filin> <filout>'","");

    if ((f1=open(argv[1],O_RDONLY|O_BINARY)) == -1)
        erro("impossivel abrir o arquivo ",argv[1]);

    if ((f2=open(argv[2],O_WRONLY|O_TRUNC|O_CREAT|O_BINARY,MODOP)) == -1)
        erro("impossivel criar o arquivo ",argv[2]);

    /* inicializa writbsiz */
    i2free=0; n2free=OBUFSIZ;
    count=0;

    /* le primeiro reclen */
    n1=read(f1,ibuf,5);


    /* loop principal */

    while (n1 == 5) {    /* enquanto reclen */

        count++; totbytes+=5;

        ibuf[5]='\0';
	if (sscanf(ibuf,"%d",&reclen) != 1) {
            printf("+++ %ld/%ld recs/bytes  (read=%d)\n",count,totbytes,n1);
	    erro("reclen invalido - ","nao numerico");
	}
	if (reclen >= IBUFSIZ) {
            printf("+++ %ld/%ld recs/bytes  (read=%d)\n",count,totbytes,n1);
	    erro("reclen invalido - ","muito grande");
	}

	n1+=read(f1,ibuf+5,(reclen-5));
	if (n1 != reclen) {
	    printf("+++ %ld recs/bytes lidos %d bytes)\n",count,totbytes,n1);
	    erro("reclen invalido - ","ou arquivo truncado ");
	}
	totbytes+=(reclen-5);

	for (iptr=ibuf, obufsize=0; n1; n1-=n, iptr+=n) {
	
	    if (n1 < 80) n=n1; else n=80;
	    
	    memmove(obuf+obufsize,iptr,n); obufsize+=n;
#if PC
	    memmove(obuf+obufsize,"\r",1); obufsize+=1;
#endif
	    memmove(obuf+obufsize,"\n",1); obufsize+=1;
	}
	
	writbsiz();

        if (parmtell) 
	    if (!(count % parmtell))
                printf("+++ %ld/%ld recs/bytes \n",count,totbytes);

        n1=read(f1,ibuf,5);
    }

    if (n1 && n1 != EOF) {
	printf("+++ %ld/%ld recs/bytes  (read=%d)\n",count,totbytes,n1);
        erro("EOF inesperado","");
    }

    breakout();

    printf("fixi80: %ld/%ld recs/bytes \n",count,totbytes);

    exit(0);
}


erro(s1,s2)     /* imprime mensagem de erro e morre */
char *s1,*s2;
{
    printf("fixi80: %s%s\n",s1,s2);
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
        n2=write(f2,bufout,OBUFSIZ);
        if (n2 < OBUFSIZ)
            erro("writbsiz - ","erro na gravacao");
        i2free=obufsize-n2free;
/*0     strncpy(bufout,&obuf[n2free],i2free); */
	for (p=bufout, q=obuf+n2free, loop=i2free; loop--; )  *p++ = *q++;
        n2free=OBUFSIZ-i2free;
    }
}


breakout()                                                      /*
---------                                                       */

{
    int n2;

/*+++
    return;
+++*/

    if (i2free) {
        n2=write(f2,bufout,i2free);
        if (n2 < i2free) {
            printf("breakout - erro na gravacao\n");
	    exit(1);
	}
    }
}

