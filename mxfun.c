/* ==----------------------------- mxfun.c ------------------------------== */

/*char * */ parmpftoutp=NULL;
/*FILE * */ parmpftoutfp=NULL;
/*char * */ parmprologp=NULL;
/*char * */ parmepilogp=NULL;
/*LONGX   */ irecepilog=0L;
#if CICGI
/*LONGX*/ wwrec1=0L;
/*LONGX*/ wwrec2=0L;
/*char * */ cgixpok=NULL;
#endif /* CICGI */

/*char * */ fmtarea=NULL;
/*LONGX*/ fmtlw=79;
/*LONGX*/ fmtasize=MAXMFRL;

/*char * */ ptabtable=NULL;
/*int    */ ptabwidth=100; //LE2+LE2;
/*LONGX   */ ptabcategories=300000;
///*LONGX   */ ptabmaxprim;
/*LONGX * */ ptabCOUNT=NULL;  /* n(t) */

/*LONGX*/ parmtell = 0;
/*LONGX*/ parmload = 0;
/*int*/ parmtrace = 0;
/*int*/ parmcopy  = 0;

/*char * */ parmdupp = NULL;
/*int*/ parmdupx = 0;

/*int*/ parmiso1  = 0;
/*int*/ parmiso2  = 0;
/*FILE * */ fpfix = NULL;    /*parmiso2=2 */

#if RECXPND
/*int*/ parmxpn01 = 0;
/*int*/ parmxpn02 = 0;
#endif
#if MXGIZP
/*char * */ parmgizp = NULL;
/*int*/ parmgizhp = 0;
#endif

/*char * */ prompt1p = "..";
/*char * */ prompt2p = "->";

/*FILE * */ fpseq = NULL;
/*FILE * */ fpprm = NULL;

/*int*/ nfpprms=0;

/*int*/ tmpseq=0;                   /* mxexit() */
/*char * */ mx_bup=NULL;/*free*/              /* global */
/*LONGX*/ mx_buplen=0L;              /* global */

/*LONGX*/ upcrec=0L;                 /* global */
/*LONGX*/ upirec=0L;                 /* global */

#if MXFULLINV
/*char * */ fullinv_ifnp=NULL;
/*char * */ fullinv_ln1p=NULL;
/*char * */ fullinv_ln2p=NULL;
/*char * */ fullinv_lk1p=NULL;
/*char * */ fullinv_lk2p=NULL;
/*int*/ fullinv_pstflag=IFUPISIS;
/*int*/ fullinv_reset=1;
/*int*/ fullinv_balan=1;
/*int*/ fullinv_keep=0;
#if LIND
/*LONGX*/ fullinv_maxmfn=0L;
#endif
/*char * */ fullinv_master=NULL;
#endif

#if MXIFUPD
/*LONGX*/ mstcrec=0L;/*free*/
/*LONGX*/ mstirec=0L;/*free*/
/*char * */ ifnp=NULL;
/*int*/ svdifupx=0;
/*int*/ svdifupy=0;
#endif

#if MXIFUPD
/*int*/ pstflag=IFUPISIS;
/*int*/ endup=IFUPCORE;
#endif

#if MXIFUPD || MXFST
/*char * */ stwp=NULL;
/*FST_CODE * */ fstpgmp=NULL;
/*char * */ lnk1p=NULL;
/*char * */ lnk2p=NULL;
/*LONGX*/ maxlk1=1000;
/*LONGX*/ maxlk2=500;
#endif

/*LONGX*/ nprms=0;                   /* actual #entries              */
/*LONGX*/ maxnprm=MAXNPRM;           /* runtime value                */


/* ------------------------------ mx.h ------------------------------------- */

#if RECXPND
#endif

#if RECGIZM
#endif




/* ------------------------------ mxbol.c --------------------------------- */

/*char * */ boolqryp=NULL;
/*LONGX*/ boolridx=0;
/*LONGX*/ boolhits=0;
/*LONGX*/ boolh=0;

/*char * */ booldbnp = NULL;          /* b4x query dbn */
/*LONGX*/ booltell=1L;               /* 0L = no msg */
/*LONGX*/ booltlir=0L;               /* b40 invx */
#if CIB71
/*b7_CIB7 * */ cib7p=NULL;
/*char*/ booltmpnam[0]='\0';/*[CIMPL+1]={'\0'}*/    /* b72 MFQTHMEM tmp file */
#else /* CIB71 */
#if LIND || CIB64
/*LONGX*/ boolnby1=0L;               /* b62 nbytes1 */
/*LONGX*/ boolnbyb=0L;               /* b62 b6bufsiz */
/*LONGX*/ boolnby0=0L;               /* b62 b5irxbyts0 */
/*char*/ booltmpnam[0]='\0';/*[CIMPL+1]={'\0'}*/    /* b62 MFQTHMEM tmp file */
/*int*/ boolerrno=0;                /* b62 errno */
#else
/*LONGX*/ boolnby2=MAXMFRL;          /* b42 nbytes2 */
#endif
#endif /* CIB71 */

/* ------------------------------ mxrel.c --------------------------------- */

/* ========================== mp_scan/mp_edit ============================= */

/*int*/ mp_n = 0;
/*char * */ mp_areap=NULL;

/* ====================== mj_scan/mj_edit/mj_join ========================= */

/*LONGX*/ mj_tidx=0;
/*char * */ mj_areap=NULL;
/*char * */ mj_buffup=NULL;
/*int*/ mj_n=0;
/*LONGX*/ mj_jmax=LONGX_MAX;
/*UWORD*/ mj_jtag=32000;

/* ------------------------------ mxtxt.c --------------------------------- */

/*int*/ mxtext_found=0;
/*int*/ mxtext_pshow=0;

/* ==----------------------------- mxfun.c ------------------------------== */

