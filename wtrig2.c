/* Program WTRIG2: trigrams 1B
*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>

#define MAXNREC (0xFFFF+12+1)  /* set CISIS Interface to 64K records in memory  */

//#define CIWTF 0  

#include "cisis.h"   /* CISIS Interface header file */
#include "cirun.h"   /* CISIS Interface runtime declarations */
//#include "cihsh.h"   /* CISIS Interface hashing & binary table search */

//#if !PC
#ifndef itoa
char *itoa(int value, char *xa, int len);
char *itoa(int value, char *xa, int len)
{
  if (len) sprintf(xa,"%d",value);
}
#endif

#define TAG1   1
#define TAG2   2
#define TAG3   3
#define TAG4   4
#define TAG5   5
#define TAG33 33

#define TAG34 34
#define TAG35 35

#define MAXPARMWIDTH  (LE2+LE2)

#if !CIWTF
typedef struct vectel {
    float weight;
    LONGX hidx;
} VECTEL;

typedef struct xlista {
    float sim;
    LONGX mfn;
    LONGX xxx;
    LONGX hit;
    void *prev;
    void *next;
    void *avail;
} LISTA;
#endif //CIWTF

void main(int argc, char *argv[])
{
    LISTA *lista; /* list of related */
    LISTA *listhdr,*listtail,*listavail; /* list of related */


    VECTEL *VECTORS;
    LONGX *VECTOFF;
    LONGX *VECTCNT;
    LONGX *VECTCMF;

    LONGX COLLECTION_SIZE;     /* N - #docs in collection */

    float *WEIGHT;      /* WEIGHT[] - readnterms */
    VECTEL *vector;     /*          -         */

    char  *HITS;        /* HITS[] */
    LONGX hitbytes;
    unsigned char hitbitmask[8] = { 0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01 };
    unsigned char hitmask;
    LONGX hitmfn;
    LONGX hitoffset;

    char *table;
    LONGX tabentries;
    //LONGX maxprim;
    //char *h;
    int hwidth;
    int found;

    RECSTRU *recp;  /* mandatory for defines REC,MFR,DIR,FIELDP */

    LONGX irec;
    LONGX irectv;
    LONGX outcrec;
    LONGX outirec;

    LONGX vifrec[MAXNREC];
    LONGX usedifrecs;
    LONGX leftifrecs;
    LONGX newparmloadinverted=0;
    int maxmfrlisincipar=0;

    LONGX docmfn;
    DIRSTRU *docdirp;
    int docxdir;
    FFI docloop;
    LONGX dochidx;

    LONGX mfn;
    DIRSTRU *dirp;
    int xdir,iocc;
    char *p,*q;
    char *fldp;
    int left;
    FFI loop,keylen;

    char *buff=NULL;

    LONGX count;
    int iarg;

    int parmcollapse=0;
    LONGX currentmfncollapsed=0;

    int parmcollmfntag=0;
    LONGX collhitmfn;

    int readwidth= -1;
    LONGX readcategories= -1;
    LONGX readmaxprim= -1;
    LONGX readcollection_size= -1;
    int readmaxtv= -1;
    LONGX readnterms= -1;
    int readwritebin= -1;
    int readwritebinv;

    char collectionsizedb[CIMPL+1] = { '\0' };
    char collectionoccsdb[CIMPL+1] = { '\0' };
    char collectiontermvectorsdb[CIMPL+1] = { '\0' };
    char collectionterminverteddb[CIMPL+1] = { '\0' };

    char documenttermvectorsdb[CIMPL+1] = { '\0' };
    char outsimilardb[CIMPL+1] = { '\0' };

    char *documentsdb=NULL;
    char *indocumentsdb=NULL;
    char *parmdocumentsdb=NULL;
    char *collectiondb=NULL;
    int parmloadvectors=0;
    LONGX parmmaxrelat=10;
    float parmmaxsim=1.0;
    float parmminsim=0.7;
    int parmtrace=0;
    int parmtrace2=0;
    LONGX parmtell=0;
    LONGX parmtell2=0;
    int parmloadinverted=0;

    int parmmaxtv=0;

    int parmwsoma=1;
    char xa[20];

    int parmlanguage=0;
    //
    int readnlangs=0;

#define PARMLANG    1
#define MAXPARMLANG 32
#if PARMLANG
    char *langshits[MAXPARMLANG];
    char *langsname[MAXPARMLANG];
    LONGX langscount[MAXPARMLANG];
    int nlangs=0;
    int langidx=(-1);
    int parmchklang=0;
#endif /* PARMLANG */



    if (argc < 3) {
        printf("%s",cicopyr("Utility WTRIG2"));
        printf(" \n");
	    printf("wtrig2 documents=<mf1>[,<mf3>] collection=<mf2> \n");
        printf(" \n");
        printf(" input document term vectors from <mf1>.v \n");
        printf(" input collection term vectors from <mf2>.v \n");
        printf(" output similar collection for each input document to <mf1>.y or <mf3>.y \n");
        printf(" \n");
        printf(" options: maxrel=<max #related>                               [%"_LD_"]   \n",parmmaxrelat);
        printf("          minsim=<min similarity for related docs>            [%3.1f] \n",parmminsim);
        printf("          maxsim=<max similarity for related docs>            [%3.1f] \n",parmmaxsim);
        printf("          dmfn=<tag>   collapse doc.v as field <mf1>.v/<tag>  [%s]    \n",(parmcollapse)?itoa(parmcollapse,xa,10):"");
        printf("          cmfn=<tag>   collapse col.v as field <mf2>.v/<tag>  [%s]    \n",(parmcollmfntag)?itoa(parmcollmfntag,xa,10):"");
        printf("          dlang=<tag>  restrict col.v as field <mf1>.v/<tag>  [%s]    \n",(parmlanguage)?itoa(parmlanguage,xa,10):"");
        printf(" \n");
        printf("          doc.v=<mf>   term vector for each document          [document.v]   \n");
        printf("          col.v=<mf>   term vector for each collection doc    [collection.v] \n");
        printf("          col.c=<mf>   collection parms                       [collection.c] \n");
        printf("          col.n=<mf>   terms in collection                    [collection.n] \n");
        printf("          col.x=<mf>   posting list for each term             [collection.x] \n");
        printf(" \n");
        printf("          out.y=<mf>   similar collection for each document   [document.y] \n");
        printf(" \n");
        printf("          loadvectors  load all col.v in memory               [%s] \n",(parmloadvectors)?"loadvectors":"");
        //printf("          loadindex    load part of col.x in memory           [%s] \n",(parmloadinverted)?"loadinverted":"");
        printf("          loadindex    load all col.x in memory               [%s]     \n",(parmloadinverted)?"loadinverted":"");
        printf("          loadindex=n  load n recs of col.x in memory         [] (up to %d)\n",MAXNREC-12);
        //printf("          w2all        weight also terms outside collection   [%s]    \n",(parmwsoma)?"w2all":"");
        printf("          -w2all       ignore doc terms outside collection    [%s] \n",(parmwsoma)?"":"-w2all");
        printf("          [maxtv=<n>]  trace[={1|2|4|8|16|32|64|3..127}]      [%d] \n",parmtrace);
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
        if (strncmp(p,"documents?",10) == 0 || strncmp(p,"documents=",10) == 0) {
            parmdocumentsdb=p+10;
            if (!*parmdocumentsdb) fatal(p);
            continue;
        }
        if (strncmp(p,"collection?",11) == 0 || strncmp(p,"collection=",11) == 0) {
            collectiondb=p+11;
            if (!*collectiondb) fatal(p);
            continue;
        }
        if (strcmp(p,"loadvectors") == 0) {
            parmloadvectors=1;
            continue;
        }
        if (strcmp(p,"loadindex") == 0) {
            parmloadinverted=1;
            continue;
        }
        if (strncmp(p,"loadindex?",10) == 0 || strncmp(p,"loadindex=",10) == 0) {
            if (sscanf(p+10,"%"_LD_,&newparmloadinverted) != 1) fatal(p);
            if (newparmloadinverted < 2 || newparmloadinverted >= MAXNREC-12) fatal(p);
            parmloadinverted=1;
            continue;
        }
        if (strncmp(p,"maxrel?",7) == 0 || strncmp(p,"maxrel=",7) == 0) {
            if (sscanf(p+7,"%"_LD_,&parmmaxrelat) != 1) fatal(p);
            if (parmmaxrelat < 1) fatal(p);
            continue;
        }
        if (strncmp(p,"maxsim?",7) == 0 || strncmp(p,"maxsim=",7) == 0) {
            if (sscanf(p+7,"%f",&parmmaxsim) != 1) fatal(p);
            if (parmmaxsim < 0.0 || parmmaxsim > 1.0) fatal(p);
            continue;
        }
        if (strncmp(p,"minsim?",7) == 0 || strncmp(p,"minsim=",7) == 0) {
            if (sscanf(p+7,"%f",&parmminsim) != 1) fatal(p);
            if (parmminsim < 0.0 || parmminsim > 1.0) fatal(p);
            continue;
        }
        if (strncmp(p,"doc.v?",6) == 0 || strncmp(p,"doc.v=",6) == 0) {
            q=p+6; if (!*q) fatal(p);
            strcpy(documenttermvectorsdb,q);
            continue;
        }
        if (strncmp(p,"col.c?",6) == 0 || strncmp(p,"col.c=",6) == 0) {
            q=p+6; if (!*q) fatal(p);
            strcpy(collectionsizedb,q);
            continue;
        }
        if (strncmp(p,"col.n?",6) == 0 || strncmp(p,"col.n=",6) == 0) {
            q=p+6; if (!*q) fatal(p);
            strcpy(collectionoccsdb,q);
            continue;
        }
        if (strncmp(p,"col.v?",6) == 0 || strncmp(p,"col.v=",6) == 0) {
            q=p+6; if (!*q) fatal(p);
            strcpy(collectiontermvectorsdb,q);
            continue;
        }
        if (strncmp(p,"col.x?",6) == 0 || strncmp(p,"col.x=",6) == 0) {
            q=p+6; if (!*q) fatal(p);
            strcpy(collectionterminverteddb,q);
            continue;
        }
        if (strncmp(p,"out.y?",6) == 0 || strncmp(p,"out.y=",6) == 0) {
            q=p+6; if (!*q) fatal(p);
            strcpy(outsimilardb,q);
            continue;
        }
        if (strncmp(p,"dmfn?",5) == 0 || strncmp(p,"dmfn=",5) == 0) {
            if (sscanf(p+5,"%d",&parmcollapse) != 1) fatal(p);
            if (parmcollapse < 0) fatal(p);
            continue;
        }
        if (strncmp(p,"cmfn?",5) == 0 || strncmp(p,"cmfn=",5) == 0) {
            if (sscanf(p+5,"%d",&parmcollmfntag) != 1) fatal(p);
            if (parmcollmfntag < 0) fatal(p);
            continue;
        }
        if (strncmp(p,"dlang?",6) == 0 || strncmp(p,"dlang=",6) == 0) {
            if (sscanf(p+6,"%d",&parmlanguage) != 1) fatal(p);
            if (parmlanguage < 1) fatal(p);
            continue;
        }
        if (strcmp(p,"w2all") == 0) {
            parmwsoma=1;
            continue;
        }
        if (strcmp(p,"-w2all") == 0) {
            parmwsoma=0;
            continue;
        }
        if (strncmp(p,"maxtv?",6) == 0 || strncmp(p,"maxtv=",6) == 0) {
            if (sscanf(p+6,"%d",&parmmaxtv) != 1) fatal(p);
            if (parmmaxtv < 1) fatal(p);
            continue;
        }
        if (strncmp(p,"tell?",5) == 0 || strncmp(p,"tell=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&parmtell) != 1)
                fatal(p);
            parmtell2=parmtell;
            continue;
        }
        if (strncmp(p,"tell2?",6) == 0 || strncmp(p,"tell2=",6) == 0) {
            if (sscanf(p+6,"%"_LD_,&parmtell2) != 1)
                fatal(p);
            continue;
        }
        if (strncmp(p,"trace?",6) == 0 || strncmp(p,"trace=",6) == 0) {
            if (sscanf(p+6,"%d",&parmtrace) != 1)
                fatal(p);
            continue;
        }
        fatal(p);
    }
    /* check mandatory parms */
    if (!parmdocumentsdb) fatal("wtrig2: missing documents= parameter");

    indocumentsdb=documentsdb=strdup(parmdocumentsdb);
    if ((p=strchr(indocumentsdb,',')) != NULL) {
        *p='\0';
        documentsdb=p+1;
    }


    /* alloc working areas
    */
    p=dbxcipar(NULL,"maxmfrl",'='); if (dbxcipok) { sscanf(p,"%"_LD_,&rec_maxmfrl); maxmfrlisincipar=1; }
    recallok(irec=nrecs,rec_maxmfrl);
    recallok(irectv=nrecs,rec_maxmfrl);
    recallok(outcrec=nrecs,sizeof(M0STRU));
    recallok(outirec=nrecs,rec_maxmfrl);

    buff=loadfile(NULL,'@',"",NULL,rec_maxmfrl*2,'\0');

    if (parmtrace2) printf("+++ maxmfrl=%"_LD_"\n",rec_maxmfrl);


    /* setup filesnames
    */

    if (!*collectionsizedb)         sprintf(collectionsizedb,        "%s.c",collectiondb);
    if (!*collectionoccsdb)         sprintf(collectionoccsdb,        "%s.n",collectiondb);
    if (!*collectionterminverteddb) sprintf(collectionterminverteddb,"%s.x",collectiondb);
    if (!*collectiontermvectorsdb)  sprintf(collectiontermvectorsdb, "%s.v",collectiondb);

    if (!*documenttermvectorsdb)    sprintf(documenttermvectorsdb,   "%s.v",indocumentsdb);
    if (!*outsimilardb)             sprintf(outsimilardb,            "%s.y",documentsdb);

    /* void file names */
    if (strcmp(collectionterminverteddb,"void") == 0) {
        collectionterminverteddb[0]='\0';
        if (newparmloadinverted || parmloadinverted) fatal("wtrig2/loadindex x col.x=void");
    }

    /* get collection size, classes, width, etc from file
    */
    /* get collection size: N */
    RECORD(irec,collectionsizedb,1);
    if (RECrc != RCNORMAL) fatal("wtrig2/COLLECTION_SIZE/RCNORMAL");
    readcollection_size=atol(recfield(buff,irec,TAG2,1,""));
    /* get other parms */
    for (iocc=1; *recfield(buff,irec,TAG1,iocc,""); iocc++) {
        char *q;
        q="^nK^v";
        p=strstr(buff,q); if (p) readcategories=atol(p+strlen(q));
        q="^nwidth^v";
        p=strstr(buff,q); if (p) readwidth=atoi(p+strlen(q));
        q="^nmaxprim^v";
        p=strstr(buff,q); if (p) readmaxprim=atol(p+strlen(q));
        q="^nmaxtv^v";
        p=strstr(buff,q); if (p) readmaxtv=atoi(p+strlen(q));
        q="^nnterms^v";
        p=strstr(buff,q); if (p) readnterms=atoi(p+strlen(q));
        q="^nlanguages^v";
        p=strstr(buff,q); if (p) readnlangs=atoi(p+strlen(q));
        q="^nbin^v";
        p=strstr(buff,q); if (p) readwritebin=atoi(p+strlen(q));
    }
    if (parmmaxtv) {
        if (parmmaxtv > readmaxtv) fatal("wtrig2/invalid maxtv");
        readmaxtv=parmmaxtv;
    }
    if (readwidth > MAXPARMWIDTH) fatal("wtrig2/invalid width");

    /* check collection size x collection maxmfn */
    //RECORD(irec,collectiondb,0);
    //COLLECTION_SIZE=MF0nxtmfn-1;
    //if (readcollection_size < COLLECTION_SIZE) fatal("wtrig2/COLLECTION_SIZE");
    COLLECTION_SIZE=readcollection_size;
    if (parmtrace & 0x02) {
        printf(" 2|    |%"_LD_"\n",COLLECTION_SIZE);
    }
    readwritebinv=readwritebin;

    /* allocate bsrch table
    */
    table=bsrchalloc(readnterms,readwidth,&tabentries);

    if (readnterms > readcategories) fatal("wtrig2/invalid categories or nterms");
    if (readnterms > readmaxprim)    fatal("wtrig2/invalid maxprim or nterms");

    hwidth=(readwidth+1);

    if (parmtrace2) printf("+++ readnterms=%"_LD_"\n",readnterms);
    if (parmtrace2) printf("+++ readwidth=%"_LD_"\n",readwidth);
    if (parmtrace2) printf("+++ tabentries=%"_LD_"\n",tabentries);

    /* loadinverted */

    recallok(vifrec[0]=nrecs,rec_maxmfrl);
    RECORD(vifrec[0],collectionterminverteddb,0L);
    if (newparmloadinverted || parmloadinverted) {
        //recallok(vifrec[0]=nrecs,rec_maxmfrl);
        //RECORD(vifrec[0],collectionterminverteddb,0L);
        if (newparmloadinverted >= MF0nxtmfn-1 || parmloadinverted) {
            if (parmtrace2) printf("+ loading all %"_LD_" index records in memory\n",MF0nxtmfn-1);
            mstsetup(collectionterminverteddb,LONGX_MAX,LONGX_MAX);
            parmloadinverted=0; /* CISIS does all we need! */
        }
    }
    if (parmloadinverted) {
        LONGX nbytes=rec_maxmfrl;
        if (readwritebin) {
            nbytes = LEADER + (1+COLLECTION_SIZE)*sizeof(DIRSTRU) + readwidth + COLLECTION_SIZE*sizeof(LONGX);
            if (nbytes > rec_maxmfrl) {
                printf("*** nbytes=%"_LD_" rec_maxmfrl=%"_LD_"\n",nbytes,rec_maxmfrl);
                if (maxmfrlisincipar) fatal("wtrig2/collection size too big for default maxmfrl");
                else fatal("wtrig2/collection size too big for maxmfrl= defined in CIPAR");
            }
        }
        //recallok(vifrec[0]=nrecs,rec_maxmfrl);
        leftifrecs=newparmloadinverted-1;
        usedifrecs=0;
    }

    /* load collection terms in the hashing table
    */
    count=0;
    for (mfn=1; ; mfn++) {
        RECORD(irec,collectionoccsdb,mfn);
        if (RECrc == RCEOF) break;
        if (RECrc != RCNORMAL) continue;
        for (dirp=MFRdir, xdir=0, loop=MFRnvf; loop--; dirp++, xdir++) {
            if (dirp->tag != TAG3) continue;
            for (keylen=0, p=fldp=FIELDP(xdir), left=dirp->len; left; ) {
                    if (*p == '^') break;
                    keylen++; p++; left--;
            }
            bsrchindex(table,tabentries,readwidth,fldp,keylen,&found);
            if (found) fatal("wtrig2/duplicated collection term");
            bsrchstore(table,readnterms,&tabentries,readwidth,fldp,keylen);
        }
        count++;
        //if (parmtell) if (count%parmtell == 0) fprintf(stderr,"+ %"_LD_"\n",count);
    }
    if (parmtell) /*if (count%parmtell == 0)*/ fprintf(stderr,"+ %"_LD_" terms in collection \n",count);


    /* allocate list of related
    */
    lista=(LISTA *)loadfile(NULL,'@',"",NULL,parmmaxrelat*sizeof(LISTA),'\0');

    /* allocate area for frequencies and weights
    */
    hitbytes=COLLECTION_SIZE/8+1; /* bit string size */
    HITS=  (char  *)loadfile(NULL,'@',"",NULL,hitbytes,'\0');

    WEIGHT=(float *)loadfile(NULL,'@',"",NULL,readnterms*sizeof(float),'\0');

    if (parmloadvectors) {
        VECTOFF=  (LONGX *)loadfile(NULL,'@',"",NULL,COLLECTION_SIZE*sizeof(LONGX),'\0');
        VECTCNT=  (LONGX *)loadfile(NULL,'@',"",NULL,COLLECTION_SIZE*sizeof(LONGX),'\0');
        VECTORS=(VECTEL *)loadfile(NULL,'@',"",NULL,COLLECTION_SIZE*readmaxtv*sizeof(VECTEL),'\0');
        if (parmcollmfntag)
        VECTCMF=  (LONGX *)loadfile(NULL,'@',"",NULL,COLLECTION_SIZE*sizeof(LONGX),'\0');
    }
    else vector=(VECTEL *)loadfile(NULL,'@',"",NULL,              1*readmaxtv*sizeof(VECTEL),'\0');


    /* load all collection' term vectors
    */
    if (parmloadvectors) {
        VECTEL *v=VECTORS;
        LONGX off=0;
        LONGX *vcntp=VECTCNT;
        LONGX *voffp=VECTOFF;
        LONGX *vcmfp=VECTCMF;

        memset(VECTCNT,0x00,COLLECTION_SIZE*sizeof(LONGX));
        memset(VECTOFF,0x00,COLLECTION_SIZE*sizeof(LONGX));
        if (parmcollmfntag)
        memset(VECTCMF,0x00,COLLECTION_SIZE*sizeof(LONGX));

if (parmtell2) parmtell2=COLLECTION_SIZE/5;
        count=0;
        for (mfn=1; mfn <= COLLECTION_SIZE; mfn++) {
            LONGX cnt=0;
            RECORD(irectv,collectiontermvectorsdb,mfn);
            if (RECrc == RCEOF) {
                //COLLECTION_SIZE=mfn-1;  /* tentativa de otimizar espaco, etc */ /* nao funciona para lilsptv */
                break;
            }
/*
            if (readwritebinv) {
                if (fldupdat(irectv,"Gsplit=4=wtrig")) fatal("wtrig2/loadvectors/Gsplit");
            }
*/
            for (dirp=MFRdir, xdir=0, loop=MFRnvf; loop--; dirp++, xdir++) {
                if (dirp->tag == parmcollmfntag) {
                    for (collhitmfn=0, p=FIELDP(xdir), left=dirp->len; left; ) {
                        if (!isdigit(*p)) break;
                        collhitmfn=collhitmfn*10+((int)(*p)-(int)'0');
                        p++; left--;
                    }
                    if (collhitmfn < 1) fatal("wtrig2/loadvectors/mfn invalid/1");
                    //if (collhitmfn > COLLECTION_SIZE) fatal("wtrig2/loadvectors/mfn invalid/2"); /* AOT, 11/05/2004 */
                }
                if (dirp->tag != TAG4) continue;
                left=dirp->len;
                fldp=FIELDP(xdir);
                if (readwritebinv) {
                     float b2aweight;
                     unsigned short int usi_w;
                     int b2a;
                     while (left) {
                        if (cnt >= readmaxtv) break;
                        b2a=sizeof(usi_w);
                        if (left < b2a+1) fatal("wtrig2/loadvectors/b2a/left/bin");
                        memmove((void *)&usi_w,fldp,b2a);
                        fldp+=b2a; left-=b2a;
                        b2aweight=(float)usi_w/(float)USHRT_MAX;
                        p=fldp; keylen=strlen(p); b2a=0;
                        while (left) {
                          q = fldp; fldp++; left--;
                          if (*q) b2a++; else break;
                        }
                        v->weight=b2aweight;
                        v->hidx=bsrchindex(table,tabentries,readwidth,p,keylen,&found);
                        if (!found) fatal("wtrig2/loadvectors/TAG4/not found/bin");
                        v++; cnt++;
                    }
                    break;
                }
                if (cnt >= readmaxtv) break;
                for (keylen=0, p=fldp; left; ) {
                    if (*p == '^') break;
                    keylen++; p++; left--;
                }
                if (left > 2) if (*p++ == '^') if (*p++ == 'w') {  /* ^w */
                    v->weight=atof(p);
                }
                v->hidx=bsrchindex(table,tabentries,readwidth,fldp,keylen,&found);
                if (!found) fatal("wtrig2/loadvectors/TAG4/not found");
                v++; cnt++;
            }
            *vcntp=cnt; *voffp=off; off+=cnt;
            vcntp++; voffp++;
            if (parmcollmfntag) { *vcmfp=collhitmfn; vcmfp++; }
            count++;
            //if (parmtell) if (count%parmtell == 0) fprintf(stderr,"++ %"_LD_"\n",count);
if (parmtell2) if (count%parmtell2 == 0) fprintf(stderr,"++ %"_LD_"/5 \n",(count-1)/parmtell2+1);

        }
        if (parmtell) /*if (count%parmtell == 0)*/ fprintf(stderr,"++ %"_LD_" term vectors in collection \n",count);
    }

    /* init output
    */
    if (*outsimilardb) {
        recisis0(outsimilardb);
        record(outcrec,outsimilardb,0L);
    }


    /* read all language bit strigs
    */
#if PARMLANG
    if (parmlanguage) {
        if (readnlangs >= MAXPARMLANG) fatal("wtrig2/terminverted/lang/MAXPARMLANG");
        for (langidx=0; langidx < readnlangs; langidx++) {
            int xdir;
            /* read */
            RECORD(irec,collectionterminverteddb,readnterms+langidx+1);
            if (RECrc != RCNORMAL) fatal("wtrig2/terminverted/lang/RCNORMAL");
            /* allocate */
            langshits[nlangs]=(char  *)loadfile(NULL,'@',"",NULL,hitbytes,'\0');
            /* store bit string */
            xdir=fieldx(irec,TAG35,1); if (xdir < 0) fatal("wtrig2/terminverted/lang/TAG35");
            memmove(langshits[nlangs],FIELDP(xdir),hitbytes);
            /* store name */
            xdir=fieldx(irec,TAG34,1); if (xdir < 0) fatal("wtrig2/terminverted/lang/TAG34");
            for (p=FIELDP(xdir), left=DIRlen(xdir); left--; ) {
                if (*p == '^') break;
                *p=isisuctab[*p]; p++;
            }
            *p='\0'; /* ok */
            langsname[nlangs]=strdup(FIELDP(xdir));
            langscount[nlangs++]=0;
        }
        if (nlangs != readnlangs) fatal("wtrig2/terminverted/lang/nlangs");
    }
#endif /* PARMLANG */


    /* main loop: process each document against all docs in collection
    */
    count=0;
    for (docmfn=1; ; docmfn++) {

        /* init similarity computation for document only components */
        float Wsomaq=0; /* init sum of squares for document terms */
	    float somaprod,somaq,simil; /* computation variables */

        /* init document most similar hit */
        float collapsesim1=0.0;
        LONGX collapsemfn1=0;
        LONGX collapsehit1=0;

        LONGX cnt=0;

        /* read term vector */
        RECORD(irec,documenttermvectorsdb,docmfn);
        if (RECrc == RCEOF) break;
        if (RECrc != RCNORMAL) continue;

        if (readwritebinv) {
            if (fldupdat(irec,"Gsplit=4=wtrig")) fatal("wtrig2/documenttermvectorsdb/Gsplit");
        }

        /* init list of relevant terms */
        if (!parmcollapse || !currentmfncollapsed) {
            LISTA *l;
            memset(lista,0x00,parmmaxrelat*sizeof(LISTA));
            for (l=lista, listavail=lista+1, loop=0; (LONGX)loop<(parmmaxrelat-1); loop++, l++, listavail++)
                l->avail=listavail;
            listhdr=listtail=listavail=lista;
        }
        /* init lastmfncollapse */
        if (parmcollapse) if (!currentmfncollapsed) {
            RECSTRU *recp;
            /* get output mfn, from tag parmcollapse of <mf1>.v */
            currentmfncollapsed=atol(recfield(buff,irec,parmcollapse,1,""));
            if (currentmfncollapsed < 1) fatal("wtrig2/documenttermvectorsdb/mfn");
            /* init output collapsed results record */
            RECORD(outirec,outsimilardb,currentmfncollapsed); MFRstatus=ACTIVE;
        }

        /* init term WEIGHTs for the document */
        memset(WEIGHT,0x00,readnterms*sizeof(float));

        /* init HIT list = docs in collection having 1+ of document terms */
        memset(HITS,0x00,hitbytes);

#if PARMLANG
        if (parmlanguage) {
            int xlang;
            langidx=(-1);
            recfield(buff,irec,parmlanguage,1,"");
            if (parmchklang) if (!*buff) {
                printf("*** docmfn=%"_LD_" tag=%d\n",docmfn,parmlanguage);
                fatal("wtrig2/lang/missing");
            }
            for (p=buff; *p; ) { *p=isisuctab[*p]; p++; }
            for (xlang=0; xlang < nlangs; xlang++) {
                if (strcmp(buff,langsname[xlang]) == 0) { langidx=xlang; break; }
            }
            if (parmchklang) if (langidx < 0) if (*buff) {
                printf("*** docmfn=%"_LD_" tag=%d data=%s\n",docmfn,parmlanguage,buff);
                for (xlang=0; xlang < nlangs; xlang++) printf("*** idx=%d/%d name=%s\n",xlang,nlangs,langsname[xlang]);
                fatal("wtrig2/lang/invalid");
            }
        }
#endif /* PARMLANG */

        /* loop term vector fields
        */
        for (docdirp=MFRdir, docxdir=0, docloop=MFRnvf; docloop--; docdirp++, docxdir++) {
            RECSTRU *keeprecp=recp;
            float weight=0;
            DIRSTRU *dirp2;
            int xdir2;
            FFI keylen2;
            char *fldp2;
            char fldarea[BUFSIZ];

            if (docdirp->tag != TAG4) continue;

            if (cnt++ >= readmaxtv) break; /* AOT, 09/05/2004 */

            for (keylen=0, p=fldp=FIELDP(docxdir), left=docdirp->len; left; ) {
                if (*p == '^') break;
                keylen++; p++; left--;
            }
            if (left > 2) if (*p++ == '^') if (*p++ == 'w') {  /* ^w */
                *(p+7)='\0';       /* AOT, July 4th, 2004 */  /* ^w0.254383E75^w0.222385 */
                weight=atof(p);
            }

            /* convert term key to index: dochidx */
            dochidx=bsrchindex(table,tabentries,readwidth,fldp,keylen,&found);

            /* keep fldp */
            memmove(fldarea,fldp,keylen);

            if (found) {
                /* similarity computation */
                Wsomaq += weight * weight;  /* sum of squares for document terms */
            }
            else { /* if (lognotfound) ... */
                if (parmwsoma) {
                    /* similarity computation */
                    Wsomaq += weight * weight;  /* sum of squares for document terms */
                }
                continue;
            }

            /* store weight */
            WEIGHT[dochidx]=weight;

            /* read term's postings from invertedmf and OR them (mfns) into HIT
            */
            if (*collectionterminverteddb) {
                LONGX loop;
                LONGX irecif;
                int found=0;
                if (parmloadinverted) {
                    for (loop=0; loop < usedifrecs; loop++) {
                        recp=vrecp[vifrec[loop]];
                        if (MFRmfn == dochidx+1) { /*irecif=vifrec[loop];*/ found=1; break; }
                    }
                    if (!found) {
                        if (leftifrecs) {
                            /* load into next */
                            LONGX ridx;
                            //for (ridx=maxnrec; ridx--; ) if (!vrecp[ridx]) break;
                            //if (ridx < 0) fatal("wtrig2/collectionterminverteddb/load/maxnrec");
                            for (ridx=maxnrec; ridx--; ) if (!vrecp[ridx]) break;
                            if (ridx < 0) fatal("wtrig2/collectionterminverteddb/load/maxnrec");
                            irecif=vifrec[1+usedifrecs]=ridx; usedifrecs++; leftifrecs--;
                        }
                        else {
                            /* page out the entry with min mfrl */
                            LONGX minmfrl=MFRL_MAX;
                            LONGX x=1;
                            for (loop=1; loop < usedifrecs; loop++) {
                                recp=vrecp[vifrec[loop]];
                                if ((LONGX)MFRmfrl < minmfrl) x=loop;
                            }
                            irecif=vifrec[x];
                        }
                        RECORD(vifrec[0],collectionterminverteddb,dochidx+1);
                        if (vrecp[irecif]) if (VMFRmfrl(irecif) < MFRmfrl) {
                            FREE(vrecp[irecif]); vrecp[irecif]=NULL; nrecs--;
                        }
                        if (!vrecp[irecif]) {
                            recallok(irecif,MFRmfrl); /* RECHSIZE+nbytes is allocated */
                        }
                        memmove(vrecp[irecif],vrecp[vifrec[0]],RECHSIZE+MFRmfrl);
                        recp=vrecp[irecif]; /* current */
                    } /* end if !found */
                } /* end if parmloadinverted */
                else { /* load into 0 */
                    irecif=vifrec[0];
                    RECORD(irecif,collectionterminverteddb,dochidx+1);
                }
            } /* end if collectionterminverteddb */
            //if (RECrc != RCNORMAL) fatal("wtrig2/collectionterminverteddb/deleted");
            if (*collectionterminverteddb) if (RECrc == RCNORMAL) {
                LONGX nmfns=0;
                for (dirp2=MFRdir, xdir2=0, loop=MFRnvf; loop--; dirp2++, xdir2++) {
                    if (dirp2->tag != TAG33) continue;
                    for (keylen2=0, p=fldp2=FIELDP(xdir2), left=dirp2->len; left; ) {
                        if (*p == '^') {
                            if (left < 2 ) fatal("wtrig2/collectionterminverteddb/nhits/left");
                            if (*++p != 'n') fatal("wtrig2/collectionterminverteddb/nhits/left");
                            left-=2;
                            while (left) {
                                if (!isdigit(*++p)) break;
                                nmfns=nmfns*10+((int)(*p)-(int)'0');
                                left--;
                            }
                            break;
                        }
                        keylen2++; p++; left--;
                    }
                }
                if (keylen2 != keylen) fatal("wtrig2/collectionterminverteddb/keylen");
                if (memcmp(fldarea,fldp2,keylen)) fatal("wtrig2/collectionterminverteddb/key");

                if (parmtrace & 0x04) printf(" 4|%"_LD_"|%s|%"_LD_"|%f\n",docmfn,table+dochidx*hwidth,MFRmfn,weight);

                /* loop through term's postings and OR the mfn's bit */
                if (readwritebin) {
                    int n=sizeof(hitmfn);
                    for (dirp2=MFRdir, xdir2=0, loop=MFRnvf; loop--; dirp2++, xdir2++) {
                        if (dirp2->tag != TAG5) continue;
                        left=dirp2->len;
                        if (left < n*nmfns) fatal("wtrig2/collectionterminverteddb/bin/left/1");
                        for (p=FIELDP(xdir2); left; ) {
                            memmove((void *)&hitmfn,p,n);
                            p+=n; left-=n;
                            if (left < 0) fatal("wtrig2/collectionterminverteddb/bin/left/2");
                            if (hitmfn < 1) fatal("wtrig2/collectionterminverteddb/bin/mfn invalid/1");
                            if (hitmfn > COLLECTION_SIZE) {
                                fatal("wtrig2/collectionterminverteddb/bin/mfn invalid/2");
                            }
                            hitmask =hitbitmask[hitmfn&BY8RMASK];
                            hitoffset = hitmfn/8;
#if PARMLANG
                            /* filter language hits */
                            if (langidx >= 0) {
                                unsigned char c = *(langshits[langidx]+hitoffset);
                                if (c & hitmask)
                                    HITS[hitoffset] |= hitmask;
                            }
                            else
#endif /* PARMLANG */
                            HITS[hitoffset] |= hitmask;
                            if (parmtrace & 0x08) printf(" 8|%"_LD_"|%s|%"_LD_"|%"_LD_"|%d/%d/%d|%d/%"_LD_"\n",docmfn,table+dochidx*hwidth,hitmfn,MFRmfn,loop,xdir,left,n,nmfns);
                        }
                        break;
                    } /* end term postings */
                }
                else {
                    for (dirp2=MFRdir, xdir2=0, loop=MFRnvf; loop--; dirp2++, xdir2++) {
                        if (dirp2->tag != TAG5) continue;
                        for (hitmfn=0, p=FIELDP(xdir2), left=dirp2->len; left; ) {
                            if (!isdigit(*p)) break;
                            hitmfn=hitmfn*10+((int)(*p)-(int)'0');
                            p++; left--;
                        }
                        if (hitmfn < 1) fatal("wtrig2/collectionterminverteddb/mfn invalid/1");
                        if (hitmfn > COLLECTION_SIZE) fatal("wtrig2/collectionterminverteddb/mfn invalid/2");
                        hitmask =hitbitmask[hitmfn&BY8RMASK];
                        hitoffset = hitmfn/8;
#if PARMLANG
                            /* filter language hits */
                            if (langidx >= 0) {
                                unsigned char c = *(langshits[langidx]+hitoffset);
                                if (c & hitmask)
                                    HITS[hitoffset] |= hitmask;
                            }
                            else
#endif /* PARMLANG */
                        HITS[hitoffset] |= hitmask;
                        if (parmtrace & 0x08) printf(" 8|%"_LD_"|%s|%"_LD_"\n",docmfn,table+dochidx*hwidth,hitmfn);
                    } /* end term postings */
                } /* end if parmwritebin */
            } /* end if RECrc == RCNORMAL */

            recp=keeprecp; /* restore current recp */

        } /* end loop term vector elements for HIT construction */


        /* loop throught hit records and process against the corresponding vector WEIGHT[0..nterms-1]
        */
        for (hitmfn=1; hitmfn <= COLLECTION_SIZE; hitmfn++) {

            VECTEL *v;
            LONGX cnt=0;

            if (*collectionterminverteddb) {
                hitmask = hitbitmask[hitmfn&BY8RMASK];
                hitoffset = hitmfn/8;
                if (hitoffset >= hitbytes) break;
                if (!(HITS[hitoffset] & hitmask)) continue;
            }

            if (parmtrace & 0x10) printf("16|%"_LD_"|%"_LD_"\n",docmfn,hitmfn);

            /* convert hitmfn to its original mfn */
            collhitmfn=hitmfn;

            /* get correspoding vector */
            if (parmloadvectors) {
                v=VECTORS+VECTOFF[hitmfn-1]; cnt=VECTCNT[hitmfn-1];
                if (parmcollmfntag) collhitmfn=VECTCMF[hitmfn-1];
            }
            else {
                 /* RECORD  */
                RECSTRU *recp;
                DIRSTRU *dirp;
                int xdir;
                FFI loop,left;
                v=vector;
                RECORD(irectv,collectiontermvectorsdb,hitmfn);

                if (readwritebinv) {
                    if (fldupdat(irectv,"Gsplit=4=wtrig")) fatal("wtrig2/readvectors/Gsplit");
                }

                for (dirp=MFRdir, xdir=0, loop=MFRnvf; loop--; dirp++, xdir++) {
                    if (dirp->tag == parmcollmfntag) {
                        for (collhitmfn=0, p=FIELDP(xdir), left=dirp->len; left; ) {
                            if (!isdigit(*p)) break;
                            collhitmfn=collhitmfn*10+((int)(*p)-(int)'0');
                            p++; left--;
                        }
                        if (collhitmfn < 1) fatal("wtrig2/readvectors/mfn invalid/1");
                        //if (collhitmfn > COLLECTION_SIZE) fatal("wtrig2/readvectors/mfn invalid/2"); /* AOT, 11/05/2004 */
                    }
                    if (dirp->tag != TAG4) continue;
                    if (cnt >= readmaxtv) break;
                    for (keylen=0, p=fldp=FIELDP(xdir), left=dirp->len; left; ) {
                        if (*p == '^') break;
                        keylen++; p++; left--;
                    }
                    if (left > 2) if (*p++ == '^') if (*p++ == 'w') {  /* ^w */
                        v->weight=atof(p);
                    }
                    v->hidx=bsrchindex(table,tabentries,readwidth,fldp,keylen,&found);
                    if (!found) fatal("wtrig2/getvector/TAG4/not found");
                    if (parmtrace & 0x20) printf("32|%"_LD_"|%"_LD_"|%s|%f\n",docmfn,hitmfn,table+v->hidx*hwidth,v->weight);
                    v++; cnt++;
                }
                v=vector; /* init VW while */
            } /* end parmloadvectors */


            /* compute similarity between input document and collection doc
            */
   			somaprod=0; /* init sum of products of corresponding terms */
            somaq=0;    /* init sum of squares for collection doc terms */
            for (; cnt--; v++) {
                float W;
                if (v->hidx < 0 || v-> hidx >= readnterms) fatal("wtrig2/VW/hidx");

                somaq += v->weight * v->weight; /* sum of squares for collection doc terms */

                W=WEIGHT[v->hidx];
                if (!W) continue;

                somaprod += W * v->weight; /* sum of products of corresponding terms */
            }

            simil=0;
            if (somaprod) {

                float denom = sqrt( Wsomaq * somaq);  /* compute */

			    if (denom > 0) simil=somaprod/denom;    /* compute */

                if (simil > 1.0) simil=1.0;   /* esteban, 09/12/2003 */
                
            } /* end if somaprod */

            /* output results
            */
            if (simil >= parmminsim) if (simil <= parmmaxsim) {

              if (parmcollapse) { /* collapse */
                if (simil > collapsesim1) {
                    collapsesim1=simil;
                    collapsemfn1=collhitmfn;
                    collapsehit1=hitmfn;
                    if (parmtrace & 0x40) printf("64|%"_LD_"|%"_LD_"|%"_LD_"|%f\n",docmfn,collhitmfn,hitmfn,simil);
                }
              }
              else { /* not collapse */
                /* mantain list of related */
                LISTA *l=NULL;
                LISTA *prev=NULL;
                LONGX nextm=0; /* flag */

                if (listhdr->mfn) l=listhdr;
                while (l) {
                    if (simil < l->sim) { prev=l; l=l->next; }
                    else break;
                }
                nextm=((l)?l->mfn:0);
                if (listavail) {
                        LISTA *newl=listavail;
                        listavail=listavail->avail;
                        newl->sim=simil; newl->mfn=collhitmfn; /**/newl->xxx=docmfn;/**/ newl->hit=hitmfn;
                        newl->prev=prev;
                        newl->next=l;
                        if (prev) prev->next=newl;
                        if (nextm) l->prev=newl;
                        if (!newl->prev) listhdr=newl;
                        if (!newl->next) listtail=newl;
                        if (parmtrace2) printf("64a|");
                }
                else {
                    if (simil > listtail->sim) {
                        LISTA *last=listtail;
                        LISTA *repl=last;
                        LISTA *lnext=NULL;
                        if (l) lnext=l->next;
                        if (last->prev) {
                            listtail=last->prev;
                            listtail->next=NULL;
                        }
                        repl->sim=simil; repl->mfn=collhitmfn; /**/repl->xxx=docmfn;/**/ repl->hit=hitmfn;
                        repl->prev=prev;
                        if (prev) prev->next=repl;
                        if (l) if (lnext) {
                            repl->next=l;
                            l->prev=repl;
                        }
                        if (!repl->prev) listhdr=repl;
                        if (!repl->next) listtail=repl;
                        if (parmtrace2) printf("64b|");
                    }
                }
                if (parmtrace2) {
                  for (l=listhdr; l; l=l->next) {
                    if (!l) break;
                    //printf("%"_LD_"|%"_LD_"|%f  p=%"_LD_"|n=%"_LD_"|a=%"_LD_"  hdr=%"_LD_" lst=%"_LD_" prev=%"_LD_"\n",docmfn,l->mfn,l->sim,
                    printf("%"_LD_"|%"_LD_"|%"_LD_"|%f  p=%"_LD_"|n=%"_LD_"|a=%"_LD_"  hdr=%"_LD_" lst=%"_LD_"\n",docmfn,l->mfn,l->hit,l->sim,
                      (l->prev)?((LISTA *)(l->prev))->mfn:-1,
                      (l->next)?((LISTA *)(l->next))->mfn:-1,
                      (l->avail)?((LISTA *)(l->avail))->mfn:-1,
                      (listhdr->mfn)?listhdr->mfn:-1,
                      (listtail->mfn)?listtail->mfn:-1);
                      //,(prev)?prev->mfn:-1);
                  }
                }
                if (parmtrace & 0x40) printf("64|%"_LD_"|%"_LD_"|%"_LD_"|%f\n",docmfn,collhitmfn,hitmfn,simil);
              } /* end collapse */

            } /* end if sim>=<= */

        } /* end loop throught hit records and process the corresponding vector */

        /* output related collection docs for the document
        */
        if (*outsimilardb) {
            RECSTRU *recp;
            LISTA *l;
            char *batchp=buff;
            buff[0]='\0';
            if (parmcollapse) { /* collapse */
                LONGX mfncollapse;
                /* get output mfn, from tag parmcollapse of <mf1>.v */
                mfncollapse=atol(recfield(buff,irec,parmcollapse,1,""));
                if (mfncollapse < currentmfncollapsed) fatal("wtrig2/outsimilardb/documenttermvectorsdb/mfn");
                /* collapse results */
                if (mfncollapse == currentmfncollapsed) if (collapsemfn1) {
                    /* mantain list of related */
                    LISTA *l=listhdr;
                    LISTA *prev=NULL;
                    while (l) {
                        if (collapsemfn1 == l->mfn) {
                            if (collapsesim1 > l->sim) {
                                /* remove from list */
                                LISTA *next=l->next;
                                LISTA *prev=l->prev;
                                if (prev) prev->next=l->next;
                                if (next) next->prev=prev;
                                l->sim=0.0; l->mfn=0; l->xxx=0; l->hit=0;
                                if (!prev) /* deletando cabeca de lista */
                                    if (next) listhdr=next; else listhdr=lista;
                                if (!next) /* deletando fim de lista */
                                    if (prev) listtail=prev; else listtail=lista;
                                l->avail=listavail; listavail=l; /* reuse it */
                                l=NULL; /* force add */
                            }
                            if (parmtrace2) printf("64=|\n");
                            break; /* l-> will remain in list */
                        } else l=l->next;
                    }
                    if (!l) {
                      /* add to list */
                      if (listhdr->mfn) l=listhdr;
                      while (l) {
                           if (collapsesim1 < l->sim) { prev=l; l=l->next; }
                           else break;
                      }
                      if (listavail) {
                            LISTA *newl=listavail;
                            LONGX nextm; /* flag */
                            nextm=((l)?l->mfn:0);
                            listavail=listavail->avail;
                            newl->sim=collapsesim1; newl->mfn=collapsemfn1; newl->xxx=docmfn; newl->hit=collapsehit1;
                            newl->prev=prev;
                            newl->next=l;
                            if (prev) prev->next=newl;
                            if (nextm) l->prev=newl;
                            if (!newl->prev) listhdr=newl;
                            if (!newl->next) listtail=newl;
                            if (parmtrace2) printf("64A|\n");
                      }
                      else {
                        if (collapsesim1 > listtail->sim) {
                            LISTA *last=listtail;
                            LISTA *repl=last;
                            LISTA *lnext=NULL;
                            if (l) lnext=l->next;
                            if (last->prev) {
                                listtail=last->prev;
                                listtail->next=NULL;
                            }
                            repl->sim=collapsesim1; repl->mfn=collapsemfn1; repl->xxx=docmfn; repl->hit=collapsehit1;
                            repl->prev=prev;
                            if (prev) prev->next=repl;
                            if (l) if (lnext) {
                                repl->next=l;
                                l->prev=repl;
                            }
                            if (!repl->prev) listhdr=repl;
                            if (!repl->next) listtail=repl;
                            if (parmtrace2) printf("64B|\n");
                        }
                      }
                    } /* end if l */
                    if (parmtrace2) {
                      for (l=listhdr; l; l=l->next) {
                        if (!l) break;
                        //printf("  |%"_LD_"|%"_LD_"|%f|%"_LD_"  p=%"_LD_"|n=%"_LD_"|a=%"_LD_"  hdr=%"_LD_" lst=%"_LD_" prev=%"_LD_" \n",docmfn,l->mfn,l->sim,l->xxx,
                        printf("  |%"_LD_"|%"_LD_"|%f|%"_LD_"|%"_LD_"  p=%"_LD_"|n=%"_LD_"|a=%"_LD_"  hdr=%"_LD_" lst=%"_LD_"\n",docmfn,l->mfn,l->sim,l->xxx,l->hit,
                          (l->prev)?((LISTA *)(l->prev))->mfn:-1,
                          (l->next)?((LISTA *)(l->next))->mfn:-1,
                          (l->avail)?((LISTA *)(l->avail))->mfn:-1,
                          (listhdr->mfn)?listhdr->mfn:-1,
                          (listtail->mfn)?listtail->mfn:-1);
                          //,(prev)?prev->mfn:-1);
                      }
                    }
                    if (parmtrace & 0x40) printf("64:|%"_LD_"|%"_LD_"|%"_LD_"|%f\n",docmfn,collapsehit1,collapsemfn1,collapsesim1);
                }
                /* output collapsed results */
                if (mfncollapse != currentmfncollapsed) {
                    buff[0]='\0';
                    for (l=listhdr; l; l=l->next) {
                        if (!l->mfn) break;
                        sprintf(batchp,"<6 0>%"_LD_"^s%f^m%"_LD_"^h%"_LD_"</6>",l->mfn,l->sim,l->xxx,l->hit);
                        batchp+=strlen(batchp);
                    }
                    if (buff[0]) {
                        if (fldupdat(outirec,buff)) fatal("wtrig2/outsimilardb/collapse/fldupdat");
                        recupdat(outcrec,outirec);
                    }
                    /* re-init list of relevant terms */
                    memset(lista,0x00,parmmaxrelat*sizeof(LISTA));
                    for (l=lista, listavail=lista+1, loop=0; (LONGX)loop<(parmmaxrelat-1); loop++, l++, listavail++)
                        l->avail=listavail;
                    listhdr=listtail=listavail=lista;
                    /* set current collapsed output mfn */
                    currentmfncollapsed=mfncollapse;
                    /* re-init output collapsed results record */
                    RECORD(outirec,outsimilardb,mfncollapse); MFRstatus=ACTIVE;
                }
            }
            else { /* not collapse */
                RECORD(outirec,outsimilardb,docmfn); MFRstatus=ACTIVE;
                for (l=listhdr; l; l=l->next) {
                    if (!l->mfn) break;
                    //sprintf(batchp,"<6 0>%"_LD_"^s%f</6>",l->mfn,l->sim);
                    sprintf(batchp,"<6 0>%"_LD_"^s%f^m%"_LD_"^h%"_LD_"</6>",l->mfn,l->sim,l->xxx,l->hit);
                    batchp+=strlen(batchp);
                }
                if (buff[0]) {
                    if (fldupdat(outirec,buff)) fatal("wtrig2/outsimilardb/fldupdat");
                    recupdat(outcrec,outirec);
                }
            }
        }

        count++;
        if (parmtell) if (count%parmtell == 0) fprintf(stderr,"+++ %"_LD_"\n",count);

    } /* end read term vector */


    /* output collapsed results last record */
    if (*outsimilardb) if (parmcollapse) {
        LISTA *l;
        char *batchp=buff;
        buff[0]='\0';
        for (l=listhdr; l; l=l->next) {
            if (!l->mfn) break;
            sprintf(batchp,"<6 0>%"_LD_"^s%f^m%"_LD_"^h%"_LD_"</6>",l->mfn,l->sim,l->xxx,l->hit);
            batchp+=strlen(batchp);
        }
        if (buff[0]) {
            if (fldupdat(outirec,buff)) fatal("wtrig2/outsimilardb/collapse/fldupdat");
            recupdat(outcrec,outirec);
        }
    }

    /* log execution
    */
    /* ... */

    exit(0);  /* AOT, 30/07/2004 - thanks MB */

} /* end of main */


