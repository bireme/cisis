/* ==----------------------------- wtrunz.h ------------------------------== */

// init/allocated in wtfnew / wtfset

//#undef  awtgblport                //((WTFUN_ARRAY *)awtfp)->globalport
//#undef  awtgblsock                //((WTFUN_ARRAY *)awtfp)->globalsock
#undef  awtmaxmfrl                //((WTFUN_ARRAY *)awtfp)->globalmaxmfrl
#undef  awtdatabuff               //((WTFUN_ARRAY *)awtfp)->globaldatabuff
//#undef  awtcipar                  //((WTFUN_ARRAY *)awtfp)->globalcipar

#undef  JDCOLFREQ                 //((WTFUN_ARRAY *)awtfp)->xJDCOLFREQ
#undef  JDDOCFREQ                 //((WTFUN_ARRAY *)awtfp)->xJDDOCFREQ
#undef  JDSIMFREQ                 //((WTFUN_ARRAY *)awtfp)->xJDSIMFREQ

#undef  JDCOLPESO                 //((WTFUN_ARRAY *)awtfp)->xJDCOLPESO
#undef  JDDOCPESO                 //((WTFUN_ARRAY *)awtfp)->xJDDOCPESO
#undef  JDSIMPESO                 //((WTFUN_ARRAY *)awtfp)->xJDSIMPESO

#undef  jdtable                   //((WTFUN_ARRAY *)awtfp)->xjdtable
#undef  jdtabentries              //((WTFUN_ARRAY *)awtfp)->xjdtabentries
#undef  jdhwidth                  //((WTFUN_ARRAY *)awtfp)->xjdhwidth
#undef  jdtabcats                 //((WTFUN_ARRAY *)awtfp)->xjdtabcats
#undef  jdtabwidth                //((WTFUN_ARRAY *)awtfp)->xjdtabwidth
#undef  jdmaxprim                 //((WTFUN_ARRAY *)awtfp)->xjdmaxprim

#undef  listjd                    //((WTFUN_ARRAY *)awtfp)->xlistjd
#undef  listjdhdr                 //((WTFUN_ARRAY *)awtfp)->xlistjdhdr

#undef  jk_n                      //((WTFUN_ARRAY *)awtfp)->xjk_n
#undef  jk_jdi                    //((WTFUN_ARRAY *)awtfp)->xjk_jdi
#undef  jk_tabcats                //((WTFUN_ARRAY *)awtfp)->xjk_tabcats
#undef  jk_tabwidth               //((WTFUN_ARRAY *)awtfp)->xjk_tabwidth

#undef  jd_n                      //((WTFUN_ARRAY *)awtfp)->xjd_n
#undef  jd_name                   //((WTFUN_ARRAY *)awtfp)->xjd_name
#undef  jd_pgmp                   //((WTFUN_ARRAY *)awtfp)->xjd_pgmp
#undef  jd_areap                  //((WTFUN_ARRAY *)awtfp)->xjd_areap
#undef  jd_asize                  //((WTFUN_ARRAY *)awtfp)->xjd_asize

#undef  jkv_n                     //((WTFUN_ARRAY *)awtfp)->xjkv_n
#undef  jkv_jdvi                  //((WTFUN_ARRAY *)awtfp)->xjkv_jdvi
#undef  jkv_disp1                 //((WTFUN_ARRAY *)awtfp)->xjkv_disp1
#undef  jkv_disp2                 //((WTFUN_ARRAY *)awtfp)->xjkv_disp2

#undef  jdv_n                     //((WTFUN_ARRAY *)awtfp)->xjdv_n
#undef  jdv_name                  //((WTFUN_ARRAY *)awtfp)->xjdv_name
#undef  jdv_pgmp                  //((WTFUN_ARRAY *)awtfp)->xjdv_pgmp
#undef  jdv_areap                 //((WTFUN_ARRAY *)awtfp)->xjdv_areap
#undef  jdv_asize                 //((WTFUN_ARRAY *)awtfp)->xjdv_asize

#undef  convent                   //((WTFUN_ARRAY *)awtfp)->xconvent


// current

#undef  parmtext                  //((WTFUN_XWT *)xwtp)->xparmtext

#undef  zirec                     //((WTFUN_XWT *)xwtp)->xzirec
#undef  trigbuff                  //((WTFUN_XWT *)xwtp)->xtrigbuff

#undef  COLLECTION                //((WTFUN_XWT *)xwtp)->xCOLLECTION
#undef  DOCUMENT                  //((WTFUN_XWT *)xwtp)->xDOCUMENT
#undef  GLOBALW                   //((WTFUN_XWT *)xwtp)->xGLOBALW
#undef  PRODW                     //((WTFUN_XWT *)xwtp)->xPRODW

#undef  list1a                    //((WTFUN_XWT *)xwtp)->xlist1a

//

#undef  lista                     //((WTFUN_XWT *)xwtp)->xlista
#undef  listhdr                   //((WTFUN_XWT *)xwtp)->xlisthdr
#undef  listtail                  //((WTFUN_XWT *)xwtp)->xlisttail
#undef  listavail                 //((WTFUN_XWT *)xwtp)->xlistavail

