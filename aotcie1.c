/* =============================================================================
   AOTCIE1     CIE-10
               Application 1
               by AOT, 05/01/2001

   aotcie1.c   CI/CIE-10 simple browser..
   -----------------------------------------------------------------------------

   aotcie1 <action>[,...] [<option> [...]]

      actions:   C  - get parameters from cgi
                 1  - display header MDL:

      options:  bool=<expr>
                cip=<>                           c:/cie/rod/cip.cip
                dcie=<>                          aotcie1A
                pft=<>                           aotcie1A.pft
   -----------------------------------------------------------------------------
============================================================================= */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "cisis.h"          /* CISIS Interface header file */
#include "cirun.h"          /* CISIS Interface AOT's runtime declarations */

#include "cib70.h"          /* CISIS Interface search engine header file */

#define ICDTAG_E     2      /* ICDSERVER tag 2 = name of cap/agr/cat3/cat4 */

/* prototypes */
#if ANSI
char *recsfldv(char *areap, char *fldp, FFI fldlen, char *defaultp);
#else
char *recsfldv();
#endif

void main(argc,argv_main)
int argc;
char *argv_main[];
{
    void *ciapip = NULL;
    b7_CIB7 *cib7p=NULL;

#define MAXARGS 512
    char *argv[MAXARGS+1];

    RECSTRU *recp;
    int i,argnext=1;
    unsigned char *p,*q;
    int parmtrace=0;

/* -------------------------------------------------------------------------- */
    char *cipfnamp=NULL;
    char *dciedbnp=NULL;
    char *pftfnamp=NULL;
    char *langp=NULL;
    char *boolqryp=NULL;     /* bool= */
    char *actionp=NULL;

    int   actionCG=0;
    int   actionDD=0;
    int   actionTA=0;
    int   actionDA=0;
/* -------------------------------------------------------------------------- */

#if CICGI
    char *cgipfxp="tag";
    UWORD cgitag=2000;
    char *wwwp=NULL;
    LONGX wrec;
#endif

    /* here we go..
    */
#if CIAPI
    ciapip = cisisApplication( MAXNREC,MAXNTRM );/* mandatory for CIAPI definitions */
#endif /* CIAPI */

    /* copy program's argv[] */
    for (i=0; i < argc; i++) argv[i]=argv_main[i];

    /* get CGI data
    */
#if CICGI
    RECORD(wrec=nrecs,"null",1L); RECrc=RCNORMAL; MFRstatus=ACTIVE;
    wwwp=NULL; cicgi0(&argc, argv, &wwwp, cgitag, cgipfxp);

    if (wwwp) {

      DIRSTRU *dirp;
      int xdir,iocc,loop;
      if (fldupdat(wrec,wwwp)) fatal(wwwp);

      /* check if trace is present */
      for (parmtrace=0, dirp=MFRdir, xdir=0, loop=MFRnvf; loop--; dirp++, xdir++)
        if (dirp->tag == cgitag) {
          p=FIELDP(xdir);
          if (strncmp(p,"^ntrace^v",2+5+2) == 0) {
              parmtrace=1;
              if (parmtrace) {
                  printf("Content-type: text/plain\n\n");
                  printf("+++trace: \n");
                  break;
              }
          }
        }

      /* check if cgi is enabled */
      for (actionCG=0, dirp=MFRdir, xdir=0, loop=MFRnvf; loop--; dirp++, xdir++)
        if (dirp->tag == cgitag) {
          p=FIELDP(xdir);
          if (strncmp(p,"^naction^vC",2+6+2+1) == 0) {
              actionCG=1;
              if (parmtrace) printf("+++trace: action='%.11s'\n",p);
              break;
          }
        }

      if (actionCG) { /* cgi is enabled */

      for (dirp=MFRdir, xdir=iocc=0, loop=MFRnvf; loop--; dirp++, xdir++)
        if (dirp->tag == cgitag) {
          FFI plen;
          p=FIELDP(xdir); plen=dirp->len; iocc++;

          /* get CGI by name
          */
          if (strncmp(p,"^ncip^v"    , 3+4) == 0)   cipfnamp  =recsfldv(NULL,p,plen,"");
          if (strncmp(p,"^ndcie^v"   , 4+4) == 0)   dciedbnp  =recsfldv(NULL,p,plen,"");
          if (strncmp(p,"^npft^v"    , 3+4) == 0)   pftfnamp  =recsfldv(NULL,p,plen,"");
          if (strncmp(p,"^nlang^v"   , 4+4) == 0)   langp     =recsfldv(NULL,p,plen,"");
          if (strncmp(p,"^nbool^v"   , 4+4) == 0)   boolqryp  =recsfldv(NULL,p,plen,"");
          if (strncmp(p,"^naction^v" , 6+4) == 0)   actionp   =recsfldv(NULL,p,plen,"");

        } /* end if (dirp->tag == cgitag) */
    } /* end if (cgi is enabled */
    } /* end if (wwwp) */
#endif /* CICGI */


    /* set default values
    */
    if (!cipfnamp)    cipfnamp    = "c:/cie/rod/cip.cip";
    if (!dciedbnp)    dciedbnp    = "aotcie1A";
    if (!pftfnamp)    pftfnamp    = "aotcie1A.pft";
    if (!langp)       langp       = "p";
    if (!actionp)    {actionCG    = 0;
                      actionDD    = 0;
                      actionTA    = 0;
                      actionDA    = 0; }

    if (argc < 2 && !wwwp) {
        printf("%s",cicopyr("Application AOTCIE1"));
        printf(" \n");
        printf("aotcie1 <action>[,...] [<option> [...]] \n");
        printf(" \n");
        printf("actions:  C   - get parameters from cgi \n");
        printf("          1   - search CIE-10 data base \n");
        printf("          TA  - search amdl \n");
        printf("          DA  - display amdl \n");
        printf(" \n");
        printf("options: bool=<expr>  \n");
        printf(" \n");
        printf("         cip    ,                              %s \n",  cipfnamp    );
        printf("         dcie   ,                              %s \n",  dciedbnp    );
        printf("         pft    ,                              %s \n",  pftfnamp    );
        printf("         lang   ,                              %s \n",  langp       );
        printf("         bool   ,                              %s \n",  boolqryp    );
        printf("         action ,                              %s \n",  actionp     );
        printf(" \n");
        exit(1);
        }


    /*
        get options
    */
    for (; argnext < argc; argnext++) {
        p=argv[argnext];
        if (strncmp(p,"cip?",4) == 0 || strncmp(p,"cip=",4) == 0) {
            cipfnamp=p+4;
            continue;
        }
        if (strncmp(p,"dcie?",5) == 0 || strncmp(p,"dcie=",5) == 0) {
            dciedbnp=p+5;
            continue;
        }
        if (strncmp(p,"pft?",4) == 0 || strncmp(p,"pft=",4) == 0) {
            pftfnamp=p+4;
            continue;
        }
        if (strncmp(p,"lang?",5) == 0 || strncmp(p,"lang=",5) == 0) {
            langp=p+5;
            continue;
        }
        if (!strncmp(p,"bool=",5)) {
            if (boolqryp) fatal(p); q=p+5;
            boolqryp=loadfile(NULL,'@',q,NULL,0L,'\n');
            if (!boolqryp) fatal(p);
            continue;
        }
        if (strncmp(p,"action?",7) == 0 || strncmp(p,"action=",7) == 0) {
            actionp=q=p+7;
            if (!*q) fatal(p);
            continue;
        }
        if (strcmp(p,"trace") == 0) {
            parmtrace=1;
            continue;
        }
        fatal(p);
    }


    /* actions */
    if (actionp) {
      for (q=actionp; *q; ) {
      
         if (*q == 'C' && strncmp(q,"C" ,1) == 0)
                   { /*actionCG=1;*/                  if (*(q+=1) == ',') q++; continue; }
                   
         if (isdigit(*q))
                   {   actionDD=(int)(*q)-(int)'0';   if (*(q+=1) == ',') q++; continue; }
                   
         if (*q == 'T' && strncmp(q,"TA",2) == 0)
                   {   actionTA=1;                    if (*(q+=2) == ',') q++; continue; }
                   
         if (*q == 'D' && strncmp(q,"DA",2) == 0)
                   {   actionDA=1;                    if (*(q+=2) == ',') q++; continue; }
                   
         if (*q) fatal(p);
      }
    }
    

if (parmtrace) {
  printf("+++trace: actionp=%s\n",actionp);
  printf("+++trace: actionDD=%d\n",actionDD);
  printf("+++trace: actionTA=%d\n",actionTA);
  printf("+++trace: actionDA=%d\n",actionDA);
}


    /* Common setup
    */

    /* set cipar */
    if ((dbxcipfp=fopen(cipfnamp,"r")) == NULL) fatal(cipfnamp);
    /* rec_maxmfrl */
    p=dbxcipar(NULL,"maxmfrl",'='); if (dbxcipok) { sscanf(p,"%"_LD_,&rec_maxmfrl); }

/* =======================         =============          ================== */
/* =======================         =============          ================== */

if (actionDD) {

#define ENVASIZE  BUFSIZ*10         /* up to 10 512-byte parms */

    char envarea[ENVASIZE];
    LONGX boolridx=0L;
    LONGX boolhits=0;
    LONGX boolh;
    LONGX booltell=0L;               /* 0L = no msg; 1L = tell; 2L = tell+root */
    LONGX booltlir=0L;               /* b70 invx */

    B7HITLIST *hitlistp;            /* hit processing */
    LONGX *mfnlistp;                 /* hit processing */

    LONGX count1=0L;                /* registros lidos */
    LONGX count2=0L;                /* casos tabulados */

    LONGX drec=0L;

    /* ---------------------------------------------------------------------- */

    LONGX dcienxtmfn;
    int errno=0;
    int errno2=0;

    /* Search ICDSERVER/aotcie1A
    */

    /* init reading */
    if (!drec) drec=nrecs;
    RECORD(drec,dciedbnp,0L);
    dcienxtmfn=MF0nxtmfn;

    /*
        Search "bool=" parameter
    */
    if (boolqryp) { /* mxbool/boolh */

        char *p,*p2=NULL;
        LONGX mfn;

        LONGX asize=sizeof(dciedbnp)+2*sizeof(boolqryp)+20*B7UPLEN+BUFSIZ;
        char *buffup=loadfile(NULL,'@',"",NULL,asize,'\0');

        if (!cib7p) cib7p=b7_cib7(cib7p,ciapip);

        /* initialize ridx and b7_xxx() parameters and run b7_run()
        */
        for (boolridx=nrecs, mfn=1; ; mfn++) { /* get actual maxmfn */
            RECORD(boolridx,dciedbnp,mfn);
            if (RECrc == RCEOF) fatal(dciedbnp);
            if (RECrc == RCNORMAL) break;
        }
        RECdbxp=NULL;                /* nao usa base de log */
        MFRmfn=0L;

        /* run b7_exp() / b7_run() / b7_hit(0)
        */
        p=b7_exp(cib7p,boolridx,dciedbnp,boolqryp,buffup,NULL,0L,&errno);
        if (p || errno) ; /* printf("Expression syntax error %d: '%s'\n",errno,p); */
        else {
            LONGX bufsiz;
            char *hitfnamp="null";
            sprintf(buffup,"D%dA%d|%s|",MFQTHMEM,MFQTHMEM,hitfnamp);
            if (fldupdat(boolridx,buffup) != NULL) fatal("aotcie1/DD/fldupdat/boolridx");

            /* br_run faz "trirecp=vrep[tlirec]" */
            if (!booltlir) record(booltlir=nrecs,"null",1L);

            /* alloc buffer for 20000 hits = 80Kbytes */
            bufsiz=20000*sizeof(LONGX);
            if (b7bufferp) b7_hbfree(cib7p,&b7bufferp);
            b7_hballoc(cib7p,bufsiz,&b7bufferp);

            /* run search */
            p2=b7_run(cib7p,boolridx,dciedbnp,buffup,sizeof(buffup),
                booltell,isisuctab,""/*btch0p*/,booltlir,0L,&errno2);
            if (p2 || errno2) ; /* printf("Execution error %d: '%s'\n",errno2,p2); */
            else {
                boolhits=b7bufferp->ndocs; /* b7_hit(cib7p,boolridx,0L,b7setno); */
                if (booltell) printf("Hits=%"_LD_"\n",boolhits);
            }
        }
        if (p2 || errno2) fatal("Expression execution error");

        /* set# */
        count2=boolhits;

    } else { /* mxbool/boolh */

        count2=1;

    } /* mxbool/boolh */

    if (errno || errno2) {
    }
    else { /* errno */
#define FMTASIZE 32000
        FMT_CODE *tabfmtp=NULL;           /* formato para tab= */
        char fmtspec[BUFSIZ];
        char *batchp;
        char fmtarea[FMTASIZE+1];
        LONGX fmtasize=FMTASIZE;

        count1=0;

        /* environment */
        batchp=envarea;
        if (cipfnamp   ) { sprintf(batchp,"a3000|%s|" , cipfnamp    ); batchp+=strlen(batchp); }
        if (dciedbnp   ) { sprintf(batchp,"a3001|%s|" , dciedbnp    ); batchp+=strlen(batchp); }
        if (pftfnamp   ) { sprintf(batchp,"a3005|%s|" , pftfnamp    ); batchp+=strlen(batchp); }
        if (langp      ) { sprintf(batchp,"a3006|%s|" , langp       ); batchp+=strlen(batchp); }
        if (actionp    ) { sprintf(batchp,"a3014|%s|" , actionp     ); batchp+=strlen(batchp); }
        /* end of envarea */

        /* compile strip CR if CRLF + display format */
        pftfnamp="aotcie1D.pft";
        sprintf(fmtspec,"%s,@%s","if size(s(#))=2 then newline(s(#)*1) fi",pftfnamp);
        if (fmt_gener(&tabfmtp,fmtspec)) {
            printf("Content-type: text/plain\n\n");
            printf("error: %s [format error %d:%s]\n",pftfnamp,fmt_error,fmtspec+fmt_errof);
            exit(12);
        }


        /*
            loop dcie
        */

        for (boolh=1; ; boolh++) {

            LONGX mfn;

            /* hit processing */
            if (boolhits) {

                if (boolh > boolhits) break;
                
                if (boolh == 1) {
                    hitlistp=(B7HITLIST *)b7bufferp->area;
                    mfnlistp=hitlistp->mfnlist;
                }
                if (b7bufferp->hcase == BITSTRING)
                    mfn=b7_hit(cib7p,boolridx,boolh,b7setno);
                else
                    mfn = *mfnlistp; mfnlistp++;

                if (mfn <= 0) fatal("aotcie1/DD/dciedbnp/hit");
    
                if (parmtrace) printf("+++ hit#%"_LD_" mfn=%"_LD_"\n",boolh,mfn);
    
                RECORD(drec,dciedbnp,mfn);
                if (RECrc != RCNORMAL) fatal("aotcie1/DD/dciedbnp/RCNORMAL");
    
            }
            else RECORD(drec,dciedbnp,mfn=dcienxtmfn);

            count1++;
            if (parmtrace) printf("+++ mfnDD=%"_LD_"\n",mfn);
    
            /* process record
            */
            batchp=fmtarea;
    
            /* add CGI and +hits data fields
            */
            sprintf(batchp,"A1001/%"_LD_"/A1002/%"_LD_"/",count1,count2); batchp+=strlen(batchp);
    
            /* add dcie data fields
            */
            sprintf(batchp,"a3203|"                      ); batchp+=strlen(batchp);
            recfield(batchp,drec,ICDTAG_E,1,"");            batchp+=strlen(batchp);
            sprintf(batchp,"|"                           ); batchp+=strlen(batchp);
    
            if (fldupdat(drec,fmtarea)) fatal("aotcie1/DD/fldupdat/fmt");
            if (fldupdat(drec,envarea)) fatal("aotcie1/DD/fldupdat/env");

            if (fmt_inter(tabfmtp,drec,fmtasize,fmtarea,fmtasize) < 0) {
                printf("Content-type: text/plain\n\n");
                printf("error: %s [format execution error %d]\n",pftfnamp,fmt_error);
                exit(21);
            }

            /* output format results
            */
            puts(fmtarea);
            
            if (!boolhits) break;

        } /* end for (boolh=1; ;  ) */

    } /* errno */

    if (!errno) if (boolhits == 0) {
    }

} /* end actionDD */

/* =======================         =============          ================== */
/* =======================         =============          ================== */

    /* exit
    */
    if (cib7p) cib7p=b7_cib7_delete(cib7p);
#if CIAPI
    cisisApplicationDelete(ciapip);
    ciapip=NULL;
#endif /* CIAPI */

    exit(0);
}


#if ANSI
char *recsfldv(char *areap, char *fldp, FFI fldlen, char *defaultp)
#else
char *recsfldv(areap,fldp,fldlen,defaultp)
char *areap;
char *fldp;
FFI fldlen;
char *defaultp;
#endif /* CIAPI_SOURCE */
{
    FFI n;
    unsigned char *p;

    p=subfldp(fldp,'v',&fldlen);

    if (p) n=fldlen; /* ja' decrementado */
    else
        if (defaultp)
            n=(FFI)strlen((char*)(p=(unsigned char*)defaultp));
        else
            n=(FFI)strlen((char*)(p=(unsigned char*)"\0"));
    if (!areap)
        if ((areap=(char *)ALLOC((ALLOPARM)n+1)) == (char *)ALLONULL)
            fatal("aotmsa1/recsfldv/ALLOC");
    memmove(areap,p,n); areap[n]='\0';
    return(areap);
}


