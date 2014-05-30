#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "cisis.h"

#if UNIX || MSC
#ifndef MAXPATH
#define MAXPATH   260
#endif /* MAXPATH */
#endif /* UNIX || MSC */

#if PC
#if MSC
#include <direct.h>
#else
#include <dir.h>
#endif /* MSC */
#endif


/* ========================== Origem: CIDBX.C ========================== */

#if CICPP || CIDBX_CISISX_SOURCE

#if MSC
#include <share.h>
#include <sys\locking.h>
#endif

#if CICPP
#define dbxcinet  xdbxcinet /* internal use */
#define dbxilock  xdbxilock /* internal use */
#define dbxinit   xdbxinit  /* internal use */
#define dbxstorp  xdbxstorp /* internal use */
#define dbxsrchp  xdbxsrchp /* internal use */
#define dbxopen   xdbxopen  /* internal use */
#define dbxopenw  xdbxopenw /* internal use */
#define dbxopenc  xdbxopenc /* internal use */

#define fatal     xfatal    /* internal use */

#define dbxciset  xdbxciset /* internal use */
#define dbxcipar  xdbxcipar /* internal use */
#define dbxgipar  xdbxgipar /* internal use */
#define gizflush  xgizflush /* internal use */
#define decflush  xdecflush /* internal use */
#define loadfile  xloadfile /* internal use */
#define loadstw   xloadstw  /* internal use */
#define dbxtmpnm  xdbxtmpnm /* internal use */
#define cntread   xcntread  /* internal use */
#define invsetup  xinvsetup /* internal use */
#define noderead  xnoderead /* internal use */
#define leafread  xleafread /* internal use */


#endif /* CICPP */

#define YES         "y"         /* dxbcipar/xxxtrace=y */


/* ------------------------- cidbx.c/fatal() ---------------------- */
#if FATRAP
#if !CICPP
/* global */
int     fatal_errcod = 1;                       /* cisis fatal() error code */
char    fatal_iomsg[MAXFATML+1] = { "" };       /* cisis fatal() flag/msg */
jmp_buf fatal_jumper;                           /* cisis fatal() longjmp() */
#endif /* !CICPP */
#endif /* FATRAP */

#if WWWISIS
#if !CICPP
#if !FATRAP
char    fatal_iomsg[MAXFATML+1] = { "" };       /* cisis fatal() flag/msg */
#endif /* !FATRAP */
#endif /* CICPP */
#if ANSI
void wwwi_fatal(unsigned char *msgp);
#else /* ANSI */
void wwwi_fatal();
#endif /* ANSI */
#endif /* WWWISIS */

#if CIAPI_SOURCE && !BUGciapi_fatal
extern "C"
{
extern void ciapi_fatal(unsigned char *msgp);
}
#endif /* CIAPI_SOURCE */


#if CICPP
void CISISX :: xfatal(char *msgp)
#else /* CICPP */
void fatal(msgp)                                                      /*
----------
		    fatal: msg;
		    abort
								      */
char *msgp;     /* ptr msg */
#endif /* CICPP */
{

#if CIAPI_SOURCE && !BUGciapi_fatal
    ciapi_fatal((unsigned char*)msgp);
#else /* CIAPI_SOURCE */
#if WWWISIS && !defined(XIS_SERVER)
    wwwi_fatal((unsigned char*)msgp);
#else /* WWWISIS */

#if FATRAP
    int msgl;

    if (fatal_iomsg[0]) {
	if (msgp != fatal_iomsg) {
	    msgl=strlen(msgp); if (msgl >= MAXFATML) msgl=MAXFATML;
	    strncpy(fatal_iomsg,msgp,msgl); fatal_iomsg[msgl]='\0';
	}
	longjmp(fatal_jumper,1 /*fatal_errcod*/);
    }
#endif /* FATRAP */

#ifdef USE_ERROR_SYS
    errsys->sys_error(errno,msgp);
#endif /* !USE_ERROR_SYS */

#if CICPP && !CIAPI_SOURCE
 Fatal Fat;

 Fat.Fa_ECode = -301;
 Fat.Fa_Abort = 1;
 strcpy(Fat.Fa_EMess, msgp);
 throw (Fat);
#endif /* CICPP && !CIAPI_SOURCE */

#if BEFORE20000323           /* because some BRM's GCC - ask rpiva */
    fprintf(cistderr,"\nfatal: %s\n\n",msgp);
#else
    fprintf(stderr,"\nfatal: %s\n\n",msgp);
#endif /* BEFORE20000323 */
    exit(1); /* abort(); */
#endif /* WWWISIS */
#endif /* CIAPI_SOURCE */
}
/* ------------------------- cidbx.c/fatal() ---------------------- */


#if MULTI

#if CICPP
int CISISX :: xdbxcinet(char *dbnamp)
#else /* CICPP */
int dbxcinet(dbnamp)                                                  /*
------------
		    retorna MONONETS, FULLNETS ou MASTNETS
								      */
char *dbnamp;
#endif /* CICPP */
{
    int multi;
    char *p,parname[CIMPL+1];

    multi=cipnetws;

#if CIPAR
    if (dbnamp) {
	strcpy(parname,dbnamp);
	strcat(parname,".net");
	p=dbxcipar(NULL,parname,'=');
    }
    else {
	dbnamp="default";
	p=dbxcipar(NULL,"14",'=');
	if (!dbxcipok) p=dbxcipar(NULL,"netws",'=');
    }
    if (!dbxcipok) multi=cipnetws; else
    if (strcmp(p,"0") == 0  || strcmp(p,"single") == 0) multi=MONONETS; else
    if (strcmp(p,"1") == 0  || strcmp(p,"full"  ) == 0) multi=FULLNETS; else
    if (strcmp(p,"2") == 0  || strcmp(p,"master") == 0) multi=MASTNETS;

#endif /* CIPAR */

#if DBXTRACE
    if (dbxtrace)
	printf("dbxcinet - dbnamp='%s'  multi=%d\n",dbnamp,multi);
#endif

    return(multi);
}

#endif /* MULTI */

#if CICPP
int CISISX :: xdbxilock(DBXSTRU *idbxp,
                        char *mdbnp,
			char *typ)
#else /* CICPP */
int dbxilock(idbxp,mdbnp,typ)
DBXSTRU *idbxp;
char *mdbnp;
char *typ;
#endif /* CICPP */
{
    DBXSTRU *dbxp;
    char m0area[sizeof(M0STRU)];
    int count;

    if (dbxtrace) printf("+++ dbxilock - %s %d\n",typ,dbxiloop);

    if (strcmp(typ,"L") == 0) {

	dbxp=idbxp;
	if (!dbxp) fatal("dbxilock/L/dbxp");

	if (dbxtrace) printf("+++ dbxilock - I=%s M=%s \n",DBXname,mdbnp);

	dbxinvmp(dbxp);
	dbxopenw(DBXname,DBXname,ix1extp,&DBIifopn,&DBIifopw,"dbxilock/L/ifopn/w");

	dbxp=dbxstorp(mdbnp);
	dbxopenw(mdbnp,mdbnp,mx1extp,&DBXmsopn,&DBXmsopw,"dbxilock/L/msopn/w");

	for (count=dbxiloop; count--; ) {

	    if (multrace) printf("dbxilock - L %s %d \n",DBXname,count);

#if !CICPP
	    if (dbxsleep) if (count+1 < dbxiloop) sleep(dbxsleep);
#endif /* CICPP */

	    /* lock MF */
	    if (dbxflock(dbxp,"M") != 0) continue;

	    /* "wait" MF/EWL (just once!) */
	    if (dbxwlock(dbxp,m0area,1) != 0) continue; /* unlock MF */

	    /* "wait" another's DEL (allow user's DEL) */
	    if (((M0STRU *)m0area)->m0mfcxx2 > 1 ||
	       (((M0STRU *)m0area)->m0mfcxx2 == 1 && !DBXdelxx)) {
		if (dbxulock(dbxp,"M")) fatal("dbxilock/L/unlock/0");
	        continue;
	    }

	    /* lock IF (2-phase locking!) */
	    if (dbxflock(idbxp,"I") != 0) {
		if (dbxulock(dbxp,"M")) fatal("dbxilock/L/unlock/1");
		continue;
	    }

	    /* update the control record */
	    DBXewlxx=((M0STRU *)m0area)->m0mfcxx3 = 1;	/* LOCK/EWL */
	    if (LSEEK64(DBXmsopn,0L,SEEK_SET) != 0) fatal("dbxilock/L/lseek");
#if DBXMSTXL /* AOT/HB 21/08/2002 */
    if (DBXmstxl) {
		((M0STRU *)m0area)->m0mftype += DBXmstxl * 256;
    }
#endif
#if CNV_PCBINUM
	    memmove(cnv_pcbuff,m0area,sizeof(M0STRU));
	    ConvertMST_CTLSTRUCT(cnv_pcbuff);
	    if (CIWRITE(DBXmsopn,cnv_pcbuff,sizeof(M0STRU)) != sizeof(M0STRU))
#else
	    if (CIWRITE(DBXmsopn,m0area,sizeof(M0STRU)) != sizeof(M0STRU))
#endif
		fatal("dbxilock/L/write");

	    /* unlock MF */
	    if (dbxulock(dbxp,"M")) fatal("dbxilock/L/unlock/2");

	    return(0);					/* IF/MF are granted */
	}
	if (multrace)
	    printf("<L> Database %s is locked by another user\n",DBXname);
#if SHTEST
	printf(".."); if (*gets(shtest)) if (strcmp(shtest,"x")) system(shtest); else fatal("dbxilock/shtest");
#endif
	return(RCLOCK);
    }


    if (strcmp(typ,"UM") == 0) {

	if (dbxtrace) printf("+++ dbxilock - M=%s \n",mdbnp);
	dbxp=dbxstorp(mdbnp);

	for (count=dbxiloop; count--; ) {
	    if (multrace) printf("dbxilock - UM %s %d \n",DBXname,count);

	    /* lock MF */
	    if (dbxflock(dbxp,"M") != 0) continue;

	    /* get control record via dbxwlock() */
	    if (dbxwlock(dbxp,m0area,1)) fatal("dbxilock/UM/dbxwlock");

	    /* MF/EWL has to be on */
	    if (((M0STRU *)m0area)->m0mfcxx3 == 0 || !DBXewlxx)
	        fatal("dbxilock/UM/EWL is off");

	    /* update the control record */
	    DBXewlxx=((M0STRU *)m0area)->m0mfcxx3 = 0;	/* UNLOCK/EWL */
	    if (LSEEK64(DBXmsopn,0L,SEEK_SET) != 0) fatal("dbxilock/UM/lseek/2");
#if DBXMSTXL /* AOT/HB 21/08/2002 */
    if (DBXmstxl) {
		((M0STRU *)m0area)->m0mftype += DBXmstxl * 256;
    }
#endif
#if CNV_PCBINUM
	    memmove(cnv_pcbuff,m0area,sizeof(M0STRU));
	    ConvertMST_CTLSTRUCT(cnv_pcbuff);
	    if (CIWRITE(DBXmsopn,cnv_pcbuff,sizeof(M0STRU)) != sizeof(M0STRU))
#else
	    if (CIWRITE(DBXmsopn,m0area,sizeof(M0STRU)) != sizeof(M0STRU))
#endif
		fatal("dbxilock/UM/write");

	    /* unlock MF */
	    if (dbxulock(dbxp,"M")) fatal("dbxilock/UM/unlock");

	    return(0);
	}
	if (multrace)
	    printf("<U> Database %s is locked by another user\n",DBXname);
#if SHTEST
	printf(".."); if (*gets(shtest)) if (strcmp(shtest,"x")) system(shtest); else fatal("dbxilock/shtest");
#endif
	return(RCLOCK);
    }

    if (strcmp(typ,"UI") == 0) {
	dbxp=idbxp;
	if (dbxtrace) printf("+++ dbxilock - I=%s \n",DBXname);
	/* unlock IF */
	dbxuclos= -dbxuclos; /* do not reopen */
	if (dbxulock(dbxp,"I")) fatal("dbxilock/UI/unlock");
	dbxuclos= -dbxuclos; /* reset */
	return(0);
    }

    fatal("dbxilock/type");
    return(-1); /* no wrn */
}


#if CICPP
//void DBXSTRU :: xdbxinit(void)
void CISISX :: xdbxinit(void)
#else /* CICPP */
void dbxinit()                                                        /*
------------
		    inicializa vdbxp
								      */
