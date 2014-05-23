/* ------------------------------ mxtxt.c --------------------------------- */

#include <stdio.h>
#include <string.h>

#if 1
#if CIAPI
#include "ciapi.h"   /* CISIS Interface API header file -
                  includes cisis, implements cirun.h and ci*.c using ci*.?pp */

#if !MXFUN
extern void *ciapip;
#endif /* MXFUN */

#else /* CIAPI */
#include "cisis.h"                  /* CISIS Interface */
#endif /* CIAPIP */
#include "mx.h"                     /* MX Program header file */
#else
#include "\cisis\include\cisis.h"   /* CISIS Interface header file */
#include "\cisis\mx\mx.h"           /* MX Program header file */
#endif

int mxtext(xmxp,irec,textp)
void *xmxp;
LONGX irec;
char *textp;
{
    RECSTRU *recp;  /* mandatory for defines REC,MFR,DIR,FIELDP */
    UWORD diridx,dirloop,chrloop;
    int textlen;
    char *p;

    recp=vrecp[irec];

    textlen=strlen(textp);
    mxtext_found=0;


        if (RECrc != RCNORMAL) return(0);

        for (diridx=0, dirloop=MFRnvf; dirloop--; diridx++) {
            p=FIELDP(diridx);
            for (chrloop=DIRlen(diridx); chrloop; p++) {
                if (*p == *textp)
                    if (chrloop >= textlen)
                        if (memcmp(p,textp,textlen) == 0) {
                            mxtext_found=1;
                            break;
                        }
                chrloop--;
            }
            if (mxtext_found) {
                if (mxtext_pshow) {
                    printf("mfn %6"_LD_"|tag %5u|occ %3d|%s\n",
                        MFRmfn,DIRtag(diridx),fldocc(irec,diridx),textp);
                    /* prtfield(recp,diridx); */
                    printf("%3u  %c",DIRtag(diridx),OPENFCHR);
                    p=FIELDP(diridx);
                    for (chrloop=DIRlen(diridx); chrloop--; p++)
                        printf("%c",*p);
                    printf("%c\n",CLOSFCHR);
                }
                break;
            }
        }

    return(0); /* (mxtext_found); */ /* AOT, 02/04/2001 */

} /* end of main */


