/* ifload.c - Load an inverted file from sorted link files
*/
#include <stdio.h>
#include <string.h>
#include "cisis.h"
#include "cirun.h"

void main(argc,argv)
int argc;
char *argv[];
{
    int rc;
#if IFFUN
    char area[BUFSIZ+BUFSIZ+1];
    char *p;
    int iarg;
    for (p=area, iarg=1; iarg<argc; iarg++) {
        strcpy(p,argv[iarg]); p+=strlen(p); if (iarg<argc-1) *p++='\n';
    }
    *p='\0';
    rc=cisis_ifload(area);
#else /* IFFUN */
    rc=ifloadexec(argc,argv);
#endif /* IFFUN */
    if (rc > 1 /* error */) {
        char *msgp,line[80];
        rc = rc - 1 /* error */;
        switch (rc) {
          case 1:  msgp="ifload/CIIFL/link file/open"; break;
          case 2:  msgp="ifload/CIIFL/invalid link record/mpe"; break;
          case 3:  msgp="ifload/CIIFL/invalid link record/fix"; break;
          case 4:  msgp="ifload/CIIFL/invalid link record/mfn"; break;
          case 5:  msgp="ifload/CIIFL/invalid link record/key"; break;
          case 6:  msgp="ifload/CIIFL/read"; break;
          case 7:  msgp="ifload/CIIFL/keys not sorted"; break;
          case 8:  msgp="ifload/CIIFL/keys/postings not sorted"; break;
          case 9:  msgp="ifload/CIIFL/ifpbuff"; break;
          default: sprintf(line,"ifload/error#%d",rc); msgp=line;
        }
        fatal(msgp);
    }
    exit(rc);
 }

