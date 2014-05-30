

/* Begin PROCXSOCKREC

         Gload[/<tag>][/nolf][/xml][/socket][/head][={<url>|<file>}]
         Gload[/<tag>][/nolf][/xml][/socket][/head][={<url>|dir=<file>}]

         Gmarx [/<tag>] [/<elem>[,<elem>..]] [=<tag>[,<tag>..]]

Gmarx/1       /documents               =2      |  1.1 2.1
Gmarx/2       /fulltext                =21     |  1.1 2.1 21.1
Gmarx/2       /receipt                 =22     |  1.1 2.1 22.1

*/
#if PROCXSOCKREC

#define MAXXPATH 99

#define bpmovemax(bp,max,len,xleft)   {                                   \
                                    if (max >= 20 && len > max) {         \
                                        int m2=max/3*2,m3=max-m2-3;         \
                                        char *q=bp+m2,*p=bp+len-m3;       \
                                        char *z=bp+max-1;                 \
                                        int left=max;                     \
                                        int lmin=(xleft)?m2:0;            \
                                        while (left-- > lmin)             \
                                          if (*z--==' ')                  \
                                            if (*z=='.' || *z==';')       \
                                              break;                      \
                                        if (left >= lmin) {               \
                                          len=left+3;                     \
                                          memmove(bp+left," ..",3);        \
                                        } else {                          \
                                          len=m2+3+m3;                    \
                                          memmove(q,"...",3); q+=3;        \
                                          while (m3--) *q++= *p++;        \
                                        }                                 \
                                    }                                     \
                                  }
//end bpmovemax(bp,max,len,xleft)

#define hpmovemax(hp,max,a,len)   {                                       \
                                    if (max >= 20 && len > max) {         \
                                        int m2=max/3*2,m3=max-m2-3;       \
                                        memmove(hp,a,m2);        hp+=m2;   \
                                        memmove(hp,"+++",3);     hp+=3;    \
                                        memmove(hp,a+len-m3,m3); hp+=m3;   \
                                    }                                     \
                                    else {                                \
                                        memmove(hp,a,len); hp+=len;        \
                                    }                                     \
                                  }
