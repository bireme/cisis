/* crunchif - convert SunOS (or MPE?) inverted file to PC format
              AOT, 26/10/93

              Alteracao <target_dbn>
              AOT, 28/11/2001

*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "cisis.h"
#include "cirun.h"

#define INVTRACE        0
#define PCCTRACE        0
#define PCNTRACE        0
#define PCLTRACE        0

#define BUFSZX          0x4000 /* 16K */

#define NOSW_20 0
#define NOSW_21 1
#define NOSW_40 0
#define NOSW_41 1
#define NOSW_42 2
#define NOSW_43 3

#define SWAP_20 1
#define SWAP_21 0
#define SWAP_40 3
#define SWAP_41 2
#define SWAP_42 1
#define SWAP_43 0

char *movp,*movtmp;
UWORD moved;

int alignment;
int mv20,mv21,mv40,mv41,mv42,mv43;
int xalign;

#define mova(a)                            \
        if (a > 1) if ((xalign=moved%a) != 0) while (xalign--) { *movp++ = '\0'; moved++; }

#define mov2(source)                            \
        if (alignment >= 2) if ((xalign=moved%2) != 0) while (xalign--) { *movp++ = '\0'; moved++; } \
        movtmp  = (char *)&(source);            \
        *movp++ = movtmp[mv20];                \
        *movp++ = movtmp[mv21];                \
        moved+=2;
#define mov4(source)                            \
        if (alignment >= 4) if ((xalign=moved%4) != 0) while (xalign--) { *movp++ = '\0'; moved++; } \
        movtmp  = (char *)&(source);            \
        *movp++ = movtmp[mv40];                \
        *movp++ = movtmp[mv41];                \
        *movp++ = movtmp[mv42];                \
        *movp++ = movtmp[mv43];                \
        moved+=4;
#define movx(sourcep,len)            \
        memmove(movp,sourcep,(size_t)len);       \
        movp+=len;                              \
        moved+=len;


#if ANSI
void filcreat(char *gidbnp, char *namp, char *extp, int lrecl);
void putpcinv(UWORD reclen);
void endpcinv(void);
int filopen(char *filename);
int filclos(int fd);
#else
void filcreat();
void putpcinv();
void endpcinv();
int filopen();
int filclos();
#endif

int inv_fd=0;
UWORD inv_left=BUFSZX;
char *inv_buffer,*inv_record;
char inv_filename[BUFSIZ];


#if !LIND
#if ANSI
void cru_ifplist(int parmtrace, TRMSTRU *trmp, INFO info1, INFO info2, int fd);
void cru_posthdr1(int parmtrace, TRMSTRU *trmp, INFO info1, INFO info2, int fd);
LONGX cru_posting(int parmtrace, TRMSTRU *trmp, LONGX nord, int fd);
void cru_postread(int parmtrace, TRMSTRU *trmp, PUNT xblk, int fd);
void cru_cruifhdr(IFPHEAD *hdrp);
#else
void cru_ifplist();
void cru_posthdr1();
LONGX cru_posting();
void cru_postread();
void cru_cruifhdr();
#endif
#endif /* !LIND */

