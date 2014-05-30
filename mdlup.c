/* ----------------------------------------------------------------------
   mdlup.c    -   NLM Medlars II - Master file update
----------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>

#include "cisis.h"       /* CISIS Interface */
#include "cirun.h"       /* runtime area and defines by AOT */

int argnext = 1;

LONGX parmfrom=1,parmto=0,parmloop=1;
LONGX parmcount=999999999L, parmtell=0;


#define NLMTAGUI        350     /* A020                      */
#define NLMTAGCU        102     /* A900                      */

#define NLMTAG18         18     /* UI                        */
#define NLMTAG56         56     /* ebcdic DD or ebcdic RR    */

#define EBCDIC_D        0xc4    /* Upper D */
#define EBCDIC_R        0xd9    /* Upper R */


unsigned char dbname[CIMPL+1],dbnupd[CIMPL+1],dbnout[CIMPL+1],dbnlog[CIMPL+1];
unsigned char *dbnamep,*dbnupdp,*dbnoutp,*dbnlogp;
unsigned char buffupd[BUFSIZ],buffup[MAXMFRL],*batchp;
LONGX irec,urec,outcrec,logcrec,logirec;
LONGX utrm;

LONGX count = 0;
LONGX countok = 0;
LONGX countdd = 0;
LONGX countrr = 0;
LONGX countcc = 0;

