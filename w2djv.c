/* ==----------------------------- w2djv.c -----------------------------== */

int w2docjv (int cmd, WTFUN_ARRAY *awtfp, int coll, char *parmdisp)
{
        WTFUN_XWT *xwtp;//=awtfp->vwtfp[coll];

        //WTFUN_XWT current
        xwtp=awtfp->vwtfp[coll];


    /* get disp/jkv/jdv parms
    */
    //jkv_n is init in w2pdoc.c

    if (!parmdisp) return 0;

    if (*parmdisp) {

      char *p=parmdisp;
      int left=strlen(parmdisp);

      for (; left; ) {     // for each spec of parmdisp
         int jdvi=(-1);
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
             if (*q == ',') {                                      // until comma
                 //q++;
                 break;
             }
         }

         for (i=0, noc=jdv_n; i < noc; i++) {  // for each jdv_occ
             if (nlen == strlen(jdv_name[i]))
                 if (strncmp(jdv_name[i],name,nlen) == 0) {
                     jdvi=i; break;
                 }
         }
         if (jdvi < 0) continue;

         if (jkv_n >= MAXJVS) continue; //fatal("MAXJVS");

         j=jkv_n;
         jkv_jdvi[j]=jdvi;
         jkv_disp1[j]=0;
         jkv_disp2[j]=0;
         jkv_n++;

      }  // end for each spec of parmdisp
    }  // end if parmdisp

    return jkv_n;  // end w2docjv    
}
