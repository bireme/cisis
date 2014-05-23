#include "cib60.hpp"

#include "cib50m.c"
#include "cib51.c"
//#include "cib50r.c"
#include "cib50t.c"
#include "cib62.c"

  void CIB60::cib51_init() {
    b5setno=0;                 /* current query set no */
    b5batchp=NULL;            /* fldupdat */
  }

  void CIB60::cib62_init() {
   b6tw2mh = 0;                 /* SOING */
   b6_gidbnp=NULL;	        /* dbn.par */
   b5maxbyts0=0L;               /* max bytes in bitstring - to override */
   b5prodtag=0;                 /* product tag on qrydbnp - via btch0p */
   b5prodp=NULL;                /* product's name */
   b53_tmpnam[0] = '\0';        /* file name for temporary storage */
   b53_fd=0;			/* open() */
   b5_savdbxp = NULL;		/* '@' processing */
   b6fd=0;			/* open() - 06/03/95 */
   b5rootf=OFF;
   b5rootmsg=OFF;
#if BIGREC
#else /* BIGREC */
   b6bufferp = NULL;		/* b6_run() processing buffer */
#endif /* BIGREC */
   b5_isn1=0L;			/* query #0 lower limit */
   b5_isn2=LONGX_MAX;		/* query #0 upper limit */
   keepndbx=0;
   b5_pfxmdl=0;
  }

  CIB60::CIB60(CISISX *parm_cisisxp) {
   cisisxp = parm_cisisxp;
   cib51_init();
   cib62_init();
  }

  CIB60::~CIB60(void) {
   if (b6bufferp)
    b6_hbfree (&b6bufferp);
  }

