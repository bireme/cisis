/* ==----------------------------- wtrun.h ------------------------------== */

// init/allocated in wtfnew / wtfset

//#define awtgblport                ((WTFUN_ARRAY *)awtfp)->globalport
//#define awtgblsock                ((WTFUN_ARRAY *)awtfp)->globalsock
#define awtmaxmfrl                ((WTFUN_ARRAY *)awtfp)->globalmaxmfrl
#define awtdatabuff               ((WTFUN_ARRAY *)awtfp)->globaldatabuff
//#define awtcipar                  ((WTFUN_ARRAY *)awtfp)->globalcipar

#define JDCOLFREQ                 ((WTFUN_ARRAY *)awtfp)->xJDCOLFREQ
#define JDDOCFREQ                 ((WTFUN_ARRAY *)awtfp)->xJDDOCFREQ
#define JDSIMFREQ                 ((WTFUN_ARRAY *)awtfp)->xJDSIMFREQ

#define JDCOLPESO                 ((WTFUN_ARRAY *)awtfp)->xJDCOLPESO
#define JDDOCPESO                 ((WTFUN_ARRAY *)awtfp)->xJDDOCPESO
#define JDSIMPESO                 ((WTFUN_ARRAY *)awtfp)->xJDSIMPESO

#define jdtable                   ((WTFUN_ARRAY *)awtfp)->xjdtable
#define jdtabentries              ((WTFUN_ARRAY *)awtfp)->xjdtabentries
#define jdhwidth                  ((WTFUN_ARRAY *)awtfp)->xjdhwidth
#define jdtabcats                 ((WTFUN_ARRAY *)awtfp)->xjdtabcats
#define jdtabwidth                ((WTFUN_ARRAY *)awtfp)->xjdtabwidth
#define jdmaxprim                 ((WTFUN_ARRAY *)awtfp)->xjdmaxprim

#define listjd                    ((WTFUN_ARRAY *)awtfp)->xlistjd
#define listjdhdr                 ((WTFUN_ARRAY *)awtfp)->xlistjdhdr

#define jk_n                      ((WTFUN_ARRAY *)awtfp)->xjk_n
#define jk_jdi                    ((WTFUN_ARRAY *)awtfp)->xjk_jdi
#define jk_tabcats                ((WTFUN_ARRAY *)awtfp)->xjk_tabcats
#define jk_tabwidth               ((WTFUN_ARRAY *)awtfp)->xjk_tabwidth

#define jd_n                      ((WTFUN_ARRAY *)awtfp)->xjd_n
#define jd_name                   ((WTFUN_ARRAY *)awtfp)->xjd_name
#define jd_pgmp                   ((WTFUN_ARRAY *)awtfp)->xjd_pgmp
#define jd_areap                  ((WTFUN_ARRAY *)awtfp)->xjd_areap
#define jd_asize                  ((WTFUN_ARRAY *)awtfp)->xjd_asize

#define jkv_n                     ((WTFUN_ARRAY *)awtfp)->xjkv_n
#define jkv_jdvi                  ((WTFUN_ARRAY *)awtfp)->xjkv_jdvi
#define jkv_disp1                 ((WTFUN_ARRAY *)awtfp)->xjkv_disp1
#define jkv_disp2                 ((WTFUN_ARRAY *)awtfp)->xjkv_disp2

#define jdv_n                     ((WTFUN_ARRAY *)awtfp)->xjdv_n
#define jdv_name                  ((WTFUN_ARRAY *)awtfp)->xjdv_name
#define jdv_pgmp                  ((WTFUN_ARRAY *)awtfp)->xjdv_pgmp
#define jdv_areap                 ((WTFUN_ARRAY *)awtfp)->xjdv_areap
#define jdv_asize                 ((WTFUN_ARRAY *)awtfp)->xjdv_asize

#define convent                   ((WTFUN_ARRAY *)awtfp)->xconvent


// current

#define parmtext                  ((WTFUN_XWT *)xwtp)->xparmtext

#define zirec                     ((WTFUN_XWT *)xwtp)->xzirec
#define trigbuff                  ((WTFUN_XWT *)xwtp)->xtrigbuff

#define COLLECTION                ((WTFUN_XWT *)xwtp)->xCOLLECTION
#define DOCUMENT                  ((WTFUN_XWT *)xwtp)->xDOCUMENT
#define GLOBALW                   ((WTFUN_XWT *)xwtp)->xGLOBALW
#define PRODW                     ((WTFUN_XWT *)xwtp)->xPRODW

#define list1a                    ((WTFUN_XWT *)xwtp)->xlist1a

//

#define lista                     ((WTFUN_XWT *)xwtp)->xlista
#define listhdr                   ((WTFUN_XWT *)xwtp)->xlisthdr
#define listtail                  ((WTFUN_XWT *)xwtp)->xlisttail
#define listavail                 ((WTFUN_XWT *)xwtp)->xlistavail

