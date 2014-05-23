LONGX hashzero(char *table, LONGX maxprim, int tabwidth);
char *hashalloc(LONGX classes, int tabwidth, LONGX *maxprimp);
LONGX hashindex(char *table, LONGX maxprim, int tabwidth, char *keyp, int keylen, int *foundp, int installit);

char *bsrchalloc(LONGX classes, int tabwidth, LONGX *tabentries);
LONGX bsrchstore(char *table, LONGX classes, LONGX *tabentries, int tabwidth, char *keyp, int keylen);
LONGX bsrchindex(char *table, LONGX tabentries, int tabwidth, char *keyp, int keylen, int *foundp);

