#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "cisis.h"
#include "cirun.h"

#define M0BSIZ  sizeof(M0STRU)

char line[CIMPL+1];

#if ANSI
LONGX get(char *msg,LONGX def,LONGX min,LONGX max);
#else
LONGX get();
#endif

void main(argc,argv)
int argc;
char *argv[];
{
    RECSTRU *recp;
    int fd;
    char *dbnp;
    off_t nbytes,nblocks;

    if (argc < 2) {
        printf("%s",cicopyr("Utility CTLMFN"));
        printf("\n");
        printf("ctlmfn <dbn> \n");
        printf("\n");
	exit(1);
    }

    dbxopt_ordwr=O_RDWR; fd=dbxopen(argv[1],dbnp=argv[1],mx1extp);

    recp=vrecp[recallok(0L,M0BSIZ)];

    if (read(fd,(char *)MFX,M0BSIZ) != M0BSIZ) fatal("read");

    prtcontrol(recp,dbnp);

    nbytes=LSEEK64(fd,0L,SEEK_END);
    nblocks=nbytes/MSBSIZ;
    if (nblocks * MSBSIZ != nbytes)
        printf("\nfile has %"P_OFF_T" bytes. %"P_OFF_T" bytes (%"P_OFF_T" blocks) considered\n",
            (LONG_LONG)nbytes,(LONG_LONG)(nblocks*MSBSIZ),(LONG_LONG)nblocks);

    MF0ctlmfn=0L;
    if (MF0nxtmfb <= 0 || MF0nxtmfb > nblocks) MF0nxtmfb = nblocks;
    if (MF0nxtmfp <= 0 || MF0nxtmfp > MSBSIZ-LEADER) MF0nxtmfp = MSBSIZ-LEADER;
    if (MF0mftype != 0 && MF0mftype != 1
        && MF0mftype != 256 && MF0mftype != 512 && MF0mftype != 768
    ) MF0mftype = 0;

    MF0nxtmfn=get("nxtmfn",MF0nxtmfn,1,MAXUPDMFN);
    MF0nxtmfb=get("nxtmfb",MF0nxtmfb,1,nblocks);
    MF0nxtmfp=get("nxtmfp",MF0nxtmfp,1,MSBSIZ);
    MF0mftype=get("mftype",MF0mftype,0,768);
    if (MF0mftype != 0 && MF0mftype != 1
        && MF0mftype != 256 && MF0mftype != 512 && MF0mftype != 768
    ) MF0mftype = 0;
    MF0reccnt=get("reccnt",MF0reccnt,0,LONGX_MAX);
    MF0mfcxx1=get("mfcxx1",MF0mfcxx1,0,LONGX_MAX);
    MF0mfcxx2=get("mfcxx2",MF0mfcxx2,0,LONGX_MAX);
    MF0mfcxx3=get("mfcxx3",MF0mfcxx3,0,LONGX_MAX);

    printf("\n");
    prtcontrol(recp,dbnp);
    printf("\nok ?  ");
    if (strcmp(gets(line),"ok")) fatal("ok expected");

    LSEEK64(fd,0L,0); /* rewind */
    if (CIWRITE(fd,(char *)MFX,M0BSIZ) != M0BSIZ) fatal("write");
    CLOSE(fd);
    exit(0);
}


LONGX get(msg,def,min,max)
char *msg;
LONGX def;
LONGX min;
LONGX max;
{
    LONGX val;
    static int last=1;

    if (last) printf("\n");

    for (; ;) {
        printf("%s [%"_LD_"]: ",msg,def);
        gets(line);
        if (line[0] == '\0') {
            val=def;
            last=0;
            break;
        }
        if (sscanf(line,"%"_LD_,&val) != 1) {
            printf("\007\n");
            continue;
        }
        if (val < min || val > max) {
            printf("\007[%"_LD_" - %"_LD_"]\n",min,max);
            continue;
        }
        last=1;
        break;
    }

    return(val);
}

