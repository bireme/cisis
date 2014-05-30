/* ---------------------------------------------------------------- EASYCI.C */

/* /////////////////////////////////////////////////////////////////////////

   [ Version 1.0 ]
   28.Oct.1998 - Cisis facilities.

   [ Version 1.1 ]
   02.Feb.1999 - "eci_field_DO": delete field occurrence.

   ///////////////////////////////////////////////////////////////////////// */

/* ---------------------------------------------------------- C HEADER FILES */
#include <string.h>
#include <ctype.h>
/* #include <mem.h> */
/* ------------------------------------------------------------ HEADER FILES */
#include "../cisis.h"
#include "easyfc.h"
#include "easyci.h"
/* --------------------------------------------------------------- externals */
extern RECSTRU *vrecp[];                /* cirun.h */
extern TRMSTRU *vtrmp[];                /* cirun.h */
extern int dbxsleep;                        /* cidbx.c */

/* ============================================================= eci_rec_new */
LONGX eci_rec_new(EFC_ERROR *err,        /* error structure */
                      LONGX mfrl)            /* master file record lenght */
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Find last unused record index, if unavailable: return error
   2. Allocate found record index
   3. Return record index

   1.0 - 28.Oct.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
    LONGX idx;                   /* auxiliary find index */

   /* 1 */
   for (idx = maxnrec-1; vrecp[idx]; idx--)
        if (!idx) {
        efc_error(err,ECI_ERROR_MAXNREC,NULL);
        return -1L;
      }

   /* 2 */

   recallok(idx,mfrl);

   /* 3 */
   return idx;
}
/* ========================================================== eci_rec_active */
LONGX eci_rec_active(LONGX idx)       /* record index */
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Set active and normal flags
   2. Return record index

   1.0 - 28.Oct.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */

   /* 1 */
   VMFRstatus(idx) = ACTIVE;
   VRECrc(idx) = RCNORMAL;
   VMFRmfn(idx) = 1L;
   VRECtype(idx) = TYPEMFR;
   VMFRmfrl(idx) = VMFRbase(idx) = LEADER;

   /* 2 */
   return idx;
}
/* ============================================================ eci_rec_copy */
void eci_rec_copy(LONGX to,      /* target record index */
                       LONGX from)   /* source record index */
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Copy record index content

   1.0 - 28.Oct.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */

   /* 1 */
    VRECrc(to) = VRECrc(from);
    memmove(VMFX(to),VMFX(from),VMFRmfrl(from));
}
/* ============================================================ eci_rec_free */
void eci_rec_free(LONGX idx) /* record index */
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Free record structure
   2. Decrease record index count

   1.0 - 28.Oct.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */

   /* 1 */
    if (!vrecp[idx]) return;
    vrecp[idx] = efc_free(vrecp[idx]);

   /* 2 */
    nrecs--;
}
/* ============================================================= eci_trm_new */
LONGX eci_trm_new(EFC_ERROR *err)        /* error structure */
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Find last unused term index, if unavailable: return error
   2. Return term index

   1.0 - 05.Nov.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
    LONGX idx;                   /* auxiliary find index */

   /* 1 */
   for (idx = maxntrm-1; vtrmp[idx]; idx--)
        if (!idx) {
        efc_error(err,ECI_ERROR_MAXNTRM,NULL);
        return -1L;
      }

   /* 2 */
   return idx;
}
/* ============================================================ eci_trm_free */
void eci_trm_free(LONGX idx) /* term index */
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Free term structure
   2. Decrease term index count

   1.0 - 05.Nov.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */

   /* 1 */
    if (!vtrmp[idx]) return;
    vtrmp[idx] = efc_free(vtrmp[idx]);

   /* 2 */
    ntrms--;
}
/* ============================================================ eci_last_mfn */
LONGX eci_last_mfn(EFC_ERROR *err,   /* error structure */
                       char *db)            /* data base name */
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Allocate a control record
   2. Access the control record, and the last MFN
   3. Garbage collector
   4. Return the last MFN

   1.0 - 04.Nov.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
    LONGX idx;           /* control record index */
   LONGX last_mfn;       /* last MFN */

    /* 1 */
   idx = eci_rec_new(err,sizeof(M0STRU));
   if (idx < 0L) return idx;

    /* 2 */
   record(idx,db,0L);
   last_mfn = VMF0nxtmfn(idx)-1L;

    /* 3 */
   eci_rec_free(idx);

    /* 4 */
   return last_mfn;
}
/* =============================================================== eci_field */
char *eci_field(LONGX idx,       /* record index */
                     int tag,
                int occ,
                     char *buff)        /* field update instructions */
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Get field directory index
   2. Copy field data to the buffer
   3. Return buffer pointer

   1.0 - 30.Oct.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
   int i;

   /* 1 */
   if ((i = fieldx(idx,tag,occ)) < 0) return NULL;

   /* 2 */
   strncpy(buff,VFIELDP(idx,i),VDIRlen(idx,i));
   *(buff+VDIRlen(idx,i)) = '\0';

   /* 3 */
   return buff;
}
/* =========================================================== eci_sub_field */
char *eci_sub_field(char subfld_char,   /* subfield character */
                    char *buff)           /* field buffer */
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1.

   1.0 - 10.Nov.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
    char upper_subfld_char;
    char *p;
   char *q;

   /* 1 */
   if (!buff) return NULL;
   upper_subfld_char = toupper(subfld_char);

   /* 2 */
   for (p = buff; *p; p++) {
    if (*p != '^') continue;
      if (*(p+1) == subfld_char || *(p+1) == upper_subfld_char) break;
   }
   if (!*p) return NULL;
   p++; p++;

   /* 3 */
   for (q = buff; *p; p++) {
    if (*p == '^') break;
    *q++ = *p;
   }
   *q = '\0';

   /* 4 */
   return buff;
}
/* ======================================================== eci_field_update */
ECI_ERROR eci_field_update(EFC_ERROR *err,/* error structure */
                           LONGX idx,       /* record index */
                           char *buff)     /* field update instructions */
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Field update

   1.0 - 28.Oct.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
   char *error_p;   /* error pointer */

   /* 1 */
   error_p = fldupdat(idx,buff);
   return error_p ? efc_error(err,ECI_ERROR_FLDUPD,error_p) : ECI_ERROR_OK;
}
/* ============================================================= eci_field_D */
ECI_ERROR eci_field_D(EFC_ERROR *err,   /* error structure */
                      LONGX idx,          /* record index */
                      int tag)           /* field tag */
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Fill the buffer with delete field instruction
   2. Field update

   1.0 - 28.Oct.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
    char buff[ECI_FIELD_UPDATE_SIZE+1]; /* auxiliary field update buffer */

   /* 1  */
   if (tag) sprintf(buff,"D%d",tag);
   else sprintf(buff,"D*");

   /* 2 */
   return eci_field_update(err,idx,buff);
}
/* ============================================================ eci_field_DO */
ECI_ERROR eci_field_DO(EFC_ERROR *err,  /* error structure */
                       LONGX idx,        /* record index */
                       int tag,         /* field tag */
                       int occ)         /* field ocurrence */
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Fill the buffer with delete field instruction
   2. Field update

   1.1 - 08.Feb.1999
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
    char buff[ECI_FIELD_UPDATE_SIZE+1]; /* auxiliary field update buffer */

   /* 1  */
   if (tag)
    if (occ) sprintf(buff,"D%d/%d",tag,occ);
    else sprintf(buff,"D%d",tag,occ);
   else sprintf(buff,"D*");

   /* 2 */
   return eci_field_update(err,idx,buff);
}
/* ============================================================ eci_field_AN */
ECI_ERROR eci_field_AN(EFC_ERROR *err,  /* error structure */
                              LONGX idx,         /* record index */
                          int tag,          /* field tag */
                      LONGX num)         /* field value */
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Fill the buffer with add field instruction
   2. Field update

   1.0 - 28.Oct.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
    char buff[ECI_FIELD_UPDATE_SIZE+1];     /* auxiliary field update buffer */

   /* 1 */
    sprintf(buff,"A%d|%"_LD_"|",tag,num);

   /* 2 */
   return eci_field_update(err,idx,buff);
}
/* ============================================================ eci_field_MN */
ECI_ERROR eci_field_MN(EFC_ERROR *err,  /* error structure */
                              LONGX idx,         /* record index */
                          int tag,          /* field tag */
                      LONGX num)         /* field value */
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Field delete, field add

   1.0 - 28.Oct.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
    ECI_ERROR error_code;   /* error code */

   /* 1 */
    error_code = eci_field_D(err,idx,tag);
    if (!error_code) error_code = eci_field_AN(err,idx,tag,num);
   return error_code;
}
/* ============================================================ eci_field_AT */
ECI_ERROR eci_field_AT(EFC_ERROR *err,  /* error structure */
                       LONGX idx,         /* record index */
                       int tag,          /* field tag */
                       char *text)       /* field content */
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Allocate space for field update
   2. Fill the buffer with add field instruction
   3. Field update
   4. Garbage collector
   5. Return error code

   1.0 - 28.Oct.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
    char *buff;                 /* auxiliary string buffer pointer */
   LONGX buff_size;          /* string buffer size needed */
    ECI_ERROR error_code;   /* error code */

   /* 1 */
   buff_size = strlen(text);
    buff = efc_new(ECI_FIELD_UPDATE_SIZE+buff_size+1);
   if (!buff) return efc_error(err,ECI_ERROR_ALLOC,text);

   /* 2 */
    sprintf(buff,"H %d %"_LD_" %s",tag,buff_size,text);

   /* 3 */
   error_code = eci_field_update(err,idx,buff);

   /* 4 */
    efc_free(buff);

   /* 5 */
   return error_code;
}
/* ============================================================ eci_field_MT */
ECI_ERROR eci_field_MT(EFC_ERROR *err,  /* error structure */
                       LONGX idx,         /* record index */
                       int tag,          /* field tag */
                       char *text)       /* field content */
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Field delete, field add

   1.0 - 28.Oct.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
    ECI_ERROR error_code;   /* error code */

   /* 1 */
    error_code = eci_field_D(err,idx,tag);
    if (!error_code) error_code = eci_field_AT(err,idx,tag,text);
   return error_code;
}
/* ============================================================ eci_field_ML */
ECI_ERROR eci_field_ML(EFC_ERROR *err,  /* error structure */
                       LONGX idx,         /* record index */
                       int tag,          /* field tag */
                       char *list)       /* field list content */
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Delete field
   2. Split each line as a new field occurrence
   3. Skip end of line characters
   4. Find next end of line position
   5. Keep end of line character, field update, restore end of line character
   6. Return error code

   1.0 - 28.Oct.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
    char *p;                        /* auxiliary string buffer pointer */
    char *q;                        /* auxiliary string buffer pointer */
   char back_char;         /* backup line end character */
    ECI_ERROR error_code;   /* error code */

   /* 1 */
    error_code = eci_field_D(err,idx,tag);
   if (error_code) return error_code;

   /* 2 */
   for (p = list; *p; p++) {

    /* 3 */
        if (*p == '\r' || *p == '\n') continue;

    /* 4 */
        for (q = p; *p && *p != '\r' && *p != '\n'; p++);

    /* 5 */
        back_char = *p;
        *p = '\0';
        error_code = eci_field_AT(err,idx,tag,q);
      *p = back_char;
    if (error_code || !*p) break;

   } /* for */

    /* 6 */
    return error_code;
}
/* ========================================================== eci_field_copy */
ECI_ERROR eci_field_copy(EFC_ERROR *err,    /* error structure */
                         BOOLEAN del,       /* delete previous value */
                         LONGX to_idx,       /* source record index */
                         int to_tag,        /* source field tag */
                         LONGX from_idx, /* target record index */
                         int from_tag)      /* target field tag */
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    0. Check invalid copy: cannot be copied onto itself
    1. Delete target field
   2. Loop all source field occurrences
   3. Get field directory index, check end of occurrences
   4. Allocate work buffer
   5. Copy source field to buffer
   6. Field add buffer to target field
   7. Return error code

   1.0 - 05.Nov.1998
   1.1 - 25.Feb.1999
    1. delete flag
   1.2 - 17.Jun.1999
    1. copy check
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
    int occ;                               /* auxiliary occurrence index */
    int i;                                   /* auxiliary field directory index */
    char *buff;                            /* auxiliary string buffer pointer */
    ECI_ERROR error_code = ECI_ERROR_OK;    /* error code */

   /* 0 */
   if (to_idx == from_idx && to_tag == from_tag) {
        return efc_error(err,ECI_ERROR_FLDUPD,"cannot be copied onto itself");
   }

   /* 1 */
   if (del) {
        error_code = eci_field_D(err,to_idx,to_tag);
        if (error_code) return error_code;
   }

   /* 2 */
   for (occ = 1; ; occ++) {

    /* 3 */
    i = fieldx(from_idx,from_tag,occ);
      if (i < 0) break;

    /* 4 */
      buff = efc_new(VDIRlen(from_idx,i)+1);
      if (!buff) return efc_error(err,ECI_ERROR_ALLOC,NULL);

    /* 5 */
       strncpy(buff,VFIELDP(from_idx,i),VDIRlen(from_idx,i));
    *(buff+VDIRlen(from_idx,i)) = '\0';

    /* 6 */
        error_code = eci_field_AT(err,to_idx,to_tag,buff);
      efc_free(buff);
        if (error_code) break;

    } /* for */

    /* 7 */
   return error_code;
}
/* ========================================================== eci_set_uctab */
ECI_ERROR eci_set_uctab(EFC_ERROR *err,/* error structure */
                        unsigned char *p)           /* upper case string buffer pointer */
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
   1.0 - 28.Oct.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
   int i;            /* auxiliary table index */
   int conv_code;    /* convertion code */

    /* [1.0] set alphabetic char table */
   for (i = 0; *p; p++) {

      /* [1.0] skip non numeric characters */
      if (!isdigit(*p)) continue;

      /* [1.0] check table limit */
      conv_code = atoi(p);
      if (conv_code < 0 || conv_code > 255)
        return efc_error(err,ECI_ERROR_UCTAB,p);

      /* [1.0] set word case table */
      isisuctab[i++] = (unsigned char)conv_code;
      if (i > 255) break;

      /* [1.0] skip numeric characters */
      while (isdigit(*p)) p++;

   } /* for */

    /* [1.0] no error */
   return ECI_ERROR_OK;
}
/* ========================================================== eci_set_actab */
ECI_ERROR eci_set_actab(EFC_ERROR *err,/* error structure */
                        char *p)            /* alphabetic character string buffer pointer */
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
   1.0 - 28.Oct.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
   int i;            /* auxiliary table index */

    /* [1.0] no table entry */
   if (!*p) return ECI_ERROR_OK;

    /* [1.0] reset alphabetic char table */
   memset(isiswctab,0x00,256);

    /* [1.0] set alphabetic char table */
   for (isiswctot = 0; *p; p++) {

      /* [1.0] skip non numeric characters */
      if (!isdigit(*p)) continue;

      /* [1.0] check table limit */
      i = atoi(p);
      if (i < 0 || i > 255) return efc_error(err,ECI_ERROR_ACTAB,p);

      /* [1.0] set word case table */
      if (!isiswctab[i]) isiswctot++;
      isiswctab[i] = 1;

      /* [1.0] skip numeric characters */
      while (isdigit(*p)) p++;

   } /* for */

    /* [1.0] no error */
   return ECI_ERROR_OK;
}
/* =============================================================== eci_uctab */
char *eci_uctab(char *buff) /* string buffer pointer */
{
   /* 03.Jul.2000
    WARNING: it has to be unsigned char
   */
   unsigned char *p;            /* auxiliary string buffer pointer */

    /* [1.0] convert all characters according the upper case table */
   if (buff)
      for (p = buff; *p; p++) *p = isisuctab[*p];

    /* [1.0] return original buffer pointer */
   return buff;
}
/* ============================================================= eci_fmt_all */
ECI_ERROR eci_fmt_all(LONGX idx,         /* record index */
                      char *buff)       /* output buffer */
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
   1.0 - 28.Oct.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */

   /* 1 */
    /* [1.0] fill a buffer with all record fields */
   char *p = buff;      /* auxiliary buffer */
   int flddir;              /* field directory index */
   char *fld;

    /* [1.0] fill the buffer with the MFN */
   sprintf(p,"mfn=%"_LD_"<BR>\n",VMFRmfn(idx));
   p += strlen(p);

   /* [1.0] loop for all fields */
   for (flddir = 0; flddir < VMFRnvf(idx); flddir++) {
      sprintf(p,"%5u %c",VDIRtag(idx,flddir),OPENFCHR);
      p += strlen(p);

      sprintf(p,"%-.*s%c<BR>\n",VDIRlen(idx,flddir),VFIELDP(idx,flddir),CLOSFCHR);
      p += strlen(p);
   } /* for */

    /* [1.0] set string terminator character */
   *p = '\0';

    /* [1.0] return OK */
   return ECI_ERROR_OK;
}
/* ------------------------------------------------------ eci_export_iso2709 */
#define ISO_LEADER_LENGHT     24
#define ISO_TAG_LENGHT        3
#define ISO_FLDLEN_LENGHT     4
#define ISO_FLDLOC_LENGHT     5
#define ISO_DIRECTORY_LENGHT  (ISO_TAG_LENGHT + ISO_FLDLEN_LENGHT + ISO_FLDLOC_LENGHT)
#define ISO_FLDSEP            '#'
#define ISO_MARCFLDSEP        30
#define ISO_FLDSEP_LENGHT     1
#define ISO_RECSEP            '#'
#define ISO_MARCRECSEP        29
#define ISO_RECSEP_LENGHT     1
#define ISO_LINE_LENGHT       80
#define ISO_MARC_TAG          3000

