   			somaprod=0; /* init sum of products of corresponding terms */
            somaq=0;    /* init sum of squares for collection doc terms */
            for (; cnt--; v++) {
                float W;
                if (v->hidx < 0 || v-> hidx >= readnterms) fatal("wtrig2/VW/hidx");

                somaq += v->weight * v->weight; /* sum of squares for collection doc terms */

                W=WEIGHT[v->hidx];
                if (!W) continue;

                somaprod += W * v->weight; /* sum of products of corresponding terms */
            }

            simil=0;
            if (somaprod) {

                float denom = sqrt( Wsomaq * somaq);  /* compute */

			    if (denom > 0) simil=somaprod/denom;    /* compute */

                if (simil > 1.0) simil=1.0;   /* esteban, 09/12/2003 */

            } /* end if somaprod */
