#if !CICPP
static INFX tlcpaddr;  /* info1 */
#if MY
static LONGX tlcppsts;  /* info2 */
#endif
static INFX tlcpdocs;  /* info3 */
static LONGX tlcinfo3;  /* info3 for ciifl.c */
static UBYTE *areabits=NULL;
static LONGX threshold;
static LONGX tlcmxmfn;  /* current */
static FILE *fplog = NULL;
static char *iyp_buff=NULL;
static unsigned int iyp_left;
#define IYPBSIZ 8192L /* ou INT_MAX */
#endif /* CICPP */

/* prototypes */
#if !CICPP
#if ANSI
LONGX tlputifp(char *areap, LONGX nbytes);
void iyp_storinit(LONGX parmxmfn, char *parmlogp);
void iyp_storlast(int storcond, char *storkeyp, int pstflag);
void iyp_storthis(LONGX mfn,LONGX parmxmfn,
                        char *keyp,LONGX count,int tag,int occ,int cnt);
void iyp_storclos(void);
#else
LONGX tlputifp();
void iyp_storinit();
void iyp_storlast();
void iyp_storthis();
void iyp_storclos();
#endif /* ANSI */
#endif /* CICPP */

#if !CICPP
static UBYTE mask;
static UBYTE *bp;
static LONGX tlputby;
static int tlcpcase;
static LONGX tlcloop,tlcmfn;
static UBYTE *tlcp,*tlcq,tlcqmfn[PMFNSIZ];
static int tlcloop2;
#define XFD 0
#if XFD
int xfd;
#endif
#endif /* CICPP */

#if CICPP
LONGX CIIFL :: tlputifp(char *areap,
	               LONGX  nbytes)
#else /*CICPP*/
LONGX tlputifp(areap,nbytes)
char *areap;
LONGX nbytes;
#endif /*CICPP*/
{
    char *p,*bufp;
    LONGX towrite;
    unsigned int n;
    if (!areap) {
	if (iyp_left == IYPBSIZ) return(0L);
	bufp=iyp_buff+(IYPBSIZ-iyp_left);
        memset(bufp,0x00,iyp_left);
        ifpwrit(dbxp,iyp_buff,IYPBSIZ);
        return(0L);
    }

#if XFD
 if (CIWRITE(xfd,areap,(size_t)nbytes) !=  nbytes) fatal("write/xfd");
#endif
    for (p=areap, towrite=nbytes; towrite; ) {
        if (towrite <= iyp_left) {
            n=(unsigned int)towrite;
            bufp=iyp_buff+(IYPBSIZ-iyp_left);
            memmove(bufp,p,n);
            iyp_left-=n;
	    break;
	}
        bufp=iyp_buff+(IYPBSIZ-iyp_left);
        memmove(bufp,p,iyp_left);
	ifpwrit(dbxp,iyp_buff,IYPBSIZ);
	towrite-=iyp_left;
        p+=iyp_left;
	iyp_left=IYPBSIZ;
    }
    return(nbytes);
}

#if CICPP
void CIIFL :: iyp_storinit(LONGX  parmxmfn,
	          	   char *parmlogp)
#else /*CICPP*/
void iyp_storinit(parmxmfn,parmlogp)
LONGX parmxmfn;
char *parmlogp;
#endif /*CICPP*/
{
#if XFD
xfd=CREAT("xfd.iyp",PERMIS); if (xfd <= 0) fatal("xfd");
#endif
    threshold=parmxmfn/8+1;
    tlcpaddr=0;
#if TRACORE
printf("%s: coreleft=%"_LD_"\n","areabits",CORELEFT());
#endif
#if CICPP
    try {areabits=(UBYTE *)new char[threshold];}
    catch (BAD_ALLOC) { areabits = (UBYTE *)ALLONULL; }
#else /* CICPP */
    areabits=(UBYTE *)ALLOC((ALLOPARM)threshold);
#endif /* CICPP */
    if (areabits == (UBYTE *)ALLONULL) fatal("ALLOC/areabits");
#if TRACORE
printf("%s: coreleft=%"_LD_"\n","areabits",CORELEFT());
#endif
#if CICPP
    try {iyp_buff=(char *)new char [IYPBSIZ];}
    catch (BAD_ALLOC) {iyp_buff = (char *)ALLONULL;}
#else /* CICPP */
    iyp_buff=(char *)ALLOC((ALLOPARM)IYPBSIZ);
#endif /* CICPP */
    if (iyp_buff == (char *)ALLONULL) fatal("ALLOC/iyp_buff");
#if TRACORE
printf("%s: coreleft=%"_LD_"\n","iyp_buff",CORELEFT());
#endif
    iyp_left=IYPBSIZ;
    memset(areabits,0x00,threshold);
    tlcpdocs=tlcmxmfn=0;
    if (parmlogp) if ((fplog=fopen(parmlogp,"w")) == NULL) fatal(parmlogp);
}
#if CICPP
void CIIFL :: iyp_storlast(int   storcond,
	         	   char *storkeyp,
		           int   pstflag)
