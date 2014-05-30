
/* wtrun.c - included from ciupd.c
*/

/* ------------------------------ wtrun.c --------------------------------- */
// rem void *memset(void *s, int c, size_t n);


/* wtrun api - defines
*/    
#include "wtrun.h"      /* define ((WTFUN_XWT *)xwtp)-> */


/* ============================== wtfnew ================================== */
WTFUN_ARRAY *wtfnew(cmd)
int cmd;
{
    WTFUN_ARRAY *awtfp;
    WTFUN_XWT *xxwtp=wtnew(cmd);                                // new
    time_t xsecs_now;    /* LONGX da' invasao de memoria */

    if (!xxwtp) return NULL;

    awtfp=(WTFUN_ARRAY *)ALLOC((ALLOPARM)sizeof(WTFUN_ARRAY));
    if (awtfp == (WTFUN_ARRAY *)ALLONULL) return NULL;
    memset(awtfp,0x00,sizeof(WTFUN_ARRAY));
    awtfp->vwtfp[0]=xxwtp;                                      // wtf4p area
    sprintf(awtfp->vwtfname[0],"coll 0 %"_LD_,(LONGX)time(&xsecs_now));  // collname#0
    awtfp->nwtfs=1;                                             // coll#0

    /* done */
    if (cmd) fprintf(stderr,"*** wtf: new [%"_LD_":%d]\n",(LONGX)awtfp,sizeof(WTFUN_ARRAY));
    return awtfp;
}



