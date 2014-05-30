#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "cisis.h"

#if CICPP
#include <ciiso.hpp>
#define CIISO_SOURCE
#include <cirun.hpp>
/* ------------------------- CIISO.HPP ---------------------------------- */
#ifdef CICPP
#define iso_openx     xiso_openx                         /* internal use */
#endif
#define iso_creat     xiso_creat                         /* internal use */
#define iso_write     xiso_write                         /* internal use */
#define iso_bmove     xiso_bmove                         /* internal use */
#define iso_break     xiso_break                         /* internal use */
#define iso_close     xiso_close                         /* internal use */
#define iso_rclos     xiso_rclos                         /* internal use */
#if GEN_ISI
#define iso_open      xiso_open                          /* internal use */
/* #define iso_read    xiso_read                           no conversion */
#define iso_loadbuff  xiso_loadbuff                      /* internal use */
#define iso_getval    xiso_getval                        /* internal use */
#endif
/* ---------------------------------------------------------------------- */
#endif /* CICPP */

/* -------------------------------- iso.c -------------------------------- */
#define TRACE           0
#define ENABHDR         0
#define ENABDIR         0
#define ENABFLD         0
#define TRACISI         0

#if !CICPP
#if !CICPP
unsigned char *iso_buff = NULL;         /* iso_bmove / iso_break */
int iso_fd = 0;                         /* iso_bmove / iso_break */
int iso_lrecb;                          /* iso_bmove / iso_break */
int iso_lrfox;                          /* iso_bmove / iso_break */
int iso_isfox;                          /* iso_bmove / iso_break */
unsigned iso_bfree;                     /* iso_bmove / iso_break */
unsigned iso_bidx;                      /* iso_bmove / iso_break */
unsigned iso_bfix;                      /* iso_bmove / iso_break */
unsigned iso_cards;                     /* iso_bmove / iso_break */
unsigned char iso_fmtlef[] = {"%0#d"};
unsigned char iso_fmtscp[] = {"%0#d"};
int iso_ismarc = 0;
#endif /* CICPP */
#if ANSI
int iso_bmove(unsigned char *obuf, unsigned obufsize);
int iso_break(int isfox);
#else
int iso_bmove();
int iso_break();
#endif
#endif /* !CICPP */

#define MARC_BEFORE_20090311 0   /* HB - following MARC21 layout*/


#if CICPP
ISO_2709 :: ISO_2709 (CISISX *cxp) : cisisxp(cxp),
                              iso_buff(NULL), iso_fd(0),    iso_lrecb(0),
                  iso_lrfox(0),   iso_isfox(0), iso_bfree(0),
                  iso_bidx(0),    iso_bfix(0),  iso_cards(0), iso_ismarc(0),
#if GEN_ISI
                  isi_buffin(NULL), isi_fd(0),
                  isi_lrecb(0),  isi_lrfix(0),
                  isi_isfix(0),  isi_count(0L)
#endif  /* GEN_ISI */
{
 iso_fmtlef=(unsigned char *)strdup("%0#d"); // DS 09/95
 iso_fmtscp=(unsigned char *)strdup("%0#d");

 if (cisisxp == NULL)
  fatal("ISO_2709/NULL");
}
#endif /* CICPP */

#if CICPP
ISO_2709 :: ~ISO_2709 (void)
{
 if (iso_fd > 0) // Existe arquivo iso de exportação aberto, entao fecha-o.
 {
  xiso_close();
  iso_fd = 0;
 }
 if (isi_buffin)
 {
  delete[] isi_buffin;
  isi_buffin = NULL;
 }
 if (isi_fd > 0)
  CLOSE(isi_fd);
 isi_fd = 0;
 isi_count = 0L;
 iso_ismarc = 0;
 free(iso_fmtlef);
 free(iso_fmtscp);
}
#endif /* CICPP */

