#include <stdio.h>
#include <string.h>
#include "cisis.h"

#if SYXTY_FOUR
#define LONGZ long
#else 
#define LONGZ long long
#endif

typedef struct {
  char ch1;
  char ch1a;
  short int x1;
  LONGZ x1a;
  short int x2;
  char ch2[7];
} str1;

typedef struct {
  char ch1;
  char ch1a;
  short int x1;
  LONGZ x1a;
  short int x2;
  char ch2[7];
} __attribute__((aligned(1))) str1_aligned1;

typedef struct {
  char ch1;
  char ch1a;
  short int x1;
  LONGZ x1a;
  short int x2;
  char ch2[7];
} __attribute__((aligned(2))) str1_aligned2;

typedef struct {
  char ch1;
  char ch1a;
  short int x1;
  LONGZ x1a;
  short int x2;
  char ch2[7];
} __attribute__((aligned(4))) str1_aligned4;

typedef struct {
  char ch1;
  char ch1a;
  short int x1;
  LONGZ x1a;
  short int x2;
  char ch2[7];
} __attribute__((aligned(8))) str1_aligned8;


typedef struct {
  char  __attribute__((aligned(2))) ch1;
  char  __attribute__((aligned(2))) ch1a;
  short int  __attribute__((aligned(2))) x1;
  LONGZ int __attribute__((aligned(2))) x1a;
  short  int __attribute__((aligned(2))) x2;
  char __attribute__((aligned(2))) ch2[7];
} str1_aligned2X;

typedef struct {
  char  __attribute__((aligned(4))) ch1;
  char  __attribute__((aligned(4))) ch1a;
  short int  __attribute__((aligned(4))) x1;
  LONGZ  int __attribute__((aligned(4))) x1a;
  short  int __attribute__((aligned(4))) x2;
  char __attribute__((aligned(4))) ch2[7];
} str1_aligned4X;

typedef struct {
  char  __attribute__((aligned(8))) ch1;
  char  __attribute__((aligned(8))) ch1a;
  short int  __attribute__((aligned(8))) x1;
  LONGZ int  __attribute__((aligned(8)))  x1a;
  short int __attribute__((aligned(8))) x2;
  char __attribute__((aligned(8))) ch2[7];
} str1_aligned8X;

typedef struct {
  char  __attribute__((packed,aligned(8))) ch1;
  char  __attribute__((packed,aligned(8))) ch1a;
  short int  __attribute__((packed,aligned(8))) x1;
  LONGZ int  __attribute__((packed,aligned(8)))  x1a;
  short int __attribute__((packed,aligned(8))) x2;
  char __attribute__((packed,aligned(8))) ch2[7];
} str1_packed_aligned8X;

typedef struct {
  char  __attribute__((aligned(8),packed)) ch1;
  char  __attribute__((aligned(8),packed)) ch1a;
  short int  __attribute__((aligned(8),packed)) x1;
  LONGZ int  __attribute__((aligned(8),packed))  x1a;
  short int __attribute__((aligned(8),packed)) x2;
  char __attribute__((aligned(8),packed)) ch2[7];
} str1_aligned8X_packed;

typedef struct {
  char ch1;
  char ch1a;
  short int x1;
  LONGZ x1a;
  short int x2;
  char ch2[7];
} __attribute__((packed)) str1_packed;

typedef struct {
  char __attribute__((packed)) ch1;
  char __attribute__((packed)) ch1a;
  short int __attribute__((packed)) x1;
  LONGZ int __attribute__((packed)) x1a;
  short int __attribute__((packed)) x2;
  char __attribute__((packed)) ch2[7];
}  str1_packedX;


typedef struct {
  short  __attribute__((packed))  idtype;
  short  __attribute__((packed))  ordn;
  short  __attribute__((packed))  ordf;
  short  __attribute__((packed))  n;
  short  __attribute__((packed))  k;
  short  __attribute__((packed))  liv;
  int    __attribute__((packed))  posrx;
  int    __attribute__((packed))  nmaxpos;
  int    __attribute__((packed))  fmaxpos;
  short  __attribute__((packed))  abnormal;
} CNSTRU_packed;

