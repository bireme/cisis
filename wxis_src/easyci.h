/* ---------------------------------------------------------------- EASYCI.H */

/* ----------------------------------------------------------------- defines */
#define ECI_FIELD_UPDATE_SIZE           64              /* field update auxiliary size */
#define ECI_LOCK_RETRIES                (8*KBYTE)   /* lock retry quantity */
/* ------------------------------------------------------------- enumeration */
typedef enum {
    ECI_ERROR_OK,
    ECI_ERROR_ACTAB,
    ECI_ERROR_ALLOC,
    ECI_ERROR_EOF,
    ECI_ERROR_FLDUPD,
    ECI_ERROR_IMPORT,
    ECI_ERROR_MAXNREC,
    ECI_ERROR_MAXNTRM,
    ECI_ERROR_UCTAB,
    ECI_ERROR_UNLOCK,
    ECI_ERROR_QTT
} ECI_ERROR;                /* error codes */
/* -------------------------------------------------------------- prototypes */
BOOLEAN     eci_export_hline    (FILE *export_fp,LONGX idx);
BOOLEAN     eci_export_iso2709(FILE *export_fp,LONGX idx,BOOLEAN writeCR);
BOOLEAN     eci_export_iso2709_marc(FILE *export_fp,LONGX idx);
BOOLEAN     eci_export_vline    (FILE *export_fp,LONGX idx);
char       *eci_field           (LONGX idx,int tag,int occ,char *buff);
ECI_ERROR   eci_field_AN        (EFC_ERROR *err,LONGX idx,int tag,LONGX num);
ECI_ERROR   eci_field_AT        (EFC_ERROR *err,LONGX idx,int tag,char *text);
ECI_ERROR   eci_field_copy      (EFC_ERROR *err,BOOLEAN del,LONGX to_idx,int to_tag,LONGX from_idx,int from_tag);
ECI_ERROR   eci_field_D         (EFC_ERROR *err,LONGX idx,int tag);
ECI_ERROR   eci_field_DO        (EFC_ERROR *err,LONGX idx,int tag,int occ);
ECI_ERROR   eci_field_ML        (EFC_ERROR *err,LONGX idx,int tag,char *list);
ECI_ERROR   eci_field_MN        (EFC_ERROR *err,LONGX idx,int tag,LONGX num);
ECI_ERROR   eci_field_MT        (EFC_ERROR *err,LONGX idx,int tag,char *text);
ECI_ERROR   eci_field_update    (EFC_ERROR *err,LONGX idx,char *buff);
ECI_ERROR   eci_fmt_all         (LONGX idx,char *buff);
ECI_ERROR   eci_import_hvline   (EFC_ERROR *err,FILE *import_fp,LONGX idx,char *buff);
ECI_ERROR   eci_import_rline    (EFC_ERROR *err,FILE *import_fp,char delimiter,LONGX idx,char *buff);
ECI_ERROR   eci_import_iso2709(EFC_ERROR *err,FILE *import_fp,LONGX idx,char *buff);
ECI_ERROR   eci_import_iso2709_marc(EFC_ERROR *err,FILE *import_fp,LONGX idx,char *buff);
LONGX        eci_last_mfn        (EFC_ERROR *err,char *db);
BOOLEAN     eci_lock            (LONGX idx,char *db,LONGX mfn,int lock_type);
LONGX        eci_rec_active      (LONGX idx);
void        eci_rec_copy        (LONGX to,LONGX from);
void        eci_rec_free        (LONGX idx);
LONGX        eci_rec_new         (EFC_ERROR *err,LONGX mfrl);
ECI_ERROR   eci_set_actab       (EFC_ERROR *err,char *p);
ECI_ERROR   eci_set_uctab       (EFC_ERROR *err,unsigned char *p);
char       *eci_sub_field       (char subfld_char,char *buff);
void        eci_trm_free        (LONGX idx);
LONGX        eci_trm_new         (EFC_ERROR *err);
char       *eci_uctab           (char *buff);
ECI_ERROR   eci_unlock          (LONGX idx,LONGX mfn,int lock_type);
