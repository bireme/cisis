/* ==----------------------------- w2rcol.c ------------------------------== */

#if WHENFUN
int w2rcol ( int cmd, WTFUN_ARRAY *awtfp, int coll )
{
    WTFUN_XWT *xwtp=awtfp->vwtfp[coll];  // current
//    int alwaysloadvectors=1;
#endif
    
    /* load all collection' term vectors
    */
    if (alwaysloadvectors) {
        int div=5;
        VECTEL *v;
        LONGX off;
        LONGX *vcntp;
        LONGX *voffp;
        LONGX *vcmfp;

        VECTOFF=  (LONGX *)loadfile(NULL,'@',"",NULL,COLLECTION_SIZE*sizeof(LONGX),'\0');
        VECTCNT=  (LONGX *)loadfile(NULL,'@',"",NULL,COLLECTION_SIZE*sizeof(LONGX),'\0');
        VECTORS=(VECTEL *)loadfile(NULL,'@',"",NULL,COLLECTION_SIZE*readmaxtv*sizeof(VECTEL),'\0');
        if (parmcollmfntag)
        VECTCMF=  (LONGX *)loadfile(NULL,'@',"",NULL,COLLECTION_SIZE*sizeof(LONGX),'\0');

        v=VECTORS;
        off=0;
        vcntp=VECTCNT;
        voffp=VECTOFF;
        vcmfp=VECTCMF;

        memset(VECTCNT,0x00,COLLECTION_SIZE*sizeof(LONGX));
        memset(VECTOFF,0x00,COLLECTION_SIZE*sizeof(LONGX));
        if (parmcollmfntag)
        memset(VECTCMF,0x00,COLLECTION_SIZE*sizeof(LONGX));

        if (parmtell) fprintf(stderr,"+ loading %s term vectors..\n",colvdb);

        if (parmtell) 
//        {
//            if (parmtell2) parmtell2=COLLECTION_SIZE/(parmtell2>0)?parmtell2:5;
//        }
//        else parmtell2=0;
        parmtell2=COLLECTION_SIZE/div;

        count=0;
        for (mfn=1; mfn <= COLLECTION_SIZE; mfn++) {
            LONGX cnt=0;
            RECORD(irectv,colvdb,mfn);
            if (RECrc == RCEOF) {
                //COLLECTION_SIZE=mfn-1;  /* tentativa de otimizar espaco, etc */ /* nao funciona para lilsptv */
                break;
            }
/*
            if (fldupdat(irectv,"Gsplit=4=wtrig")) fatal("wtrig2/loadvectors/Gsplit");
*/
            for (dirp=MFRdir, xdir=0, loop=MFRnvf; loop--; dirp++, xdir++) {
                if (dirp->tag == parmcollmfntag) {
                    for (collhitmfn=0, p=FIELDP(xdir), left=dirp->len; left; ) {
                        if (!isdigit(*p)) break;
                        collhitmfn=collhitmfn*10+((int)(*p)-(int)'0');
                        p++; left--;
                    }
                    if (collhitmfn < 1) fatal("wtrig2/loadvectors/mfn invalid/1");
                    //if (collhitmfn > COLLECTION_SIZE) fatal("wtrig2/loadvectors/mfn invalid/2"); /* AOT, 11/05/2004 */
                }
                if (dirp->tag != TAG4) continue;
                left=dirp->len;
                fldp=FIELDP(xdir);
                if (alwaysrcolwritebin) {
                     float b2aweight;
                     unsigned short int usi_w;
                     while (left) {
                        if (cnt >= readmaxtv) break;
                        if (left < sizeof(usi_w)+1) fatal("wtrig2/loadvectors/b2a/left/bin");
                        memmove((void *)&usi_w,fldp,sizeof(usi_w));
                        fldp+=sizeof(usi_w); left-=sizeof(usi_w);
                        b2aweight=(float)usi_w/(float)USHRT_MAX;
                        p=fldp; keylen=strlen(p);
                        while (left) { q=fldp; fldp++; left--; if (!*q) break; }
                        v->weight=b2aweight;
                        v->hidx=bsrchindex(table,tabentries,readwidth,p,keylen,&found);
                        if (!found) fatal("wtrig2/loadvectors/TAG4/not found/bin");
#if 1 //DISCART
                        /* optimize term processing */
                        if (parmmaxrf != 1.0) {
                            if ((float)COLLECTION[v->hidx]/COLLECTION_SIZE > parmmaxrf) continue;
                        }
#endif
                        v++; cnt++;
                    }
                    break;
                }
            }
            *vcntp=cnt; *voffp=off; off+=cnt;
            vcntp++; voffp++;
            if (parmcollmfntag) { *vcmfp=collhitmfn; vcmfp++; }
            count++;
            //if (parmtell) if (count%parmtell == 0) fprintf(stderr,"++ %"_LD_"\n",count);
if (parmtell2) if (count%parmtell2 == 0) fprintf(stderr,"++ %"_LD_"/%"_LD_" \n",(count-1)/parmtell2+1,div);

        }
        if (parmtell) /*if (count%parmtell == 0)*/ fprintf(stderr,"++ %"_LD_" term vectors loaded \n",count);


        /* close */
//        dbxflush(colvdb); /* close/flush */
    }
    
#if WHENFUN
    return 0;
}
#endif
