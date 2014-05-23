/* ----------------------------------------------------------------- CIXML.C */

/* ---------------------------------------------------------- C HEADER FILES */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
/* ------------------------------------------------------------ HEADER FILES */
#include "../cisis.h"
#include "easyfc.h"
#include "cixml.h"

/* -------------------------------------------------------------- ciXmlError */
CIXML_ERROR ciXmlError
(
    CIXML_STRUCT *ciXml,
   CIXML_ERROR errorCode
)
{
   return ciXml->errorCode = errorCode;
}
/* ----------------------------------------------------------------- i2xName */
char *i2xName
(
    FIELD_ELEMENT_LIST *i2x,
   int tag,
   char subfield
)
{
    int i;

    for (i = 0; i < i2x->qtt; i++ )
   {
        if ( i2x->list[i].tag == tag )
      {
            if ( i2x->list[i].subfield == subfield )
         {
            return i2x->list[i].name;
         }
      }
   }

   return NULL;
}

/* ---------------------------------------------------- subfieldAttributeXML */
char *subfieldAttributeXML
(
    CIXML_STRUCT *ciXml,
   int tag,
   char *p,
   LONGX *lasting
)
{
   LONGX localLasting;
   char *subfieldName;

    localLasting = *lasting;
    p++; localLasting--;
    if ( localLasting )
   {
    subfieldName = i2xName(&(ciXml->i2x),tag,*p);
      if ( subfieldName )
      {
         sprintf(ciXml->p," %s=\"",subfieldName);
        ciXml->p += strlen(ciXml->p);
      }
      else
      {
            sprintf(ciXml->p," %s%c=\"",isdigit(*p) ? "s": "",*p);
        ciXml->p += strlen(ciXml->p);
      }
      p++; localLasting--;

       for ( ; localLasting && *p != '^'; localLasting-- )
    {
            sprintf(ciXml->p,"%c",*p++);
        ciXml->p += strlen(ciXml->p);
       }

    sprintf(ciXml->p,"\"");
        ciXml->p += strlen(ciXml->p);
   }
   *lasting = localLasting;

   return p;
}

/* ------------------------------------------------------------------ occXML */
void occXML
(
    RECSTRU *recp,
    CIXML_STRUCT *ciXml,
   int fldDir    			/* field directory entry */
)
{
    char *p;
   LONGX lasting = 0L;

    sprintf(ciXml->p,"      <%s",ciXml->occElement);
    ciXml->p += strlen(ciXml->p);
   if ( ciXml->style != CIXML_STYLE_FIXED_NOSF )
   {
       for (lasting = DIRlen(fldDir),p = FIELDP(fldDir); lasting; )
    {
        if ( *p == '^' )
          {
        p = subfieldAttributeXML(ciXml,DIRtag(fldDir),p,&lasting);
           continue;
          }
            p++;
        lasting--;
       }
   }
   sprintf(ciXml->p,">");
    ciXml->p += strlen(ciXml->p);
   for (lasting = DIRlen(fldDir),p = FIELDP(fldDir); lasting; lasting--)
   {
    if ( *p == '^' )
      {
        if ( ciXml->style != CIXML_STYLE_FIXED_NOSF )
         {
            break;
         }
      }
      sprintf(ciXml->p,"%c",*p++);
    ciXml->p += strlen(ciXml->p);
   }
    sprintf(ciXml->p,"</%s>\n",ciXml->occElement);
    ciXml->p += strlen(ciXml->p);
}

