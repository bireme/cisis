#include <stdio.h>
#include "cisis.h"
#include "cirun.h"

void main(argc,argv)
int argc;
char *argv[];
{
    int i;
    RECSTRU *recp;
    LONGX irec=0L;
    int x;

    LONGX mfn,parmfrom=1,parmto=99999999, parmtell=0;

    if (argc < 2) fatal("rec1 <dbn> [<from>] [<to>] [<tell>]");

    for (i=0; i<argc; i++) {
        printf("parm#%d = %s \n",i,argv[i]);
    }
//    printf(".."); x=getchar();
//    if (x == 'x') exit(1);

    if (argc >= 3) parmfrom=atol(argv[2]);
    if (argc >= 4) parmto  =atol(argv[3]);
    if (argc >= 5) parmtell  =atol(argv[4]);

    for (mfn=parmfrom; mfn<=parmto; mfn++) {
        RECORD(irec,argv[1],mfn);
        if (RECrc == RCEOF) break;
        if (RECrc != RCNORMAL) continue;
        if (parmtell) {
           if (mfn%parmtell==0) printf("+++ mfn=%"_LD_"\n",mfn);
        }
        else prtfields(recp,MFRmfn);
    }

    exit(0);
}

