/* ==----------------------------- w2djk.c -----------------------------== */

int w2docjk (int cmd, WTFUN_ARRAY *awtfp, int coll, char *parmeval)
{
        WTFUN_XWT *xwtp;//=awtfp->vwtfp[coll];

        //WTFUN_XWT current
        xwtp=awtfp->vwtfp[coll];


    /* get eval/jk/jd parms
    */
    //jk_n is init in w2pdoc.c

    if (!parmeval) return 0;

    if (*parmeval) {

      char *p=parmeval;
      int left=strlen(parmeval);

      for (; left; ) {     // for each spec of parmeval
         int jdi=(-1);
         int j;
         unsigned int len,nlen,left2;
         int i,noc;
         char *name,*q;

         while (isspace(*p)) if (left) { p++; left--; }  //strip
         if (!left) break;

         for (name=q=p, len=0; left; p++, len++, left--) {  // go forward
             if (*p == ',') {                          // until comma
                 p++; left--; break;
             }
         }
         if (!len) continue;                      // skip null lines

         for (nlen=0, left2=len; left2; q++, nlen++, left2--) { // go forward
             if (*q == ':') {                                      // until colon
                 q++; break;
             }
         }

         for (i=0, noc=jd_n; i < noc; i++) {  // for each jd_occ
             if (nlen == strlen(jd_name[i]))
                 if (strncmp(jd_name[i],name,nlen) == 0) {
                     jdi=i; break;
                 }
         }
         if (jdi < 0) continue;

         if (jk_n >= MAXJDS) continue; //fatal("MAXJDS");

         j=jk_n;
         jk_jdi[j]=jdi;
         jk_tabcats[j]=jdtabcats;
         jk_tabwidth[j]=jdtabwidth;
         jk_n++;

         if (isdigit(*q)) {
             int tabcats=atoi(q);
             if (tabcats >= /*1*/0 && tabcats <= jdtabcats) jk_tabcats[j]=tabcats;
             while (isdigit(*q)) q++;
             if (*q == ':') q++;
         }
         if (isdigit(*q)) {
             int tabwidth=atoi(q);
             if (tabwidth >= /*1*/0 && tabwidth <= jdtabwidth) jk_tabwidth[j]=tabwidth;
             //while (isdigit(*q)) q++;
             //if (*q == ':') q++;
         }

      }  // end for each spec of parmeval
    }  // end if parmeval

    return jk_n;  // end w2docjk    
}
