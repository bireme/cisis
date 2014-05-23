/* --------------------------------------------------------------- GLOBDEF.H */

/* ---------------------------------------------------------- GLOBAL defines */
#ifndef PC
#define PC        1
#endif /* PC */
#if PC
#ifndef ANSI
#define ANSI      1
#define ALLOC     farmalloc
#define FREE      farfree
#define ALLOPARM  unsigned long
#define OPEN      _open
#define READ      _read
#define CLOSE     _close
#define UBYTE     unsigned char
#endif /* ANSI */
#endif /* PC */
#define UCHAR     unsigned char
#define ENDFC(e)  { c = e; goto LABEL_ENDFC; }
/* -------------------------------------------------------------------- enum */
//#ifndef TRUE
//#ifndef FALSE

/*typedef enum {
   FALSE,
   TRUE
} BOOLEAN;*/
//#endif
//#endif

#ifndef BOOLEAN
#define BOOLEAN int
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef BOOLEAN
typedef enum {
   BOOLFALSE,
   BOOLTRUE
} XBOOLEAN;
#endif
