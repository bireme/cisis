#ifndef CITRM_HPP
#define CITRM_HPP

#include <cidbx.hpp>

class _YOURCLASS TRMSTRU
{
 public:

    LONGX     trmnbytes;         /* nbytes allocated */
    int      trmtype;           /* layout indicator */
    DBXSTRU *trmdbxp;
    int      trmrc;             /* term/nxterm rc */
    UCHR     trmkey[LE2+1];     /* key padded with blanks and null */
    LONGX     trmpost;           /* current posting order */
    LONGX     trmpmfn;           /* current posting pmfn */
    UWORD    trmptag;           /* current posting ptag */
    UWORD    trmpocc;           /* current posting pocc */
    UWORD    trmpcnt;           /* current posting pcnt */
    L1STRU   trml1buf;          /* .l01 input buffer */
    UCHR     trml1nul;          /* .l01 input buffer idx delimiter */
    L2STRU   trml2buf;          /* .l02 input buffer */
    UCHR     trml2nul;          /* .l02 input buffer idx delimiter */
    int      trmlcase;          /* last term/nxterm tree used */
    int      trmrcase;          /* trmread() treecase+1 selection  */
    off_t    trmrseek[2];       /* last term/nxterm leafread() lseek addr */
    int      trmrsize[2];       /* last term/nxterm leafread() read size */
    int      trmliock[2];       /* last trmread() ock */
    UCHR     *trmlidxp[2];      /* last term/nxterm idxchars ptr */
    LONGX     trmpleft;          /* postings left in segment */
    UCHR     *trmpcurr;         /* trmpchar not yet incremented */
    UCHR     *trmpchar;         /* ptr to next posting in trmifbuf */
#if LIND
    LONGX     trmyaddr;          /* byte address */
    LONGX     trmypsts;          /* total no of psts */
    int       trmycase;          /* postings type */
    int       trmysize;          /* postings length */
    LONGX     trmydocs;          /* total no of docs */
    LONGX     trmydisk;          /* total no of bytes */
    LONGX     trmyread;          /* total no of bytes already read */
    LONGX     trmyleft;          /* total no of bytes left on ifbuf */
#else
    INFO     trmpcblk;          /* info1 for next posting() */
    INFO     trmpcpos;          /* info2 for next posting() */
    IFPHEAD  trmifseg;          /* .ifp current segm header */
    IFPHEAD  trmifhdr;          /* .ifp 1st segm header - total psts here */
#endif
  //UCHR     trmifbuf[1];       /* .ifp input buffer */
    UCHR     trmifbuf[IFBSIZ];   /* .ifp input buffer */
    CISISX  *cisisxp;


  TRMSTRU (CISISX *cxp);

  void    xtrminit(void);

  LONGX    xtrmalloc(LONGX nbytes);

  LONGX    xterm(UCHR *dbnamp,
		UCHR *keyp);

  int     xtrmread(int      treecase,
		   UCHR   **retkeyp);

  LONGX    xnxterm(void);

  void    xposthdr1(void);

  LONGX    xposting(LONGX nord);

  void    xpostpost(void);
};

#endif // CITRM_CPP