char ldrtag(LONGX idx,
            int tag) {
    char *p = "0";
    int flddir = 0;
    int nvf = VMFRnvf(idx);

    for (flddir = 0; flddir < nvf; flddir++) {
        if (tag == VDIRtag(idx,flddir)) {
            if (VDIRlen(idx,flddir) > 0) {
                p = VFIELDP(idx,flddir);
            }
            break;
        }
    }

    return *p;
}

BOOLEAN eci_export_iso2709_marc(FILE *export_fp,    /* export file pointer */
                                LONGX idx)           /* record index */

{
   char *buff;                  /* iso formed record buffer */
   char *p;                     /* auxiliary pointer */
   LONGX total_dir;              /* directory quantity of bytes */
   LONGX base_address;           /* base address quantity of bytes */
   LONGX total_data;             /* data quantity of bytes */
   LONGX total_record;           /* record quantity of bytes */
   LONGX location_offset;        /* location offset from the first data */
   int tag;                     /* field number */
   int flddir;                  /* field directory index */
   int skip_bytes;              /* auxiliary print quantity of bytes */
   int nvf;                     /* number of variable fields */

   char status[1];   /* Estado del registro */
   char icodes[4];   /* 0=Tipo de registro; 1=Nivel bibliográfico; 2=Tipo de control; 3=unused */
   char forusr[3];   /* 0=Nivel de codificación; 1=Forma de catalogación; 2=Linked requirement */

   /* .......................................... calculate quantity of bytes */
   for (flddir = 0,nvf=0,total_data = 0L; flddir < VMFRnvf(idx); flddir++) {
      tag = VDIRtag(idx,flddir);
      if (tag >= ISO_MARC_TAG) if (tag <= ISO_MARC_TAG+ISOHSIZE) continue;
      total_data += VDIRlen(idx,flddir) + ISO_FLDSEP_LENGHT;
      nvf++;
   }
   total_dir = nvf * ISO_DIRECTORY_LENGHT;
   base_address = ISO_LEADER_LENGHT + total_dir + ISO_FLDSEP_LENGHT;
   total_record = base_address + total_data + ISO_RECSEP_LENGHT;

   /* ................................................ allocate buffer space */
   buff = (char *)ALLOC((ALLOPARM)(total_record+1));
   if (!buff) return FALSE;

   /* ............................................................... leader */
   status[0] = ldrtag(idx, ISO_MARC_TAG + 5);
   icodes[0] = ldrtag(idx, ISO_MARC_TAG + 6);
   icodes[1] = ldrtag(idx, ISO_MARC_TAG + 7);
   icodes[2] = ldrtag(idx, ISO_MARC_TAG + 8);
   forusr[0] = ldrtag(idx, ISO_MARC_TAG + 17);
   forusr[1] = ldrtag(idx, ISO_MARC_TAG + 18);
   forusr[2] = ldrtag(idx, ISO_MARC_TAG + 19);

   p = buff;

   sprintf(p,"%05"_LD_"%c%c%c%c000%05"_LD_"%c%c%c%d%d00",total_record,status[0],
            icodes[0],icodes[1],icodes[2], base_address,
            forusr[0],forusr[1],forusr[2],
            ISO_FLDLEN_LENGHT,ISO_FLDLOC_LENGHT);

   p += ISO_LEADER_LENGHT;

   /* ............................................................ directory */
   for (flddir = 0,location_offset = 0L; flddir < VMFRnvf(idx); flddir++) {
      tag = VDIRtag(idx,flddir);
      if (tag >= ISO_MARC_TAG) if (tag <= ISO_MARC_TAG+ISOHSIZE) continue;
      if (tag == 1000) tag = 1;
      if (tag > 999) tag = tag % 1000;
      sprintf(p,"%-03.3u%04u%05lu",tag,VDIRlen(idx,flddir)+ISO_FLDSEP_LENGHT,
        location_offset);
      p += ISO_DIRECTORY_LENGHT;
      location_offset += VDIRlen(idx,flddir) + ISO_FLDSEP_LENGHT;
   } /* for */

   /* .......................................................... data fields */
   *p++ = ISO_MARCFLDSEP;
   for (flddir = 0; flddir < VMFRnvf(idx); flddir++) {
      tag = VDIRtag(idx,flddir);
      if (tag >= ISO_MARC_TAG) if (tag <= ISO_MARC_TAG+ISOHSIZE) continue;
      memmove(p,VFIELDP(idx,flddir),VDIRlen(idx,flddir));
      p += VDIRlen(idx,flddir);
      *p++ = ISO_MARCFLDSEP;
   } /* for */

   /* ..................................................... record separator */
   *p++ = ISO_MARCRECSEP;
   *p = '\0';

   /* .......................................................... show buffer */
   fprintf(export_fp,"%s", buff);

   /* ......................................................... free buffers */
   efc_free(buff);

   return TRUE;
}

