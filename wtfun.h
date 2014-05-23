/* ==----------------------------- wtfun.h ------------------------------== */

typedef struct wtfun_xwt {



//typedef struct wtfvet_xwt {

    char   *xparmtext;  // text=
    
    LONGX    xzirec;
    char   *xtrigbuff;
    
    LONGX   *xCOLLECTION;        /* n(t) - #docs trigram occurs */
    int    *xDOCUMENT;          /* f(t) - #occs of trigram in document [also flag 1st occ in doc] */
    float  *xGLOBALW;      /* g(t) - trigram global weight */
    float  *xPRODW;        /* lg(t) - trigram local*global weight */

    LIST1A *xlist1a;           /* results */

//} WTFVET_XWT;


//typedef struct wtfdoc_xwt {

    char   *xparmdocumentsdb;

    LONGX    xinirec;     // docvdb
    LONGX    xoutcrec;    // docydb ctl
    LONGX    xoutirec;    // docydb 

    char    xdocvdb[CIMPL+1];
    char    xdocydb[CIMPL+1];

    DOCVEC *xdocvector;
    LONGX    xdocvectcnt;
    
    LISTA  *xlista; /* list of related */
    LISTA  *xlisthdr;
    LISTA  *xlisttail;
    LISTA  *xlistavail; /* list of related */

//} WTFDOC_XWT;
    

    char   *xcollectiondb;

    LONGX    xirec;       // colcdb/colndb/coljdb/docvdb_V3
    LONGX    xirectv;     // colvdb
    LONGX    xifrec;      // colxdb

    char    xcolcdb[CIMPL+1];
    char    xcolndb[CIMPL+1];
    char    xcolvdb[CIMPL+1];
    char    xcolxdb[CIMPL+1];

    VECTEL *xVECTORS;         /* VECTORS[] - readmaxtv */
    LONGX   *xVECTOFF;
    LONGX   *xVECTCNT;
    LONGX   *xVECTCMF;

    float  *xWEIGHT;           /* WEIGHT[] - readnterms */
    char   *xHITS;             /* HITS[] */
    LONGX    xhitbytes;
    LONGX    xCOLLECTION_SIZE;  /* N - #docs in collection */

    char   *xtable;
    LONGX    xtabentries;
    int     xhwidth;

    int     xreadwidth;
    LONGX    xreadcategories;       //1
    LONGX    xreadmaxprim;          //1
    LONGX    xreadcollection_size;  //1
    LONGX    xreadpostingson_size;  //20110812
    int     xreadmaxtv;
    LONGX    xreadnterms;
    int     xreadwritebin;
    int     xreadnlangs;

    float   xreadmaxrf;
    int     xreadcollmfntag;
    int     xreadcolljd;
    
    unsigned char xxisisuctab[256];
    unsigned char xxisiswctab[256];
    int           xxisiswctot;
    
    int     xcollcolljd; //
    int     xcollcollmfntag; //
    float   xcollmaxrf; //
    int     xcollmaxtv; //
    int     xcollmaxrel; //
    float   xcollminsim; //
    float   xcollmaxsim; //
        
#if PARMLANG
    char   *xlangshits[MAXPARMLANG];
    char   *xlangsname[MAXPARMLANG];
    LONGX    xlangscount[MAXPARMLANG];
    int     xnlangs;
    int     xlangidx;
    int     xparmchklang;
#endif /* PARMLANG */

} WTFUN_XWT;



typedef struct wtfun_array {

     //WTFUN_XWT *wtf4p;
     WTFUN_XWT  *vwtfp[MAXWTFUN]; // wtf4p area + collections
     char vwtfname[MAXWTFUN][CIMPL+1];
     int nwtfs;
     
     int   globalport;
     int   globalsock;
     LONGX  globalmaxmfrl;
     char *globaldatabuff;
     //char *globalcipar;
    
     //JD env
     //char coljdb[CIMPL+1];
          
     LONGX *xJDCOLFREQ;     /* f(t) - #occs of jd in collection - Au set */
     LONGX *xJDDOCFREQ;     /* f(t) - #occs of jd in document   - AuTi subset */
     LONGX *xJDSIMFREQ;     /* f(t) - #occs of jd in similar    - Similar in col */
     
     float *xJDCOLPESO;    /* f(t) - pesos of jd in collection - Au set */
     float *xJDDOCPESO;    /* f(t) - pesos of jd in document   - AuTi subset */
     float *xJDSIMPESO;    /* f(t) - pesos of jd in similar    - Similar in col */

     char *xjdtable;
     LONGX  xjdtabentries;
     int   xjdhwidth;
     int   xjdtabcats;
     int   xjdtabwidth;
     LONGX  xjdmaxprim;
    
     LISTJD *xlistjd;      /* results */
     LISTJD *xlistjdhdr;   /* results hdr */

     int  xjk_n;                    // next pdl 
     int  xjk_jdi[MAXJDS];          // jk jd index
     int  xjk_tabcats[MAXJDS];      // jk jdtabcats
     int  xjk_tabwidth[MAXJDS];     // jk jdtabwidth
     
     int xjd_n;                     // next pdl
     char xjd_name[MAXJDS][LE2+1];  // jd name
     FMT_CODE *xjd_pgmp[MAXJDS];    // fmt pseudo code
     char *xjd_areap;               // format area
     int xjd_asize;                 // format area size

     int  xjkv_n;                    // next pdl 
     int  xjkv_jdvi[MAXJVS];         // jkv jdv index
     int  xjkv_disp1[MAXJVS];        // jkv jdv disp1
     int  xjkv_disp2[MAXJVS];        // jkv jdv disp2
     
     int xjdv_n;                     // next pdl
     char xjdv_name[MAXJVS][LE2+1];  // jdv name
     FMT_CODE *xjdv_pgmp[MAXJVS];    // fmt pseudo code
     char *xjdv_areap;               // format area
     int xjdv_asize;                 // format area size
     
     char *xconvent[256];           // chars to entities

} WTFUN_ARRAY;




/* function prototypes 
*/
     WTFUN_ARRAY *wtfnew(int cmd);
     WTFUN_ARRAY *wtfset(int cmd, WTFUN_ARRAY *awtfp, char *envp);
     int wtfdel(int cmd, WTFUN_ARRAY *awtfp);
     
     WTFUN_XWT *wtnew(int cmd);
     int wtdel(int cmd, WTFUN_XWT *xwtp);
     int wtexit(int cmd, int rc);
     
     int loadcoll (int cmd, WTFUN_ARRAY *awtfp, char *parmstring, char *reply);
     
     int trigcoll (int cmd, WTFUN_ARRAY *awtfp, char *parmstring, char *reply);
     
     int loadtrig (int cmd, WTFUN_ARRAY *awtfp, int coll, char *text, int textlen, float parmmaxrf );
     int w2reply ( int cmd, WTFUN_ARRAY *awtfp, int coll, int yocc, LISTA *l, int joinjd, int showv3, int showid, char *reply, char *batchp );
     int w2topjd (int cmd, WTFUN_ARRAY *awtfp, int coll, LONGX *xFREQ, float *xPESO, int howmany, float minweight);
     int w2docjk (int cmd, WTFUN_ARRAY *awtfp, int coll, char *parmeval);
     int w2docjv (int cmd, WTFUN_ARRAY *awtfp, int coll, char *parmdisp);
#if WHENFUN
     int w2rcol  ( int cmd, WTFUN_ARRAY *awtfp, int coll );
     int w2rcox  ( int cmd, WTFUN_ARRAY *awtfp, int coll );
#endif


/* ==----------------------------- wtfun.h ------------------------------== */