//end hpmovemax(hp,max,a,len)


	     if (  strncmp(p,"load=",5) == 0 || strncmp(p,"load/",5) == 0 || strcmp(p,"load") == 0 ||

	           strncmp(p,"marx=",5) == 0 || strncmp(p,"marx/",5) == 0 || strncmp(p,"marx ",5) == 0

                                         /* &&  (strstr(p,"/socket=") || strstr(p,"/socket/")  */      ) {


	        int isload = ( strncmp(p,"load=",5) == 0 || strncmp(p,"load/",5) == 0 || strcmp(p,"load") == 0 ) ? 1 : 0;
	        int ismarx = ( strncmp(p,"marx=",5) == 0 || strncmp(p,"marx/",5) == 0 || strncmp(p,"marx ",5) == 0 ) ? 1 : 0;

            LONGX xtag=1;
            int xtagzero=0;
            int parmsocket=0;
            int parmcmd=0;
            int parmhead=0;
            char *filnamp=NULL;

            int parm0=0; //1; // strip 0D
            int parm1=0;      // strip 0A
            //int parmprint=0;
            //FILE *parmfp=stdout;

            int parmparse=0;
            char *vxpath[MAXXPATH+1];
            int vxtag[MAXXPATH+1];
            int xmlspec=1;
            int colonspec=0;
            char *colonspecp="";
            char *colonspecq;
            int parmxml=0;
            int xmlopened=0;

            int  parmfile=0;
            char *filepath="";
            char *filechar="";
            char *filename="";
            char *filedot="";
            char *fileext="";

            int parmindex=0;
            char *iphbuff=NULL;
            char *ihtbuff=NULL;
            char *ihtptr[21];
            int   ihtlen[21];
            int   ihtocc[21];
            char *ihtpta[21];
            int   ihtpla[21];
            char *ihtptc[21];
            char *ihtptd[21];

            char *xbuffer=NULL;

            int parmtag=0; //allow 0
            int parmmin=0;
            int parmmax=0;
            int parm1cc=0;
            int parm1c2=0;
            int parmocc=0;
            int parmoc2=0;
            int clean=0;
            int parmxlen=0;
            int parmx=0;
            int chunch1=0;
            int chunch2=0;
            int chunch3=0;
            int parmvin=0;

            char selattr[BUFSIZ+BUFSIZ];
            int selsize;
            int selparm;
            char selcode[BUFSIZ];
            char selcodearea[BUFSIZ];


            char *xarea=NULL;
            LONGX nbytes;
            char *urlp=NULL;
            char *q;

            char *haddp=NULL;
            char *hlenp=NULL;
            char *hdatap=NULL;
            LONGX hlen=0;

            char *protocol;
            char *serverhost;
            uint16_t port;
            char *pathp;

            int maxrds=INT_MAX;
            int xport=0;

            char *msgp;
            int rc;

            //char *dbname;
            //LONGX mfn;
            //char *key;

#if VDBBNAM_SERVER_PORT
            //int dbx;
            //int ndbx=3;
            char *vdbname[] = {
                "lilacs",
                "artigo",
                "citacoes",
            };
            char *vdbserver[] = {
                "bases.bireme.br",
                "www.scielo.br",
                "serverofi.bireme.br",
            };
            int vdbport[] = {
                1417,
                1417,
                1417
            };
#endif
            vxpath[0]=NULL; vxtag[0]=0; //eol

            /* scan parms
            */
            if (ismarx) {  // Gmarx
              char *q;
              int ipath=0;
              p+=4;  /* skip marx */
              while (isspace(*p)) p++;
              if (*p != '/') fatal("fldupdat/procx/Gmarx [/<tag>] /<elem> =<tag>");
              p++;
              while (isspace(*p)) p++;
              parmtag=1; //default
              if (isdigit(*p)) for (parmtag=0; isdigit(*p); p++ ) parmtag=parmtag*10+((int)(*p)-(int)'0');
              if (parmtag < 1) fatal("fldupdat/procx/Gmarx/tag");
              while (isspace(*p)) p++;
              if (*p=='/') p++; else fatal("fldupdat/procx/Gmarx/elem dlm");
              vxpath[ipath]=p; // /element
              selattr[0]='\0'; selsize=0; selparm=0; selcode[0]='\0';
              q=strstr(p,"@@");
              if (q) {
                  int n=0; *q='\0'; p=q+2; while (isdigit(*p) || *p=='.') { n++; p++; }
                  if (n>sizeof(selcode)-1) n=sizeof(selcode)-1; memmove(selcode,q+2,n); selcode[n]='\0'; selparm=n;
              }
              else {
                q=strchr(p,'@');
                if (q) {
                  q=strstr(q+1,"=\""); if (q) q=strchr(q+2,'"');
                  if (q) {
                    char *qb;
                    int n;
                    p=strchr(p,'@'); *p++='\0'; q=p; for (; *p; p++) if (*p==' ') if (*(p+1)=='=') { qb=p-1; *p='\0'; p++; break; }
                    n=strlen(q); for (; *qb != '"'; qb--) n--;
                    if (n>sizeof(selattr)-1) n=sizeof(selattr)-1; memmove(selattr,q,n); selattr[n]='\0'; selsize=n;
                  }
                }
              }//end selection
              if (!q) for (; *p; p++) if (*p==' ' || *p=='=') { *p='\0'; p++; break; } //no selattr selparm
              while (isspace(*p) || *p=='=') p++;
              if (!isdigit(*p)) fatal("fldupdat/procx/Gmarx/=tag non numeric");
              for (xtag=0; isdigit(*p); p++ ) xtag=xtag*10+((int)(*p)-(int)'0');
              if (xtag < 1) fatal("fldupdat/procx/Gmarx/=tag lt 1");
              vxtag[ipath]=xtag; ipath++;
              while (isspace(*p)) p++;
              while (*p=='/') {
                  if (*(p+1)=='/') { *p='\0'; break; }  //comment
                  if (strncmp(p,"/file="  ,6)==0)                               { p+=6; filepath=p; while (*p) if (isspace(*p)) break; else p++; if (*p) *p++='\0';
                                                                                  for (q=p-2; q!=filepath; q--) { if (*q=='/' || *q=='\\') break;
                                                                                                                  if (*q=='.') { *q++='\0'; fileext=q; filedot="."; break; }
                                                                                                                }
                                                                                  filename=filepath;
                                                                                  if (strchr(filename,'\\')) while ((q=strchr(filename,'\\')) != NULL) filename=q+1;
                                                                                  if (strchr(filename,'/' )) while ((q=strchr(filename,'/' )) != NULL) filename=q+1;
                                                                                  if (filename==filepath) filepath="";
                                                                                  else { q=filename-1; filechar=(*q=='/')?"/":"\\"; *q='\0'; }
                                                                                  parmfile=1;
                                                                                }
                  if (strncmp(p,"/1cc="   ,5)==0)                               { p+=5; for (n=0; isdigit(*p); p++ ) n=n*10+((int)(*p)-(int)'0'); parm1cc=n; }
                  if (strncmp(p,"/1c2="   ,5)==0)                               { p+=5; for (n=0; isdigit(*p); p++ ) n=n*10+((int)(*p)-(int)'0'); parm1c2=n; }
                  if (strncmp(p,"/occ="   ,5)==0)                               { p+=5; for (n=0; isdigit(*p); p++ ) n=n*10+((int)(*p)-(int)'0'); parmocc=n; }
                  if (strncmp(p,"/oc2="   ,5)==0)                               { p+=5; for (n=0; isdigit(*p); p++ ) n=n*10+((int)(*p)-(int)'0'); parmoc2=n; }
                  if (strncmp(p,"/tag="   ,5)==0)                               { p+=5; for (n=0; isdigit(*p); p++ ) n=n*10+((int)(*p)-(int)'0'); parmvin=n; }
                  if (strncmp(p,"/clean " ,7)==0 || strncmp(p,"/clean/" ,7)==0) { p+=7; clean=1; }
                  if (strcmp (p,"/clean"    )==0)                               { p+=6; clean=1; }
                  if (strncmp(p,"/index " ,7)==0 || strncmp(p,"/index/" ,7)==0) { p+=7; parmindex=1; xmlspec=0; colonspec=1; }  // /.@level:header,text 
                  if (strcmp (p,"/index"    )==0)                               { p+=6; parmindex=1; xmlspec=0; colonspec=1; }
                  if (strncmp(p,"/i "     ,3)==0 || strncmp(p,"/i/"     ,3)==0) { p+=3; parmindex=1; xmlspec=0; colonspec=1; }  // /.@level:header,text
                  if (strcmp (p,"/i"        )==0)                               { p+=2; parmindex=1; xmlspec=0; colonspec=1; }
                  if (strncmp(p,"/len "   ,5)==0 || strncmp(p,"/len/"   ,5)==0) { p+=5; parmxlen=1; }
                  if (strcmp (p,"/len"      )==0)                               { p+=4; parmxlen=1; }
                  if (strncmp(p,"/xml "   ,5)==0 || strncmp(p,"/xml/"   ,5)==0) { p+=5; parmxml=1; }
                  if (strcmp (p,"/xml"      )==0)                               { p+=4; parmxml=1; }
                  if (strncmp(p,"/chunch ",8)==0 || strncmp(p,"/chunch/",8)==0) { p+=8; parmx=1; chunch1=224; }
                  if (strcmp (p,"/chunch"   )==0)                               { p+=7; parmx=1; chunch1=224; }
                  if (strncmp(p,"/c "     ,3)==0 || strncmp(p,"/c/"     ,3)==0) { p+=3; parmx=1; chunch1=224; }
                  if (strcmp (p,"/c"        )==0)                               { p+=2; parmx=1; chunch1=224; }
                  if (strncmp(p,"/chunch=",8)==0 ||
                      strncmp(p,"/c="     ,3)==0)                               { if (strncmp(p,"/c="     ,3)==0) p+=3; else p+=8;
                                                                                  for (n=0; isdigit(*p); p++ ) n=n*10+((int)(*p)-(int)'0'); chunch1=n;
                                                                                  if (chunch1 < 20) chunch1=20;
                                                                                  chunch2=chunch3=0;
                                                                                  while (isspace(*p)) p++;
                                                                                  if (*p==',') {
                                                                                    int n;
                                                                                    p++; for (n=0; isdigit(*p); p++ ) n=n*10+((int)(*p)-(int)'0'); chunch2=n;
                                                                                  }
                                                                                  while (isspace(*p)) p++;
                                                                                  if (*p==',') {
                                                                                    int n;
                                                                                    p++; for (n=0; isdigit(*p); p++ ) n=n*10+((int)(*p)-(int)'0'); chunch3=n;
                                                                                  }
                                                                                }
                  while (isspace(*p)) p++;
              }
              if (*p==':') {
                p++; while (isspace(*p)) p++;
                if (*p=='/') if (*(p+1)=='/') *p='\0';   //comment
                colonspecp=p;
                colonspecq=p;
                if (strlen(p)) {
                  p=p+strlen(p)-1; while (isspace(*p)) p--; colonspecq=p;
                }
                xmlspec=0; colonspec=1;
              }
              vxpath[ipath]=NULL; vxtag[ipath]=0; //eol
              parmparse=1; // !load
            }

            /* scan parms
            */
            if (isload) {  // Gload
              p+=4;  /* skip load or marx */
              if (*p) { /* proc='Gload' */
               while (*p == '/') {      /* /<tag>/socket */
                p++;
                if (isdigit(*p)) {                                                   /* /<tag> */
                  if (*p=='0') xtagzero=1;  // allow tag /0n
                  for (xtag=0; isdigit(*p); p++ ) xtag=xtag*10+((int)(*p)-(int)'0');
                  if (xtag < 1) fatal("fldupdat/procx/Gload/tag");
                  if (*p=='/' || *p=='='); else fatal("fldupdat/procx/Gload/tag dlm");
                } else
                if (strncmp(p,"no0D=",5) == 0 || strncmp(p,"no0D/",5) == 0 ||        /* /no0D */
                    strncmp(p,"noCR=",5) == 0 || strncmp(p,"noCR/",5) == 0) {        /* /noCR */
                  p+=4;
                  parm0=1;
                } else
                if (strncmp(p,"no0A=",5) == 0 || strncmp(p,"no0A/",5) == 0 ||        /* /no0A */
                    strncmp(p,"noLF=",5) == 0 || strncmp(p,"noLF/",5) == 0) {        /* /noLF */
                  p+=4;
                  parm1=1;
                } else
                if (strncmp(p,"line=",5) == 0 || strncmp(p,"line/",5) == 0 ||        /* /line */
                    strncmp(p,"nonl=",5) == 0 || strncmp(p,"nonl/",5) == 0) {        /* /nonl */
                  p+=4;
                  parm0=parm1=1;
                } else
                if (strncmp(p,"socket=",7) == 0 || strncmp(p,"socket/",7) == 0) {    /* /socket */
                  p+=6;
                  parmsocket=1;
                } else
                if (strncmp(p,"head=",5) == 0 || strncmp(p,"head/",5) == 0) {        /* /head */
                  p+=4;
                  parmhead=1;
                } else
                if (strncmp(p,"xml=",4) == 0 || strncmp(p,"xml/",4) == 0) {          /* /xml */
                  p+=3;
                  parmcmd=1;
                } else
                if (strncmp(p,"tell=",5) == 0 || strncmp(p,"tell/",5) == 0) {        /* /tell */
                  p+=4;
                  parmcmd=2;
                } else
                if (strncmp(p,"trace=",6) == 0 || strncmp(p,"trace/",6) == 0) {      /* /trace */
                  p+=5;
                  parmcmd=3;
                } else
                fatal("fldupdat/procx/Gload[/<tag>][/nolf][/xml][/socket][/head][={<url>|<file>}]");
               } /* while /x */
              } /* proc='Gload' */
              if (*p == '=') {
                p++; if (parmsocket) urlp=p; else filnamp=p; p+=strlen(p);    /* <file> | <url> */
              
              }
            }


            /* alloc working area
            */
            nbytes=(rec_maxmfrl>MAXMFRL)?rec_maxmfrl:MAXMFRL;
            if (nbytes < MFRmfrl) nbytes=MFRmfrl;
            nbytes+=BUFSIZ;     /* buffup */ /* nbytes=MFRmfrl+MFRmfrl+BUFSIZ; */
#if CICPP
            try { xarea = (char *) new char [nbytes]; }
            catch (BAD_ALLOC) { xarea = (char *)ALLONULL; }
#else /* CICPP */
            xarea=(char *)ALLOC((ALLOPARM)nbytes);
#endif /* CICPP */
            if (xarea == (char *)ALLONULL) fatal("fldupdat/procx/Gload/socket/ALLOC");
            *xarea='\0';

            q=xarea; /* batchp */

            /* del/add command
            */
            if (isload) {
                if (!xtagzero) sprintf(q,"D%"_LD_,xtag);
                else      sprintf(q,"D*");                       q+=strlen(q);   // exec tag /0n

                sprintf(haddp=q,"H%09"_LD_" ",(xtag)?(LONGX)xtag:1L); q+=strlen(q);
                sprintf(hlenp=q,"123456789 ");                   q+=strlen(q);
            }

            hdatap=q;

	     if (parmparse) {            /* ./mx null "proc='Gsplit/marx /215 /Para =2151'" */

            /* scan, xdata and parse
            */
            int min;//=parmmin;
            int max;//=parmmax;
            int occ=parm1cc;
            int oc2=parm1c2;
            int tag=parmtag;
            char *bufp=NULL;
            int xdir,loop;
            int iocc=0,lastag=0;

            haddp=hdatap; // fldupdat() ptr

            //if (tag<0) tag=1;
            //if (!tag)  tag=1; // Gload/1  Gmarx/1

            if (occ<0) occ=0;
            if (oc2<0) oc2=0;
            if (oc2<occ) oc2=occ;

            for (xdir=0, loop=MFRnvf; loop--; xdir++) {
                int itag=DIRtag(xdir);
                int ilen=DIRlen(xdir);
                if (tag) if (itag != tag) continue;   // otherwise all tags
                if (!tag) if (itag == 2000) continue; // exclude cicgi tag
                if (itag == lastag) iocc++; else { lastag=itag; iocc=1; }
                if (occ) if (iocc < occ) continue;
                if (oc2) if (iocc > oc2) continue;
                min=parmmin;
                max=parmmax;
                if (min<1) min=1;
                if (max<1) max=nbytes-BUFSIZ;
                if (ilen < min) continue;

                if (!xbuffer) {
#if CICPP
                    try { xbuffer = (char *) new char [nbytes]; }
                    catch (BAD_ALLOC) { xbuffer = (char *)ALLONULL; }
#else /* CICPP */
                    xbuffer=(char *)ALLOC((ALLOPARM)nbytes);
#endif /* CICPP */
                    if (xbuffer == (char *)ALLONULL) fatal("fldupdat/procx/Gsplit/marx/ALLOC");
                }
                *(bufp=xbuffer)='\0';

                if (ilen) {
                    char *fldp=FIELDP(xdir);
                    bpmovemax(fldp,max,ilen,0);
                }
                if (ilen) {
                    char *fldp=FIELDP(xdir);
                    int left=(ilen<100)?ilen-1:100-1;
                    int s=0;
                    char *p,*q;
                    char k=fldp[left];
                    fldp[left]='\0'; q=strstr(fldp,"<x a=\""); fldp[left]=k;
                    if (q) {
                      for (p=fldp;       ; p++) { s++; if (*p=='>') break;          } // ancest
                      for (p=q+6; *p!='"'; p++) { if (*p==',') break; *bufp++ = *p; } // ancest,*offset.lenght
                      *bufp++='|'; *bufp++='|';
                      memmove(bufp,fldp+s,ilen-s);           bufp+=(ilen-s);
                    }
                    else {
                      fldp[left]='\0'; q=strstr(fldp,"||"); fldp[left]=k;
                      if (q) {
                        for (p=fldp;  p!=q  ; p++) { s++; } s+=2;                        // ancest
                        for (p=fldp; *p!='|'; p++) { if (*p==',') break; *bufp++ = *p; } // ancest,*offset.lenght
                        *bufp++='|'; *bufp++='|';
                        memmove(bufp,fldp+s,ilen-s);          bufp+=(ilen-s);
                      }
                      else {
                        sprintf(bufp,"%d.%d||",itag,iocc);   bufp+=strlen(bufp);
                        memmove(bufp,fldp,ilen);              bufp+=ilen;
                      }
                    }
                }
                *bufp++='\n';
                *bufp='\0';

                if (*xbuffer) {

                    // rc = mainparse ( parmcmd, xbuffer, vxpath, vxtag, hdatap, nbytes-strlen(xbuffer) );

                    char *hp=haddp;
                    //char *hpback=hp;
                    char *x=xbuffer; // input data
                    //int xbufaoff=strlen(xbuffer);
                    int lft;
                    char carea[BUFSIZ];
                    char *name=carea+2;
                    char *p;
                    char *np;
                    char *path;
                    int ipath;
                    int i;
                    char *a;  //opening ptr

                    int itag,iocc=0,lastag=0;

                    carea[0]='<'; carea[1]='/';

                    while ((a=strchr(x,'<')) != NULL) // while <element..>

                      if (strchr(a,'>')) { // opening element // if

                        char *b;  //opening end
                        char *c=NULL;  //closing ptr
                        char *d=NULL;  //closing end
                        int xb;   //opening len
                        int xn;   //element name len
                        char *ba;
                        int xx=0;
                        int xen=0;
                        int len=0;
                        int hasattr=0;
                        int closed=0;
                        //
                        char *ihtba=NULL;
                        int ihtxen=0;
                        int level=0;
                        char *indattr=" level=\"";
                        int   indsize=8,xlev;
                        //

                        for (xb=1, b=a; *b != '>'; b++) xb++;                 // <x ...>....</x>
                        if (*(b-1) == '/') closed=1;                          // <x ../>

                        for (np=name, xn=0, p=a+1, lft=xb; lft--; p++) {
                            if (*p == ' ') hasattr=1;
                            if (*p == ' ' || *p == '/' || *p == '>') break;
                            xn++; *np++= *p;
                        }
                        *np='\0';

                        for (ipath=EOF, i=0; ; i++) {
                            path=vxpath[i]; if (!path) break;
                            if (strcmp(path,name) == 0) { ipath=i; itag=vxtag[ipath]; break; }
                        }

                        x=b+1;                                                // contents
                        if (ipath == EOF) continue;        // skip element


                        if (itag == lastag) iocc++; else { lastag=itag; iocc=1; hlen=0; }
#if 01
                        if (parmocc) if (iocc < parmocc) continue;
                        if (parmoc2) if (iocc > parmoc2) continue;
#endif
                        if (*colonspecp == '-') continue;  // skip element - void output

/* bug: pega todas as ocorrencias do elemento que nao tenham atributos
                        if (selsize) if (hasattr) {
*/
                        if (selsize) {
                            int skip=1;
                            char *p;
                            for (p=a; p!=b; p++) {
                                if (*p == ' ') {
                                    p++; // space
                                    if (strncmp(p,selattr,selsize)==0) {
                                        char c=p[selsize];
                                        if (c == ' ' || c == '/' || c == '>') { skip=0; break; }
                                    }
                                }
                            }
                            if (skip) continue;            // skip element
                        }

                        if (closed) {                                         // <x ../>
                            for (p=a; ; p++) { len++; if (p==b) break; }
                        }
                        else {                                                // <x ..>....</x>
                        //
                            char *q,oarea[BUFSIZ];
                            int oxn;
                        //
                            int empty=0; //(c==x)?1:0;
                            //
                            level=0;
                            //
                            strcat(carea,">"); c=strstr(x,carea);
                            if (c) d=strchr(c,'>');
                            //
                            //
                            if (parmindex) if (c) { //it's well formed
                                char *op=NULL;
                                char *q=b;       //hierarchal index stops at b instead of c
                                oxn=1+xn; oarea[0]='<'; memmove(oarea+1,name,xn); oarea[oxn]='\0';
                                *c='\0'; q=strstr(q+1,oarea); *c='<';                                                  // <name
                                if (q) if (q[oxn]==' ' || q[oxn]=='>') { op=q; for (; q!=c; q++) if (*q=='>') break; } // <name ..>
                                if (op) {
                                  c=op;   //dlm as if </index>
                                  d=op-1; //set forward x=d+1
                                }
                            }

                        //
                        // <index level=1> <index level=2> </index><index level=2><index level=3></index></index><index level=2></index></index>
                        //                 op
                        // a             b                 c      d                                                                      c     d

                        // <index level=1> <index level=2> 2A </index> <index level=2> 2B</index> <index level=2> 2C </index> </index>
                        //                 op
                        // a             b                    c      d                   c      d                    c      d C      D
                        //
                                                                              // a     b    c  d
                                if (parmindex) {
                                    if (!iphbuff) {
#if CICPP
                                        try { iphbuff = (char *) new char [nbytes]; }
                                        catch (BAD_ALLOC) { iphbuff = (char *)ALLONULL; }
#else /* CICPP */
                                        iphbuff=(char *)ALLOC((ALLOPARM)nbytes);
#endif /* CICPP */
                                        if (iphbuff == (char *)ALLONULL) fatal("fldupdat/procx/Gsplit/marx/ALLOC");
                                    }
                                    if (!ihtbuff) {
#if CICPP
                                        try { ihtbuff = (char *) new char [nbytes]; }
                                        catch (BAD_ALLOC) { ihtbuff = (char *)ALLONULL; }
#else /* CICPP */
                                        ihtbuff=(char *)ALLOC((ALLOPARM)nbytes);
#endif /* CICPP */
                                        if (ihtbuff == (char *)ALLONULL) fatal("fldupdat/procx/Gsplit/marx/ALLOC");
                                        *ihtbuff='\0'; for (xlev=0; xlev<21; xlev++) { ihtptr[xlev]=ihtbuff; ihtlen[xlev]=0; ihtocc[xlev]=0; ihtpta[xlev]=NULL; ihtpla[xlev]=0; ihtptc[xlev]=NULL; ihtptd[xlev]=NULL; }
                                    }
                                    level=0; ihtxen=0; xen=0;
                                    if (hasattr) for (p=a+1; p!=b; p++) if (*p == ' ') if (strncmp(p,indattr,indsize)==0) {                   //.level="
                                      for (p+=indsize; isdigit(*p); p++ ) level=level*10+((int)(*p)-(int)'0'); break;
                                    }
                                    if (level>=1 && level<21) {
                                      //header
                                      char *a1,*b1=NULL,*c1=NULL;
                                      char *a2,*b2=NULL,*c2=NULL;
                                      char *oarea1="<header",*carea1="</header>";
                                      char *oarea2="<text",*carea2="</text>";
                                      int oxn1=7,oxn2=5;
                                      //
                                      //header
                                      *c='\0'; a1=strstr(b+1,oarea1);  if (a1) if (a1[oxn1]==' ' || a1[oxn1]=='>') b1=strchr(a1,'>'); *c='<'; // <header..>
                                      if (b1) { c1=strstr(b1+1,carea1); }                                                                     // </header>
                                      if (c1) {
                                        int n;
                                        char *ihtp=ihtptr[level]+ihtlen[level]+1;
                                        //copy header
                                        if (level>1) *ihtp++=' ';
                                        *c1='\0'; n=strlen(b1+1); *c1='<'; memmove(ihtp,b1+1,n); ihtptr[level]=ihtp; ihtp+=n; ihtlen[level]=n; *ihtp='\0';
                                        ihtocc[level]=iocc;
                                        ihtpta[level]=a; ihtpla[level]=b-a+1; if (closed) ihtpla[level]--;
                                        ihtptc[level]=c; ihtptd[level]=d;
                                   /**/ for (xlev=level+1; xlev<21; xlev++) { ihtptr[xlev]=ihtp; ihtlen[xlev]=0; ihtocc[xlev]=0; ihtpta[xlev]=NULL; ihtpla[xlev]=0; ihtptc[xlev]=NULL; ihtptd[xlev]=NULL; }
                                        //
                                        //text
                                        *c='\0'; a2=strstr(b+1,oarea2);  if (a2) if (a2[oxn2]==' ' || a2[oxn2]=='>') b2=strchr(a2,'>'); *c='<'; // <text..>
                                        if (b2) { c2=strstr(b2+1,carea2); }                                                                     // </text>
                                        if (c2) { *c2='\0';  ihtxen=strlen(b2+1); *c2='<'; ihtba=b2+1; }
                                      }//c1
                                    }//level
                                    //ihtmove(p,max,len,xleft)
                                }//end parmindex
                            else // nested
                            if (d) {
                              q=b;       //nested elements in b+1..c-1
                              oxn=1+xn; oarea[0]='<'; memmove(oarea+1,name,xn); oarea[oxn]='\0';
                              while (d) {  //while following closing and nested opening
                                char *op=NULL;
                                *c='\0'; q=strstr(q+1,oarea); *c='<';                                                  // <name
                                if (q) if (q[oxn]==' ' || q[oxn]=='>') { op=q; for (; q!=c; q++) if (*q=='>') break; } // <name ..>
                                if (op) {                       //nested opening
                                  c=strstr(d+1,carea); d=NULL;  //following c                                          // </name>
                                  if (c) d=strchr(c,'>');       //following d
                                }
                                else break;
                              }//end while following closing and nested opening
                            }//d

                            if (selparm) {
                              int skip=1;
                              char *p;
                              if (!parmindex) {
                                level=0; ihtxen=0; xen=0;
                                if (hasattr) for (p=a+1; p!=b; p++) if (*p == ' ') if (strncmp(p,indattr,indsize)==0) {                   //.level="
                                  for (p+=indsize; isdigit(*p); p++ ) level=level*10+((int)(*p)-(int)'0'); break;
                                }
                                if (level>=1 && level<21) ihtocc[level]=iocc;
                              }
                              if (level>=1 && level<21) {
                                for (p=selcodearea, xlev=1; xlev<=level; xlev++) { sprintf(p,"%d.",ihtocc[xlev]); p+=strlen(p); } *p++='\0';
                                if (strcmp(selcodearea,selcode) == 0) skip=0;  // get this element - code
                              }
                              if (skip) continue;                              // skip element - code
                            }//end selparm

                            if (d) if (!parmindex) {
                              char *bc=c;
                              //remove CDATA marks
                              int parmy=clean;
                              if (parmy) {
                                    char *p,*q;
                                    char k= *c; *c='\0';
                                    p=b+1; while ((q=strstr(p,"<![CDATA[")) !=NULL) { memset(q,(int)' ',9); p=q+9; }
                                    p=b+1; while ((q=strstr(p,"]]>"      )) != NULL) { memset(q,(int)' ',3); p=q+3; }
                                    p=b+1; while ((q=strchr(p,'<')) != NULL) if ((p=strchr(q,'>')) == NULL) { *q=' '; p=q+1; } else for (*p=' '; p!=q; ) *--p=' ';
                                    *c=k;
                              } // parmy
                              for (p=a;  ; p++) { len++; if (p==d)  break; }
                              for (ba=b+1; ba!=c  && isspace(*ba); ) { ba++; xx++; }
                              if (ba==c)  empty=1; else for (bc=c-1; bc!=ba && isspace(*bc); ) { bc--; xx++; }
                              if (ba==bc) empty=1;
                              if (!empty) {
                                for (p=ba; ; p++) { xen++; if (p==bc) break; }
                                if (clean) {
                                    int left=xen,n=0;
                                    char *p=ba,*q=ba,x='\0';
                                    while (left--)
                                      if (x==' ' && *p==' ') { p++; n++; }
                                      else
                                      if (x=='.' && *p=='.') { p++; n++; }
                                      else
                                      { x= *p; if (p == q) { p++; q++; } else *q++ = *p++; }
                                    xen-=n;
                                }//clean
                                //
                              }//!empty
                            }//d
                        }//!closed

                        while (xmlspec || hasattr || xen || ihtxen) {
#if 01
                            if (xmlspec || hasattr || xen >= min || ihtxen >= min) {
#endif
                                char *hdatp;
                                LONGX litag=(parmvin)?parmvin:itag;
                                if (ihtxen) {
                                    //int xlev;
                                    int xe;
                                    char *iphp,*c2,k;
                                    char *pp;   //par1 a
                                    int qpcase; //case
//
                                    //execute /index@level:header,text
                                    ba=iphbuff;
                                    //copy headers
                                    for (iphp=iphbuff, xlev=1; xlev<=level; xlev++) {
                                      if (ihtlen[xlev]) { memmove(iphp,ihtptr[xlev],ihtlen[xlev]); iphp+=ihtlen[xlev]; } else *iphp++='-';
                                    //*iphp++='.'; *iphp++=' ';
                                      *iphp++='|';
                                    }
                                    *iphp++='|';
                                    *iphp='\0'; xen=strlen(iphbuff);
                                    //copy text
                                    c2=ihtba+ihtxen; k= *c2; *c2='\0'; //skip headers, search for .space

                                    p=strstr(pp=ihtba,". ");           // Par1.   Par2.
                                    while (isspace(*pp)) pp++;         // Par1.   Par2.
                                    qpcase=0;                          // Par1.   Par2.
                                    while (p) {                        // pp  p   qq  pq
                                                                       // Aetc.   Aetc.
                                                                       // Aetc.   12. Aetc
                                      char *q;
                                      char *qq; //par2 a
                                      char *pq; //par2 b
                                      int n=0;  //par1 lenght excluding .
                                      int s=0;  //spaces before par2
                                      int m=0;  //par2 lenght excluding .
                                      int v=0;  //par2 starts with digits
                                      int w=0;  //par2 is all digits, period and upper case letter
//
                                      for (q=pp;  q!=p;        q++) n++;
                                      for (q=p+1; isspace(*q); q++) s++;
                                      pq=strstr(qq=q,". ");
                                      qpcase=0;
                                      if (pq) qpcase=2; else for (; *q; q++) if (*q=='.' && !*(q+1)) { pq=q; qpcase=1; }
                                      if (pq) for (q=p+1+s; q!=pq;       q++) m++;
                                      if (pq) for (q=p+1+s; isdigit(*q); q++) v++;
                                      if (v) if (*q=='.' && *(q+1)==' ') { q+=2; while (isspace(*q)) q++; if (isupper(*q)) w=v; }
//
                                      if (n<=3) {                      //par1 is too short - 1.
                                         pp=qq; p=pq;
                                         continue;
                                      }
                                      if (w && w<=2) {                 //par2 is 12. Aetc
                                         break;
                                      }
                                      if (m) if (!isupper(*qq)) {      //par2 not Aetc     - E. coli
                                         pp=qq; p=pq;
                                         continue;
                                      }
                                      if (m<=3) {                      //par2 is too short - Tard A. O.
                                         pp=qq; p=pq;
                                         continue;
                                      }
                                      break;           //otherwise
                                    }
                                    *c2=k;
#if 1
                                    if (p) { *(p+1)='\0'; xe=strlen(ihtba); *(p+1)=' '; memmove(iphp,ihtba,xe);    ihtba+=(xe+1);  ihtxen-=(xe+1); }
                                    else   {              xe=ihtxen;                    memmove(iphp,ihtba,xe);    ihtba=NULL;     ihtxen=0;       }
#else
                                    if (p) {
                                      if (qpcase == 2)
                                           { *(p+1)='\0'; xe=strlen(ihtba); *(p+1)=' '; memmove(iphp,ihtba,xe);    ihtba+=(xe+1);  ihtxen-=(xe+1); }
                                      else
                                      if (qpcase == 1)
                                           {              xe=strlen(ihtba);             memmove(iphp,ihtba,xe);    ihtba+=(xe  );  ihtxen-=(xe  ); }
                                      else
                                           {              xe=ihtxen;                    memmove(iphp,ihtba,xe);    ihtba=NULL;     ihtxen=0;       }
                                    }
                                    else   {              xe=ihtxen;                    memmove(iphp,ihtba,xe);    ihtba=NULL;     ihtxen=0;       }
#endif
                                    if (xe) { //
                                      char *pa=iphp,*pc=iphp+xe;
                                      for (; pa!=pc && isspace(*pa); ) { pa++; xe--; }
                                      if (pa==pc) xe=0; else for (; pc!=pa && isspace(*pc); ) { pc--; xe--; }
                                      if (pa==pc) xe=0;
                                      if (xe) {
                                            int left=xe,n=0;
                                            char *p=pa,*q=iphbuff+xen,x='\0';
                                            while (left--)
                                              if (x==' ' && *p==' ') { p++; n++; }
                                              else
                                              if (x=='.' && *p=='.') { p++; n++; }
                                              else
                                              { x= *p; if (p == q) { p++; q++; } else *q++ = *p++; }
                                            xe-=n;
                                      }//!empty
                                    }//xe
                                    xen+=xe;
                                    if (!xe) xen=0; // no text to display
                                }//end if ihtxen
                                if (parmindex) if (!xen) break;

                                if (colonspec) {
                                    if (parmx) {
                                      if (chunch2 < 6 || chunch2>=chunch1) chunch2=chunch1/2;
                                      if (chunch3 < 6 || chunch3>=chunch1) chunch3=chunch1/3;
                                      max=chunch1+chunch2;
                                      if (hlen) {
                                        if (closed) { if (hasattr)              hlen=0; }
                                        else {        int xe=0;
                                                      char *xep=ba;
                                                      while (isdigit(*xep) || *xep=='.') { xep++; xe++; }
                                                      if (xe>1) if (*xep==' ')  hlen=0;   //    - / n.n.
                                                      if (xen      >  max    )  hlen=0;   //336 - / 224...112
                                                      if (xen      >  chunch1)  hlen=0;   //224 - / xen
                                        }
                                        if (hlen) {
                                          *hp++=' '; *hp++=' ';
                                        }
                                      }
                                      if (hlen==0) {
                                        hlenp=hp+11; sprintf(hp,"H%09"_LD_" 123456789 ",litag); hp+=strlen(hp); hdatp=hp;
                                        if (parmfile) { sprintf(hp,"%s ",filename);                         hp+=strlen(hp); }
                                        sprintf(hp,"%d %d: ",itag,iocc);                                    hp+=strlen(hp);
                                        if (level) { //headers ptrs/code
                                            sprintf(hp,"%d",ihtocc[1]); hp+=strlen(hp); for (xlev=2; xlev<=level; xlev++) { sprintf(hp,".%d",ihtocc[xlev]); hp+=strlen(hp); }
                                            sprintf(hp,"||");                                               hp+=strlen(hp);
                                        }
                                        bpmovemax(ba,max,xen,1);  //336  //224
                                      }
                                      max+=3;
                                    }
                                    else {  //parse/index !parmx
                                      hlenp=hp+11; sprintf(hp,"H%09"_LD_" 123456789 ",litag); hp+=strlen(hp); hdatp=hp;
                                      //if (parmfile) { sprintf(hp,"%s%s%s%s%s ",filepath,filechar,filename,filedot,fileext);  hp+=strlen(hp); }
                                      if (parmfile) { sprintf(hp,"%s ",filename);                         hp+=strlen(hp); }
                                      for (p=xbuffer; ; p++) { if (*p=='|') break; *hp++ = *p; }  // ancestor
                                      if (parmxlen) { sprintf(hp," %d.%d, %d,%d,%d,%d||",itag,iocc,xn,xb,xen,len);          hp+=strlen(hp); }
                                      else          { sprintf(hp," %d.%d||"             ,itag,iocc              );          hp+=strlen(hp); }
                                      if (level) { //headers ptrs/code
                                            sprintf(hp,"%d",ihtocc[1]); hp+=strlen(hp); for (xlev=2; xlev<=level; xlev++) { sprintf(hp,".%d",ihtocc[xlev]); hp+=strlen(hp); }
                                            sprintf(hp,"||");                                               hp+=strlen(hp);
                                      }
                                    } //end parmx
                                } else { //xmlspec/!colonspec
                                    hlenp=hp+11; sprintf(hp,"H%09"_LD_" 123456789 ",litag); hp+=strlen(hp); hdatp=hp;
                                    if (parmxml) if (!xmlopened) {
                                      sprintf(hp,"<?xml version=\"1.0\" encoding=\"iso-8859-1\" ?>\n");     hp+=strlen(hp);
                                      sprintf(hp,"<marx file=\"%s\" type=\"%s%s\" path=\"%s%s\">\n",filename,filedot,fileext,filepath,filechar);  hp+=strlen(hp);
                                      xmlopened=1;
                                    }
                                    sprintf(hp,"<x a=\"");                                                  hp+=strlen(hp);
                                    for (p=xbuffer; ; p++) { if (*p=='|') break; *hp++ = *p; } // ancestor
                                    sprintf(hp," %d.%d\"",itag,iocc);                                      hp+=strlen(hp);
                                    if (parmxlen) { sprintf(hp," b=\"%d,%d,%d,%d\"",xn,xb,xen,len);         hp+=strlen(hp); }
                                    if (level) { //headers ptrs/code
                                          sprintf(hp," c=\"");                                              hp+=strlen(hp);
                                          sprintf(hp,"%d",ihtocc[1]); hp+=strlen(hp); for (xlev=2; xlev<=level; xlev++) { sprintf(hp,".%d",ihtocc[xlev]); hp+=strlen(hp); }
                                          sprintf(hp,"\"");                                                 hp+=strlen(hp);
                                    }
                                    sprintf(hp,"/>"); hp+=strlen(hp);
                                }

                                if (colonspec) {
                                  if (*colonspecp) {
                                    for (q=colonspecp; *q; q++) {
                                      if (*q == '&') {
                                        char *p=q+1;
                                        char *m;
                                        if (isalpha(*p) || isdigit(*p)) {   //&name - attr char1
                                          char bb[BUFSIZ];
                                          char *aap=NULL;
                                          char *aaq=NULL;
                                          char k;
                                          for (m=bb, *m=' '; isalpha(*p) || isdigit(*p); p++) { *m++= *p; q++; } *m++='='; *m++='"'; *m='\0';  //.attrname="
                                          k= *b; *b='\0'; aap=strstr(a+1,bb); if (aap) aaq=strchr(aap+=strlen(bb),'"'); *b=k;
                                          if (aaq) while (aap != aaq) *hp++ = *aap++;           //output attr contents
                                        }
                                        else {                              //& - element name, attrs, contents
                                          for (  p=a+1,      m=p+xn;          p != m; p++) *hp++= *p;             //output element name
                                          if (hasattr) { *hp++=' ';                                               //output
                                            if (closed) for (p=a+1+xn+1, m=b-1/*closed*/; p != m; p++) *hp++= *p; //output all attrs
                                            else        for (p=a+1+xn+1, m=b-0/*      */; p != m; p++) *hp++= *p; //output all attrs
                                          }
                                          if (xen) {
                                            *hp++=' ';                                                            //output
                                            //for (p=ba; xen--; p++) *hp++= *p;                                   //output all contents //
                                            hpmovemax(hp,max,ba,xen);                                             //output all contents //
                                          }
                                        }// end if char1
                                      } else {                              //. - spec char
                                        *hp++ = *q;                                                               //output spec char
                                      }//& or .
                                      if (q==colonspecq) break;
                                    }//end for
                                  } //colonspecp
                                  else { //colonspecp                       //: - contents
                                      if (closed) { xen=len-3; ba=a+1; }
                                      hpmovemax(hp,max,ba,xen);                                                   //: - output contents
                                  } //colonspec
                                } else { //colonspec
                                    hpmovemax(hp,max,a,len);                                                      //: - output xml
                                } //colonspec

                                if (xmlopened) {
                                  sprintf(hp,"</marx>\n");  hp+=strlen(hp);
                                }
                                //null hp string
                                *hp='\0'; haddp=hp;

                                //actual len
                                hlen=strlen(hdatp); sprintf(hlenp,"%9"_LD_,(LONGX)hlen); hlenp[9]=' ';

                                if (parmx) {
                                        if (  hlen      >= chunch1         )    hlen=0;   //224 - /
                                        if (!closed) if ( xen > chunch3 ) {               //75  - 75./
                                                          if ( ba[xen-1]=='.' ) hlen=0;
                                                          if ( ba[xen-1]==';' ) hlen=0;
                                        }
                                } // parmx

                                //move forward ok
                                if (d) x=d+1;

                            } // end if xmlspec || hasattr || xen >= min || ihtxen >= min
                            // else continues from x=b+1

                            if (!ihtxen) break;

                        } // end while xmlspec || hasattr ||xen || ihtxen


                      } else { // else while <element..>

                        x++;

                      } // fi while <element..>

                } // *xbuffer
            } // xdir
	     } // parmparse

         else  /* else parmparse */

	     if (parmsocket) {            /* ./mx null "proc='Gload/socket=http://www.bireme.br:80/'" */

            /* scan <url>
            */
            protocol="HTTP";
            q=strstr(p=urlp,"://"); /* http:// */
            if (q) {
                char *x;
                int len;
                *q='\0'; len=strlen(p);
                if (len < 8) {
                  protocol=p;                                                    /* http | x1417*/
                  for (x=p; *x; x++) *x=toupper(*x);
                  p=q+3;
                }
                else *q=':';
            }

            serverhost=p;                                                       /* www.bireme.br | serverofi.bireme.br */

            pathp=strchr(p+1,'/');                                           /* /path | /dbname/parms */
            if (pathp) *pathp='\0';
            else pathp="/";

            port=80;                          /* :80 */
            q=strchr(p,':');
            if (q) {
                *q='\0';
                if (sscanf(q+1,"%d",&xport) == 1) port=xport;                   /* :80 | :1417 */
                else xport=0; /*fatal("fldupdat/procx/Gload/socket/port");*/
            }

            /* X1417://serverofi.bireme.br:1417/msg
                                               /dbname/control/maxmfn
                                               /dbname/text/mfn/mfn2/count/step
                                               /dbname/dict/key/key2/contains/count
                                               /dbname/bool/expression
                                               /dbname/rank/text
            */
            if (strcmp(protocol,"X1417") == 0) {
                maxrds=1;                                                        /* single read */
                if (!xport) port=1417;     /* :1417 */
            }

            msgp=hdatap;
            *msgp='\0';
            if (strcmp(protocol,"X1417") == 0) {
                sprintf(msgp,"%s\n",pathp+1);
            }
            else
            if (strcmp(protocol,"HTTP") == 0) {
                sprintf(msgp,"%s /%s HTTP/1.0\r\n\r\n",(parmhead)?"HEAD":"GET",pathp+1);
            }

            if (*msgp) rc = mainclient ( parmcmd, protocol, serverhost, port, msgp, hdatap, nbytes-strlen(xarea), maxrds);

	     }


//#define DIR_FILE 1                  /* list=directory */
//#if DIR_FILE
//#include <sys/types.h>
//#include <dirent.h>
//#endif
    /* ............................................... directory listing */
#if DIR_FILE
         else   /* else parmsocket: <file> */
             if (strncmp(filnamp,"dir=",4)==0) {
                DIR *direntdp=NULL;
                struct dirent *direntep;
                char *directory=filnamp+4;
                rc=0;
                direntdp = opendir (directory);
                if (direntdp != NULL) {
                    int n;
                    while ((direntep = readdir (direntdp)) != NULL) {
                        strcpy(hdatap,direntep->d_name); 
                        n=strlen(direntep->d_name); *(hdatap+=n)='\n'; *++hdatap='\0'; rc+=(n+1);
                    }
                    (void) closedir (direntdp);
                }
                //else fatal("Couldn't open the directory");
             }   
#endif

         else { /* else parmsocket: <file> */

             rc = mainfile ( parmcmd, filnamp, parm0, parm1, hdatap, nbytes-strlen(xarea) );

	     } /* end parmsocket */



        if (isload) {
            if (rc < 0) {
                sprintf(hdatap,"Gload%s error %d",parmsocket?"/socket":"",(-rc));
                hlen=strlen(hdatap);
            }
            else hlen=rc;

            if (hlen) {
                /* actual add lenght */
                sprintf(hlenp,"%9"_LD_,(LONGX)hlen); hlenp[9]=' ';
                /* print */
                //if (parmprint) fprintf(parmfp,"%s",hdatap);
            }
            else if (haddp) *haddp='\0';
        }

        //else printf("%s\n",xarea);

#if CICPP
        if (recp->xfldupdat(xarea)) fatal(errp);
        delete xarea;
        delete xbuffer;
#else /* CICPP */
//printf("antes do fldupdat\n");
        if (fldupdat(irec,xarea)) fatal(errp);
//printf("depois do fldupdat\n");
        if (xarea) FREE(xarea);
        if (xbuffer) FREE(xbuffer);
#endif /* CICPP */

        return NULL; //isgdump=1;

        }  /* end Gload/socket */
        else

#endif /* PROCXSOCKREC */
/* End PROCXSOCKREC
*/

#if INDEXAT
                                            for (xlev=1; xlev<=level; xlev++) {
                                              char *p=ihtpta[xlev]+1;
                                              int n=ihtpla[xlev]-1;
                                              *hp++='/';
                                              while (n) if (*p==' ' || *p=='/' || *p=='>') break; else { *hp++ = *p++; n--; }
                                              if (*p==' ') { p++; n--; }
                                              *hp++='@';
                                              n--; if (closed) n--;
                                              memmove(hp,p,n); hp+=n;
                                              *hp++='|';
                                            }
#endif

