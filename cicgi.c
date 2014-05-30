/* ------------------------------ cicgi.c ---------------------------------- */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "cisis.h"

#if CICPP
#include "cicgi.hpp"
#endif

#if CICGI

#define CICG1		0 /* for COREDATA/mxcgi.c */

#if RECGIZM
#define CICGXGIZ	1
#endif

#define PARMTAG5	((FFI)      5/*U*/)
#define PARMTAG6	((FFI)      6/*U*/)
#define PARMTAG701	((FFI)    701/*U*/)

#define TAGMXTGS	((FFI)    900/*U*/)
#define TAGMXCTS	((FFI)    901/*U*/)
#define TAGMXPFT	((FFI)    902/*U*/)

#define NAMHXSUB	           'n'
#define VALHXSUB	           'v'
#define TAGHXSUB	((FFI)    100/*U*/)
#define TAGHXNAM	((FFI)    101/*U*/)
#define TAGHXVAL	((FFI)    102/*U*/)

#if PC
#define ALTEQUAL '~'
#else
#define ALTEQUAL '='
#endif

#if ANSI
static char *getword(char *line, char stop, char stop2);
static char x2c(char *what);
static void unescape_url(char *url);
static void plustospace(char *str);
static char *makeword(char *line, char stop, char stop2);
static char *fmakeword(FILE *f, char stop, char stop2, int *len);
static char *getenvx(char *namep);
#if CICG1
static void cgierr(int error, char *p);
static void b5_keepdbx(int option);
static LONGX getridx(int n, char *msgp);
#endif
#if !CICPP
static char *gettmpnam(char *namp);
#endif /* CICPP */
#else
static char *getword();
static char x2c();
static void unescape_url();
static void plustospace();
static char *makeword();
static char *fmakeword();
static char *getenvx();
#if CICG1
static void cgierr();
static void b5_keepdbx();
static LONGX getridx();
#endif
static char *gettmpnam();
#endif


#if CICPP
char * CGIClass :: gettmpnam(char *namp)
#else /* CICPP */
static char *gettmpnam(namp)
char *namp;
#endif /* CICPP */
{
#if BEFORE20000729
    char *p,*q;
    FILE *fp;
    p=tmpnam(namp);
    if (!namp) p=strdup(p);
    fp=fopen(p,"w"); if (fp) fclose(fp);
#if PC
    if (strcmp((q=p+strlen(p)-4),".$$$") == 0) *q='\0';
#endif
    return(p);
#else
    return (dbxtmpnm("CI_TEMPDIR", 0, namp /*NULL*/));
#endif
}

#if CICG1

#define CICGSRCH	1
#define CGCGMXMX	0

static LONGX getridx(n,msgp)
int n;
char *msgp;
{
    LONGX i,j;
    for (j=i=0; i < maxnrec; i++) if (!vrecp[i]) { j=1; break; }
    if (!j) if (msgp) cgierr(n,msgp);
    return(i);
}

static void b5_keepdbx(option)
int option;
{
    static LONGX keepndbx=0;
    LONGX idbx;

    if (option == 0) keepndbx=ndbxs;
    else 
        for (idbx=ndbxs-1; idbx>=keepndbx; idbx--)
            dbxflush(vdbxp[idbx]->dbxname);
}

static void cgierr(error,p)
int error;
char *p;
{
    static char line[5+40+1+1];
    int len;
    char *lp;

    sprintf(line,"%03d [",error); lp=line+5;
    len=strlen(p); if (len > 40) len=40;
    memmove(lp,p,len);
    sprintf(lp+len,"]");
    
    fatal(line);
}
#endif /* CICG1 */

#if CICPP
static char *getenvx(char *namep)
#else /* CICPP */
static char *getenvx(namep)
char *namep;
#endif /* CICPP */
{
    static char *none = "";
    char *envp;
    
    envp=getenv(namep);
    if (!envp) envp=none;
    
    return(envp);
}


#if CICPP
static char *getword(char *line,
                     char stop,
                     char stop2)
#else /* CICPP */
static char *getword(line, stop, stop2)
char *line;
char stop;
char stop2;
#endif /* CICPP */
{
    char *word;
    int x = 0,y;

    for(x=0;((line[x]) && (line[x] != stop) && (line[x] != stop2));x++)
        /*word[x] = line[x]*/;

    word=(char *) malloc(sizeof(char) * (x + 1));
    memmove(word,line,x); word[x] = '\0';

    if(line[x]) ++x;
    y=0;

    while((line[y++] = line[x++]) != '\0');
    return word;
}

#if CICPP
static char *makeword(char *line,
                      char stop,
                      char stop2)
#else /* CICPP */
static char *makeword(line, stop, stop2)
char *line;
char stop;
char stop2;
#endif /* CICPP */
{
    int x = 0,y;
    char *word = (char *) malloc(sizeof(char) * (strlen(line) + 1));

    for(x=0;((line[x]) && (line[x] != stop) && (line[x] != stop2));x++)
        word[x] = line[x];

    word[x] = '\0';
    if(line[x]) ++x;
#if 0
fprintf(stderr,"%s=%s,,,\n",word,line+x);
#endif
    y=0;

    while((line[y++] = line[x++]) != '\0');
    return word;
}

#if CICPP
static char *fmakeword(FILE *f,
                       char stop,
                       char stop2,
                       int *cl)
#else /* CICPP */
static char *fmakeword(f, stop, stop2, cl)
FILE *f;
char stop;
char stop2;
int *cl;
#endif /* CICPP */
{
    int wsize;
    char *word;
    int ll;

    wsize = CICGI_WORDSIZE;
    ll=0;
    word = (char *) malloc(sizeof(char) * (wsize + 1));

    while(1) {
        word[ll] = (char)fgetc(f);
        if(ll==wsize) {
            word[ll+1] = '\0';
            wsize+=CICGI_WORDSIZE;
            word = (char *)realloc(word,sizeof(char)*(wsize+1));
        }
        --(*cl);
        if((word[ll] == stop || word[ll] == stop2) || (feof(f)) || (!(*cl))) {
            if(word[ll] != stop && word[ll] != stop2) ll++;
            word[ll] = '\0';
#if 0
fprintf(stderr,"\n%s..\n",word);
#endif
	    break;
        }
        ++ll;
    }
    return word;
}

#if CICPP
static char x2c(char *what)
#else /* CICPP */
static char x2c(what)
char *what;
#endif /* CICPP */
{
    register char digit;

    digit = (what[0] >= 'A' ? ((what[0] & 0xdf) - 'A')+10 : (what[0] - '0'));
    digit *= 16;
    digit += (what[1] >= 'A' ? ((what[1] & 0xdf) - 'A')+10 : (what[1] - '0'));
    return(digit);
}

#if CICPP
static void unescape_url(char *url)
#else /* CICPP */
static void unescape_url(url)
char *url;
#endif /* CICPP */
{
    register int x,y;

    for(x=0,y=0;url[y];++x,++y) {
        if((url[x] = url[y]) == '%') {
            url[x] = x2c(&url[y+1]);
            y+=2;
        }
    }
    url[x] = '\0';
}

#if CICPP
static void plustospace(char *str)
#else /* CICPP */
static void plustospace(str)
char *str;
#endif /* CICPP */
{
    register int x;

    for(x=0;str[x];x++) if(str[x] == '+') str[x] = ' ';
}

#if USEIT
static int rind(s, c)
char *s;
char c;
{
    register int x;
    for(x=strlen(s) - 1;x != -1; x--)
        if(s[x] == c) return x;
    return -1;
}

static int getline(s, n, f)
char *s;
int n;
FILE *f;
{
    register int i=0;

    while(1) {
        s[i] = (char)fgetc(f);

        if(s[i] == '\r')
            s[i] = fgetc(f);

        if((s[i] == 0x4) || (s[i] == '\n') || (i == (n-1))) {
            s[i] = '\0';
            return (feof(f) ? 1 : 0);
        }
        ++i;
    }
    return 0; /* no wrn */
}

static void send_fd(f, fd)
FILE *f;
FILE *fd;
{
    /* int num_chars=0; */
    char c;

    while (1) {
        c = fgetc(f);
        if(feof(f))
            return;
        fputc(c,fd);
    }
}
#endif /* ISUSED */


