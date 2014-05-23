/**
* Funcoes utilizadas pelo modulo ciupdxslt.c para transformacao XSLT.
**/

#if PROCXSLT

struct TagOcc {
    int tag;
    int occ;
    struct TagOcc *next;
};

typedef struct TagOcc OccLst;

int getOcc(OccLst **top,
           int tag) {
    OccLst *current = NULL;

    if (tag <= 0) {
        fatal("cidump/getOcc/tag <= 0");
    }
    for (current = *top; current; current = current->next) {
        if (current->tag == tag) {
            current->occ++;
            break;
        }
    }
    if (current == NULL) {
        current = (OccLst *)malloc(sizeof(OccLst));
        if (current == NULL) {
            fatal("cidump/getOcc/malloc");
        }
        current->tag = tag;
        current->occ = 1;
        current->next = *top;
        *top = current;
    }

    return current->occ;
}

void deleteList(OccLst **top) {
    OccLst *current = *top;
    OccLst *next = NULL;

    while(current) {
        next = current->next;
        free(current);
        current = next;
    }
    *top = NULL;
}

char *dumpRecord1(RECSTRU *crecp,
                  RECSTRU *recp,
                  char *areap,
                  char *area2p) {

    if ((crecp != NULL) && (recp != NULL)  &&
        (areap != NULL) && (area2p != NULL) && (MFRstatus == ACTIVE)) {

        int xdir = 0;
        int loop = 0;
        int xocc = 0;
        int xtag = 0;
        FFI xlen = 0;
        char stag = 0;
        int socc = 0;
        int spos = 0;
        FFI slen = 0;
        char *p = areap;
        char *q = area2p;
        char *cur = NULL;
        char *f = NULL;
        char *xxconvent[256];
        RECSTRU *arecp = recp;
        OccLst *ftop = NULL;
        OccLst *stop = NULL;

        *p = '\0';

        /* chars to entities */
        memset(xxconvent, 0x00, sizeof(xxconvent));
        xxconvent['&']="&amp;";
        xxconvent['<']="&lt;";
        xxconvent['>']="&gt;";
        xxconvent['"']="&quot;";
        xxconvent['\'']="&#39;";
        xxconvent['|']="%7C";

        // registro de controle
        recp = crecp;
        sprintf(p, "<masterfile name=\"%s\" nxtmfn=\"%"_LD_"\" mftype=\"%d\" ewlock=\"%"_LD_"\">\n",
                RDBname, MF0nxtmfn, MF0mftype, MF0mfcxx3);
        p += strlen(p);
        recp = arecp;

        sprintf(p, "<record mfn=\"%"_LD_"\" nvf=\"%d\" base=\"%"_LD_"\" len=\"%"_LD_"\" status=\"%s\" rclock=\"%d\">\n",
                MFRmfn, MFRnvf, (LONGX)MFRbase, (LONGX)MFRmfrl,
                (MFRstatus == ACTIVE) ? "active" : "deleted", REClock);
        p += strlen(p);

        for (xdir = 0, loop = MFRnvf; loop--; xdir++) {
            xtag = DIRtag(xdir);
            xlen = DIRlen(xdir);
            xocc = getOcc(&ftop, xtag);
            stag = 0;
            spos = 0;
            stop = NULL;

            sprintf(p, "<field iocc=\"%d\" tag=\"%u\" occ=\"%d\" len=\"%"_LD_"\">",
                    xdir+1, xtag, xocc, (LONGX)xlen);
            p += strlen(p);

            cur = f = FIELDP(xdir);

            for (; xlen--; cur++) {
                if (*cur == '^') {
                    if (stag > 0) {
                        socc = getOcc(&stop, stag);
                        slen = (int)((cur - f) - spos - 1);
                        sprintf(p, "<subfield tag=\"%c\" occ=\"%d\" pos=\"%d\" len=\"%"_LD_"\">",
                                stag, socc, spos, slen);
                        p += strlen(p);
                        area2p[slen] = 0;
                        strcpy(p, area2p);
                        p += strlen(p);
                        strcpy(p, "</subfield>");
                        p += strlen(p);
                    }
                    stag = *++cur;
                    xlen--;
                    spos = (int)(cur - f);
                    q = area2p;
                } else {
                    if (stag > 0) {
                        if (xxconvent[*cur]) {
                            strcpy(q, xxconvent[*cur]);
                            q += strlen(q);
                        } else {
                            *q++ = (*cur);
                        }
                    } else {
                        if (xxconvent[*cur]) {
                            strcpy(p,xxconvent[*cur]);
                            p += strlen(p);
                        } else {
                            *p++ = (*cur);
                        }
                    }
                }
            }
            if (stag > 0) {
                socc = getOcc(&stop, stag);
                slen = (int)((cur - f) - spos - 1);
                sprintf(p, "<subfield tag=\"%c\" occ=\"%d\" pos=\"%d\" len=\"%"_LD_"\">",
                        stag, socc, spos, slen);
                p += strlen(p);
                area2p[slen] = 0;
                strcpy(p, area2p);
                p += strlen(p);
                strcpy(p, "</subfield>");
                p += strlen(p);
            }
            sprintf(p, "</field>\n");
            p += strlen(p);
        }

        strcpy(p,"</record>\n");
        p += strlen(p);
        strcpy(p, "</masterfile>");

        deleteList(&stop);
        deleteList(&ftop);
    }

    return areap;
}

