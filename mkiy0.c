/* ------------------------------ mkiy0.c ----------------------------- */

#include <stdio.h>
#include <string.h>

#define TRACE   0

#include "cisis.h" /* CISIS Interface */
#include "cirun.h" /* runtime area and defines by AOT */

/* main */
#if ANSI
void main (int argc, char *argv[])
#else
void main(argc,argv)
int argc;
char *argv[];
#endif
{
    int   fdy;
    int   vfdx[1+2+2+1];    /* .cnt, .n0x, .l0x, .ifp */
    char *vextp[1+2+2+1];   /* .cnt, .n0x, .l0x, .ifp */
    off_t vflen[1+2+2+1];   /* .cnt, .n0x, .l0x, .ifp */

    char *dbnp,*p;
    unsigned int nbyts;
    unsigned int blksize=SHRT_MAX+1; /* 32K */
    off_t lv;
    int i;
    int nfiles=6;
    int parmv=1;
    char *buffp=NULL;

    vextp[0]=cx1extp;
    vextp[1]=nx12extp[0];
    vextp[2]=nx12extp[1];
    vextp[3]=lx12extp[0];
    vextp[4]=lx12extp[1];
    vextp[5]=ix1extp;

    if (argc < 2) {
        printf("%s",cicopyr("Utility MKIY0"));
        printf("\n");
        printf("mkiy0 <dbn> [-ifp] [-v] [blksize=%"_LD_"]\n",(LONGX)blksize);
        exit(1);
    }

    /* get parms */
    dbnp=argv[1];

    for (i=2; i < argc; i++) {
        p=argv[i];
    if (strncmp(p,"blksize=",8) == 0) {
        blksize=atol(p+8);
        if (blksize <= 0 || blksize > ALLOMAXV) fatal(p);
        continue;
    }
    if (strcmp(p,"-v") == 0) {
        parmv=0;
        continue;
    }
    if (strcmp(p,"-ifp") == 0 || strcmp(p,"-iyp") == 0) {
        nfiles--;
        continue;
    }
    fatal(p);
    }

    nbyts=blksize; if (nbyts < BUFSIZ) nbyts=BUFSIZ;
    buffp=(char *)ALLOC((ALLOPARM)nbyts);
    if (buffp == (char *)ALLONULL) fatal("mkiy0/ALLOC");

    /* open individual I/F files */
    for (i=0; i < nfiles; i++) {
    vfdx[i]=dbxopen("",dbnp,vextp[i]);
    vflen[i]=LSEEK64(vfdx[i],0L,SEEK_END);
    LSEEK64(vfdx[i],0L,SEEK_SET);
    }

    /* create output file */
    sprintf(buffp,"create=%s%s",dbnp,iy0extp);
    dbxopenc(dbnp,buffp,&fdy,NULL,buffp,1,0); /*..force create= */

    /* write 6-line header */
    for (i=0; i < 6; i++) {
        lv=vflen[i];
    if (i == 5) if (nfiles == 5) lv=0; /* no .iyp */
    sprintf(buffp,"%012"P_OFF_T,(LONG_LONG)lv);
#if 1 /* CRLF*/
    memset(buffp+12,' ',CNBSIZ-12-2);
    buffp[CNBSIZ-2]='\r';
    buffp[CNBSIZ-1]='\n';
#else
    memset(buffp+12,' ',CNBSIZ-12-1);
    buffp[CNBSIZ-1]='\n';
#endif
    nbyts=CNBSIZ;
    if (CIWRITE(fdy,buffp,nbyts) != nbyts) fatal("mkiy0/write");
    }
    if (parmv) printf("%s%s=%12"P_OFF_T" bytes",dbnp,iy0extp,(LONG_LONG)LSEEK64(fdy,0L,SEEK_END));

    /* append I/F files */
    for (i=0; i < nfiles; i++) {
    memset(buffp,' ',strlen(dbnp)); buffp[strlen(dbnp)]='\0';
    if (parmv) printf("\n%s%s=%12"P_OFF_T,buffp,vextp[i],(LONG_LONG)vflen[i]);
    while ((nbyts=CIREAD(vfdx[i],buffp,blksize)) > 0)
        if (CIWRITE(fdy,buffp,nbyts) != nbyts) fatal("mkiy0/write");
        CLOSE(vfdx[i]);
    }
    if (parmv) printf("\n%s%s=%12"P_OFF_T" bytes\n",dbnp,iy0extp,(LONG_LONG)LSEEK64(fdy,0L,SEEK_END));

    CLOSE(fdy);

    exit(0);
}
