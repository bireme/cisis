/* ------------------------------ serc.c ---------------------------------- */


/* Copyright (c) 1990 - 1994 Adalberto Otranto Tardelli. All rights reserved.
 * Written by A.O.Tardelli 7/90
 * Redistribution and use in source and binary forms are freely permitted
 * provided that the above copyright notice and attribution and date of work
 * and this paragraph are duplicated in all such forms.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 * This program performs a MicroISIS master file dump, formatted display,
 * search and record limits, pattern changes, data base join, field update,
 * master file update and ISO-2709 processing. Input can also be a plain file.
 * A description of this program is available in CISIS Interface Application -
 * MX Program. Sao Paulo, BIREME - Latin American and Caribbean Center on
 * Health Sciences Information/PAHO-WHO, April 1991. 12p. See also:
 * - http://www.bireme.br (IsisFamily/CisisUtilities)
 * - ftp://ftp.bireme.br/cisis
 * Currently works with CISIS Interface 4.01 and BorlandC 5.02 or
 * MSVC 6.0 for win32/console application, and for Linux/gcc, HPUX/CC and
 * Sun/CC, among other platforms.  AOT, 21/02/2001.
*/

/*
..Client program that makes a connection for a byte stream socket in the Internet namespace.
*/
/*
*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "cisis.h"      /* CISIS Interface header file */
#include "cirun.h"      /* CISIS Interface runtime declarations */

#define PORT            1417
#define MESSAGE         "wtrig2 ... text=..."
#define SERVERHOST      "serverofi.bireme.br"


#define TAG1   1
#define TAG2   2
#define TAG23  2

#define FATRAP__WT 1

void serc_syntax(void);
void serc_syntax()
{
}

