/* ------------------------------ mxaot.c ---------------------------------- */

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#if CIAPI
#include "ciapi.h"
#else /* CIAPI */
#include "cisis.h"                  /* CISIS Interface header file */
#endif /* CIAPI */

#include "mx.h"                     /* MX Program header file */

#define DIR_FILE 1                  /* list=directory */
#if DIR_FILE
#include <sys/types.h>
#include <dirent.h>
#endif 

#define XML_FILE 1                  /* xml=file,tag */
#if XML_FILE
    char tagName[64];

    char * findTag(char     *in,       /* input string */
                   char     *tagName,  /* tag name */
                   int      *type,     /* -1-unknown 0-open 1-close 2-openclose */
                   unsigned *len) {    /* tag length including < and > */

        char *initPos = NULL;
        char *curPos = in;
        int tagLen;

        *type = -1; /* unknown */
        *len = 0;

        if (tagName != NULL) {
            tagLen = strlen(tagName);
            while (curPos != NULL) {
                curPos = strchr(curPos, '<');
                if (curPos != NULL) {
                    initPos = curPos;
                    curPos++;
                    if (*curPos == '/') {
                        *type = 1; /* close */
                        curPos++;
                    }
                    if (strncmp(curPos, tagName, tagLen) == 0) {
                        curPos += tagLen;
                        if ((*curPos == ' ') || (*curPos == '/') || (*curPos == '>')) {
                            while ((*curPos != '\0') && (*curPos == ' ')) {
                                curPos++;
                            }
                            while ((*curPos != '\0') && (*curPos != '/') && (*curPos != '>')) {
                                curPos++;
                            }
                            if (*curPos == '/') {
                                *type = 2; /* openclose */
                                curPos++;
                            }
                            if (*curPos == '>') {
                                if (*type == -1) {
                                    *type = 0; /* open */
                                }
                                break;
                            }
                        }
                    }
                    initPos = NULL;
                    *type = -1;
                }
            }
            if (curPos != NULL) {
                 *len = curPos - initPos + 1;
            }
        }

        return initPos;
    }

int recfgetxml(char *xmlfile, char *ibuf, unsigned ibufsiz, char *obuf, unsigned obufsiz, char *ubuf, unsigned ubufsiz, int *fdp, unsigned *n1p, char **iptrp, LONGX *countp);

int recfgetxml(char *xmlfile, char *ibuf, unsigned ibufsiz, char *obuf, unsigned obufsiz, char *ubuf, unsigned ubufsiz, int *fdp, unsigned *n1p, char **iptrp, LONGX *countp)
{
    int fd=fdp[0];
    unsigned n1=n1p[0];
    char *iptr=iptrp[0];
    LONGX count=countp[0];
    char *batchp = ubuf;
    unsigned ubu=ubufsiz-20;
    unsigned j=0;  /* indice em obuf */
    int eol=0;
    int rc=0;
    int insideTag = 0;
    int newInstance = 0;
    int closeFound = 0;
    int type;
#if PC
    int nlSize = 2;
#else
    int nlSize = 1;
#endif

    *ubuf='\0';                                            // null update comand

    if (!iptr) {                                           // close/open file
        if (fd) CLOSE(fd);
        dbxopt_fatal=0; fd=dbxopen(NULL,xmlfile,NULL);
        if (fd < 0) return(fd);
        n1=0; iptr=""; //force read
        rc=fd;
    }

    /* read lines
    */
    if (obufsiz) {
        while (1) {
            char *jptr;
            unsigned jn=0;
            if (n1 == 0) {                                 // le buffer
                n1=read(fd,ibuf,ibufsiz-1);
                if (n1 <= 0) {  // if (j) ;
                    rc=EOF; break;                         // EOF
                }
                ibuf[n1]='\0';
                iptr=ibuf;
            }
            jptr=iptr;
#if PC
            while (n1 && *iptr != '\r') { jn++; iptr++; n1--; }
            if (*iptr == '\r') { iptr++; n1--; }
#else
            while (n1 && *iptr != '\n') { jn++; iptr++; n1--; }
#endif
            if (*iptr == '\n') { iptr++; n1--; eol=1; }
            if (jn) {
                if (j+jn >= obufsiz) jn=(obufsiz-1)-j;
                memmove(obuf+j,jptr,jn); j+=jn;
            }
            if (eol) {
                char *p,*p0;
                unsigned len,bk;
                count++;

                obuf[j]='\0';
                p0=p=obuf;
                p = findTag(p, tagName, &type, &len);
                if (insideTag) {
                    if (p == NULL) {
                        len = strlen(p0);
                    } else {
                        if (type != 1) { /* close */
                            fatal("nested tag not supported");
                        }
                        p += len;
                        insideTag = 0;
                        closeFound = 1;
                        len = p - p0;
                        if (*p != '\0') {
                            bk = (unsigned)((obuf + j) - p + nlSize);
                              if (bk > nlSize) {
                                iptr -= bk;
                                n1 += bk;
                            }
                        }
                    }
                } else {
                    if (p == NULL) {
                        len = 0;
                    } else {
                        if (type == 0) { /* open */
                            p0 = p;
                            p += len;
                            insideTag = 1;
                            newInstance = 1;

                            p = findTag(p, tagName, &type, &len);
                            if (p == NULL) {
                                len = strlen(p0);
                            } else {
                                if (type != 1) { /* close */
                                    fatal("nested tag not supported");
                                }
                                p += len;
                                insideTag = 0;
                                closeFound = 1;
                                len = p - p0;
                                if (*p != '\0') {
                                    bk = (unsigned)((obuf + j) - p + nlSize);
                                    if (bk > nlSize) {
                                        iptr -= bk;
                                        n1 += bk;
                                    }
                                }
                            }
                        } else if (type == 2) { /* openclose */
                            p0 = p;
                            p += len;
                            insideTag = 0;
                            newInstance = 1;
                            closeFound = 1;
                            if (*p != '\0') {
                                bk = (unsigned)((obuf + j) - p + nlSize);
                                if (bk > nlSize) {
                                    iptr -= bk;
                                    n1 += bk;
                                }
                            }
                        } else {
                            fatal("invalid tag");
                        }
                    }
                }
                if (len) {
                    if (newInstance) {
                        if (ubu <= 7) fatal("ubu <= 7");
                        sprintf(batchp,"d1<1 0>"); batchp+=7;
                        ubu-=7;
                        newInstance = 0;
                    }

                    if (len >= ubu) fatal("len >= ubu");
                    memmove(batchp,p0,len); batchp+=len;
                    ubu-=len;

                    if (closeFound) {
                        if (ubu <= 4) fatal("ubu <= 4");
                        sprintf(batchp,"</1>");
                        batchp+=4;
                        ubu-=4;
                    } else {
#if PC
                        if (ubu < 1) fatal("ubu < 1");
                        sprintf(batchp,"\r"); batchp+=1;
                        ubu-=1;
#endif
                        if (ubu < 1) fatal("ubu < 1");
                        sprintf(batchp,"\n"); batchp+=1;
                        ubu-=1;
                    }
                } else if (insideTag) {
#if PC
                    if (ubu < 1) fatal("ubu < 1");
                    sprintf(batchp,"\r"); batchp+=1;
                    ubu-=1;
#endif
                    if (ubu < 1) fatal("ubu < 1");
                    sprintf(batchp,"\n"); batchp+=1;
                    ubu-=1;
                }
                if (closeFound) {
                    rc=1; break;
                } else {
                    j=0;
                }
            } //end if eol
        } // end while
    } // end if obufsiz

    /* save
    */
    fdp[0]=fd;
    n1p[0]=n1;
    iptrp[0]=iptr;
    countp[0]=count;

    return rc;
}
#endif /* XML_FILE */

/* ------------------------------ fxl0.c ---------------------------------- */
//int read(int handle, void *buf, unsigned len);
//The maximum number of bytes that read can read is UINT_MAX -1, because UINT_MAX is the same as -1, the error return indicator.
//On successful completion, read returns an integer indicating the number of bytes placed in the buffer.
//On end-of-file, read returns 0. On error, read returns -1 and sets the global variable errno to one of the following values:

int recfgetseq(char *seqfile, char c, int parmseqlog, char *ibuf, unsigned ibufsiz, char *obuf, unsigned obufsiz, char *ubuf, unsigned ubufsiz, int *fdp, unsigned *n1p, char **iptrp, LONGX *countp);

int recfgetseq(char *seqfile, char c, int parmseqlog, char *ibuf, unsigned ibufsiz, char *obuf, unsigned obufsiz, char *ubuf, unsigned ubufsiz, int *fdp, unsigned *n1p, char **iptrp, LONGX *countp)
{
    int fd=fdp[0];
    unsigned n1=n1p[0];
    char *iptr=iptrp[0];
    LONGX count=countp[0];

    unsigned j=0;  /* indice em obuf */
    int eol=0;
    int rc=0;

    if (!c) c='|';                                         // default dlm
    *ubuf='\0';                                            // null update comand

    if (!iptr) {                                           // close/open file
        if (fd) CLOSE(fd);
        dbxopt_fatal=0; fd=dbxopen(NULL,seqfile,NULL);
        if (fd < 0) return(fd);
        n1=0; iptr=""; //force read
        rc=fd;
    }

    /* read lines
    */
    if (obufsiz) {
        while (n1 >= 0) {
            char *jptr;
            unsigned jn=0;
            if (n1 == 0) {                                 // le buffer
                n1=read(fd,ibuf,ibufsiz-1);
                if (n1 <= 0) {  // if (j) ;
                    rc=EOF; break;                         // EOF
                }
                ibuf[n1]='\0';
                iptr=ibuf;
            }
            jptr=iptr;
    #if PC
            while (n1 && *iptr != '\r') { jn++; iptr++; n1--; }
            if (*iptr == '\r') { iptr++; n1--; }
    #else
            while (n1 && *iptr != '\n') { jn++; iptr++; n1--; }
    #endif
            if (*iptr == '\n') { iptr++; n1--; eol=1; }
            if (jn) {
                if (j+jn >= obufsiz) jn=(obufsiz-1)-j;
                memmove(obuf+j,jptr,jn); j+=jn;
            }
            if (eol) {
                int tag=1;
                char *p,*p0,*batchp;
                unsigned len,u,ubu=ubufsiz-20;
                count++;
                obuf[j]=c; obuf[j+1]='\0';
                for (batchp=ubuf, p=obuf; *p; p++, tag++) {
                    p0=p; p=strchr(p0,c); if (p) *p='\0'; len=strlen(p0);
                    if (len) {
                        if (len >= ubu) len=0;
                        sprintf(batchp,"H%d %d ",tag,len); batchp+=(u=strlen(batchp));
                        memmove(batchp,p0,len+1); batchp+=len;
                        ubu-=(u+len);
                    }
                }
                rc=1; break;
            } //end if eol
        } // end while
    } // end if obufsiz

    /* save
    */
    fdp[0]=fd;
    n1p[0]=n1;
    iptrp[0]=iptr;
    countp[0]=count;

    return rc;

    //if (j != 0) erro("ultimo registro nao tem LF em ",seqfile);
    //exit(0);
}

#if 0
erro(s1,s2)     /* imprime mensagem de erro e morre */
char *s1,*s2;
{
    fprintf(stderr,
        "mx: %s%s\n",s1,s2);
    breakout();
    exit(1);
}

writbsiz()                                                          /*
---------                                                           */
{
    int n2;
    unsigned char *p,*q;
    int loop;

    if (n2free >= obufsize) {
        memmove(bufout+i2free,obuf,obufsize);
        n2free-=obufsize;
        i2free+=obufsize;
    }
    else {
        memmove(bufout+i2free+,obuf,n2free);
        n2=write(f2,bufout,MBUFSIZ);
        if (n2 < MBUFSIZ) erro("writbsiz - ","erro na gravacao");
        i2free=obufsize-n2free;
        memmove(bufout,obuf+n2free,i2free);
        n2free=MBUFSIZ-i2free;
    }
}

breakout()                                                      /*
---------                                                       */
{
    int n2;
    if (i2free) if (f2) {
        n2=write(f2,bufout,i2free);
        if (n2 < i2free) {
            fprintf(stderr,"breakout - erro na gravacao\n");
        exit(1);
    }
    }
}

                                                                /*
---------                                                       */
#endif //0





void mxsyntax(char *x, unsigned xibufsiz, LONGX xptabcategories, int xptabwidth);
void mxsyntax(x, xibufsiz, xptabcategories, xptabwidth)
char *x;
unsigned xibufsiz;
LONGX xptabcategories;
int xptabwidth;
{
        printf("%s",cicopyr("Utility MX"));
        printf("\n");
        printf("mx ");
#if CIPAR
        printf("[cipar=<file>] ");
#endif
        printf("[{");
#if OPEN30
        printf("files|");
#endif
#if BEFORE20060120
        printf("mfrl|fmtl|load}=<n>] [cgi={mx|<v2000_fmt>}] \n");
#else
        printf("mfrl|load}=<n>] [cgi={mx|<v2000_fmt>}] [in=<file>] \n");
#endif
        printf(" %s{[db=]<db>| \n",x);
/*
        printf(" %s seq[/blk:%"_LD_"/fi:<x>]=<file>| \n",x,(LONGX)xibufsiz);
*/
        printf(" %s seq[/%"_LD_"m]=<file>[<fldDelim>]| \n",x,(LONGX)xibufsiz);
#if DIR_FILE
        printf(" %s list={./|<dir>}| \n",x);
#endif
#if XML_FILE
        printf(" %s xml[/%"_LD_"m]=<file>,<element>| \n",x,(LONGX)xibufsiz);
#endif
        printf(" %s iso[={marc|<n>}]=<isofile> [isotag1=<tag>]| \n",x);
        printf(" %s dict=<if>[,<keytag>[,<posttag>[/<postsperrec>]]] [k{1|2}=<key>]} \n",x);
        printf(" \n");
        printf("options: \n");
        printf(" %sfrom|to|loop|count|tell|btell=<n> \n",x);
        printf(" %stext[/show]=<text> \n",x);
        printf(" %s[bool=]{<bool_expr>|@<file>}  ",x);
#if INVXBOOL
        printf("[invx=<tag%d_mf>] ",MFQTXTAG);
#endif
#if CIB71
        printf("[tmpx=<tmp_mf>] ");
#else /* CIB71 */
#if LIND || CIB64
        printf("[tmpx=<tmp_mf>] ");
#else
        printf("[tmpx=<tmp_mf>] ");
#endif
#endif /* CIB71 */
        printf(" \n");
        printf(" \n");
#if RECXPND
        printf(" %sgetdir=<tag>,<dlm> \n",x);
#endif
#if RECGIZM
        printf(" %sgizmo=<gizmo_mf>[,<taglist>]  ",x);
#if MXGIZP
        printf("[gizp[/h]=<out_mfx>] ");
#endif
#if RECDECO
        printf("[decod=<mf>]");
#endif
#else /* RECGIZM */
#if RECDECO
        printf(" decod=<mf>");
#endif
#endif /* RECGIZM */
        printf(" \n");
        printf(" \n");
        printf(" %sjoin=<mf>[:<offset>][,<taglist>]=<mfn=_fmt>\n",x);
        printf(" %sjoin=<db>[:<offset>][,<taglist>]=<upkey_fmt>",x);
        printf(" [jmax=<n>]\n");
        printf(" %sjchk=<if>[+<stwfile>]=<upkey_fmt>",x);
        printf(" \n");
#if SHOWJDICMD || 1
        printf(" %sjdi[/<options>]=<db>=<jdi_fmt>\n",x);
        printf(" %s    /lines:1000/width:60\n",x);
        printf(" %s    /tab:2/occ:50\n",x);
        printf(" %s    /tab:2\n",x);
        printf(" %s    /wmin:0.05/vmin:0.05/nmin:2\n",x);
        printf(" %s    /sort:{w|v|n}{1|2|3|4}\n",x);
        printf(" %s    /top:10\n",x);
        printf(" %s    /55\n",x);
        printf(" %s    /show\n",x);
        printf(" \n");
#endif
        printf(" %sproc=[<proc_fmt>|@<file>] \n",x);
/*
        case 'X': ok
        case 'F':
        case 'Z':
        case 'Y':
        case 'T':
        case 'C':
*/
        printf(" \n");
        printf(" %s      D{<tag>[/<occ>]|*} \n",x);
        printf(" %s      A<tag><delim><data><delim> \n",x);
        printf(" %s      H<tag> <length> <data> \n",x);
        printf(" %s      <TAG[ <stripmarklen>[ <minlen>]]><data></TAG> \n",x);
        printf(" \n");
        printf(" %s      S[<tag>] \n",x);
        printf(" %s      R<mf>,<mfn> \n",x);
        printf(" %s      G<gizmo_mf>[,<taglist>] \n",x);
        printf(" %s      Gsplit[/clean]=<tag>[={<char>|words|letters|numbers|trigrams}] \n",x);
//        printf(" %s      Gsplit[/clean]=<tag>[={<char>|words|letters|numbers}] \n",x);
//        printf(" %s      Gsplit=<tag> \n",x);
//        printf(" %s      Gsplit=<tag>={trigrams|wtrig} \n",x);
        printf(" %s      Gsplit=<tag>=6words[/if=<if>] \n",x);
#if GCC
        printf(" %s      Gload[/<tag>][/nonl][/xml][/socket][/head][={<url>|<file>|dir=<dir>}]\n",x);
#else
        printf(" %s      Gload[/<tag>][/nonl][=<file>] \n",x);
#endif
        printf(" %s      Gmark[/<tag>]{/<elem>|/keys|/decs|/<mf>,<otag>[,<ctag>]}=<if> \n",x);
        printf(" %s      Gmarx[/<tag>]/<elem>[@<att>=\"x\"] =<tag>[:&[<att>]|/c[=224]|/i]\n",x);
        printf(" %s      Gdump[/<tag>][/nonl][/xml][=<file>] \n",x);
#if PROCXSLT
        printf(" %s      Gxsl[#<rec_mode>][/<tag>|:<file>]=[[@]<xsl_spec>] \n",x);
#endif /* PROCXSLT */
        printf(" %s      =<mfn> \n",x);
        printf(" %s      X[append=]<mf> \n",x);
/*
        printf(" %s      Gport[/<tag>][=<port>] \n",x);
        printf(" %s      Gdump[/<tag>][/x]{/socket=<url>|=<file>} \n",x);
*/
        printf(" \n");
#if MXFST
        printf(" %sconvert=ansi [uctab={<file>|ansi}]         [actab={<file>|ansi}] \n",x);
        printf(" %sfst[/h]={<fst>|@[<file>]}                  [stw=@[<file>]] \n",x);
#endif
#if RECXPND
        printf(" %sputdir=<tag>,<dlm> \n",x);
#endif
        printf(" \n");
        printf(" %s",x);
#if MULTI
        printf("[mono|mast|full] ");
#endif
        printf("{create|copy|append|merge|updatf}=<out_mf> \n");
        printf(" %s[out]iso[={marc|<n>}]=<out_isofile>        [outisotag1=<tag>] \n",x);
#if LIND
#if MXFULLINV
        printf(" %sfullinv[/dict][/keep][/ansi]=<out_if>      [maxmfn=<n>|master=<mf>] \n",x);
#endif
#else /* LIND */
#if BEFORE20060711
#if MXIFUPD
#if MXFULLINV
        printf(" %s{ifupd[/create][/dict]|fullinv[/dict][/m][/ansi]}=<out_if> [-reset] \n",x);
#else
        printf(" %sifupd[/create][/dict]=<out_if> \n",x);
#endif
#endif //BEFORE20060711
#else /* MXIFUPD */
#if MXFULLINV
        printf(" %sfullinv[/dict][/m][/ansi]=<out_if> [-reset] \n",x);
#endif
#endif /* MXIFUPD */
#endif /* LIND */
        printf(" %sln{1|2}=<out_file>                         [+fix[/m]] \n",x);
        printf(" %sfix=<out_file> tbin=<tag> \n",x);
#if SHOWTABCMD || 1
        printf(" %stab[/lines:%"_LD_"/width:%"_LD_"/tab:<tag>]=<tab_fmt> \n",x,(LONGX)xptabcategories,(LONGX)xptabwidth);
#endif
        printf(" %sprolog|pft|epilog={<diplay_fmt>|@<file>}   [lw={<n>|0}] [pftout=<file>]\n",x);
        printf(" \n");
        printf(" %s{+|-}control|leader|xref|dir|fields|all    [now] [mfrl] \n",x);
        printf(" \n");
}


#if MXFUN
int cisis_mx(stringp)
char *stringp;
{
#define MAXARGS1 512
    int argc=0;
    char *argv[MAXARGS1+1];
    char *p,*q,c1,c2;
    int strip;
    int rc;
    for (argv[argc++]="mx", p=stringp; *p; ) {
        while (isspace(*p)) p++;
        c1='\r'; c2='\n'; if (*p == '"') { p++; c1=c2='"'; }
        argv[argc++]=p;
        for (strip=0, q=p; *p; ) {
            if (*p == c1) { *p++='\0'; if (*p == c2) p++; break; }
            if (*p == c2) { *p++='\0'; if (*p == c1) p++; break; }
            /* if (*p == '\\') p++; dbn... */
            if (*p) { *q++ = *p++; strip++; }
        }
#if BEFORE401R2 /* because (still) need keep trailing space in "seq=x " */
        if (c1 != '"') while (strip--) if (isspace(*(q-1))) q--; else break;
#endif
        *q='\0';
    }
    argv[argc]=NULL;
    rc=mxexec(argc,argv);
    return(rc);
}
#endif /* MXFUN */


#if MXFUN
#define FATLOCK {                           \
        fprintf(stderr,                     \
        "*** record %s#%"_LD_" is been update by another user!\n",  \
        VRDBname(upcrec),MFRmfn);               \
        mxexit(xmxp,1); /* DELOCK/EWLOCK */             \
       return(1);                                     \
    }
#else /* MXFUN */
#define FATLOCK {                           \
        fprintf(stderr,                     \
        "*** record %s#%"_LD_" is been update by another user!\n",  \
        VRDBname(upcrec),MFRmfn);               \
        mxexit(xmxp,1); /* DELOCK/EWLOCK */             \
    }
#endif /* MXFUN */

#if MXFUN
#define MXEXIT(rc) { mxexit(xmxp,rc); /* DELOCK/EWLOCK */ return(rc); }
#else /* MXFUN */
#define MXEXIT(rc)   mxexit(xmxp,rc); /* DELOCK/EWLOCK */
#endif /* MXFUN */

#if BEFORE20000323           /* because some BRM's GCC - ask rpiva */
extern FILE *cistderr;  /* 20/05/98 (Asael!) */
#else
#define cistderr stderr
#endif

/* function mxexec(argc,argv) derived from the original mx.c source code

   - declares xmxp and mxinit it
   - uses xmxp as 1st parm of call all mx functions
   - #if MXFUN then uses MXFUN_XMX struct and mxrun.h #defines

   AOT, 21/02/2001
*/