/*
*****************************************************
cicgi.c
*****************************************************
*/


#if CICPP
int CGIClass::cicgi0(int *argc,
                     char *argv[],
                     char **wwwp,
                     UWORD taghxsub,
                     char *taghpfxp)
#else /* CICPP */
int cicgi0(argc,argv,wwwp,taghxsub,taghpfxp)
int *argc;
char *argv[];
char **wwwp;
UWORD taghxsub;
char *taghpfxp;
#endif /* CICPP */
{
    CICGI_ENTRY entries[CICGI_MAXENTRIES];
    int nentries=0;
    int method=0;
    char *cicgi_p=NULL;

    char *entryval;
    char *entryname;
    register int x,m=0;
    char *cl,*p,*p1,*p2,*clp;
    int cli,argnext;
    int un,uv,ut;
    LONGX bytes;


    if(strcmp(getenvx("REQUEST_METHOD"),"GET") == 0) {

        method=1;
	clp = getenv("QUERY_STRING");
        if(clp == NULL) {
            return(0);
        }
	cl=strdup(clp);
#if 0
 printf("cl=%s",cl); getchar();
#endif
	for(m=x=0;cl[0] != '\0';x++) {
       if (x >= CICGI_MAXENTRIES) fatal("cicgi/CICGI_MAXENTRIES/GET");
            entryval=getword(cl,'&','&');
#if 0
 printf("entryval=%s",entryval); getchar();
#endif
            plustospace(entryval);
            unescape_url(entryval);
            entryname=getword(entryval,'=',ALTEQUAL);
	    entries[x].namp = entryname;
	    entries[x].valp = entryval;
#if 0
 printf("entry#%d: %s=%s",x,entries[x].namp,entries[x].valp); getchar();
#endif
            if (entries[x].namp[0]) m++;
        }
	free(cl);
    }

    else
    if(strcmp(getenvx("REQUEST_METHOD"),"POST") == 0) {

        method=2;
        if(strcmp(getenvx("CONTENT_TYPE"),"application/x-www-form-urlencoded")) {
            return(0);
        }
        cli = atoi(getenvx("CONTENT_LENGTH"));
        for(m=x=0;cli && (!feof(stdin));x++) {
            if (x >= CICGI_MAXENTRIES) fatal("cicgi/CICGI_MAXENTRIES/POST");
            entries[x].valp = fmakeword(stdin,'&','&',&cli);
            plustospace(entries[x].valp);
            unescape_url(entries[x].valp);
            entries[x].namp = makeword(entries[x].valp,'=',ALTEQUAL);
            if (entries[x].namp[0]) m++;
        }
    }

    else
    if (!wwwp) if (taghxsub) if (*argc > 1) {

        method=3;
	     for(m=0, x=1; x < *argc; x++) {
	         cl=strdup(argv[x]);
            entryval=getword(cl,'&','&');
#if 0
 printf("entryval=%s",entryval); getchar();
#endif
            plustospace(entryval);
            unescape_url(entryval);
            entryname=getword(entryval,'=',ALTEQUAL);
            entries[m].namp = entryname;
	         entries[m].valp = entryval;
#if 0
 printf("entry#%d: %s=%s",m,entries[m].namp,entries[m].valp); getchar();
#endif
            if (entries[m].namp[0]) m++; else { free(entryval); free(entryname); }
            if (m >= CICGI_MAXENTRIES) fatal("cicgi/CICGI_MAXENTRIES/argv");
	         free(cl);
        }
    }

    if (!method) return(0);

    nentries=m;

    for (x=0; x < m; x++)
	if (strcmp(entries[x].namp,"trace") == 0) if (!entries[x].valp[0]) cgitrace=1;

  if (wwwp) {
    /* store input pairs */
        for (bytes=0, x=0; x < nentries; x++) {
            bytes+=1+5+1+5+1;
	    bytes+=2+strlen(entries[x].namp)+2+strlen(entries[x].valp);
        }
	*wwwp = (char *) malloc(sizeof(char)*bytes + 1);
	if (*wwwp == NULL) return(0);
    *(p= *wwwp)='\0';
    if (!taghxsub) taghxsub=TAGHXSUB;
    for (x=0; x < nentries; x++) {
	un=strlen(entries[x].namp);
	uv=strlen(entries[x].valp);
	if (taghpfxp) {
	    p1=strstr(entries[x].namp,"^t");
	    if (p1) p1+=2;
	    else {
	        ut=strlen(taghpfxp);
	        if (strncmp(p1=entries[x].namp,taghpfxp,ut) == 0) p1+=ut;
		else p1="";
	    }
	    ut=atoi(p1);
	    if (ut > 0) {
		sprintf(p,"H%"_LD_" %"_LD_" ",(LONGX)ut,(LONGX)uv); p+=strlen(p);
		memmove(p,entries[x].valp,uv); p+=uv;
		*p='\0';
	        continue;
	    }
	}
	sprintf(p,"H%"_LD_" %"_LD_" ",(LONGX)taghxsub,(LONGX)2+un+2+uv); p+=strlen(p);
	*p++=SFLDCHR; *p++=NAMHXSUB;
	memmove(p,entries[x].namp,un); p+=un;
	*p++=SFLDCHR; *p++=VALHXSUB;
	memmove(p,entries[x].valp,uv); p+=uv;
	*p='\0';
    }
    for (x=0; x < nentries; x++) {
        free(entries[x].namp);
        free(entries[x].valp);
    }
    return(0);
  } /* end if wwwp */

    for (x=0; x < m; x++) {
        if (strcmp(entries[x].namp,"CICGI") == 0 ||
            strcmp(entries[x].namp,"cicgi") == 0) {
	    cicgi_p=entries[x].valp;
	    for (; ++x < m; ) {
	        /* alloc -> free */
	        entries[x-1].namp=entries[x].namp;
	        entries[x-1].valp=entries[x].valp;
	    }
	    m--;
	    break;
	}
    }

    if (!cicgi_p) {
	p=getenv("CICGI");
        if (!p) cicgi_p=getenv("cicgi");
        if (p) cicgi_p=strdup(p);
    }

    if (!cicgi_p) {
        for (x=0; x < nentries; x++) {
            free(entries[x].namp);
            free(entries[x].valp);
        }
        return(0);
    }

    nentries=m;
    argnext= *argc;

#define VALUEMX	0
#if VALUEMX
    if (strcmp(cicgi_p,"mx") == 0) {
        for (x=0; x < m; x++) {
            if (strcmp(entries[x].namp,"files") == 0 ||
                strcmp(entries[x].namp,"cipar") == 0 ||
                strcmp(entries[x].namp,"mfrl") == 0) {
                p1=entries[x].namp;
                p2=entries[x].valp;
                p= (char *) malloc(sizeof(char) * (strlen(p1)+1+strlen(p2) + 1));
                sprintf(p,"%s=%s",p1,p2);
                argv[argnext++]=p;
            }
        }
        for (x=0; x < m; x++) {
            if (strcmp(entries[x].namp,"db") == 0 || strcmp(entries[x].namp,"dbn") == 0) {
                p2=entries[x].valp;
                p= (char *) malloc(sizeof(char) * (strlen(p2) + 1));
                sprintf(p,"%s",p2);
	        argv[argnext++]=p;
                break;
            }
        }
        for (x=0; x < m; x++) {
            if (strcmp(entries[x].namp,"files") != 0 &&
                strcmp(entries[x].namp,"cipar") != 0 &&
                strcmp(entries[x].namp,"mfrl") != 0  &&
                strcmp(entries[x].namp,"dbn") != 0) {
                p1=entries[x].namp;
                p2=entries[x].valp;
                p= (char *) malloc(sizeof(char) * (strlen(p1)+1+strlen(p2) + 1));
#if BEFORE20000629
                sprintf(p,"%s=%s",p1,p2);
#else
                if (*p2) sprintf(p,"%s=%s",p1,p2); else sprintf(p,"%s",p1);
#endif
                argv[argnext++]=p;
            }
        }
	cicgi_p=NULL;
    }
#endif /* VALUEMX */

#if CICG1
    if (cicgi_p) cicgi1(&argnext,argv,entries,nentries,cicgi_p);
#endif /* CICG1 */

    for (x=0; x < nentries; x++) {
        free(entries[x].namp);
        free(entries[x].valp);
    }

    argv[argnext]=NULL;
    *argc=argnext;

    return(*argc-1);
}



