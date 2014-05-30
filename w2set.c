
    /* setup filesnames
    */

    if (!*colcdb) sprintf(colcdb,"%s.c",collectiondb);
    if (!*colndb) sprintf(colndb,"%s.n",collectiondb);
    if (!*colxdb) sprintf(colxdb,"%s.x",collectiondb);
    if (!*colvdb) sprintf(colvdb,"%s.v",collectiondb);
    

    /* collection processing 
    */
    /* alloc///colcdb/colndb/coljdb/docvdb_V3 */
    for (irec=maxnrec; irec--; ) { if (!vrecp[irec]) /* ja' decrementado */ break; }           
    if (irec<0L) fatal("wtrig2/irec");
    recallok(irec,awtmaxmfrl); 

    /* alloc///colvdb - w2rcol.c */
    for (irectv=maxnrec; irectv--; ) { if (!vrecp[irectv]) /* ja' decrementado */ break; }
    if (irectv<0L) fatal("wtrig2/irectv");
    recallok(irectv,awtmaxmfrl); 


#if JDIN440
    /* assumes jd comes in TAG440
    */
    RECORD(irec,collectiondb,1);
    if (RECrc != RCNORMAL) fatal("wtrig2/COLLECTION/RCNORMAL");
    if (fieldx(irec,TAG440,1) >= 0) readcolljd=1;
#else
#endif
    
    /* get collection size, classes, width, etc from file
    */
    RECORD(irec,colcdb,1);
    if (RECrc != RCNORMAL) fatal("wtrig2/COLLECTION_SIZE/RCNORMAL");
    
    /* get collection size: N */
    readcollection_size=atol(recfield(awtdatabuff,irec,TAG2,1,""));
    /* get max postings in inverteddb: HN */
    readpostingson_size=atol(recfield(awtdatabuff,irec,TAG3,1,""));
    
    /* get other parms */
    for (iocc=1; *recfield(awtdatabuff,irec,TAG1,iocc,""); iocc++) {
        char *q;
        q="^nK^v";
        p=strstr(awtdatabuff,q); if (p) readcategories=atol(p+strlen(q));
        q="^nwidth^v";
        p=strstr(awtdatabuff,q); if (p) readwidth=atoi(p+strlen(q));
        q="^nmaxprim^v";
        p=strstr(awtdatabuff,q); if (p) readmaxprim=atol(p+strlen(q));
        q="^nmaxtv^v";
        p=strstr(awtdatabuff,q); if (p) readmaxtv=atoi(p+strlen(q));
        q="^nnterms^v";
        p=strstr(awtdatabuff,q); if (p) readnterms=atoi(p+strlen(q));
        q="^nlanguages^v";
        p=strstr(awtdatabuff,q); if (p) readnlangs=atoi(p+strlen(q));
        q="^nbin^v";
        p=strstr(awtdatabuff,q); if (p) readwritebin=atoi(p+strlen(q));
        
        q="^nmaxrf^v";
        p=strstr(awtdatabuff,q); if (p) readmaxrf=atof(p+strlen(q));

        q="^ndmfn^v"; // when collection was wtrig1'ed
        p=strstr(awtdatabuff,q); if (p) readcollmfntag=atoi(p+strlen(q));

        if (readpostingson_size  < 1) readpostingson_size=readcollection_size*readmaxrf;

#if ALLOWUPCASE
        q="^ncase^v";
        p=strstr(awtdatabuff,q); if (p) { 
            char *x=strchr(q=p+strlen(q),'^');
            if (x) {
                char c= *x;
                *x='\0';                
                memset(xisiswctab,0x00,sizeof(xisisuctab)); 
                xisiswctot=0;
                if (strcmp(q,"ascii") == 0) {
                    memmove(xisisuctab,isisuctab,sizeof(xisisuctab));
                    for (; isisactab[xisiswctot]; xisiswctot=xisiswctot+1) xisiswctab[isisactab[xisiswctot]]=1;
                }
                if (strcmp(q,"ansi") == 0) {
                    memmove(xisisuctab,ansiuctab,sizeof(xisisuctab));
                    for (; ansiactab[xisiswctot]; xisiswctot=xisiswctot+1) xisiswctab[ansiactab[xisiswctot]]=1;
                } 
                else {
                    unsigned char acbuff[256];
                    if (!loaductb(NULL,isisuctab,q)) fatal(q);
                    if (!loadactb(NULL,acbuff,q))    fatal(q);
                    for (; acbuff[xisiswctot]; xisiswctot=xisiswctot+1) xisiswctab[acbuff[xisiswctot]]=1;
                }
                *x=c;
            }
        }        
#endif
    }