/* -------------------------------------------------------- fieldsStyleFixed */
void fieldsStyleFixed
(
    RECSTRU *recp,
    CIXML_STRUCT *ciXml
)
{
    int line;
   int fldQtt;    			/* quantity of fields */
   int fldDir;    			/* field directory entry */
   int occDir;

   fldQtt = MFRnvf;
    for ( line = 0; line < ciXml->i2x.qtt; line++)
   {

    if ( !ciXml->i2x.list[line].tag )
      {
        continue;
      }
    if ( ciXml->i2x.list[line].subfield )
      {
        continue;
      }

        for (fldDir = 0; fldDir < fldQtt; fldDir++)
    {
        if ( DIRtag(fldDir) == ciXml->i2x.list[line].tag )
         {
            if ( ciXml->style == CIXML_STYLE_FIXED_NOSF )
            {
                    sprintf(ciXml->p,"   <%s %s=\"%d\" %s=\"%s\">\n",ciXml->fieldElement,ciXml->tagAttribute,ciXml->i2x.list[line].tag,ciXml->nameAttribute,ciXml->i2x.list[line].name);
                ciXml->p += strlen(ciXml->p);
            }
            else
            {
                    sprintf(ciXml->p,"   <%s>\n",ciXml->i2x.list[line].name);
            }
            ciXml->p += strlen(ciXml->p);
                for (occDir = fldDir; occDir < fldQtt; occDir++)
                {
                    if ( DIRtag(occDir) != ciXml->i2x.list[line].tag )
               {
                    continue;
                 }
                occXML(recp,ciXml,occDir);
            }
            if ( ciXml->style == CIXML_STYLE_FIXED_NOSF )
            {
                    sprintf(ciXml->p,"   </%s>\n",ciXml->fieldElement);
                ciXml->p += strlen(ciXml->p);
            }
            else
            {
                    sprintf(ciXml->p,"   </%s>\n",ciXml->i2x.list[line].name);
            }
            ciXml->p += strlen(ciXml->p);
            break;
            }
      } /* for */

      if ( fldDir >= fldQtt )
      {
            if ( ciXml->style == CIXML_STYLE_FIXED_NOSF )
            {
                sprintf(ciXml->p,"   <%s %s=\"%d\" %s=\"%s\"/>\n",ciXml->fieldElement,ciXml->tagAttribute,ciXml->i2x.list[line].tag,ciXml->nameAttribute,ciXml->i2x.list[line].name);
            ciXml->p += strlen(ciXml->p);
         }
         else
         {
            sprintf(ciXml->p,"   <%s/>\n",ciXml->i2x.list[line].name);
            ciXml->p += strlen(ciXml->p);
         }
      }

   } /* for */

}

/* ---------------------------------------------------------- fieldsStyleAll */
void fieldsStyleAll
(
    RECSTRU *recp,
    CIXML_STRUCT *ciXml
)
{
   int fldQtt;    			/* quantity of fields */
   int fldDir;    			/* field directory entry */
   int tag;
   int occDir;
   char *fieldName;

   fldQtt = MFRnvf;
    for (fldDir = 0; fldDir < fldQtt; fldDir++)
   {

        tag = DIRtag(fldDir);
      for ( occDir = 0; occDir < fldDir; occDir++)
      {
            if ( DIRtag(occDir) == tag )
         {
            break;
         }
      } /* for */
      if ( occDir < fldDir )
      {
        continue;
      }

    fieldName = i2xName(&(ciXml->i2x),tag,'\0');

        sprintf(ciXml->p,"   <%s %s=\"%d\"",ciXml->fieldElement,ciXml->tagAttribute,DIRtag(fldDir));
    ciXml->p += strlen(ciXml->p);
      if ( fieldName )
       {
            sprintf(ciXml->p," %s=\"%s\"",ciXml->nameAttribute,fieldName);
        ciXml->p += strlen(ciXml->p);
        }
        sprintf(ciXml->p,">\n");
    ciXml->p += strlen(ciXml->p);

        for (occDir = fldDir; occDir < fldQtt; occDir++)
        {
            if (DIRtag(occDir) != tag)
         {
            continue;
         }
        occXML(recp,ciXml,occDir);
        } /* for */

        sprintf(ciXml->p,"   </%s>\n",ciXml->fieldElement);
    ciXml->p += strlen(ciXml->p);

   } /* for */

}

/* ------------------------------------------------------ subfieldElementXML */
#define ELELENT_SUBFIELD_MAX	5

char *subfieldElementXML
(
    CIXML_STRUCT *ciXml,
   int tag,
   char *p,
   LONGX *lasting
)
{
   LONGX localLasting;
   char *subfieldName;
   char elementSubField[ELELENT_SUBFIELD_MAX+1];

    localLasting = *lasting;
    p++; localLasting--;
    if ( localLasting )
   {
    sprintf(ciXml->p,"\n      ");
    ciXml->p += strlen(ciXml->p);
        subfieldName = i2xName(&(ciXml->i2x),tag,*p);
      if ( ciXml->style != CIXML_STYLE_GENERIC && !subfieldName )
      {
        sprintf(elementSubField,"%s%c",isdigit(*p) ? "s" : "",*p);
         subfieldName = elementSubField;
      }
      if ( subfieldName )
      {
         sprintf(ciXml->p,"<%s>",subfieldName);
        ciXml->p += strlen(ciXml->p);
      }
      else
      {
            sprintf(ciXml->p,"<%s %s=\"%c\">",ciXml->subfieldElement,ciXml->idAttribute,*p);
        ciXml->p += strlen(ciXml->p);
      }
      p++; localLasting--;

       for ( ; localLasting && *p != '^'; localLasting-- )
    {
            sprintf(ciXml->p,"%c",*p++);
        ciXml->p += strlen(ciXml->p);
       }

    sprintf(ciXml->p,"</%s>",subfieldName ? subfieldName : ciXml->subfieldElement);
    ciXml->p += strlen(ciXml->p);
   }

   *lasting = localLasting;

   return p;
}

