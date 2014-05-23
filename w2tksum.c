
            if (found) {
                /* similarity computation */
                Wsomaq += weight * weight;  /* sum of squares for document terms */
            }
            else { /* if (lognotfound) ... */
                if (parmwsoma) {
                    /* similarity computation */
                    Wsomaq += weight * weight;  /* sum of squares for document terms */
                }
                continue;
            }