typedef struct {
  short  __attribute__((aligned(2)))  idtype;
  short  __attribute__((aligned(2)))  ordn;
  short  __attribute__((aligned(2)))  ordf;
  short  __attribute__((aligned(2)))  n;
  short  __attribute__((aligned(2)))  k;
  short  __attribute__((aligned(2)))  liv;
  int    __attribute__((aligned(2)))  posrx;
  int    __attribute__((aligned(2)))  nmaxpos;									
  int    __attribute__((aligned(2)))  fmaxpos;									
  short  __attribute__((aligned(2)))  abnormal;
} CNSTRU_aligned2;																											typedef struct {
  short  __attribute__((packed, aligned(2)))  idtype;
  short  __attribute__((packed, aligned(2)))  ordn;
  short  __attribute__((packed, aligned(2)))  ordf;
  short  __attribute__((packed, aligned(2)))  n;
  short  __attribute__((packed, aligned(2)))  k;
  short  __attribute__((packed, aligned(2)))  liv;
  int    __attribute__((packed, aligned(2)))  posrx;
  int    __attribute__((packed, aligned(2)))  nmaxpos;
  int    __attribute__((packed, aligned(2)))  fmaxpos;
  short  __attribute__((packed, aligned(2)))  abnormal;							
} CNSTRU_packed_aligned2;

typedef struct {
  UWORD  PACKED  idtype;
  UWORD  PACKED  ordn;
  UWORD  PACKED  ordf;
  UWORD  PACKED  n;
  UWORD  PACKED  k;
  short  PACKED  liv;
  PUNT    PACKED  posrx;
  PUNT    PACKED  nmaxpos;
  PUNT    PACKED  fmaxpos;
  UWORD  PACKED  abnormal;
} CNSTRU_packed_aligned2_cisis;

void show(char *p, 
          int len) {
  int x=0;

  printf("[");
  for (; x<len; x++) {
    printf("%d,", (int)p[x]);
  }
  printf("]\n");
}

