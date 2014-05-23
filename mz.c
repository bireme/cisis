/* --------------------------- mz.c -------------------------------- */

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "cisis.h"      /* CISIS Interface */
#include "cirun.h"      /* runtime area and defines by AOT */

#if RECGIZM
#define MZGIZMO	1
#endif
#define MZFST   1

#define ISX(s) (strcmp(s,"x") == 0 || strcmp(s,"X") == 0)

int parmtrace = 0;
int parmwait = 1;

UCHR *key1p =  "!";
UCHR *key2p =  NULL;
UCHR qlfx = '\0';
LONGX parmpst1=0L;
LONGX parmpst2=0L;

UCHR currkey[LE2+1];

LONGX freq[LE2+1];

char line[BUFSIZ];

/* gizmo || fst */
#if MZGIZMO || MZFST
RECSTRU *recp;
LONGX keyrec=1L;
LONGX fstrec=2L;
char buffup[MAXMFRL+BUFSIZ];
int buffuplen=sizeof(buffup)-1;
int xdir;
#endif  

/* gizmo */
#if MZGIZMO
VGIZPSTRU *gizmap,*vgizmap = NULL;
#endif  

/* fst */
#if MZFST
char *stwp=NULL;
FST_CODE *fstpgmp=NULL;
char *lnk1p=NULL;
char *lnk2p=NULL;
LONGX maxlk1=1000;
LONGX maxlk2=500;
int fstfd0=0;
int fstfd1=0;
int parmcore=0;
#endif

#if MZFST
LONGX qtylk1,qtylk2;
#endif


void main(argc,argv)
int argc;
char *argv[];

