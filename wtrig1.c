/* Program WTRIG1: trigrams 1A
*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#if BEFORE55a
#include <math.h>
#include <time.h>
#endif //BEFORE55a

#include "cisis.h"   /* CISIS Interface header file */
#include "cirun.h"   /* CISIS Interface runtime declarations */
#if BEFORE55a
#include "cihsh.h"   /* CISIS Interface hashing & binary table search */
#endif //BEFORE55a


#define DISCART     1

#define PARMDTAG    1
#define PARMDTAG6W  1

#define PARMLANG    1
#define MAXPARMLANG 32

#define MAXSAVETEXT 128*1024

#if BEFORE55a
#define TAG1 1
#define TAG2 2
#define TAG3 3
#endif //BEFORE55a


#if BEFORE44B
/* tabelas de mxaot.c */
unsigned char ansiuctab[256]=
{
          0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
         16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
         32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
         48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
         64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
         80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
         96, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
         80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90,123,124,125,126,127,
        128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
        144,145,146,147,148,149,150,151,152,153,138,155,140,157,158,159,
        160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,
        176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
         65, 65, 65, 65, 65, 65, 65, 67, 69, 69, 69, 69, 73, 73, 73, 73,
         68, 78, 79, 79, 79, 79, 79,215,216, 85, 85, 85, 85, 89,222,223,
         65, 65, 65, 65, 65, 65, 65, 67, 69, 69, 69, 69, 73, 73, 73, 73,
        208, 78, 79, 79, 79, 79, 79,247,216, 85, 85, 85, 85,221,222,221
};

unsigned char ansiactab[] = {
 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66,
 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 97, 98, 99,100,101,102,103,104,
 105,106,107,108,109,110,111,112,113,114,115,116,117,
118,119,120,121,122,128,129,130,131,132,133,134,135,136,137,138,139,140,141,
142,143,144,145,146,147,148,149,150,151,152,153,154,
160,161,162,163,164,165,168,192,193,194,195,196,199,200,201,202,203,204,205,
206,207,209,210,211,212,213,214,217,218,219,220,221,
224,225,226,227,228,231,232,233,234,235,236,237,238,239,241,242,243,244,245,
246,249,250,251,252,253,255
,000
};
#endif //BEFORE44b

typedef struct xlistawt1 {
    float weight;
    LONGX hidx;
    void *prev;
    void *next;
    void *avail;
} LISTAWT1;