#undef  docvector                 //((WTFUN_XWT *)xwtp)->xdocvector
#undef  docvectcnt                //((WTFUN_XWT *)xwtp)->xdocvectcnt

#undef  VECTORS                   //((WTFUN_XWT *)xwtp)->xVECTORS
#undef  VECTOFF                   //((WTFUN_XWT *)xwtp)->xVECTOFF
#undef  VECTCNT                   //((WTFUN_XWT *)xwtp)->xVECTCNT
#undef  VECTCMF                   //((WTFUN_XWT *)xwtp)->xVECTCMF

#undef  WEIGHT                    //((WTFUN_XWT *)xwtp)->xWEIGHT
#undef  HITS                      //((WTFUN_XWT *)xwtp)->xHITS
#undef  COLLECTION_SIZE           //((WTFUN_XWT *)xwtp)->xCOLLECTION_SIZE
#undef  hitbytes                  //((WTFUN_XWT *)xwtp)->xhitbytes

#undef  table                     //((WTFUN_XWT *)xwtp)->xtable
#undef  tabentries                //((WTFUN_XWT *)xwtp)->xtabentries
#undef  hwidth                    //((WTFUN_XWT *)xwtp)->xhwidth

#undef  irec                      //((WTFUN_XWT *)xwtp)->xirec
#undef  irectv                    //((WTFUN_XWT *)xwtp)->xirectv
#undef  inirec                    //((WTFUN_XWT *)xwtp)->xinirec
#undef  outcrec                   //((WTFUN_XWT *)xwtp)->xoutcrec
#undef  outirec                   //((WTFUN_XWT *)xwtp)->xoutirec
#undef  ifrec                     //((WTFUN_XWT *)xwtp)->xifrec

#undef  readwidth                 //((WTFUN_XWT *)xwtp)->xreadwidth
#undef  readcategories            //((WTFUN_XWT *)xwtp)->xreadcategories
#undef  readmaxprim               //((WTFUN_XWT *)xwtp)->xreadmaxprim
#undef  readcollection_size       //((WTFUN_XWT *)xwtp)->xreadcollection_size
#undef  readpostingson_size       //((WTFUN_XWT *)xwtp)->xreadpostingson_size
#undef  readmaxtv                 //((WTFUN_XWT *)xwtp)->xreadmaxtv
#undef  readnterms                //((WTFUN_XWT *)xwtp)->xreadnterms
#undef  readwritebin              //((WTFUN_XWT *)xwtp)->xreadwritebin
#undef  readnlangs                //((WTFUN_XWT *)xwtp)->xreadnlangs
#undef  readmaxrf                 //((WTFUN_XWT *)xwtp)->xreadmaxrf
#undef  readcollmfntag            //((WTFUN_XWT *)xwtp)->xreadcollmfntag
#undef  readcolljd                //((WTFUN_XWT *)xwtp)->xreadcolljd
#undef  xisisuctab                //((WTFUN_XWT *)xwtp)->xxisisuctab
#undef  xisiswctab                //((WTFUN_XWT *)xwtp)->xxisiswctab
#undef  xisiswctot                //((WTFUN_XWT *)xwtp)->xxisiswctot

#undef  collcolljd                //((WTFUN_XWT *)xwtp)->xcollcolljd
#undef  collcollmfntag            //((WTFUN_XWT *)xwtp)->xcollcollmfntag
#undef  collmaxrf                 //((WTFUN_XWT *)xwtp)->xcollmaxrf
#undef  collmaxtv                 //((WTFUN_XWT *)xwtp)->xcollmaxtv
#undef  collmaxrel                //((WTFUN_XWT *)xwtp)->xcollmaxrel
#undef  collminsim                //((WTFUN_XWT *)xwtp)->xcollminsim
#undef  collmaxsim                //((WTFUN_XWT *)xwtp)->xcollmaxsim

#undef  colcdb                    //((WTFUN_XWT *)xwtp)->xcolcdb
#undef  colndb                    //((WTFUN_XWT *)xwtp)->xcolndb
#undef  colvdb                    //((WTFUN_XWT *)xwtp)->xcolvdb
#undef  colxdb                    //((WTFUN_XWT *)xwtp)->xcolxdb
#undef  docvdb                    //((WTFUN_XWT *)xwtp)->xdocvdb
#undef  docydb                    //((WTFUN_XWT *)xwtp)->xdocydb
#undef  parmdocumentsdb           //((WTFUN_XWT *)xwtp)->xparmdocumentsdb
#undef  collectiondb              //((WTFUN_XWT *)xwtp)->xcollectiondb

#if PARMLANG
#undef  langshits                 //((WTFUN_XWT *)xwtp)->xlangshits
#undef  langsname                 //((WTFUN_XWT *)xwtp)->xlangsname
#undef  langscount                //((WTFUN_XWT *)xwtp)->xlangscount
#undef  nlangs                    //((WTFUN_XWT *)xwtp)->xnlangs
#undef  langidx                   //((WTFUN_XWT *)xwtp)->xlangidx
#endif

/* ==----------------------------- wtrunz.h ------------------------------== */