//    dbxflush(colcdb); /* close/flush */   // should work..


    if (readcollection_size  < 1) fatal("wtrig2/invalid readcollection_size");
    if (readpostingson_size  < 1) fatal("wtrig2/invalid readpostingson_size");
    if (readcategories       < 1) fatal("wtrig2/invalid readcategories");
    if (readwidth            < 1) fatal("wtrig2/invalid readwidth");
    if (readmaxprim          < 1) fatal("wtrig2/invalid readmaxprim");
    if (readmaxtv            < 1) fatal("wtrig2/invalid readmaxtv");
    if (readnterms           < 1) fatal("wtrig2/invalid readnterms");
    if (readnlangs           < 0) fatal("wtrig2/invalid readnlangs");
    if (readnterms           < 1) fatal("wtrig2/invalid readnterms");
    if (readwritebin         < 1) fatal("wtrig2/invalid readwritebin");
    if (readmaxrf            < 0) fatal("wtrig2/invalid readmaxrf");
    if (readcollmfntag       < 0) fatal("wtrig2/invalid readcollmfntag");
    if (readwidth > MAXPARMWIDTH) fatal("wtrig2/invalid readwidth");

    /* set */
    parmcollmfntag=readcollmfntag;
    parmmaxrf=readmaxrf;
    parmmaxtv=readmaxtv;
    parmmaxrel=PARMMAXREL;
    parmminsim=PARMMINSIM;
    parmmaxsim=PARMMAXSIM;

    /* get */
#include "w2pcox.c"
///////////////////////////////////////////////////////////////////////////////////////////////////    

    /* chk */
    if (parmcollmfntag > readcollmfntag)   parmcollmfntag=readcollmfntag;       
    if (parmmaxrf > readmaxrf)   parmmaxrf=readmaxrf;   
    if (parmmaxtv > readmaxtv)   parmmaxtv=readmaxtv;   
    if (parmmaxrel > readcollection_size) parmmaxrel=readcollection_size;
    if (parmminsim > parmmaxsim) parmminsim=PARMMINSIM;
    if (parmminsim > parmmaxsim) parmmaxsim=PARMMAXSIM;
     
    /* keep parms in coll structure */
    collcollmfntag=parmcollmfntag;
    collmaxrf=parmmaxrf;
    collmaxtv=parmmaxtv;
    collmaxrel=parmmaxrel;
    collminsim=parmminsim;
    collmaxsim=parmmaxsim;
    
    collcolljd=readcolljd;
    
    
    /* check collection size x collection maxmfn */
    COLLECTION_SIZE=readcollection_size;
    if (parmtrace & 0x02) {
        printf(" 2|    |%"_LD_"\n",COLLECTION_SIZE);
    }
    
  
    /* allocate COLLECTION's term frequencies
    */
    COLLECTION=     (LONGX *) loadfile(NULL,'@',"",NULL, readnterms*sizeof(LONGX),   '\0');
    memset(COLLECTION,0x00,readnterms*sizeof(float));
    
    /* allocate COLLECTION's term global weights
    */
    GLOBALW=   (float *)loadfile(NULL,'@',"",NULL, readmaxprim*sizeof(float),  '\0');
    memset(COLLECTION,0x00,readnterms*sizeof(float));

    
    /* allocate bsrch table
    */
    table=bsrchalloc(readnterms,readwidth,&tabentries);

    if (readnterms > readcategories) fatal("wtrig2/invalid categories or nterms");
    if (readnterms > readmaxprim)    fatal("wtrig2/invalid maxprim or nterms");

    hwidth=(readwidth+1);
    
     /* and load term keys - distribution / COLLECTION / GLOBALW
     */
    if (parmtrace2) printf("+++ readnterms=%"_LD_"\n",readnterms);
    if (parmtrace2) printf("+++ readwidth=%"_LD_"\n",readwidth);
    /* load collection terms in the hashing table
    */
    count=0;
    for (mfn=1; ; mfn++) {
        RECORD(irec,colndb,mfn);
        if (RECrc == RCEOF) break;
        if (RECrc != RCNORMAL) continue;
        for (dirp=MFRdir, xdir=0, loop=MFRnvf; loop--; dirp++, xdir++) {
            int found;
            LONGX ndocs;
            LONGX hidx;

            if (dirp->tag != TAG3) continue;
            if (dirp->len == 0) continue; // wtrig1
            for (keylen=0, p=fldp=FIELDP(xdir), left=dirp->len; left; ) {
                    if (*p == '^') break;
                    keylen++; p++; left--;
            }
            ndocs=0;
            if (left > 2) if (*p++ == '^') if (*p++ == 'n') {  /* ^n */
                left=left-2;
                while (left) {
                        if (!isdigit(*p)) break;
                        ndocs=ndocs*10+((int)(*p)-(int)'0');
                        p++; left--;
                }
            }

            bsrchindex(table,tabentries,readwidth,fldp,keylen,&found);
            if (found) fatal("wtrig2/duplicated collection term");
            hidx=bsrchstore(table,readnterms,&tabentries,readwidth,fldp,keylen);
            if (hidx < 0 || ndocs>COLLECTION_SIZE) fatal("wtrig2/bsrchstore/bug");
            
            if (ndocs) {
                double lognum = (double)COLLECTION_SIZE;
                double logden = (double)ndocs;
                COLLECTION[hidx]=ndocs;        
                GLOBALW[hidx] = (float)sqrt(log(lognum/logden));
                if (parmtrace & 0x02) printf(" 2|%s|%"_LD_"|%f\n",table+hidx*hwidth,COLLECTION[hidx],GLOBALW[hidx]);
            }
        }
        count++;
        //if (parmtell) if (count%parmtell == 0) fprintf(stderr,"+ %"_LD_"\n",count);
    }    
