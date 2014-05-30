
/* cihsh.c - included from ciupd.c
*/

//#include <string.h>
//#include "cisis.h"   /* CISIS Interface header file */

LONGX hashzero(char *table, LONGX maxprim, int tabwidth)
{
    size_t size=maxprim*(tabwidth+1);
    memset(table,0x00,size);
    return((LONGX)size);
}

#if CICPP
char * RECSTRU :: hashalloc(LONGX classes, int tabwidth, LONGX *maxprimp)
#else /* CICPP */
char *hashalloc(LONGX classes, int tabwidth, LONGX *maxprimp)
#endif /* CICPP */
{
    LONGX maxprim=classes;
    char *p;
    LONGX maxdiv,divid;
    int done=0;
    while (!done) {
        maxdiv=maxprim/2;
        for (divid=2; divid<maxdiv; divid++)
            if (!(maxprim % divid)) { maxprim--; break; }
        if (divid >= maxdiv) done=1;
    }
    p=loadfile(NULL,'@',"",NULL,maxprim*(tabwidth+1),'\0');
    hashzero(p,maxprim,tabwidth);
    *maxprimp=maxprim;
    return(p);
}

#if CICPP
LONGX RECSTRU :: hashindex(char *table, LONGX maxprim, int tabwidth, char *keyp, int keylen, int *foundp, int installit)
#else /* CICPP */
LONGX hashindex(char *table, LONGX maxprim, int tabwidth, char *keyp, int keylen, int *foundp, int installit)
#endif /* CICPP */
{
    unsigned LONGX hashv=0;
    unsigned char *hashvp=(unsigned char *)&hashv;
    int hashvloop=sizeof(hashv);
    int loop=hashvloop;
    int left;
    unsigned char *p=hashvp;
    unsigned char *kp=(unsigned char *)keyp;
    int found=0;
    LONGX hidx,x;
    char *h;

    if (keylen > tabwidth) keylen=tabwidth;
    for (left=keylen; left--; ) {
        *p ^= *kp; p++; kp++; loop--;
        if (!loop) { p=hashvp; loop=hashvloop; }
    }
    x=hidx=hashv%maxprim;
    tabwidth++; /* (tabwidth+1) */
    h=table+hidx*tabwidth;
    while (*h) {
        if (!memcmp(h,keyp,keylen)) if (!h[keylen]) { found=1; break; }
        if (++hidx == maxprim) { hidx=0; h=table; }
        else h+=tabwidth;
        if (hidx == x) fatal("cihsh/hashindex/overflow");
    }
    if (!found) if (installit) memmove(h,keyp,keylen);
    *foundp=found;
    return(hidx);
}


#if CICPP
char * RECSTRU :: bsrchalloc(LONGX classes, int tabwidth, LONGX *tabentries)
#else /* CICPP */
char *bsrchalloc(LONGX classes, int tabwidth, LONGX *tabentries)
#endif /* CICPP */
{
    char *p;
    p=loadfile(NULL,'@',"",NULL,classes*(tabwidth+1),'\0');
    hashzero(p,classes,tabwidth);
    *tabentries=0;
    return(p);
}

#if CICPP
LONGX RECSTRU :: bsrchstore(char *table, LONGX classes, LONGX *tabentries, int tabwidth, char *keyp, int keylen)
#else /* CICPP */
LONGX bsrchstore(char *table, LONGX classes, LONGX *tabentries, int tabwidth, char *keyp, int keylen)
#endif /* CICPP */
{
    char *h;
    int w=tabwidth+1;
    LONGX entries= *tabentries;
    LONGX index;

    if (keylen > tabwidth) keylen=tabwidth;

    if (entries >= classes) fatal("cihsh/bsrchstore/overflow");

    h=table+w*(entries-1);
    if (memcmp(keyp,h,w) <= 0) fatal("cihsh/bsrchstore/unsorted");

    h+=w; index=entries;
    memmove(h,keyp,keylen); (*tabentries)++;

    return(index);
}

#if CICPP
LONGX RECSTRU :: bsrchindex(char *table, LONGX tabentries, int tabwidth, char *keyp, int keylen, int *foundp)
#else /* CICPP */
LONGX bsrchindex(char *table, LONGX tabentries, int tabwidth, char *keyp, int keylen, int *foundp)
#endif /* CICPP */
{
    LONGX i1,i2,i;
    char *h;
    int w=tabwidth+1;
    int found=0;
    LONGX index;

    if (keylen > tabwidth) keylen=tabwidth;

    i1=0;
    i2=tabentries-1;
    while (i1 <= i2) {
        i=(i1+i2)/2;
        h= table+w*i;
        //if (memcmp(keyp,h,keylen) == 0) if (!h[keylen]) { found=1; index=i; break; }
        //if (memcmp(keyp,h,keylen) < 0) i2=i-1; else i1=i+1;

        if (memcmp(keyp,h,keylen) == 0) if (!h[keylen]) { found=1; index=i; break; }

        if (memcmp(keyp,h,keylen) < 0) i2=i-1;
        else
        if (memcmp(keyp,h,keylen) > 0) i1=i+1;
        else
        if (h[keylen]) i2=i-1; else fatal("cihsh/bsrchindex/bug");
    }

    if (!found) index=(-1);
    *foundp=found;
    return(index);
}