/* ====================================================== eci_export_iso2709 */
BOOLEAN eci_export_iso2709(FILE *export_fp, /* export file pointer */
                           LONGX idx,         /* record index */
                           BOOLEAN writeCR) /* force ASCII structure */
{
   char *buff;                  /* iso formed record buffer */
   char *p;                     /* auxiliary pointer */
   LONGX total_dir;               /* directory quantity of bytes */
   LONGX base_address;            /* base address quantity of bytes */
   LONGX total_data;              /* data quantity of bytes */
   LONGX total_record;            /* record quantity of bytes */
   LONGX location_offset;         /* location offset from the first data */
   int tag;                      /* field number */
   int flddir;                   /* field directory index */
   int skip_bytes;               /* auxiliary print quantity of bytes */

   /* .......................................... calculate quantity of bytes */
   total_dir = VMFRnvf(idx) * ISO_DIRECTORY_LENGHT;
   base_address = ISO_LEADER_LENGHT + total_dir + ISO_FLDSEP_LENGHT;
   for (flddir = 0,total_data = 0L; flddir < VMFRnvf(idx); flddir++)
      total_data += VDIRlen(idx,flddir) + ISO_FLDSEP_LENGHT;
   total_record = base_address + total_data + ISO_RECSEP_LENGHT;

   /* ................................................ allocate buffer space */
   buff = (char *)ALLOC((ALLOPARM)(total_record+1));
   if (!buff) return FALSE;

   /* ............................................................... leader */
   p = buff;
   sprintf(p,"%05"_LD_"0000000%05"_LD_"000%d%d00",total_record,base_address,
      ISO_FLDLEN_LENGHT,ISO_FLDLOC_LENGHT);
   p += ISO_LEADER_LENGHT;

   /* ............................................................ directory */
   for (flddir = 0,location_offset = 0L; flddir < VMFRnvf(idx); flddir++) {
        tag = VDIRtag(idx,flddir);
      if (tag == 1000) tag = 1;
        if (tag > 999) tag = tag % 1000;
      sprintf(p,"%-03.3u%04u%05lu",tag,VDIRlen(idx,flddir)+ISO_FLDSEP_LENGHT,
        location_offset);
      p += ISO_DIRECTORY_LENGHT;
      location_offset += VDIRlen(idx,flddir) + ISO_FLDSEP_LENGHT;
   } /* for */

   /* .......................................................... data fields */
   *p++ = ISO_FLDSEP;
   for (flddir = 0; flddir < VMFRnvf(idx); flddir++) {
      memmove(p,VFIELDP(idx,flddir),VDIRlen(idx,flddir));
      p += VDIRlen(idx,flddir);
      *p++ = ISO_FLDSEP;
   } /* for */

   /* ..................................................... record separator */
   *p++ = ISO_RECSEP;
   *p = '\0';

   /* .......................................................... show buffer */
   for (p = buff; *p; p += skip_bytes) {
      skip_bytes = fprintf(export_fp,"%-.*s",ISO_LINE_LENGHT,p);
#if !PC
      if (writeCR) fprintf(export_fp,"\r");
#endif
      fprintf(export_fp,"\n");
   }

   /* ......................................................... free buffers */
   efc_free(buff);

   return TRUE;
}
/* ======================================================== eci_export_hline */
BOOLEAN eci_export_hline(FILE *export_fp,   /* export file pointer */
                         LONGX idx)        /* current record index */
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
   1. Write MFN mark
   2. Loop all fields, write field number mark
   3. Loop all field characters, write characters, ended by a NEWLINE
   4. Write end of record mark
   5. Return no error

   1.0 - 12.Nov.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
   char *p;     /* auxiliary string buffer pointer */
   int loop1;  /* loop all fields */
   FFI loop2;  /* loop all field characters */
   int xdir;   /* field directory entry */

    /* 1 */
    fprintf(export_fp,"!mfn=%"_LD_"!\n",VMFRmfn(idx));

    /* 2 */
   for (xdir=0, loop1=VMFRnvf(idx); loop1--; xdir++) {
    fprintf(export_fp,"H %d %d ",VDIRtag(idx,xdir),VDIRlen(idx,xdir));
        /* 3 */
      for (p=VFIELDP(idx,xdir), loop2=VDIRlen(idx,xdir); loop2--; p++)
        fprintf(export_fp,"%c",*p);
      fprintf(export_fp,"\n");
   }

    /* 4 */
   fprintf(export_fp,"!eor!\n");

    /* 5 */
   return TRUE;
}
/* ======================================================== eci_export_vline */
BOOLEAN eci_export_vline(FILE *export_fp,   /* export file pointer */
                         LONGX idx)        /* current record index */
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Write MFN mark
   2. Loop all fields, write field number mark
   3. Loop all field characters, write characters, ended by a NEWLINE
   4. Write end of record mark
   5. Return no error

   1.0 - 12.Nov.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
   char *p;     /* auxiliary string buffer pointer */
   int loop1;  /* loop all fields */
   FFI loop2;  /* loop all field characters */
   int xdir;   /* field directory entry */

    /* 1 */
    fprintf(export_fp,"!mfn=%"_LD_"!\n",VMFRmfn(idx));

    /* 2 */
   for (xdir=0, loop1=VMFRnvf(idx); loop1--; xdir++) {
    fprintf(export_fp,"!v%05u!",VDIRtag(idx,xdir));
        /* 3 */
      for (p=VFIELDP(idx,xdir), loop2=VDIRlen(idx,xdir); loop2--; p++)
        fprintf(export_fp,"%c",*p);
      fprintf(export_fp,"\n");
    }

    /* 4 */
   fprintf(export_fp,"!eor!\n");

    /* 5 */
   return TRUE;
}
/* ------------------------------------------------------ eci_import_iso2709 */
#define ISO_BASEADD_OFFSET    12
#define ISO_FLDLEN_OFFSET     20
#define ISO_FLDLOC_OFFSET     21