#if CICPP
int ISO_2709 :: xiso_openx(char *gidbnp,
                           unsigned char filnam[],
                           int lrecb,
                           int lrfox,
                           int isfox)
{
 char *filnamp;

 /* open output file */
 filnamp=dbxcipar(gidbnp,(char *)filnam,'=');

 if (iso_fd)
 xiso_close();
 iso_fd= OPEN(filnamp,O_RDWR|O_BINARY|O_APPEND);        /* BINARY */

 if (iso_fd <= 0)
 {
  fatal( filnamp);
  return (iso_fd); /* mffunc.cpp */
 }

 /* allocate output buffer */
 try {iso_buff = (unsigned char *) new char [ISOBSIZ+800];}
 catch (BAD_ALLOC){ iso_buff = (unsigned char *)ALLONULL;}

 if ( iso_buff == (unsigned char *)ALLONULL)
  fatal( "xiso_openx/ALLOC");
 if (iso_lrecb)
  fatal( "xiso_openx/lrecb");       /* for time being */

 /* keep lrecb/lrfox/isfox */
 iso_lrecb = lrecb;
 iso_lrfox = lrfox;
 iso_isfox = isfox;

 /* setup formats "%0#d" */
 iso_fmtlef[2]=ISOLLEF+'0';
 iso_fmtscp[2]=ISOLSCP+'0';
 return(iso_fd);
}
#endif /* CICPP */

#if CICPP
int ISO_2709 :: xiso_creat(char *gidbnp,
                           unsigned char filnam[],
               int lrecb,
               int lrfox,
               int isfox)
#else
int iso_creat(gidbnp,filnam,lrecb,lrfox,isfox)
char *gidbnp;
unsigned char filnam[];
int lrecb;
int lrfox;
int isfox;
#endif /* CICPP */
{
    char *filnamp;
#if MPE
    char dcb[16];
#endif
    /* open output file */
    filnamp=dbxcipar(gidbnp,(char *)filnam,'=');

#if MPE
    sprintf(dcb,"R%d",lrfox);
    iso_fd=OPEN(filnamp,O_WRONLY|O_CREAT|O_TRUNC|O_MPEOPTS,PERMIS,dcb);
#else
#if CICPP
#if !GCC // na verdade #if PC, pois PC no CISIS.h significa DOS ou Windows
         // em UNIX, fmode e' sempre BINARY (vide #define O_BINARY em CISIS.H)
    unsigned int mode;
    mode=_fmode; _fmode=O_BINARY;
#endif // !GCC
    iso_fd=CREAT(filnamp,PERMIS);        /* BINARY */
#if !GCC
    _fmode=mode;
#endif // !GCC
#else
    iso_fd=CREAT(filnamp,PERMIS);        /* BINARY */
#endif /* CICPP */
#endif
    if (iso_fd <= 0) {
    fatal( filnamp);
    return (iso_fd); /* mffunc.cpp */
    }

    /* allocate output buffer */
#if CICPP
    try {iso_buff = (unsigned char *) new char [ISOBSIZ+800];}
    catch (BAD_ALLOC){ iso_buff = (unsigned char *)ALLONULL;}
#else
    iso_buff = ALLOC((ALLOPARM)(ISOBSIZ+800));
#endif /* CICPP */
    if ( iso_buff == (unsigned char *)ALLONULL)
    fatal( "iso_creat/ALLOC");
    if (iso_lrecb)
    fatal( "iso_creat/lrecb");       /* for time being */
    /* keep lrecb/lrfox/isfox */
    iso_lrecb = lrecb;
    iso_lrfox = lrfox;
    iso_isfox = isfox;
    /* setup formats "%0#d" */
    iso_fmtlef[2]=ISOLLEF+'0';
    iso_fmtscp[2]=ISOLSCP+'0';
    return(iso_fd);
}


#define LDRTAGN(x) \
    for (n=0, dirp=MFRdir, loop=MFRnvf; loop--; dirp++) \
      if (dirp->tag == x) { n=dirp->len; p=(unsigned char *)MFX+(MFRbase+dirp->pos); break; }


#if CICPP
int ISO_2709 :: xiso_write(RECSTRU *recp,
               unsigned char isoxfs,
               unsigned char isoxgs,
            unsigned int ldrtag)