void main(argc,argv)
int argc;
char *argv[];
{
    char *dbnp,*pcdbnp;
#if !LIND
    DBXSTRU *dbxp;
    TRMSTRU *trmp;
    char cru_trmarea[sizeof(TRMSTRU)+IFBSIZ];
    TRMSTRU *cru_trmp=(TRMSTRU *)cru_trmarea;
#endif
    INVMAP *invp;
    N0STRU *n0p;
    N1IDXE *n1xp;
    N2IDXE *n2xp;
    L0STRU *l0p;
    L1IDXE *l1xp;
    L2IDXE *l2xp;
    UCHR leafarea[sizeof(L2STRU)];
    int treecase,loop;
    PUNT punt;
    char *p;
    LONGX count1=0,count2=0;
    int argnext=1;
    LONGX parmtell;
    int parmtrace=0;
#if !LIND
    int parmifpno=0;
    int parmifpif=0;
#endif

#define TARGET_pc     0
#define TARGET_linux  1
#define TARGET_hpux   2
#define TARGET_sun    3
#define TARGET_alpha  4
#define TARGET_vax    5
#define TARGET_unisys 6
#define TARGET_mpe    7
#define TARGET_cdc    8
                         /* 0    1       2       3     4       5     6        7     8   */
    char *target_id[]   = {"pc","linux","hpux","sun","alpha","vax","unisys","mpe","cdc",NULL};
    int target_align[]  = { 1,   4,      4,      4,    4,      1,    4,       4,    4,  0 };
    int target_swapped[]= { 1,   1,      0,      0,    1,      1,    1,       0,    0,  0 };
    int i;

#if PC
    int xtarget=TARGET_linux;
#else
    int xtarget=TARGET_pc;
#endif

    int xorigin=TARGET_alpha;
#if PC
    xorigin=TARGET_pc;
#endif
#if UNIX
#if INTELCPU
    xorigin=TARGET_linux;
#endif
#endif
#if BRME || BRMXCPU
    xorigin=TARGET_hpux;
#endif
#if SUN
    xorigin=TARGET_sun;
#endif
#if VAX
    xorigin=TARGET_vax;
#endif
#if UNISYSCPU
    xorigin=TARGET_unisys;
#endif
#if MPE
    xorigin=TARGET_mpe;
#endif
#if CDCS4320
    xorigin=TARGET_cds;
#endif


    if (argc == 2 && strcmp(argv[1],"what") == 0) {
        printf("%s",cicopyr("?Utility CRUNCHIF"));
        printf("\n");
        exit(0);
    }
    if (argc < 3) {
        printf("%s",cicopyr("Utility CRUNCHIF"));
        printf("\n");
        printf("crunchif <dbn> <target_dbn> [<option> [...]] \n");
        printf(" \n");
        printf("options: \n");
        printf("\n");
#if !LIND
        printf(" -ifp                     -> don't crunch %s file \n",ix1extp);
        printf(" /ifp                     -> crunch %s file if needed \n",ix1extp);
#endif
        printf(" tell=<n>                 -> tell <n> records processed \n");
        printf(" \n");
        printf(" target={%s",target_id[xtarget]);
        for (i=0; target_id[i]; i++)
          if (i != xtarget && i != xorigin) printf("|%s",target_id[i]);
        printf("|%s",target_id[xorigin]);
        printf("}  default: %s\n",target_id[xtarget]);
        printf(" \n");
        exit(1);
    }

    /* get positional parms */
    dbnp=argv[argnext++];
    pcdbnp=argv[argnext++];

    /* get optional parms */
    parmtell=0;
    for (; argnext < argc; ) {
        p=argv[argnext++];
        if (strncmp(p,"tell?",5) == 0 || strncmp(p,"tell=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&parmtell) != 1) fatal(p);
            continue;
        }
        if (strcmp(p,"trace?all") == 0|| strcmp(p,"trace=all") == 0) {
            parmtrace=dbxtrace=trmtrace=1;
            continue;
        }
        if (strcmp(p,"trace") == 0) {
            parmtrace=1;
            continue;
        }
#if !LIND
        if (strcmp(p,"-ifp") == 0) {
            parmifpno=1;
            continue;
        }
        if (strcmp(p,"/ifp") == 0) {
            parmifpif=1;
            continue;
        }
#endif
        if (strncmp(p,"target?",7) == 0 || strncmp(p,"target=",7) == 0) {
            int found=0;
            for (xtarget=0; target_id[xtarget]; xtarget++) {
                if (strcmp(p+7,target_id[xtarget])) continue;
                found=1; break;
            }
            if (!found) fatal(p);
            continue;
        }
        fatal(p);
    }

    /* setup processing */
    alignment=target_align[xtarget];

    mv20=NOSW_20;
    mv21=NOSW_21;
    mv40=NOSW_40;
    mv41=NOSW_41;
    mv42=NOSW_42;
    mv43=NOSW_43;
    if (target_swapped[xtarget] != target_swapped[xorigin]) {
        mv20=SWAP_20;
        mv21=SWAP_21;
        mv40=SWAP_40;
        mv41=SWAP_41;
        mv42=SWAP_42;
        mv43=SWAP_43;
    }

    /* allocate putpcinv() output buffer */
    inv_buffer=(char *)ALLOC((ALLOPARM)BUFSZX);
    if (inv_buffer == (char *)ALLONULL)
        fatal("ALLOC/inv_buffer");
    /* allocate processing buffer */
    inv_record=(char *)ALLOC((ALLOPARM)BUFSZX);
    if (inv_record == (char *)ALLONULL)
        fatal("ALLOC/inv_record");
    /* setup inverted file */
    invsetup(dbnp,0L,0L,0L);
    invp=dbxstorp(dbnp)->dbxifmap;
    /* convert .cnt */
    filcreat(NULL,pcdbnp,cx1extp,0);
    for (treecase=0; treecase < 2; treecase++) {
        movp=inv_record; moved=0;
        mov2(invp->cn[treecase].idtype);
        mov2(invp->cn[treecase].ordn);
        mov2(invp->cn[treecase].ordf);
        mov2(invp->cn[treecase].n);
        mov2(invp->cn[treecase].k);
        mov2(invp->cn[treecase].liv);
        mov4(invp->cn[treecase].posrx);
        mov4(invp->cn[treecase].nmaxpos);
        mov4(invp->cn[treecase].fmaxpos);
        mov2(invp->cn[treecase].abnormal);
        mova(alignment);
        putpcinv(moved);
    }
    endpcinv();
    /* convert .n0x */
    for (treecase=0; treecase < 2; treecase++) {
        filcreat(NULL,pcdbnp,nx12extp[treecase],0);
        for (punt=1L; punt <= invp->cn[treecase].nmaxpos; punt++) {
            n0p=noderead(invp,treecase,0,punt);
            movp=inv_record; moved=0;
            mov4(n0p->pos);
            mov2(n0p->ock);
            mov2(n0p->it);
            loop=TWORDN;
            if (treecase)
                for (n2xp=((N2STRU *)n0p)->idx; loop--; n2xp++) {
                    movx(n2xp->key,LE2);
                    mov4(n2xp->punt);
                    mova(alignment);
                }
            else
                for (n1xp=((N1STRU *)n0p)->idx; loop--; n1xp++) {
                    movx(n1xp->key,LE1);
                    mov4(n1xp->punt);
                    mova(alignment);
                }
            putpcinv(moved);
        }
        endpcinv();
    }
    /* convert .l0x */
    for (treecase=0; treecase < 2; treecase++) {
        filcreat(NULL,pcdbnp,lx12extp[treecase],0);
        for (punt=1L; punt <= invp->cn[treecase].fmaxpos; punt++) {
            l0p=leafread(leafarea,invp,treecase,punt,0);
            movp=inv_record; moved=0;
            mov4(l0p->pos);
            mov2(l0p->ock);
            mov2(l0p->it);
            mov4(l0p->ps);
#if LIND
            mov4(l0p->psb);
#endif
            loop=TWORDF;
            if (treecase)
                for (l2xp=((L2STRU *)l0p)->idx; loop--; l2xp++) {
                    movx(l2xp->key,LE2);
                    mov4(l2xp->info1);
                    mov4(l2xp->info2);
#if LIND
                    mov4(l2xp->info3info4.info3);
#endif
                    mova(alignment);
                }
            else
                for (l1xp=((L1STRU *)l0p)->idx; loop--; l1xp++) {
                    /*mova(0);*/
                    movx(l1xp->key,LE1);
                    mov4(l1xp->info1);
                    mov4(l1xp->info2);
#if LIND
                    mov4(l1xp->info3info4.info3);
#endif
                    mova(alignment);
                }
            putpcinv(moved);
        }
        endpcinv();
    }


#if !LIND /* ... */

    /* convert .ifp */

if (!parmifpno) {

  int genifp=1;
  if (parmifpif) {
    int ifpshort=4;
    if (ifpshort >= alignment)
        if (target_swapped[xtarget] == target_swapped[xorigin])
            genifp=0;
  }
  if (genifp) {

    /* copy .ifp blocks, converting .ifp block# and ifpctrl */
    filcreat(NULL,pcdbnp,ix1extp,0);
    TERM(0L,dbnp,"!"); dbxp=TRMdbxp;
#if CNLI
    if (LSEEK64(DBIifopn,invp->ci_offset,SEEK_SET) != invp->ci_offset) fatal("crunchif/ifp/seek0");
#else
    if (LSEEK64(DBIifopn,0L,SEEK_SET) != 0) fatal("crunchif/ifp/seek0");
#endif
    while (CIREAD(DBIifopn,TRMifbuf,IFBSIZ) == IFBSIZ) {
        IFPSTRU *ifp=(IFPSTRU *)TRMifbuf;
        movp=inv_record; moved=0;
        if (ifp->ifpblk == 1) {
            IFPCTRL *ifp=(IFPCTRL *)TRMifbuf;
            mov4(ifp->ifpblk);
            mov4(ifp->ifprec1);
            mov4(ifp->ifprec2);
            movx(TRMifbuf+4+4+4,IFBSIZ-4-4-4);
        }
        else {
            mov4(ifp->ifpblk);
            movx(TRMifbuf+4,IFBSIZ-4);
        }
        putpcinv(moved);
    }
    endpcinv();

    /* parse dictionary, converting ifp header and ifp segments */

    memset(cru_trmarea,0x00,sizeof(cru_trmarea));
    inv_fd=filopen(inv_filename);

    for (treecase=0; treecase < 2; treecase++) {
        PUNT punt;
        int level;
if (parmtrace) printf("+++treecase=%d liv=%"_LD_" posrx=%"_LD_"\n",treecase,(LONGX)invp->cn[treecase].liv,(LONGX)invp->cn[treecase].posrx);
        if (invp->cn[treecase].liv == NEGLIV) continue;

        punt=invp->cn[treecase].posrx;
        for (level=0; punt > 0; level++) {
            N0STRU *n0p;
if (parmtrace) printf("+++treecase=%d node=%"_LD_,treecase,(LONGX)punt);
            n0p=noderead(invp,treecase,level,punt);
            if (treecase) {
               N2STRU *n2p=(N2STRU *)n0p; punt=n2p->idx[0].punt;
            }
            else {
               N1STRU *n1p=(N1STRU *)n0p; punt=n1p->idx[0].punt;
            }
if (parmtrace) printf(" punt0=%"_LD_"\n",(LONGX)punt);
        }
        punt= -punt;

        while (punt > 0) {
            INFO info1;
            INFO info2;
            int i;
if (parmtrace) printf("+++treecase=%d leaf=%"_LD_,treecase,(LONGX)punt);
            l0p=leafread(leafarea,invp,treecase,punt,0);
if (parmtrace) printf(" ps=%"_LD_"\n",(LONGX)l0p->ps);

            loop=l0p->ock;
            for (i=0; i<loop; i++) {
                if (treecase) {
                    L2STRU *l2p=(L2STRU *)l0p;
                    info1=l2p->idx[i].info1;
                    info2=l2p->idx[i].info2;
if (parmtrace) printf("+++treecase=%d leaf=%"_LD_" key=%.30s info=%"_LD_"/%"_LD_"\n",treecase,(LONGX)punt,
      l2p->idx[i].key,(LONGX)info1,(LONGX)info2);
                }
                else {
                    L1STRU *l1p=(L1STRU *)l0p;
                    info1=l1p->idx[i].info1;
                    info2=l1p->idx[i].info2;
if (parmtrace) printf("+++treecase=%d leaf=%"_LD_" key=%.10s info=%"_LD_"/%"_LD_"\n",treecase,(LONGX)punt,
      l1p->idx[i].key,(LONGX)info1,(LONGX)info2);
                }

                cru_trmp->trmlcase=treecase;
                cru_ifplist(parmtrace,cru_trmp,info1,info2,inv_fd);
            }

            punt=l0p->ps;

        }
    }
    cru_postread(parmtrace,cru_trmp,0,inv_fd);
    filclos(inv_fd);

  }
  else {
    if (parmtrace || parmtell) printf("+++ %s%s not generated! \n",pcdbnp,ix1extp);
  } /* end if (genifp) */

} /* end if (!parmifpno) */

#endif /* !LIND */ /* ... */

    if (parmtell) fprintf(stderr,"+++ %"_LD_"/%"_LD_" \n",count1,count2);

    exit(0);
}

/* ----------------------------- pst.c ------------------------------ */

#if !LIND


void cru_ifplist(parmtrace,trmp,info1,info2,fd)
int parmtrace;
TRMSTRU *trmp;
INFO info1;
INFO info2;
int fd;
{

if (parmtrace) printf("+++cru_posthdr1 - info=%"_LD_"/%"_LD_"\n",(LONGX)info1,(LONGX)info2);

    cru_posthdr1(parmtrace,trmp,info1,info2,fd);

    do {
        ;
    } while (cru_posting(parmtrace,trmp,TRMpost+1,fd) > 0);

}

void cru_postread(parmtrace,trmp,xblk,fd)
int parmtrace;
TRMSTRU *trmp;
PUNT xblk;          /* registro desejado */
int fd;
{
    IFPSTRU *xp;
    off_t sbyte;
    LONGX cmpblkno;

    xp=(IFPSTRU *)TRMifbuf;

    if (xp->ifpblk) {
        LONGX blkno;
        movp=(char *)&blkno; mov4(xp->ifpblk);
	    sbyte=(blkno-1L)<<IFSHIFT;
	    if (LSEEK64(fd,sbyte,SEEK_SET) != sbyte) fatal("cru_postread/lseek/w");
        if (CIWRITE(fd,TRMifbuf,IFBSIZ) != IFBSIZ) fatal("cru_postread/write");
    }

    if (xblk) {
	    sbyte=(xblk-1L)<<IFSHIFT;
if (parmtrace) printf("+++cru_postread - xblk=%"_LD_" sbyte=%"P_OFF_T" \n",(LONGX)xblk,(LONG_LONG)sbyte);
	    if (LSEEK64(fd,sbyte,SEEK_SET) != sbyte) fatal("cru_postread/lseek/r");
        if (CIREAD(fd,TRMifbuf,IFBSIZ) != IFBSIZ) fatal("cru_postread/read");
        movp=(char *)&cmpblkno; mov4(xblk);
        if (xp->ifpblk != cmpblkno) fatal("cru_postread/check/xblk");
    }
}

void cru_posthdr1(parmtrace,trmp,info1,info2,fd)
int parmtrace;
TRMSTRU *trmp;      /* elemento de vtrmp, apos nx/term(), com IFBSIZ */
INFO info1;
INFO info2;
int fd;
{
    UCHR *p,*inp;
    int n;
    INFO *hdrp,*segp,*hp;
    LONGX cmpblkno;

    TRMpleft=0;
    TRMpcblk=0; TRMpcpos=IFMAXTIV;

    inp=TRMifbuf;
    movp=(char *)&cmpblkno; mov4(info1);
    if (*(INFO *)(inp) != cmpblkno) /* *(INFO *)(inp) != info1 */
        cru_postread(parmtrace,trmp,info1,fd);

    p=inp+INFOSIZE+(info2*INFOSIZE);  /* INFOSIZE = ifpblk */
    hdrp=(INFO *)&TRMifhdr;
    segp=(INFO *)&TRMifseg;
    for (hp=(INFO *)(p), n=IFHDRINFOS; n--; ) *hdrp++= *segp++= *hp++;

    TRMpcblk=info1; TRMpcpos=info2+IFHDRINFOS;
    TRMpost=0; TRMpleft=TRMifhdr.ifpsegp;
if (parmtrace) printf("+++cru_posthdr1 - post=%"_LD_" pcblk=%"_LD_" pcpos=%"_LD_" totp=%"_LD_" \n",TRMpost,TRMpcblk,TRMpcpos,TRMpleft);

    cru_cruifhdr((IFPHEAD *)p);
}

void cru_cruifhdr(hdrp)
IFPHEAD *hdrp;
{
    char hdrarea[sizeof(IFPHEAD)];

    movp=(char *)hdrarea;

    mov4(hdrp->ifpnxtb);           /* gdb .ifp ifpnxtb */
    mov4(hdrp->ifpnxtp);           /* gdb .ifp ifpnxtp */
    mov4(hdrp->ifptotp);           /* gdb .ifp ifptotp */
    mov4(hdrp->ifpsegp);           /* gdb .ifp ifpsegp */
    mov4(hdrp->ifpsegc);           /* gdb .ifp ifpsegc */

    memmove((char *)hdrp,hdrarea,sizeof(IFPHEAD));
}

LONGX cru_posting(parmtrace,trmp,nord,fd)
int parmtrace;
TRMSTRU *trmp;      /* elemento de vtrmp, apos nx/term(), com IFBSIZ */
LONGX nord;          /* numero de ordem do posting desejado ou 0L */
int fd;
{
    LONGX total;
    INFO info1,info2,*hp,*sp;
    int n;
    UCHR *p,*inp;
    LONGX cmpblkno;

    total=TRMifhdr.ifptotp;
    if (nord == 0) return(total);
    if (nord > total) {
	    TRMpost=0;
        TRMpleft=0;
	    return(-1L);
    }

    TRMpost++;
if (parmtrace) printf("+++cru_posting1 - post=%"_LD_" pcblk=%"_LD_" pcpos=%"_LD_" left=%"_LD_"\n",TRMpost,TRMpcblk,TRMpcpos,TRMpleft);
    if (nord != TRMpost) fatal("cru_posting/nord");

    while (!TRMpleft) {
	  info1=TRMifseg.ifpnxtb;
	  if (!info1) fatal("cru_posting/ifpnxtb");
	  info2=TRMifseg.ifpnxtp;
	  inp=TRMifbuf;
      movp=(char *)&cmpblkno; mov4(info1);
	  if (*(PUNT *)(inp) != cmpblkno) /* *(PUNT *)(inp) != info1 */
        cru_postread(parmtrace,trmp,info1,fd);                  /* v3.3 */
	  p=inp+INFOSIZE+(info2*INFOSIZE);      /* INFOSIZE = ifpblk */
	  for (sp=(INFO *)&TRMifseg, hp=(INFO *)p, n=IFHDRINFOS; n--; )
	    *sp++= *hp++;

	  TRMpleft=TRMifseg.ifpsegp;
	  TRMpcblk=info1;
	  TRMpcpos=info2+IFHDRINFOS;
if (parmtrace) printf("+++cru_posting2 - post=%"_LD_" pcblk=%"_LD_" pcpos=%"_LD_" left=%"_LD_" \n",TRMpost,TRMpcblk,TRMpcpos,TRMpleft);

      cru_cruifhdr((IFPHEAD *)p);
    }

    if (TRMpcpos >= (IFMAXTIV-1)) {
	  info1=TRMpcblk+1; info2=0;              /* 22/02/89 */
      cru_postread(parmtrace,trmp,info1,fd);                  /* v3.3 */
	  TRMpcblk=info1;
	  TRMpcpos=info2;                         /* 22/02/89 */
if (parmtrace) printf("+++cru_posting3 - post=%"_LD_" pcblk=%"_LD_" pcpos=%"_LD_" \n",TRMpost,TRMpcblk,TRMpcpos);
    }

    /* postpost(trmp); */
    TRMpleft--;
    TRMpcpos+=IFPSTINFOS;
    return(1); /* TRMpmfn */
}

#endif /* !LIND */



/* ------------------ xxxpcinv() and filxxxx() ----------------------------- */

void putpcinv(reclen)
UWORD reclen;
{
    char *p;
    for (p=inv_record; reclen; ) {
        if (reclen <= inv_left) {
            memmove(&inv_buffer[BUFSZX-inv_left],p,(size_t)reclen);
            inv_left-=reclen;
            break;
        }
        memmove(&inv_buffer[BUFSZX-inv_left],p,inv_left);
        if (CIWRITE(inv_fd,inv_buffer,BUFSZX) < BUFSZX) fatal("putpcinv/write");
        reclen-=inv_left;
        p+=inv_left;
        inv_left=BUFSZX;
    }
}
void endpcinv()
{
    int n;
    if (inv_left < BUFSZX) {
        n=BUFSZX-inv_left;
        if (CIWRITE(inv_fd,inv_buffer,(unsigned)n) < n) fatal("endpcinv/write");
    }
    inv_left=BUFSZX;
    if (inv_fd) {
        CLOSE(inv_fd);
        inv_fd=0;
    }
}

void filcreat(gidbnp,namp,extp,lrecl)
char *gidbnp;
char *namp;
char *extp;
int lrecl;
{
#if MPE
    char dcb[80];
#endif
    strcpy(inv_filename,namp);
    strcat(inv_filename,extp);
#if CIPAR
    strcpy(inv_filename,dbxcipar(gidbnp,inv_filename,'='));
#endif
#if MPE
    sprintf(dcb,"b R%d",lrecl);
    inv_fd=OPEN(inv_filename,O_WRONLY|O_CREAT|O_TRUNC|O_MPEOPTS,PERMIS,dcb);
#else
    inv_fd=CREAT(inv_filename,PERMIS);
    if (lrecl) fatal("filcreat/lrecl");
#endif
    if (inv_fd <= 0) fatal(inv_filename);
}

int filopen(filename)
char *filename;
{
    int fd;
    fd=OPEN(filename,O_BINARY|O_RDWR);
    if (fd <= 0) fatal(filename);
    return(fd);
}
int filclos(fd)
int fd;
{
    if (fd) CLOSE(fd);
    return(0);
}