void main(int argc, char *argv[])
{
    LONGX COLLECTION_SIZE;     /* N - #docs in collection */

    LONGX  *COLLECTION;        /* n(t) - #docs trigram occurs */
    int   *DOCUMENT;          /* f(t) - #occs of trigram in document */
                              /*        [also flag 1st occ in doc] */

    float *GLOBALWEIGHT;      /* g(t) - trigram global weight */

    float LOCALWEIGHT;        /* l(t) - trigram local weight */
    float *PRODWEIGHT;        /* lg(t) - trigram local*global weight */
    float TERMWEIGHT;         /* w(t) - trigram normalized weight */

    LONGX *OFFSET;
    LONGX *COUNT;
    LONGX *POSTINGS;

    LONGX *INDEX;              /* position before relocation */

    LISTAWT1 *lista;

    char *table;
    LONGX maxprim,hidx;
    char *h;
    int hwidth;
    int found;
    LONGX nterms,hidx2;
    LONGX countok,xcount,xcountrecs,ycount;
    char *h2;
    LONGX hloop;

    RECSTRU *recp;  /* mandatory for defines REC,MFR,DIR,FIELDP */
    LONGX irec,crec,crec2,zirec,xirec;
    LONGX maxmfn,mfn,docmfn;

    DIRSTRU *dirp;
    int xdir;
    int iocc,left;
    FFI loop,keylen;
    char *p,*q,*fldp;

    char areacmd[BUFSIZ];
    char areacmd6words[BUFSIZ];
    char areacmd6words2[BUFSIZ];

    char *buff;
    LONGX buffsize;
    char *dtagbuff=NULL;

    LONGX count;
    int iarg;
    LONGX xtag;
    LONGX n;

    int trigrams=1;

    char *indocumentsdb=NULL;
    char *documentsdb=NULL;

    char *parmdocumentsdb=NULL;
    UWORD parmtag=0;
    int parmwidth=0;
    char *parmuppercase="ascii";
    float parmmaxrf=0.50;
    int parmmaxtv=50;
    LONGX parmcategories=100000;
    char *collectiondb=NULL;
    char collectionsizedb[CIMPL+1] = { '\0' };
    char collectionoccsdb[CIMPL+1] = { '\0' };
    char outcollectionsizedb[CIMPL+1] = { '\0' };
    char outcollectionoccsdb[CIMPL+1] = { '\0' };
    char outtermvectorsdb[CIMPL+1] = { '\0' };
    char outterminverteddb[CIMPL+1] = { '\0' };
    int parmtrace=0;
    int parmtrace2=0;
    LONGX parmtell=0;
    LONGX parmtell2=0;

    int parmwritebin=0;

    LONGX readcollectionsize=0;

    LONGX usercollectionsize=0;
    UWORD usertag=0;
    int userwidth=0;
    char *useruppercase=NULL;
    float usermaxrf=0;
    int usermaxtv=0;
    LONGX usercategories=0;

    FMT_CODE *extractfmtp=NULL;	    /* formato para tab= */
    char *extractfmta=NULL;	    /* area para tab= */

    FMT_CODE *readextractfmtp=NULL;	    /* formato para tab= */
    char *readextractfmta=NULL;	    /* area para tab= */

    time_t secs_now;
    struct tm *tp;
    time_t secs_start,secs_1,secs_2,secs_3,secs_4;

    int parmrelloc=1;
    int parmsort=1;
    int parmshowh=0;
    int parmx12345=1;

#if PARMLANG
    char  *HITS;        /* HITS[] */
    LONGX hitbytes;
    unsigned char hitbitmask[8] = { 0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01 };
    unsigned char hitmask;
    //LONGX hitmfn;
    LONGX hitoffset;
    char *langshits[MAXPARMLANG];
    char *langsname[MAXPARMLANG];
    LONGX langscount[MAXPARMLANG];
    int nlangs=0;
    int langidx=(-1);
#endif /* PARMLANG */
    LONGX maxhits=0L;

    int parmdtag=0;
    char dtagc1='\0';
    char dtagc2='\0';

    int parm6words=0;
    LONGX outmfn=0; /* init */

    DIRSTRU *zdirp;
    int zxdir;
    int zloop;
    char *zfldp;
    LONGX countcollection;

    char parmexcludedb[CIMPL+1] = { '\0' };
    LONGX itrm;

    int parmsavetext=1;
    char savetext[MAXSAVETEXT+1];

    LONGX parmfrom,parmto,parmloop,parmcount;
    LONGX xpcount;

    parmfrom=1; parmto=0; parmloop=1; parmcount=LONGX_MAX;

    /* get secs_ for elapsed */
    secs_start=time(&secs_now);
    tp=localtime(&secs_start);


    if (argc < 3) {
        printf("%s",cicopyr("Utility WTRIG1"));
        printf(" \n");
	    printf("wtrig1 documents=<mf1>[,<mf3>] extract={<tag>|<fmt>} [collection=<mf2>] \n");
        printf(" \n");
        printf(" input parms and collection data from <mf2>.c and <mf2>.n, if provided \n");
        printf(" input text from <mf1>, using field <tag> or format specification <fmt> \n");
        printf(" output a term vector for each input text to <mf1>.v or <mf3>.v \n");
        //printf(" if collection is not provided, a first data reading is performed \n");
        //printf(" to get the term distribution used in term global weighting; \n");
        printf(" \n");
        printf(" options: width=<term width>           0 for trigrams         [%d] \n",parmwidth);
        printf("          case={ascii|ansi|<file>}                            [%s] \n",parmuppercase);
        printf("          maxrf=<max relative frequency for a term>           [%4.2f]\n",parmmaxrf);
        printf("          maxtv=<max #terms in a term vector>                 [%d] \n",parmmaxtv);
        printf("          K=<max #terms>                                      [%"_LD_"] \n",parmcategories);
        printf("          N=<total #docs>                                     [maxmfn] \n");
        printf(" \n");
        printf("          col.c=<mf>   collection parms                       [collection.c] \n");
        printf("          col.n=<mf>   term distribution in collection        [collection.n] \n");
        printf(" \n");
        printf("          out.c={<mf>|void}   document parms                  [document.c|void]\n");
        printf("          out.n={<mf>|void}   term distribution in documents  [document.n|void]\n");
        printf("          out.x={<mf>|void}   posting list for each term      [document.x|void]\n");
        printf("          out.v={<mf>|void}   term vector for each document   [document.v]     \n");
        printf(" \n");
        //printf("          [-relloc] [-sort] [+hash]                                            \n");
        printf("          [dmfn] [6words] [exclude=<if>]                      [%d] [%d] [%s]   \n",parmdtag,parm6words,parmexcludedb);
        printf("          [bin] trace[={1|2|4|8|16|32|64|3..127}]             [%d]             \n",parmtrace);
        printf(" \n");
	    exit(1);
    }

    /* get parameters */
    for (iarg=1; iarg < argc; iarg++) {
        p=argv[iarg];
        if (parmtrace) printf("+++ %s\n",p);
        if (strcmp(p,"trace") == 0) {
            parmtrace2=1;
            parmtrace=127;
            continue;
        }
        if (strncmp(p,"documents?",10) == 0 || strncmp(p,"documents=",10) == 0 ||
            strncmp(p,"d?",2) == 0 || strncmp(p,"d=",2) == 0) {
            if (strncmp(p,"d?",2) == 0 || strncmp(p,"d=",2) == 0) parmdocumentsdb=p+2;
            else parmdocumentsdb=p+10;
            if (!*parmdocumentsdb) fatal(p);
            continue;
        }
        if (strncmp(p,"extract?",8) == 0 || strncmp(p,"extract=",8) == 0 ||
            strncmp(p,"e?",2) == 0 || strncmp(p,"e=",2) == 0) {
            if (strncmp(p,"e?",2) == 0 || strncmp(p,"e=",2) == 0) q=p+2;
            else q=p+8;
            if (isdigit(*q)) {
                if (sscanf(q,"%"_LD_,&xtag) != 1) fatal(p);
                if (xtag < 1 || (unsigned)xtag > USHRT_MAX) fatal(p);
                usertag=(UWORD)xtag;
                if (extractfmta) fatal(p);
                continue;
            }
            else {
                extractfmta=loadfile(NULL,'@',q,NULL,0,'\0');
	            if (!extractfmta) fatal(p);
	            if (fmt_gener(&extractfmtp,extractfmta)) fatal(extractfmta+fmt_errof);
                if (usertag) fatal(p);
                continue;
            }
        }
        if (strncmp(p,"width?",6) == 0 || strncmp(p,"width=",6) == 0) {
            if (sscanf(p+6,"%d",&userwidth) != 1) fatal(p);
            if (userwidth < 0) fatal(p);
            continue;
        }
        if (strncmp(p,"case?",5) == 0 || strncmp(p,"case=",5) == 0) {
            useruppercase=p+5;
            if (strcmp(useruppercase,"ascii") == 0) {
                continue;
            }
            memset(isiswctab,0x00,256);
            if (strcmp(useruppercase,"ansi") == 0) {
                memmove(isisuctab,ansiuctab,256/*sizeof(ansiuctab)*/);
                for (isiswctot=0; ansiactab[isiswctot]; isiswctot=isiswctot+1)
                    isiswctab[ansiactab[isiswctot]]=1;
            } else {
                unsigned char acbuff[256];
                if (!loaductb(NULL,isisuctab,useruppercase)) fatal(p);
                if (!loadactb(NULL,acbuff,useruppercase)) fatal(p);
                for (isiswctot=0; acbuff[isiswctot]; isiswctot=isiswctot+1)
                    isiswctab[acbuff[isiswctot]]=1;
            }
            continue;
        }
        if (strncmp(p,"maxrf?",6) == 0 || strncmp(p,"maxrf=",6) == 0) {
            if (sscanf(p+6,"%f",&usermaxrf) != 1) fatal(p);
            if (usermaxrf <= 0 || usermaxrf > 1.0) fatal(p);
            continue;
        }
        if (strncmp(p,"maxtv?",6) == 0 || strncmp(p,"maxtv=",6) == 0) {
            if (sscanf(p+6,"%d",&usermaxtv) != 1) fatal(p);
            if (usermaxtv < 1) fatal(p);
            continue;
        }
        if (strncmp(p,"K?",2) == 0 || strncmp(p,"K=",2) == 0) {
            if (sscanf(p+2,"%"_LD_,&usercategories) != 1) fatal(p);
            if (usercategories < 1) fatal(p);
            continue;
        }
        if (strncmp(p,"N?",2) == 0 || strncmp(p,"N=",2) == 0) {
            if (sscanf(p+2,"%"_LD_,&usercollectionsize) != 1) fatal(p);
            if (usercollectionsize <= 0) fatal(p);
            continue;
        }
        if (strncmp(p,"collection?",11) == 0 || strncmp(p,"collection=",11) == 0 ||
            strncmp(p,"c?",2) == 0 || strncmp(p,"c=",2) == 0) {
            if (strncmp(p,"c?",2) == 0 || strncmp(p,"c=",2) == 0) collectiondb=p+2;
            else collectiondb=p+11;
            if (!*collectiondb) fatal(p);
            continue;
        }
        if (strncmp(p,"col.c?",6) == 0 || strncmp(p,"col.c=",6) == 0) {
            q=p+6; if (!*q) fatal(p);
            strcpy(collectionsizedb,q);
            continue;
        }
        if (strncmp(p,"col.n?",5) == 0 || strncmp(p,"col.n=",6) == 0) {
            q=p+6; if (!*q) fatal(p);
            strcpy(collectionoccsdb,q);
            continue;
        }
        if (strncmp(p,"out.c?",6) == 0 || strncmp(p,"out.c=",6) == 0) {
            q=p+6; if (!*q) fatal(p);
            strcpy(outcollectionsizedb,q);
            continue;
        }
        if (strncmp(p,"out.n?",6) == 0 || strncmp(p,"out.n=",6) == 0) {
            q=p+6; if (!*q) fatal(p);
            strcpy(outcollectionoccsdb,q);
            continue;
        }
        if (strncmp(p,"out.v?",6) == 0 || strncmp(p,"out.v=",6) == 0) {
            q=p+6; if (!*q) fatal(p);
            strcpy(outtermvectorsdb,q);
            continue;
        }
        if (strncmp(p,"out.x?",6) == 0 || strncmp(p,"out.x=",6) == 0) {
            q=p+6; if (!*q) fatal(p);
            strcpy(outterminverteddb,q);
            continue;
        }
        if (strcmp(p,"dmfn") == 0) {
            parmdtag=1;
            dtagc1='\r';
            dtagc2='\n';
            continue;
        }
        if (strcmp(p,"6words") == 0) {
            parm6words=1;
            parmdtag=1;
            dtagc1='\r';
            dtagc2='\n';
            continue;
        }
        if (strncmp(p,"exclude?",8) == 0 || strncmp(p,"exclude=",8) == 0) {
            q=p+8; if (!*q) fatal(p);
            strcpy(parmexcludedb,q);
            continue;
        }
        if (strcmp(p,"bin") == 0) {
            parmwritebin=1;
            continue;
        }
        if (strcmp(p,"-relloc") == 0) {
            parmrelloc=0;
            continue;
        }
        if (strcmp(p,"-sort") == 0) {
            parmsort=0;
            continue;
        }
        if (strcmp(p,"+hash") == 0) {
            parmshowh=1;
            continue;
        }
        if (strncmp(p,"tell?",5) == 0 || strncmp(p,"tell=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&parmtell) != 1) fatal(p);
            parmtell2=parmtell;
            continue;
        }
        if (strncmp(p,"tell2?",6) == 0 || strncmp(p,"tell2=",6) == 0) {
            if (sscanf(p+6,"%"_LD_,&parmtell2) != 1) fatal(p);
            continue;
        }
        if (strncmp(p,"trace?",6) == 0 || strncmp(p,"trace=",6) == 0) {
            if (sscanf(p+6,"%d",&parmtrace) != 1) fatal(p);
            continue;
        }
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
        if (strncmp(p,"count=",6) == 0) {
            if (sscanf(p+6,"%"_LD_,&parmcount) != 1)
                fatal(p);
            continue;
        }
        fatal(p);
    }

    /* check mandatory parms */
    if (!parmdocumentsdb) fatal("wtrig1: missing documents= parameter");


    /* program limitation: check COUNT[hidx] > COLLECTION[hidx] */
    if (collectiondb) if (*outterminverteddb)   if (strcmp(outterminverteddb,  "void") != 0) fatal("collection x out.x");
    /* parmsort optimization */
    if (collectiondb) if (*outcollectionoccsdb) if (strcmp(outcollectionoccsdb,"void") != 0) fatal("collection x out.n");
    /* disable parmsort */
    if (collectiondb) parmsort=0;

    indocumentsdb=documentsdb=strdup(parmdocumentsdb);
    if ((p=strchr(indocumentsdb,',')) != NULL) {
        *p='\0';
        documentsdb=p+1;
    }

    /* set input file names
    */
    if (collectiondb) {
        if (!*collectionsizedb)    sprintf(collectionsizedb,   "%s.c",collectiondb);
        if (!*collectionoccsdb)    sprintf(collectionoccsdb,   "%s.n",collectiondb);
    }

    /* set output file names
    */
    if (!*outcollectionsizedb)     sprintf(outcollectionsizedb,"%s.c",documentsdb);
    if (!collectiondb) {
        if (!*outcollectionoccsdb) sprintf(outcollectionoccsdb,"%s.n",documentsdb);
        if (!*outterminverteddb)   sprintf(outterminverteddb,  "%s.x",documentsdb);
    }
    if (!*outtermvectorsdb)        sprintf(outtermvectorsdb,   "%s.v",documentsdb);

    /* void file names */
    if (strcmp(outcollectionsizedb,"void") == 0) outcollectionsizedb[0]='\0';
    if (strcmp(outcollectionoccsdb,"void") == 0) outcollectionoccsdb[0]='\0';
    if (strcmp(outterminverteddb,  "void") == 0) outterminverteddb[0]='\0';
    if (strcmp(outtermvectorsdb,   "void") == 0) outtermvectorsdb[0]='\0';
    //if (strcmp(outtermvectorsdb,"void") == 0) fatal(p);


    /* alloc working areas
    */
    irec=nrecs;
    p=dbxcipar(NULL,"maxmfrl",'='); if (dbxcipok) sscanf(p,"%"_LD_,&rec_maxmfrl);
    if (rec_maxmfrl < (LONGX)MSNVSPLT || (RECHSIZE+rec_maxmfrl) > ALLOMAXV) rec_maxmfrl=MAXMFRL;
    recallok(irec,rec_maxmfrl);
    zirec=nrecs;
    recallok(zirec,rec_maxmfrl);
    crec=nrecs;
    recallok(crec,sizeof(M0STRU));
    crec2=nrecs;
    recallok(crec2,sizeof(M0STRU));

    buff=loadfile(NULL,'@',"",NULL,buffsize=rec_maxmfrl*2,'\0');
    dtagbuff=loadfile(NULL,'@',"",NULL,buffsize,'\0');

    /* here we go
    */

    /* set processing record */
    RECORD(zirec,"null",1L); /*MFRstatus=ACTIVE;*/
    
    /* read maxmfn */
    RECORD(irec,indocumentsdb,0);
    maxmfn=MF0nxtmfn-1;

    /* set collection size: N */
    COLLECTION_SIZE=maxmfn;
    if (usercollectionsize) COLLECTION_SIZE=usercollectionsize;

    /* set other parms */
    if (usertag) parmtag=usertag;

    if (userwidth) {
        parmwidth=userwidth;
        trigrams=0;
    }
    else {
        parmwidth=4; /* [][][][2] */
        trigrams=1;
    }

    if (useruppercase) parmuppercase=useruppercase;
    if (usermaxrf) parmmaxrf=usermaxrf;
    if (usermaxtv) parmmaxtv=usermaxtv;
    if (usercategories) parmcategories=usercategories;

    /* read collection info */
    if (*collectionsizedb) {
        UWORD readtag=0;
        int readwidth=0;
        char *readuppercase=NULL;
        float readmaxrf=0;
        //LONGX readmaxprim=0;
        int readmaxtv=0;
        LONGX readcategories=0;
        int readtrigrams=0;

        RECORD(irec,collectionsizedb,1);
        if (RECrc != RCNORMAL) fatal("wtrig1/COLLECTION_SIZE/RCNORMAL");

        /* read parms */
        for (iocc=1; *recfield(buff,irec,TAG1,iocc,""); iocc++) {
            q="^nextract^v";
            p=strstr(buff,q); if (p)
                if (isdigit(*(p+strlen(q)))) {
                    readtag=(UWORD)atol(p+strlen(q));
                }
                else {
                    readextractfmta=loadfile(NULL,'@',p+strlen(q),NULL,0,'\0');
                    if (!readextractfmta) fatal(buff);
                    if (fmt_gener(&readextractfmtp,readextractfmta)) fatal(readextractfmta+fmt_errof);
                }
            q="^nwidth^v";
            p=strstr(buff,q); if (p) readwidth=atoi(p+strlen(q));
            q="^ttrigrams";
            p=strstr(buff,q); if (p) readtrigrams=1;
            q="^ncase^v";
            p=strstr(buff,q); if (p) readuppercase=p;
            q="^nmaxrf^v";
            p=strstr(buff,q); if (p) readmaxrf=atof(p+strlen(q));
            q="^nmaxtv^v";
            p=strstr(buff,q); if (p) readmaxtv=atoi(p+strlen(q));
            q="^nK^v";
            p=strstr(buff,q); if (p) readcategories=atol(p+strlen(q));
        }
        readcollectionsize=atol(recfield(buff,irec,TAG2,1,""));

        if (readtag) if (readextractfmtp) fatal("wtrig1/collectin/extract=tag/extract=fmt");
        if (!usercollectionsize) COLLECTION_SIZE=readcollectionsize;
        if (!usertag) if (!extractfmtp) {
            if (readextractfmtp) {
                extractfmta=readextractfmta;
                extractfmtp=readextractfmtp;
            }
            else parmtag=readtag;
        }
        if (!userwidth) { parmwidth=readwidth; trigrams=readtrigrams; }
        if (!useruppercase) parmuppercase=strdup(readuppercase);
        if (!usermaxrf) parmmaxrf=readmaxrf;
        if (!usermaxtv) parmmaxtv=readmaxtv;
        if (!usercategories) parmcategories=readcategories;
    }

    /* check collection size x document maxmfn */
    if (COLLECTION_SIZE < maxmfn) fatal("wtrig1/COLLECTION_SIZE");

    /* check parmtag x format */
    if (parmtag) if (extractfmtp) fatal("wtrig1/extract=tag/extract=fmt");
    if (!parmtag) if (!extractfmtp) fatal("wtrig1/missing extract=");
    if (extractfmtp) parmtag=999;

    /* setup fldupdat() for input records
    */
    if (!trigrams) {
        sprintf(areacmd,"Gsplit=%"_LD_"=words",(LONGX)parmtag);
    }
    else {
        sprintf(areacmd,"Gsplit=%"_LD_"=trigrams",(LONGX)parmtag);
    }

    /* setup fldupdat() for 6-word window indexing
    */
    if (parm6words) {
        sprintf(areacmd6words, "Gsplit=%"_LD_"=6words",(LONGX)parmtag);
        sprintf(areacmd6words2,"Gsplit=%"_LD_,       (LONGX)parmtag);
    }

    /* alloc itrm: do not read postings */
    if (*parmexcludedb) {
        trmalloc(itrm=ntrms,0L);
    }

    /* calculate maxprim and allocate hash table
    */
    table=hashalloc(parmcategories,parmwidth,&maxprim);
    if (parmtrace2) printf("+++ hash=%"_LD_"\n",maxprim);
    hwidth=(parmwidth+1);

    /* allocate list of relevant terms
    */
    lista=          (LISTAWT1 *)loadfile(NULL,'@',"",NULL, parmmaxtv*sizeof(LISTAWT1),'\0');

    /* allocate frequencies and weights
    */
    COLLECTION=     (LONGX *) loadfile(NULL,'@',"",NULL, maxprim*sizeof(LONGX),   '\0');
    DOCUMENT=       (int *)  loadfile(NULL,'@',"",NULL, maxprim*sizeof(int),    '\0');
    GLOBALWEIGHT=   (float *)loadfile(NULL,'@',"",NULL, maxprim*sizeof(float),  '\0');
    PRODWEIGHT=     (float *)loadfile(NULL,'@',"",NULL, maxprim*sizeof(float),  '\0');

    INDEX=          (LONGX *) loadfile(NULL,'@',"",NULL, maxprim*sizeof(LONGX),   '\0');


    /* get term docs in collection: n(t) */
    memset(COLLECTION,0x00,maxprim*sizeof(LONGX));
    if (*collectionoccsdb) {
        /* read term docs in collection from file */
        for (mfn=1; ; mfn++) {
            RECORD(irec,collectionoccsdb,mfn);
            if (RECrc == RCEOF) break;
            if (RECrc != RCNORMAL) continue;
            for (dirp=MFRdir, xdir=0, loop=MFRnvf; loop--; dirp++, xdir++) {
                if (dirp->tag != TAG3) continue;
                if (dirp->len == 0) continue;
                for (keylen=0, p=fldp=FIELDP(xdir), left=dirp->len; left; ) {
                    if (*p == '^') break;
                    keylen++; p++; left--;
                }
                n=0;
                if (left > 2) if (*p++ == '^') if (*p++ == 'n') {  /* ^n */
                    left=left-2;
                    while (left) {
                        if (!isdigit(*p)) break;
                        n=n*10+((int)(*p)-(int)'0');
                        p++; left--;
                    }
                }
                hidx=hashindex(table,maxprim,parmwidth,fldp,keylen,&found,1);
                if (found) fatal("wtrig1/TAG3/duplicated");
                COLLECTION[hidx]=n;
            }
        }
    }
    else {
        /* read all records to build term docs in collection*/
        FFI dtagfldlen=0;
        unsigned char *dtagfldp;
        unsigned char *dtagp;
        LONGX countcollection=0;
        count=xcount=xcountrecs=0; xpcount=0;
        for (mfn=parmfrom; ; mfn+=parmloop) {
            if (parmto) if (mfn > parmto) break;
            if (parmcount) if (xpcount++ >= parmcount) break;

            RECORD(irec,indocumentsdb,mfn);
            if (RECrc == RCEOF) break;
            if (RECrc != RCNORMAL) { /* skip deleted / inexistent */
                xcount++;
                continue;
            }
            /* process extract=<fmt> */
#if PARMDTAG
            if (extractfmtp)
                fmt_inter(extractfmtp,irec,buffsize,dtagbuff,buffsize);
            else
                recfield(dtagbuff,irec,parmtag,1,"");
#else /* PARMDTAG */
            if (extractfmtp) {
                char *lenp=buff+7;
                int x=17;
                          /* +012345678901234567 */
                sprintf(buff,"d*H999 xxxxxxxxx ");
                fmt_inter(extractfmtp,irec,buffsize-x-1,buff+x,buffsize-x-1);
                sprintf(lenp,"%9"_LD_,(LONGX)strlen(buff+x)); buff[x-1]=' ';
                if (fldupdat(irec,buff)) fatal(buff);
            }
#endif /* PARMDTAG */
#if PARMDTAG
/**/
        /* process each line */
        for (dtagfldlen=0, dtagp=dtagfldp=dtagbuff; ; ) {
            if (!*dtagp || *dtagp == dtagc1 || *dtagp == dtagc2) {
                /* empty record */
                MFRmfrl=MFRbase=LEADER; MFRnvf=0;
                if (dtagfldlen) {
                    char *p=dtagfldp;
                    FFI langlen=0;
                    if (dtagfldlen >= 2) if (*p++ == '^') if (*p++ == 'l') {
                        FFI skip=2;
                        FFI left=dtagfldlen-skip;
                        while (left--) {
                            if (*p++ == '^') {
                                skip++; if (*p == 'x') skip++;
                                break;
                            }
                            langlen++;
                        }
                        skip+=langlen;
                        dtagfldlen-=skip;
                        dtagfldp+=skip;
                    }
                    p=buff;
                    sprintf(p,"H%"_LD_" %"_LD_" ",(LONGX)parmtag,(LONGX)dtagfldlen); p+=strlen(p);
                    memmove(p,dtagfldp,dtagfldlen); *(p+=dtagfldlen)='\0'; dtagfldlen=0;
                    if (fldupdat(irec,buff)) fatal("wtrig1/dtag/fldupdat/1");
/**/
#endif /* PARMDTAG */

            /* process extract proc Gsplit=tag=6words */
            if (parm6words) {
//prtfields(recp,1);
                if (fldupdat(irec,areacmd6words)) fatal(areacmd6words);
//prtfields(recp,2);
                if (fldupdat(irec,areacmd6words2)) fatal(areacmd6words2);
//prtfields(recp,3);
//printf("++++\n");
/*getchar();*/
            }

#if PARMDTAG6W
/*2*/
        /* for each 6words line
        */
        for (zdirp=MFRdir, zxdir=0, zloop=MFRnvf; zloop--; zdirp++, zxdir++) {
            char *zp;
            if (zdirp->tag != parmtag) continue;
            zfldp=FIELDP(zxdir);
            if (*parmexcludedb) {
                TRMSTRU *trmp;
                unsigned char *p,*q;
                int ucloop;
                for (q=buff, p=(unsigned char *)zfldp, ucloop=zdirp->len; ucloop--; p++) *q++=isisuctab[*p];
                *q='\0';
                TERM(itrm,parmexcludedb,buff);
                if (TRMrc == RCNORMAL) zfldp=NULL;
            }
            /* build trigrec */
            zp=buff;
            sprintf(zp,"D*"); zp+=strlen(zp);
            if (zfldp) {
                sprintf(zp,"H%"_LD_" %"_LD_" ",(LONGX)parmtag,(LONGX)zdirp->len); zp+=strlen(zp);
                memmove(zp,zfldp,zdirp->len); *(zp+zdirp->len)='\0';
            }
            if (fldupdat(zirec,buff)) fatal("wtrig1/line/fldupdat/1");
//prtfields(vrecp[zirec],4);  //debug//
/*2*/
#endif /* PARMDTAG6W */

            /* process extract proc Gsplit=tag={trigrams|words} */
            if (fldupdat(zirec,areacmd)) fatal(areacmd);
//prtfields(vrecp[zirec],5);

            xcountrecs++; /* count it <=== */
            memset(DOCUMENT,0x00,maxprim*sizeof(int));
            for (dirp=VMFRdir(zirec), xdir=0, loop=VMFRnvf(zirec); loop--; dirp++, xdir++) {
                int ucloop;
                unsigned char *p;
                if (dirp->tag != parmtag) continue;
                fldp=VFIELDP(zirec,xdir);
                /* convert words to upper case */
                if (!trigrams) if (parmuppercase)
                    for (p=(unsigned char *)fldp, ucloop=dirp->len; ucloop--; p++) *p=isisuctab[*p];
                hidx=hashindex(table,maxprim,parmwidth,fldp,dirp->len,&found,1);
                if (parmtrace & 0x01) {
                    printf(" 1|%"_LD_"|%s\n",mfn,table+hidx*hwidth);
                }
                if (!DOCUMENT[hidx]) {
                     DOCUMENT[hidx]++; COLLECTION[hidx]++;
                }
    	    }
            countcollection++;

#if PARMDTAG6W
/*2*/
        } /* end for each 6words line */
/*2*/
#endif /* PARMDTAG6W */
#if PARMDTAG
/**/
                 } /* end if dtagfldlen */ /* process each line */
                 if (parmdtag) while (*dtagp == dtagc1 || *dtagp == dtagc2) dtagp++;
                 dtagfldp=dtagp; dtagfldlen=0;
                 if (!*dtagfldp) break;
            }
            else {
                dtagp++; dtagfldlen++;
            }
        } /* end for dtagp */
/**/
#endif /* PARMDTAG */

            count++;
            if (parmtell) if (count%parmtell == 0) fprintf(stderr,"+ %"_LD_"+%"_LD_" +%"_LD_"\n",count,xcount,xcountrecs);
        }
        if (!usercollectionsize) if (!readcollectionsize) COLLECTION_SIZE=countcollection;
        ///* check collection size x document maxmfn */
        //if (countcollection < COLLECTION_SIZE < maxmfn) fatal("wtrig1/countcollection");
    }

    if (parmtrace & 0x02) {
        printf(" 2|    |%"_LD_"\n",COLLECTION_SIZE);
    }

    /* discart frequent terms
    */
    for (h=table, hidx=0; hidx < maxprim; hidx++, h+=hwidth) {
        LONGX n;
        if (!*h) continue;
        n=COLLECTION[hidx];
        if (parmtrace & 0x02)
            if (COLLECTION[hidx])
                printf(" 2|%s|%"_LD_"\n",h,COLLECTION[hidx]);
#if DISCART
        if (parmmaxrf != 1.0) {
            if ((float)n/COLLECTION_SIZE > parmmaxrf) {
                memset(h,0x00,strlen(h));
                COLLECTION[hidx]=0;
            }
        }
#endif /* DISCART */
    }


    /* step 1: calculate global weights
    */
    /* calculate */
    memset(GLOBALWEIGHT,0x00,maxprim*sizeof(float));
    for (h=table, hidx=0; hidx < maxprim; hidx++, h+=hwidth) {
        if (*h) if (COLLECTION[hidx]) {
            double lognum = (double)COLLECTION_SIZE;
            double logden = (double)COLLECTION[hidx];
            GLOBALWEIGHT[hidx] = (float)sqrt(log(lognum/logden));
        }
    }


    /* discart terms with weight=0.00
    */
    for (h=table, hidx=0; hidx < maxprim; hidx++, h+=hwidth) {
        int parmdiscartgw0=0;
        if (!*h) continue;
#if DISCART
        if (parmmaxrf == 1.0) if (parmdiscartgw0) {
            if (GLOBALWEIGHT[hidx] == 0.00) {
                memset(h,0x00,strlen(h));
                COLLECTION[hidx]=0;
            }
        }
#endif /* DISCART */
        if (COLLECTION[hidx])
            if (parmtrace & 0x04)
                printf(" 4|%s|%"_LD_"|%f\n",h,COLLECTION[hidx],GLOBALWEIGHT[hidx]);
    }

    /* get secs_ for elapsed */
    secs_1=time(&secs_now);


    /* setup output term vectors
    */
    if (*outtermvectorsdb) {
        if (parmmaxtv*sizeof(LISTAWT1) > VRECnbytes(irec)-LEADER-sizeof(DIRSTRU)) fatal("outtermvectordb/maxmfrl overflow");
        recisis0(outtermvectorsdb);
        record(crec,outtermvectorsdb,0L);
        RECORD(zirec,outtermvectorsdb,1L); MFRstatus=ACTIVE;
    }

    /* setup output term inverted file
    */
    if (*outterminverteddb) {
        LONGX *collecp=COLLECTION;
        LONGX HK=0;
        LONGX off=0;
        recisis0(outterminverteddb);
        RECORD(crec2,outterminverteddb,0L);
        for (h=table, hidx=0; hidx < maxprim; hidx++, h+=hwidth, collecp++) {
            if (*h) HK+= *collecp;
        }
        OFFSET=     (LONGX *) loadfile(NULL,'@',"",NULL, maxprim*sizeof(LONGX),   '\0');
        COUNT=      (LONGX *) loadfile(NULL,'@',"",NULL, maxprim*sizeof(LONGX),   '\0');
        POSTINGS=   (LONGX *) loadfile(NULL,'@',"",NULL, HK*sizeof(LONGX),        '\0');
        memset(OFFSET,0x00,maxprim*sizeof(LONGX));
        memset(COUNT,0x00,maxprim*sizeof(LONGX));
        memset(POSTINGS,0x00,HK*sizeof(LONGX));
        collecp=COLLECTION;
        for (h=table, hidx=0; hidx < maxprim; hidx++, h+=hwidth) {
            if (*h) { OFFSET[hidx]=off; off+=COLLECTION[hidx]; }
        }
//for (h=table, hidx=0; hidx < maxprim; hidx++, h+=hwidth)
//  if (*h) printf("OFFSET=%"_LD_"  %"_LD_"  h=%"_LD_"=%s\n",OFFSET[hidx],COLLECTION[hidx],hidx,h);
    }



    /* --------------------------------------------------
       step2: read all records to calculate local weights
    */
    /* step2: read all records to calculate local weights
    */
    countcollection=0;
    countok=xcount=ycount=0; xpcount=0;

    if (!*outtermvectorsdb) parmx12345=0;

    if (parmx12345) { /* 12345 */

    for (docmfn=parmfrom; ; docmfn+=parmloop) {
        LISTAWT1 *listhdr,*listtail,*avail,*l;
        LONGX zcount,zcount2;
        int *docump;
        float soma2,fator,invsqrts2;
        float *prodp;

        FFI dtagfldlen=0;
        unsigned char *dtagfldp;
        unsigned char *dtagp;

        if (parmto) if (mfn > parmto) break;
        if (parmcount) if (xpcount++ >= parmcount) break;

        /* read indocument */
        RECORD(irec,indocumentsdb,docmfn);
        if (RECrc == RCEOF) break;
        if (RECrc != RCNORMAL) { /* skip deleted / inexistent */
            /*xcount++;*/
            continue;
        }

        /* process extract=<fmt> */
#if PARMDTAG
        if (extractfmtp)
            fmt_inter(extractfmtp,irec,buffsize,dtagbuff,buffsize);
        else
            recfield(dtagbuff,irec,parmtag,1,"");
#else /* PARMDTAG */
        if (extractfmtp) {
            char *lenp=buff+7;
            int x=17;
                      /* +012345678901234567 */
            sprintf(buff,"d*H999 xxxxxxxxx ");
            fmt_inter(extractfmtp,irec,buffsize-x-1,buff+x,buffsize-x-1);
            sprintf(lenp,"%9"_LD_,(LONGX)strlen(buff+x)); buff[x-1]=' ';
            if (fldupdat(irec,buff)) fatal(buff);
        }
#endif /* PARMDTAG */

#if PARMDTAG
/**/
        /* process each line
        */
        for (dtagfldlen=0, dtagp=dtagfldp=dtagbuff; ; ) {
            char zilang[BUFSIZ]={'\0'};
            int langidx=(-1);
            if (!*dtagp || *dtagp == dtagc1 || *dtagp == dtagc2) {
                /* empty record */
                MFRmfrl=MFRbase=LEADER; MFRnvf=0;
                if (dtagfldlen) {
                    char *p=dtagfldp;
                    FFI langlen=0;
                    char *langp;
                    if (dtagfldlen >= 2) if (*p++ == '^') if (*p++ == 'l') {
                        FFI skip=2;
                        FFI left=dtagfldlen-skip;
                        langp=p;
                        while (left--) {
                            if (*p++ == '^') {
                                skip++; if (*p == 'x') skip++;
                                break;
                            }
                            langlen++;
                        }
                        skip+=langlen;
                        dtagfldlen-=skip;
                        dtagfldp+=skip;
                        if (langlen>=BUFSIZ) langlen=BUFSIZ-1;
                        memmove(zilang,langp,langlen); zilang[langlen]='\0';
#if PARMLANG
    if (*zilang) {
        int xlang;
        /* search&store language */
        langidx=(-1);
        for (xlang=0; xlang < nlangs; xlang++) {
            if (strcmp(zilang,langsname[xlang]) == 0) { langidx=xlang; break; }
        }
        if (langidx < 0) {
            /* allocate languages bit strings */
            if (nlangs >= MAXPARMLANG) fatal("wtrig1/terminverted/MAXPARMLANG");
            hitbytes=COLLECTION_SIZE/8+1; /* bit string size */
            HITS=  (char  *)loadfile(NULL,'@',"",NULL,hitbytes,'\0');
            /* init HIT list = documents having 1+ of document results in this lang */
            memset(HITS,0x00,hitbytes);
            /* store */
            langidx=nlangs;
            langshits[nlangs]=HITS; langsname[nlangs]=strdup(zilang); langscount[nlangs++]=0;
        }
    }
#endif /* PARMLANG */
                    }
                    p=buff;
                    sprintf(p,"H%"_LD_" %"_LD_" ",(LONGX)parmtag,(LONGX)dtagfldlen); p+=strlen(p);
                    memmove(p,dtagfldp,dtagfldlen); *(p+=dtagfldlen)='\0'; dtagfldlen=0;
                    if (fldupdat(irec,buff)) fatal("wtrig1/dtag/fldupdat/2");
/**/
#endif /* PARMDTAG */

        /* process extract proc Gsplit=tag=6words */
        if (parm6words) {
            if (fldupdat(irec,areacmd6words)) fatal(areacmd6words);
            if (fldupdat(irec,areacmd6words2)) fatal(areacmd6words2);
        }

#if PARMDTAG6W
/*2*/
        /* for each 6words line
        */
        for (zdirp=MFRdir, zxdir=0, zloop=MFRnvf; zloop--; zdirp++, zxdir++) {
            char *zp;
            FFI n;
            if (zdirp->tag != parmtag) continue;
            zfldp=FIELDP(zxdir);
            if (parmdtag) if (parmsavetext) {
                /* save original text */
                n=sizeof(savetext)-1;
                if (DIRlen(zxdir) < n) n=DIRlen(zxdir);
                memmove(savetext,zfldp,n); savetext[n]='\0';
            }
            /* exclude common words */
            if (*parmexcludedb) {
                TRMSTRU *trmp;
                unsigned char *p,*q;
                int ucloop;
                for (q=buff, p=(unsigned char *)zfldp, ucloop=zdirp->len; ucloop--; p++) *q++=isisuctab[*p];
                *q='\0';
                TERM(itrm,parmexcludedb,buff);
                if (TRMrc == RCNORMAL) zfldp=NULL;
            }
            /* build trigrec */
            zp=buff;
            sprintf(zp,"D*"); zp+=strlen(zp);
            if (zfldp) {
                sprintf(zp,"H%"_LD_" %"_LD_" ",(LONGX)parmtag,(LONGX)zdirp->len); zp+=strlen(zp);
                memmove(zp,zfldp,zdirp->len); *(zp+zdirp->len)='\0';
            }
            if (fldupdat(zirec,buff)) fatal("wtrig1/line/fldupdat/2");
/*2*/
#endif /* PARMDTAG6W */

        /* go ahead */
        if (countcollection >= COLLECTION_SIZE)
            fatal("wtrig1/COLLECTION_SIZE/dtag");
        countcollection++;

        /* process extract proc Gsplit=tag={trigrams|words} */
        if (fldupdat(zirec,areacmd)) fatal(areacmd);

        /* setup list of relevant terms
        */
        memset(lista,0x00,parmmaxtv*sizeof(LISTAWT1));
        for (l=lista, avail=lista+1, loop=0; (LONGX)loop<(parmmaxtv-1); loop++, l++, avail++)
            l->avail=avail;
        listhdr=listtail=avail=lista;
        zcount=zcount2=0;

        /* zero and build f(t)
        */
        memset(DOCUMENT,0x00,maxprim*sizeof(int));
        for (dirp=VMFRdir(zirec), xdir=0, loop=VMFRnvf(zirec); loop--; dirp++, xdir++) {
            FFI ucloop;
            unsigned char *p;
            if (dirp->tag != parmtag) continue;
            fldp=VFIELDP(zirec,xdir);
            /* convert words to upper case */
            if (!trigrams) if (parmuppercase)
                for (p=(unsigned char *)fldp, ucloop=dirp->len; ucloop--; p++) *p=isisuctab[*p];

            hidx=hashindex(table,maxprim,parmwidth,fldp,dirp->len,&found,0);
            if (!found) {
#if DISCART
                continue;
#else /* DISCART */
                /* bug compiler/code/unsigned char */
                fldp[dirp->len]='\0'; /*!*/
                fprintf(stderr,"*** fldp=%s",fldp);
                /* bc5 must be compiler/code/unsigned char */
                fatal("wtrig1/f(t)/term not found in collection");
#endif /* DISCART */
            }
            /* optimize term processing */
            if (!COLLECTION[hidx]) continue;
            /* count term */
            DOCUMENT[hidx]++;
        }

        /* calculate localweight x globalweight
        */
        memset(PRODWEIGHT,0x00,maxprim*sizeof(float));
        soma2=0;
        for (docump=DOCUMENT, h=table, hidx=0; hidx < maxprim; hidx++, h+=hwidth, docump++) {
            if (*h) if (*docump) {
                  double logarg = (double)(1 + *docump);
                  LOCALWEIGHT = (float)log(logarg);
                  PRODWEIGHT[hidx] = LOCALWEIGHT*GLOBALWEIGHT[hidx];
                  fator=PRODWEIGHT[hidx];
                  soma2+=fator*fator;
                  if (parmtrace & 0x08)
                    printf(" 8|%"_LD_"|%s|%d\n",docmfn,h,*docump);
                  if (parmtrace & 0x10)
                    printf("16|%"_LD_"|%s|%f\n",docmfn,h,LOCALWEIGHT);
                  if (parmtrace & 0x20)
                    printf("32|%"_LD_"|%s|%f\n",docmfn,h,PRODWEIGHT[hidx]);
            }
        }

        /* calculate normalization factor = sqrt of sum of squares
        */
	    if (soma2) invsqrts2=1/sqrt(soma2);
        if (parmtrace & 0x40)
            printf("64|%"_LD_"|    |%f|%f\n",docmfn,soma2,invsqrts2);

        /* skip empty vectors */
        if (!soma2) {
            ycount++;
            continue;
        }

        /* calculate final term weight = local x global x normalization
        */
        for (prodp=PRODWEIGHT, h=table, hidx=0; hidx < maxprim; hidx++, h+=hwidth, prodp++) {
            if (*h) if (*prodp) {
                LISTAWT1 *l,*prev;
                float nextw;
                float PRODW= *prodp;
                TERMWEIGHT = PRODW * invsqrts2;
                prev=NULL;
                l=NULL; if (listhdr->weight) l=listhdr;
                while (l) {
                    if (TERMWEIGHT < l->weight) { prev=l; l=l->next; }
                    else break;
                }
                nextw=((l)?l->weight:0.0);
                if (avail) {
                        LISTAWT1 *nextavail=avail->avail;
                        avail->weight=TERMWEIGHT; avail->hidx=hidx;
                        avail->prev=prev;
                        avail->next=l;
                        if (prev) prev->next=avail;
                        if (nextw) l->prev=avail;
                        if (!avail->prev) listhdr=avail;
                        if (!avail->next) listtail=avail;
                        avail=nextavail;
                        if (parmtrace2) printf("64a|");
                        zcount++;
                }
                else {
                    if (TERMWEIGHT > listtail->weight) {
                        LISTAWT1 *last=listtail;
                        LISTAWT1 *repl=last;
                        LISTAWT1 *lnext=NULL;
                        if (l) lnext=l->next;
                        if (last->prev) {
                            listtail=last->prev;
                            listtail->next=NULL;
                        }
                        repl->weight=TERMWEIGHT; repl->hidx=hidx;
                        repl->prev=prev;
                        if (prev) prev->next=repl;
                        if (l) if (lnext) {
                            repl->next=l;
                            l->prev=repl;
                        }
                        if (!repl->prev) listhdr=repl;
                        if (!repl->next) listtail=repl;
                        if (parmtrace2) printf("64b|");
                        zcount2++;
                    }
                }
                if (parmtrace2) {
                  printf("%"_LD_"|%s|%f\n",docmfn,h,TERMWEIGHT);
                  for (l=listhdr; l; l=l->next) {
                    char *p;
                    if (!l) break;
                    p=table+(l->hidx*hwidth);
                    printf("%"_LD_"|%"_LD_"=%s|%f p=%"_LD_"|n=%"_LD_"|a=%"_LD_" hdr=%"_LD_"|lst=%"_LD_"|prev=%"_LD_"\n",docmfn,l->hidx,p,l->weight,
                      (l->prev)?((LISTAWT1 *)(l->prev))->hidx:-1,
                      (l->next)?((LISTAWT1 *)(l->next))->hidx:-1,
                      (l->avail)?((LISTAWT1 *)(l->avail))->hidx:-1,
                      (listhdr->hidx)?listhdr->hidx:-1,
                      (listtail->hidx)?listtail->hidx:-1,
                      (prev)?prev->hidx:-1);
                  }
                  printf("\n");
                }
            } /* end if h */
        } /* end for hidx */

        /* list term vector
        */
        if (parmtrace & 0x40) for (l=listhdr; l; l=l->next) {
            if (!l->weight) break;
            hidx=l->hidx; h=table+hidx*hwidth;
            printf("64|%"_LD_"|%s|%f\n",docmfn,h,l->weight);
        }

        /* output term vector for the document
        */
        outmfn++;
        if (*outtermvectorsdb) {
            char *batchp=buff;
            if (parmdtag) VMFRmfn(zirec)=outmfn; else VMFRmfn(zirec)=docmfn;
            sprintf(batchp,"D*"); batchp+=strlen(batchp);
            if (parmdtag || 1) {
                sprintf(batchp,"<1 0>%"_LD_"</1>",docmfn); batchp+=strlen(batchp);
            }
            if (*zilang) {
                sprintf(batchp,"<2 0>%s</2>",zilang); batchp+=strlen(batchp);
            }
            if (parmdtag) if (parmsavetext) {
                sprintf(batchp,"<3 0>%s</3>",savetext); batchp+=strlen(batchp);
            }

            if (listhdr->weight) {
                int parmwritebinv=parmwritebin;

                if (fldupdat(zirec,buff)) fatal("wtrig1/outtermvector/fldupdat/1");
                if (parmwritebinv) {
                    char *p;
                    int n,n1,n2;
                    unsigned short int usi_w;
                    DIRSTRU *dirp;
                    int xdir=VMFRnvf(zirec);
                    sprintf(buff,"<4 0></4>");
                    if (fldupdat(zirec,buff)) fatal("wtrig1/outtermvector/fldupdat/2");
                    dirp=VMFRdir(zirec); dirp+=xdir;
                    p=VFIELDP(zirec,xdir);
                    for (l=listhdr; l; l=l->next) {
                        if (!l->weight) break;
                        hidx=l->hidx; h=table+hidx*hwidth;
                        TERMWEIGHT=l->weight;
                        usi_w=(unsigned short int)(USHRT_MAX*TERMWEIGHT);
                        memmove(p,(void *)&usi_w,sizeof(usi_w)); n1=sizeof(usi_w);  p+=n1;
                        memmove(p,h,n2=strlen(h)+1); p+=n2; n=n1+n2;
/*
                        memmove(p,h,hwidth); n=hwidth; p+=n;
                        memmove(p,(void *)&TERMWEIGHT,sizeof(TERMWEIGHT)); n+=sizeof(TERMWEIGHT);  p+=sizeof(TERMWEIGHT);
                        memmove(p,(void *)&DOCUMENT[hidx],sizeof(DOCUMENT[hidx])); n+=sizeof(DOCUMENT[hidx]);  p+=sizeof(DOCUMENT[hidx]);
*/
                        dirp->len+=n; VMFRmfrl(zirec)=VMFRmfrl(zirec)+n;
                    }
                }
                else { /* parmwritebin */
                    for (l=listhdr; l; l=l->next) {
                        if (!l->weight) break;
                        hidx=l->hidx; h=table+hidx*hwidth;
                        TERMWEIGHT=l->weight;
                        sprintf(batchp,"<4 0>%s^w%f^n%d</4>",h,TERMWEIGHT,DOCUMENT[hidx]);
                        batchp+=strlen(batchp); 
                    }
                    if (fldupdat(zirec,buff)) fatal("wtrig1/outtermvector/fldupdat/3");
                } /* parmwritebin */


                recupdat(crec,zirec); /* cisis_wwtrig2(WTRIG2HANDLEP)*/

            } /* end if listhdr->weight */
        }

        /* add postings to the inverted file
        */
        if (*outterminverteddb) {
            LONGX mfn=outmfn;
            for (l=listhdr; l; l=l->next) {
                LONGX *postp;
                if (!l->weight) break;
                hidx=l->hidx;
                if (hidx >= maxprim) {
                    printf("%"_LD_"|%s|fatal: wtrig1/terminverted/hidx\n",mfn,h);
                    fatal("wtrig1/terminverted/hidx");
                }
                if (COUNT[hidx] > COLLECTION[hidx]) {
                    printf("%"_LD_"|%s|fatal: wtrig1/terminverted/count\n",mfn,h);
                    for (l=listhdr; l; l=l->next) {
                        char *p;
                        if (!l) break;
                        p=table+(l->hidx*hwidth);
                        printf("%"_LD_"|%"_LD_"=%s|%f p=%"_LD_"|n=%"_LD_"|a=%"_LD_" hdr=%"_LD_"|lst=%"_LD_"|prev=%"_LD_"\n",mfn,l->hidx,p,l->weight,
                          (l->prev)?((LISTAWT1 *)(l->prev))->hidx:-1,
                          (l->next)?((LISTAWT1 *)(l->next))->hidx:-1,
                          (l->avail)?((LISTAWT1 *)(l->avail))->hidx:-1,
                          (listhdr->hidx)?listhdr->hidx:-1,
                          (listtail->hidx)?listtail->hidx:-1);
                    }
                    printf("\n");
                    fatal("wtrig1/terminverted/count");
                }
                postp=POSTINGS+OFFSET[hidx]+COUNT[hidx];
                *postp=mfn; COUNT[hidx]++;
            }
#if PARMLANG
            /* add posting to the language bit strigs */
            if (*zilang) if (langidx >= 0) {
                char *HITS=langshits[langidx];
                if (outmfn < 1) fatal("wtrig1/terminverted/outmfn invalid/1");
                if (outmfn > COLLECTION_SIZE) fatal("wtrig1/terminverted/outmfn invalid/2");
                hitmask =hitbitmask[outmfn&BY8RMASK];
                hitoffset = outmfn/8;
                HITS[hitoffset] |= hitmask;
                if (parmtrace) printf(" x|%"_LD_"|%s|%d|%d|%"_LD_"\n",docmfn,zilang,langidx,nlangs,outmfn);
            }
#endif /* PARMLANG */
        }

#if PARMDTAG6W
/*2*/
        } /* end for each 6words line */
/*2*/
#endif /* PARMDTAG6W */
#if PARMDTAG
/**/
                 } /* end if dtagfldlen */ /* process each line */
                 if (parmdtag) while (*dtagp == dtagc1 || *dtagp == dtagc2) dtagp++;
                 dtagfldp=dtagp; dtagfldlen=0;
                 if (!*dtagfldp) break;
            }
            else {
                dtagp++; dtagfldlen++;
            }
        } /* end for dtagp */
/**/
#endif /* PARMDTAG */

        countok++;
        if (parmtell2) if (countok%parmtell2 == 0) fprintf(stderr,"++ %"_LD_"+%"_LD_" %"_LD_" %"_LD_"+%"_LD_"\n",countok,xcount,ycount,zcount,zcount2);
    } /* end for mfn */

    } /* end if x12345 */

    /* get secs_ for elapsed */
    secs_2=time(&secs_now);


    /* rellocate hashing table of collection terms (word/trigrams)
    */
    //if (parmtell || parmtrace) printf("+");
    //if (parmtell || parmtrace) printf("+++ relloc'ing \n");
    if (parmtell2) fprintf(stderr,"++ %"_LD_" recs \n",countok);
    if (parmtell2) fprintf(stderr,"++ %"_LD_" vectors \n",outmfn);
    if (parmrelloc)
      for (nterms=0, p=h=table, hidx2=hidx=0; hidx < maxprim; hidx++, h+=hwidth) {
        if (!*h) continue;
        if (hidx2 != hidx) {
            memmove(p,h,hwidth); memset(h,0x00,hwidth);
            COLLECTION[hidx2]=COLLECTION[hidx]; COLLECTION[hidx]=0;
            GLOBALWEIGHT[hidx2]=GLOBALWEIGHT[hidx]; GLOBALWEIGHT[hidx]=0;
        }
        INDEX[hidx2]=hidx;
        p+=hwidth; hidx2++; nterms++;
      }
    else
      for (nterms=0, h=table, hidx=0; hidx < maxprim; hidx++, h+=hwidth) {
        INDEX[hidx]=hidx;
        if (!*h) continue;
        nterms++;
      }
    //if (!nterms) fatal("wtrig1/outcollectionoccs/zero terms");

    if (parmtrace & 0x04)
      for (h=table, hidx=0; hidx < maxprim; hidx++, h+=hwidth) {
        if (!*h) continue;
        printf("x4|%s|%"_LD_"|%f\n",h,COLLECTION[hidx],GLOBALWEIGHT[hidx]);
      }


    /* sort collection by term (word/trigrams)
    */
    if (parmrelloc) if (parmsort) {
      //if (parmtell || parmtrace) printf("+");
      if (parmtell || parmtrace) printf("+++ sorting %"_LD_" terms \n",nterms);
      if (parmtell2) parmtell2=nterms/5;
      for (h=table, hidx=0, hloop=nterms-1; hidx < hloop; hidx++, h+=hwidth) { /* x:x+1 */
        if (parmtell2) if ((hidx+1)%parmtell2 == 0) fprintf(stderr,"+++ %"_LD_"/5 \n",(LONGX)hidx/parmtell2+1);
        for (h2=h+hwidth, hidx2=hidx+1; hidx2 < nterms; hidx2++, h2+=hwidth) {
            LONGX index;
            LONGX n;
            float w;
            if (memcmp(h,h2,hwidth) == 0) fatal("wtrig1/outcollectionoccs/duplicated key");;
            if (memcmp(h,h2,hwidth) < 0) continue;
            memmove(buff,h,hwidth);
            memmove(h,h2,hwidth);
            memmove(h2,buff,hwidth);
            index=INDEX[hidx];
            INDEX[hidx]=INDEX[hidx2];
            INDEX[hidx2]=index;
            n=COLLECTION[hidx];
            COLLECTION[hidx]=COLLECTION[hidx2];
            COLLECTION[hidx2]=n;
            w=GLOBALWEIGHT[hidx];
            GLOBALWEIGHT[hidx]=GLOBALWEIGHT[hidx2];
            GLOBALWEIGHT[hidx2]=w;
        }
      }
    }

    /* get secs_ for elapsed */
    secs_3=time(&secs_now);

    if (parmtrace & 0x04)
      for (h=table, hidx=0; hidx < maxprim; hidx++, h+=hwidth) {
        if (!*h) continue;
        printf("y4|%s|%"_LD_"|%f|%"_LD_"\n",h,COLLECTION[hidx],GLOBALWEIGHT[hidx],INDEX[hidx]);
      }


    /* output collection occs for each term
    */
    //if (parmtell || parmtrace) printf("+");
    if (*outcollectionoccsdb) {
        char *batchp;
        LONGX mfn=1;
        if (parmtell || parmtrace) printf("+++ writing global frequencies to %s\n",outcollectionoccsdb);
        recisis0(outcollectionoccsdb);
        RECORD(crec,outcollectionoccsdb,0L);
        for (h=table, hidx=0; hidx < maxprim; hidx++, h+=hwidth) {
        //for (h=table, hidx=0; hidx < nterms; hidx++, h+=hwidth) {
            if (*h) {
                batchp=buff;
                if (parmshowh)
                    sprintf(batchp,"<3 0>%s^n%"_LD_"^w%f^h%"_LD_"</3>",h,COLLECTION[hidx],GLOBALWEIGHT[hidx],INDEX[hidx]);
                else
                    sprintf(batchp,"<3 0>%s^n%"_LD_"^w%f</3>"     ,h,COLLECTION[hidx],GLOBALWEIGHT[hidx]);
                batchp+=strlen(batchp);
                RECORD(irec,outcollectionoccsdb,mfn); MFRstatus=ACTIVE;
                if (fldupdat(irec,buff)) fatal("wtrig1/outcollectionoccs/fldupdat");
                recupdat(crec,irec); mfn++;
            }
        }
    }

    /* output term inverted file
    */
    //if (parmtell || parmtrace) printf("+");
    if (*outterminverteddb) {
        LONGX count;
        LONGX *postp;
        char *batchp;
        RECSTRU *recp;
        if (parmtell || parmtrace) printf("+++ writing list of postings to %s\n",outterminverteddb);
        //allocate larger output record /* 20110812 */
        for (h=table, hidx=0; hidx < maxprim; hidx++, h+=hwidth) {
            if (!*h) continue;
            count=COUNT[INDEX[hidx]];
            if (count > maxhits) maxhits=count; 
        }
        xirec=nrecs;
        recallok(xirec,BUFSIZ+maxhits*sizeof(*postp)); /* 20110812 */
        for (h=table, hidx=0; hidx < maxprim; hidx++, h+=hwidth) {
        //for (h=table, hidx=0; hidx < nterms; hidx++, h+=hwidth) {
            if (!*h) continue;
            count=COUNT[INDEX[hidx]];
            if (count) {
                /* write term postings in record mfn=hidx+1 */
                RECORD(xirec,outterminverteddb,hidx+1); MFRstatus=ACTIVE;
                batchp=buff;
                sprintf(batchp,"<33 0>%s^n%"_LD_"</33>",h,count); batchp+=strlen(batchp);
                postp=POSTINGS+OFFSET[INDEX[hidx]];
                if (parmwritebin) {
                    char *p,*q;
                  // 20110812
                  //int n;
                    LONGX n;
                    sprintf(batchp,"<5 0></5>");
                    if (fldupdat(xirec,buff)) fatal("wtrig1/outtermvinverted/bin/fldupdat");
                    dirp=MFRdir; dirp++;
                    p=FIELDP(1);
                  // 20110812
                  //    n=sizeof(*postp);
                  //    if (MFRmfrl+count*n > RECnbytes) fatal("wtrig1/outtermvinverted/RECnbytes"); /* 20110811 */
                  //for (; count--; postp++) {
                  //    q=(char *)postp;
                  //    memmove(p,q,n); p+=n;
                  //    dirp->len+=n; MFRmfrl+=n;
                  //}
                    q=(char *)postp;
                    n=sizeof(*postp)*COUNT[INDEX[hidx]]; 
                    memmove(p,q,(size_t)n);
                    dirp->len+=n; MFRmfrl+=n;
                }
                else {
                    for (; count--; postp++) {
                        sprintf(batchp,"<5 0>%"_LD_"</5>",*postp); batchp+=strlen(batchp);
                    }
                    if (fldupdat(xirec,buff)) fatal("wtrig1/outtermvinverted/fldupdat");
                }
                recupdat(crec2,xirec);
            }
        }
#if PARMLANG
        /* add posting to the language bit strigs */
        for (langidx=0; langidx < nlangs; langidx++) {
                char *HITS=langshits[langidx];
                LONGX n;
                /* write term postings in record mfn=nterms+(langidx+1) */
                RECORD(xirec,outterminverteddb,nterms+langidx+1); MFRstatus=ACTIVE;
                batchp=buff;
                sprintf(batchp,"<34 0>%s^n%"_LD_"^b%"_LD_"</34><35 0></35>",langsname[langidx],langscount[langidx],hitbytes); batchp+=strlen(batchp);
                if (fldupdat(xirec,buff)) fatal("wtrig1/outtermvinverted/langs/fldupdat");
                n=hitbytes;
                dirp=MFRdir; dirp++;
                memmove(FIELDP(1),HITS,n); dirp->len+=n; MFRmfrl+=n;
                recupdat(crec2,xirec);
        }
#endif /* PARMLANG */
    }

    /* get secs_ for elapsed */
    secs_4=time(&secs_now);

    /* output collection size
    */
    //if (parmtell || parmtrace) printf("+");
    if (*outcollectionsizedb) {
        char *batchp=buff;
        if (parmtell || parmtrace) printf("+++ writing parms and documents' collection data to %s\n",outcollectionsizedb);
        recisis0(outcollectionsizedb);
        RECORD(crec,outcollectionsizedb,0L);
        RECORD(irec,outcollectionsizedb,1L); MFRstatus=ACTIVE;
        sprintf(batchp,"<1 0>^ndocuments^v%s</1>",indocumentsdb);
        batchp+=strlen(batchp);
        if (extractfmta)
#if BEFORE20051103
            sprintf(batchp,"<1 0>^nextract^v%s</1>",extractfmta);  /* extractfmta may contains </1> */
#else
            sprintf(batchp,"H 1 %d ^nextract^v%s",strlen("^nextract^v")+strlen(extractfmta),extractfmta);
#endif
        else
            sprintf(batchp,"<1 0>^nextract^v%"_LD_"</1>",(LONGX)parmtag);
        batchp+=strlen(batchp);
        if (collectiondb) {
            sprintf(batchp,"<1 0>^ncollection^v%s</1>",collectiondb);
            batchp+=strlen(batchp);
        }
        sprintf(batchp,"<1 0>^nwidth^v%d^t%s</1>",parmwidth,(trigrams)?"trigrams":"words");
        batchp+=strlen(batchp);
        sprintf(batchp,"<1 0>^ncase^v%s</1>",parmuppercase);
        batchp+=strlen(batchp);
        sprintf(batchp,"<1 0>^nK^v%"_LD_"</1>",parmcategories);
        batchp+=strlen(batchp);
        sprintf(batchp,"<1 0>^nmaxrf^v%f</1>",parmmaxrf);
        batchp+=strlen(batchp);
        sprintf(batchp,"<1 0>^nmaxtv^v%d</1>",parmmaxtv);
        batchp+=strlen(batchp);
        if (parmdtag) {
            sprintf(batchp,"<1 0>^ndmfn^v%d</1>",parmdtag);
            batchp+=strlen(batchp);
        }
        if (parm6words) {
            sprintf(batchp,"<1 0>^n6words^v%d</1>",parm6words);
            batchp+=strlen(batchp);
        }
        if (*parmexcludedb) {
            sprintf(batchp,"<1 0>^nexclude^v%s</1>",parmexcludedb);
            batchp+=strlen(batchp);
        }
        sprintf(batchp,"<1 0>^nbin^v%d</1>",parmwritebin);
        batchp+=strlen(batchp);
        sprintf(batchp,"<1 0>^nmaxprim^v%"_LD_"</1>",maxprim);
        batchp+=strlen(batchp);
        sprintf(batchp,"<1 0>^nnterms^v%"_LD_"</1>",nterms);
        batchp+=strlen(batchp);
#if PARMLANG
        sprintf(batchp,"<1 0>^nlanguages^v%d</1>",nlangs);
        batchp+=strlen(batchp);
#endif /* PARMLANG */
        sprintf(batchp,"<1 0>^nndocs^v%"_LD_"</1>",countok);
        batchp+=strlen(batchp);
        sprintf(batchp,"<1 0>^nndocsread^v%"_LD_"</1>",countok+xcount);
        batchp+=strlen(batchp);
        sprintf(batchp,"<1 0>^nelapsed1^v%"_LD_"</1>",(LONGX)secs_1-secs_start);
        batchp+=strlen(batchp);
        sprintf(batchp,"<1 0>^nelapsed2^v%"_LD_"</1>",(LONGX)secs_2-secs_start);
        batchp+=strlen(batchp);
        sprintf(batchp,"<1 0>^nelapsed3^v%"_LD_"</1>",(LONGX)secs_3-secs_start);
        batchp+=strlen(batchp);
        sprintf(batchp,"<1 0>^nelapsed4^v%"_LD_"</1>",(LONGX)secs_4-secs_start);
        batchp+=strlen(batchp);
        sprintf(batchp,"<1 0>^ndate^v%04d%02d%02d %02d%02d%02d</1>",
            1900+tp->tm_year,tp->tm_mon+1,tp->tm_mday,tp->tm_hour,tp->tm_min,tp->tm_sec);
        batchp+=strlen(batchp);
        sprintf(batchp,"<2 0>%"_LD_"</2>",COLLECTION_SIZE);
        batchp+=strlen(batchp);
        sprintf(batchp,"<3 0>%"_LD_"</3>",maxhits);
        //printf("\n%s\n",buff);
        if (fldupdat(irec,buff)) fatal("wtrig1/outcollectionsize/fldupdat");
        recupdat(crec,irec);
    }

    if (parmtell || parmtrace) printf("\n");

    exit(0);  /* AOT, 30/07/2004 - thanks MB */

} /* end of main */



