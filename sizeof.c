#include <stdio.h>
#include "cisis.h"
#include "cirun.h"

void main(argc,argv)
int argc;
char *argv[];
{
 TRMSTRU *trmp;
 printf("%s",cicopyr("?Utility SIZEOF"));
 printf("\n");
 printf("size_t=%d\n",sizeof(size_t));
 printf("off_t=%d\n",sizeof(off_t));
 printf("short int=%d\n",sizeof(short));
 printf("int=%d\n",sizeof(int));
 printf("long int=%d\n",sizeof(long));
 printf("double int=%d\n",sizeof(double));
 printf("(char *)=%d\n",sizeof(char *));
 printf("FFI=%d\n",sizeof(FFI));
 printf("UWORD=%d\n",sizeof(UWORD));
 printf("LONGX=%d\n",sizeof(LONGX));
 printf("\n");
 printf("MFRL_MAX=%d=0x%04x MAXMFRL=%d=0x%04x\n",(unsigned int)MFRL_MAX,(unsigned int)MFRL_MAX,(unsigned int)MAXMFRL,(unsigned int)MAXMFRL);
 printf("LEADER=%d/MSNVSPLT=%d DIRSTRU=%d\n",LEADER,MSNVSPLT,sizeof(DIRSTRU));
 printf("ALLOMAXV=%u=0x%04x\n",(unsigned int)ALLOMAXV,(unsigned int)ALLOMAXV);
 printf("\n");
 printf("CNBSIZ=%d\n",(int)CNBSIZ);
 printf("N1BSIZ=%d\n",(int)N1BSIZ);
 printf("N2BSIZ=%d\n",(int)N2BSIZ);
 printf("L1BSIZ=%d\n",(int)L1BSIZ);
 printf("L2BSIZ=%d\n",(int)L2BSIZ);
 printf("INFX=%d\n",sizeof(INFX));
 printf("\n");
 printf("CNSTRU=%d\n",sizeof(CNSTRU));
 printf("N0STRU=%d\n",sizeof(N0STRU));
 printf("L0STRU=%d\n",sizeof(L0STRU));
 printf("M0STRU=%d\n",sizeof(M0STRU));
 printf("DIRSTRU=%d\n",sizeof(DIRSTRU));
 printf("M1STRU=%d\n",sizeof(M1STRU));
 printf("N1IDXE=%d\n",sizeof(N1IDXE));
 printf("N2IDXE=%d\n",sizeof(N2IDXE));
 printf("N1STRU=%d\n",sizeof(N1STRU));
 printf("N2STRU=%d\n",sizeof(N2STRU));
 printf("L1IDXE=%d\n",sizeof(L1IDXE));
 printf("L2IDXE=%d\n",sizeof(L2IDXE));
 printf("L1STRU=%d\n",sizeof(L1STRU));
 printf("L2STRU=%d\n",sizeof(L2STRU));
 printf("LZSTRU=%d\n",sizeof(LZSTRU));
 
 printf("B7HITBUF=%d\n",sizeof(B7HITBUF));
 printf("B7HITLIST=%d\n",sizeof(B7HITLIST));
 printf("B7HITLISTSH=%d\n",sizeof(B7HITLISTSH));


 printf("\n");
 printf("dbxstru=%d\n",sizeof(DBXSTRU));
 printf("recstru=%d\n",sizeof(RECSTRU));
 printf("trmstru=%d IFPBSIZ=%d\n",sizeof(TRMSTRU),IFBSIZ);
 
#if SAMEL
 printf("lxl1stru=%d/%d\n",sizeof(LXL1STRU),sizeof(LXL2STRU));
#endif
 exit(0);
}

#if 0
    if (treecase) {
        idxksiz=LE2;
        idxesiz=sizeof(N2IDXE);
        lbufp=(UCHR *)(&TRMl2buf);
        lidxesiz=sizeof(L2IDXE);
    }
    else {
        idxksiz=LE1;
        idxesiz=sizeof(N1IDXE);
        lbufp=(UCHR *)(&TRMl1buf);
        lidxesiz=sizeof(L1IDXE);
    }
#endif
