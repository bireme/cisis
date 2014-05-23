
#if FATRAPWT
    int     z_xerrno;
    int     z_errcod;
    char    z_iomsg[MAXFATML+1];
    jmp_buf z_jumper;                                                        
    int     z_jumprc;
#endif

    unsigned char hitbitmask[8] = { 0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01 };
    unsigned char hitmask;
    LONGX hitmfn;
    LONGX hitoffset;


    int parmcollapse=0;
    LONGX currentmfncollapsed=0;

    int parmcollmfntag=0;
    LONGX collhitmfn;
    
    float parmmaxrf; //=0.50;
    int   parmmaxtv; //=50;
    
    LONGX  parmmaxrel; //=10;        
    float parmminsim; //=0.7;
    float parmmaxsim; //=1.0;
 
    int parmtrace=0;
    int parmtrace2=0;
    LONGX parmtell=cmd;
    LONGX parmtell2=cmd;

//    int parmloadinverted=1;   // cond always true
    int alwaysloadvectors=1;    // cond always true   
    int alwaysrcolwritebin=1; // cond always true
    int alwaysrdocwritebin=1; // cond always true
    int alwaysmakemfnhits=1;  // cond always true
    
    int parmwsoma=1;
#if SYNTAX
    char xa[20];
#endif

    int parmlanguage=0;

    int argc=0;
    char *argv[MAXPRMS1+1];

    RECSTRU *recp;  /* mandatory for defines REC,MFR,DIR,FIELDP */

    DOCVEC *docv;
    
    LONGX docmfn;
    FFI docloop;
    LONGX dochidx;

    int found;

    LONGX mfn;
    DIRSTRU *dirp;
    int xdir;
    int iocc;
    char *p,*q;
    char *fldp;
    int left;
    FFI loop;
    FFI keylen;

    LONGX count;
    int iarg;
    
    int replyhere=1;
    char *dbdocydb="null";    
    
    time_t secs_now;
    time_t secs_start,secs_colok,secs_startdoc,secs_docok;
    
    int parmdebug=0;
    
    int yocc;
    