#endif /* CICPP */
{
    char *p;
#if CICPP
#else /* CICPP */
    int callrecinit=1;
    int calltrminit=1;
#endif /* CICPP */

#define CIAOTWCT 1
#if CIAOTWCT
    if (!isiswctot) {
	memset(isiswctab,0x00,256);
	for (isiswctot=0; isisactab[isiswctot]; isiswctot++)
	    isiswctab[isisactab[isiswctot]]=1;
    }
#endif

    for (ndbxs=maxndbx;ndbxs--;)
	    vdbxp[ndbxs]=NULL;
    ndbxs=0;

#if CICPP
#else /* CICPP */
    /* init vrecp - recinit() - if not init - AOT, 27/10/2005 */
    if (callrecinit) if (!nrecs) {
        for (nrecs=maxnrec;nrecs--;) vrecp[nrecs]=NULL;
        nrecs=0;
    }
    /* init vtrmp - trminit() - if not init - AOT, 27/10/2005 */
    if (calltrminit) if (!ntrms) {
        for (ntrms=maxntrm;ntrms--;) vtrmp[ntrms]=NULL;
        ntrms=0;
    }
#endif /* CICPP */


#if MULTI
    cipnetws=dbxcinet(NULL);
#endif
#if CIPAR
#if MULTI
    p=dbxcipar(NULL,"flock",'=');
    if (dbxcipok)
	if (sscanf(p,"%d",&dbxfloop) != 1) fatal("dbxinit/cipar/flock");
    p=dbxcipar(NULL,"wlock",'=');
    if (dbxcipok)
	if (sscanf(p,"%d",&dbxwloop) != 1) fatal("dbxinit/cipar/wlock");
    p=dbxcipar(NULL,"ilock",'=');
    if (dbxcipok)
	if (sscanf(p,"%d",&dbxiloop) != 1) fatal("dbxinit/cipar/ilock");
    p=dbxcipar(NULL,"uclos",'=');
    if (dbxcipok)
	if (sscanf(p,"%d",&dbxuclos) != 1) fatal("dbxinit/cipar/uclos");
    p=dbxcipar(NULL,"ewlrc",'=');
    if (dbxcipok)
	if (sscanf(p,"%d",&dbxewlrc) != 1) fatal("dbxinit/cipar/ewlrc");
    p=dbxcipar(NULL,"sleep",'=');
    if (dbxcipok)
	if (sscanf(p,"%d",&dbxsleep) != 1) fatal("dbxinit/cipar/sleep");
#endif
#if DBXMSTXL
    p=dbxcipar(NULL,"mstxl",'=');
    if (dbxcipok) {
        if (p[strlen(p)-1] == 'G') {
            int num=atoi(p);
            int cap = 1;
            cipmstxl = 1;
            while (num != cap) {
                if (num < cap) {
                    fatal("dbxinit/cipar/mstxl/G");
                }
                cipmstxl++;
                cap *= 2;
            }
        } else {
            if (sscanf(p,"%d",&cipmstxl) != 1) fatal("dbxinit/cipar/mstxl");
            /*if ((cipmstxl < 0) || (cipmstxl > 4)) fatal("dbxinit/mstxl"); > 4G*/
            if (cipmstxl < 0) fatal("dbxinit/mstxl");
            if (cipmstxl == 4) cipmstxl=3;
        }
    }
#endif
#ifndef INCLOADX   /* AOT, 22/05/2002 */
#define INCLOADX 0
#endif
#if INCLOADX
#define GETACUCTAB 1
#endif
#if GETACUCTAB  /* AOT/HB, 15/04/2002 */
    p=dbxcipar(NULL,"uctab",'=');
    if (dbxcipok) {
#if GETACUTABANSI
            if (strcmp(p,"ansi") == 0) {
                memmove(isisuctab,ansiuctab,256/*sizeof(ansiuctab)*/);
            } else {
#endif
                if (!loaductb(NULL,isisuctab,p)) fatal("dbxinit/uctab");
#if GETACUTABANSI
            }
#endif
    }
    p=dbxcipar(NULL,"actab",'=');
    if (dbxcipok) {
            unsigned char ubuffup[256];
            memset(isiswctab,0x00,256);
#if GETACUTABANSI
            if (strcmp(p,"ansi") == 0) {
                for (isiswctot=0; ansiactab[isiswctot]; isiswctot=isiswctot+1)
                    isiswctab[ansiactab[isiswctot]]=1;
            } else {
#endif
                if (!loadactb(NULL,ubuffup,p)) fatal(p);
                for (isiswctot=0; ubuffup[isiswctot]; isiswctot=isiswctot+1)
                    isiswctab[ubuffup[isiswctot]]=1;
#if GETACUTABANSI
            }
#endif
    }
#endif /* GETACUCTAB */
    p=dbxcipar(NULL,"mflush",'='); if (dbxcipok) sscanf(p,"%d",&dbxopt_mflush);
    p=dbxcipar(NULL,"mclose",'='); if (dbxcipok) sscanf(p,"%d",&dbxopt_mclose);
    p=dbxcipar(NULL,"iflush",'='); if (dbxcipok) sscanf(p,"%d",&dbxopt_iflush);
    p=dbxcipar(NULL,"maxmfrl",'='); if (dbxcipok) sscanf(p,"%"_LD_,&rec_maxmfrl); if (rec_maxmfrl < (LONGX)MSNVSPLT || (RECHSIZE+rec_maxmfrl) > ALLOMAXV) rec_maxmfrl=MAXMFRL;
#if CICPP

#else /* CICPP */
    fmt_fsiz=rec_maxmfrl;
#endif /* CICPP */
    p=dbxcipar(NULL,"mstload",'='); if (dbxcipok) sscanf(p,"%"_LD_,&rec_mstload);
    p=dbxcipar(NULL,"invload",'='); if (dbxcipok) sscanf(p,"%"_LD_,&trm_invload);
    if (strcmp(dbxcipar(NULL,"partrace",'='),YES) == 0) partrace=1;
    if (strcmp(dbxcipar(NULL,"dbxtrace",'='),YES) == 0) dbxtrace=1;
    if (strcmp(dbxcipar(NULL,"rectrace",'='),YES) == 0) rectrace=1;
    if (strcmp(dbxcipar(NULL,"dectrace",'='),YES) == 0) dectrace=1;
    if (strcmp(dbxcipar(NULL,"trmtrace",'='),YES) == 0) trmtrace=1;
    if (strcmp(dbxcipar(NULL,"b40trace",'='),YES) == 0) b40trace=1;
    if (strcmp(dbxcipar(NULL,"b50trace",'='),YES) == 0) b50trace=1;
    if (strcmp(dbxcipar(NULL,"fmttrace",'='),YES) == 0) fmttrace=1;
    if (strcmp(dbxcipar(NULL,"fsttrace",'='),YES) == 0) fsttrace=1;
    if (strcmp(dbxcipar(NULL,"multrace",'='),YES) == 0) multrace=1;
    if (strcmp(dbxcipar(NULL,"cgitrace",'='),YES) == 0) cgitrace=1;
    if (strcmp(dbxcipar(NULL,"adddel"  ,'='),YES) == 0) bugadddel=0;
    if (strcmp(dbxcipar(NULL,"what"    ,'='),YES) == 0) printf("%s",cicopyr("?"));
#endif /* CIPAR */
}

#if CICPP
//DBXSTRU * DBXSTRU :: xdbxsrchp(char *dbnamp)
DBXSTRU * CISISX :: xdbxsrchp(char *dbnamp)
#else /* CICPP */
DBXSTRU *dbxsrchp(dbnamp)                                             /*
-----------------
		    procura strcmp(dbxp->dbxname,dbnamp) == 0;
		    retorna NULL ou dbxp
								      */
char *dbnamp;       /* ptr nome base de dados */
#endif /* CICPP */
{
    DBXSTRU *dbxp;
    LONGX idbx;

    if (!ndbxs)
	return(NULL);

    for (idbx=ndbxs; idbx--; ) {
	dbxp=vdbxp[idbx];
	if (strcmp(DBXname,dbnamp) == 0) return(dbxp);
    }

    return(NULL);
}

#if CICPP
DBXSTRU * CISISX :: xdbxstorp(char *dbnamp)
#else /* CICPP */
DBXSTRU *dbxstorp(dbnamp)                                             /*
-----------------
		    procura dbxsrchp(dbnamp);
		    se nao achou, aloca e inicializa celula;
		    retorna dbxp
								      */
char *dbnamp;       /* ptr nome base de dados */
#endif /* CICPP */
{
    DBXSTRU *dbxp;
#if DBXTRAC0
    LONGX idbx;
#endif /* DBXTRAC0 */
#if !CICPP
    LONGX lval;
#endif /* CICPP */

    if (!ndbxs)
	dbxinit();

    if (!dbnamp) fatal("dbxstorp/dbnamp"); /* AOT,HB 31/07/2002 */

    if ((dbxp=dbxsrchp(dbnamp)) != NULL)
	return(dbxp);

    if (ndbxs >= maxndbx)
	fatal("dbxstorp/overflow");

#if CICPP
    try { dbxp = new DBXSTRU(this); }
    catch (BAD_ALLOC) { dbxp=(DBXSTRU *)ALLONULL; }
#else /* CICPP */
    lval=sizeof(DBXSTRU);
    if (lval > ALLOMAXV)
	fatal("dbxstorp/ALLOMAXV/sizeof(DBXSTRU)");
    dbxp=(DBXSTRU *)ALLOC((ALLOPARM)(sizeof(DBXSTRU)));
#endif /* CICPP */
    if (dbxp == (DBXSTRU *)ALLONULL)
	fatal("dbxstorp/ALLOC");

#if !CICPP
    memset(dbxp,0x00,sizeof(DBXSTRU));
#endif /* CICPP */

#if USE_VDBXP
    vdbxp[ndbxs++]=dbxp;
#else
    vdbxp[ndbxs++]=dbxp;
#endif

    if (strlen(dbnamp) > (CIMPL - 4 - 1))
	fatal("dbxstorp/name");
    strcpy(DBXname,dbnamp);

#if DBXTRAC0
printf("dbxstorp - dbnamp='%s' DBXname='%s' p=%p ndbxs=%"_LD_"\n",
 dbnamp,DBXname,vdbxp[ndbxs-1],ndbxs);
for (idbx=0L; idbx < ndbxs; idbx++)
  printf("+++%"_LD_"/%"_LD_" %s p=%p \n",idbx,ndbxs,VDBXname(idbx),vdbxp[idbx]);
#endif

    DBXmsmfn=0;                                                 /* v3.0 */

#if MULTI
    DBXnetws=dbxcinet(DBXname);
#endif

    DBXmflush= dbxopt_mflush;
    DBXmclose= dbxopt_mclose;
    DBXiflush= dbxopt_iflush;

    return(dbxp);
}

#if CICPP
int CISISX :: xdbxopen(char *gidbnp,
                       char *dbname,
                       char *extp)
#else /* CICPP */
int dbxopen(gidbnp,dbname,extp)                                       /*
-----------
		    abre dbxname.ext;
		    retorna OPEN(dbxname.ext,...);
								      */
char *gidbnp;		/* dbn.par */
char *dbname;		/* string + 4 + 1 */
char *extp;		/* .ext */
#endif /* CICPP */
{
#if !MSC
#if BEFORE20000413
    extern int errno;
#endif
#endif
    int fd;
    char filname[CIMPL+1];

#if DBXTRAC1
printf("dbxopen  - [%s.par] dbname='%s'  len=%d\n",gidbnp,dbname,strlen(dbname));
#endif

    if (!extp) extp="";

    strcpy(filname,dbname);
    strcat(filname,extp);

    strcpy(filname,dbxcipar(gidbnp,filname,'='));
#if DBXTRAC1
printf("dbxopen  - filename='%s'  len=%d\n",filname,strlen(filname));
#endif

    dbxordwr = (dbxopt_ordwr & O_RDWR) ? 1 : 0;   /* dbxopt_ordwr & O_RDWR */

#if MULTI || CDMDL
#if MSC
    fd=OPEN(filname,O_BINARY|dbxopt_ordwr,SH_DENYNO);
#else
    fd=OPEN(filname,O_BINARY|dbxopt_ordwr|O_DENYNONE);
#endif
#else
    fd=OPEN(filname,O_BINARY|dbxopt_ordwr);
#endif

    if (fd == EOF) {
	if (dbxopt_fatal) {                                     /* v4.2 */
#if 0
#ifdef USE_ERROR_SYS
            errsys->sys_error(filname);
#else
            if (dbxopt_errno)
                fprintf(cistderr,"dbxopen: '%s' errno=%d",filname,errno);
#endif
#if FATRAP
	    if (fatal_iomsg[0]) {
		sprintf(fatal_iomsg,"dbxopen: %s (%d)",filname,errno);
		fatal(fatal_iomsg);
	    }
#endif
#endif /* 0 */
#if !CICPP
#if WWWISIS
	    sprintf(fatal_iomsg,"dbxopen: %s (%d)",filname,errno);
	    fatal(fatal_iomsg);
#else /* !WWWISIS */
            if (dbxopt_errno)
                fprintf(cistderr,"dbxopen: '%s' errno=%d",filname,errno);
#if FATRAP
	    if (fatal_iomsg[0]) {
		sprintf(fatal_iomsg,"dbxopen: %s (%d)",filname,errno);
		fatal(fatal_iomsg);
	    }
#endif /* FATRAP */
#endif /* !WWWISIS */
#endif /* !CICPP */
#ifdef ISIS_DLL
		char fatal_msg[256];
	    sprintf(fatal_msg,"dbxopen: %s (%d)",filname,errno);
	    fatal(fatal_msg);
#endif /* ISIS_DLL */

	    fatal("dbxopen/open");
	}
    }
    dbxopt_fatal=1;     /* set default value */

    if (dbxtrace)
	printf("dbxopen  - %s fd=%d [%s]\n",filname,fd,dbxordwr?"RW":"R");

    if (fd == 0)
	fatal("dbxopen/fd=0");

    return(fd);
}


#if CICPP
int CISISX :: xdbxopenw(char *gidbnp,
			char *dbname,
                        char *extp,
			int  *opnp,
			int  *opwp,
			char *errmsgp)
#else /* CICPP */
int dbxopenw(gidbnp,dbname,extp,opnp,opwp,errmsgp)                    /*
------------
		    abre dbxname.ext para O_RDWR;
		    retorna OPEN(dbxname.ext,...);
								      */
char *gidbnp;		/* dbn.par */
char *dbname;		/* string + 4 + 1 */
char *extp;		/* .ext */
int *opnp;
int *opwp;
char *errmsgp;
#endif /* CICPP */
{
    int n,opn=0,opw=0;

    if (opnp) opn= *opnp;
    if (opwp) opw= *opwp;
    if (!errmsgp) errmsgp="dbxopenw";

#if 1 /* DBXTRACw */
if (dbxtrace)
  printf("dbxopenw  - [%s.par] dbname='%s'  extp=%s   opn/opw=%d/%d   '%s'\n",
    gidbnp,dbname,extp,opn,opw,errmsgp);
#endif

    if (opw) {
	if (opn <= 0) fatal("dbxopenw/opw/opn");
	return(opn);
    }

    if (opn > 0) CLOSE(opn);

    n=dbxopt_ordwr; dbxopt_ordwr=O_RDWR; dbxopt_fatal=0;
#if HASTOBETHESAME
    if (opn > 0) {
	if (dbxopen(gidbnp,dbname,extp) != opn) fatal(errmsgp);
    } else
#endif
    opn=dbxopen(gidbnp,dbname,extp); dbxopt_ordwr=n;
    if (opn <= 0) if (errmsgp) fatal(errmsgp);
    opw++;

    if (opnp) *opnp=opn;
    if (opwp) *opwp=opw;

#if DBXTRACw
printf("dbxopenw  - dbname='%s'  extp=%s   opn/opw=%d/%d \n",
 dbname,extp,opn,opw);
#endif

    return(opn);
}

#if CICPP
char *CISISX :: xdbxopenc(char *gidbnp,
                          char *filnamp,
                          int  *opnp,
                          int  *opwp,
                          char *errmsgp,
                          int   xcreate,
                          int   xappend)
#else /* CICPP */
char *dbxopenc(gidbnp,filnamp,opnp,opwp,errmsgp,xcreate,xappend)                      /*
------------
		    create filnamp + abre para O_RDWR;
		    retorna filnamp ou filnamp+strlen("create=");
								      */
char *gidbnp;		/* dbn.par */
char *filnamp;
int *opnp;
int *opwp;
char *errmsgp;
int xcreate;
int xappend;
#endif /* CICPP */
{
    char *p;
    int fd,create=0,append=0,chkc=0;

#if DBXTRACw
printf("dbxopenc  - [%s.par] '%s' \n",gidbnp,filnamp);
#endif

    p=filnamp;
    while (strncmp(p,"append=",7) == 0 || strncmp(p,"create=",7) == 0) {
	if (strncmp(p,"append=",7) == 0) { p+=7; append=1; }
	if (strncmp(p,"create=",7) == 0) { p+=7; create=1; } else chkc=1;
    }
    filnamp=p;

    p=dbxcipar(gidbnp,filnamp,'=');

    if (chkc || xappend) {
        dbxopt_fatal=0;	fd=dbxopen(NULL,p,"");
	    if (fd <=0 ) create=1; else CLOSE(fd);
    }

    if (create || xcreate) {
	    fd=CREAT(p,PERMIS);
	    if (fd <= 0) if (errmsgp) fatal(errmsgp);
	    CLOSE(fd);
    }

    fd=dbxopenw(NULL,p,"",NULL,opwp,errmsgp);
    if (fd > 0)
        if (append || xappend) LSEEK64(fd,0L,SEEK_END);
	else LSEEK64(fd,0L,SEEK_SET);	/* rewind */

    *opnp=fd;

#if DBXTRACw
printf("dbxopenc  - '%s' =  %d \n",p,fd);
#endif

    return(p);
}

/*
	gidbnp: db name | "" (to use aknamp.par) | NULL (skip dbn.par)
	argkey: file name | <parmno>=
	*namlenp: file name length
	*extlenp: .extension length
*/

#if GIPAR
#if CICPP
char * CISISX :: xdbxgipar(char *gidbnp,
			   char *ginamp,
			   int *namlenp,
			   int *extlenp)
