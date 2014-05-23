/* --------------------------------------------------------------- APPEASY.H */

/* ----------------------------------------------------------------- defines */
#define CONTROL_Z    26
/* -------------------------------------------------------------- prototypes */
#if ANSI
UCHAR  *strall         (UCHAR *p);
UCHAR  *strdeall       (UCHAR *p);
UCHAR  *strreall       (UCHAR *old,UCHAR *from);
UCHAR  *easy_trimleft  (UCHAR *s);
UCHAR  *easy_trimright (UCHAR *s);
BOOLEAN easy_getfile   (UCHAR *file_name,UCHAR *buff,long max,UCHAR *ignore);
#else
UCHAR  *strall         ();
UCHAR  *strdeall       ();
UCHAR  *strreall       ();
UCHAR  *easy_trimleft  ();
UCHAR  *easy_trimright ();
BOOLEAN easy_getfile   ();
#endif