/* cicg2 - creates a MF record MF from cgi-bin parsing
*/

#if CICG1
#if ANSI
static char *getxcgi(char *cicgi_p, char *namp);
static UWORD getxtag(LONGX irec, char *namp, char *wkap);
#else
static char *getxcgi();
static UWORD getxtag();
#endif

static char *getxcgi(cicgi_p,namp)
char *cicgi_p;
char *namp;
{
    static char retarea[80+1];
    char *p,*q;

    strcpy(retarea,"cicgi");

    p=cicgi_p; q=retarea;
    if (strcmp(namp,"dbn") == 0) {
        while (*p && *p != ',') *q++ = *p++;
        *q='\0';
    }
    else
    if (strcmp(namp,"701") == 0) {
        while (*p && *p != ',') p++;
        if (*p) p++;
        while (*p && *p != ',') *q++ = *p++;
        *q='\0';
    }

    return retarea;
}


static UWORD getxtag(irec,namp,wkap)
LONGX irec;
char *namp;
char *wkap;
{
    UWORD xtag;
    int iocc,len;
    LONGX lv;

    for (xtag=0, len=strlen(namp), iocc=1; !xtag; iocc++) {
	recfield(wkap,irec,TAGMXTGS,iocc,"");
	if (!*wkap) break;
	if (strncmp(wkap,namp,len) == 0) {
	    /* nam=tag */
	    if (sscanf(wkap+len+1,"%"_LD_,&lv) == 1) xtag=(UWORD)lv;
	    break;
	}
    }

    if (!xtag)
        if (strncmp(namp,"tag",3) == 0)
	    /* tag999 */
	    if (sscanf(namp+3,"%"_LD_,&lv) == 1) xtag=(UWORD)lv;

    return xtag;
}


#if CICPP
#if CICG1
int CGIClass::cicgi1(int *argc,
                     char *argv[],
                     CICGI_ENTRY entries[],
                     int nentries,
                     char *cicgi_p)