int mxexec(argc,argv_1)
int argc;
char *argv_1[];

{
#if CIAPI
#if !MXFUN
    void *ciapip = NULL;
#endif /* !MXFUN */
#endif /* CIAPI */

    void *xmxp; /* mandatory for #defines in mxrun.h (#if !MXFUN) */

    RECSTRU *recp; /* mandatory for defines REC RDB MF0 MFR DIR, FIELDP */


    //char *seqfile;     // seq= using recfgetseq()
    int parmseqlog=0;    // recfgetseq
    char *ibuf;          // input buffer
    unsigned ibufsiz=1*1024*1024;    // dedicated 1Mbyte-buffer
    char *obuf;          // line buffer
    unsigned obufsiz;
    char *ubuf;          // fldupdat buffer
    unsigned ubufsiz;
    int n1fdseq=0;       // file descriptor
    int n1fdxml=0;       // file descriptor
    unsigned n1left=0;   // bytes left
    char *n1iptr=NULL;   // next char
    LONGX n1count=0;      // lines read

    int parmtabtg=0;     // parmtabula

#define TB        1
#define TBISNEW   0

#if TB
    int ntagbin=0;
    UWORD vtagbin[MAXTBIN];
    UWORD tagbin;
    int itagbin;
    int fdtagbin;
#endif /* TB */

    char *dbnp=NULL;
    LONGX crec,irec,mfn;

    char *p,*q,parmxseq,*isxp=NULL;
    int i,n,xfd;
    int level,jchk;
    int isfix,lrfix,isfox,lrfox;
    unsigned int parmisildrtag=0;
    unsigned int parmisoldrtag=0;

#define MXCOMMENT 1
#if MXCOMMENT
    int incomment=0;
#endif

    int argnext=1;
    LONGX nbytes,maxmfrl=MAXMFRL;
    char *parmtextp=NULL;
    char *parmsysp=NULL;
    char *parmsysa=NULL;
    int parmsyss=0;
    char *shp;

    UWORD utag;
    int xdir;
    UCHR *batchp;

    int parmcontrol=0,parmleader=0,parmxref=0,parmdir=0,parmfields=1;
    LONGX parmfrom=0,parmto=0,parmloop=1,parmcount=LONGX_MAX,count=0;
    LONGX countdict1002=LONGX_MAX;
    int parmwait=1;
    int parmhits=0;
    LONGX /*UWORD*/ parmmfrl=0;
    int minus1=0,plus1=0;
    LONGX ld;

#if INVXBOOL
    int loop;
    LONGX irecx;
#endif

#if RECGIZM
    VGIZPSTRU *gizmap;
#endif

#if MXIFUPD
    int ifcreate=0;
    DBXSTRU *ifndbxp;
#endif

#if MXIFUPD || MXFULLINV
    char *parmmaster=NULL;
    int parmmenosreset=0;
#endif

#if MXFST
    int parmfstup=1;
    LONGX qtylk1,qtylk2;
#endif

#if IFUPDATE
    LONGX brec=0L;
    int parmmfbw=0;
#endif

    char *mi_gidbnp=NULL;               /* dbn.par */

    int parmoutmfntag=0;                /* multiple output records, accorging pft= */

    char *parmkdbnp=NULL;
    char *parmk1p=NULL;
    char *parmk2p=NULL;
    char  parmk2a[LE2+1];
    UWORD parmktag1=1;
    UWORD parmktag2=2;
    int parmkpostings=0;
    LONGX parmkkeep2=LONGX_MAX;
    LONGX parmkcount=0L;
    int xgo;

/* iso=mark= */
    int ismarc=0;
    unsigned char parmisoxfs=ISOXFS;
    unsigned char parmisoxgs=ISOXGS;

/* convert= */
    unsigned char *tabconvp=NULL;

    LONGX itrm;
    LONGX itrm2;
    TRMSTRU *trmp=NULL;
    TRMSTRU *trmp1=NULL;
    TRMSTRU *trmp2=NULL;
    int xti;
    char *xtp;
    LONGX xtl1,xtl2;

//AOT, 09/12/2008
/* tab/ jdi:<tempesos_db>= */
#define JDISORTW1 1
#define JDISORTW2 2
#define JDISORTW3 3
#define JDISORTW4 4
#define JDISORTV1 5
#define JDISORTV2 6
#define JDISORTV3 7
#define JDISORTV4 8
#define JDISORTN1 7
#define JDISORTN2 8
#define JDISORTN3 9
#define JDISORTN4 10

    char    *parmtabjdip=NULL;
    LONGX     jdiclass=1000;   /*xptabcategories/3;*/
    int      jdiwidth=LE2;    /*xptabwidth;*/ 
    LONGX     jditabtg=2;
    LONGX     jditaboc=50;
    int      jdisort=JDISORTW1;
    int      jdisstop=10;
    int      jdipshow=0;
    int      jdiadd55=55;
    int      jdiaddfm=2;
    float    jdisminw=0.05;
    float    jdisminv=0.05;
    LONGX     jdisminn=2;

    char    *jditable=NULL;
    LONGX    jdicategories;
    LONGX    jdimaxprim;

    float    jditotalw=0.0;
    float    jditotalv=0.0;
    LONGX     jditotaln=0;
    LONGX     jdintabs=0;
    LONGX     jditotal1=0;
    float   *jdiSUM0w=NULL;  /* n(t) */
    float   *jdiSUM0v=NULL;  /* n(t) */
    LONGX    *jdiSUM0n=NULL;  /* n(t) */
    LONGX    *jdiFREQ1=NULL;  /* n(t) */

    char    *jdiSBUF=NULL;   /* sort keys area */
    LONGX    *jdiSBUH=NULL;   /* sort hidx area */
    char    *jdiSBUFx=NULL;  /* sort tmp key  */
    int      jdislen=0;   /* 9+jdiwidth+1  */

    RECSTRU *jdirecp=NULL;
    TRMSTRU *jditrmp=NULL;
#if CICPP
#else /* CICPP */
    LONGX     jdirec_prat=(-1L);
    LONGX     jditrm_prat=(-1L);
#endif /* CICPP */

    FMT_CODE *jdipgmp=NULL;
    char     *jdifmtarea=NULL;
    LONGX     jdifmtlw=0;
    LONGX     jdifmtasize=MAXMFRL;


    /* ............................................... directory listing */
#if DIR_FILE
    DIR *direntdp=NULL;
    struct dirent *direntep;
#endif


    /* ............................................... get cgi parameters */
#define MAXARGS 512
    char *argv[MAXARGS+1];
    char *argv_0[MAXARGS+1];

/* mxaotx.c
*/
//CICGI
#if CICGI
    int ii,ix;
    char *xp;
    LONGX rc;
#if !CIAPI
    int cgitrace=0;
#endif /* !CIAPI */

    char *cgispecp=NULL;
    char *cgipfxp="tag";
    UWORD cgitag=2000;

    /* int cicgi0(int *argc, char *argv[], char **wwwp, UWORD taghxsub, char *taghpfxp); */
    char *wwwp=NULL;
    char *infop;

#endif


//CICGI
#if MXFUN
    if (mxinit(&xmxp)) return(1);
    if (xmxp == NULL) return(2);
#if CIAPI
    ciapip = cisisApplication( MAXNREC,MAXNTRM );/* mandatory for CIAPI definitions */
#endif /* CIAPI */
#else /* MXFUN */
    xmxp=NULL;
#endif /* MXFUN */


#if CICGI
#if CIAPI
    cgitrace=0;
#endif /* CIAPI */
    argv_0[ix=0]=argv_1[0];
#define TRACX 0
#if TRACX
cgitrace=1;
printf("Content-type: text/plain\n\n");
printf("PATH_INFO=%s\n",getenv("PATH_INFO"));
printf("PATH_TRANSLATED=%s\n",getenv("PATH_TRANSLATED"));
printf("SCRIPT_NAME=%s\n",getenv("SCRIPT_NAME"));
printf("argv_0[0]=%s\n",argv_0[0]);
#endif

    infop=getenv("PATH_INFO"); /* PATH_INFO=/cgi=mx */
    if (infop) {
        xp=strstr(infop,"cgi="); if (!xp) xp=strstr(infop,"cgi~");
        if (xp) if (*xp) {
            cgixpok=argv_0[++ix]=strdup(xp);
            xp=cgixpok+strlen(xp)-1; if (*xp == '/') *xp='\0';
#if TRACX
printf("argv_0[1]=%s\n",argv_0[1]);
#endif
        }
    }

    /* disable PATH_INFO=/cgi= */
    if (cgixpok)
        for (i=1; i < argc; i++)
            if (strcmp(argv_1[i],"cgi=null") == 0) { ix--; cgixpok=NULL; break; }

    argc+=ix; for (ii=1; ii < argc; ) argv_0[++ix]=argv_1[ii++]; argv_0[++ix]=NULL;
    for (ii=0, i=1; i < argc; i++) if (strncmp(argv_0[i],"cgi",3) == 0) { ii=1; break; }
    if (ii) { //some cgii
        /*putenv("REQUEST_METHOD=GET");*/
        /*putenv("QUERY_STRING=db~cds&pft~mfn,x1,v24/&count~2&now&btell~0&bool~plants*water");*/
        for (i=0; i < argc; i++) if (strcmp(argv_0[i],"cgitrace") == 0) cgitrace=1;
        RECORD(wwrec1=nrecs,"null",1L); RECrc=RCNORMAL; MFRstatus=ACTIVE;
        RECORD(wwrec2=nrecs,"null",1L); xp=(char *)MFX; /* current */
        for (ii=0, i=0; i < argc; i++) {
            if (strncmp(argv_0[i],"cgi",3) != 0) argv[ii++]=argv_0[i];
            else {
                if (strncmp(argv_0[i],"cgitag=",7) == 0 || strncmp(argv_0[i],"cgitag~",7) == 0) {
                    cgitag=(UWORD)atol(argv_0[i]+7); continue;
                }
                if (strncmp(argv_0[i],"cgipfx=",7) == 0 || strncmp(argv_0[i],"cgipfx~",7) == 0) {
                    cgipfxp=argv_0[i]+7; continue;
                }
                if (strncmp(argv_0[i],"cgi=",4) == 0 || strncmp(argv_0[i],"cgi~",4) == 0) {
                    cgispecp=argv_0[i]+4;
                    if (cgitrace) printf("+++ cgispecp=%s.\n",cgispecp);
                    if (strcmp(cgispecp,"null") == 0) continue;
                    if (strcmp(cgispecp,"mx") == 0) {
                        p=NULL;
                        if (!p) {
                            sprintf(xp,"@%s",argv_0[0]);
                            if (strcmp((q=xp+strlen(xp)-4),".EXE") == 0) *q='\0';
                            if (strcmp((q=xp+strlen(xp)-4),".exe") == 0) *q='\0';
                        }
                        cgispecp=strdup(strcat(xp,".pft"));
                        if (cgitrace) printf("+++ cgispecp=mx=(%s)\n",cgispecp);
                    }
                    if (cgitrace) printf("+++ REQUEST_METHOD=%s.\n",getenv("REQUEST_METHOD"));
                    if (cgitrace) printf("+++ QUERY_STRING=%s.\n",getenv("QUERY_STRING"));
                    wwwp=NULL; cicgi0(&ii, argv, &wwwp, cgitag, cgipfxp);
                    if (!wwwp) wwwp="";
                    if (cgitrace) printf("+++ wwwp=%s.\n",wwwp);
                    sprintf(xp,"D%"_LD_,(LONGX)cgitag); fldupdat(wwrec1,xp);
                    p=fldupdat(wwrec1,wwwp); if (p) fatal(p);
                    rc=recfmt(wwrec1,RECnbytes,cgispecp,p=xp,RECnbytes);
                    if (rc < 0) fatal(argv_0[i]/*cgispecp*/);
                    while (*p) {
                        for (ix=0, q=p; *p; p++)
                           if (*p == '\r' || *p == '\n') {
                               *p++='\0'; if (*p == '\n') *p++='\0'; break;
                           } else ix++;
                        if (ix) argv[ii++] = strdup(q);
                    }
                    continue;
                } /* end if */
            } /* end if else */
        } /* end for */
        argc=ii; argv[argc]=NULL;
        if (cgitrace) {
         printf("Content-type: text/html\n\n");
         printf("%s/cgi produced %d parameters:<br>\n",argv[0],argc-1);
         printf("<ul>\n");
         for(ix=1; ix < argc; ix++) printf("<li><pre>%s</pre>\n",argv[ix]);
         printf("</ul>\n");
        }
    }
    else { //some cgii
/*
REQUEST_METHOD=GET
QUERY_STRING=cicgi~mx&db~cds&pft~mfn,x1,v24/&count~2&now&btell~0&bool~plants*water
*/
        for (i=0; i < argc; i++) if (strcmp(argv_0[i],"trace=cgi") == 0) cgitrace=1;
        for (ii=0, i=0; i < argc; i++) if (strcmp(argv_0[i],"trace=cgi")) argv[ii++]=argv_0[i];
        argc=ii;
        if (cicgi0(&argc, argv, NULL, 0, NULL)) {
if (cgitrace) {
 printf("Content-type: text/html%c%c",10,10);
 printf("%s/CICGI produced the following %d parameters:<p>%c",argv[0],argc-1,10);
 printf("<ul>%c",10);
 for(ix=1; ix < argc; ix++) printf("<li> <code>%s</code>%c",argv[ix],10);
 printf("</ul>%c",10);
}
        }
    } //some cgii
#endif /* CICGI */
//CICGI

    argnext=1;
    if (argc > 1)
        if (*argv[1] == '?' || *argv[1] == '=') {
            for (i=0; i < argc; i++)
                printf("argv[%d]: %c%s%c\n",i,OPENFCHR,argv[i],CLOSFCHR);
            MXEXIT(0);
        }

#define WHATISNEW 00
#if WHATISNEW
    if (argc > 1)
        if (strcmp(argv[1],"whatisnew") == 0) {
            printf("summary \n\n");
//CICGI
#if CICGI
            printf("    CGI data        get mx parameter from cgi environment \n\n");
            printf(" set REQUEST_METHOD=GET \n");
            printf(" set QUERY_STRING=db~cds&count~2&now&btell~0&bool~plants*water&pft~mfn/ \n");
            printf(" mx cgi=mx \n");
            printf(" 000004 \n");
            printf(" 000011 \n\n");
#endif /* CICGI */
            printf("    I/F keys        read I/F keys as mx input records \n\n");
            printf(" mx dict=cds k1=africa \"k2=african languages\" now  \n");
            printf(" mfn=     1  \n");
            printf("   1  ®AFRICA^l1^s6^t5^k1¯ \n");
            printf(" mfn=     2  \n");
            printf("   1  ®AFRICAN LANGUAGES^l2^s17^t1^k2¯ \n");
            printf(" \n");
            printf("    I/F postings    read I/F keys and postings as mx input records \n\n");
            printf(" mx dict=cds,1,2/3 k1=africa count=2 now  \n");
            printf(" mfn=     1  \n");
            printf("   1  ®AFRICA^l1^s6^t5^k1¯ \n");
            printf("   2  ®^m97^t69^o1^c9^p1^k1¯ \n");
            printf("   2  ®^m115^t24^o1^c3^p2^k1¯ \n");
            printf("   2  ®^m115^t69^o1^c3^p3^k1¯ \n");
            printf(" mfn=     2  \n");
            printf("   1  ®AFRICA^l1^s6^t5^k1¯ \n");
            printf("   2  ®^m119^t69^o1^c7^p4^k1¯ \n");
            printf("   2  ®^m125^t69^o1^c4^p5^k1¯ \n");
            printf(" \n");
            printf("    field add       field add w/ markup strip \n\n");
            printf(" mx null \"proc='<10>x<mark>y</mark>z</10>'\" \n");
/*          printf(" mfn=     1 \n"); */
            printf("  10  ®xyz¯ \n\n");
            printf("    procedures      up to %d proc= parameters, including fst, read, write, I/F update \n\n",MAXNPPP);
            printf(" mx cds \"proc='T99 2 (v69/)'\" \n");
/*          printf(" mfn=     1 \n"); */
            printf("  69  ®Paper on: <plant physiology><plant transpiration><measurement and instruments>¯ \n");
            printf("  99  ®PLANT PHYSIOLOGY^m1^o1^c1^l2¯ \n");
            printf("  99  ®PLANT TRANSPIRATION^m1^o1^c2^l2¯ \n");
            printf("  99  ®MEASUREMENT AND INSTRUMENTS^m1^o1^c3^l2¯ \n\n");
            printf(" mx null proc='Rcds,1' \n");
            printf(" mfn=     1 \n");
            printf("  44  ®Methodology of plant eco-physiology: proceedings of the Montpellier Sympoium¯ \n");
            printf("  50  ®Incl. bibl.¯ \n");
            printf("  69  ®Paper on: <plant physiology><plant transpiration><measurement and instrumnts>¯ \n");
            printf("  24  ®Techniques for the measurement of transpiration of individual plants¯ \n");
            printf("  26  ®^aParis^bUnesco^c-1965¯ \n");
            printf("  30  ®^ap. 211-224^billus.¯ \n");
            printf("  70  ®Magalhaes, A.C.¯ \n");
            printf("  70  ®Franco, C.M.¯ \n\n");
            printf(" mx null proc='a10/1/' proc='Xdbn1' \n");
            printf(" mx dbn1 \n");
            printf(" mfn= 1 \n");
            printf(" 10 «1» \n\n");
            printf(" del lilacs_year_*.mst \n");
            printf(" del lilacs_year_*.xrf \n");
            printf(" mx lilacs proc='Xappend=lilacs_year_',v64.4 \n\n");
            printf(" mx null proc='a1/x/' to=3 \"proc='Yy,,,1 0 v1'\" \"pft='npost='f(npost(['y']'x'),1,0)/\" \n");
            printf(" npost=1 \n");
            printf(" npost=2 \n");
            printf(" npost=3 \n\n");
            printf(" mx cds -all now proc='Xx' proc='Zx' \n");
            printf(" ifupd x fst=@cds.fst tell=1 \n");
            printf(" 0 records updated \n");
            printf(" 0+0=0 postings added \n");
            printf(" 0+0=0 postings deleted \n\n");
            printf(" del lilacs_year_*.12 \n");
            printf(" mx lilacs proc='Fappend=lilacs_year_',v64.4,'.12',v1'-'v2/ \n\n");
#if MORE1
            printf(" mx null proc='a10/1/' create=x \n");
            printf(" mfn=     1 \n");
            printf("  10  ®1¯ \n");
            printf(" mx null proc='a20/2/' append=x \n");
/*            printf(" mfn=     1 \n"); */
            printf("  20  ®2¯ \n");
            printf(" mx null proc='Rx,1' proc='Rx,2' \n");
/*            printf(" mfn=     1 \n"); */
            printf("  10  ®1¯ \n");
            printf("  20  ®2¯ \n\n");

            printf(" mx null proc='a10/1/' proc='Xdbn1' proc='a20/2/' proc='Xdbn2' \n");
            printf(" mx dbn1 \n");
            printf(" mfn=     1 \n");
            printf("  10  ®1¯ \n");
            printf(" mx dbn2 \n");
            printf(" mfn=     1 \n");
            printf("  10  ®1¯ \n");
            printf("  20  ®2¯ \n\n");
            printf(" @echo 'd*a32701~',,mstname,,'^m',,f(mfn,8,0),,'~',, >p0 \n");
            printf(" @echo '=',,ref(['mstxrf']1,f(maxmfn,1,0)) >p1 \n");
            printf(" @echo if l(['mstxrf']v32701)=0 then 'Xmstxrf' fi >p2 \n");
            printf(" @echo if l(['mstxrf']v32701)=0 then 'Ymstxrf,mstxrf,32701 0 v32701' fi >p3 \n");
            printf(" mx full x -all now proc=@p0 proc=@p1 proc=@p2 proc=@p3 \n\n");
#endif /* MORE1 */
            /* printf("    cipar           ci_tempdir, maxmfrl, ci_fststrip \n\n"); */
            printf("    cipar           set directory for temporary files \n\n");
            printf(" echo ci_tempdir=c:\\work >xcip \n");
            printf(" mx cipar=xcip cdromdb \n\n");
            printf("    cipar           set max master file record length \n\n");
            printf(" @echo maxmfrl=32000 >xcip \n");
            printf(" mx cipar=xcip bigrecs \n\n");
#define CIFS 1
#if CIFS
            printf("    cipar           strip <markup> marks during fst \n\n");
            printf(" set echo ci_fststrip=32000 >xcip \n");
            printf(" mx cds \n");
            printf("  69  ®Paper on: <plant physiology><plant transpiration><measurement and instruments>¯ \n");
            printf(" mx cipar=xcip cds \"fst=690 4 v69\" \n");
            printf("  69  ®Paper on:                                                                     ¯ \n");
            printf(" 690  ®PAPER^m1^o1^c1^l1¯ \n");
            printf(" 690  ®ON^m1^o1^c2^l1¯ \n\n");
#endif /* CIFS */
            printf("    fst             indexing technique to extract text within <mark> and </mark> \n\n");
            printf(" mx null \"fst=1 2000 '#<B>#','text <B>text1</B> text <B>text2</B> text'\" \n");
            printf("   1  ®TEXT1TEXT2^m1^o1^c1^l1¯ \n\n");
            printf("    format          read a stdin input line as a new field occurrence \n\n");
            printf(" mx null to=2 \"pft=if mfn=1 then _10,putenv('TAG10='v10) fi,,mfn,x1,getenv('TAG10'),/ \n");
            printf(" input line \n");
            printf(" 000001 input line \n");
            printf(" 000002 input line \n\n");
            printf("    format          get the contents of a specified cgi variable \n\n");
            printf(" set REQUEST_METHOD=GET \n");
            printf(" set QUERY_STRING=db~cds&bool~plants*water \n");
            printf(" mx null pft=getenv('cgi=db') \n");
            printf(" cds \n\n");
            printf("    format          get a temporary empty file name \n\n");
            printf(" mx null pft=getenv('tmp=') \n");
            printf(" TMP1.$$$ \n\n");
#if DOS32BITS
            printf("    platform        32-bit memory support \n\n");
            printf(" mx null count=1 sys='mx what' \n");
            printf(" CISIS Interface v4.01/fmtold/PC32/M/30000/10/30/I - Utility MX \n");
            printf(" CISIS Interface v4.01/fmtold/.iy0/Z/4GB/GIZ/DEC/ISI/UTL/FAT/CIP/CGI \n");
            printf(" Copyright (c)BIREME/PAHO 2000. All rights reserved. \n\n");
#endif /* DOS32BITS */
#if CICGI
            printf("[cgitag=<tag>] [cgipfx=<pfx>] cgi={<fmt>|mx}^uy^hget mx calling parameters from CGI^xx \n\n");
            printf("    cgitag=<tag>    tag for ^n^v pairs [%"_LD_"]\n",cgitag);
            printf("    cgipfx=<pfx>    tag naming prefix [%s] \n",cgipfxp);
            printf("    cgi=<fmt>       specify mx parameters via <fmt> \n");
#if 0
            printf("    cgi=mx          specify mx parameters via "); /* built-in specification */
for (xp=argv_0[0], ix=(strcmp(xp+strlen(xp)-4,".EXE") == 0)?strlen(xp)-4:strlen(xp); ix--; xp++) printf("%c",*xp);
printf(".pft \n\n");
#else
            printf("    cgi=mx          specify mx parameters by corresponding names \n\n"); /* built-in specification */
#endif
            printf("    store CGI name/value pairs as repeatable field %"_LD_" \n",cgitag);
            printf("    and take each line produced by format <fmt> (or mx.pft) as an additional mx parameter \n\n");
            printf(" set REQUEST_METHOD=GET \n");
            printf(" set QUERY_STRING=db~cds&count~2&now&btell~0&bool~plants*water&pft~mfn/ \n");
            printf(" mx cgi=mx \n");
            printf(" 000004 \n");
            printf(" 000011 \n\n");
            printf(" <html> \n");
            printf(" <body> \n");
            printf(" <form action=\"/cgi-bin/mx.exe/cgi=mx\" method=\"post\"> \n");
            printf(" Run MX with 'bool=' and 'count=' parameters assigned to the following values:<hr> \n");
            printf(" Search expression:    <input type=\"text\" name=\"bool\"  value=\"$\"><br> \n");
            printf(" Max #records to show: <input type=\"text\" name=\"count\" value=\"10\"><br> \n");
            printf(" <input type=\"submit\" value=\"search & display\"> \n");
            printf(" <input type=\"hidden\" name=\"db\" value=\"D:\\Users\\www\\isis\\data\\cds\"> \n");
            printf(" <input type=\"hidden\" name=\"pft\" value=\"lw(8000),newline('<br>'),@D:\\Users\\www\\isis\\data\\cds.pft\"> \n");
            printf(" <input type=\"hidden\" name=\"now\"> \n");
            printf(" <input type=\"hidden\" name=\"btell\" value=\"0\"> \n");
            printf(" </form> \n");
            printf(" </body> \n");
            printf(" </html> \n\n");
#endif /* CICGI */
            printf("dict=<ifn>[,<ktag>[,<ptag>[/<pmax>]]] [k{1|2}=<key>]^hread I/F keys as mx input records^xx \n\n");
            printf("    <ifn>           I/F dictionary \n");
            printf("    <ktag>          tag for I/F key [1] \n");
            printf("    <ptag>          tag for I/F posting \n");
            printf("    <pmax>          max postings per record [all] \n");
            printf("    k1=<key>        initial key \n");
            printf("    k2=<key>        final key \n\n");
            printf("    read I/F keys from <ifn> as mx input records with field <ktag> \n");
            printf("    with key and ^l=lex ^s=keylength ^t=totalpostings ^k=keyorder \n\n");
            printf("    if <ptag> is specified, combine I/F keys and corresponding postings \n");
            printf("    as mx input records having up to <pmax> postings each; \n");
            printf("    <ptag> has the form ^m=mfn ^t=tag ^o=occ ^c=cnt ^ppstorder ^kkeyorder \n\n");
            printf(" mx dict=cds k1=africa \"k2=african languages\" now  \n");
            printf(" mfn=     1  \n");
            printf("   1  ®AFRICA^l1^s6^t5^k1¯ \n");
            printf(" mfn=     2  \n");
            printf("   1  ®AFRICAN LANGUAGES^l2^s17^t1^k2¯ \n\n");
            printf(" mx dict=cds,1,2 k1=africa count=1 now  \n");
            printf(" mfn=     1  \n");
            printf("   1  ®AFRICA^l1^s6^t5^k1¯ \n");
            printf("   2  ®^m97^t69^o1^c9^p1^k1¯ \n");
            printf("   2  ®^m115^t24^o1^c3^p2^k1¯ \n");
            printf("   2  ®^m115^t69^o1^c3^p3^k1¯ \n");
            printf("   2  ®^m119^t69^o1^c7^p4^k1¯ \n");
            printf("   2  ®^m125^t69^o1^c4^p5^k1¯ \n\n");
            printf(" mx dict=cds,1,2/3 k1=africa count=2 now  \n");
            printf(" mfn=     1  \n");
            printf("   1  ®AFRICA^l1^s6^t5^k1¯ \n");
            printf("   2  ®^m97^t69^o1^c9^p1^k1¯ \n");
            printf("   2  ®^m115^t24^o1^c3^p2^k1¯ \n");
            printf("   2  ®^m115^t69^o1^c3^p3^k1¯ \n");
            printf(" mfn=     2  \n");
            printf("   1  ®AFRICA^l1^s6^t5^k1¯ \n");
            printf("   2  ®^m119^t69^o1^c7^p4^k1¯ \n");
            printf("   2  ®^m125^t69^o1^c4^p5^k1¯ \n\n");
            printf(" mx dict=cds,1,2/1 k1=africa count=5 now  \n");
            printf(" mfn=     1  \n");
            printf("   1  ®AFRICA^l1^s6^t5^k1¯ \n");
            printf("   2  ®^m97^t69^o1^c9^p1^k1¯ \n");
            printf(" mfn=     2  \n");
            printf("   1  ®AFRICA^l1^s6^t5^k1¯ \n");
            printf("   2  ®^m115^t24^o1^c3^p2^k1¯ \n");
            printf(" mfn=     3  \n");
            printf("   1  ®AFRICA^l1^s6^t5^k1¯ \n");
            printf("   2  ®^m115^t69^o1^c3^p3^k1¯ \n");
            printf(" mfn=     4  \n");
            printf("   1  ®AFRICA^l1^s6^t5^k1¯ \n");
            printf("   2  ®^m119^t69^o1^c7^p4^k1¯ \n");
            printf(" mfn=     5  \n");
            printf("   1  ®AFRICA^l1^s6^t5^k1¯ \n");
            printf("   2  ®^m125^t69^o1^c4^p5^k1¯ \n\n");
#if TBISNEW
            printf("tb=<tag>^hsave data fields as individual files^xx \n\n");
            printf("    <tag>           field tag \n\n");
            printf("    save the contents of field <tag> into binary file MFRmfn.<tag> \n");
            printf("    if <tag> is zero save whole data segment into file MFRmfn.dat \n\n");
            printf(" mx cds tb=30 tb=70 \"pft=mfn,x2,v30,x2,v70+|; |/\" \n");
            printf(" 000001  ^ap. 211-224^billus.  Magalhaes, A.C.; Franco, C.M. \n");
            printf(" /****************************************************** \n");
            printf(" 1.30 \n");
            printf(" ******************************************************/ \n");
            printf(" ^ap. 211-224^billus. \n");
            printf(" /****************************************************** \n");
            printf(" 1.70 \n");
            printf(" ******************************************************/ \n");
            printf(" Magalhaes, A.C.Franco, C.M. \n");
#endif /* TB */
            printf("proc=<<tag>[ <marklen>[ <fldlen>]]><text></<tag>>^uy^hproc=< to add text between <marks>^xx \n\n");
            printf("    <tag>           field tag \n");
            printf("    <marklen>       max mark length [<text> length] \n");
            printf("    <fldlen>        min resulting field length to add a new occ of field <tag> [0] \n\n");
            printf("    add <text> as a new occurrence of field <tag> \n");
            printf("    <marks> up to <marklen> chars are stripped from <text> \n");
            printf("    do not add a new occurrence of field <tag> if the resulting text is shorter than <fldlen> chars \n\n");
#if MELHOR
ver se altercao acima nao estorou linha do browser
C:\cid>mx null count=1 "proc='<10>x<mark>y</mark>z</10>'"
mfn=     1
 10  «xyz»

C:\cid>mx null count=1 "proc='<10 0>x<mark>y</mark>z</10>'"
mfn=     1
 10  «x<mark>y</mark>z»

C:\cid>mx null count=1 "proc='<10 5>x<mark>y</mark>z</10>'"
mfn=     1
 10  «x<mark>y</mark>z»

C:\cid>mx null count=1 "proc='<10 6>x<mark>y</mark>z</10>'"
mfn=     1
 10  «xy</mark>z»

C:\cid>mx null count=1 "proc='<10 7>x<mark>y</mark>z</10>'"
mfn=     1
 10  «xyz»
#endif
            printf(" mx null \"proc='<10>text</10>'\" \n");
            printf(" mfn=     1 \n");
            printf("  10  ®text¯ \n\n");
            printf(" mx null \"proc='<10>x<mark>y</mark>z</10>'\" \n");
            printf(" mfn=     1 \n");
            printf("  10  ®xyz¯ \n\n");
            printf(" mx null \"proc='<10 99 3>x<mark>y</mark>z</10>'\" \n");
            printf(" mfn=     1 \n");
            printf("  10  ®xyz¯ \n");
            printf(" mx null \"proc='<10 99 4>x<mark>y</mark>z</10>'\" \n");
            printf(" mfn=     1 \n\n");
#if GENPROCT
            printf("proc=T[/h=]<fst>^hproc=T to add fst results^xx \n\n");
            printf("    <fst>           field select table specification \n\n");
            printf("    apply <fst> to MFR adding generated links as data fields \n");
            printf("    command T/h=<fst> stands for full-length key extraction \n\n");
            printf(" mx cds \"proc='T99 0 (v70/)'\" \n");
            printf(" mfn=     1 \n");
            printf("  70  ®Magalhaes, A.C.¯ \n");
            printf("  70  ®Franco, C.M.¯ \n");
            printf("  99  ®MAGALHAES, A.C.^m1^o1^c1^l2¯ \n");
            printf("  99  ®FRANCO, C.M.^m1^o1^c2^l2¯ \n\n");
            printf(" mx cds \"proc='T/h=0 0 (v70/)'\" \n");
            printf(" mfn=     1 \n");
            printf(" 32767  ®Magalhaes, A.C. \n");
            printf(" Franco, C.M. \n");
            printf(" ¯ \n\n");
            printf(" echo PLANT PHYSIOLOGY>x \n");
            printf(" echo PLANT TRANSPIRATION>>x \n");
            printf(" mx seq=x -all now \"fst=1 0 v1\" ifupd/create=thes \n");
/*            printf(" mx cds \"proc='T99 2 (v69/)'\" \"proc=(if l(['thes']v99^*)=0 then 'a999|'v99^*'|' fi)\" \"pft=(if p(v999) then 'not in thes: 'v999/ fi)\" \n");
*/
            printf(" mx cds \"proc='T99 2 (v69/)'\" \"pft=(if l(['thes']v99^*)=0 then 'not in thes: 'v99^*/ fi)\" \n");
            printf(" mfn=     1 \n");
            printf("  69  ®Paper on: <plant physiology><plant transpiration><measurement and instruments>¯ \n");
            printf("  99  ®PLANT PHYSIOLOGY^m1^o1^c1^l2¯ \n");
            printf("  99  ®PLANT TRANSPIRATION^m1^o1^c2^l2¯ \n");
            printf("  99  ®MEASUREMENT AND INSTRUMENTS^m1^o1^c3^l2¯ \n");
            printf(" not it thes: MEASUREMENT AND INSTRUMENTS \n\n");
#endif /* GENPROCT */
            printf("proc=R<dbn>,<mfn>^hproc=R to add data of another record^xx \n\n");
            printf("    <dbn>           input master file name \n");
            printf("    <mfn>           master file record number \n\n");
            printf("    add to MFR all data fields of active record <dbn>,<mfn> \n\n");
            printf(" mx null proc='a10/1/a20/1/' create=x \n");
/*            printf(" mfn=     1 \n"); */
            printf("  10  ®1¯ \n");
            printf("  20  ®1¯ \n");
            printf(" mx null proc='a20/2/' append=x \n");
/*            printf(" mfn=     1 \n"); */
            printf("  20  ®2¯ \n");
            printf(" mx null proc='Rx,1' proc='Rx,2' \n");
            printf(" mfn=     1 \n");
            printf("  10  ®1¯ \n");
            printf("  20  ®1¯ \n");
            printf("  20  ®2¯ \n\n");
            printf("proc=X[{create|copy|append|merge}=]<out_dbn>^hproc=X to write an output record^xx \n\n");
            printf("    <out_dbn>       output master file name \n\n");
            printf("    create <out_dbn> if does not exist \n");
            printf("    write MFR to <out_dbn> as record MFRmfn or \n");
            printf("    append MFR to <out_dbn> or \n");
            printf("    merge MFR to <out_dbn> \n\n");
            printf(" mx null proc='a10/1/' proc='Xdbn1' proc='a20/2/' proc='=3' proc='Xdbn2' \n");
            printf(" mx dbn1 \n");
            printf(" mfn=     1 \n");
            printf("  10  ®1¯ \n");
            printf(" mx dbn2 \n");
            printf(" mfn=     3 \n");
            printf("  10  ®1¯ \n");
            printf("  20  ®2¯ \n\n");
            printf(" echo replace(replace(s(v70[1].8),',','-'),' ','+') >xfile.pft \n");
            printf(" mx cds proc='Xappend='@xfile.pft, pft=mfn,x1,v70[1]/ \n");
            printf(" 000001 Magalhaes, A.C. \n");
            printf(" 000002 Bosian, G. \n");
            printf(" 000003 Bosian, G. \n");
            printf(" mx Magalhae pft=mfn,x1,v70[1]/ \n");
            printf(" 000001 Magalhaes, A.C. \n");
            printf(" mx Bosian-+ pft=mfn,x1,v70[1]/ \n");
            printf(" 000002 Bosian, G. \n");
            printf(" 000003 Bosian, G. \n\n");
            printf("proc=Y[create=]<out_ifn>,[<dbn>],[<mfn>],<fst>^hproc=Y to update an I/F^xx \n\n");
            printf("    <out_ifn>       output inverted file name \n");
            printf("    <dbn>           master file name, or 'null' \n");
            printf("    <mfn>           master file record number \n");
            printf("    <fst>           field select table specification \n\n");
            printf("    if <dbn> is 'null' or omitted update <out_ifn> applying <fst> to MFR \n");
            printf("    assign <mfn> [MFRmfn] to components TRMpmfn of generated postings \n");
            printf("    otherwise update <out_ifn> applying <fst> to record <mfn> [MFRmfn] of <dbn> \n");
            printf("    does not reset any IFupdPending flag \n\n");
/*          printf("    reset corresponding flag IFupdPending if exist <out_ifn>.mst \n\n"); */
            printf(" @echo Procedure to create a centralized index for a set of data bases \n");
            printf(" @echo Setup sample data base MF1, MF1.fst and MF1.pft.. \n");
            printf(" @echo 100 0 v1/,v2/,v1'/'v2/ >MF1.fst \n");
            printf(" @echo '[MF1] 'mfn/,' 1='v1/,' 2='v2/ >MF1.pft \n");
            printf(" @mx null count=1 \"proc='a1~master 1~a2~registro 1~'\" create=MF1 pft=@MF1.pft \n");
            printf(" @mx null count=1 \"proc='a1~master 1~a2~registro 2~'\" append=MF1 pft=@MF1.pft \n");
            printf(" @echo Setup sample data base MF2, MF2.fst and MF2.pft.. \n");
            printf(" @echo 200 0 v10/,v20/,v10'/'v20/ >MF2.fst \n");
            printf(" @echo '[MF2] 'mfn/,' 10='v10/,' 20='v20/ >MF2.pft \n");
            printf(" @mx null count=1 \"proc='a10~master 2~a20~registro 1~'\" create=MF2 pft=@MF2.pft \n");
            printf(" @echo And init centralized record number M/F and all indexes I/F.. \n");
            printf(" mx null count=0 create=mstxrf ifupd/create=mstxrf \n");
            printf(" @pause \n");
            printf(" @echo Load MF1 and MF2 into same mstxrf and index them.. \n");
            printf(" @rem setup centralized record numbering procs \n");
            printf(" @echo 'd*a32701~',mstname'^m'f(mfn,8,0)'~' >p1 \n");
            printf(" @echo if l(['mstxrf']v32701)=0 then '='ref(['mstxrf']1,f(maxmfn,1,0)),,'X','mstxrf' fi >p2a \n");
            printf(" @echo if l(['mstxrf']v32701)=0 then 'Y','mstxrf,null,,32701 0 v32701' fi >p2b \n");
            printf(" @echo 'Z',v32701^*','f(val(v32701^m),1,0) >p2c \n");
            printf(" @rem setup centralized index update procs \n");
            printf(" @echo 'R',v32701^*','f(val(v32701^m),1,0) >p3a \n");
            printf(" @echo 'Y','mstxrf,null,'f(l(['mstxrf']v32701),1,0)',@'v32701^*'.fst' >p3b \n");
            printf(" @echo 'Z',v32701^*','f(val(v32701^m),1,0) >p3c \n");
            printf(" @rem fldupdat() commands 'R', 'X', 'Y' and 'Z' may occur just once per 'proc=' or 'proc()'^iy^xx \n");
            printf(" @rem otherwise two or more of these procs could be put together in a single 'proc=' file^iy^xx \n");
            printf(" @rem p1 \n");
            printf(" @rem 'd*a32701~',mstname'^m'f(mfn,8,0)'~' \n");
            printf(" @rem p2a \n");
            printf(" @rem if l(['mstxrf']v32701)=0 then '='ref(['mstxrf']1,f(maxmfn,1,0)),,'X','mstxrf' fi \n");
            printf(" @rem p2b \n");
            printf(" @rem if l(['mstxrf']v32701)=0 then 'Y','mstxrf,null,,32701 0 v32701' fi \n");
            printf(" @rem p2c \n");
            printf(" @rem 'Z',v32701^*','f(val(v32701^m),1,0) \n");
            printf(" @rem p3a \n");
            printf(" @rem 'R',v32701^*','f(val(v32701^m),1,0) \n");
            printf(" @rem p3b \n");
            printf(" @rem 'Y','mstxrf,null,'f(l(['mstxrf']v32701),1,0)',@'v32701^*'.fst' \n");
            printf(" @rem p3c \n");
            printf(" @rem 'Z',v32701^*','f(val(v32701^m),1,0) \n");
            printf(" @rem \n");
            printf(" mx %1 MF1 now proc=@p1 proc=@p2a proc=@p2b proc=@p2c proc=@p3a proc=@p3b proc=@p3c \n");
            printf(" @rem mfn=     1 \n");
            printf(" @rem 32701  ®MF1^m       1¯ \n");
            printf(" @rem   1  ®master 1¯ \n");
            printf(" @rem   2  ®registro 1¯ \n");
            printf(" @rem mfn=     2 \n");
            printf(" @rem 32701  ®MF1^m       1¯ \n");
            printf(" @rem   1  ®master 1¯ \n");
            printf(" @rem   2  ®registro 2¯ \n");
            printf(" @rem \n");
            printf(" mx %1 MF2 now proc=@p1 proc=@p2a proc=@p2b proc=@p2c proc=@p3a proc=@p3b proc=@p3c \n");
            printf(" @rem mfn=     3 \n");
            printf(" @rem 32701  ®MF2^m       1¯ \n");
            printf(" @rem  10  ®master 2¯ \n");
            printf(" @rem  20  ®registro 1¯ \n");
            printf(" @rem \n");
            printf(" @pause \n");
            printf(" @echo Create default display format for centralized record number M/F.. \n");
            printf(" @rem setup header and read original record \n");
            printf(" @echo '[mstxrf 'mfn'] ',v32701/, >mstxrf.pft \n");
            printf(" @echo proc('R'v32701^*,',',v32701^m), >>mstxrf.pft \n");
            printf(" @echo proc('='f(val(v32701^m),1,0)), >>mstxrf.pft \n");
            printf(" @rem setup original display format selection \n");
            printf(" @echo if v32701^*='MF1' then @MF1.pft, fi, >>mstxrf.pft \n");
            printf(" @echo if v32701^*='MF2' then @MF2.pft, fi, >>mstxrf.pft \n");
            printf(" @echo And test it.. \n");
            printf(" mx mstxrf \"bool=registro 1\" pft=@ now \n");
            printf(" @rem \n");
            printf(" @rem mx mstxrf \"bool=registro 1\" pft=@ now \n");
            printf(" @rem       2       2  REGISTRO 1 \n");
            printf(" @rem       2       2  Set #000001 \n");
            printf(" @rem Hits=2 \n");
            printf(" @rem [mstxrf 000001] MF1^m       1 \n");
            printf(" @rem [MF1] 000001 \n");
            printf(" @rem  1=master 1 \n");
            printf(" @rem  2=registro 1 \n");
            printf(" @rem 100= \n");
            printf(" @rem [mstxrf 000003] MF2^m       1 \n");
            printf(" @rem [MF2] 000001 \n");
            printf(" @rem  10=master 2 \n");
            printf(" @rem  20=registro 1 \n");
            printf(" @rem 200= \n");
            printf(" @rem \n");
            printf(" type mstxrf.pft \n");
            printf(" @rem '[mstxrf 'mfn'] ',v32701/, \n");
            printf(" @rem proc('R'v32701^*,',',v32701^m), \n");
            printf(" @rem proc('='f(val(v32701^m),1,0)), \n");
            printf(" @rem if v32701^*='MF1' then @MF1.pft, fi, \n");
            printf(" @rem if v32701^*='MF2' then @MF2.pft, fi, \n");
            printf(" @rem \n");
            printf(" @rem mz mstxrf -control now \n");
            printf(" @rem  + MASTER 1  : 1/9/0 \n");
            printf(" @rem    #1/2: mfn=1 tag=100 occ=1 cnt=1 \n");
            printf(" @rem    #2/2: mfn=2 tag=100 occ=1 cnt=1 \n");
            printf(" @rem  + MASTER 1/REGISTRO 1           : 1/23/0 \n");
            printf(" @rem    #1/1: mfn=1 tag=100 occ=1 cnt=3 \n");
            printf(" @rem  + MASTER 1/REGISTRO 2           : 1/51/0 \n");
            printf(" @rem    #1/1: mfn=2 tag=100 occ=1 cnt=3 \n");
            printf(" @rem  + MASTER 2  : 1/65/0 \n");
            printf(" @rem    #1/1: mfn=3 tag=200 occ=1 cnt=1 \n");
            printf(" @rem  + MASTER 2/REGISTRO 1           : 1/79/0 \n");
            printf(" @rem    #1/1: mfn=3 tag=200 occ=1 cnt=3 \n");
            printf(" @rem  + MF1^M       1                 : 1/2/0 \n");
            printf(" @rem    #1/1: mfn=1 tag=32701 occ=1 cnt=1 \n");
            printf(" @rem  + MF1^M       2                 : 1/30/0 \n");
            printf(" @rem    #1/1: mfn=2 tag=32701 occ=1 cnt=1 \n");
            printf(" @rem  + MF2^M       1                 : 1/58/0 \n");
            printf(" @rem    #1/1: mfn=3 tag=32701 occ=1 cnt=1 \n");
            printf(" @rem  + REGISTRO 1: 1/16/0 \n");
            printf(" @rem    #1/2: mfn=1 tag=100 occ=1 cnt=2 \n");
            printf(" @rem    #2/2: mfn=3 tag=200 occ=1 cnt=2 \n");
            printf(" @rem  + REGISTRO 2: 1/44/0 \n");
            printf(" @rem    #1/1: mfn=2 tag=100 occ=1 cnt=2 \n");
            printf(" @rem \n\n");
            printf("proc=Z<out_dbn>[,<mfn>]^hproc=Z to reset IFupdPending^xx \n\n");
            printf("    <out_dbn>       master file name \n");
            printf("    <mfn>           master file record number \n\n");
            printf("    reset flag IFupdPending corresponding to <mfn> [MFRmfn] from <out_dbn> \n\n");
            printf(" mx cds -all now proc='Xx' proc='Zx' \n");
            printf(" ifupd x fst=@cds.fst tell=1 \n");
            printf(" 0 records updated \n");
            printf(" 0+0=0 postings added \n");
            printf(" 0+0=0 postings deleted \n\n");
            printf("proc=F[{create|append}=]<out_file>,<text>^hproc=F to write a text to a file^xx \n\n");
            printf("    <out_file>      output file name \n");
            printf("    <text>          null terminated text \n\n");
            printf("    create <out_file> if does not exist \n");
            printf("    append <text> to <out_file> \n\n");
            printf(" echo replace(replace(s(v70[1].8),',','-'),' ','+') >xfile.pft \n");
            printf(" mx cds proc='Fappend='@xfile.pft,',',v24.68,'.' pft=mfn,x1,v70[1]/ \n");
            printf(" 000001 Magalhaes, A.C. \n");
            printf(" 000002 Bosian, G. \n");
            printf(" /****************************************************** \n");
            printf(" Magalhae \n");
            printf(" ******************************************************/ \n");
            printf(" Techniques for the measurement of transpiration of individual plants. \n");
            printf(" /****************************************************** \n");
            printf(" Bosian-+ \n");
            printf(" ******************************************************/ \n");
            printf(" <The> Controlled climate in the plant chamber and its influence upon. \n\n");
            printf("cipar: ci_tempdir=<path>^uy^cy^hcipar/ci_tempdir: set directory for temporary files^xx \n\n");
            printf("    <path>          drive and/or directory name \n\n");
            printf("    defined in cipar or environment \n");
            printf("    create temporary files in directory <path> \n");
            printf("    (if ci_tempdir is not set temporary files are created in directory \n");
            printf("    defined by the environment variable TEMP or by TMP) \n\n");
            printf(" echo ci_tempdir=c:\\work >xcip \n");
            printf(" mx cipar=xcip cdromdb \n\n");
            printf(" @set ci_tempdir=c:\\work \n");
            printf(" mx cdromdb \n\n");
            printf("cipar: maxmfrl=<nbytes>^hcipar/maxmfrl: set max master file record length^xx \n\n");
            printf("    <nbytes>        number of bytes \n\n");
            printf("    defined in cipar \n");
            printf("    specify the default MFRmfrl (master file record length) [%"_LD_"]\n",rec_maxmfrl);
#if CICPP
            printf("    (the maximum MFRmfrl is %"_LD_", however the configured maximum value for <nbytes> is %"_LD_") \n\n",(LONGX)MFRL_MAX,(LONGX)(ALLOMAXV-RECHSIZE));
#else
            printf("    (the maximum MFRmfrl is %"_LD_", however the configured maximum value for <nbytes> is %"_LD_") \n\n",(LONGX)MFRL_MAX,(LONGX)ALLOMAXV);
#endif
            printf(" echo maxmfrl=32767 >xcip \n");
            printf(" mx cipar=xcip bigrecsdb \n\n");
#if CIFS
            printf("cipar: ci_fststrip=<maxlen>^hcipar/ci_fststrip: strip <markup> marks during fst^xx \n\n");
/*          printf("cipar: ci_fststrip=<maxlen>[,<tags>] \n\n"); */
            printf("    <maxlen>        max mark length \n\n");
/*          printf("    <tags>          tag or comma separated tag list \n\n"); */
            printf("    defined in cipar or environment \n");
            printf("    strip any marks as <text> or </text> up to <maxlen> chars from all data fields \n");
            printf("    at the beggining of a FST execution (data fields are padded with blanks) \n\n");
            printf(" echo ci_fststrip=21 >xcip\n");
            printf(" mx cds \n");
            printf(" mfn=     1 \n");
            printf("  69  ®Paper on: <plant physiology><plant transpiration><measurement and instruments>¯ \n");
            printf(" mx cipar=xcip cds \"fst=690 4 v69\" \n");
            printf(" mfn=     1 \n");
            printf("  69  ®Paper on: <measurement and instruments>                                       ¯ \n");
            printf(" 690  ®PAPER^m1^o1^c1^l1¯ \n");
            printf(" 690  ®ON^m1^o1^c2^l1¯ \n");
            printf(" 690  ®MEASUREMENT^m1^o1^c3^l2¯ \n");
            printf(" 690  ®AND^m1^o1^c4^l1¯ \n");
            printf(" 690  ®INSTRUMENTS^m1^o1^c5^l2¯ \n");
#endif /* CIFS */
            printf("fst=<id> 2000+<it> <fmt>^uy^hfst/IT 2000 to extract text within <mark> and </mark>^xx \n\n");
            printf("    <id>            tag component \n");
            printf("    <it>            indexing technique 2000+IT (IT=0..8, 1000..1008) \n");
            printf("    <fmt>           format specification generating <dlm><mark><dlm><mark><text></mark> \n\n");
            printf("    apply indexing technique IT to text embedded in marks <mark> and </mark> \n\n");
            printf(" mx null \"fst=2 2004 '#<B>#','xxx <B>word1 word2</B> xxx'\" \n");
            printf(" mfn=     1 \n");
            printf("   2  ®WORD^m1^o1^c1^l1¯ \n");
            printf("   2  ®WORD^m1^o1^c2^l1¯ \n\n");
            printf("pft=_<tag>^uy^hpft=_ to input data from stdin^xx \n\n");
            printf("    <tag>           field tag \n\n");
            printf("    read a stdin input line and add the input data as a new occurrence of field <tag> \n\n");
            printf(" mx null \"proc=if mfn=1 then _10,putenv('TAG10='v10) fi\" pft=mfn,x1,getenv('TAG10'),/ to=2 \n");
            printf(" input line \n");
            printf(" 000001 input line \n");
            printf(" 000002 input line \n\n");
            printf("pft=getenv('cgi=',<varfmt>)^hpft=getenv('cgi='var) to get CGI data^xx \n\n");
            printf("    <varfmt>        format specification generating a cgi variable name \n\n");
            printf("    output the contents of the specified cgi variable \n");
            printf("    multiple occurrences are separeted by a %c char \n\n",'%');
            printf(" set REQUEST_METHOD=GET \n");
            printf(" set QUERY_STRING=db~cds&bool~plants*water&bool~another \n");
            printf(" mx null pft=getenv('cgi=db')/getenv('cgi=bool')/ \n");
            printf(" cds \n");
            printf(" plants*water%another \n\n");
            printf("pft=getenv('tmp=',[<pathfmt>])^hpft=getenv('tmp=') to get a temporary file name^xx \n\n");
            printf("    <pathfmt>       format specification generating a path name \n\n");
            printf("    get a temporary empty file name in the current working directory or in the specified path \n");
            printf("    if 'ci_tempdir' is specified then path is obtained from the following environment variables: ci_tempdir, temp, tmp \n\n");
            printf(" mx null pft=getenv('tmp=') \n");
            printf(" TMP1.$$$ \n\n");
            printf(" mx null pft=getenv('tmp=','ci_tempdir') \n");
            printf(" C:\\windows\\TEMP\\TMP1.$$$ \n\n");
            printf(" set ci_tempdir=C:\\work \n");
            printf(" mx null pft=getenv('tmp=','ci_tempdir') \n");
            printf(" C:\\work\\TMP1.$$$ \n\n");
            printf(" echo ci_tempdir=C:\\work2 >xcip \n");
            printf(" mx cipar=xcip null pft=getenv('tmp=','ci_tempdir') \n");
            printf(" C:\\work2\\TMP1.$$$ \n\n");
/*          printf("fst={<fst_spec>|@[<file>]} \n\n");
            printf("    <fst_spec>      fstspec or semicolon separated fstspec list \n");
            printf("    @               fstspec from standard file <dbn>.fst \n");
            printf("    @<file>         fstspec from file <file> \n\n");
            printf("    add extracted key+posting elements to data fields \n");
            printf("    tagged according fstspec specification \n\n");
*/
            printf("syntax^uy^hsyntax^xx \n\n");
            printf(" mx \n");
            mxsyntax("", ibufsiz/(1024*1024), ptabcategories/3, ptabwidth);
            MXEXIT(0);
        }
#endif /* WHATISNEW */

    if (argc == 2 && strcmp(argv[1],"what") == 0) {
        printf("%s",cicopyr("?Utility MX"));
#if BEFORE20010109
        printf("\n");
        printf("MFRL=%"_LD_" FMT=%"_LD_" ",maxmfrl,fmtasize);
#endif
        printf("\n");
        MXEXIT(0);
    }

    if (argc == 1) {
        mxsyntax("  ", ibufsiz/(1024*1024), ptabcategories/3, ptabwidth);
        MXEXIT(0);
    }


    p=argv[argnext];
    if (sscanf(p,"files=%2d",&n) == 1) {
#if OPEN30
    d33_regs.h.ah = 0x67;   /* Set handle count */
        d33_regs.x.bx = n;      /* no of handles */
#if 1 /* DOS32BITS */
        newax=intdosx(&d33_regs,&d33_regs,&d33_sregs);
#else
    newax=intdos(&d33_regs,&d33_regs);
#endif
        if (d33_regs.x.cflag)
            fprintf(stderr,"AX = %04x [%02d]\n",newax,n);
#endif
    argnext++;
    }

#if DBXMSTXL
    p=NULL;
    for (i=argnext; i < argc; i++) if (strncmp(argv[i],"mstxl?",6) == 0 || strncmp(argv[i],"mstxl=",6) == 0) { p=argv[i]; break; }
    if (p) {
    }
#endif

#if CIWTF                                /* WTFUN support */
    p=NULL;
    for (i=argnext; i < argc; i++) if (strncmp(argv[i],"cipar?",6) == 0 || strncmp(argv[i],"cipar=",6) == 0) { p=argv[i]; break; }
    /* setup wtfnew
    */
    if (!ciawtfp) if (p) if (*(p+6)) if (dbxciset(p+6)) fatal(p);
    if (!ciawtfp) if (dbxciset("mx.cip")) dbxciset(NULL); // optional mx.cip
    if (!ciawtfp) fatal("mx/ciawtfp");
    if (ciawtfp) maxmfrl=rec_maxmfrl; //maxmfrl local //fmt_fsiz set by dbxinit
#else //CIWTF
    /* forca carga de rec_maxmfrl */
    p=dbxcipar(NULL,"maxmfrl",'=');
    if (dbxcipok) { sscanf(p,"%"_LD_,&rec_maxmfrl); maxmfrl=rec_maxmfrl; }
    fmt_fsiz=maxmfrl;
/* printf("+++maxmfrl=%"_LD_"\n",maxmfrl); */
#endif //CIWTF

    /*
        Parsing
    */
    for (i=argnext; (i < argc) || fpprm; ) {

        /* get parameter */

        if (fpprm) {
            p=fgets(prmbuff,PRMSIZE-1,fpprm);
            if (p) p[strlen(p)-1]='\0';  /* strip \n */
            else {
                fclose(fpprm);
                fpprm = (nfpprms-- > 0) ? vfpprm[nfpprms] : NULL;
                if (parmtrace) if (fpprm) printf("+++ in/pop= %p\n",fpprm);
                continue;
            }
        }
        else {
            if (i >= argc) fatal("i >= argc");
            p=argv[i++];
        }

        n=strlen(p);
        q=p+n-1;
        for (; n; n--, p++) if (!isspace(*p)) break;                    /* leading */
        if (strncmp(p,"seq?",4) == 0 || strncmp(p,"seq=",4) == 0 || strncmp(p,"seq/",4) == 0) ;      /* "seq=x " */
        else for (; n; n--, q--) if (isspace(*q)) *q='\0'; else break;  /* trailing */
        if (n < 0) fatal("bug/parm");
        if (n == 0) continue;
#if MXCOMMENT
        if (*p == '/')   if (*(p+1) == '*')       { p+=2;  incomment=1; }
        if (incomment) {
            n=0;
            q=strstr(p,"*/");
            if (q) { p=q+2; incomment=0; n=strlen(p); }
            if (n == 0) continue;
        }
#endif /* MXCOMMENT */
        if (parmtrace) printf("+++ parm=%s !\n",p);


        if (nprms >= MAXNPRM) fatal("MAXNPRM overflow");
        if ((vprmp[nprms]=(PRMSTRU *)ALLOC((ALLOPARM)sizeof(PRMSTRU)))
                        == (PRMSTRU *)ALLONULL) fatal("ALLOC/prm/stru");
        vprmp[nprms]->plen=strlen(p);
        if ((vprmp[nprms]->p=(char *)ALLOC((ALLOPARM)(vprmp[nprms]->plen+1)))
                                == (char *)ALLONULL) fatal("ALLOC/prm/p");
        memmove(vprmp[nprms]->p,p,vprmp[nprms]->plen+1); /* '\0' */

        p=vprmp[nprms++]->p;


        /* in= parameter */

        if (strncmp(p,"in?",3) == 0 || strncmp(p,"in=",3) == 0) {
            q=p+3;
            if (fpprm) {
                if (parmtrace) printf("+++ in/push=%p\n",fpprm);
                if (nfpprms >= MAXNFPRM) fatal(p);
                vfpprm[nfpprms++]=fpprm;
            }
            if (parmtrace) printf("+++ in/open=%s\n",q);
            if ((fpprm=fopen(dbxcipar(NULL,q,'='),"r")) == NULL)
                fatal(p);
            continue;
        }


        /* setup parameters */

#if MXFULLINV || MXIFUPD
        if (strcmp(p,"-reset") == 0) {
            parmmenosreset=1;
            continue;
        }
#endif
#if MXFULLINV
        if (strcmp(p,"-balan") == 0) {
            fullinv_balan=0;
            continue;
        }
#endif

        if (*p == '-') {
            minus1++;
            if (minus1 == 1) {
                parmcontrol=parmleader=parmxref=parmdir=parmfields=1;
            }
        }
        if (strcmp(p,"-all") == 0) {
            parmcontrol=parmleader=parmxref=parmdir=parmfields=0;
            continue;
        }
        if (strcmp(p,"-control") == 0) {
            parmcontrol=0;
            continue;
        }
        if (strcmp(p,"-leader") == 0) {
            parmleader=0;
            continue;
        }
        if (strcmp(p,"-xref") == 0) {
            parmxref=0;
            continue;
        }
        if (strcmp(p,"-dir") == 0) {
            parmdir=0;
            continue;
        }
        if (strcmp(p,"-fields") == 0) {
            parmfields=0;
            continue;
        }

        if (strcmp(p,"+hits") == 0) {
            parmhits=1;
            continue;
        }
#if MXFST
        if (strcmp(p,"+fix") == 0) {
            cifstfix=1;
            continue;
        }
        if (strcmp(p,"+fix/m") == 0) {
            cifstfix=cifstfim=1;
            continue;
        }
#endif
        if (*p == '+') {
            plus1++;
            if (plus1 == 1)
                parmcontrol=parmleader=parmxref=parmdir=parmfields=0;
        }
        if (strcmp(p,"+all") == 0) {
            parmcontrol=parmleader=parmxref=parmdir=parmfields=1;
            continue;
        }
        if (strcmp(p,"+control") == 0) {
            parmcontrol=1;
            continue;
        }
        if (strcmp(p,"+leader") == 0) {
            parmleader=1;
            continue;
        }
        if (strcmp(p,"+xref") == 0) {
            parmxref=1;
            continue;
        }
        if (strcmp(p,"+dir") == 0) {
            parmdir=1;
            continue;
        }
        if (strcmp(p,"+fields") == 0) {
            parmfields=1;
            continue;
        }
        if (strncmp(p,"from?",5) == 0 || strncmp(p,"from=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&parmfrom) != 1)
                fatal(p);
            continue;
        }
        if (strncmp(p,"to?",3) == 0 || strncmp(p,"to=",3) == 0) {
            if (sscanf(p+3,"%"_LD_,&parmto) != 1)
                fatal(p);
            continue;
        }
        if (strncmp(p,"loop?",5) == 0 || strncmp(p,"loop=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&parmloop) != 1)
                fatal(p);
            continue;
        }
        if (strncmp(p,"count?",6) == 0 || strncmp(p,"count=",6) == 0) {
            if (sscanf(p+6,"%"_LD_,&parmcount) != 1)
                fatal(p);
            continue;
        }
        if (strncmp(p,"tell?",5) == 0 || strncmp(p,"tell=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&parmtell) != 1)
                fatal(p);
            continue;
        }

        if (strcmp(p,"trace") == 0) {
            parmtrace=1;
            continue;
        }
        if (strcmp(p,"trace=dbx") == 0) {
            dbxtrace=1;
            continue;
        }
        if (strcmp(p,"trace=rec") == 0) {
            rectrace=dectrace=1;
            continue;
        }
        if (strcmp(p,"trace=dec") == 0) {
            dectrace=1;
            continue;
        }
        if (strcmp(p,"trace=trm") == 0) {
            trmtrace=1;
            continue;
        }
#if CIB71
        if (strcmp(p,"trace=b70") == 0) {
            b70trace=1;
            continue;
        }
#else /* CIB71 */
#if LIND || CIB64
        if (strcmp(p,"trace=b50") == 0) {
            b50trace=1;
            continue;
        }
#else
        if (strcmp(p,"trace=b40") == 0) {

            b40trace=1;
            continue;
        }
#endif
#endif /* CIB71 */
        if (strcmp(p,"trace=fmt") == 0) {
            fmttrace=1;
            continue;
        }
        if (strcmp(p,"trace=fst") == 0) {
            fsttrace=1;
            continue;
        }
        if (strcmp(p,"trace=all") == 0) {
            parmtrace=dbxtrace=rectrace=trmtrace=
#if CIB71
        b70trace=
#else /* CIB71 */
#if LIND || CIB64
            b50trace=
#else
            b40trace=
#endif
#endif /* CIB71 */
        fmttrace=fsttrace=1;
            continue;
        }

        if (strncmp(p,"cipar?",6) == 0 || strncmp(p,"cipar=",6) == 0) {
#if CIWTF                                /* WTFUN support */
#else //CIWTF
            if ((dbxcipfp=fopen(p+6,"r")) == NULL) fatal(p);
            /* forca carga de rec_maxmfrl */
            p=dbxcipar(NULL,"maxmfrl",'=');
            if (dbxcipok) { sscanf(p,"%"_LD_,&rec_maxmfrl); maxmfrl=rec_maxmfrl; }
            fmt_fsiz=maxmfrl;
#endif //CIWTF
            continue;
        }

        if (strcmp(p,"mfrl") == 0) {
            parmmfrl=LEADER-1;
            continue;
        }
        if (strncmp(p,"mfrl?",5) == 0 || strncmp(p,"mfrl=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&maxmfrl) != 1)
                fatal(p);
#if CIWTF                                /* WTFUN support */
            if (maxmfrl < (LONGX)MSNVSPLT || (RECHSIZE+maxmfrl) > ALLOMAXV) maxmfrl=MAXMFRL;
            rec_maxmfrl=maxmfrl; // mx only
            fmt_fsiz=maxmfrl;    // mx only
#else //CIWTF
            if (maxmfrl > rec_maxmfrl) rec_maxmfrl=maxmfrl;
            fmt_fsiz=maxmfrl;
#endif //CIWTF
            continue;
        }

#if BEFORE20060120 || 1  // tem que aceitar
        if (strncmp(p,"fmtl?",5) == 0 || strncmp(p,"fmtl=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&fmtasize) != 1)
                fatal(p);
            continue;
        }
#endif //BEFORE20060120

        if (strncmp(p,"load?",5) == 0 || strncmp(p,"load=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&parmload) != 1)
                fatal(p);
            if (parmload > ALLOMAXV) {
                parmload = ALLOMAXV;
                fprintf(stderr,"+++ load=%"_LD_"\n",parmload);
            }
            continue;
        }

        if (strncmp(p,"pages?",6) == 0 || strncmp(p,"pages=",6) == 0) {
            /* NXPAGES */
            p=p+6;
            for (level=0; level < MAXLIV; level++)
                if (*p)
                    if (atoi(p)) {
                        NXPAGES[level]=atoi(p);
                        if (parmtrace)
                            printf("+++ level %d = %d pages\n",
                                level,NXPAGES[level]);
                        while (isdigit(*p)) p++;
                        if (*p == ',') p++;
                    }
            continue;
        }

#if CIB71
        if (strncmp(p,"b70?",4) == 0 || strncmp(p,"b70=",4) == 0 ||
            strncmp(p,"b50?",4) == 0 || strncmp(p,"b50=",4) == 0 ||
            strncmp(p,"b40?",4) == 0 || strncmp(p,"b40=",4) == 0 ||
            strncmp(p,"tmpx?",5) == 0 || strncmp(p,"tmpx=",5) == 0) {
#endif /* CIB71 */
            /* mxbool/b40 query data base */
            if (strncmp(p,"tmpx?",5) == 0 || strncmp(p,"tmpx=",5) == 0) booldbnp=p+5; else booldbnp=p+4;
            if (strncmp(booldbnp,"create=",7) == 0) {
                booldbnp+=7;
                recisis0(booldbnp);
            }
            continue;
        }
#if CIB71
#else /* CIB71 */
#if LIND || CIB64
        if (strncmp(p,"nb1?",4) == 0 || strncmp(p,"nb1=",4) == 0) {
            /* mxbool/b62 nbytes1 parm [b5irxbytes=nbytes1;] */
            if (sscanf(p+4,"%"_LD_,&boolnby1) != 1)
                fatal(p);
            continue;
        }
        if (strncmp(p,"nbb?",4) == 0 || strncmp(p,"nbb=",4) == 0) {
            /* mxbool/b62 b6bufferp->bufsiz parm [b6_hballoc(bufsiz,&b6bufferp);] */
            if (sscanf(p+4,"%"_LD_,&boolnbyb) != 1)
                fatal(p);
            continue;
        }
        if (strncmp(p,"nb0?",4) == 0 || strncmp(p,"nb0=",4) == 0) {
            /* mxbool/b62 b5maxbyts0 parm [if (b5maxbyts0) b5maxbyts=b5maxbyts0;] */
            if (sscanf(p+4,"%"_LD_,&boolnby0) != 1)
                fatal(p);
            continue;
        }
#else
        if (strncmp(p,"nb2?",4) == 0 || strncmp(p,"nb2=",4) == 0) {
            /* mxbool/b40 nbytes2 parm */
            if (sscanf(p+4,"%"_LD_,&boolnby2) != 1)
                fatal(p);
            continue;
        }
#endif
#endif /* CIB71 */
        if (strncmp(p,"btell?",6) == 0 || strncmp(p,"btell=",6) == 0) {
            /* mxbool/b40 tell parm */
            if (sscanf(p+6,"%"_LD_,&booltell) != 1)
                fatal(p);
            continue;
        }
#if CIB71
#else /* CIB71 */
#if LIND || CIB64
#if 000
        if (strncmp(p,"b50t?",5) == 0 || strncmp(p,"b50t=",5) == 0) {
            /* mxbool/b5_run() tmp file name */
            strcpy(b5_tmpnam,p+5);
            continue;
        }
#endif
#else
        if (strncmp(p,"b40t?",5) == 0 || strncmp(p,"b40t=",5) == 0) {
            /* mxbool/b4_run() tmp file name */
            strcpy(b4_tmpnam,p+5);
            continue;
        }
#endif
#endif /* CIB71 */
        if (strncmp(p,"b40u?",5) == 0 || strncmp(p,"b40u=",5) == 0) {
            /* loaductb(mi_gidbnp,isisuctab,p+5); */
            if (!loaductb(mi_gidbnp,isisuctab,p+5)) fatal(p); /* AOT, 02/04/2001 */
            continue;
        }

        if (strncmp(p,"uctab?",6) == 0 || strncmp(p,"uctab=",6) == 0) {
            if (strcmp(p+6,"ansi") == 0) {
                memmove(isisuctab,ansiuctab,256/*sizeof(ansiuctab)*/);
            } else {
                /* loaductb(mi_gidbnp,isisuctab,p+6); */
                if (!loaductb(mi_gidbnp,isisuctab,p+6)) fatal(p); /* AOT, 02/04/2001 */
            }
            continue;
        }
        if (strncmp(p,"actab?",6) == 0 || strncmp(p,"actab=",6) == 0) {
            memset(isiswctab,0x00,256);
            if (strcmp(p+6,"ansi") == 0) {
                for (isiswctot=0; ansiactab[isiswctot]; isiswctot=isiswctot+1)
                    isiswctab[ansiactab[isiswctot]]=1;
            } else {
                unsigned char ubuffup[256];
                /* loadactb(mi_gidbnp,ubuffup,p+6); */
                if (!loadactb(mi_gidbnp,ubuffup,p+6)) fatal(p); /* AOT, 02/04/2001 */
                for (isiswctot=0; ubuffup[isiswctot]; isiswctot=isiswctot+1)
                    isiswctab[ubuffup[isiswctot]]=1;
            }
            continue;
        }

        if (strncmp(p,"dupr?",5) == 0 || strncmp(p,"dupr=",5) == 0 ||
            strncmp(p,"dupl?",5) == 0 || strncmp(p,"dupl=",5) == 0) {
            parmdupp=p+5;
            parmdupx=DUPPRIGHT;
            if (strncmp(p,"dupl",5-1) == 0)
                parmdupx=DUPPLEFT;
            continue;
        }

        if (strncmp(p,"p1?",3) == 0 || strncmp(p,"p1=",3) == 0) {
            prompt1p=p+3;
            continue;
        }
        if (strncmp(p,"p2?",3) == 0 || strncmp(p,"p2=",3) == 0) {
            prompt2p=p+3;
            continue;
        }

        if (strcmp(p,"nowait") == 0 || strcmp(p,"now") == 0) {
            parmwait=0;
            continue;
        }

#if BEFORE20000323           /* because some BRM's GCC - ask rpiva */
        if (strcmp(p,"stderr=off") == 0) {
            cistderr=stdout;
            continue;
        }
#endif

#if MULTI
        if (strcmp(p,"mono") == 0) {
            cipnetws=MONONETS;
            continue;
        }
        if (strcmp(p,"mast") == 0) {
            cipnetws=MASTNETS;
            continue;
        }
        if (strcmp(p,"full") == 0) {
            cipnetws=FULLNETS;
            continue;
        }
#endif
        if (strncmp(p,"k1=",3) == 0) {
            parmk1p=p+3;
            continue;
        }
        if (strncmp(p,"k2=",3) == 0) {
            parmk2p=p+3;
            continue;
        }


#if CIWTF
#else //CIWTF
        /* setup alloc
        */
        q=dbxcipar(NULL,"maxmfrl",'='); if (dbxcipok) sscanf(q,"%"_LD_,&maxmfrl);
        if (maxmfrl < (LONGX)MSNVSPLT || (RECHSIZE+maxmfrl) > ALLOMAXV) maxmfrl=MAXMFRL;
        rec_maxmfrl=maxmfrl;
        fmt_fsiz=maxmfrl;
#endif //CIWTF


        /* positional parameters */

        if (!dbnp) {

            /* allocate global buffer (former buffup, which is defined by mxrun.h) */
            nbytes = (mx_buplen) ? mx_buplen: maxmfrl+maxmfrl;
            if (nbytes > ALLOMAXV) nbytes=ALLOMAXV;
            mx_buplen=nbytes;
            if (parmtrace) printf("+++ mx_bup=%"_LD_" bytes\n",mx_buplen);
            if ((mx_bup=(char *)ALLOC((ALLOPARM)nbytes)) == (char *)ALLONULL)
                fatal("ALLOC/mx_bup");

            recallok((crec=nrecs),(LONGX)sizeof(M0STRU));
            recallok((irec=nrecs),maxmfrl); /* b40 */

            if (strncmp(p,"db=",3) == 0) p+=3;
            if (strncmp(p,"dbn=",4) == 0) p+=4;

            /* allow mx <dbn>.mst */
            if (strlen(p)>strlen(mx1extp)) {
                int len=strlen(p);
                int n=strlen(mx1extp);
                if (memicmp(p+len-n,mx1extp,n) == 0) p[len-n]='\0';
            }

#if DIR_FILE
            if (strncmp(p,"list=",5) == 0) {
                char *directory=p+5;
              //char *pattern=NULL,*q,c='\0';
              //for (q=directory; *q; q++) if (*q='/') pattern=q+1;
              //if (pattern) if (!*pattern) pattern=NULL;  
              //if (pattern) { c=(*pattern); *pattern='\0'; } 
                direntdp = opendir (directory);
              //if (pattern) *pattern=c; 
                if (direntdp != NULL)
                {
                    //char *q; int n,x;
                  //while ((direntep = readdir (direntdp)) != NULL)
                  //{
                //    int x=1; 
                //    if (pattern) {
                //      char *q=(char *)direntep->d_name; 
                //      int n=strlen(q); 
                //      if (strcmp(q+n-strlen(".mak"),".mak")) x=0;
                //    }
                //    if (x) 
                  //  puts (direntep->d_name);
                  //}
                  //(void) closedir (direntdp);
                }
                else
                //fatal("Couldn't open the directory");
                  fatal(p);
                tmpseq=1;
            }
#endif

            if (strncmp(p,"dict=",5) == 0) {
                xtp=p; p+=5;
                parmkdbnp=p; p="null";
                if ((q=strchr(parmkdbnp,',')) != NULL) {
                    *q++='\0';
                    if (sscanf(q,"%"_LD_,&xtl1) != 1) fatal(xtp);
                    parmktag1=(UWORD)xtl1;
                    while (isdigit(*q)) q++;
                    if (*q) {
                        if (*q++ != ',') fatal(xtp);
                        if (sscanf(q,"%"_LD_,&xtl2) != 1) fatal(xtp);
                        parmktag2=(UWORD)xtl2;
                        while (isdigit(*q)) q++;
                        if (*q == '/') { q++;
                            if (sscanf(q,"%"_LD_,&parmkkeep2) != 1) fatal(xtp);
                            if (parmkkeep2 < 1) fatal(xtp);
                            /*while (isdigit(*q)) q++; */
                        }
                        parmkpostings=1;
                    }
                }
            }

            if (strcmp(p,"tmp") == 0) {
                tmpseq=1;
            }
            else {
                if (strncmp(p,"seq?",4) == 0 || strncmp(p,"seq=",4) == 0 || strncmp(p,"seq/",4) == 0) {
                    unsigned blass=0;
                    unsigned blas2=0;
                    q=p+4;
                    if (strncmp(p,"seq/",4) == 0) {
                        p+=3; //skip seq
                        while (*p == '/') {
                            if (strncmp(p,"/fi:"   ,4) == 0) { for (p+=4; isdigit(*p); p++) blas2=blas2*10+((int)(*p)-(int)'0'); continue; }
                            for (p++; isdigit(*p); p++) blass=blass*10+((int)(*p)-(int)'0');
                            if (*p=='k')   p++;
                            if (*p=='m') { p++; blass*=1024; }
                        } /* while /option */
                        if (*p == '?' || *p == '=') p++; else fatal(q-4);
                        if (parmtrace) printf("+++ ibufsiz=%"_LD_" Kbytes log=%"_LD_"\n",(LONGX)blass,(LONGX)blas2);
                        if (blass>=1) ibufsiz=blass*1024;
                        if (blas2>=1) parmseqlog=blas2;
                        q=p;
                    }
                    strcpy(line,q);
                    q=line+strlen(line)-1; parmxseq='\0';
                    if (strlen(line) > 1) {
                        if (*(q-1) == '\\') { parmxseq= *q; if (*q == 'n') parmxseq='\n'; *(q-1)='\0'; }
                    }
                    if (!parmxseq) if (!isalnum(*q)) {parmxseq= *q; *q='\0';}
                    if (parmtrace) printf("+++ %s|=%02x=%c\n",line,parmxseq,parmxseq);
                    tmpseq=1;
                    if (strcmp(line,"con") == 0) fpseq=stdin; else
#if BEFORE20060308
                    if ((fpseq=fopen(dbxcipar(NULL,line,'='),"r")) == NULL) fatal(p);
#else
                    {
                        n1fdseq=0;
                        n1left=0;
                        n1iptr=NULL;
                        n1count=0;
                        /* allocate input buffer */
                        obufsiz=(mx_buplen-4-BUFSIZ)/2;  ubufsiz=obufsiz+BUFSIZ;
                        obuf=mx_bup; ubuf=obuf+obufsiz+2;
                        if (parmtrace) printf("+++ ibuf=%"_LD_" bytes\n",(LONGX)ibufsiz);
                        if ((ibuf=(char *)ALLOC((ALLOPARM)ibufsiz+1)) == (char *)ALLONULL) fatal("ALLOC/ibuf");
                        if (recfgetseq(line, parmxseq, parmseqlog, ibuf, ibufsiz, obuf,       0, ubuf, ubufsiz, &n1fdseq, &n1left, &n1iptr, &n1count) <= 0) fatal(p);
                    }
#endif
                }
#if XML_FILE
                else {
                    if (strncmp(p,"xml?",4) == 0 || strncmp(p,"xml=",4) == 0 || strncmp(p,"xml/",4) == 0) {
                        unsigned blass=0;
                        unsigned blas2=0;
                        q=p+4;
                        if (strncmp(p,"xml/",4) == 0) {
                            p+=3; //skip seq
                            while (*p == '/') {
                                if (strncmp(p,"/fi:"   ,4) == 0) { for (p+=4; isdigit(*p); p++) blas2=blas2*10+((int)(*p)-(int)'0'); continue; }
                                for (p++; isdigit(*p); p++) blass=blass*10+((int)(*p)-(int)'0');
                                if (*p=='k')   p++;
                                if (*p=='m') { p++; blass*=1024; }
                            } /* while /option */
                            if (*p == '?' || *p == '=') p++; else fatal(q-4);
                            if (parmtrace) printf("+++ ibufsiz=%"_LD_" Kbytes log=%"_LD_"\n",(LONGX)blass,(LONGX)blas2);
                            if (blass>=1) ibufsiz=blass*1024;
                            if (blas2>=1) parmseqlog=blas2;
                            q=p;
                        }
                        p = strchr(p, ',');
                        if (!p) fatal("missing ,");
                        *p = '\0'; p++;
                        strcpy(tagName, p);
                        strcpy(line,q);
                        q=line+strlen(line)-1; parmxseq='\0';
                        if (strlen(line) > 1) {
                            if (*(q-1) == '\\') { parmxseq= *q; if (*q == 'n') parmxseq='\n'; *(q-1)='\0'; }
                        }
                        if (!parmxseq) if (!isalnum(*q)) {parmxseq= *q; *q='\0';}
                        if (parmtrace) printf("+++ %s|=%02x=%c\n",line,parmxseq,parmxseq);
                        tmpseq=1;
                        n1fdxml=0;
                        n1left=0;
                        n1iptr=NULL;
                        n1count=0;
                        /* allocate input buffer */
                        obufsiz=(mx_buplen-4-BUFSIZ)/2;  ubufsiz=obufsiz+BUFSIZ;
                        obuf=mx_bup; ubuf=obuf+obufsiz+2;
                        if (parmtrace) printf("+++ ibuf=%"_LD_" bytes\n",(LONGX)ibufsiz);
                        if ((ibuf=(char *)ALLOC((ALLOPARM)ibufsiz+1)) == (char *)ALLONULL) fatal("ALLOC/ibuf");
                        if (recfgetxml(line, ibuf, ibufsiz, obuf,       0, ubuf, ubufsiz, &n1fdxml, &n1left, &n1iptr, &n1count) <= 0) fatal(p);
                    }
#endif /* XML_FILE */
                else {
                    if (strncmp(p,"iso?",4) == 0 || strncmp(p,"iso=",4) == 0) {
                        q=p+4;
                        if (strchr(q,'=')) {
                            if (strncmp(q,"marc=",5) == 0) {  /* AOT, 06/06/2005 */
                                q+=5;
                                isfix=(-1);    /*  no CRLF */
                                lrfix=ISOBSIZ; /* set to max line len */
                                iso_ismarc=1;
                            }
                            else
                            if (strncmp(q,"0=",2) == 0) {    /* AOT, 06/06/2005 */
                                q+=2;
                                isfix=0;      /*  single line */
                                lrfix=ISOBSIZ;
                            }
                            else {
                                if (sscanf(q,"%d=%s",&lrfix,line) != 2) fatal(p);
                                q=strchr(q,'=')+1;
                                isfix=1;
                            }
                        }
                        else
#if MPE
                            { isfix=1; lrfix=80; }
#else
                            { isfix=0; lrfix=80; }
#endif
                        if (iso_open(NULL,q,0,lrfix,isfix) <= 0) fatal(p);
                        parmiso1=1;
                        tmpseq=1;
                    }
                    else if (parmkdbnp) {
                             mi_gidbnp=parmkdbnp; dbnp="null";
                         }
                         else mi_gidbnp=dbnp=p;
                }
#if XML_FILE
                }
#endif /* XML_FILE */
            }

#if DELETETMPSEQ
            if (tmpseq) recisis0(dbnp="tmpseq");
#else
            if (tmpseq) dbnp="null";
#endif

            if (parmload) mstsetup(dbnp,parmload,parmload);        /* invsetup is called in mxbol()/mj_join() */

            if (parmtrace) printf("+++ RECORD %"_LD_" = %s#%"_LD_" \n",crec,dbnp,0L);
            RECORD(crec,dbnp,0L);

            if (parmtrace) printf("+++ RECORD %"_LD_" = %s#%"_LD_" \n",irec,dbnp,1L);
            RECORD(irec,dbnp,1L);

            continue;

        } /* end okdbnp */



        /* processing parameters */

        if (!dbnp) fatal(p);

#if IFUPDATE
        if (strcmp(p,"mfbw") == 0) { parmmfbw=1; continue; }
#endif

#if TB
        if (strncmp(p,"tb?",3) == 0 || strncmp(p,"tb=",3) == 0 ||
            strncmp(p,"tbin?",5) == 0 || strncmp(p,"tbin=",5) == 0) {
            if (strncmp(p,"tb?",3) == 0 || strncmp(p,"tb=",3) == 0) q=p+3; else q=p+5;
            if (sscanf(q,"%"_LD_,&ld) != 1) fatal(p);
             if (ntagbin >= MAXTBIN) fatal(p);
            tagbin=(UWORD)ld; vtagbin[ntagbin++]=tagbin;
            continue;
        }
#endif /* TB */

        if (strncmp(p,"join?",5) == 0 || strncmp(p,"join=",5) == 0 ||
            strncmp(p,"jchk?",5) == 0 || strncmp(p,"jchk=",5) == 0 ||
            strncmp(p,"jch0?",5) == 0 || strncmp(p,"jch0=",5) == 0 ||
            strncmp(p,"jch1?",5) == 0 || strncmp(p,"jch1=",5) == 0) {

            jchk=0;
            if (strncmp(p,"jchk",5-1) == 0)
                jchk=1;
            if (strncmp(p,"jch0",5-1) == 0)
                jchk= -1;
            if (strncmp(p,"jch1",5-1) == 0)
                jchk= -2;

            *line='\0';
            for (q=p+5, n=0; *q; q++, n++)
                if (*q == '?' || *q == '=') {
                    memmove(line,p+5,n); line[n]='\0';
                    break;
                }
            if (!*line || !*(q+1))
                fatal(p);
            if (mj_scan(xmxp,dbnp,line,q+1,jchk)) MXEXIT(1);
            continue;
        }
        if (strncmp(p,"jmax?",5) == 0 || strncmp(p,"jmax=",5) == 0) {
            /* mj_join() */
            if (sscanf(p+5,"%"_LD_,&mj_jmax) != 1)
                fatal(p);
            continue;
        }
        if (strncmp(p,"jtag?",5) == 0 || strncmp(p,"jtag=",5) == 0) {
            /* mj_join() */
            if (sscanf(p+5,"%"_LD_,&ld) != 1)
                fatal(p);
            mj_jtag=(UWORD)ld;
            continue;
        }

        if (strncmp(p,"xxxx?",5) == 0 || strncmp(p,"xxxx=",5) == 0 ||
            strncmp(p,"proc?",5) == 0 || strncmp(p,"proc=",5) == 0) {
            q=p+5;
            if (!*q)
                fatal(p);
            if (mp_scan(xmxp,dbnp,q)) MXEXIT(1);
            continue;
        }

        if (strncmp(p,"convert?",8) == 0 || strncmp(p,"convert=",8) == 0) {
            if (strcmp(p+8,"ansi") == 0) {
                tabconvp=ansimctab; /* asc2ansi */
                memmove(isisuctab,ansiuctab,256/*sizeof(ansiuctab)*/);
                for (isiswctot=0; ansiactab[isiswctot]; isiswctot=isiswctot+1)
                    isiswctab[ansiactab[isiswctot]]=1;
                continue;
            }
            if (strcmp(p+8,"arab") == 0) {
                tabconvp=arabmctab; /* 708_1256 */
                continue;
            }
            fatal(p);
        }

#if RECDECO
        if (strncmp(p,"decod?",6) == 0 || strncmp(p,"decod=",6) == 0) {
            recdecod(irec,p+6,(upirec)?upirec:(upirec=nrecs));
            continue;
        }
#endif

#if RECGIZM
#if MXGIZP
        if (strncmp(p,"gizp?",5) == 0 || strncmp(p,"gizp=",5) == 0) {
            parmgizp=p+5;
            continue;
        }
        if (strncmp(p,"gizp/h?",7) == 0 || strncmp(p,"gizp/h=",7) == 0) {
            parmgizp=p+7;
            parmgizhp=1;
            continue;
        }
#endif
        if (strncmp(p,"gizmo?",6) == 0 || strncmp(p,"gizmo=",6) == 0 ||
            strncmp(p,"giz1?",5) == 0 || strncmp(p,"giz1=",5) == 0 ||
            strncmp(p,"giz2?",5) == 0 || strncmp(p,"giz2=",5) == 0) {
            if (strncmp(p,"gizmo?",6) == 0 || strncmp(p,"gizmo=",6) == 0)
                q=p+6;
            else
                q=p+5;
            if (!*q) fatal(p);

            if (RDBvgzrp) {     /* gizmo already defined */
                for (gizmap=RDBvgzrp; gizmap->nextp; )
                    gizmap=gizmap->nextp;
                gizmread(q,&gizmap->nextp,(upirec)?upirec:(upirec=nrecs));
                gizmap=gizmap->nextp;
            }
            else {              /* first gizmo definition */
                gizmread(q,&RDBvgzrp,(upirec)?upirec:(upirec=nrecs));
            }

            if (parmtrace) {
                for (gizmap=RDBvgzrp; gizmap; gizmap=gizmap->nextp)
                    printf("+++ gizmo=%s \n",gizmap->gdbnp);
            }
            continue;
        }
#endif /* RECGIZM */

#if RECXPND
        if (strncmp(p,"putdir?",7) == 0 || strncmp(p,"putdir=",7) == 0) {
            if (sscanf(p+7,"%d,%d",&parmxpn01,&parmxpn02) != 2)
                fatal(p);                       /* recxpn00() tag,dlm */
            continue;
        }
        if (strncmp(p,"getdir?",7) == 0 || strncmp(p,"getdir=",7) == 0) {
            if (sscanf(p+7,"%d,%d",&RDBxpn01,&RDBxpn02) != 2)
                fatal(p);                       /* recxpnmo() */
            continue;
        }
#endif

        if (strncmp(p,"sys?",4) == 0 || strncmp(p,"sys=",4) == 0 ||
            strncmp(p,"sys/show?",9) == 0 ||
            strncmp(p,"sys/show=",9) == 0) {
            p+=4;
            if (strncmp(p,"show",5-1) == 0) {
                p+=5; parmsyss=1;
            }
            parmsysp=loadfile(dbnp,'@',p,NULL,fmtasize+1,'\0');
        if (!parmsysp) fatal(p);
            parmfields=0;
            continue;
        }

        if (strncmp(p,"text?",5) == 0 || strncmp(p,"text=",5) == 0 ||
            strncmp(p,"text/show?",10) == 0 ||
            strncmp(p,"text/show=",10) == 0) {
            p+=5;
            if (strncmp(p,"show",5-1) == 0) {
                p+=5;
                mxtext_pshow=1; parmfields=0;
            }
            parmtextp=p;
            continue;
        }

        if (strncmp(p,"outmfntag?",10) == 0 || strncmp(p,"outmfntag=",10) == 0) {
            if (sscanf(p+10,"%d",&parmoutmfntag) != 1)
                fatal(p);
            continue;
        }
        if (strncmp(p,"lw?",3) == 0 || strncmp(p,"lw=",3) == 0) {
            if (sscanf(p+3,"%"_LD_,&fmtlw) != 1)
                fatal(p);
            continue;
        }

        if (strncmp(p,"pft?",4) == 0 || strncmp(p,"pft=",4) == 0 ||
            strncmp(p,"tab?",4) == 0 || strncmp(p,"tab=",4) == 0 || strncmp(p,"tab/",4) == 0) {
            int parmtabula=0;
            q=p+4;
            if (strncmp(p,"tab?",4) == 0 || strncmp(p,"tab=",4) == 0 || strncmp(p,"tab/",4) == 0) {
                parmtabula=1; fmtlw=0; //5.2
                if (strncmp(p,"tab/",4) == 0) {
                    unsigned class=0;
                    unsigned width=0;
                    unsigned tabtg=0;
                    p+=3; //skip tab
                    while (*p == '/') {
                        if (strncmp(p,"/lines:",7) == 0) { for (p+=7; isdigit(*p); p++) class=class*10+((int)(*p)-(int)'0'); if (class<1) fatal(p); }
                        else
                        if (strncmp(p,"/width:",7) == 0) { for (p+=7; isdigit(*p); p++) width=width*10+((int)(*p)-(int)'0'); if (width<1) fatal(p); }
                        else
                        if (strncmp(p,"/tab:"  ,5) == 0) { for (p+=5; isdigit(*p); p++) tabtg=tabtg*10+((int)(*p)-(int)'0'); if (tabtg<1) fatal(p); }
                        else fatal(q-4);
                    } /* while /option */
                    if (*p == '?' || *p == '=') p++; else fatal(q-4);
                    if (parmtrace) printf("+++ tab class=%"_LD_" width=%"_LD_"\n",(LONGX)class,(LONGX)width);
                    if (class > 0) ptabcategories=(LONGX)class*3; // hashing;
                    if (width > 0) ptabwidth=(int)width;
                    if (tabtg > 0) parmtabtg=tabtg;
                    q=p;
                }
                if (parmtrace) printf("+++ tab class=%"_LD_" width=%"_LD_" tabtg=%"_LD_"\n",(LONGX)ptabcategories,(LONGX)ptabwidth,(LONGX)parmtabtg);
            }
            if (strcmp(q,"@") == 0) sprintf(q=mx_bup,"@%s.pft",dbnp);
            fmtarea=loadfile(dbnp,'@',q,fmtarea,fmtasize+1,'\0');
            if (!fmtarea) fatal(q);
            q=fmtarea;
            if (parmtrace) {
                printf("+++ fmt=%c",OPENFCHR); printf("%s",q);
                printf("%c ..",CLOSFCHR); if (getchar() != '\n') MXEXIT(0);
            }
            if (!*q) fatal(p);
            if (fmt_gener(&pgmp,q)) {
                fprintf(stderr,"*** fmt_error=%"_LD_"\n",fmt_error);
                fatal(q+fmt_errof);
            }
            parmfields=0;
            if (parmtabula) {
                /* calculate maxprim and allocate/zero hash table */
                LONGX maxprim;
                ptabtable=hashalloc(ptabcategories,ptabwidth,&maxprim);
                ptabmaxprim=maxprim;
                if (parmtrace) printf("+++ hash=%"_LD_"\n",ptabmaxprim);
                /* allocate/zero frequencies */
                ptabCOUNT=     (LONGX *) loadfile(NULL,'@',"",NULL, ptabmaxprim*sizeof(LONGX),   '\0');
            }
            continue;
        }


        if (strncmp(p,"jdi?",4) == 0 || strncmp(p,"jdi=",4) == 0 || strncmp(p,"jdi/",4) == 0) {
            p+=3; //skip jdi
            while (*p == '/') {
                q=p;
                if (isdigit(*(p+1))            ) { for (p+=1, jdiadd55=0; isdigit(*p); p++) jdiadd55=jdiadd55*10+((int)(*p)-(int)'0'); if (jdiadd55<0) fatal(p); }
                else
                if (strncmp(p,"/lines:",7) == 0) { for (p+=7, jdiclass=0; isdigit(*p); p++) jdiclass=jdiclass*10+((int)(*p)-(int)'0'); if (jdiclass<1) fatal(p); }
                else
                if (strncmp(p,"/width:",7) == 0) { for (p+=7, jdiwidth=0; isdigit(*p); p++) jdiwidth=jdiwidth*10+((int)(*p)-(int)'0'); if (jdiwidth<1) fatal(p); }
                else
                if (strncmp(p,"/tab:"  ,5) == 0) { for (p+=5, jditabtg=0; isdigit(*p); p++) jditabtg=jditabtg*10+((int)(*p)-(int)'0'); if (jditabtg<1) fatal(p); }
                else
                if (strncmp(p,"/occ:"  ,5) == 0) { for (p+=5, jditaboc=0; isdigit(*p); p++) jditaboc=jditaboc*10+((int)(*p)-(int)'0'); if (jditaboc<1) fatal(p); }
                else
                if (strncmp(p,"/top:"  ,5) == 0) { for (p+=5, jdisstop=0; isdigit(*p); p++) jdisstop=jdisstop*10+((int)(*p)-(int)'0'); if (jdisstop<1) fatal(p); }
                else
                if (strncmp(p,"/nmin:" ,6) == 0) { for (p+=6, jdisminn=0; isdigit(*p); p++) jdisminn=jdisminn*10+((int)(*p)-(int)'0'); if (jdisminn<1) fatal(p); }
                else
                if (strncmp(p,"/wmin:"    ,6) == 0) { p+=6; if (sscanf(p,"%f",&jdisminw) != 1) fatal(p); while (isdigit(*p) || *p=='.') p++; }
                else
                if (strncmp(p,"/vmin:"    ,6) == 0) { p+=6; if (sscanf(p,"%f",&jdisminv) != 1) fatal(p); while (isdigit(*p) || *p=='.') p++; }
                else
                if (strncmp(p,"/show"     ,5) == 0) { p+=5; jdipshow=1; parmfields=0; }
                else
                if (strncmp(p,"/sort:w1"  ,8) == 0) { p+=8; jdisort=JDISORTW1; }
                else
                if (strncmp(p,"/sort:w2"  ,8) == 0) { p+=8; jdisort=JDISORTW2; }
                else
                if (strncmp(p,"/sort:w3"  ,8) == 0) { p+=8; jdisort=JDISORTW3; }
                else
                if (strncmp(p,"/sort:w4"  ,8) == 0) { p+=8; jdisort=JDISORTW4; }
                else
                if (strncmp(p,"/sort:v1"  ,8) == 0) { p+=8; jdisort=JDISORTV1; }
                else
                if (strncmp(p,"/sort:v2"  ,8) == 0) { p+=8; jdisort=JDISORTV2; }
                else
                if (strncmp(p,"/sort:v3"  ,8) == 0) { p+=8; jdisort=JDISORTV3; }
                else
                if (strncmp(p,"/sort:v4"  ,8) == 0) { p+=8; jdisort=JDISORTV4; }
                else
                if (strncmp(p,"/sort:n1"  ,8) == 0) { p+=8; jdisort=JDISORTN1; }
                else
                if (strncmp(p,"/sort:n2"  ,8) == 0) { p+=8; jdisort=JDISORTN2; }
                else
                if (strncmp(p,"/sort:n3"  ,8) == 0) { p+=8; jdisort=JDISORTN3; }
                else
                if (strncmp(p,"/sort:n4"  ,8) == 0) { p+=8; jdisort=JDISORTN4; }
                else fatal(q);
            } /* while /option */
            if (*p == '?' || *p == '=') p++; else fatal(p); parmtabjdip=p; 
            q=p+1; p=strchr(q,'?'); if (!p) p=strchr(q,'='); if (!p) fatal(parmtabjdip); *p++='\0';
            if (parmtrace) printf("+++ jdi parmtabjdip=%s\n",parmtabjdip);
            q=p; if (strcmp(q,"@") == 0) sprintf(q=mx_bup,"@%s.pft",parmtabjdip);
            jdifmtarea=loadfile(parmtabjdip,'@',q,jdifmtarea,jdifmtasize+1,'\0');
            if (!jdifmtarea) fatal(q); q=jdifmtarea;
            if (!*q) fatal(p);
            if (parmtrace) {
                printf("+++ fmt=%c",OPENFCHR); printf("%s",q);
                printf("%c ..",CLOSFCHR); if (getchar() != '\n') MXEXIT(0);
            }
            if (fmt_gener(&jdipgmp,q)) {
                fprintf(stderr,"*** fmt_error=%"_LD_"\n",fmt_error);
                fatal(q+fmt_errof);
            }
            //parmfields=0;
            if (parmtabjdip) {
                jdislen=(9+jdiwidth+1);
                /* calculate maxprim and allocate/zero hash table */
                jdicategories=(LONGX)jdiclass*3; // hashing;
                if (parmtrace) printf("+++ jdi width=%d categories=%"_LD_"\n",jdiwidth,(LONGX)jdicategories);
                jditable=hashalloc(jdicategories,jdiwidth,&jdimaxprim);
                if (parmtrace) printf("+++ jdi hash=%"_LD_"\n",jdimaxprim);
                /* allocate/zero frequencies */
                jdiSUM0w=     (float *) loadfile(NULL,'@',"",NULL, jdimaxprim           *sizeof(float), '\0');
                jdiSUM0v=     (float *) loadfile(NULL,'@',"",NULL, jdimaxprim           *sizeof(float), '\0');
                jdiSUM0n=     (LONGX  *) loadfile(NULL,'@',"",NULL, jdimaxprim           *sizeof(LONGX ), '\0');
                jdiFREQ1=     (LONGX  *) loadfile(NULL,'@',"",NULL, jdimaxprim           *sizeof(LONGX ), '\0');
                /* allocate/zero sort area - jdiclass */
                jdiSBUF=      (char  *) loadfile(NULL,'@',"",NULL, jdimaxprim*jdislen*sizeof(char ), '\0');
                jdiSBUH=      (LONGX  *) loadfile(NULL,'@',"",NULL, jdimaxprim           *sizeof(LONGX ), '\0');
                jdiSBUFx=     (char  *) loadfile(NULL,'@',"",NULL,            jdislen*sizeof(char ), '\0');
            }
            //alloc jdi term
#if CICPP
            try { jditrmp=new TRMSTRU(cisisxp); }
            catch (BAD_ALLOC) { fatal("mx/jdi/reftrm/next"); }
#else /* CICPP */
            for (jditrm_prat=maxntrm; jditrm_prat--; ){
                if (!vtrmp[jditrm_prat]) /* ja' decrementado */
                    break;
            }
            if (!jditrm_prat) fatal("mx/jdi/reftrm/next");
#endif /* CICPP */
            //alloc jdi record 
#if CICPP
            try { jdirecp=new RECSTRU(cisisxp); }
            catch (BAD_ALLOC) { fatal("mx/jdi/refrec/next"); }
#else /* CICPP */
            for (jdirec_prat=maxnrec; jdirec_prat--; ){
                if (!vrecp[jdirec_prat]) /* ja' decrementado */
                    break;
            }
            if (!jdirec_prat) fatal("mx/jdi/refrec/next");
#endif /* CICPP */
            //open
#if CICPP
            jditrmp->term(parmtabjdip,"A");
#else
            jditrmp=vtrmp[term(jditrm_prat,parmtabjdip,"A")];
#endif
#if CICPP
            jdirecp->record(parmtabjdip,1L);
#else
            jdirecp=vrecp[record(jdirec_prat,parmtabjdip,1L)];
#endif
            continue;
        }


#if INVXBOOL
        if (strncmp(p,"invx?",5) == 0 || strncmp(p,"invx=",5) == 0) {
            /* mxbool/b40 tl_irec */
            if (booltlir == 0) {
#if BEFORE20020128
                record(booltlir=nrecs,dbnp,VMF0nxtmfn(crec)); /* alloc/init */
                VMFRstatus(booltlir)=ACTIVE;                  /* fmt */
                VMFRmfn(booltlir)=1L;                         /* good */
#else
            /* b7_run faz "trirecp=vrep[tlirec]" */
            if (!booltlir) record(booltlir=nrecs,"null",1L);
#endif
            }
            record(irecx=nrecs,p+5,1L);
            *(batchp=mx_bup)='\0';
            while (VRECrc(irecx) != RCEOF) {
                if (VRECrc(irecx) == RCNORMAL)
                    for (xdir=0, loop=VMFRnvf(irecx); loop--; xdir++)
                        if (VDIRtag(irecx,xdir) == MFQTXTAG) {
                            sprintf(batchp,"H%d %d ",
                                        MFQTXTAG,VDIRlen(irecx,xdir));
                            batchp+=strlen(batchp);
                            memmove(batchp,
                                VFIELDP(irecx,xdir),VDIRlen(irecx,xdir));
                            batchp+=VDIRlen(irecx,xdir);
                            *batchp='\0';
                        }
                record(irecx,VRDBname(irecx),VMFRmfn(irecx)+1);
            }
#if BEFORE20020128
#else
            dbxflush(VRDBname(irecx)); /* libera x101 */
#endif
            if (fldupdat(booltlir,mx_bup)) fatal(mx_bup);
#if CIAPI
            FREE(vrecp[irecx]); vrecp[irecx]=NULL; nrecs-=1;
#else /* CIAPI */
            FREE(vrecp[irecx]); vrecp[irecx]=NULL; nrecs--;
#endif /* CIAPI */

            if (parmtrace) prtfields(vrecp[booltlir],VMFRmfn(booltlir));
            continue;
        }
#endif


//#if CICGI
        if (strncmp(p,"pftout?",7) == 0 || strncmp(p,"pftout=",7) == 0) {
            parmpftoutp=p+7;
            if ((parmpftoutfp=fopen(dbxcipar(NULL,parmpftoutp,'='),"w")) == NULL)
                fatal(p);
            continue;
        }
        if (strncmp(p,"prolog?",7) == 0 || strncmp(p,"prolog=",7) == 0) {
            parmprologp=p+7;
            continue;
        }
        if (strncmp(p,"epilog?",7) == 0 || strncmp(p,"epilog=",7) == 0) {
            parmepilogp=p+7;
            irecepilog=irec;
            continue;
        }
//#endif /* CICGI */

        /* output parameters */

        if (strncmp(p,"iso?",4) == 0 || strncmp(p,"iso=",4) == 0 ||
            strncmp(p,"outiso?",7) == 0 || strncmp(p,"outiso=",7) == 0) {
            if (strncmp(p,"iso?",4) == 0 || strncmp(p,"iso=",4) == 0) q=p+4; else q=p+7;
                if (strchr(q,'=')) {
                    if (strncmp(q,"marc=",5) == 0) {  /* AOT, 06/06/2005 */
                        q+=5;
                        lrfox=ISOBSIZ; /* set to max line len */
                        isfox=(-1);    /*  no CRLF */
                        parmisoxfs=ISOXFSMARC;
                        parmisoxgs=ISOXGSMARC;
                        iso_ismarc=ismarc=1;
                    }
                    else
                    if (strncmp(q,"0=",2) == 0) {    /* AOT, 06/06/2005 */
                        q+=2;
                        lrfox=ISOBSIZ; /* set to max line len */
                        isfox=0;       /*  with [CR]LF */
                        parmisoxfs=ISOXFSMARC;
                        parmisoxgs=ISOXGSMARC;
                    }
                    else {
                        if (sscanf(q,"%d=%s",&lrfox,line) != 2) fatal(p);
                        if (lrfox > ISOBSIZ) fatal(p);
                        q=strchr(q,'=')+1;
                        isfox=1;    /* test ok */
                    }
                }
                else {
#if MPE
                    isfox=1;
#else
                    isfox=0;
#endif
                    lrfox=80;
                }
            if (iso_creat(NULL,q,0,lrfox,isfox) <= 0) fatal(p);
            parmiso2=1;
            continue;
        }

        if (strncmp(p,"isotag1?",8) == 0 || strncmp(p,"isotag1=",8) == 0) {
            if (sscanf(p+8,"%u",&parmisildrtag) != 1)
                fatal(p);
            continue;
        }
        if (strncmp(p,"outisotag1?",11) == 0 || strncmp(p,"outisotag1=",11) == 0) {
            if (sscanf(p+11,"%u",&parmisoldrtag) != 1)
                fatal(p);
            continue;
        }

        if (strncmp(p,"fix?",4) == 0 || strncmp(p,"fix=",4) == 0) {
            q=p+4;
            if ((fpfix=fopen(dbxcipar(NULL,q,'='),"w")) == NULL) fatal(p);
            parmiso2=2;
            continue;
        }

#if MXFST
        if (strncmp(p,"ln1?",4) == 0 || strncmp(p,"ln1=",4) == 0) {
#if MXIFUPD
            if (ifnp) fatal(p);
#endif
             if (strcmp(p+4,"void") == 0) {
                parmfstup=0;
            }
            else    /* 21/05/98 */
                if (fst_open(NULL,p+4,0) <= 0) fatal(p);
            continue;
        }
        if (strncmp(p,"ln2?",4) == 0 || strncmp(p,"ln2=",4) == 0) {
#if MXIFUPD
            if (ifnp) fatal(p);
#endif
             if (strcmp(p+4,"void") == 0) {
                parmfstup=0;
            }
            else    /* 21/05/98 */
                if (fst_open(NULL,p+4,1) <= 0) fatal(p);
            continue;
        }

        if (strncmp(p,"fst?",4) == 0 || strncmp(p,"fst=",4) == 0 ||
            strncmp(p,"fst/h?",6) == 0 || strncmp(p,"fst/h=",6) == 0) {
            if (strncmp(p,"fst/h",5) == 0) {
                q=p+6;
                fst_hdrp=(char *)ALLOC((ALLOPARM)(fst_hdrx=1000));
                if (fst_hdrp == ALLONULL) fatal("ALLOC/fst_hdrp");
            }
            else q=p+4;
            if (strcmp(q,"@") == 0) sprintf(q=line,"@%s.fst",dbnp);
            if (!loadfile(mi_gidbnp,'@',q,mx_bup,mx_buplen,'\n')) fatal(q);
            fst_gener(&fstpgmp,mx_bup);
            if (parmtrace) printf("%s.\n",mx_bup); 
            if (fst_error) {
                printf("\n*** FST error#%"_LD_" at line %"_LD_"\n",fst_error,fst_errl);
                if (fst_error == FSTERRF)
                    printf("\n*** FMT error#%"_LD_" at offset %"_LD_"\n",
                                                fmt_error,fmt_errof);
                fatal(fst_errp);
            }
            continue;
        }

        if (strncmp(p,"stw?",4) == 0 || strncmp(p,"stw=",4) == 0) {
            q=p+4;
            if (*q++ != '@') fatal(p);
            if (!*q) sprintf(q=line,"%s.stw",dbnp);
        stwp=loadstw(mi_gidbnp,q,stwp=NULL,0L,&n);
        if (!stwp) fatal(p);
            if (parmtrace) {
                for (q=stwp; *q; q+=LE1+1) printf("%s/",q);
                printf("=%d\n",n);
            }
            continue;
        }
#endif

#if MXFULLINV
        if (strncmp(p,"fullinv",7) == 0) {
            if (!fstpgmp) fatal(p);
#if MXIFUPD
            if (ifnp) fatal(p);
#endif
            q=p+7;
            /* fullinv[/dict][/keep][/m][/ansi]=<ifn> */
            if (strncmp(q,"/dict",5) == 0) {q+=5; fullinv_pstflag=IFUPDICT;
                                                  ciiflfim=1;
                                                  cifstfix=cifstfim=1;
                                           }
            if (strncmp(q,"/keep",5) == 0) {q+=5; fullinv_keep=1;}
#if LIND
            ciiflfim=1;
            cifstfix=cifstfim=1;
#endif
            if (strncmp(q,"/m" ,2)   == 0) {q+=2; ciiflfim=1;
                                                  cifstfix=cifstfim=1;
                                           }
            if (strncmp(q,"/ansi",5) == 0) {q+=5;
                memmove(isisuctab,ansiuctab,256/*sizeof(ansiuctab)*/);
                for (isiswctot=0; ansiactab[isiswctot]; isiswctot=isiswctot+1)
                    isiswctab[ansiactab[isiswctot]]=1;
            }
            if (strncmp(q,"?",1) == 0 || strncmp(q,"=",1) == 0) q+=1; else fatal(p);
            if (!*q) fatal(p);
            fullinv_ifnp=q;
            parmfstup=0;
            cifstfix=1;                                           /* +fix */
            parmcontrol=parmleader=parmxref=parmdir=parmfields=0; /* -all */
            parmwait=0;                                           /* now */
            if (!fst_fd[0]) {
                sprintf(mx_bup,"%s.ln1",fullinv_ifnp);
                if (fst_open(NULL,mx_bup,0) <= 0) fatal(p);
            }
            if (!fst_fd[1]) {
                sprintf(mx_bup,"%s.ln2",fullinv_ifnp);
                if (fst_open(NULL,mx_bup,1) <= 0) fatal(p);
            }
            continue;
        }
#if LIND
        if (strncmp(p,"maxmfn?",7) == 0 || strncmp(p,"maxmfn=",7) == 0) {
            if (sscanf(p+7,"%"_LD_,&fullinv_maxmfn) != 1) fatal(p);
            continue;
        }
#endif
#endif /* MXFULLINV */


#if MXFULLINV || MXIFUPD
        if (strncmp(p,"master?",7) == 0 || strncmp(p,"master=",7) == 0) {
            parmmaster=p+7; if (!*parmmaster) fatal(p);
            continue;
        }
#endif /* MXFULLINV || MXIFUPD */

#if MXIFUPD
        if (strncmp(p,"ifupd",5) == 0) {
            if (ifnp) fatal(p);
            if (fst_fd[0] || fst_fd[1]) fatal(p);
            q=p+5;
            /* ifupd[/create][/dict]=<ifn> */
            if (strncmp(q,"/create",7) == 0) {q+=7; ifcreate=1;}
            if (strncmp(q,"/dict",5) == 0) {q+=5; pstflag=IFUPDICT;}
            if (strncmp(q,"?",1) == 0 || strncmp(q,"=",1) == 0) q+=1;
            else fatal(p);
            ifnp=q;
            if (!*ifnp) fatal(p);
            continue;
        }
        if (strncmp(p,"maxlk1?",7) == 0 || strncmp(p,"maxlk1=",7) == 0) {
            if (sscanf(p+7,"%d",&maxlk1) != 1)
                fatal(p);
            continue;
        }
        if (strncmp(p,"maxlk2?",7) == 0 || strncmp(p,"maxlk2=",7) == 0) {
            if (sscanf(p+7,"%d",&maxlk2) != 1)
                fatal(p);
            continue;
        }
#endif

        if (strncmp(p,"copy?",5) == 0 || strncmp(p,"copy=",5) == 0 ||
            strncmp(p,"append?",7) == 0 || strncmp(p,"append=",7) == 0 ||
            strncmp(p,"updatf?",7) == 0 || strncmp(p,"updatf=",7) == 0 ||
            strncmp(p,"create?",7) == 0 || strncmp(p,"create=",7) == 0 ||
            strncmp(p,"merge?",6) == 0 || strncmp(p,"merge=",6) == 0) {

            parmcopy=COPY;
            if (strncmp(p,"create",7-1) == 0) {                 /* create */
                p+=7;
                recisis0(p);
            }
            else {
                if (strncmp(p,"copy",5-1) == 0)                 /* copy */
                    p+=5;
                else
                    if (strncmp(p,"updatf",7-1) == 0)           /* update */
                    {
                        p+=7;
                        parmcopy=UPDATF;
                    }
                    else
                        if (strncmp(p,"append",7-1) == 0)       /* append */
                            {p+=7; parmcopy=APPEND;}
                        else
                            {p+=6; parmcopy=MERGE;}             /* merge */
                dbxopt_fatal=0; xfd=dbxopen(p,p,mx1extp);
                if (xfd <= 0) recisis0(p); else CLOSE(xfd);
            }
            recallok((upcrec=nrecs),(LONGX)sizeof(M0STRU));
            recallok((upirec=nrecs),maxmfrl); /* 18/11/91 */
#if MULTI
        if (strcmp(p,dbnp) == 0) /* dbxstorp has already set dbxnetws */
#if CIAPI
            dbnp_dbxnetws(p)=dbxcinet(p);
            if (dbnp_dbxnetws(p) != MONONETS) {
#else /* CIAPI */
            dbxstorp(p)->dbxnetws=dbxcinet(p);
            if (dbxstorp(p)->dbxnetws != MONONETS) {
#endif /* CIAPI */
                VREClock(upirec)=RLOCK;
                VREClock(upcrec)=DELOCK;
        }
#endif
            record(upcrec,p,0L);        /* append */    /* RLOCK */
#if MULTI
            if (VRECrc(upcrec) == RCLOCK) FATLOCK;
#endif
            continue;
        }

        if (parmtextp) {
            if (tmpseq) fatal("text x tmpseq");
            fatal("text x bool");
        }

        if (mxbool(xmxp,p,dbnp)) MXEXIT(1);

    } /* end of argv[] */


    if (!dbnp) fatal("input missing");


#if MXFULLINV
    if (fullinv_ifnp) {
        if (parmmaster) fullinv_master=parmmaster;
        else {
            fullinv_master=dbnp;
            if (parmcopy) fullinv_master=VRDBname(upcrec);
        }
#if LIND
        fullinv_reset=0;  /* init=1 */
#else
        if (parmmenosreset) fullinv_reset=0;
        else
            if (!parmmaster)
                if (strcmp(fullinv_master,fullinv_ifnp)) fullinv_reset=0;
#endif

#if 0
        if (!fst_fd[0]) fatal("fullinv/missing ln1=");
        if (!fst_fd[1]) fatal("fullinv/missing ln2=");
#endif
        trmisis0(fullinv_ifnp);
    }
#endif /* MXFULLINV */

#if MXIFUPD
    if (ifnp) {
        if (parmmenosreset) ifupd_reset=0;
        parmfstup=0;
        if (ifcreate) trmisis0(ifnp);
        if (parmcopy) {
            mstcrec=upcrec; mstirec=upirec;
        }
        else {
            mstcrec=crec; mstirec=irec;
#if BEFORE20000831
            svdifupx++;
            upif_init(ifnp);
#else
            svdifupx=1;
#endif
        }
#if CIAPI
        ifndbxp=dbxstorp(ifnp);
        dbnp_dbxitrac(ifnp)=parmtrace;
#else /* CIAPI */
        (ifndbxp=dbxstorp(ifnp))->dbxitrac=parmtrace;
#endif /* CIAPI */
        endup = IFUPCORE;
#if MULTI

#if CIAPI
        if (dbnp_dbxnetws(VRDBname(mstcrec)) != MONONETS) endup = IFUPWRIT;
#else /* CIAPI */
        if (dbxstorp(VRDBname(mstcrec))->dbxnetws != MONONETS) endup = IFUPWRIT;
#endif /* CIAPI */

#endif
    }
#endif



    /*
        Execution
    */

    if (parmloop < 0) {  /* AOT, 26/07/2005 */
        if (!parmfrom) parmfrom = VMF0nxtmfn(crec) - 1;
        if (!parmto)   parmto=1;
    }

    if (tmpseq)
        if (!parmfrom)
            parmfrom = (parmcopy) ? VMF0nxtmfn(upcrec) : 1L;

    if (!parmfrom)
        parmfrom=1L;

    if (!parmto)
        parmto=LONGX_MAX;

    if (parmcontrol)
        prtcontrol(vrecp[crec],dbnp);

    if (parmkdbnp) {
        if (parmk1p) {
            for (xtp=parmk1p, xti=LE2; *xtp && xti; xtp++, xti--) *xtp = isisuctab[(int)*xtp];
            *xtp='\0';
        }
        else parmk1p="!";
        memset(parmk2a,highv[0],LE2); parmk2a[LE2]='\0';
        if (parmk2p) {
            for (xtp=parmk2p, xti=LE2; *xtp && xti; xtp++, xti--) *xtp = isisuctab[(int)*xtp];
            *xtp='\0';
            memmove(parmk2a,parmk2p,strlen(parmk2p));
        }
        itrm=ntrms;
        if (parmtrace) printf("+++ TERM %"_LD_" = %s,%s,%s \n",itrm,parmkdbnp,parmk1p,parmk2p);
#if LIND
        if (!parmkpostings) trmalloc(itrm,0L);  /* avoid opening .iyp */
        if (!parmkpostings) if (parmtrace) printf("+++ trmalloc(%"_LD_",0)\n",itrm);
#endif
        TERM(itrm,parmkdbnp,parmk1p); parmkcount=1;
        itrm2=ntrms; trmp1=trmp;
        if (parmtrace) printf("+++ TERM %"_LD_" = %s,%s,%s \n",itrm2,parmkdbnp,TRMkey,parmk2p);
#if LIND
        if (!parmkpostings) trmalloc(itrm2,0L);  /* avoid opening .iyp */
        if (!parmkpostings) if (parmtrace) printf("+++ trmalloc(%"_LD_",0)\n",itrm2);
#endif
        TERM(itrm2,parmkdbnp,TRMkey); trmp2=trmp;
        trmp=trmp1;
    }

    if (parmprologp) {
#if CICGI
#if CIWTF
        if (ciahcgi) {
            if (fldupdat(irec,ciahcgi)) fatal(ciahcgi);
        }
#endif //CIWTF
#endif /* CICGI */
#define PROL1 1 // AOT 27/06/2009
#if PROL1
        if (1) {
            LONGX v31009=VMF0nxtmfn(crec);
            LONGX v31409=boolhits;
            sprintf(mx_bup,"a31009|%"_LD_"|a31409|%"_LD_"|",v31009,v31409);
            if (fldupdat(irec,mx_bup)) fatal(mx_bup);
        }
#endif //PROL1
        if (recfmt(irec,rec_maxmfrl,parmprologp,mx_bup,mx_buplen) < 0) {
            printf("Content-type: text/plain\n\nMX fatal: %s",parmprologp);
        }
        if (parmpftoutfp && 0) fprintf(parmpftoutfp,"%s",mx_bup); else
        printf("%s",mx_bup);
    }
#if CICGI
    else if (cgixpok) { printf("Content-type: text/html\n\n");
                        printf("<html><body>\n"); }
#endif /* CICGI */


    /*
        Loops throught...
    */

LOOP:

    for (mfn=parmfrom, boolh=0; ; mfn+=parmloop) {

        LONGX xboolmfn;

        if (boolqryp) { /* mxbool/boolh */
            if (boolh) {
                boolh+=parmloop;
                if (parmloop < 0) {                                                /* AOT, 26/07/2005 */
                    if (boolh < 1)        boolh=boolhits+1; /* end of hits */
                }
                else {
                    if (boolh > boolhits) boolh=boolhits+1; /* end of hits */
                }
            }
            else {
                if (parmloop < 0)                                                  /* AOT, 26/07/2005 */
                    boolh=boolhits;
                else
                    boolh=1;
            }
#if CIB71
            xboolmfn=b7_hit(cib7p,boolridx,boolh,b7setno);
#else /* CIB71 */
#if LIND || CIB64
#if BIGREC
            xboolmfn=b5_hit(boolridx,boolh,mx_bup);
#else
            xboolmfn=b6_hit(boolridx,boolh,b5setno,b6bufferp);
#endif
#else
            xboolmfn=b4_hit(boolridx,boolh);
#endif
#endif /* CIB71 */
            if (parmtrace) printf("+++ xboolmfn=%"_LD_"\n",xboolmfn);
            if (xboolmfn <= 0L) break;

            if (parmloop < 0) {  /* AOT, 26/07/2005 */
                if (xboolmfn > parmfrom) continue;
                if (xboolmfn < parmto)   continue;
            }
            else {
                if (xboolmfn < parmfrom) continue;
                if (xboolmfn > parmto)   continue;
            }
            mfn=xboolmfn;
        } /* end if boolqryp */

        if (parmloop < 0) {  /* AOT, 26/07/2005 */
            if (mfn < parmto) break;
        }
        else {
            if (mfn > parmto) break;
        }

        if (count >= parmcount) break;

        if (parmwait)
            if (count) {
                n=1;
                if (parmtextp && !mxtext_found) n=0;
                if (n) {
                    printf(prompt1p); isxp=gets(line); p=isxp;
                    if (ISX(p)) break;
                    if (p) if (*p) { if (mxbool(xmxp,p,dbnp)) MXEXIT(1); count=0; goto LOOP; }
                }
            }

        ++count; /* in advance */

        /* Sequential RECORD */

        if (tmpseq) {
#if DIR_FILE
            if (direntdp) {
                if (direntep = readdir (direntdp))
                {
                    //puts (direntep->d_name);
                    RECORD(irec,dbnp,mfn);
                    RECrc=RCNORMAL; MFRmfn=mfn; MFRstatus=ACTIVE;
                    sprintf(mx_bup,"a1|%s|",direntep->d_name); 
                    if (fldupdat(irec,mx_bup)) fatal(mx_bup);
                    MFRmfn=mfn;
                }
                else {
                    (void) closedir (direntdp);
                    break; // end of directory listing
                }
            }
            else
#endif
            if (fpseq) {
            /* uses global mx_bup as input buffer */
                mx_bup[0]=parmxseq;
                if (recfgets(crec,irec,mx_bup,mx_buplen,fpseq) == EOF)
                    break;
                MFRmfn=mfn;
            }
            else
            if (n1fdseq) {
                if (recfgetseq(NULL, parmxseq, parmseqlog, ibuf, ibufsiz, obuf, obufsiz, ubuf, ubufsiz, &n1fdseq, &n1left, &n1iptr, &n1count) == EOF) break;
                RECORD(irec,dbnp,mfn);
                RECrc=RCNORMAL; MFRmfn=mfn; MFRstatus=ACTIVE;
                if (fldupdat(irec,ubuf)) fatal(ubuf);
                MFRmfn=mfn;
            }
#if XML_FILE
            else
            if (n1fdxml) {
                if (recfgetxml(NULL, ibuf, ibufsiz, obuf, obufsiz, ubuf, ubufsiz, &n1fdxml, &n1left, &n1iptr, &n1count) == EOF) break;
                RECORD(irec,dbnp,mfn);
                RECrc=RCNORMAL; MFRmfn=mfn; MFRstatus=ACTIVE;
                if (fldupdat(irec,ubuf)) fatal(ubuf);
                MFRmfn=mfn;
            }
#endif /* XML_FILE */
            else
                if (parmiso1) {
                    if (iso_read(crec,irec,parmisildrtag) == EOF) break;
                    MFRmfn=mfn;
                }
                else {                                          /* "tmp" */
                    RECORD(irec,dbnp,mfn);
                    RECrc=RCNORMAL; MFRmfn=mfn; MFRstatus=ACTIVE;
                }
#if RECXPND
            if (RDBxpn01) recxpnmo(irec);
#endif
#if RECGIZM
            for (gizmap=RDBvgzrp; gizmap; gizmap=gizmap->nextp)
                recgizmo(irec,gizmap);
#endif
#if RECDECO
            if (RDBvderp) recdecex(irec);
#endif
        }

        else
        if (parmkdbnp) {
          sprintf(mx_bup,"D*"); if (fldupdat(irec,mx_bup)) fatal(mx_bup);
          sprintf(mx_bup,"=%"_LD_,mfn); if (fldupdat(irec,mx_bup)) fatal(mx_bup);
KLOOP:
          trmp=trmp2;
          if (TRMrc != RCEOF) {
            NXTERM(itrm2);
            if (TRMrc == RCEOF) {
              countdict1002=count;
              sprintf(mx_bup,"D1001D1002A1001/%"_LD_"/A1002/%"_LD_"/",count,countdict1002);
              if (fldupdat(irec,mx_bup)) fatal(mx_bup);
            } else {
              if (strcmp(TRMkey,parmk2a) > 0) {
                countdict1002=count;
                sprintf(mx_bup,"D1001D1002A1001/%"_LD_"/A1002/%"_LD_"/",count,countdict1002);
                if (fldupdat(irec,mx_bup)) fatal(mx_bup);
              }
            }
          }
          trmp=trmp1;

          if (parmkpostings) TRMpmfn=posting(itrm,TRMpost+1);
#if 0
          if (count==1 || !parmkpostings || (parmkpostings && TRMpmfn <= 0)) { /* */
#endif
          xgo=0;
          if (count==1) xgo=1;
          if (!parmkpostings) xgo=1;
          if (parmkpostings) if (TRMpmfn <= 0) xgo=1;
          if (xgo) { /* */
            if (TRMrc == RCEOF) break;
            if (strcmp(TRMkey,parmk2a) > 0) break;
             if (count > 1L) {
                NXTERM(itrm);
                if (TRMrc == RCEOF) break;
                if (strcmp(TRMkey,parmk2a) > 0) break;
                if (parmkpostings) TRMpmfn=posting(itrm,TRMpost+1);
                parmkcount++;
            }
            for (xtp=TRMkey+(xti=(TRMlcase)?LE2:LE1)-1; xti--; ) if (*xtp == ' ') *xtp--='\0'; else break;
          } /* */
          if (parmtrace) { printf("+++ TRMkey:\n"); for (xtp=TRMkey, xti=LE2; xti--; xtp++) printf("%c=%02x ",*xtp,*xtp); printf("\n"); }
          if (!parmkpostings || TRMpost%parmkkeep2 == 1 || parmkkeep2 == 1) {
            sprintf(mx_bup,"H%9"_LD_" %9"_LD_" %s^l%d^s%"_LD_"^t%"_LD_"^k%"_LD_,(LONGX)parmktag1,0L,TRMkey,TRMlcase+1,(LONGX)strlen(TRMkey),TRMxytotp,parmkcount);
            sprintf(mx_bup+1+10,"%9"_LD_,(LONGX)strlen(mx_bup)-(1+10+10)); mx_bup[10+10]=' ';
            if (fldupdat(irec,mx_bup)) fatal(mx_bup);
          }
          if (parmkpostings && TRMpmfn > 0) { /* */
            sprintf(mx_bup,"A%"_LD_"/^m%"_LD_"^t%"_LD_"^o%"_LD_"^c%"_LD_"^p%"_LD_"^k%"_LD_"/",(LONGX)parmktag2,TRMpmfn,(LONGX)TRMptag,(LONGX)TRMpocc,(LONGX)TRMpcnt,TRMpost,parmkcount);
            if (fldupdat(irec,mx_bup)) fatal(mx_bup);
          } /* */
if ((parmkpostings && TRMpost < TRMxytotp) && (TRMpost%parmkkeep2 != 0)) goto KLOOP;
        }

        else {
            RECORD(irec,dbnp,mfn);
            if (RECrc == RCEOF) break;
        }

        if (parmhits) {
            if (boolqryp) /* mxbool/boolh */
                sprintf(mx_bup,"A1001/%"_LD_"/A1002/%"_LD_"/",boolh,boolhits);
            else
              if (parmkdbnp)
                sprintf(mx_bup,"A1001/%"_LD_"/A1002/%"_LD_"/",count,countdict1002);
              else
/*
                sprintf(mx_bup,"A1001/%"_LD_"/A1002/%"_LD_"/",MFRmfn,VMF0nxtmfn(crec)-1);
*/
                {
                    sprintf(mx_bup,"A1001/%"_LD_"/",MFRmfn-parmfrom+1);
                    if (MFRmfn == VMF0nxtmfn(crec)-1 ||
                        MFRmfn == parmto ||
                        count == parmcount)
                      sprintf(mx_bup+strlen(mx_bup),"A1002/%"_LD_"/",MFRmfn-parmfrom+1);
                }
            if (fldupdat(irec,mx_bup) != NULL) fatal(mx_bup);
        }

        if (parmtextp) {
            if (mxtext(xmxp,irec,parmtextp)) MXEXIT(1);
            if (!mxtext_found)
                continue;
        }

#if IFUPDATE
    if (parmmfbw) {
        if (MFRmfbwb == 0) continue;
        else {
          if (!brec) { brec=nrecs; record(brec,dbnp,mfn); }
          recreadb=MFRmfbwb; recreadp=MFRmfbwp;
          recread(vrecp[brec],MFRmfn);
          memmove(MFX,VMFX(brec),VMFRmfrl(brec)); /* in trust.. */
        }
    }
#endif

        if (parmtell)
            if ((count % parmtell) == 0)
                fprintf(stderr,"+++ %"_LD_" \n",count);

        if (parmxref)   prtxref(recp,mfn);

        if (parmleader) prtleader(recp,0L);

        if (RECrc != RCNORMAL) {

            if (parmdir)        prtdir(recp,mfn);

            if (parmfields)     prtfields(recp,mfn);

            if (parmcopy == COPY) if (!parmoutmfntag) {

                record(upirec,VRDBname(upcrec),MFRmfn); /* RLOCK */
                if (VRECrc(upirec) == RCLOCK) FATLOCK;
                if (VRECrc(upirec) == RCNORMAL) {
#if MULTI
                    memmove(VMFX(upirec),MFX,MFRmfrl);
#else
                    reccopy(upirec,upcrec,recp,MFRmfn);
#endif
                    recupdat(upcrec,upirec);
#if MXIFUPD
                    if (ifnp)
                        ifupdat(VRDBname(upcrec),MFRmfn,MFRmfn,
                           ifnp,fstpgmp,stwp,maxlk1,maxlk2,pstflag,endup);
#endif


                } /* end recupdat() */
#if MULTI
#if CIAPI
                if (dbnp_dbxnetws(p) != MONONETS) recunlck(upirec,RLOCK);
#else /* CIAPI */
                if (dbxstorp(p)->dbxnetws != MONONETS) recunlck(upirec,RLOCK);
#endif /* CIAPI */
#endif
            }

            continue;
        }


        /* ACTIVE records */

        if (MFRstatus != ACTIVE) fatal("mx/ACTIVE");

        /* join */
        if (mj_n) if (mj_edit(xmxp,irec,0,mj_n-1,mj_jtag,NULL)) MXEXIT(1);

        /* jdi */
        if (parmtabjdip) {
            /* tabulate generated lines */
            char delim=0x01; //'|';
            FFI len;
            char *p0=jdifmtarea;
            if (fmt_inter(jdipgmp,irec,(LONGX)jdifmtlw,jdifmtarea,(LONGX)fmtasize) < 0)
                fatal("mx/jdi/format execution error");
            for (len=0, p=p0; ; ) {
                /* next line */
                if (!*p || *p == '\r' || *p == '\n' || *p == delim) {
                    if (len) {
                        /* tabulate line */
                        LONGX jdimfn=0L;
                        char c=(*p);//save
                        *p='\0';
#if CICPP
                        jditrmp->term(parmtabjdip,p0);
                        if (jditrmp->trmrc == RCNORMAL) jdimfn=trmp->posting(1L);
#else
                        term(jditrm_prat,parmtabjdip,p0);
                        if (jditrmp->trmrc == RCNORMAL) jdimfn=posting(jditrm_prat,1L);
#endif
                        if (jdimfn > 0) {
#if CICPP
                          jdirecp->record(parmtabjdip,jdimfn);
#else
                          record(jdirec_prat,parmtabjdip,jdimfn);
#endif
                          if (jdirecp->recrc == RCNORMAL) {
                            RECSTRU *recp=jdirecp;//local
                            DIRSTRU *dirp=MFRdir;
                            int loop=MFRnvf;
                            int xocc=jditaboc;
                            int xdir=0;
                            unsigned char c,*cp;
                            float lastjdiw=0.0;
                            
                            // conta apenas 1 vez
                            jdintabs++;
                            for (; loop--; dirp++, xdir++) {
                                char *jp,*p;//local
                                int jlen=0,len;
                                float jdiw=0.0;
                                float jdiv=0.0;
                                LONGX  jdin=0;
                                if (dirp->tag != jditabtg) continue;
                                if (!xocc--) break;
                                jp=p=FIELDP(xdir);
                                len=dirp->len; cp=p+len; c=(*cp); p[len]='\0';
                                for (; len--; p++, jlen++) if (*p=='^') break;                                
                                if (jlen) for (; len-->2; p++) if (*p=='^') {
                                    if (*(p+1)=='w') { if (sscanf(p+2,"%f" ,&jdiw) != 1) jdiw=0.0; len-=2; p+=2; continue; } else
                                    if (*(p+1)=='v') { if (sscanf(p+2,"%f" ,&jdiv) != 1) jdiv=0.0; len-=2; p+=2; continue; } else                                    
                                    if (*(p+1)=='n') { if (sscanf(p+2,"%"_LD_,&jdin) != 1) jdin=0;   len-=2; p+=2; continue; }
                                }                                
                                if ((lastjdiw > 0) && (jdiw > lastjdiw)) {
                                    char aux[64];
                                    sprintf(aux, "mx/jdi/jdiw is not sorted. key=%s lastjdiw=%f jdiw=%f\n", jp, lastjdiw, jdiw);
                                    fatal(aux);
                                }
                                lastjdiw = jdiw;                                
                                *cp=c;
                                //tabulate jd
                                if (jdiw>0) {
                                    /* tabulate line */
                                    LONGX  hidx;
                                    int hwidth=(jdiwidth+1);
                                    int hfound;
                                    hidx=hashindex(jditable,jdimaxprim,jdiwidth,jp,jlen,&hfound,1);
                                    //if (hfound) if (!hfound) fatal("");
                                    jdiSUM0w[hidx]+=jdiw;
                                    jdiSUM0v[hidx]+=jdiv;
                                    jdiSUM0n[hidx]+=jdin;
                                    jdiFREQ1[hidx]++;
                                    jditotalw+=jdiw;
                                    jditotalv+=jdiv;
                                    jditotaln+=jdin;
                                    // conta 1 vez para cada tema 
                                    //jdintabs++;
                                    jditotal1++;
                                    if (parmtrace) {
                                        char *h=jditable+hidx*hwidth;
                                        printf("[%9"_LD_"] %f,%f,%"_LD_"/%6"_LD_" %d,%s %d,",hidx,jdiSUM0w[hidx],jdiSUM0v[hidx],jdiSUM0n[hidx],jdiFREQ1[hidx],strlen(h),h,jlen);
                                        for (; jlen--; jp++) printf("%c",*jp); //getchar();
                                        printf("\n");
                                    }
                                }
                            }//end for dirp
                          }//end if RECORD
                        }//end if jdimfn
                        *p=c;//restore
                    }//end if len
                    /* move line ptr */
                    while (*p == '\r' || *p == '\n' || *p == delim) p++;
                    p0=p; len=0;
                    if (!*p0) break;
                }//if !*p || *p == '\r' || *p == '\n' || *p == delim
                else {
                    p++; len++;
                }//end if !*p || *p == '\r' || *p == '\n' || *p == delim
            } /* end for */
            //list and reinit
            if (jditotalw) {
                LONGX  hidx=0;
                char *h=jditable;
                int hwidth=(jdiwidth+1);
                float *w=jdiSUM0w;
                float *v=jdiSUM0v;
                LONGX  *n=jdiSUM0n;
                LONGX  *f=jdiFREQ1;
                LONGX  rev;
                int   sbufs=0;
                char *sbufp=jdiSBUF;
                int   s1u,s1,s2;
                char *p1,*p2;
                *(batchp=mx_bup)='\0';
                for (; hidx < jdimaxprim; hidx++, h+=hwidth, w++, v++, n++, f++) {
                    if (!*h) continue;
                    if (!*w) continue;
                    if (jdisminw) if (*w / *f < jdisminw) continue;
                    if (jdisminv) if (*v / *f < jdisminv) continue;
                    if (jdisminn) if (     *f < jdisminn) continue;
                    rev=999999999L;
                    if (jdisort == JDISORTW1) /* w1 */   rev-=(LONGX)( 100000*         *w / *f        );
                    if (jdisort == JDISORTW2) /* w2 */   rev-=(LONGX)( 100000*         *w / jdintabs  );
                    if (jdisort == JDISORTW3) /* w3 */   rev-=(LONGX)( 100000*         *w / jditotalw );
                    if (jdisort == JDISORTW4) /* w4 */   rev-=(LONGX)( 100000*         *w / jditotal1 );
                    if (jdisort == JDISORTV1) /* v1 */   rev-=(LONGX)( 100000*         *v / *f        );
                    if (jdisort == JDISORTV2) /* v2 */   rev-=(LONGX)( 100000*         *v / jdintabs  );
                    if (jdisort == JDISORTV3) /* v3 */   rev-=(LONGX)( 100000*         *v / jditotalv );
                    if (jdisort == JDISORTV3) /* v4 */   rev-=(LONGX)( 100000*         *v / jditotal1 );
                    if (jdisort == JDISORTN1) /* n1 */   rev-=(LONGX)(         (float) *n / *f        );
                    if (jdisort == JDISORTN2) /* n2 */   rev-=(LONGX)( 100000* (float) *n / jdintabs  );
                    if (jdisort == JDISORTN3) /* n3 */   rev-=(LONGX)( 100000* (float) *n / jditotaln );
                    if (jdisort == JDISORTN3) /* n4 */   rev-=(LONGX)( 100000* (float) *n / jditotal1 );
                    sprintf(sbufp,"%09"_LD_"%s",rev,h); sbufp+=jdislen; jdiSBUH[sbufs++]=hidx;
                } /* end hidx */
                /*if (sbufs>jdisstop) sbufs=jdisstop;*/
                for (s1=0, p1=jdiSBUF, s1u=sbufs-1; s1<s1u; s1++, p1+=jdislen) {
                    for (s2=s1+1, p2=jdiSBUF+jdislen*s2; s2<sbufs; s2++, p2+=jdislen) {
                        if (memcmp(p1,p2,jdislen) <= 0) continue;
                        memmove(jdiSBUFx,p1  ,jdislen); 
                        memmove(p1  ,p2  ,jdislen); 
                        memmove(p2  ,jdiSBUFx,jdislen); 
                        hidx=jdiSBUH[s1]; jdiSBUH[s1]=jdiSBUH[s2]; jdiSBUH[s2]=hidx;
                    } /* end s2 */
                } /* end s1 */
                if (sbufs>jdisstop) sbufs=jdisstop;
                for (sbufp=jdiSBUF, s1=0; s1<sbufs; s1++, sbufp+=jdislen) {
                    sscanf(sbufp,"%09"_LD_,&rev); hidx=jdiSBUH[s1];
                    h=jditable+hidx*hwidth;;
                    w=jdiSUM0w+hidx;
                    v=jdiSUM0v+hidx;
                    n=jdiSUM0n+hidx;
                    f=jdiFREQ1+hidx;
                  //if (jdiadd55) { sprintf(batchp,"A%"_LD_"\001",(LONGX)jdiadd55); batchp+=strlen(batchp); }
                    if (jdiadd55) { sprintf(batchp,"<%"_LD_" 0>" ,(LONGX)jdiadd55); batchp+=strlen(batchp); }
                    if (jdiaddfm == 2) 
                    sprintf(batchp,"%s|w|%8.6f|%8.6f|%8.6f|%8.6f|v|%8.6f|%8.6f|%8.6f|%8.6f|n|%4.2f|%4.2f|%8.6f|%4.2f|t|%6.4f|%6.4f|%"_LD_"|%"_LD_"|T|%4.2f|%4.2f|%"_LD_"|%"_LD_"|%"_LD_
                           ,         h
                           ,        *w / *f
                           ,        *w / jdintabs
                           ,        *w / jditotalw
                           ,        *w / jditotal1
                           ,        *v / *f
                           ,        *v / jdintabs
                           ,        *v / jditotalv
                           ,        *v / jditotal1
                           ,(float) *n / *f
                           ,(float) *n / jdintabs
                           ,(float) *n / jditotaln
                           ,(float) *n / jditotal1
                           ,        *w
                           ,        *v
                           ,        *n
                           ,        *f
                           ,         jditotalw
                           ,         jditotalv
                           ,         jditotaln
                           ,         jdintabs
                           ,         jditotal1
                    );
                    else
                    sprintf(batchp,"%s|T|%4.2f|%4.2f|%"_LD_"|%"_LD_"|%"_LD_"|t|%f|%f|%"_LD_"|%"_LD_"|w|%8.6f|%8.6f|%8.6f|%8.6f|v|%8.6f|%8.6f|%8.6f|%8.6f|n|%4.2f|%4.2f|%8.6f|%4.2f"
                           ,         h
                           ,         jditotalw
                           ,         jditotalv
                           ,         jditotaln
                           ,         jdintabs
                           ,         jditotal1
                           ,        *w
                           ,        *v
                           ,        *n
                           ,        *f
                           ,        *w / *f
                           ,        *w / jdintabs
                           ,        *w / jditotalw
                           ,        *w / jditotal1
                           ,        *v / *f
                           ,        *v / jdintabs
                           ,        *v / jditotalv
                           ,        *v / jditotal1
                           ,(float) *n / *f
                           ,(float) *n / jdintabs
                           ,(float) *n / jditotaln
                           ,(float) *n / jditotal1
                    );
                    if (parmtrace || jdipshow) printf("%"_LD_"|%d|%s\n",mfn,s1+1,batchp);
                    batchp+=strlen(batchp); 
                  //if (jdiadd55) { sprintf(batchp,"\001"); batchp+=strlen(batchp); } 
                    if (jdiadd55) { sprintf(batchp,"</%"_LD_">",(LONGX)jdiadd55); batchp+=strlen(batchp); }
                    *w=0.0; *v=0.0; *n=0.0; *f=0; *h='\0'; //reinit
                } /* end hidx */
                jditotalw=0.0; jditotalv=0.00; jditotaln=0; jdintabs=0;jditotal1=0;
            } /* end jditotalw */
            if (jdiadd55) if (mx_bup[0]) { char *p=fldupdat(irec,mx_bup); if (p) { if (parmtrace) fatal(p); else fatal("mx/jdi/fldupdat"); } }
        } /* end parmtabjdip */

        /* proc */
        if (mp_n) if (mp_edit(xmxp,irec,0,mp_n-1)) MXEXIT(1);

        /* convert */
        if (tabconvp) {
            unsigned char *p,c;
            unsigned int n,i,pi;
            for (p=FIELDP(0), n=MFRmfrl-MFRbase; n--; p++) {
                pi=(int)(*p);
                i=(int)tabconvp[pi];
                c = (unsigned char )i;
                *p=c;
            }
        }

#if MXFST
        if (fstpgmp) {
            *mx_bup='\0'; if (parmfstup) fst_batchup=mx_bup; /* fst_batchp=mx_bup; */
            fst_inter(fstpgmp,irec,stwp,
                        &lnk1p,maxlk1,&lnk2p,maxlk2,&qtylk1,&qtylk2);
            fst_fx[0]+=qtylk1; /* lnk1p */
            fst_fx[1]+=qtylk2; /* lnk2p */
            if (*mx_bup) if (fldupdat(irec,mx_bup)) fatal(mx_bup);
            if (fst_hdrp) if (*fst_hdrp) {
                sprintf(mx_bup,"H 33000 %d ",fst_hdru);
                batchp=mx_bup+strlen(mx_bup);
                memmove(batchp,fst_hdrp,fst_hdru); *(batchp+fst_hdru)='\0';
                if (fldupdat(irec,mx_bup)) fatal(mx_bup);
            }
        }
#endif /* MXFST */


        if (parmdir)        prtdir(recp,MFRmfn);

        if (parmfields)     prtfields(recp,MFRmfn);

        if (fmtarea) {
#if BEFORE20010213
            fmt_inter(pgmp,irec,(LONGX)fmtlw,fmtarea,(LONGX)fmtasize);
#else
            if (fmt_inter(pgmp,irec,(LONGX)fmtlw,fmtarea,(LONGX)fmtasize) < 0)
                fatal("mx/pft/format execution error");
#endif
            if (parmoutmfntag) {
              char delim='|';
              int tag=parmoutmfntag+1;
              FFI len;
              char *p0=fmtarea;
              batchp=mx_bup; *batchp='\0';
              for (len=0, p=p0; ; ) {
                if (!*p || *p == '\r' || *p == '\n' || *p == delim) {
                    if (len) {
                        sprintf(batchp,"H%"_LD_" %"_LD_" ",(LONGX)tag,(LONGX)len); batchp+=strlen(batchp);
                        memmove(batchp,p0,len); *(batchp+=len)='\0';
                    }
                    if (mx_bup[0]) {
                        /* out rec */
                        RECSTRU *recp;
                        RECORD(upirec,VRDBname(upcrec),VMF0nxtmfn(upcrec)); MFRstatus=ACTIVE;
                        sprintf(batchp+1,"A%"_LD_"/%"_LD_"/",(LONGX)parmoutmfntag,VMFRmfn(irec));
                        if (fldupdat(upirec,batchp+1)) fatal("mx/outpft/fldupdat/1");
                        if (fldupdat(upirec,mx_bup)) fatal("mx/outpft/fldupdat/2");
                        recupdat(upcrec,upirec);
                        /* re-init */
                        batchp=mx_bup; *batchp='\0'; tag=parmoutmfntag+1;
                    }
                    else tag++;
                    /* move line ptr */
                    while (*p == '\r' || *p == '\n' || *p == delim) p++;
                    p0=p; len=0;
                    if (!*p0) break;
                }
                else {
                    p++; len++;
                }
              } /* end for */
            } /* end parmoutpft */
            else
            if (ptabtable) {
              /* tabulate generated lines */
              char delim=0x01; //'|';
              FFI len;
              char *p0=fmtarea;
              for (len=0, p=p0; ; ) {
                /* next line */
                if (!*p || *p == '\r' || *p == '\n' || *p == delim) {
                    if (len) {
                        /* tabulate line */
                        LONGX hidx;
                        int hwidth=(ptabwidth+1);
                        int hfound;
                        int tabv=1;
                        if (parmtabtg) {
                            FFI loop;
                            DIRSTRU *dirp;
                            int xdir;
                            for (dirp=MFRdir, xdir=0, loop=MFRnvf; loop--; dirp++, xdir++) {
                                int len;
                                char *p;
                                if (dirp->tag != parmtabtg) continue;
                                p=FIELDP(xdir);
                                for (tabv=0, len=dirp->len; len--; p++) if (isdigit(*p)) tabv=tabv*10+((int)*p - (int)'0'); else break;
                            }
                        }
                        hidx=hashindex(ptabtable,ptabmaxprim,ptabwidth,p0,len,&hfound,1);
                        //if (hfound) if (!hfound) fatal("");
                        ptabCOUNT[hidx]+=tabv;
                        if (parmtrace) { char *h=ptabtable+hidx*hwidth; printf("[%9"_LD_"] %9"_LD_" %s\n",hidx,jdiFREQ1[hidx],h); }
                    }//end if len
                    /* move line ptr */
                    while (*p == '\r' || *p == '\n' || *p == delim) p++;
                    p0=p; len=0;
                    if (!*p0) break;
                }//if !*p || *p == '\r' || *p == '\n' || *p == delim 
                else {
                    p++; len++;
                }//end if !*p || *p == '\r' || *p == '\n' || *p == delim 
              } /* end for */
            } /* end ptabtable */
            else
            if (parmpftoutfp) { char *q=mx_bup;
              for (p=fmtarea; *p; p++)
                if (*p != '\r') *q++=(*p);
              *q='\0'; if (*mx_bup) fprintf(parmpftoutfp,"%s",mx_bup);
            }    
            else
              for (p=fmtarea; *p; p++)
                if (*p != '\r')
                    putchar(*p);
        }

        if (parmsysp) {
            if (!parmsysa) {
                parmsysa=loadfile(NULL,'@',"",NULL,fmtasize+1,'\0');
        if (!parmsysa) fatal("ALLOC/parmsysa");
        }
#if BEFORE20010213
            recfmt(irec,MAXMFRL,parmsysp,parmsysa,fmtasize);
#else
            if (recfmt(irec,MAXMFRL,parmsysp,parmsysa,fmtasize) < 0)
                fatal("mx/sys/format execution error");
#endif
            for (shp=parmsysa; *shp; ) {
                for (p=shp; *p; p++)
                    if (*p == '\n' || *p == '\r') {
                        *p++ = '\0';
                        if (*p == '\n' || *p == '\r') *p++ = '\0';
                        break;
                    }
                if (*shp) {
                    n=1;
                    if (parmsyss) {
                        printf("%s\n",shp);
                        printf(prompt1p); isxp=gets(line); q=isxp;
                        if (ISX(q)) n=0;
                    }
                    if (n) system(shp);
                }
                shp = p;
            }
        }

        if (parmmfrl)
            if (MFRmfrl > parmmfrl) {
                printf("MFN=%"_LD_" -> MFRmfrl=%d\n",MFRmfn,MFRmfrl);
                parmmfrl=MFRmfrl;
            }

#if TB
    for (itagbin=0; itagbin < ntagbin; itagbin++) {
        tagbin=vtagbin[itagbin];
#if 1
#if BEFORE20000913
        if (tagbin > 999) {
#else
        if (tagbin == 0) {
#endif
          sprintf(line,"create=%"_LD_".dat",MFRmfn);
          dbxopenc(NULL,line,&fdtagbin,NULL,line,0,0);
          if (parmtrace) printf("+++ %s fd=%d\n",line,fdtagbin);
          for (xdir=0; xdir < MFRnvf; xdir++)
                if ((unsigned)CIWRITE(fdtagbin,FIELDP(xdir),DIRlen(xdir))
                != DIRlen(xdir)) fatal("mx/tbin/write");
          CLOSE(fdtagbin);
        } else
#endif
        if ((xdir=fieldx(irec,tagbin,1)) >= 0) {
          sprintf(line,"create=%"_LD_"%c%d",MFRmfn,'.',tagbin);
          dbxopenc(NULL,line,&fdtagbin,NULL,line,0,0);
          if (parmtrace) printf("+++ %s fd=%d\n",line,fdtagbin);
          for (xdir=0; xdir < MFRnvf; xdir++)
           if (DIRtag(xdir) == tagbin)
              if ((unsigned)CIWRITE(fdtagbin,FIELDP(xdir),DIRlen(xdir))
              != DIRlen(xdir)) fatal("mx/tbin/write");
          CLOSE(fdtagbin);
        }
    }
#endif /* TB */

        if (parmcopy || parmiso2) {
#if RECXPND
            if (parmxpn01) recxpn00(irec,parmxpn01,parmxpn02); /* tag,dlm */
#endif
            if (parmiso2 && MFRnvf > 0) {
                if (parmiso2 == 1) iso_write(recp,parmisoxfs,parmisoxgs,parmisoldrtag);
                        else {
                            p=mx_bup; *p='\0';
                            for (xdir=0; xdir < MFRnvf; xdir++) {
                                memmove(p,FIELDP(xdir),n=DIRlen(xdir));
                                p+=n;
                                *p++='|';
                            }
                            *(p-1)='\n'; *p='\0';
                            fprintf(fpfix,"%s",mx_bup);
                        }
            }
            if (parmcopy) if (!parmoutmfntag) {
                if (parmdupp) {
                    if (MFRnvf == 0) {
                        if (fldupdat(irec,"A1//")) fatal("A1//");
                    }
                    for (p=parmdupp; *p; p++) {
                        record(upirec,VRDBname(upcrec),VMF0nxtmfn(upcrec));
                        VMFRstatus(upirec)=ACTIVE;
                        q=mx_bup; n=DIRlen(0); n++;
                        sprintf(q,"H%d %d ",DIRtag(0),n); q+=strlen(q);
                        if (parmdupx == DUPPLEFT) *q++ = *p;
                        memmove(q,FIELDP(0),DIRlen(0)); q+=DIRlen(0);
                        if (parmdupx == DUPPRIGHT) *q++ = *p;
                        *q = '\0';
                        if (fldupdat(upirec,mx_bup)) fatal(mx_bup);
                        recupdat(upcrec,upirec);
                    }
                }
                else {
                    n=1;
                    if (parmcopy == UPDATF) if (MFRnvf <= 0) n=0;
                    if (parmcopy == UPDATF && n) {
                        nbytes=sizeof(LONGX)*MAXUPFTAG;
                        if (!tagfreq) {
                            tagfreq=(LONGX *)ALLOC((ALLOPARM)nbytes);
                            if (tagfreq == (LONGX *)ALLONULL)
                                fatal("ALLOC/tagfreq");
                        }
                        memset(tagfreq,0x00,nbytes);
                        record(upirec,VRDBname(upcrec),MFRmfn); /* RLOCK */
                        if (VRECrc(upirec) == RCLOCK) FATLOCK;
                        for (xdir=0; xdir < MFRnvf; xdir++)
                            if (DIRtag(xdir) >= MAXUPFTAG) fatal("MAXUPFTAG");
                            else tagfreq[DIRtag(xdir)]++;
                        batchp=mx_bup;
                        for (utag=0; utag < MAXUPFTAG; utag++)
                            if (tagfreq[utag]) {
                                sprintf(batchp,"D%"_LD_,(LONGX)utag);
                                batchp+=strlen(batchp);
                            }
                        *batchp='\0';
                        if ((p=fldupdat(upirec,mx_bup)) != NULL) fatal(p);
                        batchp=mx_bup;
                        for (xdir=0; xdir < MFRnvf; xdir++) {
                            sprintf(batchp,"H%6"_LD_" %6"_LD_" ",
                                (LONGX)DIRtag(xdir),(LONGX)DIRlen(xdir));
                            batchp+=strlen(batchp);
                            memmove(batchp,FIELDP(xdir),DIRlen(xdir));
                            batchp+=DIRlen(xdir);
                        }
                        *batchp='\0';
                        if ((p=fldupdat(upirec,mx_bup)) != NULL) fatal(p);
                    }

                    if (parmcopy == MERGE) {
                        record(upirec,VRDBname(upcrec),MFRmfn); /* RLOCK */
                    if (VRECrc(upirec) == RCLOCK) FATLOCK;
                        if (VRECrc(upirec) == RCNORMAL) n=0;
                    }

                    if (parmcopy == APPEND) if (MFRnvf <= 0) n=0;

                    if (n) {
#if MULTI
                        if (VRDBnetws(upcrec) != MONONETS) {
                            if (parmcopy == APPEND) {
                                memmove(VMFX(upirec),MFX,MFRmfrl);
                                VMFRmfn(upirec)=0L;
                                VRECwlock(upirec)|=NEWREC;
                            }
                            else {
                                if (parmcopy != UPDATF) {
                                    record(upirec,VRDBname(upcrec),MFRmfn); /* RLOCK */
                            if (VRECrc(upirec) == RCLOCK) FATLOCK;
                                    memmove(VMFX(upirec),MFX,MFRmfrl);
                                }
                                VRECwlock(upirec)|=NEWREC|FORCE;
                            }
                        }
                        else {
                            if (parmcopy != UPDATF)
                            reccopy(upirec,upcrec,recp,MFRmfn);
                            if (parmcopy == APPEND)
                                VMFRmfn(upirec)=VMF0nxtmfn(upcrec);
                        }
                        recupdat(upcrec,upirec);
#else
                        if (parmcopy != UPDATF)
                            reccopy(upirec,upcrec,recp,MFRmfn);
                        if (parmcopy == APPEND)
                            VMFRmfn(upirec)=VMF0nxtmfn(upcrec);
                        recupdat(upcrec,upirec);
#endif
                    }
                }
            }
        }

#if MXIFUPD
        if (ifnp) {
            if (svdifupx) {
#if BEFORE20000831
#else
                if (svdifupx == 1) {
                    upif_init(ifnp); svdifupx=2;
#if MULTI
#if CIAPI
                    if (dbnp_dbxnetws(ifnp) == FULLNETS) svdifupy=1;
#else /* CIAPI */
                    if (dbxstorp(ifnp)->dbxnetws == FULLNETS) svdifupy=1;
#endif /* CIAPI */
#endif /* MULTI */
                }
                else if (svdifupy) upif_init(ifnp);
#endif
                svdifupd(lnk1p,qtylk1,lnk2p,qtylk2,             /* add new */
                         NULL,0,NULL,0,                         /*         */
                         pstflag,0L);
#if CIAPI
                     dbnp_dbxirecs(ifnp)+=1;
                     dbnp_dbxipadd(ifnp)[0]+=qtylk1;
                     dbnp_dbxipadd(ifnp)[1]+=qtylk2;
#else /* CIAPI*/
                ifndbxp->dbxirecs++;
                ifndbxp->dbxipadd[0]+=qtylk1;
                ifndbxp->dbxipadd[1]+=qtylk2;
#endif /* CIAPI*/
#if BEFORE20000831
#else
                if (svdifupy) { upif_end(ifnp,pstflag); invflush(ifnp); }
#endif
            }
            else
                ifupdat(VRDBname(mstcrec),VMFRmfn(mstirec),VMFRmfn(mstirec),
                        ifnp,fstpgmp,stwp,maxlk1,maxlk2,pstflag,endup);
        }
#endif

    }  /* end of record */


    if (ISX(isxp)) MXEXIT(0);

    if (parmwait && boolqryp != NULL) {
        do {
            printf(prompt2p); isxp=gets(line); p=isxp;
            if (!p) break;
        } while (!*p);
        if (ISX(p) || !p)
            MXEXIT(0);
        if (mxbool(xmxp,p,dbnp)) MXEXIT(1); count=0;
        goto LOOP;
    }

    MXEXIT(0);
}