/* ============================== wtfset ================================== */
WTFUN_ARRAY *wtfset(cmd, awtfp, envp)                             // init
int cmd;
WTFUN_ARRAY *awtfp;
char *envp;
{
    //time_t xsecs_now;
    LONGX  xmaxprim;
    int xdebug=cmd;

#if CIWTF
#else
    /* init rec_maxmfrl */
    if (!ndbxs) dbxinit();  /* init vdbxp/vrecp/vtrmp if not yet init - AOT, 28/10/2005 */
#endif

    awtmaxmfrl=rec_maxmfrl;   // awtmaxmfrl awtdatabuff jd are based on awtfp->x

    /* working area - local envp */
    awtdatabuff=(char *)loadfile(NULL,'@',"",NULL, awtmaxmfrl*2,   '\0');


    /* default jd hashing parms
    */
    jdtabcats=MAXTABCATS;
    jdtabwidth=MAXTABWIDTH;

    jd_n=0;
    jd_areap=NULL;    
    jd_asize=awtmaxmfrl;
    
    jdv_n=0;    
    jdv_areap=NULL;    
    jdv_asize=awtmaxmfrl;
    
    /* get jd hashing parms 
    */
    if (envp) {                 
      char *p=strcpy(awtdatabuff,envp); //returns dest
      int left=strlen(p);
      for (; left; ) {     // for each line of envp
         int len;
         char *q;
         while (isspace(*p)) if (left) { p++; left--; }  //strip
         if (!left) break;
         for (q=p, len=0; left;  p++, len++, left--) { // go forward
             if (*p=='\n' || *p=='\r') {               // until newline                 
                 while (*p=='\n' || *p=='\r') { *p++='\0'; left--; }
                 break;       
             }
         }
         if (!len) continue;                      // skip null lines                 
         if (strncmp(q,"EVAL KEYS=",10) == 0) {
             jdtabcats=atoi(q+10);
             if (jdtabcats < 1) jdtabcats=1;
             continue;
         }
         if (strncmp(q,"EVAL WIDTH=",11) == 0) {
             jdtabwidth=atoi(q+11);
             if (jdtabwidth < 1) jdtabwidth=1;
             continue;
         }
         if (strncmp(q,"EVAL ",5) == 0) {  // compile EVAL parms
             char *namp=q+5;
             int left=strlen(namp);
             int nlen=0;
             char *specp;
             for (specp=namp; left;  specp++, nlen++, left--) { // scan name
                 if (*specp == '=') break;       // until jd=
             }
             if (!nlen) continue;                      // skip null lines
             specp++; // skip =
             if (jd_n >= MAXJDS) ; //fatal("MAXJDS");
             else {
                 if (nlen > LE2) nlen=LE2;
                 memmove(jd_name[jd_n],namp,nlen); 
                 jd_name[jd_n][nlen]='\0';
                 jd_pgmp[jd_n]=(FMT_CODE *)NULL;
                 if (fmt_gener(&jd_pgmp[jd_n],specp)) ; //fatal(specp+fmt_errof);            
                 else jd_n++;
             }
         }    
         if (strncmp(q,"SHOW ",5) == 0) {  // compile SHOW parms
             char *namp=q+5;
             int left=strlen(namp);
             int nlen=0;
             char *specp;
             for (specp=namp; left;  specp++, nlen++, left--) { // scan name
                 if (*specp == '=') break;       // until detailed=
             }
             if (!nlen) continue;                      // skip null lines
             specp++; // skip =
             if (jdv_n >= MAXJVS) ; //fatal("MAXJVS");
             else {
                 if (nlen > LE2) nlen=LE2;
                 memmove(jdv_name[jdv_n],namp,nlen); 
                 jdv_name[jdv_n][nlen]='\0';
                 jdv_pgmp[jdv_n]=(FMT_CODE *)NULL;
                 if (fmt_gener(&jdv_pgmp[jdv_n],specp)) ; //fatal(specp+fmt_errof);            
                 else jdv_n++;
             }
         }    
      }  // end for each line of envp      
    }  // end if envp

    
    if (jd_n) {
        /* format area */
        jd_areap=(char *)loadfile(NULL,'@',"",NULL, jd_asize,'\0');
    }

    if (jdv_n) {
        /* format area */
        jdv_areap=(char *)loadfile(NULL,'@',"",NULL, jdv_asize,'\0');
    }
    
    /* allocate jd's hashing table
    */
    jdtable=hashalloc(jdtabcats,jdtabwidth,&xmaxprim);
    jdmaxprim=xmaxprim;
    if (xdebug) fprintf(stderr,"+++ hash=%"_LD_"\n",jdmaxprim);
    jdhwidth=(jdtabwidth+1);
    jdtabentries=0;
    
    /* allocate list of relevant terms
    */
    listjd=    (LISTJD *)loadfile(NULL,'@',"",NULL, jdmaxprim*sizeof(LISTJD), '\0');

    /* allocate frequencies and weights
    */
    JDCOLFREQ=  (LONGX *) loadfile(NULL,'@',"",NULL, jdmaxprim*sizeof(LONGX),  '\0');
    JDDOCFREQ=  (LONGX *) loadfile(NULL,'@',"",NULL, jdmaxprim*sizeof(LONGX),  '\0');
    JDSIMFREQ=  (LONGX *) loadfile(NULL,'@',"",NULL, jdmaxprim*sizeof(LONGX),  '\0');
    
    JDCOLPESO= (float *) loadfile(NULL,'@',"",NULL, jdmaxprim*sizeof(float), '\0');
    JDDOCPESO= (float *) loadfile(NULL,'@',"",NULL, jdmaxprim*sizeof(float), '\0');
    JDSIMPESO= (float *) loadfile(NULL,'@',"",NULL, jdmaxprim*sizeof(float), '\0');
    
    memset(JDCOLFREQ,0x00,jdmaxprim*sizeof(LONGX));
    memset(JDDOCFREQ,0x00,jdmaxprim*sizeof(LONGX));
    memset(JDSIMFREQ,0x00,jdmaxprim*sizeof(LONGX));
       
    memset(JDCOLPESO,0x00,jdmaxprim*sizeof(float));
    memset(JDDOCPESO,0x00,jdmaxprim*sizeof(float));
    memset(JDSIMPESO,0x00,jdmaxprim*sizeof(float));
        
    /* chars to entities */
    memset(convent,0x00,sizeof(convent));
    convent['&']="&amp;"; 
    convent['<']="&lt;";  
    convent['>']="&gt;";  
    convent['"']="&quot;";

    /* done */
    if (cmd) fprintf(stderr,"*** wtf: new [%"_LD_":%d]\n",(LONGX)awtfp,sizeof(WTFUN_ARRAY));
    return awtfp;
}



/* ============================== wtfdel ================================== */
int wtfdel(cmd, awtfp)
int cmd;
WTFUN_ARRAY *awtfp;
{
    if (cmd) fprintf(stderr,"*** wtf: del [%"_LD_"]\n",(LONGX)awtfp);
    //...
    //...    while (jd_n--) {
    //...        if (jd_pgmp[jd_n]) 
    //...	    { fmt_free(jd_pgmp[jd_n]); jd_pgmp[jd_n]=(FMT_CODE *)NULL; }
    //...    }
    //...    if (jd_areap) { FREE(jd_areap); jd_areap=(char *)NULL; }
    //...    jd_n=0;

    FREE(awtfp);
    return 0;
}


/* ============================== wtnew ================================== */
WTFUN_XWT *wtnew(cmd)
int cmd;
{
    WTFUN_XWT *xwtp;
    xwtp=(WTFUN_XWT *)ALLOC((ALLOPARM)sizeof(WTFUN_XWT));
    if (xwtp == (WTFUN_XWT *)ALLONULL) return NULL;
    memset(xwtp,0x00,sizeof(WTFUN_XWT));
    if (cmd) fprintf(stderr,"*** wt: new [%"_LD_":%d]\n",(LONGX)xwtp,sizeof(WTFUN_XWT));
    return xwtp;
}