#else
int iso_write(recp, isoxfs, isoxgs, ldrtag)
RECSTRU *recp;
unsigned char isoxfs;
unsigned char isoxgs;
unsigned int ldrtag;
#endif /* CICPP */
{
    ISOHSTRU *isohp;
    ISODSTRU *isodp;
    DIRSTRU *dirp;
    unsigned loop;
    short isolen;
    short isopos;
    int nvfs=0;
    char areah[ISOHSIZE+1],aread[ISODSIZE+1]; /* last byte for str moving */

    /* check previous iso_creat() */
    if (!iso_buff || iso_fd <= 0)
    fatal( "iso_write/creat");
    /* initialize iso_bmove() */
    iso_bfree=ISOBSIZ;
    iso_bidx=0;
    iso_bfix=iso_lrfox;
    iso_cards=0;
    /* leader segment */
    isohp = (ISOHSTRU *)&areah[0]; /* TC: &areah */
    isolen=ISOHSIZE;

    for (dirp=MFRdir, loop=MFRnvf; loop--; dirp++) {
      /* skip leader data fields for MARC processing - AOT 11/06/2001 */
      if (ldrtag) if (dirp->tag >= ldrtag) if (dirp->tag <= ldrtag+ISOHSIZE) continue;
      isolen+=(ISODSIZE + dirp->len + 1); nvfs++;
    }

    isolen+=2;
    sprintf((char *)isohp->length,"%05d",isolen);
        
#if MARC_BEFORE_20090311
    *isohp->status='0';
    strcpy((char *)isohp->icodes,"0000");
    *isohp->indlen='0';
    *isohp->sublen='0';
    sprintf((char *)isohp->basead,"%05d",ISOHSIZE + nvfs*ISODSIZE + 1);
    strcpy((char *)isohp->forusr,"000");
    *isohp->lenlef=ISOLLEF+'0';
    *isohp->lenscp=ISOLSCP+'0';
    strcpy((char *)isohp->forfut,"00");

   /* store leader data fields for MARC processing - AOT 11/06/2001 */
#if 0
byte 5 = Estado del registro
byte 6 = Tipo de registro
byte 7 = Nivel bibliográfico
byte 8 = Tipo de control
byte 17 = Nivel de codificación
byte 18 = Forma de catalogación
byte 19 = Linked requirement
typedef struct isohstru {
     unsigned char length[5];
     unsigned char status[1]; /* 5=Estado del registro */
     unsigned char icodes[4]; /* 6=Tipo de registro; 7=Nivel bibliográfico; 8=Tipo de control; unused */
     unsigned char indlen[1];
     unsigned char sublen[1];
     unsigned char basead[5];
     unsigned char forusr[3]; /* 17=Nivel de codificación; 18=Forma de catalogación; 19=Linked requirement */
#endif
    if (ldrtag) {
      FFI n;
      unsigned char *p;
      LDRTAGN(ldrtag+5); if (n) isohp->status[0]= *p;
      LDRTAGN(ldrtag+6); if (n) isohp->icodes[0]= *p;
      LDRTAGN(ldrtag+7); if (n) isohp->icodes[1]= *p;
      LDRTAGN(ldrtag+8); if (n) isohp->icodes[2]= *p;
      LDRTAGN(ldrtag+17); if (n) isohp->forusr[0]= *p;
      LDRTAGN(ldrtag+18); if (n) isohp->forusr[1]= *p;
      LDRTAGN(ldrtag+19); if (n) isohp->forusr[2]= *p;
    }
#else /* MARC_BEFORE_20090311 */
    /* 
       See http://www.itsmarc.com/crs/auth0669.htm 
            bytes 00-04 - Logical record length
            byte  05    - Record status
            byte  06    - Type of record
            bytes 07-08 - Undefined
            byte  09    - Character coding scheme
            byte  10    - Indicator count
            byte  11    - Subfield code length
            bytes 12-16 - Base address of data
            byte  17    - Encoding level
            bytes 18-19 - Undefined
            bytes 20-23 - Entry map       
    */
    if (iso_ismarc) {
        *isohp->status='n';
        isohp->icodes[0]='z';
        isohp->icodes[1]=' ';
        isohp->icodes[2]=' ';
        isohp->icodes[3]=' ';
        *isohp->indlen='2';
        *isohp->sublen='2';
        sprintf((char *)isohp->basead,"%05d",ISOHSIZE + nvfs*ISODSIZE + 1);
        isohp->forusr[0]='n';
        isohp->forusr[1]=' ';
        isohp->forusr[2]=' ';
        *isohp->lenlef='4';
        *isohp->lenscp='5';
        strcpy((char *)isohp->forfut,"00");
        if (ldrtag) {
           FFI n;
           unsigned char *p;
           LDRTAGN(ldrtag+5);  if (n) isohp->status[0]= *p; /* Record status */
           LDRTAGN(ldrtag+9);  if (n) isohp->icodes[3]= *p; /* Character coding scheme */
           LDRTAGN(ldrtag+17); if (n) isohp->forusr[0]= *p; /* Encoding level */
        }
    } else {
        *isohp->status='0';
        strcpy((char *)isohp->icodes,"0000");
        *isohp->indlen='0';
        *isohp->sublen='0';
        sprintf((char *)isohp->basead,"%05d",ISOHSIZE + nvfs*ISODSIZE + 1);
        strcpy((char *)isohp->forusr,"000");
        *isohp->lenlef=ISOLLEF+'0';
        *isohp->lenscp=ISOLSCP+'0';
        strcpy((char *)isohp->forfut,"00");
        if (ldrtag) {
           FFI n;
           unsigned char *p;
           LDRTAGN(ldrtag+5);  if (n) isohp->status[0]= *p;
           LDRTAGN(ldrtag+6);  if (n) isohp->icodes[0]= *p;
           LDRTAGN(ldrtag+7);  if (n) isohp->icodes[1]= *p;
           LDRTAGN(ldrtag+8);  if (n) isohp->icodes[2]= *p;
           LDRTAGN(ldrtag+17); if (n) isohp->forusr[0]= *p;
           LDRTAGN(ldrtag+18); if (n) isohp->forusr[1]= *p;
           LDRTAGN(ldrtag+19); if (n) isohp->forusr[2]= *p;
        }
    }    
#endif /* MARC_BEFORE_20090311 */
    iso_bmove((unsigned char *)isohp,ISOHSIZE);

    /* directory segment */
    isodp = (ISODSTRU *)&aread[0]; /* TC: &aread */
    isopos=0;
    for (dirp=MFRdir, loop=MFRnvf; loop--; dirp++) {
    /* skip leader data fields for MARC processing - AOT 11/06/2001 */
    if (ldrtag) if (dirp->tag >= ldrtag) if (dirp->tag <= ldrtag+ISOHSIZE) continue;
    sprintf((char *)isodp->tagfld,"%03d",dirp->tag);
        sprintf((char *)isodp->lenfld,(CONST char *)iso_fmtlef,dirp->len + 1);
        sprintf((char *)isodp->locfld,(CONST char *)iso_fmtscp,isopos);
    isopos+=(dirp->len + 1);
    iso_bmove((unsigned char *)isodp,ISODSIZE);
    }
    /* variable data fields segment */
    for (dirp=MFRdir, loop=0; loop < MFRnvf; loop++, dirp++) {
    /* skip leader data fields for MARC processing - AOT 11/06/2001 */
    if (ldrtag) if (dirp->tag >= ldrtag) if (dirp->tag <= ldrtag+ISOHSIZE) continue;
    iso_bmove(&isoxfs,1);
    iso_bmove((unsigned char*)FIELDP(loop),DIRlen(loop));
    }
    /* end of iso record */
    iso_bmove(&isoxfs,1);
    iso_bmove(&isoxgs,1);
    iso_break(iso_isfox);
    return(iso_cards);
}

#if CICPP
int ISO_2709 :: xiso_bmove(unsigned char *obuf,
               unsigned  obufsize)
#else
int iso_bmove(obuf,obufsize)
unsigned char *obuf;
unsigned obufsize;
#endif /* CICPP */
{
    unsigned char *p,*q;
    if (obufsize > ISOBSIZ)
    fatal( "iso_bmove/overflow");
    for (q=obuf; obufsize; ) {
#if TRAC0
printf("iso_bmove - obufsize=%d\n",obufsize);
#endif
    if (iso_bfree > 2) {
        p = iso_buff + iso_bidx; *p++ = *q++; obufsize--;
        iso_bfree--; iso_bidx++; iso_bfix--;
        if (iso_bfix == 0) {
#if !MPE
#if CRLF
        *p++ = '\r'; iso_bfree--; iso_bidx++;
#endif
        *p = '\n';   iso_bfree--; iso_bidx++;
#endif
        iso_bfix=iso_lrfox;
        iso_cards++;
        }
    }
    else {
#if TRACE
printf("iso_bmove - iso_bidx=%d\n",iso_bidx);
#endif
        if (CIWRITE(iso_fd,iso_buff,iso_bidx) != iso_bidx)
        fatal( "iso_bmove/write");
        iso_bfree=ISOBSIZ;
        iso_bidx=0;
    }
    }
    return(iso_cards);
}

#if CICPP
int ISO_2709 :: xiso_break(int isfox)
#else
int iso_break(isfox)
int isfox;
#endif /* CICPP */
{
    if (iso_bidx) {
    if (iso_bfix != iso_lrfox) {
#if TRACE
printf("iso_break - iso_bidx=%d\n",iso_bidx);
#endif
        if (isfox >= 0) {   /* AOT, 10/06/2005 */
            if (isfox)
                while (iso_bfix--)
                    iso_buff[iso_bidx++]=' ';
#if !MPE
#if CRLF
            iso_buff[iso_bidx++] = '\r';
#endif
            iso_buff[iso_bidx++] = '\n';
#endif
        }
        iso_cards++;
    }
#if TRACE
printf("iso_break - iso_bidx=%d\n",iso_bidx);
#endif
    if (CIWRITE(iso_fd,iso_buff,iso_bidx) != iso_bidx)
        fatal( "iso_break/write");
    iso_bfree=ISOBSIZ;
    iso_bidx=0;
    iso_bfix=iso_lrfox;
    }
    return(iso_cards);
}

#if CICPP
int ISO_2709 :: xiso_close(void)
#else
#if ANSI
int iso_close(void)
#else
int iso_close()
#endif
#endif /* CICPP */
{
    int rc;
    /* check previous iso_creat() */
    if (!iso_buff || iso_fd <= 0)
    fatal( "iso_close/creat");
#if CICPP
    delete[] iso_buff;
#else
    FREE(iso_buff);
#endif /* CICPP */
    iso_buff = NULL;
    rc = CLOSE(iso_fd);         iso_fd = 0;
    return(rc);
}


/* -------------------------------- isi.c -------------------------------- */
#if GEN_ISI
#define ISIBSIZ  ISOBSIZ                             /* AOT, 10/06/2005 */

#if !CICPP

#if !CICPP
UBYTE *isi_buffin = NULL;                            /* iso_loadbuff */
int isi_fd = 0;                                      /* iso_loadbuff */
int isi_lrecb;                                       /* iso_loadbuff */
int isi_lrfix;                                       /* iso_loadbuff */
int isi_isfix;                                       /* iso_loadbuff */
LONGX isi_count;                                      /* iso_loadbuff */
#endif /* CICPP */

#define LENLEN          5
#define LENTAG          3
#define LENLEF          4
#define LENSCP          5
/* prototypes */
#if ANSI
int iso_loadbuff(int opt);
int iso_getval(char *p, int n);
#else
int iso_loadbuff();
int iso_getval();
#endif
#endif /* !CICPP */

#if CICPP
int ISO_2709 :: xiso_open(char *gidbnp,
                          unsigned char filnam[],
              int lrecb,
              int lrfix,
              int isfix)
#else
int iso_open(gidbnp,filnam,lrecb,lrfix,isfix)
char *gidbnp;
unsigned char filnam[];
int lrecb;
int lrfix;
int isfix;
#endif /* CICPP */
{
    char *filnamp;

    /* open input file */
    filnamp=dbxcipar(gidbnp,(char *)filnam,'=');
    isi_fd=OPEN(filnamp,O_RDONLY|O_BINARY);
    if (isi_fd <= 0) {
    fatal( filnamp);
    return (isi_fd); /* mffunc.cpp */
    }

    /* allocate input buffer */
#if CICPP
    try {isi_buffin = (UBYTE *) new char [ISIBSIZ];}
    catch (BAD_ALLOC){isi_buffin = (UBYTE *)ALLONULL;}
#else
    isi_buffin=ALLOC((ALLOPARM)(ISIBSIZ));
#endif /* CICPP */
    if (isi_buffin == (UBYTE *)ALLONULL)
     fatal( "iso_open/ALLOC");

    /* keep lrecb/lrfix/isfix */
    isi_lrecb = lrecb;
    isi_lrfix = lrfix;
    isi_isfix = isfix;
    /* reset count */
    isi_count = 0L;
    return(isi_fd);
}

#if CICPP
int ISO_2709 :: xiso_read(RECSTRU * recp, unsigned int ldrtag)
#else
int iso_read(crec, irec, ldrtag)
LONGX crec;
LONGX irec;
unsigned int ldrtag;
#endif /* CICPP */
{
#if !CICPP
    RECSTRU *recp;
#endif /* CICPP */
    int n1,n,nvf,fld;
    int tagfld,lenfld,locfld,basead,lenlef,lenscp;
    UBYTE *buffup,*batchp,*p;
    ISOHSTRU *isohp;
    ISODSTRU *isodp;
#if ENABFLD
    UBYTE c;
#endif
    /* check previous iso_creat() */
    if (!isi_buffin || isi_fd <= 0)
    fatal( "iso_read/open");
    /* set current MFR to irec */
#if !CICPP
    recp=vrecp[irec];
#endif /* CICPP */
    /* allocate fldupdat() buffer */
#if CICPP
    try {buffup = (UBYTE *) new char [BUFFUPL];}
    catch (BAD_ALLOC){buffup = (UBYTE *)ALLONULL;}
#else
    buffup=ALLOC((ALLOPARM)(BUFFUPL));
#endif /* CICPP */
    if (buffup == (UBYTE *)ALLONULL)
     fatal( "iso_read/ALLOC");
    /* initialize iso_loadbuff and/or read next iso record */
    n1=iso_loadbuff((isi_count) ? LENLEN : 0 );

    /* load it */
    if (n1 != EOF) {
    isi_count++;
    isohp= (ISOHSTRU *)isi_buffin;
#if TRACISI
printf("isi_buffin=%.60s\n",isi_buffin);
#endif
    if (iso_getval((char *)isohp->length,LENLEN) != n1)
     fatal( "iso2i/length");

    basead=iso_getval((char *)isohp->basead,5);
    lenlef=iso_getval((char *)isohp->lenlef,1);

    if (lenlef != LENLEF) fatal( "LENLEF");

    lenscp=iso_getval((char *)isohp->lenscp,1);

    if (lenscp != LENSCP)
        fatal( "LENSCP");
    nvf=(basead-ISOHSIZE-1)/(LENTAG+lenlef+lenscp);
#if ENABHDR
 printf("iso %"_LD_" - isolen=%d base=%d lenlef=%d lenscp=%d nvf=%d\n",
  isi_count,n1,basead,lenlef,lenscp,nvf);
#endif
#if BEFORE20000809
    if (nvf < 1) {
        fprintf(stderr,"*** iso #%"_LD_"  -> nvf=%d\n",isi_count,nvf);
        fatal( "iso_read/nvf");
    }
#endif
    isodp=(ISODSTRU *)(isi_buffin+ISOHSIZE);
    batchp=buffup;

#if MARC_BEFORE_20090311    
   /* load leader data fields for MARC processing - AOT 11/06/2001 */
#if 0
byte 5 = Estado del registro
byte 6 = Tipo de registro
byte 7 = Nivel bibliográfico
byte 8 = Tipo de control
byte 17 = Nivel de codificación
byte 18 = Forma de catalogación
byte 19 = Linked requirement
typedef struct isohstru {
     unsigned char length[5];
     unsigned char status[1]; /* 5=Estado del registro */
     unsigned char icodes[4]; /* 6=Tipo de registro; 7=Nivel bibliográfico; 8=Tipo de control; unused */
     unsigned char indlen[1];
     unsigned char sublen[1];
     unsigned char basead[5];
     unsigned char forusr[3]; /* 17=Nivel de codificación; 18=Forma de catalogación; 19=Linked requirement */
#endif
   if (ldrtag) {
     sprintf((char *)batchp,"H%d 1 %c",ldrtag+5,isohp->status[0]); batchp+=strlen((const char *)batchp);
     sprintf((char *)batchp,"H%d 1 %c",ldrtag+6,isohp->icodes[0]); batchp+=strlen((const char *)batchp);
     sprintf((char *)batchp,"H%d 1 %c",ldrtag+7,isohp->icodes[1]); batchp+=strlen((const char *)batchp);
     sprintf((char *)batchp,"H%d 1 %c",ldrtag+8,isohp->icodes[2]); batchp+=strlen((const char *)batchp);
     sprintf((char *)batchp,"H%d 1 %c",ldrtag+17,isohp->forusr[0]); batchp+=strlen((const char *)batchp);
     sprintf((char *)batchp,"H%d 1 %c",ldrtag+18,isohp->forusr[1]); batchp+=strlen((const char *)batchp);
     sprintf((char *)batchp,"H%d 1 %c",ldrtag+19,isohp->forusr[2]); batchp+=strlen((const char *)batchp);
   }
#else /* MARC_BEFORE_20090311 */
    /* 
       See http://www.itsmarc.com/crs/auth0669.htm 
            bytes 00-04 - Logical record length
            byte  05    - Record status
            byte  06    - Type of record
            bytes 07-08 - Undefined
            byte  09    - Character coding scheme
            byte  10    - Indicator count
            byte  11    - Subfield code length
            bytes 12-16 - Base address of data
            byte  17    - Encoding level
            bytes 18-19 - Undefined
            bytes 20-23 - Entry map       
    */
   if (ldrtag) {
       if (iso_ismarc) {
          sprintf((char *)batchp,"H%d 1 %c",ldrtag+5,isohp->status[0]); batchp+=strlen((const char *)batchp);
          sprintf((char *)batchp,"H%d 1 %c",ldrtag+9,isohp->icodes[3]); batchp+=strlen((const char *)batchp);
          sprintf((char *)batchp,"H%d 1 %c",ldrtag+17,isohp->forusr[0]); batchp+=strlen((const char *)batchp);
       } else {
          sprintf((char *)batchp,"H%d 1 %c",ldrtag+5,isohp->status[0]); batchp+=strlen((const char *)batchp);
          sprintf((char *)batchp,"H%d 1 %c",ldrtag+6,isohp->icodes[0]); batchp+=strlen((const char *)batchp);
          sprintf((char *)batchp,"H%d 1 %c",ldrtag+7,isohp->icodes[1]); batchp+=strlen((const char *)batchp);
          sprintf((char *)batchp,"H%d 1 %c",ldrtag+8,isohp->icodes[2]); batchp+=strlen((const char *)batchp);
          sprintf((char *)batchp,"H%d 1 %c",ldrtag+17,isohp->forusr[0]); batchp+=strlen((const char *)batchp);
          sprintf((char *)batchp,"H%d 1 %c",ldrtag+18,isohp->forusr[1]); batchp+=strlen((const char *)batchp);
          sprintf((char *)batchp,"H%d 1 %c",ldrtag+19,isohp->forusr[2]); batchp+=strlen((const char *)batchp);
       }
   }    
#endif /* MARC_BEFORE_20090311 */

    isodp=(ISODSTRU *)(isi_buffin+ISOHSIZE);
    for (fld=1; fld <= nvf; fld++, isodp++) {

        tagfld=iso_getval((char *)isodp->tagfld,LENTAG);
        lenfld=iso_getval((char *)isodp->lenfld,LENLEF);
        locfld=iso_getval((char *)isodp->locfld,LENSCP);

#if ENABDIR
 printf("fld %d - tagfld=%d lenfld=%d locfld=%d  '",
  fld,tagfld,lenfld,locfld);
#if ENABFLD
 for (p=isi_buffin+basead+locfld, n=lenfld; n--; p++) {
  c= *p;
  printf("%c",c);
 }
#endif
 printf("'\n");
#endif
        n=lenfld-1;
            sprintf((char *)batchp,(CONST char *)"H%d %d ",tagfld,n);  batchp+=strlen((CONST char *)batchp);
        memmove(batchp,isi_buffin+basead+locfld,n); batchp+=n;
    }
    *batchp = '\0';

    /* get an empty record and add the fields */
#if CICPP
#if 0
    recp->xrecord(crecp->recdbxp->dbxname,crecp->recmfp->m0.m0nxtmfn);
#else
    memset(MFX,0x00,LEADER);
    MFRmfn=1L; MFRmfrl=MFRbase=LEADER;
    RECtype=TYPEMFR;
#endif /* 0 */
#else
    record(irec,VRDBname(crec),VMF0nxtmfn(crec));
#endif /* CICPP */
    RECrc=RCNORMAL;
    MFRstatus=ACTIVE;                       /* PDEL status is DELETED */
#if CICPP
     if ((p=(unsigned char *)recp->xfldupdat((char *)buffup)) != NULL)
#else
     if ((p=fldupdat(irec,buffup)) != NULL)
#endif /* CICPP */
        fatal( (char *)p);
    }
    else iso_rclos();                  /* End of File */

    /* deallocate working area */ /* hb/aot 20010606 */
#if CICPP
    delete[] buffup;
#else
    FREE(buffup);
#endif /* CICPP */

    return (n1);
}

#if CICPP
int ISO_2709 :: xiso_loadbuff(int opt)
#else
int iso_loadbuff(opt)
int opt;
#endif /* CICPP */
{
    int n,isolen,k,j,kj;
#if CRLF                                                /* CRLF */
    int eorlen = 2;
#else                                                   /* LF */
    int eorlen = 1;
#endif
    static char *nextbufp = NULL;
#if TRACISI
printf("0. lrecb,lrfix,isfix=%d,%d,%d\n",isi_lrecb,isi_lrfix,isi_isfix);
#endif
    if (isi_isfix == (-1)) eorlen = 0;  /* AOT, 10/06/2005 */
    if (opt == 0) {
    n=CIREAD(isi_fd,isi_buffin,LENLEN);
    if (n == 0) return(EOF);
    if (n != LENLEN) fatal( "iso_loadbuff/LENLEN");
#if TRACISI
isi_buffin[n]='\0';
printf("1. '%s'\n",isi_buffin);
#endif
    }
    else {
    if (!nextbufp)
        return(EOF);
#if PC
    if (*nextbufp == 0x1A) return(EOF);
#endif
    memmove(isi_buffin,nextbufp,LENLEN);
#if TRACISI
isi_buffin[LENLEN]='\0';
printf("2. '%s'\n",isi_buffin);
#endif
    }
    isolen=iso_getval((char *)isi_buffin,LENLEN);
    if (isi_lrecb)                                      /* BINARY */
    k = (isolen+isi_lrecb-1)/isi_lrecb * isi_lrecb;
    else                                                /* [CR]LF */
    k = isolen + (isolen+isi_lrfix-1)/isi_lrfix*eorlen;
    kj=k;
    if (isi_isfix > 0) {  /* AOT, 10/06/2005 */
    j = isi_lrfix - (isolen % isi_lrfix);
    if (j == isi_lrfix) j=0;
    kj = k+j;
#if TRACISI
printf("isolen,k,j,kj=%d,%d,%d,%d\n",isolen,k,j,kj);
#endif
    }
    if (isolen < ISOHSIZE) {
    fprintf(stderr,"*** iso #%"_LD_"  -> len=%d\n",isi_count,isolen);
    fatal( "iso_loadbuff/isolen/ISOHSIZE");
    }
    if (kj+LENLEN > ISIBSIZ) {
    fprintf(stderr,"*** iso #%"_LD_"  -> len=%d\n",isi_count,isolen);
    fatal( "iso_loadbuff/isolen/overflow");
    }
    n=CIREAD(isi_fd,isi_buffin+LENLEN,kj);
    if (n == kj) {
    nextbufp=(char *)isi_buffin+kj;
    }
    else {
#if CRLF
    if ( LENLEN+n == kj || (LENLEN+n == kj+1 && isi_buffin[kj] == 0x1A) )
#else
    if ( LENLEN+n == kj )
#endif
        ;
    else {
        fprintf(stderr,
        "*** iso #%"_LD_"  -> %d n=%d kj=%d \n",isi_count,isolen,n,kj);
        fatal( "iso_loadbuff/kj");
    }
    nextbufp=NULL;
    n=kj;
    }
#if TRACISI
printf("n=%d  \n",n);
#endif

    if (!isi_lrecb) {           /* Not BINARY */
    for (k=isi_lrfix; k < n; )
#if CRLF
        if (isi_buffin[k] == '\r') {
        if (isi_buffin[k+1] == '\n') {
            j=n-k-2;
#if TRACISI
printf("n=%d k=%d moving %d \n",n,k,j);
#endif
            memmove(isi_buffin+k,isi_buffin+k+2,j);
            k+=isi_lrfix; n-=2;
        }
        }
#else
        if (isi_buffin[k] == '\n') {
        j=n-k-1;
        memmove(isi_buffin+k,isi_buffin+k+1,j);
        k+=isi_lrfix; n-=1;
        }
#endif
        else
        break;
    }
    return(isolen);
}

#if CICPP
int ISO_2709 :: xiso_getval(char *p,
                int   n)
#else
int iso_getval(p,n)                                                /*
-----------                                                     */
char *p;
int n;
#endif /* CICPP */
{
    unsigned char *q;
    int loop,result;
    for (result=0, q=(unsigned char *)p, loop=n; loop--; q++) {
    if (!(isdigit(*q)))
        fatal( "iso_getval");
    result=result*10+(*q-'0');
    }
    return(result);
}
#endif /* GEN_ISI */

#if CICPP
int ISO_2709 :: xiso_rclos(void)
#else
#if ANSI
int iso_rclos(void)
#else
int iso_rclos()
#endif
#endif /* CICPP */
{
    int rc;
    /* check previous iso_open() */
    if (!isi_buffin || isi_fd <= 0)
    fatal("iso_rclos/creat");
#if CICPP
    delete[] isi_buffin;
#else
    FREE(isi_buffin);
#endif /* CICPP */
    isi_buffin = NULL;
    rc = CLOSE(isi_fd);         isi_fd = 0;
    /* reset count */
    isi_count = 0L;
    return(rc);
}