#else /*CICPP*/
void iyp_storlast(storcond,storkeyp,pstflag)
int storcond;
char *storkeyp;
int pstflag;
#endif /*CICPP*/
{
    unsigned char *q;
    if (storcond) {
	if (tlcpdocs * PMFNSIZ >= threshold ) tlcpcase = BITSTRING;
	else tlcpcase = MFNSTRING;
	switch (tlcpcase) {
        case BITSTRING:
#define FIXSTRING 1
#if FIXSTRING
            tlcinfo3= -threshold;
#else
            tlcinfo3= -(tlcmxmfn/8+1);
#endif
            if (pstflag == IFUPDICT) { tlputby= -tlcinfo3; break; }
            tlputby=tlputifp((char *)areabits,-tlcinfo3);
            break;
        case MFNSTRING:
            tlcinfo3=0;
            if (pstflag == IFUPDICT) { tlputby=tlcpdocs*PMFNSIZ; break; }
            tlputby=0; tlcmfn=0;
            for (tlcp=areabits, tlcloop=tlcmxmfn/8+1; tlcloop--; tlcp++) {
		if (!*tlcp) { tlcmfn+=8; continue; }
                for (tlcloop2=8; tlcloop2--; tlcmfn++)
                    if (*tlcp & bitmask[tlcmfn&BY8RMASK]) {
                        tlcq=tlcqmfn;
			q=(unsigned char *)&tlcmfn;
			*tlcq++ = q[M1];
			*tlcq++ = q[M2];
			*tlcq++ = q[M3];
#if LIND4
			*tlcq++ = q[M4];
#endif 
                        tlputby+=tlputifp((char *)tlcqmfn,(LONGX)PMFNSIZ);
                    }
            }
            break;
        default: fatal("tlcpcase");
        }
	/* call svd to store btree */
        lifp_store_btree(storkeyp,tlcpaddr,tlcpdocs,treecase);
        /* and initialize new posting list */
        memset(areabits,0x00,threshold);
        tlcpdocs=tlcmxmfn=0;
        tlcpaddr+=tlputby;
    }
}

#if CICPP
void CIIFL ::iyp_storthis(LONGX  mfn,
	         	  LONGX  parmxmfn,
		          char *keyp,
		          LONGX  count,
		          int   tag,
		          int   occ,
		          int   cnt)
#else /*CICPP*/
void iyp_storthis(mfn,parmxmfn,keyp,count,tag,occ,cnt)
LONGX mfn,parmxmfn;
char *keyp;
LONGX count;
int tag,occ,cnt;
#endif /*CICPP*/
{
    if (mfn < 1 || mfn > parmxmfn) {
	fprintf(stderr,"key/post=%s/%"_LD_" mfn=%"_LD_" maxmfn=%"_LD_,
		keyp,count,mfn,parmxmfn);
	fatal("ciifl/ciifll2/offset");
    }
    bp =  &areabits[ mfn / 8];
    mask = bitmask [ mfn & BY8RMASK ];
    if (*bp & mask) {
	if (fplog)
	    fprintf(fplog,"duplicated posting|%8"_LD_"|%5d|%5d|%5d|%s\n",
		    mfn,tag,occ,cnt,keyp);
    }
    else {
	*bp |= mask;
	tlcpdocs++; /* one more doc */
    }
#if 0
printf("%8"_LD_"|%5d|%5d|%5d|%s %02x %02x %02x %02x %02x %02x \n",
mfn,tag,occ,cnt,keyp,
areabits[0],areabits[1],areabits[2],areabits[3],areabits[4],areabits[5]);
#endif
    if (mfn > tlcmxmfn) tlcmxmfn=mfn;
}
void iyp_storclos()
{
    if (iyp_buff) {
	tlputifp((char *)NULL,0L);
#if CICPP
	delete[] iyp_buff;
#else
	FREE(iyp_buff);
#endif /* CICPP */
    }
#if CICPP
    if (areabits) delete[] areabits;
#else
    if (areabits) FREE(areabits);
#endif /* CICPP */
}
