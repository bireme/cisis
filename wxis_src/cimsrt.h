/* ---------------------------------------------------------------- CIMSRT.H */

/* -------------------------------------------------------------- prototypes */
void cms_getkey(char *buff,
                     LONGX buff_size,
                LONGX idx,
                     LONGX mfn,
                LONGX keysize,
                BOOLEAN fmtgen,
                FMT_CODE *pgmp,
                LONGX keyField);
void cms_sort_list(LONGX regsin,
                  char *tbuf,
                  char *ibuf,
              LONGX buf_size,
              LONGX idx,
                  LONGX keysize,
              BOOLEAN fmtgen,
              FMT_CODE *pgmp,
              LONGX keyField);
BOOLEAN cms_sort(char *db,
                  LONGX rec_size,
                      LONGX parmkeylen,
                  FMT_CODE *fmtcode,
                  BOOLEAN fmtgen,
                  BOOLEAN parmmfn,
                 LONGX keyField);

