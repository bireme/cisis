
/* --------------------------- ly.c -------------------------------- */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>

#include "cisis.h"
#include "cirun.h"

#define TRACE  0
#define TRACE0 0
#define TRACE1 0
#define TRACE2 0

#if TRACE0+TRACE1+TRACE2
#define TRACE 1
#endif


#define MAXT 1024
#define MINT 1


/* ----------------------------- tlc.h ------------------------------ */

typedef union {
    L0STRU l0x;
    L1STRU l1x;
    L2STRU l2x;
} LUSTRU;

typedef struct maxstru {	/* list element */
    char key[LE2+1];
    int  len;
    LONGX inf;
    LONGX val;
} MAXSTRU;

MAXSTRU *vmaxp[MAXT+2];		/* entries + maxy ($) + maxz (in) */
int nmaxt;			/* no of entries = input parameter */

LUSTRU tlcleaf;			/* static for lp */

extern int trmtrace;

void main(argc,argv)
int argc;
char *argv[];
{
    char *dbnp;
    int treecase;

    char *up1p,*up2p;
    int up1len,up2len;

    LONGX punt1,punt2;

    LONGX tell;
    LONGX countell,rangtell;

    int weight,unique;
    int show1,show2,show3;
#if TRACE
    int now=0;
#endif
    char qlfx = '\0';
    int fixx = 0;
    int trace = 0;

    TRMSTRU *trmp;
    INVMAP *invp;
    LUSTRU *lp;
    MAXSTRU *maxp;

    PUNT punt;
    char up1[LE2+1];
    char up2[LE2+1];
    FILE *fp;

    char *p;
    int iarg,n,iock;
    int onmaxy=0;
    int maxy,maxz;
    int maxi,maxj,maxk,xi,nval,mval,ninstall=0;
    LONGX totval,medval,accval;
    LONGX itrm;
#if !LIND
    LONGX last;
#endif

#define VP(xi)	vmaxp[xi]
#define VPmaxi	vmaxp[maxi]
#define VPmaxj	vmaxp[maxj]
#define VPmaxk	vmaxp[maxk]
#define VPmaxy	vmaxp[maxy]
#define VPmaxz	vmaxp[maxz]

#define keyp	vmaxp[maxz]->key
#define keylen  vmaxp[maxz]->len
#define info	vmaxp[maxz]->inf
#define value	vmaxp[maxz]->val

#define VPxkey	vmaxp[xi]->key
#define VPxlen  vmaxp[xi]->len
#define VPxinf	vmaxp[xi]->inf
#define VPxval	vmaxp[xi]->val



    if (argc < 4) {
        printf("%s",cicopyr("Utility LYFREQ"));
        printf("\n");
	printf("lyfreq <dbn> <treecase> <maxt> <filout> [options..] \n");
	printf("\n");
	printf("options: {punt1/punt2}=<pos> \n");
	printf("         {up1/up2}=<upkey> \n");
	printf("         trace/show1/show2/show3 %s \n",(TRACE)?"[now]":" ");
	printf("         tell=<npages> ...\n");
	printf("         qlf=<char> \n");
	printf("         fix=<bytes> \n");
	printf("         weight%s \n",(!LIND)?"/unique":" ");
	printf("\n");
	exit(1);
    }

    dbnp=argv[1];
    invsetup(dbnp,0L,0L,0L);
    invp=dbxstorp(dbnp)->dbxifmap;

    if (sscanf(argv[2],"%d",&treecase) != 1) fatal(argv[3]);
    if (treecase != 1 && treecase != 2) fatal("treecase 1/2");
    treecase--;

    if (sscanf(argv[3],"%d",&nmaxt) != 1) fatal(argv[3]);
    if (nmaxt >= MAXT) fatal("MAXT Overflow");
    if (nmaxt <  MINT) fatal("MINT Underflow");

    maxy=nmaxt; maxz=nmaxt+1;           /* maxy + maxz */
    for (xi=0; xi < nmaxt+2; xi++) {	/* maxy + maxz */
	VP(xi)=(MAXSTRU *)ALLOC((ALLOPARM)sizeof(MAXSTRU));
	if (VP(xi) == NULL)
	    fatal("Not enough memory");
	memset(VP(xi),0x00,sizeof(MAXSTRU));
	VPxval=0;
    }

    if ((fp=fopen(argv[4],"w")) == NULL) fatal(argv[4]);


    up1p=up2p=NULL; itrm=1L;
    punt1=1; punt2=invp->cn[treecase].fmaxpos;
    show1=show2=show3=0; weight=unique=0; tell=0;

    for (p=up1, n=LE1; n--; ) *p++ = '!'; *p='\0';
    for (p=up2, n=LE2; n--; ) *p++ = '!'; *p='\0';

    for (iarg=5; iarg < argc; iarg++) {
	p=argv[iarg];

	if (strncmp(p,"up1=",4) == 0) {
	    up1p=p+4; up1len=strlen(up1p);
	    if (up1len > vlex[treecase]) fatal(p);
	    strcpy(up1,up1p);
	    for (n=up1len; n < vlex[treecase]; ) up1[n++] = '!';
	    up1[vlex[treecase]] = '\0';
	    continue;
	}
	if (strncmp(p,"up2=",4) == 0) {
	    up2p=p+4; up2len=strlen(up2p);
	    if (up2len > vlex[treecase]) fatal(p);
	    strcpy(up2,up2p);
	    for (n=up2len; n < vlex[treecase]; ) up2[n++] = '!';
	    up2[vlex[treecase]] = '\0';
	    continue;
	}

	if (strncmp(p,"punt1=",6) == 0) {
	    if (sscanf(p+6,"%"_LD_,&punt1) != 1) fatal("punt1=");
	    if (punt1 < 1) fatal("punt1");
	    continue;
	}
	if (strncmp(p,"punt2=",6) == 0) {
	    if (sscanf(p+6,"%"_LD_,&punt2) != 1) fatal("punt2=");
	    if (punt2 < 1) fatal("punt2");
	    continue;
	}

	if (strcmp(p,"show1") == 0) {
	    show1=1; continue;
	}
	if (strcmp(p,"trace") == 0) {
	    trmtrace=trace=1; continue;
	}
	if (strcmp(p,"show2") == 0) {
	    show2=1; continue;
	}
	if (strcmp(p,"show3") == 0) {
	    show3=1; continue;
	}
#if TRACE
	if (strcmp(p,"now") == 0) {
	    now=1; continue;
	}
#endif

	if (strncmp(p,"tell=",5) == 0) {
	    if (sscanf(p+5,"%"_LD_,&tell) != 1) fatal("tell=");
	    if (tell < 0) fatal("tell");
	    continue;
	}

	if (strcmp(p,"weight") == 0) {
	    weight=1; continue;
	}
#if !LIND
	if (strcmp(p,"unique") == 0) {
	    unique=1; continue;
	}
#endif

	if (strncmp(p,"qlf=",4) == 0) {
	    qlfx=p[4]; continue;
	}

	if (strncmp(p,"fix=",4) == 0) {
	    if (sscanf(p+4,"%d",&fixx) != 1) fatal("fix=");
	    if (fixx <= 0) fatal("fix");
	    continue;
	}


	fatal(p);
    }

    if (trace)
	printf("punt1=%"_LD_" punt2=%"_LD_" fmaxpos=%"_LD_" %s\n",
	    punt1,punt2,invp->cn[treecase].fmaxpos,
	    (unique) ? "- unique" : " ");

    if (up1p) {
	TERM(-itrm,dbnp,up1);
	if (TRMlcase != treecase)
	    fatal("up1p");
	punt1 = (treecase) ? TRMl2buf.pos : TRMl1buf.pos;
	if (trace)
	    printf("up1=%s(%d) punt1=%"_LD_" \n",up1p,up1len,punt1);
    }
    if (up2p) {
	TERM(-itrm,dbnp,up2);
	if (TRMlcase != treecase)
	    fatal("up2p");
	punt2 = (treecase) ? TRMl2buf.pos : TRMl1buf.pos;
	if (trace)
	    printf("up2=%s(%d) punt2=%"_LD_" \n",up2p,up2len,punt2);
    }

#if !LIND
    if (!up1p)
	if (!up2p) {
	    p = (treecase) ? up2 : up1;
	    TERM(-itrm, dbnp, p);
	    if (TRMlcase != treecase)
		fatal("upxp");
	if (trace)
		printf("upx=%s(%d) puntx=%"_LD_" \n",
		    p,strlen(p),(treecase) ? TRMl2buf.pos : TRMl1buf.pos);
	}
#endif

    if (tell) {
	tell=(rangtell=punt2-punt1+1)/tell;
	if (tell <= 0) tell=1;
	if (rangtell <= 0) rangtell=1;
    }
    countell=0;


    /*
	for each leaf
    */
    lp= &tlcleaf; /* static */
    totval=0;

    if (punt1 > punt2)
	fatal("punt1/punt2");

#if 0
    for (punt=punt1; punt <= punt2; punt++) {
#else
    for (punt=punt1; punt2; punt=lp->l0x.ps) {
#endif
        if (!punt) break;
	
	leafread((char *)lp,invp,treecase,punt,0); /* read a leaf*/
	++countell;
	if (tell)
	    if (countell % tell == 0) {
		fprintf(stderr,"+%6"_LD_"p %3"_LD_"%% ",countell,
		    (10 * (countell*100/rangtell) + 5) / 10);
		n=ninstall-1;
		if (n < 0) n = 0;
		if (n > nmaxt-1) n=nmaxt-1;
		fprintf(stderr,"     t=%9"_LD_"   %4d=%9"_LD_"   %4d=%9"_LD_" \n",
		    totval,1,VP(1-1)->val,n+1,VP(n+1-1)->val);
	    }

	/*
	    for each term
	*/
	for (iock=0; iock < lp->l0x.ock; iock++) {

	    keylen=vlex[treecase];

	    p = (treecase) ? lp->l2x.idx[iock].key
			   : lp->l1x.idx[iock].key;

	    memmove(keyp,p,keylen);

#if LIND
/*	    /* if LIND */

	    if (treecase) {
#if !SAMEL
		info=lp->l2x.idx[iock].info2;	/* tlc - tot no of postings */
#else
#define isamel 0
		info=lp->l2x.idx[iock].lxidxei[isamel].info2;
#endif
	    }
	    else {
#if !SAMEL
		info=lp->l1x.idx[iock].info2;	/* tlc - tot no of postings */
#else
		info=lp->l1x.idx[iock].lxidxei[isamel].info2;
#endif
	    }

#else
/*	    /* else LIND */

	    if (TRMlcase != treecase)
		fatal("TRMlcase");

	    if (iock == 0)
		if (treecase)
		    memmove(&TRMl2buf,lp,L2BSIZ);
		else
		    memmove(&TRMl1buf,lp,L1BSIZ);

	    TRMliock(treecase)=iock;

	    /* posthdr1 still uses TRMlidxp(TRMlcase) */
	    TRMlidxp(treecase) = (treecase) ? lp->l2x.idx[iock].key
					    : lp->l1x.idx[iock].key;
	    posthdr1(trmp);

	    info=TRMifhdr.ifptotp;
	    if (unique)
		if (info > 1)
		    for (last=info=0; posting(itrm,TRMpost+1) > 0; ) {
			if (TRMpmfn == last) continue;
			info++; last=TRMpmfn;
		    }

	    if (trace) {
		memmove(TRMkey,keyp,keylen); *(TRMkey+keylen)='\0';
		printf("TRMrc=%d  TRMlcase=%d/%d,%p  TRMkey=%s(%d)\n",
		    TRMrc,TRMlcase,
		    TRMliock(TRMlcase),TRMlidxp(TRMlcase),
		    TRMkey,strlen(TRMkey));
	    }

/*	    /* endif LIND */
#endif


	    if (show1) {
		printf("         |%7"_LD_"|%2d|",info,keylen);
		for (p=keyp, n=keylen; n--; p++) printf("%c",*p);
#if TRACE0
		printf("|***");
		if (now) printf("\n"); else getchar();
#else
		printf("|\n");
#endif
	    }

	    if (up2p) {
		if (memcmp(keyp,up2p,up2len) > 0) {
		    if (show3) {
			for (p=keyp+keylen-1; keylen; ) {
			    if (*p != ' ') break;
			    p--; keylen--;
			}
			printf("+++++++++|%7"_LD_"|%2d|",info,keylen);
			for (p=keyp, n=keylen; n--; p++)
			    printf("%c",*p);
			printf("|\n");
		    }
		    punt2=0;
		    break;
		}
	    }

	    for (p=keyp+keylen-1; keylen; ) {
		if (*p != ' ') break;
		p--; keylen--;
	    }

	    if (!keylen)
		continue;

	    if (up1p) {
		if (keylen < up1len) fatal("up1len");
		if (memcmp(keyp,up1p,up1len) < 0) {
		    if (show3) {
			printf("---------|%7"_LD_"|%2d|",info,keylen);
			for (p=keyp, n=keylen; n--; p++)
			    printf("%c",*p);
			printf("|\n");
		    }
		    continue;
		}
		else up1p=NULL;
	    }

	    value = (weight) ? info*keylen : info;

	    if (qlfx || fixx) {
		if (fixx)
		    if (keylen >= fixx) {
			keylen=fixx;
			value = (weight) ? info*keylen : info;
		    }
		if (qlfx)
		    if (memchr(keyp,qlfx,keylen)) {
			for (p=keyp, keylen=0; *p++ != qlfx; ) keylen++;
			value = (weight) ? info*keylen : info;
		    }
		if (onmaxy == 0) {
		    memmove(VPmaxy,VPmaxz,sizeof(MAXSTRU));
		    onmaxy=1;
		    continue; 		/* end qlfx - 1st */
		}
		if (keylen == VPmaxy->len) {
		    if (memcmp(keyp,VPmaxy->key,keylen) == 0) {
			VPmaxy->inf++;
			if (qlfx)
			    VPmaxy->inf = 0; /* + */
			VPmaxy->val += value;
			continue;	/* end qlfx */
		    }
		}
		xi=maxy;
		maxy=maxz;	/* maxy points to processed input */
		maxz=xi;	/* maxz is tabulated */
	    }


	    if (value > VP(nmaxt-1)->val) {		/* last = minimum */

#if TRACE1
printf("... \n");
for (xi=0; xi < nmaxt; xi++) {
 printf("%9"_LD_"|%7"_LD_"|%2d|",VPxval,VPxinf,VPxlen);
 for (p=VPxkey, n=VPxlen; n--; p++) printf("%c",*p);
 printf("|\n");
}
#endif
		if (show2) {
		    xi=maxz;
		    printf("%9"_LD_"|%7"_LD_"|%2d|",VPxval,VPxinf,VPxlen);
		    for (p=VPxkey, n=VPxlen; n--; p++) printf("%c",*p);
		    printf("|\n");
		}

		for (maxi=nmaxt-2; maxi >= 0; maxi--)	/* previous */
		    if (VPmaxi->val > value)		/* stop moving up */
			break;
		maxi++;

		totval-=VP(nmaxt-1)->val;
		for (maxj=nmaxt-1; maxj > maxi; maxj--) {   /* shift down */
		    maxk=maxj-1; /* top */
		    maxp=VPmaxk;
		    VPmaxk=VPmaxj;
		    VPmaxj=maxp;
		}

		memmove(VPmaxi->key,keyp,keylen);	    /* install it */
		VPmaxi->len=keylen;
		VPmaxi->inf=info;
		VPmaxi->val=value;
		ninstall++;
#if TRACE2
printf("=== \n");
for (xi=0; xi < nmaxt; xi++) {
 printf("%9"_LD_"|%7"_LD_"|%2d|",VPxval,VPxinf,VPxlen);
 for (p=VPxkey, n=VPxlen; n--; p++) printf("%c",*p);
 printf("|\n");
}
#endif
		totval+=value;
	    }
	}
    }


    if (tell) {
	fprintf(stderr,"+%6"_LD_"p %3"_LD_"%% ",countell,
	    (10 * (countell*100/rangtell) + 5) / 10);
    }

    medval=(10*totval+5)/10/2; mval=0;
    for (accval=0, maxi=0; maxi < nmaxt; maxi++)
	if (VPmaxi->val) {
	    mval++;
	    accval+=VPmaxi->val;
	    if (accval >= medval)
		break;
	}
    for (totval=0, nval=0, maxi=0; maxi < nmaxt; maxi++)
	if (VPmaxi->val) {
	    nval++;
	    totval+=VPmaxi->val;
	    xi=maxi;
	    fprintf(fp,"%9"_LD_"|%7"_LD_"|%2d|",VPxval,VPxinf,VPxlen);
	    for (p=VPxkey, n=VPxlen; n--; p++) fprintf(fp,"%c",*p);
	    fprintf(fp,"|");
	    for (n=vlex[NTREE-1]-VPxlen; n--; ) fprintf(fp," ");
	    fprintf(fp,"|%6d|%9"_LD_"|%d|%d\n",nval,totval,
		(nval <= mval) ? 1 : 0, treecase+1);
	}

    if (tell) {
	fprintf(stderr,"     t=%9"_LD_"   %4d=%9"_LD_"   %4d=%9"_LD_" \n",
	    totval,1,VP(1-1)->val,nval,VP(nval-1)->val);

	fprintf(stderr,"                 p50=%9"_LD_"   %4d=%9"_LD_" \n",
	    medval,mval,accval);
    }


    exit(0);
}
