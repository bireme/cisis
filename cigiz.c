#include <string.h>
#if !CICPP
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "cisis.h"

#endif /* CICPP */

/* ----------------------------- giz.c ------------------------------ */

#if RECGIZM

#define TRACER0         00
#define TRACEG1         00
#define TRACEG2         00
#define TRACEG3         00
#define TRACEGT         00
#if TRACEGT
#define GETCHAR printf("\n");
#else
#define GETCHAR getchar()
#endif


/* procedures */

#if CICPP
GIZMSTRU *gizmalloc(int n)
#else /* CICPP */
GIZMSTRU *gizmalloc(n)                                  /*
-------------------                                     */
int n;
#endif /* CICPP */
{
    GIZMSTRU *gizmp;

#if CICPP
    try { gizmp = (GIZMSTRU *)new char [n]; }
    catch (BAD_ALLOC)
    { gizmp = (GIZMSTRU *)ALLONULL; }
#else /* CICPP */
    gizmp=(GIZMSTRU *)ALLOC((ALLOPARM)n);
#endif /* CICPP */

    if (gizmp == (GIZMSTRU *)ALLONULL)
#if CICPP
	(new CISISX())->xfatal("gizmalloc/ALLOC/GIZMSTRU");
#else
        fatal("gizmalloc/ALLOC/GIZMSTRU");
#endif

    memset((unsigned char *)gizmp,'\0',(int)n);

    return(gizmp);
}


#if CICPP
unsigned char *pattalloc(int n)
#else /* CICPP */
unsigned char *pattalloc(n)                                     /*
------------------------                                        */
int n;
#endif /* CICPP */
{
    unsigned char *p;

    if (n) {
#if CICPP
        try { p = new unsigned char [n]; }
        catch (BAD_ALLOC)
        { p = (unsigned char *)ALLONULL; }
#else /* CICPP */
        p=(unsigned char *)ALLOC((ALLOPARM)n);
#endif /* CICPP */

        if (p == (unsigned char *)ALLONULL)
#if CICPP
            (new CISISX())->xfatal("pattalloc/ALLOC/patt");
#else
            fatal("pattalloc/ALLOC/patt");
#endif
    }
    else p=NULL;

    return(p);
}




#if CICPP
#define CORELEFT() 0
#endif /* CICPP */

#if CICPP
int gizmread(CISISX * cisisxp,
             char *gizdbnp,
             VGIZPSTRU **vgizmpp)
#else /* CICPP */
int gizmread(gizdbnp, vgizmpp, irec)                          /*
------------                                                  */

char *gizdbnp;
VGIZPSTRU **vgizmpp;
LONGX irec;
#endif /* CICPP */