/* --------------------------------------------------------- fieldContentXML */
void fieldContentXML
(
    RECSTRU *recp,
    CIXML_STRUCT *ciXml,
   int fldDir    			/* field directory entry */
)
{
    char *p;
   LONGX lasting = 0L;
   int hasSubField = 0;

   for (lasting = DIRlen(fldDir),p = FIELDP(fldDir); lasting; )
   {
       if ( *p == '^' )
      {
        p = subfieldElementXML(ciXml,DIRtag(fldDir),p,&lasting);
        hasSubField = 1;
         continue;
      }
      else
      {
        sprintf(ciXml->p,"%c",*p);
        ciXml->p += strlen(ciXml->p);
      }
        p++;
      lasting--;
   }
   if ( hasSubField )
   {
    sprintf(ciXml->p,"\n   ");
    ciXml->p += strlen(ciXml->p);
   }
}

/* ------------------------------------------------------ fieldsStyleGeneric */
#define GENERIC_FIELD_MAX	10

void fieldsStyleGeneric
(
    RECSTRU *recp,
    CIXML_STRUCT *ciXml
)
{
   int fldQtt;    			/* quantity of fields */
   int fldDir;    			/* field directory entry */
   char *fieldName;
   char genericField[GENERIC_FIELD_MAX+1];

   fldQtt = MFRnvf;
    for (fldDir = 0; fldDir < fldQtt; fldDir++)
   {

    fieldName = i2xName(&(ciXml->i2x),DIRtag(fldDir),'\0');

      if ( ciXml->style == CIXML_STYLE_GENERIC )
      {
            sprintf(ciXml->p,"   <%s %s=\"%d\"",ciXml->fieldElement,ciXml->tagAttribute,DIRtag(fldDir));
        ciXml->p += strlen(ciXml->p);
          if ( fieldName )
           {
            sprintf(ciXml->p," %s=\"%s\"",ciXml->nameAttribute,fieldName);
            ciXml->p += strlen(ciXml->p);
            }
            sprintf(ciXml->p,">");
         ciXml->p += strlen(ciXml->p);

      }
      else
      {
        if ( !fieldName )
         {
            if ( ciXml->style == CIXML_STYLE_SPECIFIED )
            {
                continue;
            }
            sprintf(genericField,"v%d",DIRtag(fldDir));
                fieldName = genericField;
         }
         sprintf(ciXml->p,"   <%s>",fieldName);
        ciXml->p += strlen(ciXml->p);
      }

        fieldContentXML(recp,ciXml,fldDir);

        sprintf(ciXml->p,"</%s>\n",ciXml->style == CIXML_STYLE_GENERIC ? ciXml->fieldElement : fieldName);
    ciXml->p += strlen(ciXml->p);

   } /* for */

}

/* --------------------------------------------------------------- fieldsXML */
void fieldsXML
(
    RECSTRU *recp,
    CIXML_STRUCT *ciXml
)
{
    switch (ciXml->style)
   {

    case CIXML_STYLE_GENERIC:
    case CIXML_STYLE_FULL:
    case CIXML_STYLE_SPECIFIED:
        fieldsStyleGeneric(recp,ciXml);
        break;

    case CIXML_STYLE_ALL:
        fieldsStyleAll(recp,ciXml);
        break;

        case CIXML_STYLE_FIXED_NOSF:
        if ( !ciXml->i2x.qtt )
         {
                fieldsStyleAll(recp,ciXml);
                break;
         }
    case CIXML_STYLE_FIXED:
        fieldsStyleFixed(recp,ciXml);
        break;

   } /* switch */
}