void main(argc,argv)
int argc;
char *argv[];
{
    RECSTRU *recp;
    TRMSTRU *trmp;
    LONGX mfn,data1,data2,umfn,umfn0;
    int iarg,xdir,rc;
    unsigned char c,*p,*q;
    unsigned char ui[8+1],uiasc[sizeof(ui)];
    unsigned char cu[8+1],cuasc[sizeof(cu)];

    argnext=1;
    if (argc < 5) {
        printf("%s",cicopyr("Utility MDL/MDLUP"));
        printf("\n");
        printf("mdlup <dbname> <dbnupd> [create=]<dbnout> <dbnlog> [options]' \n");
        printf("\n");
        printf("options: {from/to/loop/count/tell}=<n> \n");
        printf("\n");
        exit(1);
    }

    if (argc > argnext)
        dbnamep=argv[argnext++];
    else {
        printf("input data base:  "); dbnamep=gets(dbname);
    }
    if (argc > argnext)
        dbnupdp=argv[argnext++];
    else {
        printf("update data base: "); dbnupdp=gets(dbnupd);
    }
    if (argc > argnext)
        dbnoutp=argv[argnext++];
    else {
        printf("output data base: "); dbnoutp=gets(dbnout);
    }
    if (argc > argnext)
        dbnlogp=argv[argnext++];
    else {
        printf("log data base:    "); dbnlogp=gets(dbnlog);
    }


    for (iarg=argnext; iarg<argc; iarg++) {
        p=argv[iarg];
        if (strncmp(p,"from=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&parmfrom) != 1)
                fatal(p);
            continue;
        }
        if (strncmp(p,"to=",3) == 0) {
            if (sscanf(p+3,"%"_LD_,&parmto) != 1)
                fatal(p);
            continue;
        }
        if (strncmp(p,"loop=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&parmloop) != 1)
                fatal(p);
            continue;
        }

        if (!strncmp(p,"count=",6)) {
            if ((sscanf(p+6,"%"_LD_,&parmcount)) != 1)
                fatal(p);
            continue;
        }
        if (!strncmp(p,"tell=",5)) {
            if ((sscanf(p+5,"%"_LD_,&parmtell)) != 1)
                fatal(p);
            continue;
        }

        fatal(p);
    }


    /*
        setup bases de dados
    */

    /* entrada */
    RECORD((irec=nrecs),dbnamep,0L);
    prtcontrol(vrecp[irec],RDBname);
    if (!parmto)
        parmto=MF0nxtmfn-1;

    /* entrada update */
    RECORD((urec=nrecs),dbnupdp,0L);
    prtcontrol(vrecp[urec],RDBname);
    TERM((utrm=ntrms),dbnupdp,"!");

    /* saida */
    if (strncmp(dbnoutp,"create=",7) == 0) {
        printf("*%s\n",dbnoutp);
        recisis0((dbnoutp+=7));
    }
    RECORD((outcrec=nrecs),dbnoutp,1L);         /* recupdat()/xropn */
    RECORD(outcrec,dbnoutp,0L);
    prtcontrol(vrecp[outcrec],RDBname);

    /* log */
    RECORD((logcrec=nrecs),dbnlogp,0L);
    prtcontrol(vrecp[logcrec],RDBname);
    RECORD((logirec=nrecs),dbnlogp,1L);         /* recupdat()/xropn */

    /*
        loop principal
    */
    for (mfn=parmfrom; mfn<=parmto; mfn+=parmloop) {

        RECORD(irec,dbnamep,mfn);

        if (RECrc != RCNORMAL)
            continue;

        xdir=fieldx(irec,NLMTAGUI,1);
        if (xdir < 0 || DIRlen(xdir) >= sizeof(ui)) {
            printf("*** mfn=%"_LD_"  tag %d \n",mfn,NLMTAGUI);
            fatal("mdlup/UI");
        }
        memmove(ui,FIELDP(xdir),DIRlen(xdir)); ui[DIRlen(xdir)]='\0';
        for (p=ui, q=uiasc; *p; p++, q++) {
            c= *p; c&=0x0F; /* digit */
            c+='0'; *q=c;
        }
        *q='\0';
        TERM(utrm,dbnupdp,ui);
        if (TRMrc != RCNORMAL) {
/*
            printf("*** mfn=%"_LD_"  UI='%s'\n",mfn,uiasc);
            fatal("mdlup/TRMrc");
*/
                MFRmfn=VMF0nxtmfn(outcrec);     /* append */
                rc=recupdat(outcrec,irec);
                if (rc) {
                    printf("*** recupdat=%d\n",rc);
                    fatal("mdlup/recupdat/out");
                }
                countcc++;
        }
        if (TRMrc == RCNORMAL) {
            if (TRMxytotp == 0) {
                printf("*** mfn=%"_LD_"  UI='%s'\n",mfn,uiasc);
                fatal("mdlup/posting");
            }
            batchp=buffup;
            sprintf(batchp,"A1#%s# A2#%s^m%"_LD_"#",uiasc,dbnamep,mfn);
            batchp+=strlen(batchp);
            umfn=umfn0=posting(utrm,1L);
            for (data1= -1L; ; ) {
                RECORD(urec,dbnupdp,TRMpmfn);
                if (RECrc != RCNORMAL) {
                    printf("*** mfn=%"_LD_"/%"_LD_"='%s'\n",MFRmfn,mfn,uiasc);
                    fatal("mdlup/record");
                }
                xdir=fieldx(urec,NLMTAGCU,1);
                if (xdir >= 0) {
                    if (DIRlen(xdir) >= sizeof(cu)) {
                        printf("*** mfn=%"_LD_"  tag %d \n",mfn,NLMTAGCU);
                         fatal("mdlup/CU");
                    }
                    memmove(cu,FIELDP(xdir),DIRlen(xdir)); cu[DIRlen(xdir)]='\0';
                    for (data2=0L, p=cu, q=cuasc; *p; p++, q++) {
                        c= *p;
                        if ((c & 0xF0) != 0xF0) {
                            printf("*** mfn=%"_LD_"  UI='%s'\n",mfn,uiasc);
                            fatal("mdlup/invalid CU");
                        }
                        c&=0x0F; /* digit */
                        data2 = 10*data2 + c;
                        c+='0'; *q=c;
                    }
                    *q='\0';
                    sprintf(batchp,"A3#%"_LD_"^d%"_LD_"#",TRMpmfn,data2);
                    batchp+=strlen(batchp);
                    if (data2 >= data1) {
                        if (TRMpost > 1L)
                            printf("*** mfn=%"_LD_"  UI='%s'  %"_LD_"/%"_LD_" -> %"_LD_"/%s\n",
                                mfn,uiasc,umfn,data1,MFRmfn,cuasc);
                        data1=data2;
                        umfn=TRMpmfn;
                    }
                }
                else {
                    if (TRMpost > 1L)
                        printf("*** mfn=%"_LD_"  UI='%s'  %"_LD_"/%"_LD_" -> %"_LD_"\n",
                                mfn,uiasc,umfn,data1,MFRmfn);
                    sprintf(batchp,"A3#%"_LD_"#",TRMpmfn);
                    batchp+=strlen(batchp);
                    umfn=TRMpmfn;
                }
                if (posting(utrm,TRMpost+1) <= 0L)
                    break;
            }
            if (umfn != umfn0)
                RECORD(urec,dbnupdp,umfn);

            xdir=fieldx(urec,NLMTAG56,1);
            if (xdir < 0) {
                printf("*** mfn=%"_LD_"/%"_LD_"='%s'\n",MFRmfn,mfn,uiasc);
                fatal("mdlup/56 missing");
            }
            p=FIELDP(xdir);
            if (DIRlen(xdir) != 2 ||
                (*p != EBCDIC_D || *(p+1) != EBCDIC_D) &&
                (*p != EBCDIC_R || *(p+1) != EBCDIC_R)   ) {
                    printf("*** mfn=%"_LD_"/%"_LD_"='%s'\n",MFRmfn,mfn,uiasc);
                    fatal("mdlup/56 invalid");
            }

            if (*p == EBCDIC_R) {               /* REPLACE */
                sprintf(buffupd,"D%dD%d",NLMTAG18,NLMTAG56);
                p=fldupdat(urec,buffupd);
                if (p) {
                    printf("*** mfn=%"_LD_"  fldupdat='%s'\n",mfn,p);
                    fatal("mdlup/fldupdat/out");
                }
                MFRmfn=VMF0nxtmfn(outcrec);     /* append */
                rc=recupdat(outcrec,urec);
                if (rc) {
                    printf("*** recupdat=%d\n",rc);
                    fatal("mdlup/recupdat/out");
                }
                sprintf(batchp,"A4#%s^m%"_LD_"# A5#%s^m%"_LD_"#",
                    dbnupdp,umfn,dbnoutp,VMFRmfn(urec));
                batchp+=strlen(batchp);
                countrr++;
            }
            else {
                sprintf(batchp,"A4#%s^m%"_LD_"#",
                    dbnupdp,umfn);
                batchp+=strlen(batchp);
                countdd++;
            }
            RECORD(logirec,dbnlogp,VMF0nxtmfn(logcrec));
            MFRstatus=ACTIVE;
            if (fldupdat(logirec,buffup)) fatal("mdlup/fldupdat/log");
            rc=recupdat(logcrec,logirec);
            if (rc) {
                printf("*** recupdat=%d\n",rc);
                fatal("mdlup/recupdat/log");
            }
        }
        else countok++;

        count++;
        if (parmtell)
            if (!(count % parmtell))
              fprintf(stdout,
                    "+++ %"_LD_" recs - ok=%"_LD_" dd=%"_LD_" rr=%"_LD_" cc=%"_LD_"\n",
                        count,countok,countdd,countrr,countcc);

        if (count >= parmcount)
            break;
    }

    fprintf(stdout,
        "+++ %"_LD_" recs - ok=%"_LD_" dd=%"_LD_" rr=%"_LD_" cc=%"_LD_"\n",
            count,countok,countdd,countrr,countcc);

    exit(0);
}

