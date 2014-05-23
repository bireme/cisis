
            /* convert hitmfn to its original mfn */
            collhitmfn=hitmfn;

            /* get correspoding vector */
            v=VECTORS+VECTOFF[hitmfn-1]; cnt=VECTCNT[hitmfn-1];
            if (parmcollmfntag) collhitmfn=VECTCMF[hitmfn-1];