//    dbxflush(colndb); /* close/flush */
    if (parmtrace2) printf("+++ tabentries=%"_LD_"\n",tabentries);
    if (parmtell) /*if (count%parmtell == 0)*/ fprintf(stderr,"+ %"_LD_" terms in collection \n",count);


    
     /* load index
     */
    /* alloc///colxdb */
    for (ifrec=maxnrec; ifrec--; ) { if (!vrecp[ifrec]) /* ja' decrementado */ break; }
    if (ifrec<0L) fatal("wtrig2/ifrec");
  //recallok(ifrec,awtmaxmfrl);
    recallok(ifrec,BUFSIZ+readpostingson_size*sizeof(LONGX));

    /* mstsetup col.x */                      /* AOT, 04/08/2005 */
    if (parmtell) fprintf(stderr,"+ loading %s term indexes..\n",colxdb);
    mstsetup(colxdb,LONGX_MAX,LONGX_MAX);
    RECORD(ifrec,colxdb,0L);
    if (parmtell) fprintf(stderr,"+ %"_LD_" term indexes loaded\n",MF0nxtmfn-1);


    
     /* document processing 
     */
     /* alloc///docvdb */
    for (inirec=maxnrec; inirec--; ) { if (!vrecp[inirec]) /* ja' decrementado */ break; }
    if (inirec<0L) fatal("wtrig2/inirec");
    recallok(inirec,awtmaxmfrl); 

    /* alloc///docydb ctl */
    for (outcrec=maxnrec; outcrec--; ) { if (!vrecp[outcrec]) /* ja' decrementado */ break; }
    if (outcrec<0L) fatal("wtrig2/outcrec");
    recallok(outcrec,sizeof(M0STRU)); 
    
    /* alloc///docydb */
    for (outirec=maxnrec; outirec--; ) { if (!vrecp[outirec]) /* ja' decrementado */ break; }
    if (outirec<0L) fatal("wtrig2/outirec");
    recallok(outirec,awtmaxmfrl);  // RECsetup in w2setrt.c / w2outx.c + w2out1.c
    
    /* alloc document' term vector
    */
    docvector= (DOCVEC *)loadfile(NULL,'@',"",NULL,readmaxtv*sizeof(DOCVEC),'\0');
    docvectcnt=0;

    /* allocate area for term index hits
    */
    hitbytes=COLLECTION_SIZE/8+1; /* bit string size */
    HITS=           (char  *)loadfile(NULL,'@',"",NULL,hitbytes,'\0');

    /* allocate DOCUMENT's term frequencies and weights
    */
    DOCUMENT=       (int *)  loadfile(NULL,'@',"",NULL, readnterms*sizeof(int),    '\0');
    PRODW=          (float *)loadfile(NULL,'@',"",NULL, readnterms*sizeof(float),  '\0');
    WEIGHT=         (float *)loadfile(NULL,'@',"",NULL, readnterms*sizeof(float),  '\0');

    /* allocate list of related
    */
    lista=(LISTA *)loadfile(NULL,'@',"",NULL,parmmaxrel*sizeof(LISTA),'\0');


    /* Program WTRIG1: trigrams 1A 
    */
    
    /* alloc/// */
    for (zirec=maxnrec; zirec--; ) { if (!vrecp[zirec]) /* ja' decrementado */ break; }
    if (zirec<0L) fatal("wtrig2/zirec");
    recallok(zirec,awtmaxmfrl);
        
    /* alloc working areas
    */
    trigbuff=loadfile(NULL,'@',"",NULL,awtmaxmfrl+awtmaxmfrl,'\0');

    /* allocate list of relevant terms
    */
    list1a=          (LIST1A *)loadfile(NULL,'@',"",NULL, readmaxtv*sizeof(LIST1A), '\0');

/*
*/