ECI_ERROR eci_import_iso2709_marc(EFC_ERROR *err,
                                  FILE *import_fp,  /* import file pointer */
                                  LONGX idx,
                                  char *buff)
{
   char *line;                /* line pointer */
   char *p;                   /* auxiliary string buffer pointer */
   char *fldupd;              /* field update area string buffer pointer */
   LONGX linesize;             /* line size */
   LONGX total_record;         /* record quantity of bytes */
   LONGX base_address;         /* base address quantity of bytes */
   int fldlen;                /* field lenght */
   int fldloc;                /* field location */
   LONGX qtt_read;             /* quantity of bytes read */
   int tag;                   /* tag to be added */

   char status[1];   /* Estado del registro */
   char icodes[4];   /* 0=Tipo de registro; 1=Nivel bibliográfico; 2=Tipo de control; 3=unused */
   char forusr[3];   /* 0=Nivel de codificación; 1=Forma de catalogación; 2=Linked requirement */

   /* ............................................................ read line */
   line = fgets(buff,MAXMFRL,import_fp);
   if (!line) return ECI_ERROR_EOF;

   /* ......................................................... clear record */
   VRECrc(idx) = RCNORMAL;
   VMFRstatus(idx) = ACTIVE;
   VRECtype(idx) = TYPEMFR;
   if (eci_field_D(err,idx,0)) return err->code;

   /* ............................................... get leader information */
   sscanf(line,"%05"_LD_,&total_record);
   if (total_record < ISO_BASEADD_OFFSET) return efc_error(err,ECI_ERROR_IMPORT,line);

   sscanf(line+5,"%c",&status);

   p = (char *)&icodes;
   sscanf(line+6,"%c", p++);
   sscanf(line+7,"%c", p++);
   sscanf(line+8,"%c", p++);
   sscanf(line+9,"%c", p++);

   sscanf(line+ISO_BASEADD_OFFSET,"%05"_LD_,&base_address);

   p = (char *)&forusr;
   sscanf(line+17,"%c", p++);
   sscanf(line+18,"%c", p++);
   sscanf(line+19,"%c", p);

   qtt_read = 0L;

   /* ............................................ loop for all record lines */
   while (line) {

      /* ............................................ get line with no CR/LF */
      linesize = strlen(line);
      for (p = line+linesize-1; (*p == '\n' || *p == '\r') && linesize; p--,linesize--)
         *p = '\0';

      /* ............................................... check end of record */
      qtt_read += linesize;
      if (qtt_read >= total_record) break;
      line += linesize;

      /* ..................................................... get next line */
      line = fgets(line,MAXMFRL,import_fp);
      if (!line) return efc_error(err,ECI_ERROR_IMPORT,buff);

   } /* while */

   /* use the lasting buffer for field update */
   fldupd = p+2;

   /* ....................................... update marc fields */
   sprintf(fldupd,"H %d 1 %c", ISO_MARC_TAG + 5, status[0]);
   if (eci_field_update(err,idx,fldupd))
     efc_error(err,ECI_ERROR_IMPORT,NULL);

   sprintf(fldupd,"H %d 1 %c", ISO_MARC_TAG + 6, icodes[0]);
   if (eci_field_update(err,idx,fldupd))
     efc_error(err,ECI_ERROR_IMPORT,NULL);

   sprintf(fldupd,"H %d 1 %c", ISO_MARC_TAG + 7, icodes[1]);
   if (eci_field_update(err,idx,fldupd))
     efc_error(err,ECI_ERROR_IMPORT,NULL);

   sprintf(fldupd,"H %d 1 %c", ISO_MARC_TAG + 8, icodes[2]);
   if (eci_field_update(err,idx,fldupd))
     efc_error(err,ECI_ERROR_IMPORT,NULL);

   /* sprintf(fldupd,"H %d 1 %c", ISO_MARC_TAG + 9, icodes[3]);
   if (eci_field_update(err,idx,fldupd))
     efc_error(err,ECI_ERROR_IMPORT,NULL); */

   sprintf(fldupd,"H %d 1 %c", ISO_MARC_TAG + 17, forusr[0]);
   if (eci_field_update(err,idx,fldupd))
     efc_error(err,ECI_ERROR_IMPORT,NULL);

   sprintf(fldupd,"H %d 1 %c", ISO_MARC_TAG + 18, forusr[1]);
   if (eci_field_update(err,idx,fldupd))
     efc_error(err,ECI_ERROR_IMPORT,NULL);

   sprintf(fldupd,"H %d 1 %c", ISO_MARC_TAG + 19, forusr[2]);
   if (eci_field_update(err,idx,fldupd))
     efc_error(err,ECI_ERROR_IMPORT,NULL);

    /* use the lasting buffer for field update */
   fldupd = p+2;

   /* ....................................... loop for all directory entries */
   for (qtt_read = ISO_LEADER_LENGHT; qtt_read+1 < base_address; qtt_read += ISO_DIRECTORY_LENGHT) {

      /* ................................................... directory entry */
      p = buff + qtt_read;
      sscanf(p,"%03d%04d%05d",&tag,&fldlen,&fldloc);
      fldlen--;

      /* ................................................. field update data */
      sprintf(fldupd,"H %u %lu %-.*s",tag,fldlen,fldlen,buff+(base_address+fldloc));
      if (eci_field_update(err,idx,fldupd))
        efc_error(err,ECI_ERROR_IMPORT,NULL);

   } /* for */

   /* ..................................................... return exit code */
   return ECI_ERROR_OK;
}

