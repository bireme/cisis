/* ----------------------------------------------------------------- CIENV.H */

/* ----------------------------------------------------------------- version */
#define STATIC_ENVB            (!WWWISIS)
/* ----------------------------------------------------------------- defines */
#define FMTFILEID              '@'
#define SUBFIELD_ID            '^'
#define SUBFIELD_LITERAL       '\\'
#define NCAUXBUFF              30L
#define NCENVBUFF              MAXMFRL
#define NCTOTENVB              (NCENVBUFF+NCAUXBUFF)
/* -------------------------------------------------------------- prototypes */
#if ANSI
UCHAR  *env_strtab    (UCHAR *s);
BOOLEAN env_getfile   (UCHAR *format_name,UCHAR *b,int max,UCHAR *ignore);
BOOLEAN env_fmtfile   (UCHAR *format_name,UCHAR *b,int max);
long    env_creatrec  (long crec,long irec);
void    env_workrec   (long idx,long mfrl);
void    env_copyrec   (long torec,long fromrec);
int     env_recupd    (long irec,UCHAR *db);
int     env_recdel    (long irec);
long    env_recall    (long nbytes);
void    env_recdeall  (long idx);
UCHAR  *env_getfld    (UCHAR *p,long idx,int tag,int occ,int nc);
UCHAR  *env_subfld    (UCHAR *to,UCHAR *fld,UCHAR *subf);
UCHAR  *env_getsubfld (UCHAR *p,long idx,int tag,int occ,UCHAR *sf,int nc);
UCHAR  *env_allocfld  (long idx,int tag,int occ);
BOOLEAN env_getnumber (int *value,long idx,int tag,int occ,UCHAR *subfld);
BOOLEAN env_getlong   (long *value,long idx,int tag,int occ,UCHAR *subfld);
UCHAR  *env_delfld    (long idx,int tag,int occ);
UCHAR  *env_addfld    (long idx,int tag,UCHAR *p,long n);
UCHAR  *env_modfld    (long idx,int tag,UCHAR *p,long n);
long    env_trmall    (long nbytes);
void    env_trmdeall  (long idx);
#else
UCHAR  *env_strtab    ();
BOOLEAN env_fmtfile   ();
long    env_creatrec  ();
void    env_workrec   ();
void    env_copyrec   ();
int     env_recupd    ();
int     env_recdel    ();
long    env_recall    ();
void    env_recdeall  ();
UCHAR  *env_getfld    ();
UCHAR  *env_subfld    ();
UCHAR  *env_getsubfld ();
UCHAR  *env_allocfld  ();
BOOLEAN env_getnumber ();
BOOLEAN env_getlong   ();
UCHAR  *env_delfld    ();
UCHAR  *env_addfld    ();
UCHAR  *env_modfld    ();
long    env_trmall    ();
void    env_trmdeall  ();
#endif
