
/* ==----------------------------- wtfun.c ------------------------------== */

/* text */

    parmtext=NULL;  // text=
    
    trigbuff=NULL;
  
    //COLLECTION=NULL;     /* n(t) - #docs trigram occurs */
    DOCUMENT=NULL;         /* f(t) - #occs of trigram in document [also flag 1st occ in doc] */
    GLOBALW=NULL;          /* g(t) - trigram global weight */
    PRODW=NULL;            /* lg(t) - trigram local*global weight */

    list1a=NULL;           /* results */

    
/* document */

    parmdocumentsdb=NULL;
    
    docvector=NULL;
    lista=NULL;              /* list of related */

    
/* collection */

    collectiondb=NULL;

    VECTORS=NULL;            /* VECTORS[] - readmaxtv */
    WEIGHT=NULL;             /* WEIGHT[] - readnterms */
    HITS=NULL;               /* HITS[] */


    readwidth= -1;
    readcategories= -1;
    readmaxprim= -1;
    readcollection_size= -1;
    readpostingson_size= -1;
    readmaxtv= -1;
    readnterms= -1;
    readwritebin= -1;
    readnlangs=0;

    readmaxrf= -1;
        
    readcollmfntag=0; // -1;   /* optional ^ndmfn^v1 */

    readcolljd=0; // /* optional TAG440 */

    
#if PARMLANG
    nlangs=0;
    langidx= -1;
    parmchklang=0;
#endif /* PARMLANG */

/* ==----------------------------- wtfun.c ------------------------------== */