/* ------------------------------------------------------- eci_import_iso2709 */
ECI_ERROR eci_import_iso2709(EFC_ERROR *err,
                             FILE *import_fp,      /* import file pointer */
                             LONGX idx,
                             char *buff)
{
   char *line;                /* line pointer */
   char *p;                   /* auxiliary string buffer pointer */
   char *fldupd;              /* field update area string buffer pointer */
   LONGX linesize;             /* line size */
   LONGX total_record;         /* record quantity of bytes */
   LONGX base_address;         /* base address quantity of bytes */
   int fldlen;                /* field lenght */
   int fldloc;                /* field location */
   LONGX qtt_read;             /* quantity of bytes read */
   int tag;                   /* tag to be added */

   /* ............................................................ read line */
   line = fgets(buff,MAXMFRL,import_fp);
   if (!line) return ECI_ERROR_EOF;

   /* ......................................................... clear record */
   VRECrc(idx) = RCNORMAL;
   VMFRstatus(idx) = ACTIVE;
   VRECtype(idx) = TYPEMFR;
   if (eci_field_D(err,idx,0)) return err->code;

   /* ............................................... get leader information */
   sscanf(line,"%05"_LD_,&total_record);
   if (total_record < ISO_BASEADD_OFFSET) return efc_error(err,ECI_ERROR_IMPORT,line);
   sscanf(line+ISO_BASEADD_OFFSET,"%05"_LD_,&base_address);
   qtt_read = 0L;

   /* ............................................ loop for all record lines */
   while (line) {

      /* ............................................ get line with no CR/LF */
      linesize = strlen(line);
      for (p = line+linesize-1; (*p == '\n' || *p == '\r') && linesize; p--,linesize--)
         *p = '\0';

      /* ............................................... check end of record */
      qtt_read += linesize;
      if (qtt_read >= total_record) break;
      line += linesize;

      /* ..................................................... get next line */
      line = fgets(line,MAXMFRL,import_fp);
      if (!line) return efc_error(err,ECI_ERROR_IMPORT,buff);

   } /* while */

   /* use the lasting buffer for field update */
   fldupd = p+2;

   /* ....................................... loop for all directory entries */
   for (qtt_read = ISO_LEADER_LENGHT; qtt_read+1 < base_address; qtt_read += ISO_DIRECTORY_LENGHT) {

      /* ................................................... directory entry */
      p = buff + qtt_read;
      sscanf(p,"%03d%04d%05d",&tag,&fldlen,&fldloc);
      fldlen--;

      /* ................................................. field update data */
      sprintf(fldupd,"H %u %lu %-.*s",tag,fldlen,fldlen,buff+(base_address+fldloc));
      if (eci_field_update(err,idx,fldupd))
        efc_error(err,ECI_ERROR_IMPORT,NULL);

   } /* for */

   /* ..................................................... return exit code */
   return ECI_ERROR_OK;
}
/* ------------------------------------------------------- eci_import_hvline */
ECI_ERROR eci_import_hvline(EFC_ERROR *err,
                            FILE *import_fp,       /* import file pointer */
                            LONGX idx,
                            char *buff)
{
   char *line;       /* line pointer */
   char *p;          /* auxiliary string buffer pointer */
   char *fldupd;     /* field update area string buffer pointer */
   LONGX linesize;       /* line size */
   LONGX mfn;            /* MFN */
   int tag;          /* tag to be added */

   /* ......................................................... clear record */
   VRECrc(idx) = RCNORMAL;
   VMFRstatus(idx) = ACTIVE;
   VRECtype(idx) = TYPEMFR;
   if (eci_field_D(err,idx,0)) return err->code;

   /* ............................................ loop for all record lines */
   do {

      /* ............................................ get line with no CR/LF */
      line = fgets(buff,MAXMFRL,import_fp);
      if (!line) return ECI_ERROR_EOF;

      linesize = strlen(line);
      for (p = line+linesize-1; (*p == '\n' || *p == '\r') && linesize; p--,linesize--)
         *p = '\0';

    /* use the lasting buffer for field update */
    fldupd = p+2;

      /* ............................................... check end of record */
      if (strncmp(line,"!eor!",5) == 0) break;

      /* ................................................. get record number */
      if (strncmp(line,"!mfn=",5) == 0) {
         mfn = atol(line+5);
         if (mfn < 1L) efc_error(err,ECI_ERROR_IMPORT,line);
          VMFRmfn(idx) = mfn;
         continue;
      }

      /* ........................................ get field data vline style */
      if (strncmp(line,"!v",2) == 0) {

         /* ............................................... get field number */
         line++; line++; linesize--; linesize--;
         tag = atoi(line);
         while (*line++ != '!') linesize--;

         /* .............................................. field update data */
         sprintf(fldupd,"H %u %lu %s",tag,--linesize,line);
          if (eci_field_update(err,idx,fldupd))
        efc_error(err,ECI_ERROR_IMPORT,NULL);

         continue;
      }

      /* ............................................ field data Hline style */
      if (strncmp(line,"H ",2) == 0) {
          if (eci_field_update(err,idx,line))
        efc_error(err,ECI_ERROR_IMPORT,NULL);
         continue;
      }

      /* ................................................ invalid data found */
    efc_error(err,ECI_ERROR_IMPORT,line);
      break;

   } while (line);

   /* ..................................................... return exit code */
   return ECI_ERROR_OK;
}
/* -------------------------------------------------------- eci_import_rline */
ECI_ERROR eci_import_rline(EFC_ERROR *err,
                           FILE *import_fp,        /* import file pointer */
                           char delimiter,
                           LONGX idx,
                           char *buff)
{
    char *fmt = "H %06d %06"_LD_;
   int fmt_size = 16;
   char *line;       /* line pointer */
   char *text;       /* text data pointer */
   char *p;          /* auxiliary string buffer pointer */
   int tag;             /* tag to be added */
   LONGX textSize;

   /* ......................................................... clear record */
   VRECrc(idx) = RCNORMAL;
   VMFRstatus(idx) = ACTIVE;
   VRECtype(idx) = TYPEMFR;
   if (eci_field_D(err,idx,0)) return err->code;

   /* ............................................ get line with no CR/LF */
   line = fgets(buff+fmt_size,MAXMFRL,import_fp);
    if (!line) return ECI_ERROR_EOF;

    for (p = text = line,textSize = 0L,tag = 0; *p; p++) {

        if (*p != delimiter && *p != '\n' && *p != '\r') {
            textSize++;
            continue;
      }

      tag++;

      if (textSize) {

        *p = '\0';
         sprintf(text-fmt_size,fmt,tag,textSize);
         *(text-1) = ' ';

         if (eci_field_update(err,idx,text-fmt_size))
            efc_error(err,ECI_ERROR_IMPORT,text-fmt_size);

      }

      text = p+1;
      textSize = 0;
   }

   /* ..................................................... return exit code */
   return ECI_ERROR_OK;
}
/* ========================================================== eci_lock_netws */
void eci_lock_netws(LONGX idx,       /* record index */
                    char *db)       /* database name */
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Set database structure
    2. Set lock flags

   1.0 - 08.Dec.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */

    /* 1 */
    if (!VRECdbxp(idx)) VRECdbxp(idx) = dbxstorp(db);

    /* 2 */
