/* ==----------------------------- w2.c ------------------------------== */

#if WHENFUN
#include "w2rcol.c"    // usa colvdb/close        
#include "w2ralbs.c"   // usa colxdb/mstsetup
#endif

int w2reply ( int cmd, WTFUN_ARRAY *awtfp, int coll, int yocc, LISTA *l, int joinjd, int showv3, int showid, char *reply, char *batchp )
{
    WTFUN_XWT *xwtp=awtfp->vwtfp[coll];  // current

    if (reply) {

      *reply='\0';
      
      //open
      sprintf(reply,"<similar rank=\"%d\" s=\"%f\"",yocc,l->sim);
      reply+=strlen(reply);

      //if (parmcollmfntag)
      sprintf(reply," c=\"%"_LD_"\"",l->mfn);
      reply+=strlen(reply);
      ////if (parmcollapse)
      ////if (!parmtext)
      ////sprintf(reply," m=\"%"_LD_"\"",l->xxx);
      ////reply+=strlen(reply);
      //if (parmcollmfntag)
      sprintf(reply," h=\"%"_LD_"\"",l->hit);
      reply+=strlen(reply);

      //showid
      if (showid) {
          sprintf(reply," id=\"");
          reply+=strlen(reply);
          
#include "w2oid.c"
///////////////////////////////////////////////////////////////////////////////////////////////////
          sprintf(reply,"\"");
          reply+=strlen(reply);
      }

      
      //output collection's jdlist as " jd=\"..;..;..\""
      if (jk_n /*joinjd*/) {
                    
#include "w2ojd.c"
///////////////////////////////////////////////////////////////////////////////////////////////////
      }

      sprintf(reply,">");
      reply+=strlen(reply);

      //showv3
      if (jkv_n || showv3) {

#include "w2ov3.c"
///////////////////////////////////////////////////////////////////////////////////////////////////
      }

      //close
      sprintf(reply,"</similar>\n");
      reply+=strlen(reply);
      
      /* ... */
      if (!reply) fatal("w2reply/reply");
    }
    
    return 0;
}




int trigcoll (int cmd, WTFUN_ARRAY *awtfp, char *parmstring, char *reply)
{
    WTFUN_XWT *xwtp;            
    
    int coll=(-1);
    int icol;
    int parmdetails=(cmd >=3)?1:0;
    
    char *parmeval=NULL;    // check for eval= in w2pdoc.c
    int joinjd=0;           // check for TAG440/jd= in w2set.c / w2pdoc.c
    
    char *parmdisp=NULL;    // check for disp= in w2pdoc.c
    int showv3=(-TAG3);     // usage
    int showid=0;           // usage

    char *ry=reply; // watch
       
#include "w2loc.c"
///////////////////////////////////////////////////////////////////////////////////////////////////

    *reply='\0';

    xwtp=awtfp->vwtfp[0]; // wtf4p use
    memset(xwtp,0x00,sizeof(WTFUN_XWT));
    
#include "wtfun.c"
///////////////////////////////////////////////////////////////////////////////////////////////////

    /* get secs_ for elapsed */
    secs_start=time(&secs_now);

#if FATRAPWT
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
         if (reply) {
             sprintf(reply,"<error type=\"fatal\" in=\"trigcoll\" code=\"%d\">%s</error>\n",fatal_errcod,fatal_iomsg);
         }
         /* restore/notrap */  //strcpy(fatal_iomsg,""); 
    	 memmove(fatal_jumper,z_jumper,sizeof(fatal_jumper)); strcpy(fatal_iomsg,z_iomsg); 
         fatal_errcod=z_errcod; 
         dbxopt_errno=z_xerrno;
         return (-1);
     }

#endif


    /* get calling parameters from parmstring
    */