{
    TRMSTRU *trmp;
    LONGX itrm = 0L;
    char *dbnp;
    int parmcontrol,minus1,plus1;
    int parmterms,parminfo,parmposts,parmfind=0,parmstrip=0;
    LONGX parmtell,parmcount,count=0L;
    int i,argnext,found,n;
    int treecase;
    INFX info1;
    INFO info2;
    INFX info3=0;
    CNSTRU *cp;
    UCHR *p,*q;
    int key2l;
    LONGX parmload=0L;
#if SAMEL
    int lxy;
#endif


    if (argc == 1) {
        printf("%s",cicopyr("Utility MZ"));
        printf("\n");
        printf("mz <dbname> [<option> [...]] \n");
        printf("\n");
        printf("options: \n");
        printf(" \n");
	printf("   {from|to}=<upkey>\n");
        printf("   {minpost|maxpost}=<no_posts> \n");
        printf(" \n");
#if BIREME
        printf("   qlf=[<char>|?] \n");
        printf("   load=<no_bytes> \n");
        printf("\n");
#endif
        printf("   {tell|count}=<no_terms> \n");
        printf("   {+|-}{control|terms[/i]|posts|all} [now] [strip] \n");
#if MZGIZMO
        printf("\n");
        printf("   gizmo=<gizmo_dbn>[,<taglist>] \n");
#endif
#if MZFST
        printf("   fst={<fst_spec>|@[<file>]} [stw=@[<file>]] ");
        printf("[ln{1|2}=<file> [+fix[/m]]] \n");
        printf("   actab=<file> \n");
        printf("   wlookup={@|<ifn>} \n");
#endif
        printf("\n");
        exit(1);
    }
    else {
        dbnp=argv[1];
        argnext=2;
    }

    if (strcmp(dbnp,"trace") == 0) {
        parmtrace=1;
        dbnp=argv[argnext++];
    }
    if (strcmp(dbnp,"trace=trm") == 0) {
        parmtrace=1;
        trmtrace=1;
        dbnp=argv[argnext++];
    }
    if (strcmp(dbnp,"trace=dbx") == 0) {
        parmtrace=1;
        dbxtrace=1;
        dbnp=argv[argnext++];
    }
    if (strcmp(dbnp,"trace=all") == 0) {
        parmtrace=1;
        dbxtrace=trmtrace=1;
        dbnp=argv[argnext++];
    }


    parmcontrol=0; parmterms=1; parminfo=parmposts=0; parmtell=parmcount=0;
    minus1=plus1=0;

    for (i=argnext; i < argc; i++) {

        p = argv[i];

        if (*p == '-') {
            minus1++;
            if (minus1 == 1)
                parmcontrol=parmterms=parminfo=parmposts=1;
        }

        if (strcmp(p,"-all") == 0) {
            parmcontrol=parmterms=parminfo=parmposts=0;
            continue;
        }

        if (strcmp(p,"-control") == 0) {
            parmcontrol=0;
            continue;
        }
        if (strcmp(p,"-terms") == 0) {
            parmterms=0;
            continue;
        }
        if (strcmp(p,"-terms/i") == 0) {
            parmterms=parminfo=0;
            continue;
        }
        if (strcmp(p,"-posts") == 0) {
            parmposts=0;
            continue;
        }

        if (strcmp(p,"nowait") == 0 || strcmp(p,"now") == 0) {
            parmwait=0;
            continue;
        }

#if MZFST
        if (strcmp(p,"+fix") == 0) {
            cifstfix=1;
            continue;
        }
        if (strcmp(p,"+fix/m") == 0) {
            cifstfix=cifstfim=1;
            continue;
        }
#endif
        if (strcmp(p,"+find") == 0) {
            parmfind=1;
            continue;
        }

        if (*p == '+') {
            plus1++;
            if (plus1 == 1)
                parmcontrol=parmterms=parminfo=parmposts=0;
        }
        if (strcmp(p,"+all") == 0) {
            parmcontrol=parmterms=parminfo=parmposts=1;
            continue;
        }

        if (strcmp(p,"+control") == 0) {
            parmcontrol=1;
            continue;
        }
        if (strcmp(p,"+terms") == 0) {
            parmterms=1;
            continue;
        }
        if (strcmp(p,"+terms/i") == 0) {
            parmterms=parminfo=1;
            continue;
        }
        if (strcmp(p,"+posts") == 0) {
            parmposts=1;
            continue;
        }

        if (strncmp(p,"tell:",5) == 0 || strncmp(p,"tell=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&parmtell) != 1)
                fatal(p);
            continue;
        }
        if (strncmp(p,"count:",6) == 0 || strncmp(p,"count=",6) == 0) {
            if (sscanf(p+6,"%"_LD_,&parmcount) != 1)
                fatal(p);
            continue;
        }

        if (strcmp(p,"trace") == 0) {
            parmtrace=1;
            trmtrace=1;
            continue;
        }

        if (strncmp(p,"key1=",5) == 0 || strncmp(p,"from=",5) == 0) {
            key1p=p+5;
            continue;
        }

        if (strncmp(p,"key2=",5) == 0 || strncmp(p,"to=",3) == 0) {
	    if (strncmp(p,"key2=",5) == 0) key2p=p+5; else key2p=p+3;
            key2l=strlen(key2p);
            continue;
        }

        if (strncmp(p,"qlf=",4) == 0) {
            qlfx=p[4]; continue;
        }

        if (strncmp(p,"pst1:",5) == 0 || strncmp(p,"pst1=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&parmpst1) != 1)
                fatal(p);
            continue;
        }
        if (strncmp(p,"pst2:",5) == 0 || strncmp(p,"pst2=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&parmpst2) != 1)
                fatal(p);
            continue;
        }
        if (strncmp(p,"minpost:",8) == 0 || strncmp(p,"minpost=",8) == 0) {
            if (sscanf(p+8,"%"_LD_,&parmpst1) != 1)
                fatal(p);
            continue;
        }
        if (strncmp(p,"maxpost:",8) == 0 || strncmp(p,"maxpost=",8) == 0) {
            if (sscanf(p+8,"%"_LD_,&parmpst2) != 1)
                fatal(p);
            continue;
        }

        if (strncmp(p,"tidx:",5) == 0 || strncmp(p,"tidx=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&itrm) != 1)
                fatal(p);
            continue;
        }

        if (strncmp(p,"load:",5) == 0 || strncmp(p,"load=",5) == 0) {
            if (sscanf(p+5,"%"_LD_,&parmload) != 1)
                fatal(p);
            if (parmload > ALLOMAXV) {
                parmload = ALLOMAXV;
                fprintf(stderr,"+++ load=%"_LD_"\n",parmload);
            }
            continue;
        }

#if MZGIZMO
        if (strncmp(p,"gizmo:",6) == 0 || strncmp(p,"gizmo=",6) == 0) {
            q=p+6;
            if (!*q) fatal(p);
            if (vgizmap) {      /* gizmo already defined */
                for (gizmap=vgizmap; gizmap->nextp; )
                    gizmap=gizmap->nextp;
                gizmread(q,&gizmap->nextp,keyrec);
                gizmap=gizmap->nextp;
            }
            else {              /* first gizmo definition */
                gizmread(q,&vgizmap,keyrec);
            }
            if (parmtrace) {
                for (gizmap=vgizmap; gizmap; gizmap=gizmap->nextp)
                    printf("+++ gizmo=%s \n",gizmap->gdbnp);
            }
            continue;
        }
#endif /* MZGIZMO */

#if MZFST
        if (strcmp(p,"core") == 0) {
            parmcore=1;
            continue;
        }
        if (strncmp(p,"ln1?",4) == 0 || strncmp(p,"ln1=",4) == 0) {
            if (fst_open(NULL,p+4,0) <= 0) fatal(p);
	    fstfd0=fst_fd[0];
            continue;
        }
        if (strncmp(p,"ln2?",4) == 0 || strncmp(p,"ln2=",4) == 0) {
            if (fst_open(NULL,p+4,1) <= 0) fatal(p);
	    fstfd1=fst_fd[1];
            continue;
        }

        if (strncmp(p,"fst?",4) == 0 || strncmp(p,"fst=",4) == 0) {
	         q=p+4;
            if (strcmp(q,"@") == 0) sprintf(q=line,"@%s.fst",dbnp);
            if (!loadfile("",'@',q,buffup,buffuplen,'\n')) fatal(q);
            fst_gener(&fstpgmp,buffup);
            if (parmtrace) { showcore("+++ fst"); printf("%s.\n",buffup); }
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
            stwp=loadstw("",q,stwp=NULL,0L,&n);
            if (parmtrace) {
                for (q=stwp; *q; q+=LE1+1) printf("%s/",q);
                printf("=%d\n",n);
                showcore("+++ stw");
            }
            continue;
        }


        if (strcmp(p,"strip") == 0) {
            parmstrip=1;
            continue;
        }


        if (strncmp(p,"actab?",6) == 0 || strncmp(p,"actab=",6) == 0) {
            if (strcmp(p+6,",numbers") == 0) {
              int c;
    if (!isiswctot) {
	memset(isiswctab,0x00,256);
	for (isiswctot=0; isisactab[isiswctot]; isiswctot++)
	    isiswctab[isisactab[isiswctot]]=1;
    }
              for (c=(int)'0'; c <=(int)'9'; c++)
                  isiswctab[c]=1;
            }
            else {
              unsigned char ubuffup[256];
              memset(isiswctab,0x00,256);
              if (!loadactb(NULL,ubuffup,p+6)) fatal(p);
              for (isiswctot=0; ubuffup[isiswctot]; isiswctot++)
                  isiswctab[ubuffup[isiswctot]]=1;
            }
            continue;
        }

        if (strncmp(p,"wlookup?",8) == 0 || strncmp(p,"wlookup=",8) == 0) {
            fst_wlupifnp=p+8;
            if (strcmp(fst_wlupifnp,"@") == 0) fst_wlupifnp=dbnp;
            continue;
        }

#endif

#if MZFST
        if (strncmp(p,"ln1?",4) == 0 || strncmp(p,"ln1=",4) == 0) {
#if MXIFUPD
            if (ifnp) fatal(p);
#endif
            if (fst_open(NULL,p+4,0) <= 0) fatal(p);
            continue;
        }
        if (strncmp(p,"ln2?",4) == 0 || strncmp(p,"ln2=",4) == 0) {
#if MXIFUPD
            if (ifnp) fatal(p);
#endif
            if (fst_open(NULL,p+4,1) <= 0) fatal(p);
            continue;
        }
#endif

        fatal(p);
    }



    /* Set up */

#if MZFST
    if (fstpgmp) parmposts=1;
#endif

    if (parmposts+parmpst1+parmpst2 == 0) {
        trmalloc(labs(itrm),0L);
    }
    if (parmload) {
        invflush(dbnp); /* test it */
        invsetup(dbnp,parmload,parmload,parmload);
    }
    memset(freq,0x00,sizeof(freq));

    dbxopt_ordwr=O_RDONLY;
    TERM(itrm,dbnp,key1p);

    if (parmcontrol || parmtrace) {
#if SAMEL
        printf("*%s [%d/%d]\n",dbnp,TDBiflxx,TDBifmap->iflxn);
#else
        printf("*%s\n",dbnp);
#endif

        printf("idtype ordn ordf   n   k  liv    ");
        printf("posrx nmaxpos  fmaxpos  abnormal\n");

        for (treecase=0; treecase < 2; treecase++) {
            cp= &TDBifmap->cn[treecase];
            printf("%4d  %4d %4d %4d%4d",
                                cp->idtype,cp->ordn,cp->ordf,cp->n,cp->k);
            printf("%4d %8"_LD_,cp->liv,cp->posrx);
            printf("%8"_LD_" %8"_LD_"    %4d\n",
                                cp->nmaxpos,cp->fmaxpos,cp->abnormal);
        }
        if (parmtrace) {
#if IFUPDAT
        printf("cn =%3d     \n",TDBifmap->cnopn);
#else
        printf("cn =%3d = %p  +%6"_LD_"\n",0,NULL,TDBifmap->cc_offset);
#endif
#if CNCLx
	printf("n1 =%3d = %p  +%6"_LD_"\n",
                TDBifmap->n1opn,TDBifmap->nybasep[0],TDBifmap->cn_offset[0]);
        printf("n2 =%3d = %p  +%6"_LD_"\n",
                TDBifmap->n2opn,TDBifmap->nybasep[1],TDBifmap->cn_offset[1]);
        printf("l1 =%3d = %p  +%6"_LD_"\n",
                TDBifmap->l1opn,TDBifmap->lybasep[0],TDBifmap->cl_offset[0]);
        printf("l2 =%3d = %p  +%6"_LD_"\n",
		TDBifmap->l2opn,TDBifmap->lybasep[1],TDBifmap->cl_offset[1]);
#endif
#if SAMEL
        printf("if =%3d = %p  +%6"_LD_" [#%d]\n",
                TDBifmap->ifopn[0],TDBifmap->iybasep,TDBifmap->ci_offset,
                TDBiflxx);
        for (lxy=2; lxy <= TDBifmap->iflxn; lxy++)
            printf("if =%3d = %p          [#%d]\n",
                TDBifmap->ifopn[lxy-1],NULL,lxy);
#else
        printf("if =%3d = %p\n",TDBifmap->ifopn,TDBifmap->iybasep);
#endif
#if NX
        printf("n1 0 %3d %3d %p\n",TDBifmap->nx[0][0].pages);
#endif
    } /* parmtrace */
    }



    /* Execution */

    for (; TRMrc != RCEOF; ) {

                if (key2p)
                    if (memcmp(TRMkey,key2p,key2l) > 0)
                        break;

	        if (parmwait)
	            if (count) {
	                printf(".."); gets(line);
			if (ISX(line) || strcmp(line,"/*") == 0) break;
	                if (line[0]) { 
printf("%s<=\n",line);			
			    TERM(itrm,dbnp,line); 
			    continue; 
			}
	            }

                if (parmposts+parmpst1+parmpst2) {
                    posthdr1(trmp); /* get ifp 1st segm */
                }

                    if (TRMlcase) {            /* {} yes */
                        info1=((L2IDXE *)TRMlidxp(1))->info1;
                        info2=((L2IDXE *)TRMlidxp(1))->info2;
#if LIND
                        info3=((L2IDXE *)TRMlidxp(1))->info3info4.info3;
#endif
                    }
                    else {
                        info1=((L1IDXE *)TRMlidxp(0))->info1;
                        info2=((L1IDXE *)TRMlidxp(0))->info2;
#if LIND
                        info3=((L1IDXE *)TRMlidxp(0))->info3info4.info3;
#endif
                    }
		    
                if (parmpst1) 
		    if (TRMxytotp < parmpst1) { NXTERM(itrm); continue; }
                if (parmpst2)
		    if (TRMxytotp > parmpst2) { NXTERM(itrm); continue; }

#if MZFST || MZGIZMO
	if (
#if MZGIZMO
	    vgizmap ||
#endif
		       fstpgmp) {
		if (!recinit) recinit();
		if (!vrecp[keyrec]) recallok(keyrec,MAXMFRL);
		recp=vrecp[keyrec];
		RECtype=TYPEMFR;
		memset(MFX,0x00,LEADER); MFRbase=MFRmfrl=LEADER;
		MFRmfn=count+1;
      for (n=vlex[TRMlcase]; n--; ) if (TRMkey[n] != ' ') break;
		p=buffup; *p='\0';
#if BEFORE20030113
		sprintf(p,"A1\x01%s\x01",TRMkey);
#else
      if (parmstrip) {
        int wlen;
        UCHR word[LE2+1+1]/*,*wp*/;
		  for (q=(UCHR *)TRMkey; ; ) {
          word[wlen=0]='\0';
		    while (isiswctab[*q]) {
			    if (wlen < sizeof(word)) word[wlen++] = *q;
			    if (!*++q) break;
		    }
		    while (!isiswctab[*q]) if (*q) q++; else break;
		    if (wlen == 0) if (*q) continue; else break;
		    word[wlen] = '\0';
		    /* for (wp=word; *wp; wp++) *wp = isisuctab[*wp]; */
          if (wlen == n+1) {
		      sprintf(p,""); break;
          }
          else {
		      sprintf(p,"A1\x01");  p+=strlen(p);
            sprintf(p,"%s ",word); p+=strlen(p);
            sprintf(p,"\x01");    p+=strlen(p);
          }
          if (!*q) break;
        }
      }
      else {
		    sprintf(p,"A1\x01%s\x01",TRMkey);
      }
#endif
		p+=strlen(p);
		sprintf(p,"A2~%d~",n+1);
		p+=strlen(p);
		sprintf(p,"A5~^l%d^w%d^p%"_LD_"^i%d^o%d~",
			TRMlcase+1,vlex[TRMlcase],
			(TRMlcase)?TRMl2buf.pos:TRMl1buf.pos,
			TRMliock(TRMlcase)+1,
			(TRMlcase)?TRMl2buf.ock:TRMl1buf.ock);
		p+=strlen(p);
		sprintf(p,"A3~^a%"P_OFF_T"^b%"_LD_"^c%"P_OFF_T"~",(LONG_LONG)info1,info2,(LONG_LONG)info3);
		p+=strlen(p);
		sprintf(p,"A10~%"_LD_"~",TRMxytotp);
		if (fldupdat(keyrec,buffup)) fatal(buffup);
		if (parmtrace || parmcontrol) prtfields(recp,MFRmfn);
	}
#if MZGIZMO
            for (gizmap=vgizmap; gizmap; gizmap=gizmap->nextp)
                recgizmo(keyrec,gizmap);
#endif
#endif /* MZFST || MZGIZMO */


                if (parmterms) {
                    if (qlfx) {
                        found=0;
                        for (n=(TRMlcase)?LE2:LE1; n--; )
                            if (TRMkey[n] != ' ') break;

                        if (qlfx == '?') {
                            /* fprintf(stderr,"%s=%d\n",TRMkey,n+1); */
                            freq[n+1]++;
                        }
                        else {
                            if (memchr(TRMkey+1,qlfx,n))
                                found=1; /* /xx is ok */
                            if (!found) {
                                TRMkey[n+1]='\0';
                                printf("%s\n",TRMkey);
                            }
                        }
                    }
                    else 
		        if (/*parmcontrol || */ parminfo || parmtrace)
                            printf(" + %s: %"P_OFF_T"/%"_LD_"/%"P_OFF_T"\n",
			        TRMkey,(LONG_LONG)info1,info2,(LONG_LONG)info3);
			else
			    printf("%s\n",TRMkey);
                }

                if (parmposts) {
                    for (; posting(itrm,TRMpost+1) > 0; ) {
#if MZFST
        if (fstpgmp) {
		recp=vrecp[keyrec];
		MFRmfn=TRMpmfn;
		sprintf(buffup,
			"D11D12D13D14A11~%"_LD_"~A12~%u~A13~%u~A14~%u~",
				TRMpmfn,TRMptag,TRMpocc,TRMpcnt);
		if (fldupdat(keyrec,buffup)) fatal(buffup);
		if (parmtrace || parmcontrol) prtfields(recp,MFRmfn);
if (parmcore) {
            *buffup='\0'; fst_batchp=buffup; fst_fd[0]=fst_fd[1]=0;
} /* parmcore */
            fst_inter(fstpgmp,keyrec,stwp,
                        &lnk1p,maxlk1,&lnk2p,maxlk2,&qtylk1,&qtylk2);
            fst_fx[0]+=qtylk1; /* lnk1p */
            fst_fx[1]+=qtylk2; /* lnk2p */
if (parmcore) {
		fst_fd[0]=fstfd0; fst_fd[1]=fstfd1;
		if (!vrecp[fstrec]) recallok(fstrec,(MAXMFRL+BUFSIZ));
		recp=vrecp[fstrec];
		RECtype=TYPEMFR;
		memset(MFX,0x00,LEADER); MFRbase=MFRmfrl=LEADER;
		MFRmfn=count+1;
            if (*buffup) if (fldupdat(fstrec,buffup)) fatal(buffup);
		if (parmtrace || parmcontrol) prtfields(recp,MFRmfn);
	    for (xdir=0; xdir < MFRnvf; xdir++) {
/*
  1  ®ABSORPTION^m9^o1^c1^l1¯
*/
	    	;
	    }
} /* parmcore */
        }
	else
#endif
                        printf("   #%"_LD_"/%"_LD_": mfn=%"_LD_" tag=%d occ=%d cnt=%d\n",
                            TRMpost,TRMxytotp,
                            TRMpmfn,TRMptag,TRMpocc,TRMpcnt);
                    }
                }

                count++;
                if (parmtell)
                    if ((count % parmtell) == 0)
                        fprintf(stderr,"+++ %"_LD_"\n",count);

                if (parmcount)
                    if (count >= parmcount)
                        break;
			
		NXTERM(itrm);
		if (parmfind) TERM(itrm,TDBname,TRMkey);
    }

    if (qlfx == '?') 
        for (n=0; n <= LE2; n++)
            if (freq[n]) printf("%2d|%9"_LD_"\n",n,freq[n]);


#if MZFST
    if (fstpgmp) {
        if (lnk1p) FREE(lnk1p); 
        if (lnk2p) FREE(lnk2p);
        lnk1p=lnk2p=NULL;
        fst_clos(0); fst_clos(1); fst_fd[0]=fst_fd[1]=0;
        if (fst_fx[0]+fst_fx[1])
	  if (parmtrace || parmcontrol)
            fprintf(stderr,"+++ %"_LD_"+%"_LD_" links \n",fst_fx[0],fst_fx[1]);
    }
#endif

    exit(0);
}