/* ============================== wtdel ================================== */
int wtdel(cmd, xwtp)
int cmd;
WTFUN_XWT *xwtp;
{
    if (cmd) fprintf(stderr,"*** wt: del [%"_LD_"]\n",(LONGX)xwtp);
    //...
    FREE(xwtp);
    return 0;
}

/* ============================== wtexit ================================== */
int wtexit(cmd, rc)
int cmd;
int rc;
{
    if (cmd) fprintf(stderr,"*** wt: end of execution [%d]\n",rc);
    exit(rc);
    return 0; //
}
/* ======================================================================== */




/* ========================= wtrig2/wtrig1/jd source code ============= */

//int w2reply 
//int trigcoll
///////////////
#include "w2.c"
///////////////

//int loadtrig 
////////////////
#include "w21.c"
////////////////

//int w2topjd
////////////////
#include "w2top.c"
////////////////

//int w2docjk
////////////////
#include "w2djk.c"
////////////////

//int w2docjv
////////////////
#include "w2djv.c"
////////////////
/* ======================================================================== */




/* ========================= wtrig2/wtrig1/jd service ================= */

/* ============================== loadcoll ================================ */
int loadcoll (int cmd, WTFUN_ARRAY *awtfp, char *parmstring, char *reply)
{
    char hdrparmstring[HDRPARMSTRING+1];

    int replyh=1;
    int addeight=1;
    
    int joinjd=1;
    int replyhx=1;

    int rc;
    time_t secs_now;
    time_t secs_start,secs_end;
    struct tm *tp;

    char *p;

    char *movetxtp; //=NULL;
    char *txt1vp; //=NULL;
    char *txtvp=NULL;
    int   txtvlen=0;
           
    *reply='\0';

    /* get secs_ for elapsed */
    secs_start=time(&secs_now);
    tp=localtime(&secs_start);

    //reply..
    if (replyh) {
        sprintf(reply,"<serw version=\"1.0\" date=\"%04d%02d%02d\" time=\"%02d%02d%02d\">\n",
            1900+tp->tm_year,tp->tm_mon+1,tp->tm_mday,tp->tm_hour,tp->tm_min,tp->tm_sec);
        reply+=strlen(reply);
    }


    /* run wtrig2 for a given collection - w2.c
    */

    txt1vp=strstr(parmstring," text1=");
    if (txt1vp) {
        movetxtp=txt1vp;
        txt1vp+=strlen(" text1=");
        txtvp=strstr(txt1vp," text=");        // text1=.. text=..

        // init " text=" keyword
        sprintf(movetxtp," text="); movetxtp+=6;

        if (txtvp) {
            // copy .. until text=
            for (p=txt1vp; p != txtvp; ) {                                   // *movetxtp++ = *p++;
                if (isspace(*p)) { if (*p != '\r') *movetxtp++ = ' '; }
                else *movetxtp++ = *p;
                p++;
            }
            *movetxtp='\0';
            txtvp+=strlen(" text=");                                         //w2ci
            txtvlen=strlen(txtvp);                                           //w2ci
            strcpy(hdrparmstring,parmstring);                                //w2arg 
        }
        else {                                // text1=.. only
            txtvp=movetxtp;                                                  //w2ci
            txtvlen=strlen(txt1vp);                                          //w2ci
            // copy .. until eot
            for (p=txt1vp; *p; ) {                                           // *movetxtp++ = *p++;
                if (isspace(*p)) { if (*p != '\r') *movetxtp++ = ' '; }
                else *movetxtp++ = *p;
            }
            *movetxtp='\0';
            movetxtp=NULL;                                                   //w2ci single run
        }
    }

    if (!txtvp) {
        txtvp=strstr(parmstring," text=");    // text=.. only
        if (txtvp) {
            txtvp+=strlen(" text=");                                         //w2ci
            txtvlen=strlen(txtvp);                                           //w2ci
            movetxtp=NULL;                                                   //w2ci single run
        }
    }
    
//    if (movetxtp) {
        // several trigcoll (cmd, awtfp, parmstring, reply+=) 
/////////////////
#include "w2ci.c"
/////////////////

//    }
//    else {
//        rc=trigcoll (cmd, awtfp, parmstring, reply);
//        reply+=strlen(reply);
//    }


    /* get secs_ for elapsed */
    secs_end=time(&secs_now);

    //reply..
    if (replyh) {
        sprintf(reply,"<elapsed>%"_LD_"</elapsed>\n",(LONGX)secs_end-secs_start);
        reply+=strlen(reply);
        sprintf(reply,"</serw>\n");
        reply+=strlen(reply);
    }
    /* ... */
    if (!reply) fatal("loadcoll/reply");
    
    return rc;
}
/* ============================== loadcoll end ============================ */

/* wtrun api - undefs
*/    
#include "wtz.h"
#include "wtrunz.h"

/* ------------------------------ wtrun.c --------------------------------- */