/* --------------------------------------------------------------- cixmlLoad */
CIXML_ERROR cixmlLoad
(
    char *i2x,
   CIXML_STRUCT *ciXml,
   CIXML_STYLE default_style
)
{
   int line;
   FIELD_ELEMENT *auxFieldElement;
   char *p;

    ciXml->errorCode = CIXML_ERROR_OK;
    ciXml->recordElement = "record";
   ciXml->mfnAttribute = "mfn";
   ciXml->fieldElement = "field";
   ciXml->tagAttribute = "tag";
   ciXml->nameAttribute = "name";
   ciXml->occElement = "occ";
   ciXml->subfieldElement = "subfield";
   ciXml->idAttribute = "id";
   ciXml->style = default_style;
   memset(&(ciXml->fnt),0x00,sizeof(EFC_SPLIT_LINES));
   memset(&(ciXml->i2x),0x00,sizeof(FIELD_ELEMENT_LIST));
   ciXml->p = NULL;

    if ( efc_split_lines(&(ciXml->fnt),i2x) < 0L )
   {
    return ciXmlError(ciXml,CIXML_ERROR_ALLOC);
   }

   if ( ciXml->fnt.qtt )
   {
        ciXml->i2x.list = (FIELD_ELEMENT *)efc_new( (size_t)(sizeof(FIELD_ELEMENT) * ciXml->fnt.qtt) );
    if ( !ciXml->i2x.list )
       {
        return ciXmlError(ciXml,CIXML_ERROR_ALLOC);
       }
   }

    for (line = 0; line < ciXml->fnt.qtt; line++)
   {

    if ( strncmp(ciXml->fnt.list[line].text,"root=",5) == 0 )
      {
        ciXml->recordElement = ciXml->fnt.list[line].text+5;
         continue;
      }

    if ( strncmp(ciXml->fnt.list[line].text,"record=",7) == 0 )
      {
        ciXml->recordElement = ciXml->fnt.list[line].text+7;
         continue;
      }

    if ( strncmp(ciXml->fnt.list[line].text,"mfn=",4) == 0 )
      {
        ciXml->mfnAttribute = ciXml->fnt.list[line].text+4;
         continue;
      }

    if ( strncmp(ciXml->fnt.list[line].text,"field=",6) == 0 )
      {
        ciXml->fieldElement = ciXml->fnt.list[line].text+6;
         continue;
      }

    if ( strncmp(ciXml->fnt.list[line].text,"tag=",4) == 0 )
      {
        ciXml->tagAttribute = ciXml->fnt.list[line].text+4;
         continue;
      }

    if ( strncmp(ciXml->fnt.list[line].text,"name=",5) == 0 )
      {
        ciXml->nameAttribute = ciXml->fnt.list[line].text+5;
         continue;
      }

    if ( strncmp(ciXml->fnt.list[line].text,"occ=",4) == 0 )
      {
        ciXml->occElement = ciXml->fnt.list[line].text+4;
         continue;
      }

    if ( strncmp(ciXml->fnt.list[line].text,"subfield=",9) == 0 )
      {
        ciXml->subfieldElement = ciXml->fnt.list[line].text+9;
         continue;
      }

    if ( strncmp(ciXml->fnt.list[line].text,"id=",3) == 0 )
      {
        ciXml->idAttribute = ciXml->fnt.list[line].text+3;
         continue;
      }

    if ( strncmp(ciXml->fnt.list[line].text,"style=",6) == 0 )
      {
        if ( strcmp(ciXml->fnt.list[line].text+6,"generic") == 0 )
       {
            ciXml->style = CIXML_STYLE_GENERIC;
          }
        if ( strcmp(ciXml->fnt.list[line].text+6,"full") == 0 )
       {
            ciXml->style = CIXML_STYLE_FULL;
          }
        if ( strcmp(ciXml->fnt.list[line].text+6,"specified") == 0 )
       {
            ciXml->style = CIXML_STYLE_SPECIFIED;
          }
        if ( strcmp(ciXml->fnt.list[line].text+6,"old-fixed") == 0  ||
              strcmp(ciXml->fnt.list[line].text+6,"edit") == 0 )
       {
            ciXml->style = CIXML_STYLE_FIXED_NOSF;
          }
        if ( strcmp(ciXml->fnt.list[line].text+6,"all") == 0 )
       {
            ciXml->style = CIXML_STYLE_ALL;
          }
        if ( strcmp(ciXml->fnt.list[line].text+6,"fixed") == 0 )
       {
            ciXml->style = CIXML_STYLE_FIXED;
          }
            continue;
      }

        auxFieldElement = &(ciXml->i2x.list[ciXml->i2x.qtt]);
        auxFieldElement->tag = atoi(ciXml->fnt.list[line].text);
      for ( p = ciXml->fnt.list[line].text; isdigit(*p); p++);
        if ( *p == '^' )
      {
            auxFieldElement->subfield = *++p;
         p++;
      }
      else
      {
        auxFieldElement->subfield = '\0';
      }
      for ( ; *p == ' '; p++);
        auxFieldElement->name = p;
      ciXml->i2x.qtt++;

   }

   return ciXmlError(ciXml,CIXML_ERROR_OK);
}

/* ---------------------------------------------------------------- cixmlRun */
void cixmlRun
(
    RECSTRU *recp,
   CIXML_STRUCT *ciXml,
   char *outputBuffer
)
{
    ciXml->p = outputBuffer;

    sprintf(ciXml->p,"<%s %s=\"%"_LD_"\">\n",ciXml->recordElement,ciXml->mfnAttribute,MFRmfn);
   ciXml->p += strlen(ciXml->p);
   fieldsXML(recp,ciXml);
    sprintf(ciXml->p,"</%s>\n",ciXml->recordElement);
   ciXml->p += strlen(ciXml->p);
}

/* --------------------------------------------------------------- cixmlFree */
void cixmlFree
(
   CIXML_STRUCT *ciXml
)
{
    efc_free(ciXml->i2x.list);
    efc_split_lines_free(&(ciXml->fnt));
}

