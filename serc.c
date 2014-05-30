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
#include <ctype.h>                                                        .
#include <string.h>

#include "cisis.h"      /* CISIS Interface header file */
#include "cirun.h"      /* CISIS Interface runtime declarations */
//#include "cihsh.h"      /* CISIS Interface hashing & binary table search */

#if !PC
#include "ciupdsocx.c"  /* CISIS socket mainclient() */
#endif

#if GCC
#define PGCC  1
#else
#define uint16_t unsigned short
#endif

#define PORT            1417
#define MESSAGE         "wtrig2 ... text=..."
#define SERVERHOST      "serverofi.bireme.br"


#define TAG1   1
#define TAG2   2
#define TAG23  2


void serc_syntax(void);
void serc_syntax()
{
}


int
main(int argc, char *argv[])
{
    //RECSTRU *recp; /* mandatory for defines REC RDB MF0 MFR DIR, FIELDP */
    LONGX maxmfrl=MAXMFRL;   // default

    char *serverhost=SERVERHOST;
    uint16_t port=PORT;
    LONGX xval;

    char *wwwcgip=NULL;     // allocated in cicgi0
    char *parmwtmsg=NULL;   // wtrig2 command line - calling argument wtrig2
    char *wtmsg=NULL;   // wtrig2 command line - allocated if not parmwtmsg

    char *parmhtmsg=NULL;   // http command line - calling argument http
    char *htmsg=NULL;       // http command line - allocated if not parmhtmsg

    char *parmxtmsg=NULL;   // xttt command line - calling argument xttt
    char *xtmsg=NULL;       // xttt command line - allocated if not parmxtmsg

    int debug=0;
    int force=0;
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


    int rc; //=0;

    int i;


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
            printf("     \"wtrig2 c=col [maxrel=n] [minsim=0.n] [text1=x] [text=x]\" \n");
            printf("\n");
            exit(1);
        }
        if (strcmp(p,"force") == 0) {
            force=1;
            continue;
        }
        if (strcmp(p,"debug") == 0) {
            debug=1;
            continue;
        }
        if (strncmp(p,"cipar=",6) == 0) {
            if ((dbxcipfp=fopen(p+6,"r")) == NULL) fatal(p);
            continue;
        }
        if (strncmp(p,"port=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&xval) != 1) fatal(p);
            port=(uint16_t)xval;
            continue;
        }
        if (strncmp(p,"host=",5) == 0) {
            if (!*(p+5)) fatal(p);
            serverhost=p+5;
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
    
    
        if (force) {
            putenv("REQUEST_METHOD=GET");
//putenv("QUERY_STRING=wtrig2=on&c=lilrelff&text1=Jose+Meio+Silva&text=titulo+1%0D%0A%0D%0Atitulo+2%0D%0Atitulo+3%0D%0A&maxrel=3&minsim=0.20&serc.go=submit");
          //putenv("QUERY_STRING=wtrig2=on&collection=lilrelff&text1=Jos%E9+da+Silva+Junior&text=Publica%E7%E3o+eletr%F4nica+hoje%0D%0AElectronic+publishing+today%0D%0AOn+the+Virtual+Health+Library&maxrel=3&minsim=0.20&serc.go=submit");

putenv("QUERY_STRING=wtrig2=on&cipar=similar/jdlilacs.cip&collection=lilrelff&text1=Jose+da+Silva+Junior&text=Publicacao+eletronica+hoje%0D%0AElectronic+publishing+today%0D%0AOn+the+Virtual+Health+Library&maxrel=3&minsim=0.20&serc.goxml=submit");

        }

        
    /* setup alloc
    */
    p=dbxcipar(NULL,"maxmfrl",'='); if (dbxcipok) sscanf(p,"%"_LD_,&maxmfrl);
    if (maxmfrl < (LONGX)MSNVSPLT || (RECHSIZE+maxmfrl) > ALLOMAXV) maxmfrl=MAXMFRL;
    rec_maxmfrl=maxmfrl;
    //fmt_fsiz=maxmfrl;
    cgimsgsize=maxmfrl;
    buffersize=(int)maxmfrl;

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

        fprintf(stdout, "<sercla-jd version=\"1.0\">\n");
    }

    /* setup wtrig2 parms
    */
    if (!parmwtmsg) {
        /* get CGI data
        */
        p=getenv("REQUEST_METHOD"); if (debug) if (p) printf("+++ REQUEST_METHOD=%s<br />\n",p);
        q=getenv("QUERY_STRING");   if (debug) if (q) printf("+++ QUERY_STRING=%s<br />\n",q);
        if (p) cicgi0(NULL, NULL, &wwwcgip, (UWORD)1, "tag");  // H1 10 ^ndbn^vcdsH1 10 ^ncount^v2H1 7 ^nnow^vH1 10 ^nbtell^v0H1 20 ^nbool^vplants*waterH1 15 ^npft^vmfn/v24/
        if (debug) if (!wwwcgip) printf("+++ wwwcgip=%s<br />\n","void");

        /* setup service parameters
        */
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
            for (p=wwwcgip, npairs=0; strncmp(p,"H1 ",3) == 0; ) {

                hhp=p;
                p+=3; y=3;
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

                  //internal parms - execute 
                  if (strcmp(np,"debug") == 0) {
                      if (!vlen) debug=1;
                      else sscanf(vp,"%d",&debug);
                      continue;
                  }
                  if (strcmp(np,"cipar") == 0) {
                      if (vlen) { char c=vp[vlen]; vp[vlen]='\0'; if ((dbxcipfp=fopen(vp,"r")) == NULL) fatal(vp); vp[vlen]=c; }
                      continue;
                  }
                  if (strcmp(np,"port") == 0) {
                      if (sscanf(vp,"%"_LD_,&xval) != 1) fatal(np);
                      port=(uint16_t)xval;
                      continue;
                  }
//                  if (strcmp(np,"host") == 0) {
//                      if (!vp) fatal(np);
//                      serverhost=vp;   // falta delimitar
//                      continue;
//                  }
                          
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

    // alloc reply buffer / move nul to 1st byte
    buffer=loadfile(NULL,'@',"",NULL,(LONGX)buffersize,'\0');
    if (!buffer) fatal("serc/alloc/buffer");

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
#if !PC
           rc=mainclient(cmd, message, serverhost, port, buffer, buffersize, maxrds);
#else
           rc=0;
#endif
           if (debug || cmd >= 3) if (rc < 0) printf("serc: failure %d \n", rc);
       }

    } /* end parmwtmsg || wtmsg */


    // reply
    if (buffer) printf("%s",buffer);

    // reply trailer
    if (outfmthtml) 
        printf("</pre></body></html>\n");
    else
        printf("</sercla-jd>\n");


    /* release working areas
    */
    //if (buffer)  FREE(buffer);
    //if (cgimsg)  FREE(cgimsg);
    //if (wwwcgip) FREE(wwwcgip);

    /* Done
    */
    return 0;
}

/* 
*/




