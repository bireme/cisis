#ifndef CIISO_HPP
#define CIISO_HPP

#include <cirec.hpp> // RECSTRU

class ISO_2709
{
 public:

  CISISX *cisisxp;
  unsigned char *iso_buff;                /* iso_bmove / iso_break */
  int iso_fd;                             /* iso_bmove / iso_break */
  int iso_lrecb;                          /* iso_bmove / iso_break */
  int iso_lrfox;                          /* iso_bmove / iso_break */
  int iso_isfox;                          /* iso_bmove / iso_break */
  unsigned iso_bfree;                     /* iso_bmove / iso_break */
  unsigned iso_bidx;                      /* iso_bmove / iso_break */
  unsigned iso_bfix;                      /* iso_bmove / iso_break */
  unsigned iso_cards;                     /* iso_bmove / iso_break */
  int iso_fatal;                          /* iso_rfatal() */
  unsigned char *iso_fmtlef;
  unsigned char *iso_fmtscp;
  int iso_ismarc;

#if GEN_ISI
  UBYTE *isi_buffin;                     /* iso_loadbuff */
  int    isi_fd;                         /* iso_loadbuff */
  int    isi_lrecb;                      /* iso_loadbuff */
  int    isi_lrfix;                      /* iso_loadbuff */
  int    isi_isfix;                      /* iso_loadbuff */
  LONGX   isi_count;                      /* iso_loadbuff */

#define LENLEN          5
#define LENTAG          3
#define LENLEF          4
#define LENSCP          5
#endif // GEN_ISI

  ISO_2709 (CISISX *cxp);

  ~ISO_2709 ();

  int xiso_openx(char *gidbnp,
                 unsigned char filnam[],
                 int lrecb = 0,
                 int lrfox = 80,
                 int isfox = 0);

  int xiso_creat(char *gidbnp,
                 unsigned char filnam[],
		 int  lrecb = 0,
		 int  lrfox = 80,
		 int  isfox = 0);

  int xiso_write(RECSTRU      *recp,
		 unsigned char isoxfs = '#',
		 unsigned char isoxgs = '#',
       unsigned int ldrtag = 0);

  int xiso_bmove(unsigned char *obuf,
		 unsigned       obufsize);

  int xiso_break(int isofix);

  int xiso_close(void);

  int xiso_rclos(void);

#if GEN_ISI
  int xiso_open(char *gidbnp,
                unsigned char filnam[],
                int  lrecb = 0,
                int  lrfix = 80,
                int  isfix = 0);

  int xiso_read(RECSTRU *recp, unsigned int ldrtag = 0);


  int xiso_loadbuff(int opt);

  int xiso_getval(char *p,
                  int   n);
#endif // GEN_ISI
};

#endif // CIISO_HPP
