/* ------------------------------ app0.c ----------------------------- */

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
    char *filioxp,*filnamp,*p;
    int fd0=0,fd1,i;
    off_t skip;
    unsigned int nread,blksize=BUFSIZ;
    char *bufferp=NULL;

    if (argc < 2) {
        printf("%s",cicopyr("Utility AOT/APP0"));
        printf("\n");
        printf("app0 [create=]<filapp> <fil1>[,skip] <fil2>[,skip] ...");
        printf(" [blksize=<%d>]\n",blksize);
        printf("\n");
        exit(1);
    }

    /* get parms */
    filioxp=argv[1];
    filioxp=dbxopenc(filioxp,filioxp,&fd0,NULL,filioxp,0,0); /*..do not force create= */
    printf("%s=%"P_OFF_T" bytes\n",filioxp, (LONG_LONG)LSEEK64(fd0,0L,SEEK_END));

    for (i=2; i < argc; i++) {
	if (strncmp(argv[i],"blksize=",8) == 0) {
	    blksize=atol(argv[i]+8);
	    printf("blksize=%"_LD_"\n",(LONGX)blksize);
	    if (blksize <= 0 || blksize > ALLOMAXV) fatal(argv[i]);
	    if (bufferp) { FREE(bufferp); bufferp=NULL; }
	    continue;
	}
	if (!bufferp) {
	    bufferp=(char *)ALLOC((ALLOPARM)blksize);
	    if (bufferp == (char *)ALLONULL) fatal(argv[i]);
	}
	filnamp=argv[i];
	p=strchr(filnamp,',');
	if (p) {*p='\0'; skip=atol(p+1);} else skip=0;
	fd1=dbxopen("",filnamp,"");
	printf("%s=%"P_OFF_T" bytes.. ",filnamp, (LONG_LONG)LSEEK64(fd1,0L,SEEK_END));
	LSEEK64(fd1,skip,SEEK_SET);
	while ((nread=read(fd1,bufferp,blksize /*41*/)) > 0)
	    if (write(fd0,bufferp,nread) != nread) fatal("write");
	close(fd1);
	printf("%s=%"P_OFF_T" bytes\n",filioxp,(LONG_LONG)LSEEK64(fd0,0L,SEEK_END));
    }

    close(fd0);
    exit(0);
}