{

    unsigned char gfld11[4];
    unsigned char gfld21[4];

#if BEFORECD
    LONGX irec;
#endif

    RECSTRU *recp;

#if CICPP
    LONGX irec=-1;
    recp=new RECSTRU(cisisxp);
#endif /* CICPP */

    VGIZPSTRU *vgizmap;
    GIZMSTRU *gizmhp,*gizmp,*gizmbp,*gizmnp;

    short igiz,fld1len,fld2len;
    unsigned char *fld1p,*fld2p,*p,*p2;

    int xdir,n,i;
    LONGX nbytes,mfn,ubytes;

    unsigned char *q,*cq,c;
    VGIZPSTRU *gmap;
    LONGX xtag1,xtag2;


    if (rectrace) {
        printf("gizmread - %s=%p [%"_LD_"]\n",gizdbnp,vgizmpp,irec);
        ubytes=CORELEFT();
    }

    /*
        aloca e inicializa gizmo
    */
    nbytes=sizeof(VGIZPSTRU);

#if CICPP
    try { vgizmap = (VGIZPSTRU *) new char[nbytes]; }
    catch (BAD_ALLOC)
    { vgizmap = (VGIZPSTRU *)ALLONULL; }
#else /* CICPP */
    vgizmap=(VGIZPSTRU *)ALLOC((ALLOPARM)nbytes);
#endif /* CICPP */
    if (vgizmap == (VGIZPSTRU *)ALLONULL)
        fatal("gizmread/ALLOC");
    memset(vgizmap,0x00,(unsigned)nbytes);
    *vgizmpp=vgizmap;
    for (igiz=0; igiz < 256; igiz++) {
        vgizmap->ghdrp[igiz] = NULL;
    }
    vgizmap->grngs=0;
    vgizmap->gdbnp=pattalloc((n=strlen(gizdbnp))+1);
    memmove(vgizmap->gdbnp,gizdbnp,n); vgizmap->gdbnp[n]='\0';

    if (rectrace)
        printf("gizmread - %s=%p=%p \n",gizdbnp,*vgizmpp,&vgizmap->nextp);

#if BEFORECD
    /* alocate working record */
    for (irec=maxnrec-1; irec--; ) {
        if (!vrecp[irec])
            break;
    }
    if (irec == 0)
        fatal("gizmread/irec");
    recallok(irec,MAXMFRL);
#endif

    /* setup tag list */
    if ((cq=(unsigned char *)strchr(gizdbnp+1,c=',')) == NULL) {
        c='\0'; q=(unsigned char *)"";           /* no tag list */
    }
    else {
        *cq='\0'; q=cq+1;       /* ends dbn and point to tag list */
    }
    gmap=vgizmap;


    /* carrega conteudo, segundo o primeiro char, e ordenado por tamanho */

#if CICPP
    for (mfn=1L; ; mfn++) {
        recp->xrecord(gizdbnp,mfn);
        if (RECrc == RCEOF) break;
#else /* CICPP */
    for (mfn=1L; (RECORD(irec,gizdbnp,mfn))->recrc != RCEOF; mfn++) {
#endif /* CICPP */

        if (RECrc != RCNORMAL)
            continue;
        if (MFRnvf == 0)
            continue;

#if CICPP
        if ((n=recp->xnocc(GIZMTAG11)) > 0) {
#else /* CICPP */
        if ((n=nocc(irec,GIZMTAG11)) > 0) {
#endif /* CICPP */
           if (n != 1) {
                if (rectrace) fprintf(stderr,"mfn=%"_LD_"\n",mfn);
                fatal("gizmread/GIZMTAG11/nocc");
           }
#if CICPP
           xdir=recp->xfieldx(GIZMTAG11,1);
#else /* CICPP */
           xdir=fieldx(irec,GIZMTAG11,1);
#endif /* CICPP */
           if (DIRlen(xdir) > 3) {
                if (rectrace) fprintf(stderr,"mfn=%"_LD_"\n",mfn);
                fatal("gizmread/GIZMTAG11/3");
           }


           strncpy((char *)gfld11,FIELDP(xdir),DIRlen(xdir));
           gfld11[DIRlen(xdir)]='\0';
           if (strcmp((CONST char *)gfld11,"hex") != 0 && strcmp((CONST char *)gfld11,"asc") != 0) {
                if (rectrace) fprintf(stderr,"mfn=%"_LD_"\n",mfn);
                fatal("gizmread/GIZMTAG11/xxx");
            }
        }
        else
            *gfld11='\0';

#if CICPP
        if ((n=recp->xnocc(GIZMTAG21)) > 0) {
#else /* CICPP */
        if ((n=nocc(irec,GIZMTAG21)) > 0) {
#endif /* CICPP */
            if (n != 1) {
                if (rectrace) fprintf(stderr,"mfn=%"_LD_"\n",mfn);
                fatal("gizmread/GIZMTAG21/nocc");
            }
#if CICPP
           xdir=recp->xfieldx(GIZMTAG21,1);
#else /* CICPP */
           xdir=fieldx(irec,GIZMTAG21,1);
#endif /* CICPP */
           if (DIRlen(xdir) > 3) {
                if (rectrace) fprintf(stderr,"mfn=%"_LD_"\n",mfn);
                fatal("gizmread/GIZMTAG21/3");
           }

           strncpy((char *)gfld21,FIELDP(xdir),DIRlen(xdir));
           gfld21[DIRlen(xdir)]='\0';
           if (strcmp((CONST char *)gfld21,"hex") != 0 && strcmp((CONST char *)gfld21,"asc") != 0) {
                if (rectrace) fprintf(stderr,"mfn=%"_LD_"\n",mfn);
                fatal("gizmread/GIZMTAG21/xxx");
           }
        }
        else
            *gfld21='\0';

#if CICPP
        if (recp->xnocc(GIZMTAG1) != 1) {
#else /* CICPP */
        if (nocc(irec,GIZMTAG1) != 1) {
#endif /* CICPP */
            if (rectrace) fprintf(stderr,"mfn=%"_LD_"\n",mfn);
            fatal("gizmread/GIZMTAG1/nocc");
        }
#if CICPP
        xdir=recp->xfieldx(GIZMTAG1,1);
#else /* CICPP */
        xdir=fieldx(irec,GIZMTAG1,1);
#endif /* CICPP */

        switch (*gfld11) {
        case '\0':
            if ((fld1len=DIRlen(xdir)) < 1) {
                if (rectrace) fprintf(stderr,"mfn=%"_LD_"\n",mfn);
                fatal("gizmread/GIZMTAG1/len");
            }
            fld1p=(unsigned char *)FIELDP(xdir);
            break;
        case 'h':
            if ((fld1len=DIRlen(xdir)) < 2  || fld1len & 0x01) {
                if (rectrace) fprintf(stderr,"mfn=%"_LD_"\n",mfn);
                fatal("gizmread/GIZMTAG1/hexlen");
            }
            fld1p=(unsigned char *)FIELDP(xdir);
            for (p=fld1p, p2=p, n=fld1len; n; n-=2, p2+=2) {
                if (sscanf((CONST char *)p2,"%2x",&i) != 1) {
                    if (rectrace) fprintf(stderr,"mfn=%"_LD_"\n",mfn);
                    fatal("gizmread/GIZMTAG1/hex");
                }
                *p++ = i;
            }
            fld1len/=2;
            break;
        case 'a':
            if ((fld1len=DIRlen(xdir)) < 3  || (fld1len/3*3 != fld1len)) {
                if (rectrace) fprintf(stderr,"mfn=%"_LD_"\n",mfn);
                fatal("gizmread/GIZMTAG1/asclen");
            }
            fld1p=(unsigned char *)FIELDP(xdir);
            for (p=fld1p, p2=p, n=fld1len; n; n-=3, p2+=3) {
                if (sscanf((CONST char *)p2,"%3d",&i) != 1) {
                    if (rectrace) fprintf(stderr,"mfn=%"_LD_"\n",mfn);
                    fatal("gizmread/GIZMTAG1/asc");
                }
                *p++ = i;
            }
            fld1len/=3;
            break;
        default: fatal("gizmread/GIZMTAG1/switch");
        }

        fld2p=NULL; fld2len=0;

#if CICPP
        if ((n=recp->xnocc(GIZMTAG2)) != 1)
#else /* CICPP */
        if ((n=nocc(irec,GIZMTAG2)) != 1)
#endif /* CICPP */
            if (n) {
                if (rectrace) fprintf(stderr,"mfn=%"_LD_"\n",mfn);
                fatal("gizmread/GIZMTAG2/nocc");
            }
        if (n > 0) {
#if CICPP
            xdir=recp->xfieldx(GIZMTAG2,1);
#else /* CICPP */
            xdir=fieldx(irec,GIZMTAG2,1);
#endif /* CICPP */

            switch (*gfld21) {
            case '\0':
                if ((fld2len=DIRlen(xdir)) < 1) {
                    if (rectrace) fprintf(stderr,"mfn=%"_LD_"\n",mfn);
                    fatal("gizmread/GIZMTAG2/len");
                }
                fld2p=(unsigned char *)FIELDP(xdir);
                break;
            case 'h':
                if ((fld2len=DIRlen(xdir)) < 2  || fld2len & 0x01) {
                    if (rectrace) fprintf(stderr,"mfn=%"_LD_"\n",mfn);
                    fatal("gizmread/GIZMTAG2/hexlen");
                }
                fld2p=(unsigned char *)FIELDP(xdir);
                for (p=fld2p, p2=p, n=fld2len; n; n-=2, p2+=2) {
                    if (sscanf((CONST char *)p2,"%2x",&i) != 1) {
                        if (rectrace) fprintf(stderr,"mfn=%"_LD_"\n",mfn);
                        fatal("gizmread/GIZMTAG2/hex");
                    }
                    *p++ = i;
                }
                fld2len/=2;
                break;
            case 'a':
                if ((fld2len=DIRlen(xdir)) < 3  || (fld2len/3*3 != fld2len)) {
                    if (rectrace) fprintf(stderr,"mfn=%"_LD_"\n",mfn);
                    fatal("gizmread/GIZMTAG2/asclen");
                }
                fld2p=(unsigned char *)FIELDP(xdir);
                for (p=fld2p, p2=p, n=fld2len; n; n-=3, p2+=3) {
                    if (sscanf((CONST char *)p2,"%3d",&i) != 1) {
                        if (rectrace) fprintf(stderr,"mfn=%"_LD_"\n",mfn);
                        fatal("gizmread/GIZMTAG2/asc");
                    }
                    *p++ = i;
                }
                fld2len/=3;
                break;
            default: fatal("gizmread/GIZMTAG2/switch");
            }
        }

        gizmhp=vgizmap->ghdrp[*fld1p]; gizmnp=gizmhp;

        for (gizmbp=NULL; gizmnp; gizmbp=gizmnp, gizmnp=gizmnp->nextp)
            if (fld1len > gizmnp->isize)
                break;

        gizmp=gizmalloc(sizeof(GIZMSTRU));
        gizmp->isize=fld1len;
        gizmp->osize=fld2len;
        gizmp->ipatt=pattalloc(fld1len); memmove(gizmp->ipatt,fld1p,fld1len);
        gizmp->opatt=pattalloc(fld2len); memmove(gizmp->opatt,fld2p,fld2len);
        gizmp->nextp=gizmnp;

        if (gizmbp)
            gizmbp->nextp=gizmp;
        else
            vgizmap->ghdrp[*fld1p]=gizmp;
    }


#if TRACER0
for (i=0; i<=255; i++) {
 gizmhp=vgizmap->ghdrp[i];
 if (!gizmhp)
     continue;
 printf("char1=%c\n",i);
 for (gizmp=gizmhp; gizmp; gizmp=gizmp->nextp) {
  printf("from/%d=",gizmp->isize);
  for (p=gizmp->ipatt, n=gizmp->isize; n--; p++)
   printf("%c(%02x)",*p,*p);
  putchar('\n');
  printf("  to/%d=",gizmp->osize);
  for (p=gizmp->opatt, n=gizmp->osize; n--; p++)
   printf("%c(%02x)",*p,*p);
  putchar('\n');
 }
 printf(".."); GETCHAR;
}
#endif



    /* close input data base */

    dbxflush(gizdbnp);          /* close .mst/.xrf and free input buffers */

#if CICPP
    delete recp;
#endif /* CICPP */

    if (rectrace) {
        ubytes-=CORELEFT();
        printf("gizmread - %s=%"_LD_" bytes\n",(*vgizmpp)->gdbnp,ubytes);
    }


    /* process tag list */

            while (*q) {
		if (gmap->grngs >= MAXGIZR) fatal((char *)p);
                if (sscanf((CONST char *)q,"%"_LD_"/%"_LD_,&xtag1,&xtag2) == 2) {
		    if (xtag1 < 1 || xtag2 > USHRT_MAX) fatal((char *)p);
		    if (xtag1 > xtag2) fatal((char *)p);
		    while (*q != '/') q++; q++;
		    while (isdigit(*q)) q++;
		    gmap->grng1[gmap->grngs]=(UWORD)xtag1;
		    gmap->grng2[gmap->grngs]=(UWORD)xtag2;
		    if (*q == ',') if (*(q+1)) q++;
		    gmap->grngs++;
		    continue;
		}
                if (sscanf((CONST char *)q,"%"_LD_,&xtag1) == 1) {
		    while (isdigit(*q)) q++;
		    if (xtag1 > USHRT_MAX) fatal((char *)p);
		    gmap->grng1[gmap->grngs]=(UWORD)xtag1;
		    gmap->grng2[gmap->grngs]=(UWORD)xtag1;
		    if (*q == ',') if (*(q+1)) q++;
		    gmap->grngs++;
		    continue;
		}
		fatal((char *)p);
	    }

    if (c) *cq=c;


    return(0);
}



#if CICPP
LONGX recgizmo(CISISX *cisisxp,
              RECSTRU *recp,
              VGIZPSTRU *vgizmap)
#else /* CICPP */
LONGX recgizmo(irec,vgizmap)                                           /*
-------------
                    abenda se vrecp nao inicializado;
                    abenda se registro irec nao alocado;
                    abenda se registro irec invalido;
                    processa irec segundo vgizmap;
                    retorna o numero de matchs
                                                                      */

LONGX irec;          /* indice de vrecp, para info/store */
VGIZPSTRU *vgizmap; /* estrutura gizmo a considerar */
#endif /* CICPP */

{
#if CICPP
    LONGX irec=0;
#else /* CICPP */
    RECSTRU *recp;
#endif /* CICPP */

    GIZMSTRU *gizmhp,*gizmp;

    short match;
    unsigned char *p,*p1,*p2;
    unsigned char *buffup,*batchp,*fuplenp;
    LONGX nbytes,nmatchs;
    int xdir,dirloop,fldloop,fuplen,n,irng,rngok;
    UWORD tag;


#if !CICPP
    if (rectrace)
        printf("recgizmo - irec=%"_LD_" %s=%p\n",irec,vgizmap->gdbnp,vgizmap);

    if (!nrecs)
        fatal("recgizmo/nrecs");

    recp=vrecp[irec];
#endif /* CICPP */

    if (!recp)
	fatal("recgizmo/irec");
#if 0
printf("+++ recgizmo: ");
prtleader(recp,0L);
prtfields(recp,0L);
#endif

    if (RECtype != TYPEMFR) {
        if (rectrace) printf("*** irec=%"_LD_"  RECtype=%d\n",irec,RECtype);
        fatal("recgizmo/check");
    }

    /* allocate buffer for fldupdat */
    nbytes=BUFFUPL;

#if CICPP
    try { buffup= new unsigned char [nbytes]; }
    catch (BAD_ALLOC)
    { buffup = (unsigned char *)ALLONULL; }
#else /* CICPP */
    buffup=(char *)ALLOC((ALLOPARM)nbytes);
#endif /* CICPP */
    if (buffup == (unsigned char *)ALLONULL)
        fatal("recgizmo/ALLOC");

    nmatchs=0;
    if (vgizmap->ghdrp == NULL) fatal("recgizmo/ghdrp");

    for (batchp=buffup, xdir=0, dirloop=MFRnvf; dirloop--; xdir++) {
        tag=DIRtag(xdir); rngok=1;
#if TRACEG1
printf("\\begin tag %d..",tag); GETCHAR;
#endif

        if (vgizmap->grngs)
            for (rngok=0, irng=0; irng < vgizmap->grngs; irng++) {
                if (tag < vgizmap->grng1[irng]) continue;
                if (tag > vgizmap->grng2[irng]) continue;
                rngok=1; break;
            }

        if (rngok) {
            sprintf((char *)batchp,"H%u 00000000 ",tag); batchp+=strlen((CONST char *)batchp);
            fuplenp=batchp-1-8; fuplen=0; /* fuplenp points to 00000000 */
#if TRACEG2
printf("\\buffup=%s",buffup); GETCHAR;
#endif
            for (p=(unsigned char *)FIELDP(xdir), fldloop=DIRlen(xdir); fldloop; ) {
#if TRACEG2
printf("\\fldloop=%d *p=%c",fldloop,*p); GETCHAR;
#endif
                gizmhp=vgizmap->ghdrp[*p];
                if (gizmhp) {
                    for (gizmp=gizmhp; gizmp; ) {
                        if ((n=gizmp->isize) > fldloop) {
#if TRACEG2
printf("\\need %d chars - continue",n); GETCHAR;
#endif
                            gizmp=gizmp->nextp;
                            match=0; /* for the last cell - next char */
                            continue;
                        }
                        for (match=1, p1=gizmp->ipatt, p2=p; n--; p1++, p2++)
                            if (*p1 != *p2) {
#if TRACEG2
printf("\\%c != %c - no match",*p1,*p2); GETCHAR;
#endif
                                match=0;
                                break;
                            }
#if TRACEG2
                            else {
printf("\\%c == %c - may match",*p1,*p2); GETCHAR;
                            }
#endif
                        if (match) {
#if TRACEG2
printf("\\match %d -> %d chars",gizmp->isize,gizmp->osize); GETCHAR;
#endif
                            p1=gizmp->opatt;
                            for (n=gizmp->osize; n--; )
                                *batchp++ = *p1++;
                            p+=(n=gizmp->isize);
                            fldloop-=n; fuplen+=gizmp->osize;
                            gizmp->nused++;
                            gizmp=NULL; /* break */
                            nmatchs++;
                        }
			else {
                            gizmp=gizmp->nextp;
#if TRACEG2
printf("\\next"); GETCHAR;
#endif
                        }
                    }
                    if (!match) {
#if TRACEG2
printf("\\no match - copying %c",*p); GETCHAR;
#endif
                        *batchp++ = *p++;
                        fldloop--; fuplen++;
                    }
                }
                else {
#if TRACEG2
printf("\\no gizmo - copying %c",*p); GETCHAR;
#endif
                    *batchp++ = *p++;
                    fldloop--; fuplen++;
                }
	    } /* end field chrs */

            sprintf((char *)fuplenp,"%08d",fuplen); *(fuplenp+8)=' ';

        }

        else {  /* just copy it */
            sprintf((char *)batchp,"H%u %u ",tag,DIRlen(xdir)); batchp+=strlen((CONST char *)batchp);
            memmove(batchp,FIELDP(xdir),DIRlen(xdir)); batchp+=DIRlen(xdir);
        }

    } /* end dir entries */

    *batchp='\0';

    if (nmatchs) {

        MFRmfrl=MFRbase=LEADER; MFRnvf=0;

#if CICPP
	p = (unsigned char *)recp->xfldupdat((char *)buffup);
#else /* CICPP */
	p=fldupdat(irec,buffup);
#endif /* CICPP */

        if (p) {
            if (rectrace) {
                printf("***   buffup='%s'\n",buffup); /* */
                printf("*** fldupdat='%s'\n",p); /* */
                for (p=buffup; *p; p++) {printf("p=%02x=%c",*p,*p); if (getchar() != '\n') break;}
            }
            fatal("recgizmo/fldupdat");
        }

#if TRACEG3
    /* prtleader(recp,MFRmfn);
    prtfields(recp,MFRmfn); */
#endif
    }

    /* deallocate working area */
#if CICPP
    delete[] buffup;
#else /* CICPP */
    FREE(buffup);
#endif /* CICPP */

    if (rectrace)
        printf("recgizmo - %s %"_LD_" match(s)\n",vgizmap->gdbnp,nmatchs);

    return(nmatchs);
}


#undef GETCHAR

#endif /* RECGIZM */



#if RECXPND

#define TRACXG1 0
#define TRACXG3 0

#define TRACYG1 0
#define TRACYG3 0


#define SIZEOFTAG sizeof(DIRlen(0))


#if CICPP
LONGX recxpnmo(RECSTRU *recp)
#else /* CICPP */
LONGX recxpnmo(irec)                                                   /*
-------------
                    abenda se vrecp nao inicializado;
                    abenda se registro irec nao alocado;
                    abenda se registro irec invalido;
                    processa irec segundo vxpnmp;
                                                                      */

LONGX irec;          /* indice de vrecp, para info/store */
#endif /* CICPP */
{
#if !CICPP
    RECSTRU *recp;
#endif /* CICPP */

    unsigned char *buffup,*batchp,*fuplenp,*p;
    LONGX nmatchs,nbytes;
    int xdir,dirloop,fuplen;
    int minlen,len,tag,loop;


    if (!nrecs)
        fatal("recxpnmo/nrecs");

#if !CICPP
    recp=vrecp[irec];
#endif /* CICPP */
    if (!recp)
        fatal("recxpnmo/irec");

#if TRACXG1
 printf("recxpnmo - irec: %"_LD_" RDBxpn01,02: %d,%d\n",irec,RDBxpn01,RDBxpn02);
#endif

    if (RECtype != TYPEMFR) {
        if (rectrace) printf("*** irec=%"_LD_"  RECtype=%d\n",irec,RECtype);
        fatal("recxpnmo/check");
    }

    /* allocate buffer for fldupdat */
    nbytes=BUFFUPL;
#if CICPP
    try { buffup= new char [nbytes]; }
    catch (BAD_ALLOC)
    { buffup = (char *)ALLONULL); }
#else /* CICPP */
    buffup=(char *)ALLOC((ALLOPARM)nbytes);
#endif /* CICPP */
    if (buffup == (char *)ALLONULL)
        fatal("recxpnmo/ALLOC");

    nmatchs=0; batchp=buffup; minlen=SIZEOFTAG+0+1;
    sprintf(batchp,"D%d",RDBxpn01); batchp+=strlen(batchp);
    for (xdir=0, dirloop=MFRnvf; dirloop--; xdir++) {
        if (DIRtag(xdir) != RDBxpn01)
            continue;
        for (p=FIELDP(xdir), len=DIRlen(xdir); len >= minlen; ) {
#if SWAPPED
            p+=SIZEOFTAG;
            for (tag= *--p, len--, loop=SIZEOFTAG-1; loop--; ) { /* swapped */
                tag*=256; tag+=(*--p); len--;
            }
            p+=SIZEOFTAG;
#else
            for (tag= *p++, len--, loop=SIZEOFTAG-1; loop--; ) { /* normal */
                tag*=256; tag+=(*p++); len--;
            }
#endif
            sprintf(batchp,"H%d 000000 ",tag); batchp+=strlen(batchp);
            fuplenp=batchp-1-6; fuplen=0; /* fuplenp points to 000000 */
            for (; *p != RDBxpn02; len--, fuplen++) {
                if (len <= 0) fatal("recxpnmo/len/move");
                *batchp++ = *p++;
            }
            sprintf(fuplenp,"%06d",fuplen); *(fuplenp+6)=' ';
            p++; len--;
#if 0
*batchp='\0';
printf("len=%d buffup=%s\n",len,buffup);
#endif
        }
        if (len != 0) fatal("recxpnmo/len/end");
    }
    *batchp=NULL;

    MFRmfrl=MFRbase=LEADER;     MFRnvf=0;

#if CICPP
    p= recp->xfldupdat(buffup);
#else
    p=fldupdat(irec,buffup);
#endif /* CICPP */

    if (p) {
        /* printf("*** fldupdat='%s'\n",p); */
        fatal("recxpnmo/fldupdat");
    }

#if TRACXG3
    prtleader(recp,MFRmfn);
    prtfields(recp,MFRmfn);
#endif

    /* deallocate working area */
#if CICPP
    delete[] buffup;
#else
    FREE(buffup);
#endif /* CICPP */

    return(nmatchs);
}

#if CICPP
LONGX recxpn00 (RECSTRU *recp,
               int xpn01,
               int xpn02)
#else /* CICPP */
LONGX recxpn00(irec,xpn01,xpn02)                                       /*
-------------
                    abenda se vrecp nao inicializado;
                    abenda se registro irec nao alocado;
                    abenda se registro irec invalido;
                    processa irec segundo vxpnmp;
                                                                      */

LONGX irec;          /* indice de vrecp, para info/store */
int xpn01;          /* tag */
int xpn02;          /* delimiter */
#endif /* CICPP */
{

#if !CICPP
    RECSTRU *recp;
#endif /* CICPP */

    unsigned char *buffup,*batchp,*fuplenp,*p;
    LONGX nmatchs,nbytes;
    int xdir,dirloop,fuplen;


    if (!nrecs)
        fatal("recxpn00/nrecs");

#if !CICPP
    recp=vrecp[irec];
#endif /* CICPP */

    if (!recp)
        fatal("recxpn00/irec");

#if TRACYG1
 printf("recxpn00 - irec: %"_LD_" xpn01,02: %d,%d  SIZEOFTAG=%d\n",
  irec,xpn01,xpn02,SIZEOFTAG);
#endif

    if (RECtype != TYPEMFR) {
        if (rectrace) printf("*** irec=%"_LD_"  RECtype=%d\n",irec,RECtype);
        fatal("recxpn00/check");
    }

    /* allocate buffer for fldupdat */
    nbytes=BUFFUPL;

#if CICPP
    try { buffup = new char [nbytes]; }
    catch (BAD_ALLOC)
    { buffup = (char *)ALLONULL; }

#else /* CICPP */
    buffup=(char *)ALLOC((ALLOPARM)nbytes);
#endif /* CICPP */
    if (buffup = (char *)ALLONULL)
        fatal("recxpn00/ALLOC");

    nmatchs=0; batchp=buffup;
    sprintf(batchp,"H%d 000000 ",xpn01); batchp+=strlen(batchp);
    fuplenp=batchp-1-6; fuplen=0; /* fuplenp points to 000000 */
    for (xdir=0, dirloop=MFRnvf; dirloop--; xdir++) {
        memmove(batchp,(char *)&DIRtag(xdir),SIZEOFTAG);
        batchp+=SIZEOFTAG;
        fuplen+=SIZEOFTAG;
        memmove(batchp,FIELDP(xdir),DIRlen(xdir));
        batchp+=DIRlen(xdir);
        fuplen+=DIRlen(xdir);
        *batchp++ = xpn02;
        fuplen++;
    }
    *batchp=NULL;
    sprintf(fuplenp,"%06d",fuplen); *(fuplenp+6)=' ';

    if (fuplen) {
        MFRmfrl=MFRbase=LEADER; MFRnvf=0;

#if CICPP
        p = recp->xfldupdat(buffup);
#else
        p=fldupdat(irec,buffup);
#endif /* CICPP */

        if (p) {
            /* printf("*** fldupdat='%s'\n",p); */
            fatal("recxpn00/fldupdat");
        }
    }

#if TRAXYG3
    prtleader(recp,MFRmfn);
    prtfields(recp,MFRmfn);
#endif

    /* deallocate working area */
#if CICPP
    delete[] buffup;
#else /* CICPP */
    FREE(buffup);
#endif /* CICPP */

    return(nmatchs);
}


#undef SIZEOFTAG

#endif /* RECXPND */



/* ----------------------------- dec.c ------------------------------ */

#if RECDECO

/* cidec.h */

#if CICPP
#define R2RECORD(r,dbn,mfn) \
        (R2recp=r)->xrecord(dbn,mfn)

#define R2RECnbytes   R2recp->recnbytes
#define R2RECtype     R2recp->rectype
#define R2RECdbxp     R2recp->recdbxp
#define R2RECrc       R2recp->recrc

#define R2MFRmfn      R2recp->recmfp->m1.m1mfn
#define R2MFRmfrl     R2recp->recmfp->m1.m1mfrl
#define R2MFRmfbwb    R2recp->recmfp->m1.m1mfbwb
#define R2MFRmfbwp    R2recp->recmfp->m1.m1mfbwp
#define R2MFRbase     R2recp->recmfp->m1.m1base
#define R2MFRnvf      R2recp->recmfp->m1.m1nvf
#define R2MFRstatus   R2recp->recmfp->m1.m1status
#define R2MFRdir      R2recp->recmfp->m1.m1dir

#define R2DIRtag(x)   R2recp->recmfp->m1.m1dir[x].tag
#define R2DIRpos(x)   R2recp->recmfp->m1.m1dir[x].pos
#define R2DIRlen(x)   R2recp->recmfp->m1.m1dir[x].len

#define R2FIELDP(x)   (R2recp->recmfp->mx)+(R2MFRbase+R2DIRpos(x))

#define R2RDBname     R2RECdbxp->dbxname
#else /* CICPP */
#define R2RECORD(r,dbn,mfn) \
        R2recp=vrecp[record(((r)?r:(r=nrecs)),dbn,mfn)]

#define R2RECnbytes   R2recp->recnbytes
#define R2RECtype     R2recp->rectype
#define R2RECdbxp     R2recp->recdbxp
#define R2RECrc       R2recp->recrc

#define R2MFRmfn      R2recp->recmf.m1.m1mfn
#define R2MFRmfrl     R2recp->recmf.m1.m1mfrl
#define R2MFRmfbwb    R2recp->recmf.m1.m1mfbwb
#define R2MFRmfbwp    R2recp->recmf.m1.m1mfbwp
#define R2MFRbase     R2recp->recmf.m1.m1base
#define R2MFRnvf      R2recp->recmf.m1.m1nvf
#define R2MFRstatus   R2recp->recmf.m1.m1status
#define R2MFRdir      R2recp->recmf.m1.m1dir

#define R2DIRtag(x)   R2recp->recmf.m1.m1dir[x].tag
#define R2DIRpos(x)   R2recp->recmf.m1.m1dir[x].pos
#define R2DIRlen(x)   R2recp->recmf.m1.m1dir[x].len

#define R2FIELDP(x)   VMFX(decridx)+(R2MFRbase+R2DIRpos(x))     /* wall!! */

#define R2RDBname     R2RECdbxp->dbxname
#endif /* CICPP */
/* end cidec.h */

#define TRACED1         00
#define TRACED2         00
#define TRACEDx         00
#define TRACEDT         00
#if TRACEDT
#define GETCHAR printf("\n");
#else
#define GETX    1
int xgetx;
#define GETCHAR (xgetx=getchar())
#endif


#if CICPP
int recdecod(CISISX * cisisxp,
	     RECSTRU *recp,
             char *decdbnp)
#else /* CICPP */
int recdecod(irec,decdbnp,wrec)                                         /*
------------                                                            */
LONGX irec;
char *decdbnp;
LONGX wrec;
#endif /* CICPP */

{
    VDECPSTRU *dmap,*lastdmap;
    unsigned char *datp,*p;
    LONGX xtag1,xtag2;
    int n,irng;

    LONGX wmfn;
    int wdir,wtag,wocc,wox;

    DBXSTRU *dbxp;

#if CICPP
    RECSTRU *wrecp=new RECSTRU(cisisxp);
    RECSTRU *keeprecp;
#else
    RECSTRU *recp;
    recp=vrecp[irec];
#endif /* CICPP */

#if !CICPP
    recp=vrecp[irec];
#endif /* CICPP */

    dbxp=RECdbxp;

    if (dectrace)
        printf("recdecod - 1. %s %p +%"_LD_"\n",DBXname,DBXvderp,CORELEFT());


#if CICPP
keeprecp=recp;
for (wmfn=1; ; wmfn++) {
        wrecp->xrecord(decdbnp,wmfn);
	recp=wrecp; /* current */
        if (RECrc == RCEOF) break;
#else /* CICPP */
for (wmfn=1; (RECORD(wrec,decdbnp,wmfn))->recrc != RCEOF; wmfn++) {
#endif /* CICPP */

    if (RECrc != RCNORMAL) continue;
#if 0
prtfields(recp,MFRmfn);
printf("+++\n");
if (getchar() != '\n') break;
#endif

    /*
        aloca e inicializa decod
    */
    n=sizeof(VDECPSTRU);

#if CICPP
    try { dmap = (VDECPSTRU *) new char [n]; }
    catch (BAD_ALLOC)
    { dmap = (VDECPSTRU *)ALLONULL; }
#else
    dmap=(VDECPSTRU *)ALLOC((ALLOPARM)n);
#endif /* CICPP */

    if (dmap == (VDECPSTRU *)ALLONULL)
        fatal("recdecod/ALLOC/0");
    memset(dmap,0x00,(unsigned)n);

    if (DBXvderp == NULL) DBXvderp=dmap;
    else {
        for (lastdmap=DBXvderp; lastdmap->nextp; )
            lastdmap=lastdmap->nextp;
        lastdmap->nextp=dmap;
    }

    /* process data base name */
#if CICPP
    if ((wdir=wrecp->xfieldx(wtag=DECOTAG1,wocc=1)) < 0) {
#else /* CICPP */
    if ((wdir=fieldx(wrec,wtag=DECOTAG1,wocc=1)) < 0) {
#endif /* CICPP */
        if (rectrace) fprintf(stderr,"*** %s/%"_LD_"/%d/%d \n",DBXname,wmfn,wtag,wocc);
        fatal("recdecod/DECOTAG1");
    }
    n=DIRlen(wdir);

#if CICPP
    try { dmap->ddbnp = new unsigned char [n+1]; }
    catch (BAD_ALLOC)
    { dmap->ddbnp = (unsigned char *)ALLONULL; }
#else
    dmap->ddbnp=(unsigned char *)ALLOC((ALLOPARM)n+1);
#endif /* CICPP */

    if ((dmap->ddbnp == (unsigned char *)ALLONULL))
        fatal("recdecod/ALLOC/dmap");
    memmove(dmap->ddbnp,FIELDP(wdir),n); dmap->ddbnp[n]='\0';

    if (dectrace)
        printf("recdecod - 2. %s=%p +%"_LD_"\n",dmap->ddbnp,dmap,CORELEFT());

    /* process repeatable subfield delimiter */
#if CICPP
    if ((wdir=wrecp->xfieldx(wtag=DECOTAG2,wocc=1)) < 0) dmap->drdlm = '\0';
#else /* CICPP */
    if ((wdir=fieldx(wrec,wtag=DECOTAG2,wocc=1)) < 0) dmap->drdlm = '\0';
#endif /* CICPP */

    else if (DIRlen(wdir) == 0) dmap->drdlm = '\0';
         else dmap->drdlm = *(FIELDP(wdir));

    /* allocate working area */
#if CICPP
    try { datp = new unsigned char [MAXMFRL]; }
    catch (BAD_ALLOC)
    { datp = (unsigned char *)ALLONULL; }
#else /* CICPP */
    datp=(char *)ALLOC((ALLOPARM)MAXMFRL);
#endif /* CICPP */
    if (datp == (unsigned char *)ALLONULL)
        fatal("recdecod/ALLOC/datp");

    /* process tag list */
#if CICPP
    wtag=DECOTAG3; dmap->drngs=wrecp->xnocc(wtag);
#else /* CICPP */
    wtag=DECOTAG3; dmap->drngs=nocc(wrec,wtag);
#endif /* CICPP */

    if ((wocc=dmap->drngs) > MAXDECR) {
        if (rectrace) fprintf(stderr,"*** %s/%"_LD_"/%d/%d \n",DBXname,wmfn,wtag,wocc);
        fatal("recdecod/DECOTAG3/nocc");
    }
    for (irng=0, wocc=1; wocc <= dmap->drngs; wocc++, irng++) {
        if (irng >= MAXDECR) fatal("recdecod/MAXDECR");

#if CICPP
        wdir=wrecp->xfieldx(wtag=DECOTAG3,wocc);
#else /* CICPP */
        wdir=fieldx(wrec,wtag=DECOTAG3,wocc);
#endif /* CICPP */

        if (wdir < 0) fatal("recdecod/internal error");
        memmove(datp,FIELDP(wdir),n=DIRlen(wdir)); datp[n]='\0';
        dmap->drng1[irng]=0;
        if (sscanf((CONST char *)datp,"%"_LD_"/%"_LD_,&xtag1,&xtag2) == 2)
            if (xtag1 >= 1 && xtag2 <= USHRT_MAX)
                if (xtag1 <= xtag2) {
                    dmap->drng1[irng]=(UWORD)xtag1;
                    dmap->drng2[irng]=(UWORD)xtag2;
                }
        if (sscanf((CONST char *)datp,"%"_LD_,&xtag1) == 1)
            if (xtag1 <= USHRT_MAX) {
                dmap->drng1[irng]=(UWORD)xtag1;
                dmap->drng2[irng]=(UWORD)xtag1;
            }
        if (dmap->drng1[irng] == 0) {
            if (rectrace) fprintf(stderr,"*** %s/%"_LD_"/%d/%d \n",DBXname,wmfn,wtag,wocc);
            fatal("recdecod/DECOTAG3/tag_or_tag/tag");
        }
    }

    /* process subfield list */
#if CICPP
    if ((wocc=dmap->dsflds=wrecp->xnocc(wtag=DECOTAG4)) > MAXDECX) {
#else /* CICPP */
    if ((wocc=dmap->dsflds=nocc(wrec,wtag=DECOTAG4)) > MAXDECX) {
#endif /* CICPP */
        if (rectrace) fprintf(stderr,"*** %s/%"_LD_"/%d/%d \n",DBXname,wmfn,wtag,wocc);
        fatal("recdecod/DECOTAG4/nocc");
    }
#if TRACEDx
printf("+++ 1. dsfld=%s=%d \n",dmap->dsfld,dmap->dsflds);
#endif
    for (wocc=1; wocc <= dmap->dsflds; wocc++) {

#if CICPP
        wdir=wrecp->xfieldx(wtag=DECOTAG4,wocc); wox=wocc-1;
#else /* CICPP */
        wdir=fieldx(wrec,wtag=DECOTAG4,wocc); wox=wocc-1;
#endif /* CICPP */

        memmove(dmap->dsfld,FIELDP(wdir),n=DIRlen(wdir));
        dmap->dsfld[n]='\0';
        for (p=dmap->dsfld; *p; p++) *p=isisuctab[*p];
        if ((p=(unsigned char *)strchr((char *)dmap->dsfld,SFLDCHR)) != NULL)
            if (*++p) {
                dmap->dsfldx[wox] = *p;
                dmap->dsfldv[wox] = atol((CONST char *)(p+1));
            }
    }

    /* deallocate working area */
#if CICPP
    delete[] datp;
    delete wrecp;
    recp=keeprecp;
#else /* CICPP */
    FREE(datp);
#endif /* CICPP */

#if TRACEDx
printf("+++ 2. dsfld=%s=%d \n",dmap->dsfld,dmap->dsflds);
#endif
} /* end record */

    /* flush parameters data base */
    dbxflush(decdbnp);

    if (dectrace)
        printf("recdecod - 3. %s %p +%"_LD_"\n",DBXname,DBXvderp,CORELEFT());

    for (dmap=DBXvderp; dmap; dmap=dmap->nextp)
#if CICPP
	recp->xmstsetup((char *)dmap->ddbnp,0L,0L);
#else /* CICPP */
        mstsetup(dmap->ddbnp,0L,0L);
#endif /* CICPP */

    if (dectrace)
        printf("recdecod - 4. %s %p +%"_LD_"\n",DBXname,DBXvderp,CORELEFT());

    return(0);
}



#if CICPP
LONGX recdecex (CISISX * cisisxp,
               RECSTRU *recp)
#else /* CICPP */
LONGX recdecex(irec)                                                   /*
-------------
                    abenda se vrecp nao inicializado;
                    abenda se registro irec nao alocado;
                    abenda se registro irec invalido;
                    processa irec segundo vdecop;
                    retorna o numero de matchs
                                                                      */

LONGX irec;          /* indice de vrecp, para info/store */
#endif /* CICPP */
{
    VDECPSTRU *dmap;

    RECSTRU *R2recp;
#if CICPP
    RECSTRU *decrecp=NULL;
    LONGX irec=0;
#else /* CICPP */
    RECSTRU *recp;
    LONGX decridx=0L;
#endif /* CICPP */

    short match,check,left;
    unsigned char *p,*q,c,*leftp;
    unsigned char *buffup,*batchp,*fuplenp,sfldc,rchar;
    LONGX nbytes,nmatchs,valmfn;
/* ?FFI */
    int xdir,dirloop,fldloop,fuplen,n,irng,rngok,sfldn,sfldlen,decxdir,imap,i;
    UWORD tag;


#if !CICPP
    if (!nrecs)
        fatal("recdecex/nrecs");
    recp=vrecp[irec];
#endif /* CICPP */

    if (!recp)
        fatal("recdecex/irec");

    if (RECtype != TYPEMFR) {
        if (rectrace) printf("*** irec=%"_LD_"  RECtype=%d\n",irec,RECtype);
        fatal("recdecex/check");
    }

    /* set up decod master files */
    if (dectrace)
        printf("recdecex - irec=%"_LD_"=%s/%"_LD_" +%"_LD_"\n",
            irec,RDBname,MFRmfn,CORELEFT());

    /* allocate buffer for fldupdat */
    nbytes=BUFFUPL;

#if CICPP
    try { buffup = new unsigned char [nbytes]; }
    catch (BAD_ALLOC)
    { buffup = (unsigned char *)ALLONULL; }
#else /* CICPP */
    buffup=(char *)ALLOC((ALLOPARM)nbytes);
#endif /* CICPP */

    if (buffup == (unsigned char *)ALLONULL)
        fatal("recdecex/ALLOC");
    buffup[0]='\0';

    nmatchs=0;

    batchp=buffup;

    for (xdir=0, dirloop=MFRnvf; dirloop--; xdir++) {
        tag=DIRtag(xdir);

#if TRACED1
printf("\\begin tag %d..",tag); GETCHAR;
#if GETX
if (xgetx != '\n') exit(1);
#endif
#endif

        for (imap=0, dmap=RDBvderp; dmap; dmap=dmap->nextp, imap++) {
            rngok=1;
#if TRACEDx
printf("+++ 3. dsfld=%s=%d \n",dmap->dsfld,dmap->dsflds);
#endif
            if (dmap->drngs)
                for (rngok=0, irng=0; irng < dmap->drngs; irng++) {
                    if (tag < dmap->drng1[irng]) continue;
                    if (tag > dmap->drng2[irng]) continue;
                    rngok=1; break;
                }
            if (rngok) break;
        } /* end decod list */

        if (rngok) {
            if (dectrace) {
                printf("recdecex - irec=%"_LD_"=%s/%"_LD_" %s,%02x=%c,",
                    irec,RDBname,MFRmfn,dmap->ddbnp,dmap->drdlm,dmap->drdlm);
                printf("%d,",tag);
                for (i=0; i < dmap->dsflds; i++) {
                    printf("^%c",dmap->dsfld[i]);
                    if (dmap->dsfldx[i])
                        printf("+%c=%"_LD_,dmap->dsfldx[i],dmap->dsfldv[i]);
                }
                printf("\n");
            }
#if TRACED2
*batchp='\0';
printf("\\buffup=%s",buffup); GETCHAR;
#endif
            sprintf((char *)batchp,"H%u 000000 ",tag); batchp+=strlen((CONST char *)batchp);
            fuplenp=batchp-1-6; fuplen=0; /* fuplenp points to 000000 */
            sfldn=0; sfldc='\0';
            for (p=(unsigned char *)FIELDP(xdir), fldloop=DIRlen(xdir); fldloop; sfldn++) {
                rchar = ~(dmap->drdlm);
#if TRACED2
printf("\\fldloop=%d *p=%c",fldloop,*p); GETCHAR;
#endif
                for (sfldlen=1, q=p+1, n=fldloop-1; n--; q++)
                    if (*q == SFLDCHR || (rchar = *q) == dmap->drdlm) break;
                    else sfldlen++;
                if (sfldlen > 1)
                    if (*p == SFLDCHR) {
                        sfldc = *(p+1);
                        sfldlen-=2;
                        memmove(batchp,p,2); batchp+=2; p+=2;
                        fldloop-=2; fuplen+=2;
                    }
                if (sfldlen == 0) continue;

                if (dmap->dsflds == 0)                  /* no dsfld */
                    check=0;
                else
                    if (sfldn == 0)
                        for (n=0; n < dmap->dsflds; n++)
                            if (dmap->dsfld[n] == '*') {
                                check=0; break;
                            }

                if (check) {
                    c=isisuctab[(int)sfldc];
                    for (n=0; n < dmap->dsflds; n++)
                        if (dmap->dsfld[n] == c) {
                            check=0; break;
                        }
                    if (check) {
                        memmove(batchp,p,n=sfldlen); batchp+=n; p+=n;
                        fldloop-=n; fuplen+=n;
                        continue;
                    }
                }

                match=0;
                for (valmfn=0L, q=p, left=n=sfldlen; n--; q++, left--)
                    if (isdigit(*q)) valmfn=valmfn*10+((int)*q-(int)'0');
                    else break;
                leftp=q;

                if (rchar == dmap->drdlm) sfldlen++; /* skip */
#if TRACED2
printf("\\valmfn=%"_LD_,valmfn); GETCHAR;
#endif
                /* decode */
                if (valmfn > 0L) {
#if CICPP
                    if (!decrecp) {
                        decrecp=new RECSTRU(cisisxp);
                    }
#else /* CICPP */
                    if (!decridx) {
                        for (decridx=maxnrec; decridx--; )
                            if (!vrecp[decridx]) break;
                        if (decridx > 0) recallok(decridx,MAXMFRL);
                        else valmfn=0L;                         /* ok */
                    }
#endif /* CICPP */
                }
                if (valmfn > 0L) {                              /* ok */
#if CICPP
		    R2RECORD(decrecp,(char *)dmap->ddbnp,valmfn);
#else /* CICPP */
                    R2RECORD(decridx,dmap->ddbnp,valmfn);
#endif /* CICPP */
                    if (R2MFRnvf == 0) R2RECrc=RCLDEL;
                    if (R2RECrc == RCNORMAL) { /* break */
                        nmatchs++; match=1;
#if TRACED2
printf("\\match #%"_LD_,nmatchs); GETCHAR;
#endif
                        if (dectrace)
                            printf("recdecex - %s/%"_LD_"/%d[%d] = %s/%"_LD_,
                                RDBname,MFRmfn,DIRtag(xdir),xdir,
                                R2RDBname,R2MFRmfn);
                        decxdir=0;
                        for (; decxdir < R2MFRnvf; decxdir++) {
                            if (dectrace) printf("/%d",R2DIRtag(decxdir));
                            q=(unsigned char *)R2FIELDP(decxdir);
                            memmove(batchp,q,n=R2DIRlen(decxdir));
                            batchp+=n; fuplen+=n;
                            if (left) {
                                memmove(batchp,leftp,left);
				batchp+=left; fuplen+=left;
                            }
                        }
                        if (dectrace) printf("\n");
                        p+=sfldlen;
                        fldloop-=sfldlen;
                    }
                }

                if (!match) {
#if TRACED2
printf("\\no match"); GETCHAR;
#endif

                    if (rchar == dmap->drdlm) {
                        memmove(batchp,p,n=sfldlen-1); batchp+=n; p+=n+1;
                        fldloop-=n+1; fuplen+=n;
                    }
                    else {
                        memmove(batchp,p,n=sfldlen); batchp+=n; p+=n;
                        fldloop-=n; fuplen+=n;
                    }
		}

                if (rchar == dmap->drdlm) {
                    sprintf((char *)fuplenp,"%06d",fuplen); *(fuplenp+6)=' ';
                    sprintf((char *)batchp,"H%u 000000 ",tag); batchp+=strlen((CONST char *)batchp);
                    fuplenp=batchp-1-6; fuplen=0; /* fuplenp ... 000000 */
                    sfldn=0; sfldc='\0';
                }

            } /* end field chrs */

            sprintf((char *)fuplenp,"%06d",fuplen); *(fuplenp+6)=' ';

        } /* end rngok */

        else {  /* just copy it */
            sprintf((char *)batchp,"H%u %u ",tag,DIRlen(xdir)); batchp+=strlen((CONST char *)batchp);
            memmove(batchp,FIELDP(xdir),DIRlen(xdir)); batchp+=DIRlen(xdir);
        }

    } /* end dir entries */

    *batchp='\0';

    /* deallocate decridx */
#if CICPP
    if (decrecp) delete decrecp;
#else /* CICPP */
    if (decridx) {
        FREE(vrecp[decridx]); vrecp[decridx]=NULL; nrecs--;
    }
#endif /* CICPP */

    if (nmatchs) {

	MFRmfrl=MFRbase=LEADER; MFRnvf=0;

#if CICPP
	p=(unsigned char *)recp->xfldupdat((char *)buffup);
#else /* CICPP */
	p=fldupdat(irec,buffup);
#endif /* CICPP */

	if (p) {
	    /* printf("*** fldupdat='%s'\n",p); */
            fatal("recdecex/fldupdat");
        }
    }

    /* deallocate working area */
#if CICPP
    delete[] buffup;
#else /* CICPP */
    FREE(buffup);
#endif /* CICPP */

    if (dectrace)
       printf("recdecex - irec=%"_LD_"/%"_LD_" matchs=%"_LD_" +%"_LD_"\n",
           irec,MFRmfn,nmatchs,CORELEFT());

    return(nmatchs);
}

#if CICPP
#undef CORELEFT
#endif /* CICPP */

#undef GETCHAR

#endif /* RECDECO */
