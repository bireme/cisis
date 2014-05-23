/* ----------------------------------------------------------------- CIXML.H */

/* ------------------------------------------------------------- enumeration */
typedef enum {
    CIXML_ERROR_OK,
    CIXML_ERROR_ALLOC,
   CIXML_ERROR_QTT
} CIXML_ERROR;

typedef enum {
    CIXML_STYLE_GENERIC,
    CIXML_STYLE_FULL,
    CIXML_STYLE_SPECIFIED,
    CIXML_STYLE_FIXED_NOSF, /* NOSF: means "no subfield" handle */
    CIXML_STYLE_ALL,
   CIXML_STYLE_FIXED,
   CIXML_STYLE_QTT
} CIXML_STYLE;

/* -------------------------------------------------------------- structures */
typedef struct {
    int tag;
   char subfield;
   char *name;
} FIELD_ELEMENT;

typedef struct {
    LONGX qtt;
    FIELD_ELEMENT *list;
} FIELD_ELEMENT_LIST;

typedef struct {
    CIXML_ERROR errorCode;
    char *recordElement;
   char *mfnAttribute;
   char *fieldElement;
   char *tagAttribute;
   char *nameAttribute;
   char *occElement;
   char *subfieldElement;
   char *idAttribute;
   CIXML_STYLE style;
    EFC_SPLIT_LINES fnt;
   FIELD_ELEMENT_LIST i2x;
   char *p;
} CIXML_STRUCT;

/* -------------------------------------------------------------- prototypes */
CIXML_ERROR cixmlLoad ( char *i2x, CIXML_STRUCT *ciXml, CIXML_STYLE default_style);
void cixmlRun ( RECSTRU *recp, CIXML_STRUCT *ciXml, char *outputBuffer );
void cixmlFree ( CIXML_STRUCT *ciXml );

