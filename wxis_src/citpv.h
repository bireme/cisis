/* ----------------------------------------------------------------- CITPV.H */

/* ----------------------------------------------------------------- defines */
#ifndef UCHAR
#define UCHAR 			unsigned char
#endif /* UCHAR */
/* -------------------------------------------------------------- structures */
typedef struct {

  UCHAR    PT_Limit      [LE2+1];
  UCHAR    PT_Key        [LE2+1];
  UCHAR    PT_WorkKey    [LE2+1];
  UCHAR    PT_AuxWorkKey [LE2+1];

  UCHAR   *PT_Begin;
  UCHAR   *PT_Middle;
  UCHAR   *PT_End;

  TRMSTRU *PT_Trmp;
  LONGX     PT_iTrm;

  LONGX     PT_FirstChar;  /* First char index in isiswctab. */
  LONGX     PT_LastChar;   /* Last  char index in isiswctab. */

  LONGX     PT_DifPosition;

} STRUCT_PREVTERM;
/* -------------------------------------------------------------- prototypes */
#if ANSI
void IniPreviousTerm(STRUCT_PREVTERM *prvtrm);
void PT_CharLimits(STRUCT_PREVTERM *prvtrm);
void PT_Search(STRUCT_PREVTERM *prvtrm);
LONGX PT_FindPreviousKey(STRUCT_PREVTERM *prvtrm,
                        UCHAR * root,
                        LONGX itrm,
                        UCHAR *previous);
LONGX TermPrevious(STRUCT_PREVTERM *prvtrm,
                  LONGX itrm,
                  UCHAR *prefix,
                  UCHAR *key);
#else /* !ANSI */
void IniPreviousTerm();
void PT_CharLimits();
void PT_Search();
LONGX PT_FindPreviousKey();
LONGX TermPrevious();
#endif /* ANSI */

