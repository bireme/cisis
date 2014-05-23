/* ifupd.c - Update an inverted file
*/

#include <stdio.h>
#include <string.h>

#include "cisis.h"
#include "ciupi.h"
#include "cirun.h"

char *fstarea=NULL;
char *stwp=NULL;
int nstws;

FST_CODE *fstpgmp;
LONGX maxlk1=1000;
LONGX maxlk2=500;


void main(argc,argv)
int argc;
char *argv[];

{
    char *dbnp,*ifnp;
    DBXSTRU *dbxp;
    char *p,*q,qline[CIMPL+1];
    int argnext=1;
    int parmtrace=0;
    LONGX parmfrom,parmto,parmcount,tell,rangtell,mfn;
    int pstflag=IFUPISIS;
    int rc=0;

#if LIND
    LONGX parmxmfn=0L;
    char *parmlogp=NULL;
#endif

    if (argc < 2) {
#if LINDLUX	    
        printf("%s",cicopyr("Utility IFUPD/LINDLUX"));
#else
	printf("%s",cicopyr("Utility IFUPD"));
#endif
	printf("\n");
        printf("ifupd ");
#if MULTI
        printf("[mono|full] ");
#endif  
	printf("[create=]<ifname> [<option> [...]]\n");
        printf(" \n");
#if MULTI
        printf(" [mono|full]              -> single/multi user operation \n");
#endif  
        printf(" [create=]                -> to delete+create <ifname> \n");
        printf(" <ifname>                 -> output inverted file \n");
        printf(" \n");
        printf("options: \n");
        printf("\n");
        printf(" fst=<fstspec>|@[fstfile] -> field select table \n");
        printf(" stw=<stwspec>|@[stwfile] -> stop words \n");
        printf(" -posts                   -> (init and) do not load .ifp \n");
#if !LIND
        printf(" master=<name>            -> alternate master file \n");
#else
        printf(" maxmfn=<n>               -> max master file number \n");
        printf(" master=<dbn>             -> master file to get maxmfn \n");
        printf(" log=<file>               -> log file name \n");
#endif
        printf(" actab=<file>             -> alphabetic chars table \n");
        printf(" uctab=<file>             -> upper case chars table \n");
        printf(" from=<n>                 -> initial mfn \n");
        printf(" to=<n>                   -> final mfn \n");
        printf(" count=<n>                -> max mfns \n");
        printf(" tell=<n>                 -> tell <n>%% loaded \n");
        printf(" \n");
/*
        printf("FMT=%"_LD_" \n",fst_fmtby);
        printf(" \n");
*/
        exit(1);
    }

    /* get dbn */
    dbnp=argv[argnext++];
#if MULTI
        if (strcmp(dbnp,"mono") == 0) {
            cipnetws=MONONETS;
	    dbnp=argv[argnext++];
        } else
        if (strcmp(dbnp,"mast") == 0) {
            cipnetws=MASTNETS;
	    dbnp=argv[argnext++];
        }
        if (strcmp(dbnp,"full") == 0) {
            cipnetws=FULLNETS;
	    dbnp=argv[argnext++];
        }
#endif
    if (strncmp(dbnp,"create?",7) == 0 || strncmp(dbnp,"create=",7) == 0) {
        dbnp+=7;
        trmisis0(dbnp);
    }
    ifnp=dbnp;

    parmfrom=1L; parmto=parmcount=9999999L; tell=0L;
    for (; argnext < argc; ) {

        p=argv[argnext++];

        if (strncmp(p,"reset?",6) == 0 || strncmp(p,"reset=",6) == 0) {
            if (sscanf(p+6,"%d",&ifupd_reset) != 1) fatal(p);
            continue;
        }
        if (strncmp(p,"master?",7) == 0 || strncmp(p,"master=",7) == 0) {
            dbnp=p+7;
            if (!*dbnp) fatal(p);
            continue;
        }

#if LIND
        if (strncmp(p,"maxmfn?",7) == 0 || strncmp(p,"maxmfn=",7) == 0) {
            if (sscanf(p+7,"%"_LD_,&parmxmfn) != 1) fatal(p);
            if (parmxmfn < 1) fatal(p);
            continue;
        }
        if (strncmp(p,"log?",4) == 0 || strncmp(p,"log=",4) == 0) {
            parmlogp=p+4;
            if (!*parmlogp) fatal(p);
            continue;
        }
#endif
        if (strncmp(p,"from?",5) == 0 || strncmp(p,"from=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&parmfrom) != 1) fatal(p);
            continue;
        }
        if (strncmp(p,"to?",3) == 0 || strncmp(p,"to=",3) == 0) {
            if (sscanf(p+3,"%"_LD_,&parmto) != 1) fatal(p);
            continue;
        }
        if (strncmp(p,"count?",6) == 0 || strncmp(p,"count=",6) == 0) {
            if (sscanf(p+6,"%"_LD_,&parmcount) != 1) fatal(p);
            continue;
        }
        if (strncmp(p,"tell=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&tell) != 1) fatal(p);
            if (tell < 0) fatal(p);
            continue;
        }
        if (strcmp(p,"trace?all") == 0|| strcmp(p,"trace=all") == 0) {
            dbxtrace=rectrace=parmtrace=fsttrace=multrace=trmtrace=1;
            continue;
        }
        if (strcmp(p,"trace") == 0) {
            dbxtrace=rectrace=multrace=parmtrace=1;
            continue;
        }

        if (strncmp(p,"maxlk1?",7) == 0 || strncmp(p,"maxlk1=",7) == 0) {
            if (sscanf(p+7,"%"_LD_,&maxlk1) != 1) fatal(p);
            continue;
        }
        if (strncmp(p,"maxlk2?",7) == 0 || strncmp(p,"maxlk2=",7) == 0) {
            if (sscanf(p+7,"%"_LD_,&maxlk2) != 1) fatal(p);
            continue;
        }

        if (strncmp(p,"fst?",4) == 0 || strncmp(p,"fst=",4) == 0) {
            q=p+4;
            if (strcmp(q,"@") == 0) sprintf(q=qline,"@%s.fst",ifnp);
            fstarea=loadfile(ifnp,'@',q,fstarea,0L,'\n');
	    if (!fstarea) fatal(q);
            if (parmtrace) { showcore("+++ fst"); printf("%s.\n",fstarea); }
            fst_gener(&fstpgmp,fstarea);
            if (parmtrace) showcore("+++ fst");
            if (fst_error) {
                printf("\n*** FST error#%"_LD_" at line %"_LD_"\n",fst_error,fst_errl);
                if (fst_error == FSTERRF)
                    printf("\n*** FMT error#%"_LD_" at offset %"_LD_"\n",
                                                fmt_error,fmt_errof);
                fatal(fst_errp);
            }
            continue;
        }

        if (strncmp(p,"stw?",4) == 0 || strncmp(p,"stw=",4) == 0) {
            q=p+4;
            if (*q++ != '@') fatal(p);
            if (!*q) sprintf(q=qline,"%s.stw",ifnp);
            if (parmtrace) showcore("+++ stw");
            stwp=loadstw(ifnp,q,stwp,0L,&nstws);
            if (parmtrace) {
                for (q=stwp; *q; q+=LE1+1) printf("%s/",q);
                printf("=%d\n",nstws);
                showcore("+++ stw");
            }
            continue;
        }

        if (strcmp(p,"-posts") == 0) {
            pstflag=IFUPDICT;
            continue;
        }

        if (strncmp(p,"uctab?",6) == 0 || strncmp(p,"uctab=",6) == 0) {
            if (!loaductb(ifnp,isisuctab,p+6)) fatal(p); /* AOT, 02/04/2001 */
            continue;
        }
        if (strncmp(p,"actab?",6) == 0 || strncmp(p,"actab=",6) == 0) {
            unsigned char ubuffup[256];
            memset(isiswctab,0x00,256);
            if (!loadactb(ifnp,ubuffup,p+6)) fatal(p); /* AOT, 02/04/2001 */
            for (isiswctot=0; ubuffup[isiswctot]; isiswctot=isiswctot+1)
                isiswctab[ubuffup[isiswctot]]=1;
            continue;
        }

        fatal(p);
    }

#if LIND
    /* get maxmfn */
    if (!parmxmfn) {
        parmxmfn=VMF0nxtmfn(record(nrecs,dbnp,0L))-1;
        dbxflush(dbnp);
    }
#endif

   dbxp=dbxstorp(ifnp); /* inverted file ! */

   if (parmto == 9999999L) {
        parmto=VMF0nxtmfn(record(nrecs,dbnp,0L))-1;
   }
   if (parmto > parmfrom+parmcount-1) parmto=parmfrom+parmcount-1;

   if (tell) {
        rangtell=parmto-parmfrom+1;
        if (rangtell <= 0) rangtell=1;
	if (tell < 0 || tell > 100) tell=1;
        tell=rangtell/(100/tell);
        if (tell <= 0) tell=1;
   }


   if (tell) { DBXitell=tell; DBXirang=rangtell; }
   if (parmtrace) DBXitrac=parmtrace;

#if LIND
   invsetup(ifnp,0L,0L,0L);
#if LINDLUX
   DBXifmap->ltxthresh=1;               /* noderead()/nodewrit()           */
   DBXifmap->luxthresh=parmxmfn/8+1;    /* leafread()/leafwrit()/threshold */
   DBXifmap->lvxiflind=0;               /* ifupdat()                       */
   DBXifmap->lvxmaxmfn=parmxmfn;        /* ifupdat()                       */
#endif /* LINDLUX */
#endif

#if MULTI
   if (DBXnetws != MONONETS)
       for (mfn=parmfrom; mfn <= parmto; mfn++) {
           rc=ifupdat(dbnp,mfn,mfn,
                        ifnp,fstpgmp,stwp,maxlk1,maxlk2,pstflag,IFUPWRIT);
	   if (rc) break;
       }
   else {
       rc=ifupdat(dbnp,parmfrom,parmto,
                        ifnp,fstpgmp,stwp,maxlk1,maxlk2,pstflag,IFUPCORE);

       rc+=ifupdat(dbnp,0L,0L,
                        ifnp,fstpgmp,stwp,maxlk1,maxlk2,pstflag,IFUPWRIT);
   }
#else
   rc=ifupdat(dbnp,parmfrom,parmto,
                        ifnp,fstpgmp,stwp,maxlk1,maxlk2,pstflag,IFUPCORE);

   rc+=ifupdat(dbnp,0L,0L,
                        ifnp,fstpgmp,stwp,maxlk1,maxlk2,pstflag,IFUPWRIT);
#endif

#if !LIND /* v */
if (tell) {						/* 02/06/98 */
   printf("%"_LD_" records updated          \n",DBXirecs);
   printf("%"_LD_"+%"_LD_"=%"_LD_" postings added   \n",DBXipadd[0],DBXipadd[1],
                                            DBXipadd[0]+DBXipadd[1]);
   printf("%"_LD_"+%"_LD_"=%"_LD_" postings deleted \n",DBXipdel[0],DBXipdel[1],
                                            DBXipdel[0]+DBXipdel[1]);
}					    
#else /* !LIND v */
   printf("%"_LD_" records read             \n",DBXirecs);
   printf("%"_LD_"+%"_LD_" records on %s+%s     \n",DBXifmap->cn[0].nmaxpos,
					    DBXifmap->cn[1].nmaxpos,
					    nx12extp[0],nx12extp[1]);
   printf("%"_LD_"+%"_LD_" records on %s+%s     \n",DBXifmap->cn[0].fmaxpos,
					    DBXifmap->cn[1].fmaxpos,
					    lx12extp[0],lx12extp[1]);
   printf("%"_LD_"+%"_LD_"=%"_LD_" postings to add  \n",DBXipadd[0],DBXipadd[1],
                                            DBXipadd[0]+DBXipadd[1]);
#endif /* !LIND v */

#if LIND
  if (pstflag != IFUPDICT) {
#if LINDLUX
   DBXifmap->ltxthresh=1;               /* noderead()/nodewrit()           */
   DBXifmap->luxthresh=parmxmfn/8+1;    /* leafread()/leafwrit()/threshold */
   DBXifmap->lvxiflind=1;               /* svdifupd()                      */
#endif /* LINDLUX */

   ifupdat(dbnp,parmfrom,parmto,
                        ifnp,fstpgmp,stwp,maxlk1,maxlk2,pstflag,IFUPCORE);

   ifupdat(dbnp,0L,0L,
                        ifnp,fstpgmp,stwp,maxlk1,maxlk2,pstflag,IFUPWRIT);

   printf("%"_LD_" records read             \n",DBXirecs);
   printf("%"_LD_"+%"_LD_"=%"_LD_" postings added   \n",DBXipadd[0],DBXipadd[1],
                                            DBXipadd[0]+DBXipadd[1]);
  } /* end if pstflag != IFUPDICT */
#endif

  if (rc) fatal("error while updating the IF");
  
  exit(0);
}