char * dumpRecord2(RECSTRU *crecp,
                   RECSTRU *recp,
                   char *areap,
                   char *area2p) {

    if ((crecp != NULL) && (recp != NULL)  &&
        (areap != NULL) && (area2p != NULL) && (MFRstatus == ACTIVE)) {

        int xdir = 0;
        int loop = 0;
        int xocc = 0;
        int xtag = 0;
        FFI xlen = 0;
        char stag = 0;
        int socc = 0;
        int spos = 0;
        FFI slen = 0;
        char *p = areap;
        char *q = area2p;
        char *cur = NULL;
        char *f = NULL;
        char *xxconvent[256];
        RECSTRU *arecp = recp;
        OccLst *ftop = NULL;
        OccLst *stop = NULL;

        *p = '\0';

        /* chars to entities */
        memset(xxconvent, 0x00, sizeof(xxconvent));
        xxconvent['&']="&amp;";
        xxconvent['<']="&lt;";
        xxconvent['>']="&gt;";
        xxconvent['"']="&quot;";
        xxconvent['\'']="&#39;";
        xxconvent['|']="%7C";

        sprintf(p,"<isis>\n");
        p += strlen(p);

        // registro de controle
        recp = crecp;
        sprintf(p, "<item type=\"masterfile\" name=\"%s\" nxtmfn=\"%"_LD_"\" mftype=\"%d\" ewlock=\"%"_LD_"\"/>\n",
                RDBname, MF0nxtmfn, MF0mftype, MF0mfcxx3);
        p += strlen(p);
        recp = arecp;

        sprintf(p, "<item type=\"record\" mfn=\"%"_LD_"\" nvf=\"%d\" base=\"%"_LD_"\" len=\"%"_LD_"\" status=\"%s\" rclock=\"%d\"/>\n",
                MFRmfn, MFRnvf, (LONGX)MFRbase, (LONGX)MFRmfrl,
                (MFRstatus == ACTIVE) ? "active" : "deleted", REClock);
        p += strlen(p);

        for (xdir = 0, loop = MFRnvf; loop--; xdir++) {
            xtag = DIRtag(xdir);
            xlen = DIRlen(xdir);
            xocc = getOcc(&ftop, xtag);
            stag = 0;
            spos = 0;
            stop = NULL;

            sprintf(p, "<item type=\"field\" iocc=\"%d\" tag=\"%u\" occ=\"%d\" len=\"%"_LD_"\">",
                        xdir+1, xtag, xocc, (LONGX)xlen);
            p += strlen(p);
            cur = f = FIELDP(xdir);
            for (; xlen--; cur++) {
                if (*cur == '^') {
                    if (stag > 0) {
                        socc = getOcc(&stop, stag);
                        slen = (int)((cur - f) - spos - 1);
                        sprintf(p, "<item type=\"subfield\" tag=\"%d\" occ=\"%d\" dlm=\"%c\" socc=\"%d\" pos=\"%d\" len=\"%"_LD_"\">",
                                xtag, xocc, stag, socc, spos, slen);
                        p += strlen(p);
                        area2p[slen] = 0;
                        strcpy(p, area2p);
                        p += strlen(p);
                    }
                    strcpy(p, "</item>\n");
                    p += strlen(p);
                    stag = *++cur;
                    xlen--;
                    spos = (int)(cur - f);
                    q = area2p;
                } else {
                    if (stag > 0) {
                        if (xxconvent[*cur]) {
                            strcpy(q, xxconvent[*cur]);
                            q += strlen(q);
                        } else {
                            *q++ = (*cur);
                        }
                    } else {
                        if (xxconvent[*cur]) {
                            strcpy(p,xxconvent[*cur]);
                            p += strlen(p);
                        } else {
                            *p++ = (*cur);
                        }
                    }
                }
            }

            if (stag > 0) {
                socc = getOcc(&stop, stag);
                slen = (int)((cur - f) - spos - 1);
                sprintf(p, "<item type=\"subfield\" tag=\"%d\" occ=\"%d\" dlm=\"%c\" socc=\"%d\" pos=\"%d\" len=\"%"_LD_"\">",
                                xtag, xocc, stag, socc, spos, slen);
                p += strlen(p);
                area2p[slen] = 0;
                strcpy(p, area2p);
                p += strlen(p);
            }
            strcpy(p, "</item>\n");
            p += strlen(p);
        }
        strcpy(p,"</isis>");

        deleteList(&stop);
        deleteList(&ftop);
    }

    return areap;
}

#endif /* PROCXSLT */