void main(int arc,
	  char *argv[]) {
  str1_aligned1 ali1;
  LONGZ val = 1695472800892422L;
  str1_aligned2 ali2;
  str1_aligned4 ali4;
  str1_aligned8 ali8;
  str1_aligned2X ali2X;
  str1_aligned4X ali4X;
  str1_aligned8X ali8X;
  str1_packed_aligned8X paali8X;
  str1_aligned8X_packed ali8Xpa;
  str1_packed pa;
  str1_packedX paX;
  CNSTRU_packed cnstru_packed;
  CNSTRU_aligned2 cnstru_aligned2;
  CNSTRU_packed_aligned2 cnstru_packed_aligned2;
  CNSTRU_packed_aligned2_cisis cnstru_packed_aligned2_cisis;

  printf("char=%ld\n", sizeof(char));
  printf("short=%ld\n", sizeof(short));
  printf("long=%ld\n", sizeof(long));
  printf("LONGZ=%ld\n", sizeof(LONGZ));
  printf("void*=%ld\n", sizeof(void*));
  printf("str1=%ld\n", sizeof(str1));
  printf("LONGX=%ld\n", sizeof(LONGX));
  printf("\n");
  printf("str1_aligned1=%ld\n", sizeof(str1_aligned1));
  printf("str1_aligned2=%ld\n", sizeof(str1_aligned2));
  printf("str1_aligned4=%ld\n", sizeof(str1_aligned4));
  printf("str1_aligned8=%ld\n", sizeof(str1_aligned8));
  printf("\n");
  printf("str1_aligned2X=%ld\n", sizeof(str1_aligned2X));
  printf("str1_aligned4X=%ld\n", sizeof(str1_aligned4X));
  printf("str1_aligned8X=%ld\n", sizeof(str1_aligned8X));
  printf("\n");
  printf("str1_packed_aligned8X=%ld\n", sizeof(str1_packed_aligned8X));
  printf("str1_aligned8X_packed=%ld\n", sizeof(str1_aligned8X_packed));
  printf("\n");
  printf("str1_packed=%ld\n", sizeof(str1_packed));
  printf("str1_packedX=%ld\n", sizeof(str1_packedX));
  printf("\n");
  printf("cnstru_packed=%ld\n", sizeof(cnstru_packed));
  printf("cnstru_aligned2=%ld\n", sizeof(cnstru_aligned2));
  printf("cnstru_packed_aligned2=%ld\n", sizeof(cnstru_packed_aligned2));
  printf("cnstru_packed_aligned2_cisis=%ld\n", sizeof(cnstru_packed_aligned2_cisis));
  printf("\n");

  memset(&ali1, 0x00, sizeof(ali1));
  ali1.ch1 = 1;
  ali1.ch1a = 2;
  ali1.x1 = 771;
  ali1.x1a = 289360691352306692L;
  ali1.x2 = 1285;
  memmove(ali1.ch2, &val, 7);

  memset(&ali2, 0x00, sizeof(ali2));
  ali2.ch1 = 1;
  ali2.ch1a = 2;
  ali2.x1 = 771;
  ali2.x1a = 289360691352306692L;
  ali2.x2 = 1285;
  memmove(ali2.ch2, &val, 7);

  memset(&ali4, 0x00, sizeof(ali4));
  ali4.ch1 = 1;
  ali4.ch1a = 2;
  ali4.x1 = 771;
  ali4.x1a = 289360691352306692L;
  ali4.x2 = 1285;
  memmove(ali4.ch2, &val, 7);

  memset(&ali8, 0x00, sizeof(ali8));
  ali8.ch1 = 1;
  ali8.ch1a = 2;
  ali8.x1 = 771;
  ali8.x1a = 289360691352306692L;
  ali8.x2 = 1285;
  memmove(ali8.ch2, &val, 7);

  memset(&ali2X, 0x00, sizeof(ali2X));
  ali2X.ch1 = 1;
  ali2X.ch1a = 2;
  ali2X.x1 = 771;
  ali2X.x1a = 289360691352306692L;
  ali2X.x2 = 1285;
  memmove(ali2X.ch2, &val, 7);

  memset(&ali4X, 0x00, sizeof(ali4X));
  ali4X.ch1 = 1;
  ali4X.ch1a = 2;
  ali4X.x1 = 771;
  ali4X.x1a = 289360691352306692L;
  ali4X.x2 = 1285;
  memmove(ali4X.ch2, &val, 7);

  memset(&ali8X, 0x00, sizeof(ali8X));
  ali8X.ch1 = 1;
  ali8X.ch1a = 2;
  ali8X.x1 = 771;
  ali8X.x1a = 289360691352306692L;
  ali8X.x2 = 1285;
  memmove(ali8X.ch2, &val, 7);

  memset(&paali8X, 0x00, sizeof(paali8X));
  paali8X.ch1 = 1;
  paali8X.ch1a = 2;
  paali8X.x1 = 771;
  paali8X.x1a = 289360691352306692L;
  paali8X.x2 = 1285;
  memmove(paali8X.ch2, &val, 7);

  memset(&ali8Xpa, 0x00, sizeof(ali8Xpa));
  ali8Xpa.ch1 = 1;
  ali8Xpa.ch1a = 2;
  ali8Xpa.x1 = 771;
  ali8Xpa.x1a = 289360691352306692L;
  ali8Xpa.x2 = 1285;
  memmove(ali8Xpa.ch2, &val, 7);

  memset(&pa, 0x00, sizeof(pa));
  pa.ch1 = 1;
  pa.ch1a = 2;
  pa.x1 = 771;
  pa.x1a = 289360691352306692L;
  pa.x2 = 1285;
  memmove(pa.ch2, &val, 7);

  memset(&paX, 0x00, sizeof(paX));
  paX.ch1 = 1;
  paX.ch1a = 2;
  paX.x1 = 771;
  paX.x1a = 289360691352306692L;
  paX.x2 = 1285;
  memmove(paX.ch2, &val, 7);

  memset(&cnstru_packed, 0x00, sizeof(cnstru_packed));
  cnstru_packed.idtype = 257;
  cnstru_packed.ordn = 514;
  cnstru_packed.ordf = 771;
  cnstru_packed.n = 1285;
  cnstru_packed.k = 1542;
  cnstru_packed.liv = 1799;
  cnstru_packed.posrx = 117901063L;
  cnstru_packed.nmaxpos = 134744072L;  
  cnstru_packed.fmaxpos = 151587081L;
  cnstru_packed.abnormal = 2570;

  memset(&cnstru_aligned2, 0x00, sizeof(cnstru_aligned2));
  cnstru_aligned2.idtype = 257;
  cnstru_aligned2.ordn = 514;
  cnstru_aligned2.ordf = 771;
  cnstru_aligned2.n = 1285;
  cnstru_aligned2.k = 1542;
  cnstru_aligned2.liv = 1799;
  cnstru_aligned2.posrx = 117901063L;
  cnstru_aligned2.nmaxpos = 134744072L;
  cnstru_aligned2.fmaxpos = 151587081L;
  cnstru_aligned2.abnormal = 2570;

  memset(&cnstru_packed_aligned2, 0x00, sizeof(cnstru_packed_aligned2));
  cnstru_packed_aligned2.idtype = 257;
  cnstru_packed_aligned2.ordn = 514;
  cnstru_packed_aligned2.ordf = 771;
  cnstru_packed_aligned2.n = 1285;
  cnstru_packed_aligned2.k = 1542;
  cnstru_packed_aligned2.liv = 1799;
  cnstru_packed_aligned2.posrx = 117901063L;
  cnstru_packed_aligned2.nmaxpos = 134744072L;
  cnstru_packed_aligned2.fmaxpos = 151587081L;
  cnstru_packed_aligned2.abnormal = 2570;

  memset(&cnstru_packed_aligned2_cisis, 0x00, sizeof(cnstru_packed_aligned2_cisis));
  cnstru_packed_aligned2_cisis.idtype = 257;
  cnstru_packed_aligned2_cisis.ordn = 514;
  cnstru_packed_aligned2_cisis.ordf = 771;
  cnstru_packed_aligned2_cisis.n = 1285;
  cnstru_packed_aligned2_cisis.k = 1542;
  cnstru_packed_aligned2_cisis.liv = 1799;
  cnstru_packed_aligned2_cisis.posrx = 117901063L;
  cnstru_packed_aligned2_cisis.nmaxpos = 134744072L;
  cnstru_packed_aligned2_cisis.fmaxpos = 151587081L;
  cnstru_packed_aligned2_cisis.abnormal = 2570;

  show((char*)&ali1, sizeof(ali1));
  show((char*)&ali2, sizeof(ali2));
  show((char*)&ali4, sizeof(ali4));
  show((char*)&ali8, sizeof(ali8));
  printf("\n");
  show((char*)&ali2X, sizeof(ali2X));
  show((char*)&ali4X, sizeof(ali4X));
  show((char*)&ali8X, sizeof(ali8X));
  printf("\n");
  show((char*)&paali8X, sizeof(paali8X));
  show((char*)&paali8X, sizeof(ali8Xpa));
  printf("\n");
  show((char*)&pa, sizeof(pa));
  show((char*)&paX, sizeof(paX));
  printf("\n");
  show((char*)&cnstru_packed, sizeof(cnstru_packed));
  show((char*)&cnstru_aligned2, sizeof(cnstru_aligned2));
  show((char*)&cnstru_packed_aligned2, sizeof(cnstru_packed_aligned2));
  show((char*)&cnstru_packed_aligned2_cisis, sizeof(cnstru_packed_aligned2_cisis));
  printf("\n");
}