#else /* CICPP */
char *dbxgipar(gidbnp,ginamp,namlenp,extlenp)
char *gidbnp;          				/* dbn / "" / NULL */
char *ginamp;          				/* name.ext / <parmno>= */
int *namlenp;          				/*  */
int *extlenp;          				/*  */
#endif /* CICPP */
{
    DBXSTRU *dbxp;
    char *p,*q,*nextlinp,*extp,*fnp;
    int parameter,i,n,naml,fnl;
    int ginnn=0;
    LONGX keepndbx,idbx;
    char tmpgikey[sizeof(dbxgikey)];         /* dbxgipar() tmp */

    if (!ginamp || !*ginamp) return(dbxgikey);

    /* setup returning value */
    naml=strlen(ginamp);
    if (naml >= sizeof(dbxgikey)) naml=sizeof(dbxgikey)-1;
    memmove(dbxgikey,ginamp,naml); dbxgikey[naml]='\0';

    extp="";
    for (p=dbxgikey+naml-1, n=naml; n; p--, n--) {
	if (*p == '/') break;
	if (*p == '\\') break;
	if (*p == '.') {
	    extp=p;
	    break;
	}
    }
    if (namlenp) *namlenp=naml;
    if (extlenp) *extlenp=strlen(extp);

    if (!gidbnp) return(dbxgikey);

    /* alloc&load dbxgicip */
    memmove(tmpgikey,dbxgikey,naml);
    tmpgikey[naml-strlen(extp)]='\0';
    keepndbx=ndbxs; dbxp=dbxstorp((*gidbnp)?gidbnp:tmpgikey);
    if (dbxp) {
	if (!DBXgicip) {
	    strcpy(tmpgikey,DBXname);
	    strcat(tmpgikey,".par");
	    DBXgicip=loadfile(NULL,'\0',tmpgikey,NULL,0L,'\n');
	}
    }

    /* flush new vdbxp[] entry */
    if (!DBXgicip) {
	for (idbx=ndbxs-1; idbx>=keepndbx; idbx--)
	    dbxflush(vdbxp[idbx]->dbxname);
        return(dbxgikey);
    }

    n=strlen(dbxgikey); extp="";
    for (p=dbxgikey+n-1; n; p--, n--) {
	if (*p == '/') break;
	if (*p == '\\') break;
	if (*p == '.') {
	    extp=p;
	    break;
	}
    }

    for (i=0; dbxgiext[i]; i++)
#if !BRME
        if (memicmp(dbxgiext[i],extp,strlen(extp)) == 0) {
#else
	if (strcmp(dbxgiext[i],extp) == 0) {
#endif
	    ginnn=dbxgiexn[i];
	    break;
	}

    if (DBXgicip) for (q=DBXgicip; *q; ) {

	for (p=q; *p; nextlinp=p)
	    if (*p == '\n' || *p == '\r') {
	        while (*p == '\n' || *p == '\r') p++;
                nextlinp=p;
		break;
	    } else p++;

	while (isspace(*q)) q++;

	parameter=atoi(q);
	if (parameter == ginnn) {
	    q=strchr(q,'=');
	    if (q) {
	        q++; /* '=' */
		while (isspace(*q)) q++;
		for (n=0, p=q; *p; p++, n++)
		    if (*p == '\n' || *p == '\r') {

			for (p=q+n-1; n; )
			    if (isspace(*p)) { p--; n--;} else break;

			fnp=NULL;
			for (fnl=naml-strlen(extp), p=dbxgikey+fnl-1; fnl > 1; p--, fnl--) {
			    if (*p == '/' || *p == '\\' || *p == ':') { fnp=p+1; break; }
			}
			if (!fnp) fnp=dbxgikey;
			fnl=strlen(p);
			memmove(tmpgikey,fnp,fnl); tmpgikey[fnl]='\0';
			if (fnl+n >= sizeof(dbxgikey)) n=sizeof(dbxgikey)-fnl-1;
		        memmove(dbxgikey,q,n);
		        memmove(dbxgikey+n,tmpgikey,fnl);
                        dbxgikey[n+fnl]='\0';

#if DBGTRACE
  if (dbxtrace) printf("dbxgipar - %s - %s=%s #%d\n",
			gidbnp,ginamp,dbxgikey,ginnn);
#endif
			break;
		    }
		break;
	    }
	}
	else q=nextlinp;
    }

    return(dbxgikey);
}
#endif /* GIPAR */

#if CICPP
int CISISX :: xdbxciset(char *cipfile)
#else /* CICPP */
int dbxciset(cipfile)
char *cipfile;
#endif /* CICPP */
{
    dbxcdcip=NULL;
    if (cipfile) if (*cipfile) {
        dbxcdcip=loadfile(NULL,'\0',cipfile,NULL,EOF,'\n');            // env
        if (!dbxcdcip) return(1); //fatal("dbxciset/cipfile");
    }

    dbxcipfp=NULL;
    memset(dbxcikey,0x00,sizeof(dbxcikey));
    dbxcipok=0;

#if CIWTF
    /* alloc global AWTFUN -      if not alloc - AOT, 19/01/2006 */
    if (!ciawtfp) {
        ciawtfp=wtfnew(0);                                             // new
        if (!ciawtfp) return(2); //fatal("dbxciset/ciawtfp");
    }
#endif /* CIWTF */

    dbxinit();                                                         // par

#if CIWTF
    /* init global AWTFUN -        if not init  - AOT, 20/01/2006 */
    if (!/*awtmaxmfrl*/ciawtfp->globalmaxmfrl) {
        wtfset(0,ciawtfp,dbxcdcip);                                    // set
        if (!/*awtmaxmfrl*/ciawtfp->globalmaxmfrl) return(3); //fatal("dbxciset/ciawtfp/set");
    }
#endif /* CIWTF */

    //dbxcipar(NULL,"eval.1",'=');
    //if (dbxcipok) sss=dbxcikey;                                      // eval

    return(0);
}

#if CICPP
char * CISISX :: xdbxcipar(char *gidbnp,
			   char *argkey,
			   char  argchar)

#else /* CICPP */

char *dbxcipar(gidbnp, argkey, argchar)
char *gidbnp;
char argkey[];
char argchar;

#endif /* CICPP */
{
    int keyl;
    UBYTE *retp;
    char *argkp;

#if CIPAR
    UBYTE *p,*p1,*p2;
    int n,xl;
    UBYTE line[BUFSIZ];
    UBYTE ibuf[BUFSIZ];
    UBYTE *cip;
#endif

    dbxcipok=0;
    argkp=argkey;
#if GIPAR
    if (gidbnp) argkp=dbxgipar(gidbnp,argkey,NULL,NULL);
#else
    gidbnp=gidbnp; /* no wrn */
#endif

    keyl=strlen(argkp);
    if (keyl >= sizeof(dbxcikey)) keyl=sizeof(dbxcikey)-1;
    retp=(UBYTE *)memmove(dbxcikey,argkp,keyl); retp[keyl]='\0';

#if CIPAR /* CIPAR */

#if PC || UNIX
    if (!dbxcdcip && !dbxcipfp) {
	if ((p=(UBYTE *)getenv("CIPAR")) == NULL) p=(UBYTE *)getenv("cipar");
        if (p) if ((dbxcipfp=fopen((CONST char *)p,"r")) == NULL) fatal((char *)p);
    }
#endif

    if (dbxcipfp)
	rewind(dbxcipfp); /* fseek(dbxcipfp,0L,SEEK_SET); */

    *ibuf='\0';
    cip = (dbxcdcip) ? (UBYTE *)dbxcdcip : ((dbxcipfp) ? ibuf : NULL);
/*
    if (dbxcdcip) cip=dbxcdcip;
    else if (dbxcipfp) cip=ibuf; else cip=NULL;
*/

    if (cip) {
	for (p=cip; ; ) {
	    n=1;
	    if (!*p) {
		n=0;
		if (dbxcipfp)
		     if (fgets((char *)(p=cip),BUFSIZ-1,dbxcipfp))
			 if (*p)
			     n=1;
	    }
	    if (!n) break;
#if 0
printf("n=%d *p=%c\n",n,*p);
#endif
	    while (isspace(*p) || *p == ';') p++;
	    for (xl=0; *p; p++) {
		/* if (isspace(*p) || *p == ';') break; RP, 29/03/2001 */
                 if ((isspace(*p) && *p != ' ') || *p == ';') break; /* RP, 29/03/2001 */
		line[xl++] = *p;
	    }
	    if (xl <= keyl)
		for (n=keyl-xl; n >= 0; n--) line[keyl-n]='"';
#if 0
printf("?%s%c - xl=%d=",argkp,line[keyl],xl);
for (n=0; n < keyl; n++) printf("%c(%02x)",line[n],line[n]);
getchar();
#endif
	    if (xl == 0) continue;
	    line[xl]='\0';
	    if (xl == 2) if (line[0] == '/') if (line[1] == '*') break;
#if CIPARIC
	    if (memicmp(line,argkp,n=keyl) == 0) {
#else
	    if (memcmp(line,argkp,n=keyl) == 0) {
#endif
		if (line[n] == argchar) {
		    retp=(UBYTE *)
                        memmove(dbxcikey,line+(n+1),strlen((CONST char *)line+n)); /*'\0'*/
		    dbxcipok=1;
		    break;
		}
	    }
	    for (p1=line, p2=(UBYTE *)argkp; ; p1++, p2++) {
#if CIPARIC
		if (toupper(*p1) == toupper(*p2)) continue;
#else
		if (*p1 == *p2) continue;
#endif
		if (*p1 == '*') {
		    if (*(p1+1) != argchar) continue;
		    if (line[xl-1] != '*')  continue;
		    p1++; p1++;
		    line[xl-1]='\0';
                    retp=(UBYTE *)memmove(dbxcikey,p1,n=strlen((CONST char *)p1));
                    memmove(dbxcikey+n,p2,strlen((CONST char *)p2)+1); /*'\0'*/
		    dbxcipok=1;
		    break;
		}
		break;
	    }
	}
    }

#endif /* CIPAR */

#if UC_EXTENSION
    n=strlen(retp);
    for (p=retp+n-1; n; p--, n--) {
	if (*p == '/') break;
	if (*p == '\\') break;
	if (*p == '.') {
	    for (p=p+1; *p; p++) if (islower(*p)) *p = toupper(*p);
	    break;
	}
    }
#endif

    if (dbxtrace)
        if (partrace || strcmp(argkey,(CONST char *)retp))
	    printf("dbxcipar - %s%c%s\n",argkey,argchar,retp);

    return((char *)retp);
}

#if RECGIZM
#if CICPP
void CISISX :: xgizflush(VGIZPSTRU *vgizmap)
#else /* CICPP */
void gizflush(vgizmap)                                                  /*
-------------
		    seta dbxp;
		    faz close dos arquivos abertos do master file
									*/
VGIZPSTRU *vgizmap;             /* gizmo to be flushed */
#endif /* CICPP */
{
    VGIZPSTRU *nxtp;
    GIZMSTRU *gizmhp,*gizmp,*gizmnp;
    int i;

    if (dbxtrace) printf("gizflush - vgizmap=%p\n",vgizmap);
    for (; vgizmap; vgizmap=nxtp) {

	if (dbxtrace) printf("gizflush - %s\n",vgizmap->gdbnp);
	for (i=0; i <= 255; i++) {
	    gizmhp=vgizmap->ghdrp[i];
	    for (gizmp=gizmhp; gizmp; gizmp=gizmnp) {
#if CICPP
		delete [] gizmp->ipatt;
		delete [] gizmp->opatt;
#else /* CICPP */
		FREE(gizmp->ipatt);
		FREE(gizmp->opatt);
#endif /* CICPP */
		gizmnp=gizmp->nextp;
#if CICPP
		delete gizmp;
#else /* CICPP */
		FREE(gizmp);
#endif /* CICPP */
	    }
	}
#if CICPP
	delete [] vgizmap->gdbnp;
#else /* CICPP */
	FREE(vgizmap->gdbnp);
#endif /* CICPP */

	nxtp=vgizmap->nextp;
#if CICPP
	delete vgizmap;
#else /* CICPP */
	FREE(vgizmap);
#endif /* CICPP */
    }
}
#endif /* RECGIZM */


#if RECDECO
#if CICPP
void CISISX :: xdecflush(VDECPSTRU *vdecmap)
#else /* CICPP */
void decflush(vdecmap)                                                  /*
-------------
		    seta dbxp;
		    faz close dos arquivos abertos do master file
									*/
VDECPSTRU *vdecmap;             /* decod to be flushed */
#endif /* CICPP */
{
    VDECPSTRU *nxtp;

    if (dbxtrace) printf("decflush - vdecmap=%p\n",vdecmap);
    for (; vdecmap; vdecmap=nxtp) {

	if (dbxtrace) printf("decflush - %s\n",vdecmap->ddbnp);
#if CICPP
	delete [] vdecmap->ddbnp;
#else /* CICPP */
	FREE(vdecmap->ddbnp);
#endif /* CICPP */

	nxtp=vdecmap->nextp;
#if CICPP
	delete vdecmap;
#else /* CICPP */
	FREE(vdecmap);
#endif /* CICPP */
    }
}
#endif /* RECDECO */

#if CICPP
char * CISISX :: xloadfile(char *gidbnp,
	       	       	   char  at,
	                   char *atp,
                           char *areap,
	                   LONGX asize,
	                   char lf2x)
#else /*CICPP*/
char *loadfile(gidbnp,at,atp,areap,asize,lf2x)
char *gidbnp;			/* dbn.par */
char at;
char *atp;
char *areap;
LONGX asize;
char lf2x;
#endif /*CICPP*/
{
    LONGX len,alloced=0;
    unsigned char *p;
    FILE *fp;
    int rc;
    int voidcipar=0;                                                                               /* AOT, 19/01/2006 */
    if (asize==EOF) { voidcipar=1; asize=0L; }                                                     /* AOT, 19/01/2006 */

    if (areap == NULL) {
    	if (!asize) {
    	    if (at && *atp != at) {
        		len=strlen(atp);
        		asize=len+1;
    	    }
    	    else {
        		p=(unsigned char *)atp; if (at) p++;
                if (!voidcipar) p=dbxcipar(gidbnp,(char *)p,'=');      /* AOT, 19/01/2006 */
                if ((fp=fopen(p,"r")) == NULL) {                       /* AOT, 19/01/2006 */
        		    return(NULL);
        		}
        		fseek(fp,0L,SEEK_END);
        		asize=ftell(fp)+2;
        		fclose(fp);
        		if (asize < 0) {fatal("loadfile/lseek"); return(NULL);}
        	}
    	}
#if CICPP
    	areap=new char [asize];
#else
    	areap=(char *)ALLOC((ALLOPARM)(asize));
#endif
    	if (areap == (char *)NULL) {fatal("loadfile/ALLOC"); return(NULL);}
    	alloced=1;
    }

    *areap='\0';

    if (at && *atp != at) {
    	len=strlen(atp);
    	if (len >= asize) len=asize-1;
    	memmove(areap,atp,len); areap[len]='\0';
    }
    else {
        p=(unsigned char *)atp; if (at) p++;
    	if (!voidcipar) p=dbxcipar(gidbnp,(char *)p,'=');      /* AOT, 19/01/2006 */
    	if ((fp=fopen(p,"r")) == NULL) {                       /* AOT, 19/01/2006 */
#if BEFORE970318
	    return(areap); /* CICPP */
#else
	    if (alloced)
#if CICPP
		delete [] areap;
#else
		FREE(areap);
#endif
	    return(NULL); /* CICPP */
#endif
	}
	for (--asize, p=(unsigned char *)areap; ; )
	    if (asize > 0) {
    		if ((rc=fgetc(fp)) == EOF) break;
    		if (rc)
    		    if (rc == '\n') {
        			if (lf2x) {
        			    *p++ = lf2x;
        			    asize--;
        			}
    		    }
    		    else {
#if MPE
        			if (rc) *p++ = rc; else *p++ = ' ';
#else
        			*p++ = rc;
#endif
        			asize--;
    		    }
	    }
	    else {
    		fatal("loadfile/overflow");
    		break; /* CICPP */
	    }

#ifdef USE_ERROR_SYS
    	if (p>(unsigned char *)areap && *(p-1)=='\\')	/* D.Storti 03/98 */
    		*p++ = '\n';
#endif
    	*p='\0';
    	fclose(fp);
    }
    return(areap);
}

#if CICPP
char * CISISX :: xloadstw(char *gidbnp,
                          char *atp,
	                  char *areap,
	                  LONGX  asize,
	                  int  *nstws)
#else /*CICPP*/
char *loadstw(gidbnp,atp,areap,asize,nstws)
char *gidbnp;			/* dbn.par */
char *atp;
char *areap;
LONGX asize;
int *nstws;
#endif /*CICPP*/
{
    FILE *fp;
    LONGX nbytes;
    char line[BUFSIZ],*ap,*p;
    int nlin,len;
    *nstws=0;
    if ((fp=fopen(dbxcipar(gidbnp,atp,'='),"r")) == NULL) return(NULL);
#ifdef CI_WINISIS
	 for (nlin=1; fgets(line,sizeof(line),fp); ) nlin++;
#else
	 for (nlin=0; fgets(line,sizeof(line),fp); ) nlin++;
#endif /* CI_WINISIS */
    nbytes=nlin*(LE1+1)+1;
    if (asize) { if (asize < nbytes) {fclose(fp); fatal("loadstw/overflow"); return(NULL);}}
    else asize=nbytes;
    if (areap == NULL) {
#if CICPP
	areap=new char [asize];
#else
	areap=(char *)ALLOC((ALLOPARM)(asize));
#endif
	if (areap == (char *)NULL) {fclose(fp); fatal("loadstw/ALLOC"); return(NULL);}
    }
    memset(areap,0x00,(size_t)asize);
    fseek(fp,0L,SEEK_SET);
    for (ap=areap; fgets(line,sizeof(line),fp); ) {
	len=strlen(line);
	line[--len]='\0'; /* strip fgets() '\n' */
	for (p=line+len-1; len; p--) /* strip trailing spaces */
	    if (*p == ' ') line[--len]='\0'; else break;
	if (len < 1) /*fatal("loadstw/blanked line");*/ continue;
	if (len > LE1) /*fatal(line);*/ line[len=LE1]='\0';
#ifdef CI_WINISIS
	{
	char *ai,*apstart=ap;
	for (ai=line; *ai; ai++, ap++)
		*ap=isisuctab[(unsigned int)*ai];
	*ap=0;
	ap=apstart+LE1+1;
	}
#else
	memmove(ap,line,len);
	ap+=LE1+1;
#endif /* CI_WINISIS */
	(*nstws)++;
    }
#ifdef CI_WINISIS
	if ((*nstws)/2*2!=(*nstws))		// must be pair!!
		(*nstws)++;
#endif /* CI_WINISIS */
    fclose(fp);
    return(areap);
}

//
// dbxtmpnm() original - 
// copied from 44a/cisisx.c - AOT, 01/12/2006
//
#if CICPP
char * CISISX :: xdbxtmpnm(char *dirtmp,
                           int strip,
                           char *filnamp)
#else /* CICPP */
char *dbxtmpnm(dirtmp, strip, filnamp)
char *dirtmp;
int strip;
char *filnamp;
#endif /* CICPP */
/*
                get a temporary file name in directory dirtmp (if dirtmp)
                  or in directory "CI_TEMPDIR" defined in cipar
          or env TEMP or TMP
                strip the .$$$ extension (if strip)
                store the filename in filnamp (if filnamp)
                  or allocate a new area
                return the resulting file name or NULL (if error)
*/
{
    FILE *fp;
    char *p,*q;
    int len;
#if PC
    char cwd[MAXPATH+1];
    char *cip;
#endif

    if (!ndbxs)
        dbxinit(); /* activate CIPAR */

    if (filnamp) *filnamp='\0';

#if PC
    if (dirtmp) if (strcmp(dirtmp,"CI_TEMPDIR") == 0 || strcmp(dirtmp,"ci_tempdir") == 0) {
            cip=dbxcipar(NULL,"CI_TEMPDIR",'=');
            if (dbxcipok) dirtmp=cip; else dirtmp=NULL;
            if (!dirtmp) {
           cip=dbxcipar(NULL,"ci_tempdir",'=');
                if (dbxcipok) dirtmp=cip;
       }
        if (!dirtmp) if ((dirtmp=getenv("TEMP")) == NULL) dirtmp=getenv("TMP");
    }
    if (dirtmp) {
        if (!getcwd(cwd,MAXPATH)) return(NULL);
        if (chdir(dirtmp)) return(NULL);
    }
#endif

    p=tmpnam(NULL);

    fp=fopen(p,"w"); if (fp) fclose(fp); else return(NULL);

    len=strlen(p)+4; /* .ext */

#if PC
    if (dirtmp) {
        if (chdir(cwd)) return(NULL);
        len+=strlen(dirtmp);
    }
#endif

#if PC
    if (strip) if (strcmp((q=p+strlen(p)-4),".$$$") == 0) *q='\0';
#endif

    if (!filnamp) {
#if CICPP
        try { filnamp= new char [len+1]; }
        catch (BAD_ALLOC) { filnamp=(char *)NULL; }
#else /* CICPP */
        filnamp=(char *)ALLOC((ALLOPARM)(len+1));
#endif /* CICPP */
        if (filnamp == (char *)ALLONULL) fatal("dbxtmpnm/ALLOC");
        *filnamp='\0';
    }

#if PC
    if (dirtmp)
    {
        char c;
        strcpy(filnamp,dirtmp);
        c = *(filnamp+strlen(filnamp));
      if (c != '/' && c != '\\')
      {
        if (strchr(filnamp,'/')) strcat(filnamp,"/");
        else strcat(filnamp,"\\");
      }
    }
#endif
    strcat(filnamp,p);

    if (dbxtrace && (b40trace||b50trace)) printf("+++dbxtmpnm='%s'\n",filnamp);
    return(filnamp);
}
#endif /* CICPP || CIDBX_CISISX_SOURCE */



/* ========================== Origem: CIUPD.C ========================== */

#if CICPP || CIUPD_CISISX_SOURCE

#if CICPP
int CISISX :: xrecisis0(char *dbnamp)
#else /* CICPP */
int recisis0(dbnamp)                                                  /*
------------
		    cria .mst;
		    cria .xrf;
		    retorna CREAT()
								      */
char *dbnamp;       /* ptr nome bases de dados */
#endif /* CICPP */
{
    /* FFI n; */
    int n,fd;
    char *areap;

    XRSTRU *xrp;
    M0STRU *ms0p;   /* defines MS0 */
    XRPTR xrftiv;

#if DBXMSTXL
    int pow, vtot;
#endif

#if RUITRACE
printf("recisis0 - dbnamp=%s\n",dbnamp);
#endif

#if MULTI
    if (dbxstorp(dbnamp)->dbxnetws != MONONETS)
	fatal("recisis0/MONONETS expected");
#endif

    n = (XRBSIZ > MSBSIZ) ? XRBSIZ : MSBSIZ;
#if CICPP
    try { areap = (char *) new char [n]; }
    catch (BAD_ALLOC) { areap = (char *)ALLONULL; }
#else /* CICPP */
    areap=(char *)ALLOC((ALLOPARM)n);
#endif /* CICPP */
    if (areap == (char *)ALLONULL)
	fatal("recisis0/ALLOC");

    strcpy(areap,dbnamp);
    strcat(areap,xx1extp);
    strcpy(areap,dbxcipar(dbnamp,areap,'='));
#if RUITRACE
printf("recisis0 - filename='%s'\n",areap);
#endif
#if MPE
    if ((fd=OPEN(areap,
		O_WRONLY|O_CREAT|O_TRUNC|O_MPEOPTS,PERMIS,"b R512")) <= 0) {
#else
    if ((fd=CREAT(areap,PERMIS)) <= 0) {
#endif
#if CICPP
	delete[] areap;
#else /* CICPP */
	FREE(areap);
#endif /* CICPP */
	fatal("recisis0/xrf");
    }
    if (rectrace) printf("+++ recisis0 - %s=%d\n",areap,fd);

    xrp=(XRSTRU *)areap;
    xrp->xrxrpos= -1;                   /* 1st block = flagged */

    xrftiv = (0)*XRXDIVIDE + (0);       /* inexistant records */
    for (n=0; n < XRMAXTIV; ) {
	xrp->xrmfptr[n++]=xrftiv;
    }
#if CNV_PCBINUM
    ConvertXRF_REC(areap);              /* local */
#endif

    if ((n=CIWRITE(fd,areap,XRBSIZ)) != XRBSIZ) { /* CNV_PCBINUM local */
#if RUITRACE
printf("recisis0 - write/xrf  n=%d\n",n);
#endif
	fatal("recisis0/write/xrf");
    }
    CLOSE(fd);

    n=recisis0_m; recisis0_m=1;
    if (!n) {
#if CICPP
	delete[] areap;
#else /* CICPP */
	FREE(areap);
#endif /* CICPP */
	return(fd);
    }

    strcpy(areap,dbnamp);
    strcat(areap,mx1extp);
    strcpy(areap,dbxcipar(dbnamp,areap,'='));
#if RUITRACE
printf("recisis0 - filename='%s'\n",areap);
#endif

#if MPE
    if ((fd=OPEN(areap,
		O_WRONLY|O_CREAT|O_TRUNC|O_MPEOPTS,PERMIS,"b R512")) <= 0) {
#else
    if ((fd=CREAT(areap,PERMIS)) <= 0) {
#endif
#if CICPP
	delete[] areap;
#else /* CICPP */
	FREE(areap);
#endif /* CICPP */
	fatal("recisis0/mst");
    }
    if (rectrace) printf("+++ recisis0 - %s=%d\n",areap,fd);

#if DBXMSTXL
    strcpy(areap,dbnamp);
    strcat(areap,"/mstxl");
    strcpy(areap,dbxcipar(NULL,areap,'='));
    if (!dbxcipok) {
        strcpy(areap, "mstxl");
        strcpy(areap,dbxcipar(NULL,areap,'='));
    }
    if (dbxcipok) {
        char * q = areap + strlen(areap) - 1;
        while (*q == ' ') *q = 0;

        if (areap[strlen(areap)-1] == 'G') {
            int num=atoi(areap);
            int cap = 1;
            n = 1;
            while (num != cap) {
                if (num < cap) {
                    fatal("recisis0/cipar/mstxl/G");
                }
                n++;
                cap *= 2;
            }
        } else {
            n=atoi(areap);
            if (n > 4) fatal("recisis0/mstxl");
            if (n == 4) n=3;
        }
    } else {
        n=cipmstxl;
        if (n) if (n < 0) fatal("recisis0/mstxl");
        if (n == 4) n=3;
    }
    dbxstorp(dbnamp)->dbxmstxl=n;
    if (rectrace) printf("+++ recisis0 - mstxl=%d\n",n);
#endif

    ms0p=(M0STRU *)areap;

    MS0ctlmfn=0;
    MS0nxtmfn=1;
    MS0nxtmfb=1;
    MS0nxtmfp=sizeof(M0STRU)+sizeof(M0STRU)+1;                  /* gdb */
    MS0mftype=MSMFTUSR;
#if DBXMSTXL
    /* MS0mftype += n * 0x00FF; NAO 255! */
    MS0mftype += n * 256; /* AOT/AARG 05/01/99 */

    /* first rec pos & cipmstxl alignment - 
    see ciupd.c: if (thiscomp & 0x01 || (thiscomp & vtot))*/
    vtot = 1;
    for (pow = 1; pow <= n; pow++) {
        vtot *= 2;
    }
    vtot -= 1;
    MS0nxtmfp--;
    while (MS0nxtmfp & vtot)
        MS0nxtmfp++;
    MS0nxtmfp++;
#endif
    if (recisis0_s ||
	strcmp(dbnamp+strlen(dbnamp)-3,"msg") == 0 ||
	strcmp(dbnamp+strlen(dbnamp)-3,"MSG") == 0   ) {
	MS0nxtmfb=2;
	MS0nxtmfp=1;                                            /* gdb */
	MS0mftype=MSMFTMSG;
	recisis0_s = 0;						/* 21/09/94 */
    }
    MS0reccnt=MS0mfcxx1=MS0mfcxx2=MS0mfcxx3=0;

#if MST_INFO_FLAG
    MS0comp = MST_FLAG;
#endif /* MST_INFO_FLAG */

#if MST0FILL0XFF
    memset(areap+sizeof(M0STRU),0xFF,MSBSIZ-sizeof(M0STRU));
#else
    memset(areap+sizeof(M0STRU),0x00,MSBSIZ-sizeof(M0STRU));
#endif

#if CNV_PCBINUM
    ConvertMST_CTLSTRUCT(areap); /* local */
#endif
    if ((n=CIWRITE(fd,areap,MSBSIZ)) != MSBSIZ) { /* CNV_PCBINUM local */
#if RUITRACE
printf("recisis0 - write/mst  n=%d\n",n);
#endif
	fatal("recisis0/write/mst");
    }
    CLOSE(fd);

#if CICPP
    delete[] areap;
#else /* CICPP */
    FREE(areap);
#endif /* CICPP */

    return(fd);
}


/* ----------------------------- recreset ------------------------------ */
#if IFUPDATE
#if CICPP
void CISISX :: xrecreset(char *dbnp,
			 LONGX  mfn1,
			 LONGX  mfn2,
			 RECSTRU *xrecp,
			 LONGX  parmtell)
#else /* CICPP */
void recreset(dbnp,mfn1,mfn2,xrec,parmtell)
char *dbnp;
LONGX mfn1;
LONGX mfn2;
LONGX xrec;
LONGX parmtell;
#endif /* CICPP */
{
    RECSTRU *recp;
    DBXSTRU *dbxp;
#if !CICPP
    LONGX irec;
#endif /* CICPP */
    LONGX mfn;
    LONGX xrftiv;
    int flagnew,flagmod;
    int w,thisidx;
    LONGX thispos,count=0L,count2=0L;
    off_t xbyte;
#ifdef USE_INFO_SYS
	 char mess_str[64];
//	 char resettingmess[64];
	UIW_STRING *procstr=NULL;
	UIW_PROGRESSBAR *progbar=NULL;
	if (wprogress) {
		UI_EVENT event;
		procstr=((UIW_STRING*)wprogress->Get("PROCESSING_STRING"));
//		((UIW_BUTTON*)wprogress->Get("101"))->woFlags|=WOF_NON_SELECTABLE;
//		((UIW_BUTTON*)wprogress->Get("101"))->Information(I_CHANGED_FLAGS,NULL);
		wprogress->eventManager->Get(event,Q_NO_BLOCK|Q_BEGIN|Q_DESTROY|Q_POLL);
		wprogress->windowManager->Event(event);
		progbar=((UIW_PROGRESSBAR*)wprogress->Get("PROG_BAR"));
		progbar->Assign(0,0);
		progbar->SetColor(11);
//		sprintf(resettingmess,"%s",mess->get(453));
		}
#endif

    if (rectrace) printf("recreset - %s / %"_LD_" / %"_LD_" \n",dbnp,mfn1,mfn2);

    /* allocate irec */
    if (mfn1) {
#if CICPP
	try { recp = new RECSTRU(this); }
	catch (BAD_ALLOC) { fatal("recreset/ALLOC/recp"); }
	recp->xrecord(dbnp,mfn=mfn1); /* RECdbxp and mstsetup() */
#else /* CICPP */
	if (!nrecs) recinit();
	for (irec=maxnrec; --irec > 0; )
	    if (!vrecp[irec]) {
		recallok(irec,MAXMFRL);
		recp=vrecp[irec];
		break;
	    }
	if (irec == 0) fatal("recreset/maxnrec");
	record(irec,dbnp,mfn=mfn1); /* RECdbxp and mstsetup() */
#endif /* CICPP */
    }
    else {
#if CICPP
	recp=xrecp; // "delete recp" only if (mfn1) is true
#else /* CICPP */
	irec=xrec;
	recp=vrecp[irec];
#endif /* CICPP */
	mfn=mfn2=MFRmfn;
    }
    if (rectrace) printf("recreset : %s / %"_LD_" / %"_LD_" \n",dbnp,mfn1,mfn2);

    dbxp=RECdbxp;
    dbxopenw(DBXname,DBXname,xx1extp,&DBXxropn,&DBXxropw,"recreset/xropn/w");
    dbxopenw(DBXname,DBXname,mx1extp,&DBXmsopn,&DBXmsopw,"recreset/msopn/w");

#if MULTI
    if (DBXnetws != MONONETS) if (!DBXewlxx) fatal("recreset/EWL is off");
#endif

    if (mfn1) DBXirecs=0;

    for (; mfn <= mfn2; mfn++) {

	if (mfn1) {
	    if (rectrace) printf("recreset - reading %s / %"_LD_" \n",RDBname,mfn);
	    recreadl=1;			/* leader */

	    recread(recp,mfn);		/* leader */
	}
	flagnew=recxrefn; flagmod=recxrefm;

	if (rectrace) printf("recreset - bw=%"_LD_"/%d f=%d/%d\n",MFRmfbwb,MFRmfbwp,flagnew,flagmod);

	if (RECrc == RCEOF) break;

	if (parmtell)
	    if (++count % parmtell == 0)
#ifdef USE_INFO_SYS
			{
	//		errsys->sys_info_data(ltoa(count,mfnstr,10));
			if (wprogress) {
				UI_EVENT event;	// Resetting MFN
//				sprintf(mess_str,"%s %"_LD_,resettingmess,mfn);
				sprintf(mess_str,"MFN: %"_LD_,mfn);
				procstr->DataSet(mess_str);
				progbar->Assign(mfn-mfn1,mfn2-mfn1);
				wprogress->eventManager->Get(event,Q_NO_BLOCK|Q_BEGIN|Q_DESTROY|Q_POLL);
				if (event.type==10235) {		// EXPORT_CANCEL) {
					int ok=errsys->ErrorMessage(wprogress->windowManager,WOS_INVALID,mess->get(420));
					if (ok==WOS_INVALID) {
						mfn=mfn2+2;    // STOP
						continue;
						}
					}
				wprogress->windowManager->Event(event);
				wprogress->eventManager->Get(event,Q_NO_BLOCK|Q_BEGIN|Q_DESTROY|Q_POLL);
				if (event.type==10235) {		// EXPORT_CANCEL) {
					int ok=errsys->ErrorMessage(wprogress->windowManager,WOS_INVALID,mess->get(420));
					if (ok==WOS_INVALID) {
						mfn=mfn2+2;    // STOP
						continue;
						}
					}
				wprogress->windowManager->Event(event);
				}
			}
#else
		fprintf(stderr,"+++ %"_LD_"/%"_LD_" \n",mfn,count2);
#endif

	if (RECrc == RCPDEL) continue;

	w=0;
	/* reset .mst bw ptrs */
	if (MFRmfbwb || MFRmfbwp) {
	    MFRmfbwb=0;
	    MFRmfbwp=0;
#if MULTI
            /* keep existing RLOCK - AOT/HB 09/11/98*/
            if (RECgdbw) MFRmfrl=(FFI)0-MFRmfrl;
#endif
	    if (rectrace) printf("recreset - writing mst %s \n",RDBname);

	    recwmast(NULL,recp,recxrefb,recxrefp,0,1);	/* leader */

	    w++;
	}
	/* reset .xrf flags */
	if (flagnew || flagmod) {
#if DBXMSTXL /* AOT 20/09/2001 */
	    if (rectrace) printf("recreset - writing xrf %s  DBXmstxl=%d XRXDIVIDE=%d recxrefb=%"_LD_" recxrefp=%d\n",RDBname,DBXmstxl,XRXDIVIDE,recxrefb,recxrefp);
        xrftiv = recxrefb * (XRXDIVIDE>>DBXmstxl) + (recxrefp>>DBXmstxl);
#else /* DBXMSTXL */
	    xrftiv = recxrefb * XRXDIVIDE + recxrefp;
#endif /* DBXMSTXL */
	    if (MFRstatus == DELETED) xrftiv= -xrftiv;
	    thispos=(MFRmfn+XRMAXTV1)/XRMAXTIV;
	    thisidx=(MFRmfn-1)%XRMAXTIV;
	    xbyte=(off_t)(((off_t)(thispos-1)<<XRSHIFT)+XRPOSSIZ+thisidx*XRPTRSIZ );
	    if (LSEEK64(DBXxropn,xbyte,SEEK_SET) != xbyte)
		fatal("recreset/recwxref/seek");
#if CNV_PCBINUM
	    memmove(cnv_pcbuff,&xrftiv,XRPTRSIZ);
	    ConvertXRF_PTR(cnv_pcbuff);
	    if (CIWRITE(DBXxropn,cnv_pcbuff,XRPTRSIZ) != XRPTRSIZ)
#else
	    if (CIWRITE(DBXxropn,(char *)&xrftiv,XRPTRSIZ) != XRPTRSIZ)
#endif
		fatal("recreset/recwxref/write");
	    if (labs(DBXxribp->xrxrpos) == thispos)     /* update buffer */
		DBXxribp->xrmfptr[thisidx]=xrftiv;
	    w++;
	}
	if (w) {count2++; if (mfn1) DBXirecs++; }
    }
    if (parmtell)
	if (count % parmtell)
#ifdef USE_INFO_SYS
//	     errsys->sys_info_data(ltoa(count,mfnstr,10));
			{
			if (wprogress) {
				UI_EVENT event;
				sprintf(mess_str,"MFN %"_LD_,mfn);
				((UIW_STRING*)wprogress->Get("PROCESSING_STRING"))->DataSet(mess_str);
				wprogress->eventManager->Get(event,Q_NO_BLOCK|Q_BEGIN|Q_DESTROY|Q_POLL);
				wprogress->windowManager->Event(event);
				wprogress->eventManager->Get(event,Q_NO_BLOCK|Q_BEGIN|Q_DESTROY|Q_POLL);
				wprogress->windowManager->Event(event);
				}
			}
#else
	    fprintf(stderr,"+++ %"_LD_"/%"_LD_" \n",mfn,count2);
#endif

    if (mfn1) {
#if CICPP
	delete recp;
#else /* CICPP */
	if (rectrace) printf("+++ recreset - free %"_LD_" \n",irec);
	FREE(vrecp[irec]); vrecp[irec]=NULL; nrecs--;
#endif /* CICPP */
    }


    /* ??? close_ do .mst=dbxulock
                  do .xrf=never!!!
    */

}

#endif /* IFUPDATE */

#endif /* CICPP || CIUPD_CISISX_SOURCE */



/* ========================== Origem: CITRM.C ========================== */

#if CICPP || CITRM_CISISX_SOURCE

#if CICPP
int CISISX :: xcntread(DBXSTRU *dbxp,
                        int *ifpopen,
                        char *ifpname,
                        int *firstcnli)
#else /* CICPP */
int cntread(dbxp,ifpopen,ifpname,firstcnli)
DBXSTRU *dbxp;
int *ifpopen;
char *ifpname;
int *firstcnli;
#endif /* CICPP */
{
    INVMAP *invp;
    int fd,treecase;
    unsigned char *p;
#if TRCTRACE
    CNSTRU *cp;
    int n;
#endif

    if (!DBXifmap) dbxinvmp(dbxp);
    invp=DBXifmap;

    /* open .cnt */
    if (!invp->cnopn || (trmifupd && !invp->cnopw)
#if MULTI
        || DBXnetws != MONONETS
#endif
	) {

	if (strcmp(DBXname,"null") == 0) {
/*
idtype=1 ordn=5 ordf=5 n=15 k=5 liv=-1 posrx=0 nmaxpos=0 fmaxpos=0 abnormal=0
idtype=2 ordn=5 ordf=5 n=15 k=5 liv=-1 posrx=0 nmaxpos=0 fmaxpos=0 abnormal=0
*/
	    for (treecase=0; treecase < 2; treecase++) {
		invp->cn[treecase].idtype=treecase+1;
		invp->cn[treecase].ordn=ORDN;
		invp->cn[treecase].ordf=ORDF;
		invp->cn[treecase].n=CNTN;
		invp->cn[treecase].k=CNTK;
		invp->cn[treecase].liv=NEGLIV;
		invp->cn[treecase].posrx=0;
		invp->cn[treecase].nmaxpos=0;
		invp->cn[treecase].fmaxpos=0;
		invp->cn[treecase].abnormal=ABNORMAL;
	    }
	}
#if TRSTRACE
if (trmtrace && dbxtrace) {
printf("cntread - invp=%p DBXname=%s ",DBXifmap,DBXname);
printf(" opn=%d opw=%d\n",invp->cnopn,invp->cnopw);
}
#endif
	if (strcmp(DBXname,"null") == 0) return(0);

	p=(UCHR *)invp->cn;
	invp->cc_offset=0;                                      /* v4.2 */
#if CNLI
	dbxopt_fatal=0; /* set value for next dbxopen() call */
	if ((fd=dbxopen(DBXname,DBXname,iy0extp)) > 0) {
	    if (CIREAD(fd,p,CNBSIZ) != CNBSIZ) fatal("cntread/read/iy0");
	    invp->cc_offset+=(6*CNBSIZ);
	    if (trmtrace)
		printf("cntread - cc_offset   =%"_LD_"\n",
		    invp->cc_offset);
            invp->cc_offset+=atol((CONST char *)p);
	    if (invp->cc_offset != (6+2)*CNBSIZ) fatal("cntread/cc_offset");
	    for (treecase=0; treecase < 2; treecase++) {
		if (CIREAD(fd,p,CNBSIZ) != CNBSIZ) fatal("cntread/read/iy0");
		invp->cn_offset[treecase]=invp->cc_offset;
		if (trmtrace)
		    printf("cntread - cn_offset[%d]=%"_LD_"\n",treecase,
			invp->cn_offset[treecase]);
		if (treecase) invp->n2opn=fd; else invp->n1opn=fd;
		if (dbxordwr) if (treecase) invp->n2opw++; else invp->n1opw++;
                invp->cc_offset+=atol((CONST char *)p);
	    }
	    for (treecase=0; treecase < 2; treecase++) {
		if (CIREAD(fd,p,CNBSIZ) != CNBSIZ) fatal("cntread/read/iy0");
		invp->cl_offset[treecase]=invp->cc_offset;
		if (trmtrace)
		    printf("cntread - cl_offset[%d]=%"_LD_"\n",treecase,
			invp->cl_offset[treecase]);
		if (treecase) invp->l2opn=fd; else invp->l1opn=fd;
		if (dbxordwr) if (treecase) invp->l2opw++; else invp->l1opw++;
                invp->cc_offset+=atol((CONST char *)p);
	    }
	    if (CIREAD(fd,p,CNBSIZ) != CNBSIZ) fatal("cntread/read/iy0");
	    invp->ci_offset=0;
            if (atol((CONST char *)p))
		invp->ci_offset=invp->cc_offset;
	    if (trmtrace)
		printf("cntread - ci_offset   =%"_LD_"\n",invp->ci_offset);
	    if (invp->ci_offset) {
#if SAMEL
		invp->ifopn[0]=fd;
#else
		invp->ifopn=fd;
		if (dbxordwr) invp->ifopw++;
#endif
		if (ifpopen) *ifpopen=1;
	    }
	    else {
		if (ifpname) strcpy(ifpname,DBXname);
		if (strlen(DBXname) < 2) fatal("cntread/ifpname");
		if (ifpname) ifpname[strlen(DBXname)-1]='\0'; /* strip last char */
	    }
	    invp->cc_offset=6*CNBSIZ;
	    if (firstcnli) *firstcnli=1;
	}
	else
	    fd=dbxopen(DBXname,DBXname,cx1extp);
#else /* CNLI */
	if (ifpopen)	{;} /* no wrn */
	if (ifpname)	{;} /* no wrn */
	if (firstcnli)	{;} /* no wrn */
#if MULTI
	if (trmifupd || DBXnetws != MONONETS)
            fd=dbxopenw(DBXname,DBXname,cx1extp,&DBIcnopn,&DBIcnopw,"cnread/cnopn/w");
	else {
	    fd=invp->cnopn=dbxopen(DBXname,DBXname,cx1extp);
	    if (dbxordwr) invp->cnopw++;
	}
#else /* MULTI */
	fd=dbxopen(DBXname,DBXname,cx1extp);
	if (dbxordwr) invp->cnopw++;
#endif /* MULTI */
#endif /* CNLI */
	if (trmtrace) printf("cntread - %s opn=%d opw=%d\n",DBXname,invp->cnopn,invp->cnopw);
    }

    /* read .cnt into DBIcn[] */
#if BEFORE950820
    if (LSEEK64(fd,0L,SEEK_SET) != 0) fatal("cntread/seek"); /* see CNLI */
#endif
#if CNV_PCFILES
    if (CIREAD(fd,p,CNBSIZ-CNBUNI) != CNBSIZ-CNBUNI)
#else
    if (CIREAD(fd,p,CNBSIZ) != CNBSIZ)
#endif
        fatal("cntread/read/cnt1");
#if CNV_PCBINUM
    ConvertCNT_REC(p);
#endif
#if TRCTRACE
printf("+++cnt1:\n");
    cp=(CNSTRU *)p;
                printf("idtype=%d ordn=%d ordf=%d n=%d k=%d ",
                        cp->idtype,cp->ordn,cp->ordf,cp->n,cp->k);
                printf("liv=%d posrx=%"_LD_"\n",
                        cp->liv,cp->posrx);
                printf("nmaxpos=%"_LD_" fmaxpos=%"_LD_" abnormal=%d\n",
                        cp->nmaxpos,cp->fmaxpos,cp->abnormal);
 for (n=0; n<CNBSIZ; n++) printf("%02x ",p[n]); printf("\n");
#endif
#if SAMEL
    if (invp->cn[0].idtype > 1000) {
        invp->iflxn=invp->cn[0].idtype-1000;
        if (invp->iflxn > MAXSAMEL) fatal(dbp);
        invp->cn[0].idtype = 1;
    }
    if (lxx > invp->iflxn) fatal(dbp);
    DBXiflxx=lxx;
    if (trmtrace) printf("cntread - iflxx=%d iflxn=%d \n",
    					DBXiflxx,invp->iflxn);
#endif
#if ZTREE
    invp->iflzx=0;
    if (invp->cn[0].ordf > ORDF) {
        invp->iflzx=invp->cn[0].ordf-ORDF;
        invp->cn[0].ordf=ORDF;
        if (trmtrace) printf("cntread - iflzx=%d \n",invp->iflzx);
    }
#endif
    if (invp->cn[0].idtype != 1) fatal("cntread/idtype/1");
    if (invp->cn[0].ordf != ORDF) fatal("cntread/ordf/1");
    p+=CNBSIZ;
#if CNV_PCFILES
    if (CIREAD(fd,p,CNBSIZ-CNBUNI) != CNBSIZ-CNBUNI)
#else
    if (CIREAD(fd,p,CNBSIZ) != CNBSIZ)
#endif
        fatal("cntread/read/cnt2");
#if CNV_PCBINUM
    ConvertCNT_REC(p);
#endif
#if TRCTRACE
printf("+++cnt2:\n");
    cp=(CNSTRU *)p;
                printf("idtype=%d ordn=%d ordf=%d n=%d k=%d ",
                        cp->idtype,cp->ordn,cp->ordf,cp->n,cp->k);
                printf("liv=%d posrx=%"_LD_"\n",
                        cp->liv,cp->posrx);
                printf("nmaxpos=%"_LD_" fmaxpos=%"_LD_" abnormal=%d\n",
                        cp->nmaxpos,cp->fmaxpos,cp->abnormal);
 for (n=0; n<CNBSIZ; n++) printf("%02x ",p[n]); printf("\n");
#endif
    if (invp->cn[1].idtype != 2) fatal("cntread/idtype/2");
    if (invp->cn[1].ordf != ORDF) fatal("cntread/ordf/2");

    if (!invp->cc_offset)                                   /* v4.2 */
        if (!invp->cnopw) {
	    CLOSE(fd); fd=invp->cnopn=0;
	}

    return(fd);
}

#if CICPP
void CISISX :: xinvsetup(UCHR *dbnamp,
			  LONGX  loadn0x,
			  LONGX  loadl0x,
			  LONGX  loadpst)
#else /* CICPP */
void invsetup(dbnamp,loadn0x,loadl0x,loadpst)                         /*
-------------
		    seta dbxp;
		    aloca celula INVMAP para o database;
		    abre, carrega registros e fecha .cnt;
		    abre .n01 e .n02;
		    inicializa estrutura de buffers nx;
		    se loadn0x carrega e fecha .n0x
		    abre .l01 e .l02;
		    se loadl0x inicializa estrutura lx e carrega e fecha .l0x;
		    abre .ifp;
		    se loadpst inicializa estrutura px e carrega e fecha .ifp;
								      */
UCHR *dbnamp;       /* ptr nome base de dados a ler */               /* v3.3
*/
LONGX loadn0x;       /* carga total de .n0x */                        /* v3.3
*/
LONGX loadl0x;       /* carga total de .n0x */                        /* v3.3
*/
LONGX loadpst;       /* carga total de .ifp */                        /* v3.3
*/
#endif /* CICPP */
{
#if BEFORE20010221 /* now in cisis.h - AOT, 21/02/2001 */
#if !CICPP
    extern int NXPAGES[];
#endif /* CICPP */
#endif /* BEFORE20010221 */
    DBXSTRU *dbxp;
    INVMAP *invp;
    NXSTRU *nxp;
#if !LIND
    IFPCTRL ifpctrl;                                            /* v3.3 */
#endif
    int fd;
    UCHR *p;
    int treecase,liv,level,pagesize;
    off_t npages,nbytes,loop;
    int ifpopen;                                                /* v4.2 */
    int firstcnli;                                              /* v4.2 */
    char ifpname[sizeof(DBXname)];                              /* v4.2 */
#if SAMEL
    char dbxname[sizeof(DBXname)],*dbp;
    int lxy;
    int lxx = 0;
#endif

    if (trm_invload) loadn0x=loadl0x=loadpst=trm_invload;	/* 28/03/97 */

#if SAMEL
    if (loadpst) fatal("invsetup/loadpst");
    strcpy(dbxname,dbp=dbnamp);
    for (p=dbnamp=dbxname; *p; p++)
	if (*p == '#') {
	    *p++ = '\0';
	    for (; isdigit(*p); p++) lxx=10*lxx+(int)*p-(int)'0';
	    if (*p) fatal(dbp);
	    if (lxx < 1 || lxx > MAXSAMEL) fatal(dbp);
	    break;
	}
    if (trmtrace) printf("invsetup - %s,%s,lxx=%d\n",dbp,dbnamp,lxx);
#endif
    dbxp=dbxstorp((char *)dbnamp);
    if (trmtrace) printf("invsetup - %s ifmap=%p\n",dbnamp,DBXifmap);
    firstcnli=0;                                                /* v4.2 */
    ifpopen=0;
    ifpname[0]='\0';
    invp=DBXifmap;
    if (!DBXifmap || trmifupd) {
        if (!DBXifmap) invp=dbxinvmp(dbxp);
	if (!invp->cn[0].idtype) fd=cntread(dbxp,&ifpopen,ifpname,&firstcnli);
    }

    if (strcmp((CONST char *)dbnamp,"null") == 0)
	return;

    /* node's */
    if (firstcnli ||                                            /* v4.2 */
	!invp->n1opn && invp->nybasep[0] == NULL &&
	!invp->n2opn && invp->nybasep[1] == NULL) {
	if (!firstcnli) {
	    fd=invp->n1opn=dbxopen(DBXname,DBXname,nx12extp[0]);
	    if (dbxordwr) invp->n1opw++;
	}
	pagesize=N1BSIZ;
	for (treecase=0; treecase<2; treecase++) {
	    liv=invp->cn[treecase].liv;
	    if (liv < 0)
		if (liv != NEGLIV)                              /* v3.1 */
		    fatal("invsetup/liv/NEGLIV");
#if TRETRACE
		else
		    printf("invsetup - liv=%d for treecase %d\n",liv,treecase);
#endif
	    if (liv >= MAXLIV)
		fatal("invsetup/liv/MAXLIV");
	    invp->nybasep[treecase]=NULL;
	    if ((liv == NEGLIV || loadn0x) && !trmifupd) {	/* 08/03/95 */
		npages=invp->cn[treecase].nmaxpos;
		if (npages) {
		    nbytes=pagesize*npages;
#if !CICPP
		    if (loadn0x > ALLOMAXV)
			fatal("invsetup/ALLOMAXV/.n0x");
#endif /* CICPP */
		    if (nbytes > loadn0x)
			goto OPENN02;
#if CICPP
            try { invp->nybasep[treecase]= new char [nbytes]; }
		    catch (BAD_ALLOC) { invp->nybasep[treecase] = (char *)ALLONULL; }
#else /* CICPP */
		    invp->nybasep[treecase]=(char *)ALLOC((ALLOPARM)(nbytes));
#endif /* CICPP */
            if (invp->nybasep[treecase] == (char *)ALLONULL)
			fatal("invsetup/ALLOC/nypages");
#if TRSXRACE
if (trmtrace) {
printf("invsetup - nybasep[%d]=%p %6"_LD_"/%6"_LD_"=%6"_LD_" bytes/pages  fd=%d\n",
 treecase,invp->nybasep[treecase],nbytes,npages,nbytes/npages,fd);
}
#endif
		}
		else {
		    ;
#if TRSXRACE
if (trmtrace) {
printf("invsetup - nybasep[%d]=%p %"_LD_" pages\n",
 treecase,invp->nybasep[treecase],npages);
}
#endif
		}
#if CNLI
		/* .... 951208 */
		LSEEK64(fd,invp->cn_offset[treecase],SEEK_SET);
#endif
		for (p=(unsigned char *)invp->nybasep[treecase], loop=npages; loop--;
								p+=pagesize)
		    if (CIREAD(fd,p,pagesize) != pagesize)
			fatal("invsetup/read/nypages");
		if (!invp->cc_offset) {                         /* v4.2 */
		    CLOSE(fd);
		    if (treecase) invp->n2opn=0; else invp->n1opn=0;
		    if (treecase) invp->n2opw=0; else invp->n1opw=0;
		}
	    }
OPENN02:
	    if ((invp->nybasep[treecase] == NULL) && !trmifupd) {	/* 08/03/95 */
		for (level=0; level<=liv; level++) {
		    nxp= &invp->nx[treecase][level];
#if CICPP
		    nxp->pages = 1;
#else /* CICPP */
		    nxp->pages = NXPAGES[level];
#endif /* CICPP */
		    nxp->left=nxp->pages;
		    nbytes=pagesize*nxp->pages;
#if CICPP
            try { nxp->basep=(char *) new char [nbytes]; }
            catch (BAD_ALLOC) { nxp->basep = (char *)ALLONULL; }
#else /* CICPP */
		    if ((nbytes) > ALLOMAXV)
			fatal("invsetup/ALLOMAXV/nbytes)");
		    nxp->basep=(char *)ALLOC((ALLOPARM)(nbytes));
#endif /* CICPP */
            if (nxp->basep == (char *)ALLONULL)
			fatal("invsetup/ALLOC/nxpages");
#if TRSTRACE
if (trmtrace && dbxtrace) {
printf("invsetup - nxp[%d][%d]=%p com %"_LD_" bytes\n",treecase,level,nxp,nbytes);
}
#endif
		}
	    }
	    if (treecase == 0)
		if (!firstcnli) {
		    fd=invp->n2opn=dbxopen(DBXname,DBXname,nx12extp[1]);
		    if (dbxordwr) invp->n2opw++;
		}
	    pagesize=N2BSIZ;
	}
    }
    /* leaf's */
    if (firstcnli ||                                            /* v4.2 */
	!invp->l1opn && invp->lybasep[0] == NULL &&
	!invp->l2opn && invp->lybasep[1] == NULL) {
	if (!firstcnli) {
	    fd=invp->l1opn=dbxopen(DBXname,DBXname,lx12extp[0]);
	    if (dbxordwr) invp->l1opw++;
	}
	pagesize=L1BSIZ;
	for (treecase=0; treecase<2; treecase++) {
	    invp->lybasep[treecase]=NULL;
	    if ((liv == NEGLIV || loadl0x) && !trmifupd) {	/* 08/03/95 */
		npages=invp->cn[treecase].fmaxpos;
		if (npages) {
		    nbytes=pagesize*npages;
#if !CICPP
		    if (loadl0x > ALLOMAXV)
			fatal("invsetup/ALLOMAXV/.l0x");
#endif /* CICPP */
		    if (nbytes > loadl0x)
			goto OPENL02;
#if CICPP
            try { invp->lybasep[treecase]= (char *) new char [nbytes]; }
            catch (BAD_ALLOC) {invp->lybasep[treecase] = (char *)ALLONULL; }
#else /* CICPP */
		    invp->lybasep[treecase]=(char *)ALLOC((ALLOPARM)(nbytes));
#endif /* CICPP */
            if (invp->lybasep[treecase] == (char *)ALLONULL)
			fatal("invsetup/ALLOC/lypages");
#if TRSXRACE
if (trmtrace) {
printf("invsetup - lybasep[%d]=%p %6"_LD_"/%6"_LD_"=%6"_LD_" bytes/pages  fd=%d\n",
 treecase,invp->lybasep[treecase],nbytes,npages,nbytes/npages,fd);
}
#endif
		}
		else {
		    ;
#if TRSXRACE
if (trmtrace) {
printf("invsetup - lybasep[%d]=%p %"_LD_" pages\n",
 treecase,invp->lybasep[treecase],npages);
}
#endif
		}
#if CNLI
		/* .... 951208 */
		LSEEK64(fd,invp->cl_offset[treecase],SEEK_SET);
#endif
		for (p=(unsigned char *)invp->lybasep[treecase], loop=npages; loop--;
							p+=pagesize) {
		    if (CIREAD(fd,p,pagesize) != pagesize)
			fatal("invsetup/read/lypages");
		    if (loop+((L0STRU *)p)->pos != npages){
			printf("*** loop=%"P_OFF_T"  pos=%"_LD_,
						(LONG_LONG)loop,((L0STRU *)p)->pos);
			fatal("invsetup/check/lypages");
		    }
		}
		if (!invp->cc_offset) {                         /* v4.2 */
		    CLOSE(fd);
		    if (treecase) invp->l2opn=0; else invp->l1opn=0;
		    if (treecase) invp->l2opw=0; else invp->l1opw=0;
		}
	    }
OPENL02:
	    if (treecase == 0)
		if (!firstcnli) {
		    fd=invp->l2opn=dbxopen(DBXname,DBXname,lx12extp[1]);
		    if (dbxordwr) invp->l2opw++;
		}
	    pagesize=L2BSIZ;
	}
    }
    /* post's */
#if IFPDUMMY
if (loadpst >= 0L) {
#endif
    if (ifpopen ||                                              /* v4.2 */
#if SAMEL
	!invp->ifopn[0] &&
#else
	!invp->ifopn &&
#endif
	invp->iybasep == NULL) {
	if (!*ifpname) strcpy(ifpname,DBXname);
	if (!ifpopen) {
#if SAMEL
	    if (lxx) sprintf(ifpname,"%s#%d",DBXname,lxx);
	    fd=invp->ifopn[0]=dbxopen(DBXname,ifpname,ix1extp);
	    for (lxy=2; lxy <= invp->iflxn; lxy++) {
		sprintf(ifpname,"%s#%d",DBXname,lxy);
		fd=invp->ifopn[lxy-1]=dbxopen(DBXname,ifpname,ix1extp);
#if TRSXRACE
if (trmtrace) printf("invsetup - ifpname=%s fd=%d\n",ifpname,fd);
#endif
	    }
#else
            dbxopt_fatal=0;
	    fd=invp->ifopn   =dbxopen(DBXname,ifpname,ix1extp);
	    if (fd <= 0) {
	      fd=invp->ifopn =dbxopen(DBXname,DBXname,ix1extp);
	    }
	    if (dbxordwr) invp->ifopw++;
#endif
	} /* end if (!ifpopen) */

	pagesize=IFBSIZ;
	invp->iybasep=NULL;
	if ((invp->cn[0].liv == NEGLIV &&
	     invp->cn[1].liv == NEGLIV || loadpst) && !trmifupd) { /* 08/03/95 */
#if SAMEL
	    if (lxx) {
		printf("*** dbname=%s\n",dbp);
		fatal("invsetup/SAMEL/loadpst");
	    }
#endif
#if CNLI
	    /* .... 951208 */
	    loop=LSEEK64(fd,invp->ci_offset,SEEK_SET);
#endif
#if !LIND
	    if (CIREAD(fd,(char *)&ifpctrl,sizeof(IFPCTRL)) != sizeof(IFPCTRL)) {
		fatal("invsetup/read/ifpctrl");
	    }
#if CNV_PCBINUM
	    ConvertIFP_BLKCTL((char *)&ifpctrl,1);
#endif
	    LSEEK64(fd,-sizeof(IFPCTRL),SEEK_CUR);
	    npages=ifpctrl.ifprec1;
	    if (ifpctrl.ifprec2 == 0)
		npages--;
	    nbytes=pagesize*npages;
#else /* LIND */
#if BEFORE951208
	    nbytes=LSEEK64(fd,0L,SEEK_CUR);
#else
	    nbytes=LSEEK64(fd,0L,SEEK_END);
#if CNLI
	    nbytes-=loop;
#endif
	    LSEEK64(fd,loop,SEEK_SET);
#endif
	    npages=nbytes/pagesize;
#endif
#if !CICPP
	    if (loadpst > ALLOMAXV)
		fatal("invsetup/ALLOMAXV/.ifp");
#endif
	    if (nbytes > loadpst || npages == 0) /* 04/10/91 */
		;
	    else {
#if CICPP
        try { invp->iybasep=(char *) new char [nbytes]; }
        catch (BAD_ALLOC) {invp->iybasep = (char *)ALLONULL; }
#else /* CICPP */
		invp->iybasep=(char *)ALLOC((ALLOPARM)(nbytes));
#endif /* CICPP */
#if TRSXRACE
if (trmtrace) {
printf("invsetup - iybasep   =%p %6"_LD_"/%6"_LD_"=%6"_LD_" bytes/pages  fd=%d\n",
 invp->iybasep,nbytes,npages,nbytes/npages,fd);
}
#endif
        if (invp->iybasep == (char *)ALLONULL)
		    fatal("invsetup/ALLOC/iypages");
		for (p=(unsigned char *)invp->iybasep, loop=npages; loop--; p+=pagesize)
		    if (CIREAD(fd,p,pagesize) != pagesize)
			fatal("invsetup/read/iypages");
		if (!invp->cc_offset) {                         /* v4.2 */
		    CLOSE(fd);
#if SAMEL
		    invp->ifopn[0]=0;
#else
		    invp->ifopn=0;
		    invp->ifopw=0;
#endif
		}
	    }
	} /* end if (invp->cn[] == NEGLIV || loadpst) */
    } /* end if (ifpopen || ! invp->ifopn) */
#if IFPDUMMY
} /* end if (loadpst >= 0L) */
#endif
}
#if CICPP
N0STRU * CISISX :: xnoderead(INVMAP *invp,
			      int     treecase,
			      int     level,
			      PUNT    punt)
#else /* CICPP */
N0STRU *noderead(invp,treecase,level,punt)                            /*
----------------
		    aponta para a area de Nos em memoria;
		    pesquisa o No' desejado;
		    se achou, retorna pointer;
		    determina endereco para leitura;
		    posiciona registro desejado;
		    le registro;
		    retorna pointer N0STRU ou abenda;
								      */
INVMAP *invp;       /* ptr descritor do inverted file */
int treecase;       /* indice da B*Tree a pesquisar */
int level;          /* nivel da arvore a pesquisar */
PUNT punt;          /* registro desejado */
#endif /* CICPP */
{
    N0STRU *np;
    N1STRU *n1p;                                                /* v3.3 */
    N2STRU *n2p;                                                /* v3.3 */
    NXSTRU *nxp;
    int total,left,used;
    UCHR *basep;
    int n,nodesize,fd;
    UCHR *p;
    off_t xbytes;
#if IFUPDATE
    static UCHR nodearea[N2BSIZ];       /* area para ler pagina de.n01/.n02 */
#endif
#if CNV_PCFILES
    N0STRU *unp;
    char *up;
#endif
#if LINDLUX
    if (invp->ltxpages[treecase]) {
	if (punt < 1) fatal("noderead/punt");
	if (punt > invp->ltxpages[treecase]) fatal("noderead/punt");
	if ((np=invp->ltxvpagp[treecase][punt]) == NULL) fatal("noderead/ltx");
	return(np);
    }
#endif /* LINDLUX */
    if (treecase) {
	nodesize=N2BSIZ;
	fd=invp->n2opn;
    }
    else {
	nodesize=N1BSIZ;
	fd=invp->n1opn;
    }
#if TRNTRACE
printf("noderead - invp=%p  treecase=%d  level=%d  punt=%"_LD_"\n",
    invp,treecase,level,(LONGX)punt);
#if CNLI
printf("noderead - fd=%d  offset=%"_LD_"\n",fd,invp->cn_offset[treecase]);
#endif
#endif
    nxp= &invp->nx[treecase][level];
    p=(unsigned char *)invp->nybasep[treecase];                                  /* v3.3 */
    if (fd == 0 && p == NULL)
	fatal("noderead/fd&p");
    if (p != NULL) {
#if IFUPDATE
	if (trmifupd) fatal("noderead/IFUPDATE/p");
#endif
	if (treecase) {
	    n2p=(N2STRU *)p; n2p+=(punt-1L);
#if TRNTRACE
printf("noderead - p=%p  n2p=%p\n",p,n2p);
#endif
	    p=(UCHR *)n2p;
	}
	else {
	    n1p=(N1STRU *)p; n1p+=(punt-1L);
#if TRNTRACE
printf("noderead - p=%p  n1p=%p\n",p,n1p);
#endif
	    p=(UCHR *)n1p;
	}
    }
else {                                                          /* v3.3 */
    total=nxp->pages;
    left= nxp->left; used=total-left;
    basep=(unsigned char *)nxp->basep;
#if TRNTRACE
printf("noderead - nodesize=%d  fd=%d  total=%d  left=%d  basep=%p\n",
    nodesize,fd,total,left,basep);
#endif
#if IFUPDATE
    if (trmifupd)                                               /* 15/04/93 */
	p=nodearea;
    else
	for (n=used, p=basep; n--; p+=nodesize) {
	    np=(N0STRU *)p;
#if TRNTRACE
printf("noderead - teste com np->pos=%"_LD_"\n",(LONGX)np->pos);
#endif
	    if (np->pos == punt)
		return(np); /* CNV_PCBINUM ok */
	}
#else
    for (n=used, p=basep; n--; p+=nodesize) {
	np=(N0STRU *)p;
#if TRNTRACE
printf("noderead - teste com np->pos=%"_LD_"\n",(LONGX)np->pos);
#endif
	if (np->pos == punt)
	    return(np); /* CNV_PCBINUM ok */
    }
    if (left) {
	p=basep+nodesize*used;                  /* first unused buffer */
	left--; nxp->left=left;
    }
    else {
	p=basep+nodesize*(punt%total);          /* any one */
    }
#endif

#if CNV_PCFILES
    nodesize-=(treecase)?TWORDN*N2BUNI:TWORDN*N1BUNI;
#endif
    xbytes=((off_t)(punt-1L))*nodesize;
#if CNLI
    xbytes+=invp->cn_offset[treecase];
#endif
#if TRNTRACE
printf("noderead - xbytes=%"P_OFF_T" nodesize=%d \n",(LONG_LONG)xbytes,nodesize);
#endif
    if (LSEEK64(fd,xbytes,SEEK_SET) != xbytes)
	fatal("noderead/lseek");
#if TRNTRACE
printf("noderead - read em %p (punt=%"_LD_")\n",p,punt);
#endif
#if CNV_PCFILES
    if (CIREAD(fd,nodeunibuff,nodesize) != nodesize) fatal("noderead/read");
#if CNV_PCBINUM
    if (treecase) ConvertN02_REC(nodeunibuff);
             else ConvertN01_REC(nodeunibuff);
#endif
    np=(N0STRU *)p;
    n1p=(N1STRU *)p;
    n2p=(N2STRU *)p;
    unp=(N0STRU *)nodeunibuff;
    np->pos=unp->pos;
    np->ock=unp->ock;
    np->it=unp->it;
    up=unp->idxchars;
    for (n=0; n < TWORDN; n++, up+=sizeof(PUNT))
	if (treecase) {
	    memmove(n2p->idx[n].key,up,LE2); up+=LE2;
	    memmove(&(n2p->idx[n].punt),up,sizeof(PUNT));
	}
	else {
	    memmove(n1p->idx[n].key,up,LE1); up+=LE1;
	    memmove(&(n1p->idx[n].punt),up,sizeof(PUNT));
	}
#else /* CNV_PCFILES */
    if (CIREAD(fd,p,nodesize) != nodesize) fatal("noderead/read");
#if CNV_PCBINUM
    if (treecase) ConvertN02_REC(p);
             else ConvertN01_REC(p);
#endif
#endif
}                                                               /* v3.3 */
    np=(N0STRU *)p;
#if !SUNBUG_GCC1
    if (np->pos != punt)
	fatal("noderead/pos");
    if (np->it  != treecase+1)
	fatal("noderead/it");
    if (np->ock < 1 || np->ock > TWORDN)
	fatal("noderead/ock");
#endif /* !SUNBUG_GCC1 */
    return(np);
}
#if CICPP
L0STRU * CISISX :: xleafread(UCHR   *fbufp,
			      INVMAP *invp,
			      int     treecase,
			      PUNT    punt,
			      int     lxx)
#else /* CICPP */
L0STRU *leafread(fbufp,invp,treecase,punt,lxx)                          /*
----------------
		    posiciona registro desejado;
		    le registro;
		    retorna pointer L0STRU ou abenda;
								      */
UCHR *fbufp;        /* endereco do buffer de entrada */
INVMAP *invp;       /* ptr descriptor do inverted file */
int treecase;       /* indece da B*Tree a pesquisar */
PUNT punt;          /* registro desejado */
int lxx;            /* dbx samel */
#endif /* CICPP */
{
    L0STRU *lp;
    int lbufsiz,fd;
    int lidxesiz;
    int n;
    UCHR *p,*lbufp;
    off_t xbytes;
#if ZTREE
    LZSTRU lzxleaf;
    int lzxeq,lzxkl,lzxne,iock;
    unsigned char *lzxkp;
    unsigned char lzx0k[LE2+1];
    L1IDXE *lzx1p;
    L2IDXE *lzx2p;
#endif
#if SAMEL
    UCHR *iflxp;
    UCHR *lxlbufp;
    int lxlbufsiz;
    LONGX lbytes;
    int klen,loop;
    L1STRU *l1p;
    L2STRU *l2p;
    LXL1STRU *lxl1p;
    LXL2STRU *lxl2p;
    L0STRU *lxlp;
    UWORD lxi;
#else
#if SAMTRACx
    int loop;
    L1STRU *l1p;
    L2STRU *l2p;
#endif
#endif
#if LEAFCNV_PCFILES
    L0STRU *unp;
    char *up;
    L1STRU *l1p;
    L2STRU *l2p;
#endif

    lrlrseek=0L;
    lrlrsize=0L;

    lbufp=fbufp;
#if TRLTRACE
printf("leafread - treecase=%d  punt=%"_LD_"  lbufp=%p  lxx=%d\n",
 treecase,(LONGX)punt,lbufp,lxx);
#if SAMEL
printf("leafread - lxn=%d\n",invp->iflxn);
#endif
#endif
#if LINDLUX
    if (invp->luxpages[treecase]) {
	if (punt > invp->luxpages[treecase]) fatal("leafread/punt");
	if ((lp=invp->luxvpagp[treecase][punt]) == NULL) fatal("leafread/lux");
	if (lbufp) {
	    memmove(lbufp,lp,(treecase)?L2BSIZ:L1BSIZ);
	    return((L0STRU *)lbufp);
	}
	return(lp);
    }
#endif /* LINDLUX */
#if SAMEL
    if (lxx < ((invp->iflxn) ? 1 : 0) || lxx > invp->iflxn)
#else
    if (lxx)
#endif
	fatal("leafread/lxx");
    if (punt < 1)
	fatal("leafread/punt");
    if (treecase) {
	lbufsiz=L2BSIZ;
	fd=invp->l2opn;
	lidxesiz=sizeof(L2IDXE);
    }
    else {
	lbufsiz=L1BSIZ;
	fd=invp->l1opn;
	lidxesiz=sizeof(L1IDXE);
    }
#if SAMEL
    if (invp->iflxn) {
	if (treecase) {
	    lbytes=sizeof(LXL2STRU);
	    lbytes+=(invp->iflxn-1)*sizeof(lxl2p->idx);
	    if (!invp->ifl2p) { /* ALLOC ok */
#if SAMTRACE
printf("leafread - treecase=%d lbytes=%d+%d*%d=%"_LD_"\n",
 treecase,sizeof(LXL2STRU),invp->iflxn-1,sizeof(lxl2p->idx),lbytes);
#endif
#if CICPP
		try { invp->ifl2p= new char[lbytes]; }
		catch (BAD_ALLOC) {invp->ifl2p = (char *)ALLONULL; }
#else /* CICPP */
		invp->ifl2p=(char *)ALLOC((ALLOPARM)lbytes);
#endif /* CICPP */
		if (invp->ifl2p == (char *)ALLONULL) fatal("leafread/ALLOC");
		memset(invp->ifl2p,0x00,(size_t)lbytes);
	    }
	    iflxp=invp->ifl2p;
	}
	else {
	    lbytes=sizeof(LXL1STRU);
	    lbytes+=(invp->iflxn-1)*sizeof(lxl1p->idx);
	    if (!invp->ifl1p) { /* ALLOC ok */
#if SAMTRACE
printf("leafread - treecase=%d lbytes=%d+%d*%d=%"_LD_"\n",
 treecase,sizeof(LXL1STRU),invp->iflxn-1,sizeof(lxl1p->idx),lbytes);
#endif
#if CICPP
		try { invp->ifl1p= new char[lbytes]; }
		catch (BAD_ALLOC) {invp->ifl1p = (char *)ALLONULL; }
#else /* CICPP */
		invp->ifl1p=(char *)ALLOC((ALLOPARM)lbytes);
#endif /* CICPP */
		if (invp->ifl1p == (char *)ALLONULL) fatal("leafread/ALLOC/2");
		memset(invp->ifl1p,0x00,(size_t)lbytes);
	    }
	    iflxp=invp->ifl1p;
	}
	lxlbufsiz=lbufsiz;                              /* save */
	lxlbufp=lbufp;                                  /* save */
	lxlp=(L0STRU *)lbufp;                           /* save */
	lbufsiz=lbytes;                                 /* set */
	lbufp=iflxp;                                    /* set */
#if SAMTRACE
printf("leafread - treecase=%d  punt=%"_LD_"  lxx=%d lxn=%d bytes=%"_LD_"/%p/%d\n",
 treecase,(LONGX)punt,lxx,invp->iflxn,lbytes,lbufp,lbufsiz);
#endif
    }
#endif /* SAMEL */
    lp=(L0STRU *)lbufp;                                 /* use */
#if TRLTRACE
printf("leafread - treecase=%d  punt=%"_LD_"  lbufp=%p  lp=%p\n",
 treecase,(LONGX)punt,lbufp,lp);
#if CNLI
printf("leafread - fd=%d  offset=%"_LD_"\n",fd,invp->cl_offset[treecase]);
#endif
#endif
    /* might ignore possible old leaf's ... */
    if (treecase != (lp->it-1) || punt != lp->pos) {
#if TRLTRACE
printf("leafread - invp=%p  treecase=%d  punt=%"_LD_"\n",
    invp,treecase,(LONGX)punt);
#endif
	p=(unsigned char *)invp->lybasep[treecase];                              /* v3.3 */
	if (fd == 0 && p == NULL)
	    fatal("leafread/fd&p");
	if (p != NULL) {
	    p+=(lbufsiz*(punt-1L));
#if TRLTRACE
printf("leafread - lybasep[%d][%"_LD_"]=%p  p=%p  lp=%p\n",
  treecase,(punt-1L),invp->lybasep[treecase],p,lp);
#endif
	    memmove(lbufp,p,lbufsiz);                    /* use */
#if CNV_PCBINUM
	    if (treecase) ConvertL02_REC(lbufp);
	             else ConvertL01_REC(lbufp);
#endif
#if TRLTRACE
printf("leafread - moved pos=%"_LD_" ock=%d it=%d ps=%"_LD_"\n",
 lp->pos,lp->ock,lp->it,(LONGX)lp->ps);
#endif
	}
	else {                                                  /* v3.3 */
#if LEAFCNV_PCFILES
	    lbufsiz-=(treecase)?TWORDF*L2BUNI:TWORDF*L1BUNI;
	    xbytes=((off_t)(punt-1L))*lbufsiz;
#if TRLTRACE
printf("leafread - xbytes=%"P_OFF_T" punt=%"_LD_" lbufsiz=%d \n",(LONG_LONG)xbytes,(LONGX)punt, lbufsiz);
#endif
        if (LSEEK64(fd,xbytes,SEEK_SET) != xbytes) {
            printf("leafread - xbytes=%"P_OFF_T" punt=%"_LD_" lbufsiz=%d \n",(LONG_LONG)xbytes,(LONGX)punt, lbufsiz);            
            printf("errno=%"_LD_"\n", (long)errno);
            fatal("leafread/lseek");
        }
	    lrlrseek=xbytes;
	    lrlrsize=lbufsiz;
	    if (CIREAD(fd,leafunibuff,lbufsiz) != lbufsiz)      /* use */
		fatal("leafread/read");
#if CNV_PCBINUM
	    if (treecase) ConvertL02_REC(leafunibuff);
	             else ConvertL01_REC(leafunibuff);
#endif
    lp=(L0STRU *)lbufp;
    l1p=(L1STRU *)lbufp;
    l2p=(L2STRU *)lbufp;
    unp=(L0STRU *)leafunibuff;
    lp->pos=unp->pos;
    lp->ock=unp->ock;
    lp->it=unp->it;
    lp->ps=unp->ps;
    up=unp->idxchars;
    for (n=0; n < TWORDF; n++)
	if (treecase) {
	    memmove(l2p->idx[n].key,up,LE2); up+=LE2;
	    memmove(&(l2p->idx[n].info1),up,sizeof(PUNT)); up+=sizeof(PUNT);
	    memmove(&(l2p->idx[n].info2),up,sizeof(PUNT)); up+=sizeof(PUNT);
	}
	else {
	    memmove(l1p->idx[n].key,up,LE1); up+=LE1;
	    memmove(&(l1p->idx[n].info1),up,sizeof(PUNT)); up+=sizeof(PUNT);
	    memmove(&(l1p->idx[n].info2),up,sizeof(PUNT)); up+=sizeof(PUNT);
	}
#else /* LEAFCNV_PCFILES */
	    xbytes=((off_t)(punt-1L))*lbufsiz;
#if ZTREE
	    if (invp->iflzx) xbytes=punt-1; /* punt < 0 */
#endif
#if CNLI
	    xbytes+=invp->cl_offset[treecase];
#endif
#if TRLTRACE
printf("leafread - xbytes=%"P_OFF_T" punt=%"_LD_" lbufsiz=%d \n",(LONG_LONG)xbytes,(LONGX)punt, lbufsiz);
#endif
        if (LSEEK64(fd,xbytes,SEEK_SET) != xbytes) {
            printf("leafread - xbytes=%"P_OFF_T" punt=%"_LD_" lbufsiz=%d \n",(LONG_LONG)xbytes,(LONGX)punt, lbufsiz);            
            printf("errno=%"_LD_"\n", (LONGX)errno);
            fatal("leafread/lseek/2");
        }
	    lrlrseek=xbytes;
	    lrlrsize=lbufsiz;
#if ZTREE
	    if (invp->iflzx) {
		/* read max length */
		n = (treecase) ? sizeof(LZSTRU) : LZHSIZE + LZKASZ1;
		lrlrsize=n;
		if (CIREAD(fd,(char *)&lzxleaf,n) != n)      /* use */
		    fatal("leafread/read");
#if CNV_PCBINUM
		fatal("leafread/ZTREE");
		/* ConvertLZX_REC((char *)&lzxleaf); */
#endif
		/* move header */
		lp->pos = lzxleaf.pos;
		lp->ock = lzxleaf.ock;
		lp->it  = lzxleaf.it;
		lp->ps  = lzxleaf.ps;
#if LIND
		lp->psb = lzxleaf.psb;
#endif
if (trmtrace) {
 printf("leafread - treecase=%d punt=%"_LD_" pos=%"_LD_" ock=%d it=%d ps=%"_LD_,
  treecase,punt,(LONGX)lp->pos,lp->ock,lp->it,(LONGX)lp->ps);
#if LIND
 printf("/%"_LD_" \n",(LONGX)lp->psb);
#else
 printf(" \n");
#endif
}
		/* setup ptrs */
		lzxkp = (unsigned char *)lzxleaf.keys;
		lzx1p = (L1IDXE *)(L1STRU *)lp->idxchars;
		lzx2p = (L2IDXE *)(L2STRU *)lp->idxchars;
		memmove(lzx0k,lzxkp+1+1,(treecase)?LE2:LE1);
		/* decompress existing terms */
		for (iock=0; iock < lzxleaf.ock; iock++) {
		    lzxeq = (int)*lzxkp++; /* eq */
		    lzxkl = (int)*lzxkp++; /* keylen */
		    lzxne = lzxkl-lzxeq;
		    if (treecase) {
			if (lzxeq+lzxne > LE2) fatal("leafread/ztree/LE2");
			memmove(p=lzx2p->key,lzx0k,lzxeq); p+=lzxeq;
			memmove(p,lzxkp,lzxne); p+=lzxne;
			memset(p,' ',LE2-lzxkl);
			lzx2p->info1 = lzxleaf.info1[iock];
			lzx2p->info2 = lzxleaf.info2[iock];
#if LIND
			lzx2p->info3info4.info3 = lzxleaf.info3[iock];
#endif
			memmove(lzx0k,lzx2p->key,LE2);
if (trmtrace) {
  printf("+++ %2d|%3d|%3d|",iock,lzxeq,lzxne+lzxeq);
  for (p=lzx2p->key; lzxkl--; p++) printf("%c",*p);
  printf("|\n");
}
			lzx2p++;
		    }
		    else {
			if (lzxeq+lzxne > LE1) fatal("leafread/ztree/LE1");
			memmove(p=lzx1p->key,lzx0k,lzxeq); p+=lzxeq;
			memmove(p,lzxkp,lzxne); p+=lzxne;
			memset(p,' ',LE1-lzxkl);
			lzx1p->info1 = lzxleaf.info1[iock];
			lzx1p->info2 = lzxleaf.info2[iock];
#if LIND
			lzx1p->info3info4.info3 = lzxleaf.info3[iock];
#endif
			memmove(lzx0k,lzx1p->key,LE1);
if (trmtrace) {
  printf("+++ %2d|%3d|%3d|",iock,lzxeq,lzxne+lzxeq);
  for (p=lzx1p->key; lzxkl--; p++) printf("%c",*p);
  printf("|\n");
}
			lzx1p++;
		    }
		    lzxkp+=lzxne;
		}
		/* setup inexisting terms */
		for (; iock < TWORDF; iock++) {
		    if (treecase) {
			memset(lzx2p->key,' ',LE2);
			lzx2p->info1 = 0;
			lzx2p->info2 = 0;
#if LIND
			lzx2p->info3info4.info3 = 0;
#endif
			lzx2p++;
		    }
		    else {
			memset(lzx1p->key,' ',LE1);
			lzx1p->info1 = 0;
			lzx1p->info2 = 0;
#if LIND
			lzx1p->info3info4.info3 = 0;
#endif
			lzx1p++;
		    }
		    lzxkp+=lzxne;
		}
	    }
	    else
#endif /* ZTREE */
	    if (CIREAD(fd,lbufp,lbufsiz) != lbufsiz)      /* use */
		fatal("leafread/read");
#if CNV_PCBINUM
	    if (treecase) ConvertL02_REC(lbufp);
	             else ConvertL01_REC(lbufp);
#endif
#endif /* LEAFCNV_PCFILES */
	}                                                       /* v3.3 */
	if ((n=lp->ock) < 1 || n >TWORDF)
	    fatal("leafread/ock");
#if SAMEL
	if (invp->iflxn) {
	    lxi=lxx-1;
	    memmove(lxlbufp,lbufp,sizeof(L0STRU));       /* move */
	    if (treecase) {
		l2p=(L2STRU *)lxlbufp; lxl2p=(LXL2STRU *)lbufp; klen=LE2;
		for (loop=0; n--; loop++) {
		    memmove(l2p->idx[loop].key,lxl2p->key[loop],klen);
		    l2p->idx[loop].info1=lxl2p->idx[lxi][loop].info1;
		    l2p->idx[loop].info2=lxl2p->idx[lxi][loop].info2;
#if LIND
		    l2p->idx[loop].info3info4.info3=
				    lxl2p->idx[lxi][loop].info3info4.info3;
#endif
		}
	    }
	    else {
		l1p=(L1STRU *)lxlbufp; lxl1p=(LXL1STRU *)lbufp; klen=LE1;
		for (loop=0; n--; loop++) {
		    memmove(l1p->idx[loop].key,lxl1p->key[loop],klen);
		    l1p->idx[loop].info1=lxl1p->idx[lxi][loop].info1;
		    l1p->idx[loop].info2=lxl1p->idx[lxi][loop].info2;
#if LIND
		    l1p->idx[loop].info3info4.info3=
				    lxl1p->idx[lxi][loop].info3info4.info3;
#endif
		}
	    }
	    lbufsiz=lxlbufsiz;                          /* restore */
	    lbufp=lxlbufp;                              /* restore */
	    lp=lxlp;                                    /* restore */
#if SAMTRACE
printf("leafread - treecase=%d  punt=%"_LD_"  lxx=%d lxn=%d bytes=%"_LD_"/%p/%d\n",
 treecase,(LONGX)punt,lxx,invp->iflxn,lbytes,lbufp,lbufsiz);
#endif
	} /* end if invp->iflxn */
#endif /* SAMEL */
#if SAMTRACx
if (treecase) {
 l2p=(L2STRU *)lbufp;
 for (loop=0; loop < l2p->ock; loop++)
  printf("%d/%d=%.30s=%"_LD_"/%"_LD_"/%"_LD_"\n",loop+1,l2p->ock,
   l2p->idx[loop].key,
   l2p->idx[loop].info1,
   l2p->idx[loop].info2,
#if LIND
   l2p->idx[loop].info3info4.info3);
#else
   0L);
#endif
 getchar();
}
else {
 l1p=(L1STRU *)lbufp;
 for (loop=0; loop < l1p->ock; loop++)
  printf("%d/%d=%.10s=%"_LD_"/%"_LD_"/%"_LD_"\n",loop+1,l1p->ock,
   l1p->idx[loop].key,
   l1p->idx[loop].info1,
   l1p->idx[loop].info2,
#if LIND
   l1p->idx[loop].info3info4.info3);
#else
   0L);
#endif
 getchar();
}
#endif
	for (p=lbufp+lbufsiz, n=TWORDF - lp->ock; n--; ) {
	    p-=lidxesiz; *p='\0';  /* inactive keys were not cleared */
	    /* printf("p=%p\n",p); */
	}
    } /* end if punt != lp->pos */
    lp=(L0STRU *)fbufp;                         /* !!! */       /* 15/04/92 */
    if (lp->ock < TWORDF) {
	p=(unsigned char *)lp->idxchars;
	p+=lidxesiz*lp->ock;
	if (*p != '\0') {
	    /* printf("p=%p+%d*%d=%p\n",lp->idxchars,lidxesiz,lp->ock,p); */
	    printf("leafread/check/punt=%"_LD_" -> char=%c(%02x)",punt,*p,*p);
	    if (getchar() != '\n') fatal("leafread/getchar");
	}
    }
#if TRLTRACE
printf("leafread - ock=%d  it=%d  ps=%"_LD_"\n",lp->ock,lp->it,(LONGX)lp->ps);
#endif
    return(lp);
}

#if CICPP
#if !LIND
IFPSTRU * CISISX :: xpostread(UCHR   *xbufp,
			       INVMAP *invp,
			       PUNT    xblk,
			       int     lxx)
#else
IFPSTRU * CISISX :: xpostread(UCHR   *xbufp,
			       INVMAP *invp,
			       off_t   addr,
			       unsigned int toread,
			       int     lxx)
#endif
#else /* CICPP */
#if !LIND
IFPSTRU *postread(xbufp,invp,xblk,lxx)
#else
IFPSTRU *postread(xbufp,invp,addr,toread,lxx)
#endif
								     /*
----------------
		    posiciona registro desejado;
		    le registro;
		    retorna pointer IFPSTRU ou abenda;
								      */
UCHR *xbufp;        /* endereco do buffer de entrada */
INVMAP *invp;       /* ptr descritor do inverted file */
#if !LIND
PUNT xblk;          /* registro desejado */
#else
off_t addr;          /* byte address */
unsigned int toread;/* bytes to read */
#endif
int lxx;            /* dbx samel */
#endif /* CICPP */
{
    IFPSTRU *xp;
    int fd;
    unsigned int xbufsiz;
#if IFUPDATE
    LONGX nread;
#endif
    off_t sbyte;
    UCHR *p;

#if SAMEL
    if (lxx < 0) fatal("postread/lxx");
    fd=invp->ifopn[(lxx) ? lxx-1 : 0];
#else
    fd=invp->ifopn;
    if (lxx) fatal("postread/lxx");
#endif

    xp=(IFPSTRU *)xbufp;
#if !LIND
    xbufsiz=sizeof(IFPSTRU);
#if TRYTRACE
printf("postread - invp=%p  xblk=%"_LD_"  xbufp=%p  xp=%p  fd=%d \n",
 invp,(LONGX)xblk,xbufp,xp,fd);
#endif
#else
    xbufsiz=toread;
#if TRYTRACE
printf("postread - invp=%p  addr=%"P_OFF_T"/%d  xbufp=%p  xp=%p  fd=%d \n",
 invp,(LONG_LONG)addr,xbufsiz,xbufp,xp,fd);
#endif
#endif

#if !LIND
#if IFUPDATE
    if (trmifupd || xblk != xp->ifpblk) {                       /* 15/04/93 */
#else
    if (xblk != xp->ifpblk) {
#endif
#endif
	p=(unsigned char *)invp->iybasep;
	if (fd == 0 && p == NULL)
	    fatal("postread/fd&p");
	if (p != NULL) {
#if !LIND
	    p+=(xbufsiz*(xblk-1L));
#else
	    p+=addr;
#endif
#if TRYTRACE
printf("postread - iybasep=%p  p=%p\n",invp->iybasep,p);
#endif
	    memmove(xbufp,p,xbufsiz);
#if !LIND
#if TRYTRACE
printf("postread - blk=%"_LD_" \n",xp->ifpblk);
#endif
#endif
	}
	else {
#if !LIND
	    sbyte=((off_t)(xblk-1L))<<IFSHIFT;
#else
	    sbyte=addr;
#endif
#if CNLI
	    sbyte+=invp->ci_offset;
#endif
#if TRYTRACE
printf("postread - sbyte=%"P_OFF_T" \n",(LONG_LONG)sbyte);
#endif
	    if (LSEEK64(fd,sbyte,SEEK_SET) != sbyte)
		fatal("postread/lseek");
#if IFUPDATE
            if ((nread=CIREAD(fd,xbufp,xbufsiz)) != xbufsiz)
                if (trmifupd && !nread) { /* next block */
#if LIND
                    memset(xbufp,0x00,xbufsiz);
#else
                    memset(xbufp,0xFF,xbufsiz);
                    ((IFPSTRU *)xbufp)->ifpblk=xblk;
#endif
                }
                else fatal("postread/read");
#else /* IFUPDATE */
            if (CIREAD(fd,xbufp,xbufsiz) != xbufsiz) fatal("postread/read");
#endif /* IFUPDATE */
	}
#if !LIND
#if CNV_PCBINUM
        ConvertIFP_BLKCTL(xbufp,(xblk == 1) ? 1 : 0);
#endif
    }
#endif
#if !LIND
    if (xp->ifpblk != xblk)
	 fatal("postread/check/xblk");
#endif
    return(xp);
}

#endif /* CICPP || CITRM_CISISX_SOURCE */