#include "w2arg.c"
#include "w2p0.c"
#include "w2pcol.c"
///////////////////////////////////////////////////////////////////////////////////////////////////
            
    if (!collectiondb) return -1;
    
    /* select collection from structure
    */
    //if (cmd) fprintf(stderr,"*** wt: %s \n",collectiondb);   
    for (icol=1; icol < awtfp->nwtfs; icol++) { // wtf4p skip
        if (strcmp(collectiondb,awtfp->vwtfname[icol]) == 0) { 
            xwtp=awtfp->vwtfp[coll=icol]; 
            if (cmd) fprintf(stderr,"*** wt: %s [coll#%d] OK  \n",awtfp->vwtfname[coll],coll);   
            parmdocumentsdb=NULL;
            break;
        }
    }
    
    if (coll == -1) {
        /* validade collection  */        
            char *q=dbxcipar(NULL,collectiondb,'='); 
            int value=(dbxcipok)?atoi(q):0;
            if (value != awtfp->globalport) return -1;
    }    
    if (coll == -1) {
        /* setup collection in [0] */
            WTFUN_XWT *xxwtp=wtnew(cmd);                 // new coll
            if (!xxwtp) return -1;
            memmove(xxwtp,xwtp,sizeof(WTFUN_XWT));        // copy parms
            xwtp=xxwtp;                                  // set current 
            coll=awtfp->nwtfs;                           // coll#
            awtfp->vwtfp[coll]=xwtp;                     // wtfp
            strcpy(awtfp->vwtfname[coll],collectiondb);  // wtfname
            awtfp->nwtfs++;                              // push
            if (cmd) fprintf(stderr,"*** wt: %s [coll#%d] NEW \n",awtfp->vwtfname[coll],coll);   
#include "w2set.c"  // ///inirec///outcrec///outirec
                       // usar colcdb/close
                       // usar colndb/close
                       // mstsetup(colxdb,LONGX_MAX,LONGX_MAX);
#if WHENFUN
            int w2rcol  ( cmd, awtfp, coll );
            int w2rcox  ( cmd, awtfp, coll );
#else
#include "w2rcol.c"    // usa colvdb/close        
#include "w2ralbs.c"   // usa colxdb/mstsetup
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
//        /* then store new entry */
//        if (COLLECTION_SIZE > 0) { 
//        }
    }
    else {                                            // coll already loaded
        /* set */
        parmcollmfntag=collcollmfntag;
        parmmaxrf=collmaxrf;
        parmmaxtv=collmaxtv;
        parmmaxrel=collmaxrel;
        parmminsim=collminsim;
        parmmaxsim=collmaxsim;        
        /* get */
#include "w2pcox.c"
///////////////////////////////////////////////////////////////////////////////////////////////////    
        /* chk */
        if (parmcollmfntag > collcollmfntag)   parmcollmfntag=collcollmfntag;   
        if (parmmaxrf > collmaxrf)   parmmaxtv=collmaxrf;   
        if (parmmaxtv > collmaxtv)   parmmaxtv=collmaxtv;         
        if (parmmaxrel > collmaxrel) parmmaxrel=collmaxrel;
        if (parmminsim > parmmaxsim) parmminsim=collminsim;
        if (parmminsim > parmmaxsim) parmmaxsim=collmaxsim;
    }
    
    /* pcol parms
    */ 
    if (parmmaxrel > collmaxrel) parmmaxrel=collmaxrel;
    if (!parmmaxrel) parmmaxrel=collmaxrel;
    
#if JDIN440
    /* joinjd parm
    */ 
    joinjd=collcolljd;
#else
    joinjd=jd_n;
#endif

#if ALLOWUPCASE
    /* load proper uppercase */
    memmove(isisuctab,xisisuctab,sizeof(xisisuctab));
    memmove(isiswctab,xisiswctab,sizeof(xisiswctab)); 
    isiswctot=xisiswctot;
#else
    // default is ansi
    memmove(isisuctab,ansiuctab,256/*sizeof(ansiuctab)*/);
    for (isiswctot=0; ansiactab[isiswctot]; isiswctot=isiswctot+1) isiswctab[ansiactab[isiswctot]]=1;       
#endif


    /* get secs_ for elapsed */
    secs_colok=time(&secs_now);

        
    /* read/build document */
