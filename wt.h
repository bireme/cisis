/* ------------------------------ wt.h ------------------------------------- */

/* wt.h - header derived from Program WTRIG2: trigrams 1B
*/

#define FATRAPWT FATRAP

#define MAXPRMS1         128  //32
#define HDRPARMSTRING  32000

#define MAXWTFUN           4   //1024 //MAXNDBX
#define MAXJDS            51
#define MAXJVS            51
#define MAXTABCATS     30000
#define MAXTABWIDTH      100


#define PARMMAXREL   10
#define PARMMINSIM  0.7
#define PARMMAXSIM  1.0

#if 00000000
//#if !PC
//#ifndef itoa
char *itoa(int value, char *xa, int len);
char *itoa(int value, char *xa, int len)
{
  if (len) sprintf(xa,"%d",value);
}
#endif

#define TAG1   1
#define TAG2   2
#define TAG3   3
#define TAG4   4
#define TAG5   5
#define TAG33 33

#define TAG34 34
#define TAG35 35

#define TAG440 440
#define TAG999 999


#define MAXPARMWIDTH  (LE2+LE2)

#define SHOWV3LEN     3200 /* 400 x 8 */ //200

typedef struct vectel {
    float weight;
    LONGX hidx;
} VECTEL;

typedef struct xvector {
    float weight;
    LONGX hidx;
    char *foundp;
    char key[MAXPARMWIDTH+1];
    FFI keylen;
} DOCVEC;

typedef struct xlista {
    float sim;
    LONGX mfn;
    LONGX xxx;
    LONGX hit;
    void *prev;
    void *next;
    void *avail;
} LISTA;


typedef struct xlist1a { /* Program WTRIG1: trigrams 1A */
    float weight;
    LONGX hidx;
    void *prev;
    void *next;
    void *avail;
} LIST1A;

     
typedef struct xlistjd { /* JD ordered list */
    LONGX freq;
    float weight;
    LONGX hidx;
    void *prev;
    void *next;
    void *avail;
} LISTJD;

     
/* ------------------------------ wt.h ------------------------------------- */
