/* iflind.c - Load an inverted file from sorted link files
*/
#include <stdio.h>
#include <string.h>
#include "cisis.h"
#include "cirun.h"
extern int ifl_balan; /* cisis.h */
void main(argc,argv)
int argc;
char *argv[];
{
    char *dbnp,*fillk1p,*fillk2p,*rdbnp;
    int pstflag=IFUPISIS;
    LONGX parmxmfn=0L;
    char *parmxmfp=NULL;
    char *parmlogp=NULL;
    LONGX parmtell=0L;
    int parmtrace=0;
    int parmreset=0;
    int argnext=1;
    char *p;
    if (argc < 4) {
        printf("%s",cicopyr("Utility IFLIND"));
        printf("\n");
        printf("iflind <dbname> <file_lk1> <file_lk2> [<option> [...]] \n");
        printf("\n");
        printf(" <dbname>               -> output inverted file \n");
        printf(" <file_lk1>             -> sorted %2d-byte link file\n",LE1);
        printf(" <file_lk2>             -> sorted %2d-byte link file\n",LE2);
        printf("\n");
        printf("options: \n");
        printf("\n");
        printf(" -posts                 -> do not load .iyp \n");
        printf(" -balan                 -> do not rebalance the dict \n");
        printf(" maxmfn=<n>             -> max master file number \n");
        printf(" master=<dbn>           -> master file to get maxmfn \n");
        printf(" log=<file>             -> log file name \n");
        printf(" tell=<n>               -> tell <n> links has been loaded \n");
#if CIIFLFIX
        printf(" +fix[/m]               -> read fixed length link records \n");
#endif
        printf(" +reset[=<dbn>]         -> reset IF update is pending \n");
        printf(" \n");
        exit(1);
    }

    /* get positional parms */
    dbnp=argv[argnext++];
    fillk1p=argv[argnext++];
    fillk2p=argv[argnext++];
    /* get optional parms */
    rdbnp=dbnp;
    for (; argnext < argc; argnext++) {
        p=argv[argnext];
        if (strncmp(p,"maxmfn?",7) == 0 || strncmp(p,"maxmfn=",7) == 0) {
            if (sscanf(p+7,"%"_LD_,&parmxmfn) != 1) fatal(p);
            if (parmxmfn < 1) fatal(p);
            continue;
        }
        if (strncmp(p,"master?",7) == 0 || strncmp(p,"master=",7) == 0) {
            parmxmfp=p+7;
            if (!*parmxmfp) fatal(p);
            continue;
        }
        if (strncmp(p,"log?",4) == 0 || strncmp(p,"log=",4) == 0) {
            parmlogp=p+4;
            if (!*parmlogp) fatal(p);
            continue;
        }
        if (strncmp(p,"tell?",5) == 0 || strncmp(p,"tell=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&parmtell) != 1) fatal(p);
            if (parmtell < 0) fatal(p);
            continue;
        }
        if (strcmp(p,"-balan") == 0) {
            ifl_balan=0;
            continue;
        }
        if (strcmp(p,"-posts") == 0) {
            pstflag=IFUPDICT;
            continue;
        }
#if CIIFLFIX
        if (strcmp(p,"+fix") == 0) {
            ciiflfix=1;
            continue;
        }
        if (strcmp(p,"+fix/m") == 0) {
            ciiflfix=ciiflfim=1;
            continue;
        }
#endif
        if (strcmp(p,"+reset") == 0) {
            parmreset=1;
            continue;
        }
        if (strncmp(p,"+reset?",7) == 0 || strncmp(p,"+reset=",7) == 0) {
            rdbnp=p+7;
            if (!*rdbnp) fatal(p);
            continue;
        }
        if (strcmp(p,"trace") == 0) {
            parmtrace=1;
            dbxtrace=1; trmtrace=1;
            continue;
        }
        fatal(p);
    }
    /* get maxmfn */
    if (!parmxmfn) {
        if (!parmxmfp) parmxmfp=dbnp;
        parmxmfn=VMF0nxtmfn(record(nrecs,parmxmfp,0L))-1;
        dbxflush(parmxmfp);
    }
    
    /* should pause */
    if (parmtrace || parmtell) {
        printf("+++ create=%s \n",dbnp);
        printf("+++ maxmfn=%"_LD_"\n",parmxmfn);
    }
    if (parmtrace) printf("+++ coreleft=%"_LD_"\n",CORELEFT());
    /* initialize IF and load link files */
    svdiflind (dbnp, fillk1p ,fillk2p, parmxmfn, parmlogp, pstflag, parmtell);
    if (parmtrace) printf("+++ coreleft=%"_LD_"\n",CORELEFT());
    dbxflush(dbnp);
    if (parmtrace) printf("+++ coreleft=%"_LD_"\n",CORELEFT());
    if (parmreset) {
        recreset(rdbnp,1L,MAXUPDMFN,0L,parmtell);
    }
    exit(0);
}