#include "w2pdoc.c"
///////////////////////////////////////////////////////////////////////////////////////////////////

    /* pdoc parms
    */ 
    //

    /* reply parms
    */
    //reply..
    if (replyhere) {
        char *pcolname;        
        
        sprintf(reply,"<parms>\n");
        reply+=strlen(reply);
        
        sprintf(reply,"<collection path=\"%s\" size=\"%"_LD_"\" terms=\"%"_LD_"\" width=\"%d\" maxtv=\"%d\" maxrf=\"%f\" maxrel=\"%d\" minsim=\"%f\" maxsim=\"%f\"",
            awtfp->vwtfname[coll],
            COLLECTION_SIZE,readnterms,0/*,readwidth+trigs*/,readmaxtv,
            collmaxrf,collmaxrel,collminsim,collmaxsim);
        reply+=strlen(reply);        
        if (parmdetails) { sprintf(reply," mfntag=\"%d,%d\" jdkeys=\"%d\" jdwidth=\"%d\"",collcollmfntag,parmcollmfntag,jdtabcats,jdtabwidth); reply+=strlen(reply); }        
        pcolname=awtfp->vwtfname[coll]; 
        sprintf(reply,">%s",pcolname);
        reply+=strlen(reply);
        sprintf(reply,"</collection>\n");
        reply+=strlen(reply);
        
        if (parmtext) {
            sprintf(reply,"<document size=\"%d\" maxrf=\"%f\" maxrel=\"%d\" minsim=\"%f\" maxsim=\"%f\"",
                strlen(parmtext),
                parmmaxrf,parmmaxrel,parmminsim,parmmaxsim);
            reply+=strlen(reply);
            if (parmdetails) { sprintf(reply," jd=\"%d\" v=\"%d\" id=\"%d\"",joinjd,showv3,showid); reply+=strlen(reply); }
            sprintf(reply,">%s",parmtext);
            reply+=strlen(reply);
            sprintf(reply,"</document>\n");
            reply+=strlen(reply);
        }
        sprintf(reply,"</parms>\n");
        reply+=strlen(reply);
    }
        
    /* end col
    */
    /* return if documents= missing */
    if (!parmtext) if (!parmdocumentsdb) return coll;


    
    /* Document
    */
    
    /* get secs_ for elapsed */
    secs_startdoc=time(&secs_now);
        
    /* setup filesnames
    */    
    if (parmtext) {
    }
    else {
        if (!*docvdb) sprintf(docvdb,"%s.v",parmdocumentsdb);
        if (!*docydb) sprintf(docydb,"%s.y",parmdocumentsdb);
    }

    if (*docydb) dbdocydb=docydb;
    
    /* init output
    */
    if (*docydb) {
        recisis0(docydb);
        record(outcrec,docydb,0L);
    }

    /* main loop: process each document against all docs in collection
    */
    count=0;
    for (docmfn=1; ; docmfn++) {

        /* init similarity computation and most similar hit */
#include "w2iscms.c"
///////////////////////////////////////////////////////////////////////////////////////////////////

        /* read term vector */
#include "w2rdoc.c"
///////////////////////////////////////////////////////////////////////////////////////////////////

        /* init list of relevant terms */
#include "w2setrt.c"
///////////////////////////////////////////////////////////////////////////////////////////////////

        /* init term WEIGHTs for the document */
        memset(WEIGHT,0x00,readnterms*sizeof(float));

        /* init HIT list = docs in collection having 1+ of document terms */
        memset(HITS,0x00,hitbytes);

#include "w2slng.c"
///////////////////////////////////////////////////////////////////////////////////////////////////

        /* loop term vector fields
        */
        for (docv=docvector, docloop=docvectcnt; docloop--; docv++) {

            /* get term key */
#include "w2tkey.c"
///////////////////////////////////////////////////////////////////////////////////////////////////

            /* similarity computation */
#include "w2tksum.c"
///////////////////////////////////////////////////////////////////////////////////////////////////

            /* store weight */
            WEIGHT[dochidx]=weight;
            
            /* OR collection mfns (where term occurs) into HIT - w2rhit(HITS,colxdb,fldarea) */
#include "w2rhit.c"
///////////////////////////////////////////////////////////////////////////////////////////////////

        } /* end loop term vector elements for HIT construction */

        
        /* loop throught hit records and process against the corresponding vector WEIGHT[0..nterms-1]
        */
        for (hitmfn=1; hitmfn <= COLLECTION_SIZE; hitmfn++) {

#include "w2rtphix.c"
///////////////////////////////////////////////////////////////////////////////////////////////////

            if (parmtrace & 0x10) printf("16|%"_LD_"|%"_LD_"\n",docmfn,hitmfn);

#include "w2ctv.c"
///////////////////////////////////////////////////////////////////////////////////////////////////

            /* compute similarity between input document and collection doc
            */
#include "w2simil.c"
///////////////////////////////////////////////////////////////////////////////////////////////////

            /* output results
            */
#include "w2rel1.c"
///////////////////////////////////////////////////////////////////////////////////////////////////

        } /* end loop throught hit records and process the corresponding vector */

        /* output related collection docs for the document
        */
#include "w2out1.c"
///////////////////////////////////////////////////////////////////////////////////////////////////

        count++;
        if (parmtell) if (count%parmtell == 0) fprintf(stderr,"+++ %"_LD_"\n",count);
        
        if (parmtext) break;

        
    } /* end read term vector */


    /* output collapsed results last record */
#include "w2outx.c"
///////////////////////////////////////////////////////////////////////////////////////////////////

    /* close input text and results
    */
    if (*docvdb) {
        dbxflush(docvdb); /* close/flush */
    }
    if (*docydb) {
        dbxflush(docydb); /* close/flush */
    }

    /* release list of related
    */
#if IFFREE
#if CICPP
    delete lista;
#else /* CICPP */
     if (lista) FREE(lista);
#endif /* CICPP */
    lista=NULL;
#endif

    /* get secs_ for elapsed */
    secs_docok=time(&secs_now);

    /* ... */
    if (!reply) fatal("trigcoll/reply");

    /* log execution
    */
    //reply..
    if (replyhere && 0) {
        sprintf(reply,"<elapsed");
        reply+=strlen(reply);
        if (secs_colok-secs_start) { 
            sprintf(reply," col=\"%"_LD_"\" doc=\"%"_LD_"\"",(LONGX)secs_colok-secs_start, (LONGX)secs_docok-secs_startdoc);
            reply+=strlen(reply);
        }
        sprintf(reply,">%"_LD_"</elapsed>\n",(LONGX)secs_docok-secs_start);
        reply+=strlen(reply);
    }

#if FATRAPWT
     /* restore/notrap */  //strcpy(fatal_iomsg,""); 
     memmove(fatal_jumper,z_jumper,sizeof(fatal_jumper)); strcpy(fatal_iomsg,z_iomsg); fatal_errcod=z_errcod; dbxopt_errno=z_xerrno;
#endif

    /* end doc
    */
    return coll;   
}

