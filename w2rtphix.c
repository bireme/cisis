            VECTEL *v;
            LONGX cnt=0;

            if (*colxdb) {
                hitmask = hitbitmask[hitmfn&BY8RMASK];
                hitoffset = hitmfn/8;
                if (hitoffset >= hitbytes) break;
                if (!(HITS[hitoffset] & hitmask)) continue;
            }