#if MULTI
   VRDBnetws(idx) = FULLNETS;
   dbxewlrc = 1;
   dbxfloop = ECI_LOCK_RETRIES;
   dbxwloop = ECI_LOCK_RETRIES;
   dbxiloop = ECI_LOCK_RETRIES;
   dbxsleep = 0;
#endif /* MULTI */
}
/* ================================================================ eci_lock */
BOOLEAN eci_lock(LONGX idx,         /* record index */
                 char *db,         /* database name */
                 LONGX mfn,         /* MFN */
                 int lock_type)    /* lock type */
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    1. Net work operation cisis status
   2. Get record with lock flag
   3. Restore normal flags
   4. Get record
   5, Return TRUE if it was locked, otherwise FALSE

   1.0 - 08.Dec.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */

    /* 1 */
   eci_lock_netws(idx,db);

#if MULTI
    /* 2 */
   VREClock(idx) = lock_type;
   record(idx,db,mfn);

    /* 3 */
   VREClock(idx) = NOLOCK;
   if (lock_type == EWLOCK) VRDBnetws(idx) = MONONETS;
#else /* !MULTI */
    /* 4 */
   record(idx,db,mfn);
#endif /* MULTI */

    /* 5 */
   return (BOOLEAN)(VRECrc(idx) == RCLOCK);
}
/* ============================================================== eci_unlock */
ECI_ERROR eci_unlock(LONGX idx,          /* record index */
                     LONGX mfn,           /* MFN */
                     int lock_type)      /* lock type */
{
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
   1. Net work operation cisis status
   2. Restore locked flags
   3. Unlock record
   4. Return no error

   1.0 - 08.Dec.1998
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */

    /* 1 */
   eci_lock_netws(idx,VRDBname(idx));

#if MULTI
    /* 2 */
    if (mfn > 0L) {
        VRECgdbl(idx) = (FFI)0-VMFRmfrl(idx);
        VRECgdbw(idx) = (FFI)0-VMFRmfrl(idx);
    }

    /* 3 */
    if (recunlck(idx,lock_type)) return ECI_ERROR_UNLOCK;
#endif /* MULTI */

    /* 4 */
   return ECI_ERROR_OK;
}

