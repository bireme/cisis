/**
*  Aplica uma transformacao XSLT de um xml proveniente de um registro Isis e
*  mostra o resultado da mesma na saida padrao ou arquivo de saida ou ainda em
*  campo de registro Isis.
*
*  Nota: requer que a biblioteca libxslt (The XSLT C library for GNOME) esteja
*        instalada na maquina.
*
*  Uso:  "mx <dbname> proc='Gxslt'"
*
*  Formato:
*             xsl[#<rec_mode>][/<tag>|:<file>][=[@]<xsl_spec>]
*  onde:
*           #<rec_mode> - valor 1 ou 2 - indica o formato de saida do registro.
*           /tag - escreve resultado da transformacao no campo tag do registro.
*           <file> - escreve resultado da transformacao em arquivo de saida.
*           [@]<xsl_spec> - especificacao da transformacao. @ le de arquivo.
*
**/

#if PROCXSLT
        //xsl[#<rec_mode>][/<tag>|:<file>][=[@]<xsl_spec>]
        if (strncmp(p,"xsl", 3) == 0) {
            extern int xmlLoadExtDtdDefaultValue;

            char *args = p + 3;
            char *aux = NULL;
            char *spec = NULL;
            char *outFile = NULL;
            char *buffer = NULL;
            char *buffer2 = NULL;
            xmlDocPtr doc = NULL;
            xmlDocPtr res = NULL;
            xmlDocPtr check = NULL;
            xmlChar * doc_txt_ptr = NULL;
            int doc_txt_len = 0;
            xsltStylesheetPtr stylesheet = NULL;
            RECSTRU *arecp = NULL;
            RECSTRU *crecp = NULL;
            LONGX crec = 0;
            int tag = 0;
            int recMode = 1;

            aux = (char *) memchr(args, '=', strlen(args));
            if (aux != NULL) {
                spec = aux + 1;
                *aux = 0;
            }

            aux = args;
            if (*aux == '#') {
                int ch = (int)aux[1];

                recMode = ch - 48;
                if ((recMode != 1) && (recMode != 2)) {
                    fatal("fldupdat/procx/Gxsl/rec_mode");
                }
                aux += 2;
            }
            if (*aux == '/') {
                tag = atoi(aux + 1);
                if ((tag <= 0) || (tag > 99999)) {
                    fatal("fldupdat/procx/Gxsl/tag");
                }
            } else if (*aux == ':') {
                outFile = aux + 1;
            } else if (*aux != 0) {
                fatal("fldupdat/procx/Gxsl/parameter");
            }

            // Lê registro da base
#if CICPP
            crecp = new RECSTRU(cisisxp);
            if (!crecp) fatal("fldupdat/procx/upcrec");
            crecp->xrecalloc(sizeof(M0STRU));
            crecp->xrecord(p,0L);
#else
            for (crec=maxnrec; crec--; ) {
                if (!vrecp[crec]) {
                    break;
                }
            }
            if (crec < 0) {
                fatal("fldupdat/procx/crec");
            }
            recallok(crec,(LONGX)sizeof(M0STRU));
            record(crec, RECdbxp->dbxname, 0);
            crecp = vrecp[crec];
#endif /* CICPP */
            buffer = malloc(3 * MAXMFRL);
            if (buffer == NULL) {
                fatal("fldupdat/procx/Gxsl/xml/malloc/buffer");
            }

            buffer2 = malloc(MAXMFRL);
            if (buffer2 == NULL) {
                fatal("fldupdat/procx/Gxsl/xml/malloc/buffer2");
            }

            if (recMode == 1) {
                buffer = dumpRecord1(crecp, recp, buffer, buffer2);
            } else {
                buffer = dumpRecord2(crecp, recp, buffer, buffer2);
            }

            if (spec) {
                // Le e analisa documento XML de entrada
                //doc = xmlParseMemory(buffer, strlen(buffer));
                doc = xmlReadMemory(buffer, 2 * MAXMFRL, NULL, "ISO8859-1",
                                                          XML_PARSE_NOWARNING);
                if (doc == NULL) {
                    fatal("fldupdat/procx/Gxsl/xml/not wellformed");
                }

                // Le e analisa documento XSLT
                if (*spec == '@') {
                    stylesheet =
                             xsltParseStylesheetFile((const xmlChar *)(spec+1));
                } else {
                    xmlDocPtr xsltDoc = xmlReadMemory(spec + 1, strlen(spec + 1),
                                       NULL, "ISO8859-1", XML_PARSE_NOWARNING);
                    if (xsltDoc == NULL) {
                        fatal("fldupdat/procx/Gxsl/xslt/parse");
                    }
                    stylesheet = xsltParseStylesheetDoc(xsltDoc);
                    xmlFreeDoc(xsltDoc);
                }
                if (stylesheet == NULL) {
                    fatal("fldupdat/procx/Gxsl/stylesheet/parse");
                }

                // Ajusta parametros globais
                xmlSubstituteEntitiesDefault(1);
                xmlLoadExtDtdDefaultValue = 1;

                // Processa arquivo
                if ((stylesheet != NULL) && (doc != NULL)) {
                    res = xsltApplyStylesheet(stylesheet, doc, NULL);
                    if (res == NULL) {
                        fatal("fldupdat/procx/Gxsl/transformation");
                    }
                    if (xsltSaveResultToString(&doc_txt_ptr, &doc_txt_len,
                                               res,  stylesheet) == -1) {
                        fatal("fldupdat/procx/Gxsl/xml/tostring");
                    }
                }
            } else {/* spec */
                doc_txt_ptr = strdup(buffer);
            }

            if (tag > 0) {  // Escreve resultado em registro
                sprintf(buffer, "h%.5d %.8d %s", tag, strlen(doc_txt_ptr),
                                                                   doc_txt_ptr);
#if CICPP
                if (recp->xfldupdat(buffer)) {
                    fatal("fldupdat/procx/Gxsl/fldupdat");
                }
#else
                if (fldupdat(irec, buffer)) {
                    fatal("fldupdat/procx/Gxsl/fldupdat");
                }

#endif /* CICPP */
            } else if (outFile) { // Escreve resultado em arquivo
                int fd = 0;
                size_t size = strlen(doc_txt_ptr);

                if ((fd = CREAT(outFile, PERMIS)) <= 0) {
                    fatal("fldupdat/procx/Gxsl/create");
                }
                if (CIWRITE(fd, doc_txt_ptr, size) != size) {
                    fatal("fldupdat/procx/Gxsl/write");
                }
                CLOSE(fd);
            } else {  // Escreve resultado em registro na standard output
#if CICPP
                if (recp->xfldupdat("d*")) {
                    fatal("fldupdat/procx/Gxsl/fldupdat");
                }
#else
                if (fldupdat(irec, "d*")) {
                    fatal("fldupdat/procx/Gxsl/fldupdat");
                }
#endif /* CICPP */
                printf("%s\n", doc_txt_ptr);
            }

            // Apaga registro da memoria
#if CICPP
            delete crecp;
#else
            FREE(vrecp[crec]);
            vrecp[crec] = NULL;
            nrecs--;
#endif /* CICPP */

            // Libera memoria
            free(buffer);
            free(buffer2);
            free(doc_txt_ptr);

            if (spec) {
                xmlFreeDoc(doc);
                xmlFreeDoc(res);
                xmlFreeDoc(check);

                xsltFreeStylesheet(stylesheet);
                xsltCleanupGlobals();
                xmlCleanupParser();
            }

            return NULL;
        }
        else
#endif /* PROCXSLT */