#define docvector                 ((WTFUN_XWT *)xwtp)->xdocvector
#define docvectcnt                ((WTFUN_XWT *)xwtp)->xdocvectcnt

#define VECTORS                   ((WTFUN_XWT *)xwtp)->xVECTORS
#define VECTOFF                   ((WTFUN_XWT *)xwtp)->xVECTOFF
#define VECTCNT                   ((WTFUN_XWT *)xwtp)->xVECTCNT
#define VECTCMF                   ((WTFUN_XWT *)xwtp)->xVECTCMF

#define WEIGHT                    ((WTFUN_XWT *)xwtp)->xWEIGHT
#define HITS                      ((WTFUN_XWT *)xwtp)->xHITS
#define COLLECTION_SIZE           ((WTFUN_XWT *)xwtp)->xCOLLECTION_SIZE
#define hitbytes                  ((WTFUN_XWT *)xwtp)->xhitbytes

#define table                     ((WTFUN_XWT *)xwtp)->xtable
#define tabentries                ((WTFUN_XWT *)xwtp)->xtabentries
#define hwidth                    ((WTFUN_XWT *)xwtp)->xhwidth

#define irec                      ((WTFUN_XWT *)xwtp)->xirec
#define irectv                    ((WTFUN_XWT *)xwtp)->xirectv
#define inirec                    ((WTFUN_XWT *)xwtp)->xinirec
#define outcrec                   ((WTFUN_XWT *)xwtp)->xoutcrec
#define outirec                   ((WTFUN_XWT *)xwtp)->xoutirec
#define ifrec                     ((WTFUN_XWT *)xwtp)->xifrec

#define readwidth                 ((WTFUN_XWT *)xwtp)->xreadwidth
#define readcategories            ((WTFUN_XWT *)xwtp)->xreadcategories
#define readmaxprim               ((WTFUN_XWT *)xwtp)->xreadmaxprim
#define readcollection_size       ((WTFUN_XWT *)xwtp)->xreadcollection_size
#define readpostingson_size       ((WTFUN_XWT *)xwtp)->xreadpostingson_size
#define readmaxtv                 ((WTFUN_XWT *)xwtp)->xreadmaxtv
#define readnterms                ((WTFUN_XWT *)xwtp)->xreadnterms
#define readwritebin              ((WTFUN_XWT *)xwtp)->xreadwritebin
#define readnlangs                ((WTFUN_XWT *)xwtp)->xreadnlangs
#define readmaxrf                 ((WTFUN_XWT *)xwtp)->xreadmaxrf
#define readcollmfntag            ((WTFUN_XWT *)xwtp)->xreadcollmfntag
#define readcolljd                ((WTFUN_XWT *)xwtp)->xreadcolljd
#define xisisuctab                ((WTFUN_XWT *)xwtp)->xxisisuctab
#define xisiswctab                ((WTFUN_XWT *)xwtp)->xxisiswctab
#define xisiswctot                ((WTFUN_XWT *)xwtp)->xxisiswctot

#define collcolljd                ((WTFUN_XWT *)xwtp)->xcollcolljd
#define collcollmfntag            ((WTFUN_XWT *)xwtp)->xcollcollmfntag
#define collmaxrf                 ((WTFUN_XWT *)xwtp)->xcollmaxrf
#define collmaxtv                 ((WTFUN_XWT *)xwtp)->xcollmaxtv
#define collmaxrel                ((WTFUN_XWT *)xwtp)->xcollmaxrel
#define collminsim                ((WTFUN_XWT *)xwtp)->xcollminsim
#define collmaxsim                ((WTFUN_XWT *)xwtp)->xcollmaxsim

#define colcdb                    ((WTFUN_XWT *)xwtp)->xcolcdb
#define colndb                    ((WTFUN_XWT *)xwtp)->xcolndb
#define colvdb                    ((WTFUN_XWT *)xwtp)->xcolvdb
#define colxdb                    ((WTFUN_XWT *)xwtp)->xcolxdb
#define docvdb                    ((WTFUN_XWT *)xwtp)->xdocvdb
#define docydb                    ((WTFUN_XWT *)xwtp)->xdocydb
#define parmdocumentsdb           ((WTFUN_XWT *)xwtp)->xparmdocumentsdb
#define collectiondb              ((WTFUN_XWT *)xwtp)->xcollectiondb

#if PARMLANG
#define langshits                 ((WTFUN_XWT *)xwtp)->xlangshits
#define langsname                 ((WTFUN_XWT *)xwtp)->xlangsname
#define langscount                ((WTFUN_XWT *)xwtp)->xlangscount
#define nlangs                    ((WTFUN_XWT *)xwtp)->xnlangs
#define langidx                   ((WTFUN_XWT *)xwtp)->xlangidx
#endif

/* ==----------------------------- wtrun.h ------------------------------== */