#if MX_SERX
int
mainserx(int argc, char *argv[])
#else //MX_SERX
int
main(int argc, char *argv[])
#endif //MX_SERX
{
    RECSTRU *recp; /* mandatory for defines REC RDB MF0 MFR DIR, FIELDP */
    LONGX irec=0;

    LONGX maxmfrl=MAXMFRL;   // default

    char serverhost[CIMPL]; //=SERVERHOST;
    uint16_t port=PORT;
    LONGX xval;

    char *wwwcgip=NULL;     // allocated in cicgi0
    char cipfilearea[CIMPL+1];
#if MX_SERX
    char *cipfile="mx.cip";
#else //MX_SERX
    char *cipfile="serx.cip";
#endif //MX_SERX

    char *parmwtmsg=NULL;   // wtrig2 command line - calling argument wtrig2
    char *wtmsg=NULL;   // wtrig2 command line - allocated if not parmwtmsg

    char *parmhtmsg=NULL;   // http command line - calling argument http
    char *htmsg=NULL;       // http command line - allocated if not parmhtmsg

    char *parmxtmsg=NULL;   // xttt command line - calling argument xttt
    char *xtmsg=NULL;       // xttt command line - allocated if not parmxtmsg

    int debug=0;
    char force[BUFSIZ+CIMPL] = { '\0' };
    int outfmthtml=0;

    char *txtvp="";        // text parameter
    int txtvlen=0;         // text parameter
    char *txt1vp="";        // text1 parameter
    int txt1vlen=0;         // text1 parameter

    char *p,*q,*x;
    char *hhp;
//    char *hdatap;

    int len,y;
    char *np,*vp;
    int nlen,vlen;
    int npairs; //=0;

    char *buffer;//=NULL    // reply from mainserver/wtrig2
    int buffersize;

    char *cgimsg=NULL;      // reply from mainserver/wtrig2
    LONGX cgimsgsize;

    char *databuff;//=NULL    // reply from mainserver/wtrig2
    int databuffsize;

    char *textbuff;//=NULL    // reply from mainserver/wtrig2
    int textbuffsize;
    char *tbp;

    char *pcmdp;
    char *procp;
    char *pftxp;

    LONGX dbmfn=1;
    int dbtag=0;
    int dbocc=0;
    int dboc2=0;
    char dbname[CIMPL+1] = { '\0' };

    int rc; //=0;
    int i;

    int mustclose=0;

    time_t secs_now;
    time_t secs_start,secs_end;
    struct tm *tp;

#if FATRAP__WT
    int     z_xerrno;
    int     z_errcod;
    char    z_iomsg[MAXFATML+1];
    jmp_buf z_jumper;
    int     z_jumprc;
#endif

#if FATRAP__WT
     /* save/notrap */
     strcpy(z_iomsg,fatal_iomsg); memmove(z_jumper,fatal_jumper,sizeof(fatal_jumper)); z_errcod=fatal_errcod;
     z_xerrno=dbxopt_errno;
     /* trap cisis fatal() */
     strcpy(fatal_iomsg,"trap");
     /* disable dbxopen msg */
     dbxopt_errno=0;
     z_jumprc = setjmp(fatal_jumper);
     if (z_jumprc != 0) {
         /* error */
         if (outfmthtml) {
             printf("error type=\"fatal\" in=\"serx\" code=\"%d\" error=\"%s\"\n",fatal_errcod,fatal_iomsg);
             if (mustclose) printf("</pre></body></html>\n");
         }
         else {
             printf("<error type=\"fatal\" in=\"serx\" code=\"%d\">%s</error>\n",fatal_errcod,fatal_iomsg);
             if (mustclose) {
                 printf("</request>\n");
                 printf("</serc>\n");
             }
         }
         /* restore/notrap */  //strcpy(fatal_iomsg,"");
    	 memmove(fatal_jumper,z_jumper,sizeof(fatal_jumper)); strcpy(fatal_iomsg,z_iomsg);
         fatal_errcod=z_errcod;
         dbxopt_errno=z_xerrno;
         return (-1);
     }

#endif

    /* init
    */
    strcpy(serverhost,SERVERHOST);

    /* get secs_ for elapsed */
    secs_start=time(&secs_now);
    tp=localtime(&secs_start);

    /* go
    */
    for (i=1; i < argc; i++) {
        p=argv[i];
        if (strcmp(p,"hello") == 0) {
            //serc_syntax();
            printf("%s",cicopyr("Utility SERC"));
            printf("\n");
            printf("serc [hello] [debug] [cipar=<file>] \n");
            printf("     [port=1417|<port>] \n");
            printf("     [host=localhost|<host>]  \n");
 //                                    [^c<file>]
            printf("     \"wtrig2 c=col[^p<port>] [maxrel=n] [minsim=0.n] [text1=x] [text=x]\" \n");
            printf("     [db=x] [mfn=n] [tag=n] [occ=n] [oc2=n] [proc.n=text] [pft.n=text] [pcmd.n=text] \n");
            printf("\n");
            exit(1);
        }
        if (strncmp(p,"force=",6) == 0) {
            int len=strlen(force);
            if (len) force[len]='&';
            else     strcpy(force,"QUERY_STRING=\"");
            strcat(force,p+6);
            strcat(force,"\"");
            continue;
        }
        if (strcmp(p,"debug") == 0) {
            debug=1;
            continue;
        }
        if (strncmp(p,"cipar=",6) == 0) {
            cipfile=p+6;
            continue;
        }
        if (strncmp(p,"port=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&xval) != 1) fatal(p);
            port=(uint16_t)xval;
            continue;
        }
        if (strncmp(p,"host=",5) == 0) {
            if (!*(p+5)) fatal(p);
            strcpy(serverhost,p+5);
            continue;
        }
        if (strncmp(p,"wtrig2 ",6) == 0) {
            parmwtmsg=p;
            continue;
        }
        if (strncmp(p,"xttp ",5) == 0) {
            parmxtmsg=p;
            continue;
        }
        parmhtmsg=p;
        //if (strncmp(p,"http ",5) == 0) {
        //    continue;
        //}
        //printf("Content-type: text/plain\n\n");
        //fatal(p);
    }

    if (*force) {
        putenv("REQUEST_METHOD=GET");
        putenv(force);
    }


    /* get CGI data
    */
    p=getenv("REQUEST_METHOD"); if (debug) if (p) printf("<debug getenv=\"REQUEST_METHOD\"><![CDATA[ %s ]]></debug>\n",p);
    q=getenv("QUERY_STRING");   if (debug) if (q) printf("<debug getenv=\"QUERY_STRING\"><![CDATA[ %s ]]></debug>\n",q);
    if (p) cicgi0(NULL, NULL, &wwwcgip, (UWORD)2000, "tag");  // H1 10 ^ndbn^vcdsH1 10 ^ncount^v2H1 7 ^nnow^vH1 10 ^nbtell^v0H1 20 ^nbool^vplants*waterH1 15 ^npft^vmfn/v24/
    if (debug) if (!wwwcgip) printf("+++ wwwcgip=%s<br />\n","void");

    //internal parms - execute
    if (wwwcgip) {
        np=strstr(wwwcgip,"^ndebug^v");
        if (np) {
            for (xval=0,p=np+9; *p && isdigit(*p); p++) xval=xval*10+(int)(*p)-(int)'0';
            debug=(int)xval;
        }

        np=strstr(wwwcgip,"^ncipar^v");
        if (np) {
            for (vlen=0,vp=p=np+9; *p; p++) if (strncmp(p,"H2000 ",6) == 0) break; else vlen++;
            if (vlen) { char c; if (vlen>CIMPL) vlen=CIMPL; c=vp[vlen]; vp[vlen]='\0'; strcpy(cipfile=cipfilearea,vp); vp[vlen]=c; }
        }

        np=strstr(wwwcgip,"^nport^v");
        if (np) {
            for (xval=0,p=np+8; *p && isdigit(*p); p++) xval=xval*10+(int)(*p)-(int)'0';
            port=(uint16_t)xval;
        }
        np=strstr(wwwcgip,"^nhost^v");
        if (np) {
            for (vlen=0,vp=p=np+8; *p; p++) if (strncmp(p,"H2000 ",6) == 0) break; else vlen++;
            if (vlen) { memmove(serverhost,vp,vlen); serverhost[vlen]='\0'; }
        }
    } // end wwwcgip


    /* setup wtfnew
    */
    if (!ciawtfp) if (dbxciset(cipfile)) fatal(cipfile);

    /* setup alloc
    */
    maxmfrl=rec_maxmfrl;

    cgimsgsize=maxmfrl;
    buffersize=(int)maxmfrl;

    // alloc reply buffer / move nul to 1st byte
    buffer=loadfile(NULL,'@',"",NULL,(LONGX)buffersize,'\0');
    if (!buffer) fatal("serc/alloc/buffer");

    // alloc reply databuff / move nul to 1st byte
    databuffsize=buffersize;
    databuff=loadfile(NULL,'@',"",NULL,databuffsize,'\0');
    if (!databuff) fatal("serc/alloc/databuff");

    // alloc reply textbuff / move nul to 1st byte
    textbuffsize=buffersize;
    textbuff=loadfile(NULL,'@',"",NULL,textbuffsize,'\0');
    if (!textbuff) fatal("serc/alloc/textbuff");

    /* reply header
    */
    if (outfmthtml) {
        printf("Content-type: text/html\n\n");
        printf("<html><body><pre>\n");
    }
    else {
        //printf("Content-type: text/plain\n\n");
        printf("Content-type: text/xml\n\n");
        printf("<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>\n");
        printf("<serc cisis=\"CIFFI\" version=\"4.4a\" date=\"%04d%02d%02d\" time=\"%02d%02d%02d\">\n",
            1900+tp->tm_year,tp->tm_mon+1,tp->tm_mday,tp->tm_hour,tp->tm_min,tp->tm_sec);
        printf("<request>\n");
    }
    mustclose=1;

    /* setup wtrig2 parms
    */
    if (!parmwtmsg) {

        //drive mainserver to procs
        if (wwwcgip) if (strstr(wwwcgip,"^nprocs^von") || strstr(wwwcgip,"^ntagtext^v") || strstr(wwwcgip,"^ntagtext1^v")
             || strstr(wwwcgip,"^ndb^v") || strstr(wwwcgip,"^nmfn^v") || strstr(wwwcgip,"^ntag^v")
             || strstr(wwwcgip,"^nocc^v") || strstr(wwwcgip,"^noc2^v")
             || strstr(wwwcgip,"^npcmd^v") || strstr(wwwcgip,"^nproc^v") || strstr(wwwcgip,"^npft^v") ) {

            if (debug) printf("+++ wwwcgip=%s<br />\n",wwwcgip);

            np=strstr(wwwcgip,"^ndb^v");
            if (np) {
                for (vlen=0,p=np+6; *p; p++) if (strncmp(p,"H2000 ",6) == 0) break; else vlen++;
                memmove(dbname,np+6,vlen); dbname[vlen]='\0';
            }
            np=strstr(wwwcgip,"^nmfn^v");
            if (np) {
                for (dbmfn=0,p=np+7; *p && isdigit(*p); p++) dbmfn=dbmfn*10+(int)(*p)-(int)'0';
            }
            np=strstr(wwwcgip,"^ntag^v");
            if (np) {
                for (dbtag=0,p=np+7; *p && isdigit(*p); p++) dbtag=dbtag*10+(int)(*p)-(int)'0';
            }
            np=strstr(wwwcgip,"^nocc^v");
            if (np) {
                for (dbocc=0,p=np+7; *p && isdigit(*p); p++) dbocc=dbocc*10+(int)(*p)-(int)'0';
            }
            np=strstr(wwwcgip,"^noc2^v");
            if (np) {
                for (dboc2=0,p=np+7; *p && isdigit(*p); p++) dboc2=dboc2*10+(int)(*p)-(int)'0';
            }

            //read
            if (!dbname[0]) strcpy(dbname,"null");
            if (dbmfn <= 0) dbmfn=1;
            RECORD(irec,dbname,dbmfn);
//            if (RECrc != RCNORMAL || MFRstatus != ACTIVE) {
//                RECrc=RCNORMAL; MFRstatus=ACTIVE; MFRnvf=0;
//            }

#if 0
            //sort dir entries
            if (fldupdat(irec,"S")) fatal("mxget/read/S");

            //convert entities
            if (fldupdat(irec,"Gserxgiz0")) fatal("mxget/read/Gserxgiz0");
#endif
            //add cgi data
            if (fldupdat(irec,wwwcgip)) fatal("mxget/read/wwwcgip");

            np=strstr(wwwcgip,"^nproc^v");
            if (np) {
                for (vlen=0,p=vp=np+8; *p; p++) if (strncmp(p,"H2000 ",6) == 0) break; else vlen++;
                if (vlen) {
                    char k=vp[vlen];
                    vp[vlen]='\0';
                    rc=recfmt(irec,databuffsize,vp,databuff,databuffsize);
                    vp[vlen]=k;
                    if (rc < 0) fatal("mxget/procs/proc/recfmt");
                    if (fldupdat(irec,databuff)) fatal("mxget/procs/proc/fldupdat");
                }
            }
            np=strstr(wwwcgip,"^npcmd^v");
            if (np) {
                for (vlen=0,p=vp=np+8; *p; p++) if (strncmp(p,"H2000 ",6) == 0) break; else vlen++;
                if (vlen) {
                    char k=vp[vlen];
                    vp[vlen]='\0';
                    if (fldupdat(irec,vp)) fatal("mxget/procs/pcmd");
                    vp[vlen]=k;
                }
            }

            textbuff[0]='\0'; tbp=textbuff;
            np=strstr(wwwcgip,"^ntagtext^v");
            if (np) {
                for (vlen=0,p=vp=np+11; *p; p++) if (strncmp(p,"H2000 ",6) == 0) break; else vlen++;
                if (isdigit(*vp)) {
                    int iocc=1,tag=atoi(vp);
                    for (; *recfield(tbp+strlen(tbp),irec,tag,iocc,""); iocc++) {
                        strcat(tbp,". ");
                    }
                }
                else {
                    char k=vp[vlen];
                    vp[vlen]='\0';
                    rc=recfmt(irec,textbuffsize,vp,tbp,textbuffsize);
                    vp[vlen]=k;
                    if (rc < 0) fatal("mxget/tagtext/recfmt");
                }
                txtvlen=strlen(tbp);
                if (txtvlen) { txtvp=tbp; tbp+=txtvlen; }
                *++tbp='\0';
            }
            np=strstr(wwwcgip,"^ntagtext1^v");
            if (np) {
                for (vlen=0,p=vp=np+12; *p; p++) if (strncmp(p,"H2000 ",6) == 0) break; else vlen++;
                if (isdigit(*txt1vp)) {
                    int iocc=1,tag=atoi(txt1vp);
                    for (; *recfield(tbp+strlen(tbp),irec,tag,iocc,""); iocc++) {
                        strcat(tbp,"\n");
                    }
                }
                else {
                    int rc;
                    char k=vp[vlen];
                    vp[vlen]='\0';
                    rc=recfmt(irec,textbuffsize,vp,tbp,textbuffsize);
                    vp[vlen]=k;
                    if (rc < 0) fatal("mxget/tagtext/recfmt");
                }
                txt1vlen=strlen(tbp);
                if (txt1vlen) { txt1vp=tbp; tbp+=txt1vlen; }
                *++tbp='\0';
            }

            pcmdp=strstr(wwwcgip,"^npcmd^v");
            procp=strstr(wwwcgip,"^nproc^v");
            pftxp=strstr(wwwcgip,"^npft^v");

            *buffer='\0';

            //default is dump field 1
            if (!txtvlen) if (!txt1vlen)
            if (!pcmdp) if (!procp) if (!pftxp) {
                int tag=dbtag;
                int occ=dbocc;
                int oc2=dboc2;
#if 0
                for (; occ <= oc2; occ++) {
                    int xdir,len=0;
                    xdir=fieldx(irec,tag,occ);
                    if (xdir < 0) break;
                    memmove(buffer,FIELDP(xdir),len=DIRlen(xdir)); buffer[len]='\0';
                }
#endif
                    int xdir,loop;
                    int xocc=0,lastag=0;

                if (tag<0) tag=0;
                if (occ<0) occ=0;
                if (oc2<0) oc2=0;
                if (oc2<occ) oc2=occ;

                    sprintf(buffer+strlen(buffer),"<file name=\"%s\">\n",dbname);
                    sprintf(buffer+strlen(buffer),"<record mfn=\"%"_LD_"\" mfrl=\"%"_LD_"\" nvf=\"%d\" status=\"%s\">\n",
                                MFRmfn,(LONGX)MFRmfrl,MFRnvf,(MFRstatus == ACTIVE) ? "active" : "deleted");
                    if (MFRstatus == ACTIVE) {
                        sprintf(buffer+strlen(buffer),"<fields>\n");
                        for (xdir=0, loop=MFRnvf; loop--; xdir++) {
                            int xtag=DIRtag(xdir);
                            if (!debug) if (xtag == 2000) continue;
                            if (xtag == lastag) xocc++; else { lastag=xtag; xocc=1; }
                            if (tag) if (xtag != tag) continue;
                            if (occ) if (xocc  < occ) continue;
                            if (oc2) if (xocc  > oc2) continue;
                            sprintf(buffer+strlen(buffer),"<field tag=\"%u\" occ=\"%d\">",DIRtag(xdir),xocc);
                            memmove(buffer+strlen(buffer),FIELDP(xdir),DIRlen(xdir));
                            sprintf(buffer+strlen(buffer),"</field>\n");
                        }
                        sprintf(buffer+strlen(buffer),"</fields>\n");
                    }
                    sprintf(buffer+strlen(buffer),"</record>\n");
                    sprintf(buffer+strlen(buffer),"</file>\n");

                printf("%s",buffer);
            }

            np=strstr(wwwcgip,"^npft^v");
            if (np) {
                for (vlen=0,p=vp=np+7; *p; p++) if (strncmp(p,"H2000 ",6) == 0) break; else vlen++;
                *buffer='\0';
                if (strncmp(vp,"all",3) == 0) {
                    int xdir,loop;
                    int xocc=0,lastag=0;
                    sprintf(buffer+strlen(buffer),"<file name=\"%s\">\n",dbname);
                    sprintf(buffer+strlen(buffer),"<record mfn=\"%"_LD_"\" mfrl=\"%"_LD_"\" nvf=\"%d\" status=\"%s\">\n",
                                MFRmfn,(LONGX)MFRmfrl,MFRnvf,(MFRstatus == ACTIVE) ? "active" : "deleted");
                    if (MFRstatus == ACTIVE) {
                        sprintf(buffer+strlen(buffer),"<fields>\n");
                        for (xdir=0, loop=MFRnvf; loop--; xdir++) {
                            int xtag=DIRtag(xdir);
                            if (!debug) if (xtag == 2000) continue;
                            if (xtag == lastag) xocc++; else { lastag=xtag; xocc=1; }
                            sprintf(buffer+strlen(buffer),"<field tag=\"%u\" occ=\"%d\">",DIRtag(xdir),xocc);
                            memmove(buffer+strlen(buffer),FIELDP(xdir),DIRlen(xdir));
                            sprintf(buffer+strlen(buffer),"</field>\n");
                        }
                        sprintf(buffer+strlen(buffer),"</fields>\n");
                    }
                    sprintf(buffer+strlen(buffer),"</record>\n");
                    sprintf(buffer+strlen(buffer),"</file>\n");
                }
                else
                if (isdigit(*vp)) {
                    int iocc=1,tag=atoi(vp);
                    for (; *recfield(buffer+strlen(buffer),irec,tag,iocc,""); iocc++) {
                        strcat(buffer,"\n");
                    }
                }
                else {
                    char k=vp[vlen];
                    vp[vlen]='\0';
                    rc=recfmt(irec,buffersize,vp,buffer,buffersize);
                    vp[vlen]=k;
                    if (rc < 0) fatal("mxget/procs/pft/recfmt");
                }
                printf("%s\n",buffer);
            }

            //check proc.n parms

            /* loop throught cgi data pairs ^nproc.n^v[fldupdat_cmds]
            */

        } //end drive mainserver to procs


        //drive mainserver to wtrig2
        if (wwwcgip) if (strstr(wwwcgip,"^nwtrig2^von")) {

            if (debug) printf("+++ wwwcgip=%s<br />\n",wwwcgip);

            // alloc cgi message
            cgimsg=loadfile(NULL,'@',"",NULL,cgimsgsize,'\0');
            if (!cgimsg) fatal("serc/alloc/cgimsg");
            x=cgimsg;

            /* loop throught cgi data pairs ^nNam^v[Val]
            */
            // H1.len.^nNam^v[Val]
            // <--y--><---len---->
            for (p=wwwcgip, npairs=0; strncmp(p,"H2000 ",6) == 0; ) {

                hhp=p;
                p+=6; y=6;
                for (len=0; isdigit(*p); p++, y++)
                    len=10*len+(int)(*p)-(int)'0';
                p++; y++;
                // ^nNam^v[Val]
                p+=2;
                // Nam^v[Val]
                np=p;
                for (nlen=0; *p; p++) if (*p == SFLDCHR) break; else nlen++;
                // ^v[Val]
                p+=2;
                // [Val]
                vp=p;
                p=hhp+y+len;
                npairs++;
                //
                np[nlen]='\0';                        // Nam dlm
                vlen=len-nlen-2-2;                    // memmove
                if (debug) printf("+++ pair %d: %s[%d+%d,%d,%d]<br />\n",npairs,hhp,y,len,nlen,vlen);
                if (vlen < 0) fatal("serc/vlen");

                  //start wtrig2 parms
                  if (strcmp(np,"wtrig2") == 0) {
                      wtmsg=x;                           //trig move parms from now on
                      sprintf(x,"wtrig2"); x+=strlen(x); //drive
                      continue;
                  }
                  // text1= text= parms
                  if (strcmp(np,"text") == 0) {
                      txtvp=vp; txtvlen=vlen;
                      continue;
                  }
                  if (strcmp(np,"text1") == 0) {
                      txt1vp=vp; txt1vlen=vlen;
                      continue;
                  }
                  // normalize c= d= parms
                  if (strcmp(np,"c") == 0 || strcmp(np,"collection") == 0) {
                      np="c";
                      if (vlen) if (strstr(vp,"^p")) {
                          char *p=strstr(vp,"^p")+2;
                          port=(uint16_t)atoi(p);
                          vlen-=2; for (; isdigit(*p); p++) vlen--;
                      }
                  }
                  if (strcmp(np,"d") == 0 || strcmp(np,"document") == 0) {
                      np="d";
                  }
                  //move wtrig2/other parms - w2p0.c now discart non-w2 parms
                  if (wtmsg) {
                      if (!vlen) { sprintf(x," %s" ,np); x+=strlen(x); }
                      else       { sprintf(x," %s=",np); x+=strlen(x); memmove(x,vp,vlen); x+=vlen; }
                  }
                  else if (debug) printf("+++ pair %d: %s[%d+%d,%d,%d] - ignored <br />\n",npairs,hhp,y,len,nlen,vlen);

            } // end loop

            *x='\0';
            if (*p) fatal("serc/wwwcgip");

        } //end drive mainserver to wtrig2

    } /* end if !parmwtmsg */



    /* Run
    */
    *buffer='\0';

    // drive mainserver to wtrig2
    if (parmwtmsg || wtmsg) {

       int cmd=debug;
       char *message=parmwtmsg;
       int maxrds=1;

       if (wtmsg) {  //cgi overrides parms!
           char *x=wtmsg+strlen(wtmsg);
           message=wtmsg;
           if (txt1vlen) {
             sprintf(x," text1="); x+=strlen(x); memmove(x,txt1vp,txt1vlen); x+=txt1vlen; *x='\0';
           }
           if (txtvlen) {
             sprintf(x," text=" ); x+=strlen(x); memmove(x,txtvp,txtvlen);   x+=txtvlen;  *x='\0';
           }
       } /* end wtmsg */

       if (debug) printf("serc: message=%s<br />\n",message);
       if (message) {
           if (debug || cmd >= 3) printf("serc: \"%s\" %s %d [%d read]\n",message,serverhost,port,maxrds);
           rc=mainclient(cmd, "X1417", serverhost, port, message, buffer, buffersize, maxrds);
           if (debug || cmd >= 3) if (rc < 0) printf("serc: failure %d \n", rc);
       }

    } /* end parmwtmsg || wtmsg */



    /* get secs_ for elapsed */
    secs_end=time(&secs_now);

#if MX_SERX

#if MXFUN
    char *p;
    int iarg;
    for (p=area, iarg=1; iarg<argc; iarg++) {
        strcpy(p,argv[iarg]); p+=strlen(p); if (iarg<argc-1) *p++='\n';
    }
    *p='\0';
    rc=cisis_mx(area...);
#else /* MXFUN */
    rc=mxexec(argc,argv...);
#endif /* MXFUN */
    exit(rc);

#else //MX_SERX

    //reply..
    if (outfmthtml) {
        printf("%s",buffer);
    } else {
        printf("%s",buffer);
        printf("</request>\n");
        printf("<elapsed>%"_LD_"</elapsed>\n",(LONGX)secs_end-secs_start);
    }

    // reply trailer
    if (outfmthtml)
        printf("</pre></body></html>\n");
    else
        printf("</serc>\n");

#endif //MX_SERX

    /* release working areas
    */
    //if (databuff)  FREE(databuff);
    //if (textbuff)  FREE(textbuff);
    //if (buffer)  FREE(buffer);
    //if (cgimsg)  FREE(cgimsg);
    //if (wwwcgip) FREE(wwwcgip);

    /* Done
    */
    return 0;
}

/*
*/
