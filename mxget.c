/* ------------------------------ mxget.c ---------------------------------- */

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "cisis.h"   /* CISIS Interface header file */
#include "cirun.h"   /* CISIS Interface runtime declarations */


void mxsyntax(void);
void mxsyntax()
{
        printf("%s",cicopyr("Utility MXGET"));
        printf("\n");
        printf("mxget [cipar=<file>] <fmt_spec> \n");
        printf("\n");
}


#define cistderr stderr


void main(argc,argv)
int argc;
char *argv[];

{
    RECSTRU *recp; /* mandatory for defines REC RDB MF0 MFR DIR, FIELDP */

    int iarg;
    long rc;

    char *cgipfxp="tag";
    UWORD cgitag=2000;

    /* int cicgi0(int *argc, char *argv[], char **wwwp, UWORD taghxsub, char *taghpfxp); */
    char *wwwp;
    char *xp,*p,*q;
    long wwrec1,wwrec2;
    char *cgispecp;

    if (argc < 2) {
        mxsyntax();
        exit(1);
    }

        RECORD(wwrec1=nrecs,"null",1L); RECrc=RCNORMAL; MFRstatus=ACTIVE;
        RECORD(wwrec2=nrecs,"null",1L); xp=(char *)MFX; /* current */
        for (iarg=1; iarg < argc; iarg++) {

                    cgispecp=argv[iarg];
                    wwwp=NULL; cicgi0(NULL, NULL, &wwwp, cgitag, cgipfxp);
                    if (!wwwp) wwwp="";
                    sprintf(xp,"D%"_LD_,(long)cgitag); fldupdat(wwrec1,xp);
                    p=fldupdat(wwrec1,wwwp); if (p) fatal(p);
                    rc=recfmt(wwrec1,RECnbytes,cgispecp,p=xp,RECnbytes);
                    if (rc < 0) fatal(cgispecp);
                    while (*p) {
                        for (q=p; *p; p++)
                           if (*p == '\r' || *p == '\n') {
                               *p++='\0'; if (*p == '\n') *p++='\0'; break;
                           }
                        printf("%s\n",q);
                    }
                    break;

        } /* end for */

    exit(0);
}