#endif /* CICG1 */
#else /* CICPP */
int cicgi1(argc,argv,entries,nentries,cicgi_p)
int *argc;
char *argv[];
CICGI_ENTRY entries[];
int nentries;
char *cicgi_p;
#endif /* CICPP */
{

    LONGX buffridx,workridx,parmridx;
    LONGX irec;
    UWORD xtags[CICGI_MAXENTRIES],xtag;
    int tagged=0;

    char *cicgi_dbnp=NULL;
    int retc=0;

    char *wka=NULL;
    char *buffup,*parmp;

    RECSTRU *recp;
    LONGX upirec=0L;
    LONGX upcrec=0L;

    LONGX mfn;
    int iex;
    UWORD nexttag,thistag;
    char *lastnamp="";
    unsigned int uv,un;
    LONGX lv;
    char *x701p;

    int rc;
    char *p,*q,*xbp,*xwp,xchr,*q2;
    LONGX xnby,xlv;
    char *fmtp;
    int n;
#if CICGXGIZ
    VGIZPSTRU *gizmap;
    GIZMSTRU *gizmp;
    int ngizmos=0;
    int gizminfo;
#endif

    FILE *fp;
    int fd,left;

    int nexec;
    UWORD tagexec[32];
    int occexec[32];

    char *outfilnp=NULL;
    int   outfiltt=0;
    FILE *ofp=stdout;

#if CICGSRCH
#define MAXSRCH 8
    int  vsrchoff[MAXSRCH];
    int  vsrchmfn1[MAXSRCH];
    int  vsrchmfn2[MAXSRCH];
    LONGX vsrchirec[MAXSRCH];
    LONGX vsrchitrm[MAXSRCH];
    int  vsrchoccex[MAXSRCH];
    int  vsrchoccexx[MAXSRCH];
    LONGX vsrchbastag[MAXSRCH];
    int  vsrchmaster[MAXSRCH];
    char *vsrchdbxname[MAXSRCH];
    char *vsrchdictk1p[MAXSRCH];
    char *vsrchdictk2p[MAXSRCH];
    LONGX vsrchdicth[MAXSRCH];
    LONGX vsrchdicths[MAXSRCH];
    char *srchqrydbnp=NULL;
    int srchsrch=0;

    char *srchdbnp;
    char *srchqryp;
    LONGX srchirec,srchitrm,srchbastag;
    LONGX srchnbytes1;
    LONGX srchh,srchhits,mfn1,mfn2,mfn0,lvh;
    int srchi,srcherrno;
    char buffedit[B4UPLEN+1];
    char *b,*up,*tp,*ep;
    char *bufsizbuff;
    LONGX bufsizbridx=0;

    int     f2_xerrno;
    int     f2_errcod;
    char    f2_iomsg[MAXFATML+1];
    jmp_buf f2_jumper;

#endif /* CISISRCH */
    extern int errno;

    /* init rec_maxmfrl */
    //dbxinit();
    if (!ndbxs) dbxinit();  /* init vdbxp/vrecp/vtrmp if not yet init - AOT, 28/10/2005 */                

    /* trap cisis fatal() */
    dbxopt_errno=0; /* dbxopen/errno */
    strcpy(fatal_iomsg,"trap");
    rc = setjmp(fatal_jumper);
    if (rc != 0) {
        printf("Content-type: text/html%c%c",10,10);
	printf("CISIS/CGI detected an error:  %s [%d]\n",fatal_iomsg,errno);
	strcpy(fatal_iomsg,""); /* notrap fatal() */
	return(fatal_errcod);
    }


    /* allocate fdlupdat() buffer */
    RECALLOC(buffridx=nrecs,rec_maxmfrl);
    buffup=VMFX(buffridx); /* ds */
    
    /* allocate another fdlupdat() buffer - small */
#if CICGSRCH
    RECALLOC(bufsizbridx=nrecs,BUFSIZ+1);
    bufsizbuff=VMFX(bufsizbridx); /* ds */
#endif /* CISISRCH */

    /* allocate working buffer */
    RECALLOC(workridx=nrecs,rec_maxmfrl);
    wka=VMFX(workridx); /* ds */
    
    /* allocate parameter area */
    RECALLOC(parmridx=nrecs,rec_maxmfrl/*BUFSIZ+BUFSIZ*/);
    parmp=VMFX(parmridx); /* ds */
    
    /* load parameter record */
    RECALLOC(irec=nrecs,rec_maxmfrl/*+rec_maxmfrl*/);
    RECtype=TYPEMFR;
    RECrc=RCNORMAL;
    MFRmfn=1;
    MFRmfrl=MFRbase=LEADER; MFRnvf=0;
    MFRstatus=ACTIVE;

    for (mfn=1L; RECrc != RCEOF; mfn++) {
	RECORD(irec,(cicgi_dbnp=strdup(getxcgi(cicgi_p,"dbn"))),mfn);
	if (RECrc != RCNORMAL) continue;
	if (cgitrace) prtfields(recp,MFRmfn);
        x701p=getxcgi(cicgi_p,"701");
	if (!*x701p) break;
#if 0
	if (strcmp(recfield(wka,irec,PARMTAG5,1,"W"),"W") == 0 &&
	    strcmp(recfield(wka,irec,PARMTAG6,1,"cicgi"),"cicgi") == 0 &&
	    strcmp(recfield(wka,irec,PARMTAG701,1,x701p),x701p) == 0) { /* if "cicgi" */
#else
	if (
#endif
	    strcmp(recfield(wka,irec,PARMTAG701,1,""),x701p) == 0) { /* if "cicgi" */
            break;
	} /* end if "cicgi" */
    } /* end for mfn */

    if (MFRnvf == 0) fatal("CISIS/CGI record not found");


    /*
	Process <name>=<val> parameters
    */
    /* store input pairs */
    *(p=buffup)='\0';
    for (iex=0; iex < nentries; iex++) {
	un=strlen(entries[iex].namp);
	uv=strlen(entries[iex].valp);
	sprintf(p,"H%"_LD_" %"_LD_" ",(LONGX)TAGHXSUB,(LONGX)2+un+2+uv); p+=strlen(p);
	*p++=SFLDCHR; *p++=NAMHXSUB;
	memmove(p,entries[iex].namp,un); p+=un;
	*p++=SFLDCHR; *p++=VALHXSUB;
	memmove(p,entries[iex].valp,uv); p+=uv;
	*p='\0';
    }
    if (*buffup) fldupdat(irec,buffup);

    /* assign tags */
    for (nexttag=1, lastnamp="", iex=0; iex < nentries; iex++) {
	if (strcmp(entries[iex].namp,lastnamp) == 0) {
	    thistag=xtags[iex-1];
	}
	else {
	    thistag=nexttag;
	    xtag=getxtag(irec,entries[iex].namp,wka);
            if (xtag) {
	        thistag=xtag;
		nexttag=thistag+1;
		tagged++;
	    }
	    else nexttag++;
	    lastnamp=entries[iex].namp;
	}
	xtags[iex]=thistag;
    }

    /* store input data names */
    *(p=buffup)='\0';
    for (iex=0; iex < nentries; iex++) {
	un=strlen(entries[iex].namp);
	sprintf(p,"H%"_LD_" %"_LD_" ",(LONGX)TAGHXNAM,(LONGX)un); p+=strlen(p);
	memmove(p,entries[iex].namp,un); p+=un;
	*p='\0';
    }
    if (*buffup) fldupdat(irec,buffup);

    /* store input data values */
    *(p=buffup)='\0';
    for (iex=0; iex < nentries; iex++) {
	uv=strlen(entries[iex].valp);
	xtag = (tagged) ? xtags[iex] : TAGHXVAL;
	sprintf(p,"H%"_LD_" %"_LD_" ",(LONGX)xtag,(LONGX)uv); p+=strlen(p);
	memmove(p,entries[iex].valp,uv); p+=uv;
	*p='\0';
    }
    if (*buffup) fldupdat(irec,buffup);


    /*
	Process "mx" commands
    */
    tagexec[0]=TAGMXCTS; 
    occexec[0]=0;
    for (nexec=1; nexec > 0; ) { /* for iocc */

	occexec[nexec-1]++;
	p=recfield(parmp,irec,tagexec[nexec-1],occexec[nexec-1],"");

	if (cgitrace) {
	    printf("+++ %s:\n",p);
	}
	
	while (*p == ' ') p++;
	
	if (*p == '*') continue; /* comment */

	if (*p == '\0') {
	    nexec--;
	    continue;
	}

	if (strncmp(p,"exec=",5) == 0) {
	    if (sscanf(p+5,"%"_LD_,&lv) != 1) continue;
	    if (lv <= 0 || lv > (LONGX)USHRT_MAX) continue;
	    tagexec[nexec]=(UWORD)lv;
	    occexec[nexec]=0;
	    nexec++;
	    continue;
	}

	if (strcmp(p,"trace") == 0) {
            cgitrace=1;
	    continue;
	}
	if (strcmp(p,"prtfields") == 0) {
            prtfields(vrecp[irec],VMFRmfn(irec));
	    continue;
	}

	if (strcmp(p,"setdbx") == 0) {
	    b5_keepdbx(0);
	    continue;
	}
	if (strcmp(p,"resetdbx") == 0) {
	    b5_keepdbx(1);
	    continue;
	}
	if (strncmp(p,"flush=",6) == 0) {
	    dbxflush(p+6);
	    continue;
	}

#if CICGSRCH
	if (strncmp(p,"search/",7) == 0 ||
	    strncmp(p,"master/",7) == 0 ||
	    strncmp(p,"dict/"  ,5) == 0) {
	    if (strncmp(p,"dict/"  ,5) == 0) srchbastag=atol(q=p+5); 
	    else srchbastag=atol(q=p+7); while (isdigit(*q)) q++;
	    if (srchbastag <= 0) cgierr(1,p);
if (cgitrace) for (srchi=0; srchi < srchsrch; srchi++) printf("--- 1. %d/%"_LD_" irec=%"_LD_"\n",srchi,vsrchbastag[srchi],vsrchirec[srchi]);
	    for (n=srchi=0; srchi < srchsrch; srchi++) 
	        if (vsrchbastag[srchi] == srchbastag) { n=1; break;	}
	    if (!n) {
	        if (srchsrch >= MAXSRCH) break;
		srchi=srchsrch;	srchsrch++;
	    }
	    if (srchi != srchsrch-1) { 
	        printf("*** search/master/%"_LD_": %d/%d\n",srchbastag,srchi,srchsrch-1);
	        break;
	    }
	    vsrchirec[srchi]=0;
	    vsrchitrm[srchi]=0;
	    vsrchoccex[srchi]=0;
	    vsrchoccexx[srchi]=0;
            vsrchmaster[srchi]=0;
            vsrchbastag[srchi]=srchbastag;
            vsrchdbxname[srchi]=NULL;
            vsrchdictk1p[srchi]=NULL;
            vsrchdictk2p[srchi]=NULL;
	    vsrchdicth[srchi]=0;
	    vsrchdicths[srchi]=0;
	    b=buffup;
	    sprintf(b,"D%"_LD_,srchbastag); b+=strlen(b);
	    sprintf(b,"D%"_LD_,srchbastag+1); b+=strlen(b);
	    sprintf(b,"D%"_LD_,srchbastag+2); b+=strlen(b);
	    sprintf(b,"D%"_LD_,srchbastag+9); b+=strlen(b);
	    sprintf(b,"D%"_LD_,srchbastag+11); b+=strlen(b);
	    sprintf(b,"D%"_LD_,srchbastag+99);
	    fldupdat(irec,buffup);
	    if (*q != '=') cgierr(2,p);
	    srchdbnp = ++q;
	    if (strncmp(p,"dict/"  ,5) == 0) {
	        vsrchmaster[srchi]=2;
		q=strchr(q,' '); 
		if (q) {
		    *q++='\0'; tp=strstr(q," to="); if (tp) *tp='\0';
		    vsrchdictk1p[srchi]=strdup(q);
		    for (up=vsrchdictk1p[srchi]; *up; up++) *up = isisuctab[*up]; 
		    if (cgitrace) printf("+++ dict k1=%s\n",vsrchdictk1p[srchi]);
		    if (tp) {
			q=tp+4; n=strlen(q);
			if (n < LE2) { memset(q+n,*highv,LE2-n); q[LE2]='\0'; }
			vsrchdictk2p[srchi]=strdup(q);
			for (up=vsrchdictk2p[srchi]; *up; up++) *up = isisuctab[*up]; 
		        if (cgitrace) printf("+++ dict k2=%s\n",vsrchdictk2p[srchi]);
		    }
		} else vsrchdictk1p[srchi]=strdup("!");
                vsrchdbxname[srchi]=strdup(srchdbnp);;
	    }
	    else 
	    if (strncmp(p,"master/",7) == 0) {
	        vsrchmaster[srchi]=1;
		q=strchr(q,','); if (!q) cgierr(3,p);
		*q++='\0';
		if (sscanf(q,"%"_LD_,"%"_LD_,&mfn1,&mfn2) != 2) cgierr(4,p);
	        if (mfn1 <= 0L) cgierr(5,p);
	        if (mfn2 <= 0L) cgierr(6,p);
                vsrchdbxname[srchi]=strdup(srchdbnp);;
	    }
	    else {
		if ((q=strchr(q,'=')) == NULL) cgierr(7,p);
		*q++='\0';
		if (!*q) cgierr(8,p); /* points to srchqryp */
		if (!loadfile(RDBname,'@',q,wka,rec_maxmfrl,'\0')) cgierr(9,p);
		if (!*wka) cgierr(10,p);
		if (recfmt(irec,rec_maxmfrl,wka,buffup,rec_maxmfrl-1L) < 0) cgierr(11,p);
		if (!*buffup) cgierr(12,p);
		srchqryp=buffup;
	    }
	    if (!*srchdbnp) srchdbnp=RDBname;
	    
	    srchirec=vsrchirec[srchi];
	    if (!srchirec) {
	        for (srchirec=0; srchirec < maxnrec; srchirec++)
	            if (!vrecp[srchirec]) { 
		        if (cgitrace) printf("+++ srchirec/%"_LD_"=%"_LD_"\n",srchbastag,(LONGX)srchirec);
		        vsrchirec[srchi]=srchirec; break; 
		    }
	        if (!vsrchirec[srchi]) cgierr(13,p);
		recallok(srchirec,(VRECnbytes(irec) > rec_maxmfrl) ? VRECnbytes(irec) : rec_maxmfrl );
	    }
	    if (vsrchmaster[srchi] == 2) {
	      srchitrm=vsrchitrm[srchi];
	      if (!srchitrm) {
	        for (srchitrm=1; srchitrm < maxntrm; srchitrm++)
	            if (!vtrmp[srchitrm]) { 
		        if (cgitrace) printf("+++ srchitrm/%"_LD_"=%"_LD_"\n",srchbastag,(LONGX)srchitrm);
		        vsrchitrm[srchi]=srchitrm; break; 
		    }
	        if (!vsrchitrm[srchi]) cgierr(14,p);
		/* if (!vsrchdicps[srchi]) trmalloc(srchirec,0L); */
	      }
	    }
	    
	    /* trap cisis fatal() */
	    f2_xerrno=dbxopt_errno; /* dbxopen/errno */
	    strcpy(f2_iomsg,fatal_iomsg);
	    memmove(f2_jumper,fatal_jumper,sizeof(fatal_jumper));
	    f2_errcod=fatal_errcod;
	    strcpy(fatal_iomsg,"trap");
	    rc = setjmp(fatal_jumper);
	    if (rc != 0) {
	        retc=fatal_errcod;
		memmove(fatal_jumper,f2_jumper,sizeof(fatal_jumper));
		strcpy(fatal_iomsg,f2_iomsg);
		fatal_errcod=f2_errcod;
		dbxopt_errno=f2_xerrno;
		break; /* continue! */
	    }

	    if (vsrchmaster[srchi] == 2) {
	        term(srchitrm,vsrchdbxname[srchi],vsrchdictk1p[srchi]);
		for (lv=0; VTRMrc(srchitrm) != RCEOF; ) {
		    if (vsrchdictk2p[srchi])
		        if (strcmp(VTRMkey(srchitrm),vsrchdictk2p[srchi]) > 0) break;
		    if (VTRMxytotp(srchitrm)) lv++;
		    nxterm(srchitrm);
		}
		vsrchdicths[srchi]=lv;
	    }
            else { 
                record(srchirec,srchdbnp,0L);
                lv=VMF0nxtmfn(srchirec)-1; 
            }
	    b=bufsizbuff; /* srchqryp is using buffup */
	    sprintf(b,"A%"_LD_"#%s#",srchbastag+9,srchdbnp); b+=strlen(b);
	    sprintf(b,"A%"_LD_"#%"_LD_"#",srchbastag+11,0L); b+=strlen(b);
	    fldupdat(irec,bufsizbuff);
	    if (vsrchmaster[srchi] == 1) {
	        mfn=VMF0nxtmfn(srchirec)-1;
		if (mfn1 > mfn) mfn1=mfn;
		if (mfn2 > mfn) mfn2=mfn;
		vsrchmfn1[srchi]=mfn1;
		vsrchmfn2[srchi]=mfn2;
	    }
	    if (vsrchmaster[srchi] == 0) {
		srchnbytes1=VMF0nxtmfn(srchirec)/8+1;
		for (mfn=1; ; mfn++) {
		    record(srchirec,srchdbnp,mfn); 
		    if (VRECrc(srchirec) == RCEOF) break;
		    if (VRECrc(srchirec) == RCNORMAL) break;
		}
		lv=BUFSIZ+strlen(srchqryp)+srchnbytes1;
		if (lv > VRECnbytes(srchirec)) {
		    FREE(vrecp[srchirec]); vrecp[srchirec]=NULL; nrecs--;
		    recallok(srchirec,lv);
		}
		if (!srchqrydbnp) {
		    srchqrydbnp=gettmpnam(NULL); /* wka*/
		}
		/* b4_exp assigns MFRmfn=RDBmsmfn */
		VRECdbxp(srchirec)=dbxstorp(srchqrydbnp);
		if (VRDBmsmfn(srchirec) > 0) {
		    for (b4setno=1, n=0; n < srchi; n++) 
		        if (vsrchmaster[srchi] == 0) b4setno++;
		    VRDBmsmfn(srchirec) = b4setno;
		}
		ep=b4_exp(srchirec,srchdbnp,srchqryp,srchqrydbnp,&srcherrno); /* buffup */
		if (srcherrno) fatal(ep);
		ep=b4_run(srchirec,srchdbnp,srchnbytes1,rec_maxmfrl,(LONGX)cgitrace,
						     isisuctab,0L,&srcherrno);
		if (srcherrno) fatal(ep);
		sprintf(buffup,"A%"_LD_"#%s^m%"_LD_"#",srchbastag+99,srchqrydbnp,VMFRmfn(srchirec)); 
		fldupdat(irec,buffup);
	    } /* end else vsrchmaster[srchi] */
	    continue;
	}

	if (strncmp(p,"gethit/",7) == 0) {
	    srchbastag=atol(q=p+7); while (isdigit(*q)) q++;
	    if (srchbastag <= 0) cgierr(15,p);
if (cgitrace) for (srchi=0; srchi < srchsrch; srchi++) printf("--- 2. %d/%"_LD_" irec=%"_LD_"/%"_LD_"\n",srchi,vsrchbastag[srchi],irec,vsrchirec[srchi]);
	    for (n=srchi=0; srchi < srchsrch; srchi++) 
	        if (vsrchbastag[srchi] == srchbastag) { n=1; break;	}
	    if (!n) cgierr(16,p);
	    if (srchi != srchsrch-1) { 
	        printf("*** gethit/%"_LD_": %d/%d\n",srchbastag,srchi,srchsrch-1);
	        break;
	    }
	    
	    srchirec=vsrchirec[srchi]; 
	    srchitrm=vsrchitrm[srchi];
	    
	    if (vsrchmaster[srchi] == 2) {;
	        srchh=vsrchdicth[srchi];
	        srchhits=vsrchdicths[srchi];
	    }
	    else
	    if (vsrchmaster[srchi] == 1) {
	        srchh=VMFRmfn(srchirec);
		mfn1=vsrchmfn1[srchi];
		mfn2=vsrchmfn2[srchi];
		srchhits=mfn2-mfn1+1; if (srchhits < 0) srchhits=(-srchhits);
	    }
	    else {
		recfield(buffedit,srchirec,MFQTHCUR,1,"");
		if (!*buffedit) cgierr(17,p);
		sscanf(buffedit,B4EDIT1,&srchh);
		srchhits=b4_hit(srchirec,0L);
	    }
	    mfn0=0;
	    if (srchh == 0) {  /* 1st time */
		if (cgitrace) printf("+++ gethit/%"_LD_"=%"_LD_" hits\n",srchbastag,srchhits);
	        srchh=1L; vsrchoff[srchi] = 1;
		if (vsrchmaster[srchi] == 1) {
		    if (mfn1 > mfn2) vsrchoff[srchi] = -1;
		    VMFRmfn(srchirec)=mfn1;
		}
		else 
		if (vsrchmaster[srchi] == 0) if (strcmp(q,"/rev") == 0) { 
		    srchh=srchhits; vsrchoff[srchi] = -1;
		}
		vsrchoccex[srchi]=occexec[nexec-1];	/* save gethit occ (tag) */
		for (n=occexec[nexec-1]; ++n; ) {
		    q=recfield(buffup,irec,tagexec[nexec-1],n,"");
		    if (!*q) break;
		    if (strncmp(q,"endhit/",7) != 0) continue;
		    if (atol(q+7) != srchbastag) continue;
		    vsrchoccexx[srchi]=n+1;		/* save endhit occ+1 (tag) */
		    break;
		}
		if (!vsrchoccexx[srchi]) cgierr(18,p);
	    }
	    else {
	        if (vsrchmaster[srchi] == 1) {
		    mfn0=VMFRmfn(srchirec);
		    VMFRmfn(srchirec)+=vsrchoff[srchi];
		}
		else if (vsrchmaster[srchi] == 0) {
		    mfn0=b4_hit(srchirec,srchh);
		    srchh+=vsrchoff[srchi]; 
		}
	    }

	    if (vsrchmaster[srchi] == 2) {
	        mfn=0; 
		if (!vsrchdicth[srchi]) *VTRMkey(srchitrm)='\0';
		while (mfn == 0) {
		    if (!*VTRMkey(srchitrm))
		        term(srchitrm,VTDBname(srchitrm),vsrchdictk1p[srchi]); /* rewind */
		    else
		        nxterm(srchitrm);
		    if (VTRMrc(srchitrm) == RCEOF) break;
		    if (vsrchdictk2p[srchi]) 
		        if (strcmp(VTRMkey(srchitrm),vsrchdictk2p[srchi]) > 0) break;
		    mfn=posting(srchitrm,VTRMpost(srchitrm)+1);
		    if (mfn <= 0) {mfn=0; break;}
	        }
	       	lvh= ++vsrchdicth[srchi]; 
	    }
	    else
	    if (vsrchmaster[srchi] == 1) {
	        for (mfn=VMFRmfn(srchirec); ; ) {
		    if (mfn == mfn2+vsrchoff[srchi]) { mfn=0; break; }
		    else {
                        record(srchirec,vsrchdbxname[srchi],mfn);
		        if (VRECrc(srchirec) == RCEOF) { mfn=0; break; }
		        if (VMFRstatus(srchirec) == ACTIVE) break;
		        mfn+=vsrchoff[srchi];
		    }
		}
		lvh=(vsrchoff[srchi] == 1)?mfn-mfn1+1:mfn1-mfn+1;
	    }
	    else {
	        if (srchh/*its*/) mfn=b4_hit(srchirec,srchh); else mfn=0;
		lvh=(vsrchoff[srchi] == 1)?srchh:srchhits-srchh+1;
	    }

	    if (mfn > 0) {
		if (vsrchmaster[srchi] == 2) {
		    b=buffup;
		    sprintf(b,"D%"_LD_"A%"_LD_"\n",srchbastag+51,srchbastag+51); b+=strlen(b);
		    strcpy(b,VTRMkey(srchitrm)); 
		    for (b+=strlen(b); ; b--) if (*(b-1) != ' ') break;
		    sprintf(b,"\n");
		    fldupdat(irec,buffup);
		}
		b=buffup;
		sprintf(b,"D%"_LD_"D%"_LD_"D%"_LD_"D%"_LD_,srchbastag,srchbastag+1,srchbastag+2,srchbastag+11); 
		b+=strlen(b);
		sprintf(b,"A%"_LD_"#%"_LD_"#",srchbastag,mfn); b+=strlen(b);
		sprintf(b,"A%"_LD_"#%"_LD_"#",srchbastag+1,lvh); b+=strlen(b);
		sprintf(b,"A%"_LD_"#%"_LD_"#",srchbastag+2,srchhits); b+=strlen(b);
		sprintf(b,"A%"_LD_"#%"_LD_"#",srchbastag+11,mfn0);
		fldupdat(irec,buffup);
	    }
	    else {
	        if (srchitrm) if (vtrmp[srchitrm]) {
	            FREE(vtrmp[srchitrm]); vtrmp[srchitrm]=NULL; ntrms--;
		}
	        if (srchirec) if (vrecp[srchirec]) {
	            FREE(vrecp[srchirec]); vrecp[srchirec]=NULL; nrecs--;
		}
                if (vsrchdictk1p[srchi]) FREE(vsrchdictk1p[srchi]);
                if (vsrchdictk2p[srchi]) FREE(vsrchdictk2p[srchi]);
                if (vsrchdbxname[srchi]) FREE(vsrchdbxname[srchi]);
		if (srchsrch) srchsrch--;
		if (srchsrch == 0) 
		    if (srchqrydbnp) {
		        dbxflush(srchqrydbnp);
		        FREE(srchqrydbnp); srchqrydbnp=NULL;
		    }
		occexec[nexec-1]=vsrchoccexx[srchi]; /* set endhit occ+1 (tag) */
		occexec[nexec-1]--;		 /* occexec[nexec-1]++; */
	    }
	    continue;
	}

	if (strncmp(p,"endhit/",7) == 0) {
	    srchbastag=atol(q=p+7); while (isdigit(*q)) q++;
	    if (srchbastag <= 0) cgierr(22,p);
if (cgitrace) for (srchi=0; srchi < srchsrch; srchi++) printf("--- 3. %d/%"_LD_" irec=%"_LD_"/%"_LD_"\n",srchi,vsrchbastag[srchi],irec,vsrchirec[srchi]);
	    for (n=srchi=0; srchi < srchsrch; srchi++) 
	        if (vsrchbastag[srchi] == srchbastag) { n=1; break;	}
	    if (!n) cgierr(23,p);
	    if (srchi != srchsrch-1) { 
	        printf("*** endhit/%"_LD_": %d/%d\n",srchbastag,srchi,srchsrch-1);
	        break;
	    }
	    if (vsrchoccex[srchi]) {
		occexec[nexec-1]=vsrchoccex[srchi]; /* set gethit occ (tag) */
		occexec[nexec-1]--;		/* occexec[nexec-1]++; */
	    }
	    continue;
	}
#if 00
#define MFQTDBN0        11
#define MFQTQRY0        12
#define MFQTOBJ0        13
#define MFQTXCTS        20
#define MFQSWTIM        't'
#define MFQSWDBN        'd'
#define MFQSWQTY        'q'
#define MFQTASET        21
#define MFQTALCC        22
#define MFQTHITS        31
#define MFQTHCUR        32
#define MFQTHMFN        33
#define MFQTHSIZ        34
#define MFQTHMEM        35
#define B4EDIT1         "%06"_LD_         /* sprintf format for field edit */
#endif
#endif /* CICGSRCH */

	if (strncmp(p,"goto=",5) == 0) {
	    q=p+5; if (!*q) continue;
	    loadfile(RDBname,'@',q,wka,rec_maxmfrl,'\0');
	    if (recfmt(irec,rec_maxmfrl,wka,buffup,rec_maxmfrl-1L) < 0) fatal(wka);
	    if (!*buffup) continue;		/* accepts no label */
	    for (n=1; ++n; ) {
	        q=recfield(wka,irec,tagexec[nexec-1],n,"");
	        if (!*q) break;
	        if (strncmp(q,"label=",6)) continue;
	        if (strcmp(q+6,buffup)) continue;
		occexec[nexec-1]=n;		/* occexec[nexec-1]++; */
		n=0;
	        break;
	    }
	    if (n) cgierr(24,p);
	    continue;
	}

	if (strncmp(p,"sys=",4) == 0) {
	    q=p+4; if (!*q) continue;
	    loadfile(RDBname,'@',q,wka,rec_maxmfrl,'\0');
            if (cgitrace) printf("+++ sys=%s\n",wka);
	    system(wka);
	    continue;
	}
	
	if (strncmp(p,"set_of=",7) == 0) {
	    q=p+7; if (!*q) continue;
	    loadfile(RDBname,'@',q,wka,rec_maxmfrl,'\0');
	    if (recfmt(irec,rec_maxmfrl,wka,buffup,rec_maxmfrl-1L) < 0) fatal(wka);
	    outfiltt=1;
	    if (outfilnp) FREE(outfilnp); outfilnp=strdup(buffup);
            if (cgitrace) printf("+++ set_of=%s\n",outfilnp);
	    fp=fopen(outfilnp,"w");
	    if (fp != (FILE *)NULL) ofp=fp;
	    continue;
	}
        if (strcmp(p,"get_of") == 0) {
#if 0
	    q=getenv("GET_OF");
	    if (!q) continue;
	    if (*q != 'Y') continue;
#endif
	    q=getenv("OUTPUT_FILE");
	    if (!q) { q=gettmpnam(wka); outfiltt=1; }
	    if (outfilnp) FREE(outfilnp); outfilnp=strdup(q);
            if (cgitrace) printf("+++ getof=%s\n",outfilnp);
	    fp=fopen(outfilnp,"w");
	    if (fp != (FILE *)NULL) ofp=fp;
	    continue;
	}
        if (strcmp(p,"type_of") == 0) {
	    if (!outfilnp) continue;
	    if (!ofp) continue;
	    if (ofp == stdout) continue;
	    fclose(ofp); ofp=stdout; /*(FILE *)NULL;*/
#if PC
	    sprintf(wka,"type %s",outfilnp);
#else
	    sprintf(wka,"cat %s",outfilnp);
#endif
	    system(wka);
	    continue;
	}
        if (strcmp(p,"del_of") == 0) {
	    if (!outfilnp) continue;
	    if (!outfiltt) continue;
	    if (ofp) if (ofp != stdout) { fclose(ofp); ofp=stdout; /*(FILE *)NULL;*/ }
	    outfiltt=0; 
#if PC
	    sprintf(wka,"del %s",outfilnp);
#else
	    sprintf(wka,"rm %s",outfilnp);
#endif
	    system(wka);
	    continue;
	}
        if (strcmp(p,"close_of") == 0) {
	    if (!outfilnp) continue;
	    if (ofp) if (ofp != stdout) { fclose(ofp); ofp=stdout; /*(FILE *)NULL;*/ }
	    outfiltt=0; 
	    continue;
	}

	if (strncmp(p,"cat=",4) == 0) {
	    q=p+4; if (!*q) continue;
	    loadfile(RDBname,'@',q,wka,rec_maxmfrl,'\0');
            if (cgitrace) printf("+++ cat=%s\n",wka);
	    fd=dbxopen(RDBname,wka,"");
	    if (fd <= 0) continue;
	    while ((left=CIREAD(fd,wka,BUFSIZ)) > 0) {
	        if (wka[left-1] == 0x1A) left--;
	        for (q=wka; left--; q++) if (*q != '\r') fprintf(ofp,"%c",*q);
	    }
	    CLOSE(fd);
	    continue;
	}
	
#if CICGMXMX
#endif /* CICGMXMX */


        if (strncmp(p,"rec=",4) == 0) {
            q=p+4; 
	    q2=strchr(q,'='); if (!q2) fatal(p); *q2='\0'; q2++;
            if (!*q) q=gettmpnam(wka);
	    
            if (cgitrace) printf("+++ rec=%s/%s\n",q,q2);

            recallok((upcrec=getridx(31,p)),(LONGX)sizeof(M0STRU));
#if 0	    
	    VRECtype(upcrec)=TYPEMF0;
            VRECdbxp(upcrec)=dbxstorp(q); /* wka */
	    VRECrc(upcrec)=RCNORMAL;
	    memset(VMFX(upcrec),0x00,sizeof(M0STRU));
	    VMF0nxtmfn(upcrec)=1L;
#endif
            record(upcrec,q,0L);
	    
            recallok((upirec=getridx(32,p)),
	    (VRECnbytes(irec) > rec_maxmfrl) ? VRECnbytes(irec) : rec_maxmfrl);

	    VRECtype(upirec)=TYPEMFR;
            VRECdbxp(upirec)=VRECdbxp(upcrec);
	    VRECrc(upirec)=RCNORMAL;
#if 0
	    memset(VMFX(upirec),0x00,LEADER);
	    VMFRmfn(upirec)=1L; VMFRmfrl(upirec)=VMFRbase(upirec)=LEADER;
#endif
	    memmove(VMFX(upirec),VMFX(irec),VMFRmfrl(irec));
	    VMFRmfn(upirec)=VMF0nxtmfn(upcrec);
	    
	    if (q2) if (fldupdat(upirec,q2)) fatal(p);
	    
	    recupdat(upcrec,upirec);
	    
	    FREE(vrecp[upcrec]); vrecp[upcrec]=NULL; nrecs--;
	    FREE(vrecp[upirec]); vrecp[upirec]=NULL; nrecs--;
	    
	    continue;
	}

	if (strncmp(p,"log=",4) == 0) {
	    q=p+4; if (!*q) continue;
	    loadfile(RDBname,'@',q,wka,rec_maxmfrl,'\0');
            if (cgitrace) printf("+++ log=%s\n",wka);
	    recisis0(wka); record(buffridx,wka,0L);
	    mfn=VMFRmfn(irec); 
	    VMFRmfn(irec)=1; recupdat(buffridx,irec); 
	    VMFRmfn(irec)=mfn;
	    continue;
	}

	if (strncmp(p,"app=",4) == 0) {
	    q=p+4; if (!*q) continue;
	    loadfile(RDBname,'@',q,wka,rec_maxmfrl,'\0');
            if (cgitrace) printf("+++ app=%s\n",wka);
	    record(buffridx,wka,0L);
	    mfn=VMFRmfn(irec); 
	    VMFRmfn(irec)=VMF0nxtmfn(buffridx); recupdat(buffridx,irec); 
	    VMFRmfn(irec)=mfn;
	    continue;
	}
	
	if (strncmp(p,"zer=",4) == 0) {
	    q=p+4; if (!*q) continue;
	    loadfile(RDBname,'@',q,wka,rec_maxmfrl,'\0');
            if (cgitrace) printf("+++ zer=%s\n",wka);
	    recisis0(wka);
	    continue;
	}
	
	if (strncmp(p,"tmpnam=",7) == 0) {
	    if (sscanf(p+7,"%"_LD_,&lv) != 1) continue;
	    if (lv <= 0 || lv > (LONGX)USHRT_MAX) continue;
	    q=gettmpnam(wka);
	    sprintf(buffup,"D%"_LD_" H%"_LD_" %d %s",lv,lv,strlen(q),q);
	    fldupdat(irec,buffup);
	    continue;
	}

	if (strncmp(p,"cipar=",6) == 0) {
	    q=p+6; if (!*q) continue;
	    loadfile(RDBname,'@',q,wka,rec_maxmfrl,'\0');
	    if (recfmt(irec,rec_maxmfrl,wka,buffup,rec_maxmfrl-1L) < 0) fatal(wka);
            dbxcdcip=loadfile(NULL,'\0',buffup,NULL,0L,'\n');
	    continue;
	}
	
	if (strncmp(p,"write=",6) == 0) {
	    q=p+6; if (!*q) continue;
	    loadfile(RDBname,'@',q,wka,rec_maxmfrl,'\0');
	    if (recfmt(irec,rec_maxmfrl,wka,buffup,rec_maxmfrl-1L) < 0) fatal(wka);
	    for (q=buffup; *q; q++) 
	        if (*q != '\r')
	            fprintf(ofp,"%c",*q);
	    continue;
	}
	
	if (strncmp(p,"echo=",5) == 0) {
	    q=p+5; 
	    for (; *q; q++) fprintf(ofp,"%c",*q); fprintf(ofp,"\n");
	    continue;
	}
	
	if (strncmp(p,"fwrite/",7) == 0) {
	    if (sscanf(p+7,"%"_LD_,&lv) != 1) continue;
	    if (lv <= 0 || lv > (LONGX)USHRT_MAX) continue;
	    xtag=(UWORD)lv;
	    q=recfield(wka,irec,xtag,1,"");
	    if (!*q) continue;
	    fp=fopen(wka,"w");
	    if (fp == (FILE *)NULL) continue;
	    q=strchr(p+7+1,'='); if (!*q) continue;
	    q++; if (!*q) continue;
	    loadfile(RDBname,'@',q,wka,rec_maxmfrl,'\0');
	    if (recfmt(irec,rec_maxmfrl,wka,buffup,rec_maxmfrl-1L) < 0) fatal(wka);
	    for (q=buffup; *q; q++) if (*q != '\r') fprintf(fp,"%c",*q);
	    fclose(fp);
	    continue;
	}

	if (strncmp(p,"fldupdat=",9) == 0) {
	    q=p+9; if (!*q) continue;
	    loadfile(RDBname,'@',q,buffup,rec_maxmfrl,'\0');
	    if (fldupdat(irec,buffup)) fatal(buffup);
	    continue;
	}
	if (strncmp(p,"proc=",5) == 0) {
	    q=p+5; if (!*q) continue;
	    loadfile(RDBname,'@',q,wka,rec_maxmfrl,'\0');
	    if (recfmt(irec,rec_maxmfrl,wka,buffup,rec_maxmfrl-1L) < 0) fatal(wka);
	    if (fldupdat(irec,buffup)) fatal(buffup);
	    continue;
	}

	if (isdigit(*p)) { /* number= */
	 if (sscanf(p,"%"_LD_"%c",&xlv,&xchr) == 2) {
	  for (q=p; isdigit(*q); ) q++;
	  if (*q == '=' || (*q == '+' && *(q+1) == '=')) {
	    if (*q == '+') q++; q++;
	    *bufsizbuff='\0';
	    if (xchr == '=') sprintf(bufsizbuff,"D%"_LD_,xlv);
	    *wka='\0'; 
	    if (*q) {
	        loadfile(RDBname,'@',q,wka,rec_maxmfrl,'\0');
	        if (recfmt(irec,rec_maxmfrl,wka,buffup,rec_maxmfrl-1L) < 0) {
		    strcpy(buffup,wka+fmt_errof);
	        }
                for (*(xwp=wka)='\0', xbp=buffup; *xbp; ) {
                    for (xnby=0, q=xbp; *q; q++)
                        if (*q == '\n' || *q == '\r') {
                            *q++ = '\0';
                            while (*q == '\n' || *q == '\r') *q++ = '\0';
                            break;
                        } else xnby++;
                    if (xnby) {
		        sprintf(xwp,"H%"_LD_" %"_LD_" ",xlv,xnby); xwp+=strlen(xwp);
			memmove(xwp,xbp,(size_t)xnby); xwp+=xnby; *xwp='\0';
                    }
                    xbp = q;
                }
	    }
	    if (*bufsizbuff) if (fldupdat(irec,bufsizbuff)) fatal(bufsizbuff);
	    if (*wka) if (fldupdat(irec,wka)) fatal(wka);
	    continue;
	  }
	 }
	}

#if CICGXGIZ
	if (strncmp(p,"gizmo=",6) == 0 || strncmp(p,"gizmo/?=",8) == 0) {
	    if (strncmp(p,"gizmo/?=",8) == 0) gizminfo=1; else gizminfo=0;
	    gizmread(p+6,&gizmap,buffridx);
	    recgizmo(irec,gizmap);
	    ngizmos++;
	    if (gizminfo) {
	      q=buffup; lv=10000/*U*/+ngizmos*100;
	      sprintf(q,"A%"_LD_"\n%s\n",lv,gizmap->gdbnp);	       q+=strlen(q);
	      for (n=0; n <= 255; n++) { /* header list */
		gizmp=gizmap->ghdrp[n];
		if (!gizmp) continue;
		for (; gizmp; gizmp=gizmp->nextp) { /* list */
		    if (gizmp->nused == 0) continue;
		    sprintf(q,"A%"_LD_"#%"_LD_"#",(lv+10),gizmp->nused); q+=strlen(q);
		    sprintf(q,"H%"_LD_" %d ",(lv+1),gizmp->isize);   q+=strlen(q);
		    memmove(q,gizmp->ipatt,n=gizmp->isize);       q+=n;
		    sprintf(q,"H%"_LD_" %d ",(lv+2),gizmp->osize);   q+=strlen(q);
		    memmove(q,gizmp->opatt,n=gizmp->osize);       q+=n;
		    sprintf(q,"A%"_LD_"#%d#",(lv+31),gizmp->isize);  q+=strlen(q);
		    sprintf(q,"A%"_LD_"#%d#",(lv+32),gizmp->osize);
		    fldupdat(irec,buffup);
		} /* end list */
	      } /* end header */
	    }
	    gizflush(gizmap);
	    continue;
	}
#endif /* CICGXGIZ */

        if (strncmp(p,"label=",6) == 0) continue;

        if (strcmp(p,"exit") == 0) break;

	fatal(p);

    } /* end for iocc */



    /*
	Process format lines
    */
    fmtp=recfield(buffup,irec,TAGMXPFT,1,"");
    if (*fmtp) {
	if (recfmt(irec,rec_maxmfrl,fmtp,wka,rec_maxmfrl-1L) < 0) fatal(fmtp);
        if (cgitrace) printf("recfmt:\n%s\n",wka);
	for (p=fmtp=wka; *fmtp; ) {
	    if (*p != '\r') if (*p != '\n') if (*p) {p++; continue;}
	    while (*p == '\r' || *p == '\n') *p++='\0';
	    if (*fmtp) {
                argv[*argc]=(char *) malloc(sizeof(char) * (strlen(fmtp) + 1));
		strcpy(argv[*argc],fmtp);
		*argc= *argc+1;
	    }
	    fmtp=p;
	} /* end of for fmtp */
    }
    if (cgitrace) if (*argc-1) {
        printf("Content-type: text/html%c%c",10,10);
        printf("CISIS/CGI %s produced the following %d parameters:<p>%c",cicgi_p,*argc-1,10);
        printf("<ul>%c",10);
        for(n=1; n < *argc; n++) printf("<li> <code>%s</code>%c",argv[n],10);
        printf("</ul>%c",10);
    }


    /*
	Release all CISIS runtime data
    */
    for (lv=0; lv < maxnrec; lv++)
	if (vrecp[lv]) {
	    FREE(vrecp[lv]); vrecp[lv]=(RECSTRU *)NULL; nrecs--;
	}
    for (lv=0; lv < maxntrm; lv++)
	if (vtrmp[lv]) {
	    FREE(vtrmp[lv]); vtrmp[lv]=(TRMSTRU *)NULL; ntrms--;
	}
    for (lv=0; lv < maxndbx; lv++) 
        if (vdbxp[lv])
	    dbxflush(VDBXname(lv));

    if (cicgi_dbnp) FREE(cicgi_dbnp);
    
    strcpy(fatal_iomsg,""); /* notrap fatal() */

    return(retc);
}

#endif /* CICG1 */


#if CICPP
int CGIClass :: cigt_function(char *varp,
                              char *action)
#else /* CICPP */
#if ANSI
int cigt_function(char *varp, char *action);
#else
int cigt_function();
#endif

int cigt_function(varp, action)
char *varp;
char *action;
#endif /* CICPP */
{
   char *def_pfxtmp="tmp";
   int def_tmps=1;

   char *pip,*p,*op,*cp;
   char area[200];
   int loop;
   
   pip=getenv(varp);
   if (!pip) return(0);
   pip=strdup(pip);
   
   for (p=pip; *p; p=cp+1) {
       op=strchr(p,'[');
       if (!op) break;
       op++;
       cp=strchr(op,']');
       if (!cp) break;
       *cp='\0';
       if (strncmp(op,"pfxtmp=",7) == 0 || strncmp(op,"pfxtmp~",7) == 0) {
           def_pfxtmp=op+7;
           continue;
       }
       if (strncmp(op,"tmps=",5) == 0 || strncmp(op,"tmps~",5) == 0) {
           def_tmps=atoi(op+5);
           continue;
       }
   }
   
   if (strcmp(action,"del") == 0) 
       for (loop=1; loop <= def_tmps; loop++) {
           sprintf(area,"%s%d",def_pfxtmp,loop);
           p=getenv(area);
	   if (p) {
	       /* unset */
               strcat(area,"=");
	       putenv(area);
	       /* delete */
#if PC
	       sprintf(area,"del %s.$$$",p);
#else
	       sprintf(area,"rm %s",p);
#endif
	       system(area);
	   }
       } 

   if (strcmp(action,"set") == 0) 
       for (loop=1; loop <= def_tmps; loop++) {
           p=gettmpnam(NULL);
           sprintf(area,"%s%d=%s",def_pfxtmp,loop,p);
           putenv(strdup(area));
       }   

   free(pip);
      
   return(def_tmps);
}

#endif /* CICGI */
