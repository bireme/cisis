/* -------------------------------------------------------------- XIS_COMP.C */

/* ---------------------------------------------------------- C HEADER FILES */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/* ------------------------------------------------------------ HEADER FILES */
/* include "cisis.h" */
#include "easyfc.h"
#include "xis_comp.h"
/* ----------------------------------------------------------------- defines */
#define CPL_CMD_BEGIN		'<'
#define CPL_CMD_END			'>'
#define CPL_ASSIGN			'='
#define CPL_2QUOTE			'\"'

#define CPL_CMD_CLOSE		'/'
/* ------------------------------------------------------------------ global */
char *cplSpace = " \t\r\n";								/* space list */
char *cplSeparator = "<>=\"";								/* separator list */
CPL_IDENTIFIER cplElement[ELEMENT_LIST_QTT];			/* element list */
CPL_IDENTIFIER cplAttribute[ATTRIBUTE_LIST_QTT];	/* attribute list */
CPL_IDENTIFIER cplAtValue[ATVALUE_LIST_QTT];			/* attribute value list */
CPL_IDENTIFIER cplCommentBegin[COMMENT_LIST_QTT];	/* comment begin list */
CPL_IDENTIFIER cplCommentEnd[COMMENT_LIST_QTT];		/* comment end list */

/* ============================================================== cplIsSpace */
char cplIsSpace(char ch)		/* character */
{
    char *find;		/* auxiliary find pointer */

   /* Check if the character is considered a space */
    find = strchr(cplSpace,ch);
    if (find) return *find;
    return '\0';

} /* cplIsSpace */

/* ================================================================ cplError */
CPL_ERROR cplError(CPL_STRUCT *program,	/* program structure */
                         CPL_ERROR errorCode,	/* error code */
                   char *text)				/* complement message text */
{
    char *p;		/* auxiliary string loop pointer */

   /* Set error code and complement text information */
    program->error.code = errorCode;
    sprintf(program->error.info,"%-.*s",EFC_ERROR_INFO_MAX,text);

   /* Set end of command with string terminator */
   for (p = program->error.info; *p; p++) {
    if (*p == CPL_CMD_END) *(p+1) = '\0';
      if (!cplIsSpace(*p)) continue;
        *p = '\0';
        break;
   }

   /* Return error code */
   return errorCode;

} /* cplError */

/* =========================================================== cplSkipSpaces */
char *cplSkipSpaces(char *p)		/* character pointer */
{

   /* Skip space characters, return next non-space character */
    while (cplIsSpace(*p)) p++;
   return p;

} /* cplSkipSpaces */

/* ======================================================== cplSeparatorFind */
char cplSeparatorFind(char ch)		/* current script position */
{
    char *find;		/* auxiliary find pointer */

   /* Find separator */
   find = strchr(cplSeparator,ch);
   if (!find) return '\0';
   return *find;

} /* cplSeparatorFind */

/* ================================================================ cplIsEoi */
BOOLEAN cplIsEoi(char *p)		/* current script position */
{

   /* Check if it is end of identifier */
   return (BOOLEAN)(!*p || cplIsSpace(*p) || cplSeparatorFind(*p));

} /* cplIsEoi */

/* ============================================================== cplStrNDup */
char *cplStrNDup(char *from,		/* source string buffer */
                      int len)			/* quantity of characters to copy */
{
    char *dupl;		/* new string buffer pointer */

   /* Allocate a new string buffer, copy len bytes of source buffer */
//printf("cplStrNDup/antes do efc_new [%s]\n", from);
    dupl = (char *)efc_new(len+1);
//printf("cplStrNDup/depois do efc_new\n");
   if (dupl) {
    memmove(dupl,from,len);
      dupl[len] = '\0';
    }

   /* Return the new string */
   return dupl;

} /* cplStrNDup */

/* ========================================================== cplSkipComment */
char *cplSkipComment(char *p,								/* script instructions */
                            CPL_IDENTIFIER *commentEnd)	/* coment end identifier */
{

    /* Find comment end */
    for ( ; *p; p++) {
    if (*p != *(commentEnd->text)) continue;
        if (strncmp(p,commentEnd->text,commentEnd->len) != 0) continue;
        p += commentEnd->len;
        break;
   }

    /* Return next character after comment */
   return p;

} /* cplSkipComment */

/* =========================================================== cplIdentifier */
int cplIdentifier(CPL_IDENTIFIER identList[],	/* identifier list */
                        int listQtt,
                       char *p)						/* current script position */
{
    int ident;		/* Identifier index */

   /* Find element, skip first undefined element identList[0] */
    for (ident = 1; ident < listQtt; ident++) {
        if (strncmp(p,identList[ident].text,identList[ident].len) != 0) continue;
      if (cplIsEoi(p+identList[ident].len)) break;
    }

   /* Return element index, index==listQtt means not found */
   return ident;

} /* cplIdentifier */

/* ================================================================ cplToken */
char *cplToken(char *p,							/* current script position */
                    CPL_TOKEN_STRU *token)		/* token struct return */
{
    char separator;
   CPL_COMMENT i;

    /* Skip space characters to find next token */
    token->statment = token->text = p;
    p = cplSkipSpaces(p);
   if (!*p) {
        token->type = TOKEN_END;
    return p;
   }
    token->text = p;

   /* Check if it is a separator */
   separator = cplSeparatorFind(*p);

   switch (separator) {

   case CPL_CMD_BEGIN:
       /* Skip spaces */
        p = cplSkipSpaces(++p);
       if (!*p) {
            token->type = TOKEN_END;
        return p;
       }

    /* Check if it is an element */
    token->index.element = cplIdentifier(cplElement,ELEMENT_LIST_QTT,p);
       if (token->index.element < ELEMENT_LIST_QTT) {
            token->type = TOKEN_ELEMENT;
            return p+cplElement[token->index.element].len;
       }

    /* Check if it is an element close */
        if (*p == CPL_CMD_CLOSE) {
           token->index.element = cplIdentifier(cplElement,ELEMENT_LIST_QTT,++p);
        if (token->index.element < ELEMENT_LIST_QTT) {
                token->type = TOKEN_ELCLOSE;
                return p+cplElement[token->index.element].len;
        }
       }

       /* Check if it is a comment */
      for (i = 0; i < COMMENT_LIST_QTT; i++) {
            if (strncmp(p,cplCommentBegin[i].text,cplCommentBegin[i].len) == 0) {
          p = cplSkipComment(p+cplCommentBegin[i].len,&(cplCommentEnd[i]));
            return cplToken(p,token);
        }
      }
    break;

   case CPL_CMD_END:
    token->type = TOKEN_ELEND;
      return ++p;

   case CPL_ASSIGN:
    token->type = TOKEN_ASSIGN;
      return ++p;

   } /* switch */

   /* Check if it is an attribute */
   token->index.attribute = cplIdentifier(cplAttribute,ATTRIBUTE_LIST_QTT,p);
   if (token->index.attribute < ATTRIBUTE_LIST_QTT) {
        token->type = TOKEN_ATTRIBUTE;
        return p+cplAttribute[token->index.attribute].len;
   }

   /* Return no token found */
    token->type = TOKEN_LIST_QTT;
   return p;
}

/* ============================================================= cplStatment */
char *cplStatment(char *p,					/* current script position */
                        CPL_COMMAND *cmd)		/* command structure */
{
    char *text;					/* statment begin */
   int len;						/* statment lenght */
   CPL_TOKEN_STRU token;	/* token structure */

   /* Find end of statment, count number of characters */
    for (text = p,len = 0; *p; p++,len++) {
        if (*p != CPL_CMD_BEGIN) continue;
        cplToken(p,&token);
        if (token.type == TOKEN_ELCLOSE && token.index.element == cmd->element) break;
    }

   /* Allocate statment, copy text */
    cmd->text = cplStrNDup(text,len);

   /* Return next script position */
   return p;

} /* cplStatment */

/* ================================================================= cplWord */
char *cplWord(char *p,									/* current script position */
                  CMD_ATTRIBUTE_STRU *attribute)		/* command structure */
{
    char *text;					/* statment begin */
   int len;						/* statment lenght */

   /* Find end of word, count number of characters */
    for (text = p,len = 0; !cplIsEoi(p); p++,len++);

   /* Allocate statment, copy text */
    attribute->text = efc_strrepl(attribute->text,cplStrNDup(text,len));

   /* Return next script position */
   return p;

} /* cplWord */

/* ========================================================== cplDoubleQuote */
char *cplDoubleQuote(char *p,										/* current script position */
                            CMD_ATTRIBUTE_STRU *attribute)		/* command structure */
{
    char *text;					/* statment begin */
    char *dup;
   int len;						/* statment lenght */

   /* Find end of work, count number of characters */
    for (text = p,len = 0; *p && *p != CPL_2QUOTE; p++,len++);

   /* Allocate statment, copy text */
   dup = cplStrNDup(text,len);   /* Code Guard */
    attribute->text = efc_strrepl(attribute->text,dup);
    efc_free(dup);

   /* Return next script position */
   return *p ? ++p : p;

} /* cplDoubleQuote */

/* ======================================================== cplPrintCmdBegin */
void cplPrintCmdBegin(BOOLEAN convLT)		/* convert less than character */
{

    /* Print less than character or related HTML entity */
    if (convLT) printf("&lt;");
   else printf("%c",CPL_CMD_BEGIN);

} /* cplPrintCmdBegin */

/* =========================================================== cplPrintScope */
void cplPrintScope(int scope)		/* command scope */
{
    int i;		/* auxiliary loop index */

   /* Print scope spaces */
    for (i = 0; i < scope; i++) printf("  ");

} /* cplPrintScope */

/* ========================================================= cplPrintCommand */
void cplPrintCommand(CPL_COMMAND *cmd,		/* command structure */
                            BOOLEAN convLT)		/* convert less than character */
{
    CPL_ATTRIBUTE i;		/* attribute index */

   /* Show command information */
    cplPrintCmdBegin(convLT);
   printf("%s",cplElement[cmd->element].text);
    for (i = 0; i < ATTRIBUTE_LIST_QTT; i++)
    if (cmd->attributeList[i].text)
        printf(" %s%c%c%s%c",cplAttribute[i].text,CPL_ASSIGN,CPL_2QUOTE,cmd->attributeList[i].text,CPL_2QUOTE);
   printf("%c",CPL_CMD_END);
   if (cmd->text) printf("%s",cmd->text);

} /* cplPrintCommand */

/* ==================================================== cplPrintCommandClose */
void cplPrintCommandClose(CPL_COMMAND *cmd,		/* command structure */
                                  BOOLEAN convLT)			/* convert less than character */
{

   /* Show command close */
    cplPrintCmdBegin(convLT);
   printf("%c%s%c\n",CPL_CMD_CLOSE,cplElement[cmd->element].text,CPL_CMD_END);

} /* cplPrintCommandClose */

/* ================================================================ cplPrint */
void cplPrint(CPL_COMMAND *cmd,		/* command structure */
                  int scope)            /* scope counter */
{

    /* Loop all commands, call sub-command */
   printf("\n");
    for ( ; cmd; cmd = cmd->next) {
    cplPrintScope(scope);
        cplPrintCommand(cmd,FALSE);
    if (cmd->sub) {
        cplPrint(cmd->sub,scope+1);
        cplPrintScope(scope);
      }
        cplPrintCommandClose(cmd,FALSE);
   }

} /* cplPrint */

/* ========================================================== cplIncludeFile */
char *cplIncludeFile(CPL_STRUCT *program,				/* program structure */
                        CPL_COMMAND *cmd,					/* list first command */
                     CPL_ERROR *errorCode)			/* error code */
{
    char *p;
    char includeName[PATH_NAME_LEN+1];	/* auxiliary file name string */
   EFC_ERROR errorFile;

    /* Load include file content, original file name */
    p = efc_std_filecontent(&errorFile,cmd->text);
    if (p) {
        return p;
    }

    /* Verify path lenght + file name lenght */
   if (strlen(program->pathSource) + strlen(cmd->text) > PATH_NAME_LEN) {
        *errorCode = cplError(program,CPL_ERROR_PATHNAMELEN,cmd->text);
        return NULL;
   }

    /* Load include file content, source path */
   sprintf(includeName,"%s%s",program->pathSource,cmd->text);
    p = efc_std_filecontent(&errorFile,includeName);
    if (!p) {
        *errorCode = cplError(program,CPL_ERROR_NOTFOUND,includeName);
        return NULL;
    }

    /* Return allocated content file buffer */
   return p;

} /* cplIncludeFile */

/* =========================================================== cplIsCompiled */
BOOLEAN cplIsCompiled
(
    CPL_STRUCT *program,		/* program structure */
    char *scriptName		  	/* script name */
)
{
    int lenDiff;		/* lenght difference */

   /* script name minus compiled extension, it can't compare the string tail */
    lenDiff = strlen(scriptName) - strlen(program->compiledExt);
    if (lenDiff < 0) return FALSE;

   /* compare the script name tail with the default compiled extension */
   return (BOOLEAN)(strcmp(scriptName+lenDiff,program->compiledExt) == 0);

} /* cplIsCompiled */

/* ========================================================== cplLoadCommand */
CPL_COMMAND *cplLoadCommand(CPL_COMMAND *add_cmd,		/* command to be added */
                                     CPL_CMD_LIST *cmdList)		/* command list */
{
   CPL_COMMAND *cmd;		/* new allocated area */

   /* Allocate area */
//printf("cplLoadCommand/antes do efc_new\n");
   cmd = (CPL_COMMAND *)efc_new(sizeof(CPL_COMMAND));
//printf("cplLoadCommand/antes do efc_new\n");
   if (!cmd) return NULL;

   /* Copy command data */
   memmove(cmd,add_cmd,sizeof(CPL_COMMAND));

   /* Append on the list of commands */
   if (!cmdList->first) cmdList->first = cmd;
   if (cmdList->last) cmdList->last->next = cmd;
   cmd->prev = cmdList->last;
   cmdList->last = cmd;

   /* Return allocated area pointer */
   return cmd;

} /* cplLoadCommand */

/* ======================================================== cplLoadAttribute */
char *cplLoadAttribute(char *p,								/* instructions to be compiled */
                              CPL_STRUCT *program,				/* program structure */
                          CPL_COMMAND *cmd,					/* list first command */
                          CPL_ATTRIBUTE attributeIndex,	/* attribute index */
                          CPL_ERROR *errorCode)				/* error code */
{
   CPL_TOKEN_STRU token;			/* token structure */

    /* Get assign token */
   p = cplToken(p,&token);
    if (token.type != TOKEN_ASSIGN) {
        *errorCode = cplError(program,CPL_ERROR_INVALID,token.text);
        return p;
    }

    /* Get next token */
   p = cplToken(p,&token);
    if (*p == CPL_2QUOTE) {
    token.text = ++p;
        p = cplDoubleQuote(p,&(cmd->attributeList[attributeIndex]));
   } else
        p = cplWord(token.text,&(cmd->attributeList[attributeIndex]));

    /* Set attribute value */
   cmd->attributeList[attributeIndex].atValue = cplIdentifier(cplAtValue,ATVALUE_LIST_QTT,token.text);

   return p;

} /* cplLoadAttribute */

/* ================================================================= cplLoad */
char *cplLoad(char *script,						/* instructions to be compiled */
              CPL_STRUCT *program,				/* program structure */
              CPL_ELEMENT headerElement,		/* caller command */
              CPL_COMMAND **first,				/* list first command */
              CPL_CMD_LIST *cmdList,			/* command list */
              CPL_ERROR *errorCode)				/* error code */
{
    char *p;								/* auxiliary loop pointer */
    char *includeText;				/* the include file content */
    char *pInclude;					/* auxiliary text pointer */
   CPL_TOKEN_STRU token;			/* token structure */
   CPL_TOKEN_STRU prevToken;		/* previous token */
   CPL_COMMAND cmd;					/* command structure */
   CPL_CMD_LIST subCmdList;		/* sub-command list */

   /* Start variables */
   *errorCode = CPL_ERROR_OK;
   *first = NULL;
    memset(&prevToken,0x00,sizeof(CPL_TOKEN_STRU));
   prevToken.type = TOKEN_ELCLOSE;
    memset(&cmd,0x00,sizeof(CPL_COMMAND));
    memset(cmdList,0x00,sizeof(CPL_CMD_LIST));

   /* Loop all script characters */
    for (p = script; *p; prevToken = token) {

    /* Get token, index and skip token lenght */
 //printf("cplLoad/antes do cplToken\n");
      p = cplToken(p,&token);
//printf("cplLoad/antes do cplToken\n");
    /* Evaluate token */
        switch (token.type) {

      case TOKEN_ATTRIBUTE:
        /* Check previous token */
            if (prevToken.type == TOKEN_ELEND) goto LABEL_TOKEN_STATMENT;
        if (prevToken.type != TOKEN_ELEMENT && prevToken.type != TOKEN_ATTRIBUTE) {
            *errorCode = cplError(program,CPL_ERROR_INVALID,prevToken.text);
            return p;
         }

        /* Load attribute */
         p = cplLoadAttribute(p,program,&cmd,token.index.attribute,errorCode);
         if (*errorCode) return p;
         break;

      case TOKEN_ELCLOSE:

        /* Load command instruction */
        if (cmd.element) cplLoadCommand(&cmd,cmdList);

        /* Check sub-command end */
        if (token.index.element == headerElement) {
               *first = cmdList->first;
            return cmd.element ? p : token.text;
         }

        /* [0.8] Check element close couple */
        if (token.index.element != cmd.element) {
            *errorCode = cplError(program,CPL_ERROR_INVALID,token.text);
            return p;
         }

        /* [4.1] Include element */
            if (cmd.element == ELEMENT_INCLUDE && *(cmd.text)) {

            /* Not compiled */
            if (!cplIsCompiled(program,cmd.text))
            {
                /* Load include file content */
//printf("cplLoad/ntes do cplIncludeFile\n");
                includeText = cplIncludeFile(program,&cmd,errorCode);
//printf("cplLoad/depois do cplIncludeFile\n");
                if (!includeText) return p;

                /* Load include file instructions, use sub-command as an auxiliar list */
                    pInclude = cplLoad(includeText,program,ELEMENT_LIST_QTT,&(cmd.sub),&subCmdList,errorCode);
                if (*errorCode) return pInclude;
//printf("cplLoad/antes do efc_free\n");
                /* Garbage collector */
#ifndef XIS_SERVER
             efc_free(includeText);
#endif
//printf("cplLoad/depois do efc_free\n");
            }
            /* Compiled */
            else
            {
                   /* Load compiled instructions, use sub-command as an auxiliar list */
                    *errorCode = cplLoadCompiled(cmd.text,program,&(cmd.sub),&subCmdList);
                    if (*errorCode)
               {
                    return p;
                 }
            }

            /* Append include commands in current command list */
            if (subCmdList.first) {
                cmdList->last->next = subCmdList.first;
                subCmdList.first->prev = cmdList->last;
               cmdList->last = subCmdList.last;
            }

         }

        /* Reset command container */
            memset(&cmd,0x00,sizeof(CPL_COMMAND));
//printf("cplLoad/break\n");
        break;

      case TOKEN_ELEMENT:
        /* Check previous token */
        if (prevToken.type != TOKEN_ELEND && prevToken.type != TOKEN_ELCLOSE) {
            *errorCode = cplError(program,CPL_ERROR_INVALID,prevToken.text);
            return p;
         }

        if (prevToken.type == TOKEN_ELEND) {
                p = cplLoad(token.text,program,cmd.element,&(cmd.sub),&subCmdList,errorCode);
              if (*errorCode) return p;
         } else
            cmd.element = token.index.element;

         break;

      case TOKEN_ELEND:
        /* End of element */
         if (prevToken.type == TOKEN_ELCLOSE) token = prevToken;
        break;

      case TOKEN_END:
        /* End of script */
         break;

      default:

            LABEL_TOKEN_STATMENT:

        /* Check if it is a statment */
        if (prevToken.type == TOKEN_ELEND &&
            cmd.element != ELEMENT_DO &&
            cmd.element != ELEMENT_FUNCTION &&
            cmd.element != ELEMENT_HL &&
            cmd.element != ELEMENT_ISISSCRIPT &&
            cmd.element != ELEMENT_LOOP &&
            cmd.element != ELEMENT_SECTION &&
            cmd.element != ELEMENT_UPDATE) {
            p = cplStatment(token.statment,&cmd);
                break;
            }

        /* Invalid token */
        *errorCode = cplError(program,CPL_ERROR_INVALID,token.text);
        return p;

      } /* switch */
//printf("cplLoad/proximo laco do loop\n");
   } /* for */
//printf("cplLoad/sai do loop xxx\n");
    /* Return no error */
   *first = cmdList->first;
   return p;

} /* cplLoad */

/* =========================================================== cplStartIdent */
void cplStartIdent(CPL_IDENTIFIER *ident,	/* identifier structure */
                         char *text)				/* identifier text */
{

   /* Set identifier values: text and text lenght */
   ident->text = text;
   ident->len = strlen(text);

} /* cplStartIdent */

/* ================================================================ cplStart */
void cplStart
(
    CPL_STRUCT *program		/* program structure */
)
{
   /* Check if it is the first call */
    if (program->cplStarted) return;

   /* Reset program structure */
   memset(&cplElement,	0x00,sizeof(CPL_IDENTIFIER)*ELEMENT_LIST_QTT);
   memset(&cplAttribute,0x00,sizeof(CPL_IDENTIFIER)*ATTRIBUTE_LIST_QTT);
   memset(&cplAtValue,	0x00,sizeof(CPL_IDENTIFIER)*ATVALUE_LIST_QTT);

   /* Set compiler elements */
    cplStartIdent(&(cplElement[ELEMENT_0]),				"");
    cplStartIdent(&(cplElement[ELEMENT_CALL]),			"call");
    cplStartIdent(&(cplElement[ELEMENT_CGITABLE]),		"cgitable");
    cplStartIdent(&(cplElement[ELEMENT_DEFINE]),			"define");
    cplStartIdent(&(cplElement[ELEMENT_DISPLAY]),		"display");
    cplStartIdent(&(cplElement[ELEMENT_DISPL]),			"displ");
    cplStartIdent(&(cplElement[ELEMENT_DO]),				"do");
    cplStartIdent(&(cplElement[ELEMENT_EXPORT]),			"export");
    cplStartIdent(&(cplElement[ELEMENT_EXTRACT]),		"extract");
    cplStartIdent(&(cplElement[ELEMENT_FIELD]),			"field");
    cplStartIdent(&(cplElement[ELEMENT_FILE]),			"file");
    cplStartIdent(&(cplElement[ELEMENT_FLOW]), 			"flow");
    cplStartIdent(&(cplElement[ELEMENT_FUNCTION]),		"function");
    cplStartIdent(&(cplElement[ELEMENT_HL]),				"hl");
    cplStartIdent(&(cplElement[ELEMENT_HTMLPFT]),		"htmlpft");
    cplStartIdent(&(cplElement[ELEMENT_INCLUDE]),		"include");
    cplStartIdent(&(cplElement[ELEMENT_ISISSCRIPT]),	"IsisScript");
    cplStartIdent(&(cplElement[ELEMENT_LABEL]),			"label");
    cplStartIdent(&(cplElement[ELEMENT_LIST]),			"list");
    cplStartIdent(&(cplElement[ELEMENT_LOOP]),			"loop");
    cplStartIdent(&(cplElement[ELEMENT_PARM]),			"parm");
    cplStartIdent(&(cplElement[ELEMENT_PFT]),				"pft");
    cplStartIdent(&(cplElement[ELEMENT_PROC]),      	"proc");
    cplStartIdent(&(cplElement[ELEMENT_RETURN]),			"return");
    cplStartIdent(&(cplElement[ELEMENT_SECTION]),		"section");
    cplStartIdent(&(cplElement[ELEMENT_TRACE]),			"trace");
    cplStartIdent(&(cplElement[ELEMENT_UPDATE]),			"update");
    cplStartIdent(&(cplElement[ELEMENT_WRITE]),			"write");
    /* 22.Feb.2001 */
    cplStartIdent(&(cplElement[ELEMENT_ISISXML]),		"isisxml");

   /* Set compiler attributes */
    cplStartIdent(&(cplAttribute[ATTRIBUTE_0]),			"");
    cplStartIdent(&(cplAttribute[ATTRIBUTE_ACTION]),	"action");
    cplStartIdent(&(cplAttribute[ATTRIBUTE_FROM]),		"from");
    cplStartIdent(&(cplAttribute[ATTRIBUTE_NAME]),		"name");
    cplStartIdent(&(cplAttribute[ATTRIBUTE_PREVIOUS]),	"previous");
    cplStartIdent(&(cplAttribute[ATTRIBUTE_SPLIT]),		"split");
    cplStartIdent(&(cplAttribute[ATTRIBUTE_TAG]),		"tag");
    cplStartIdent(&(cplAttribute[ATTRIBUTE_TASK]),		"task");
    cplStartIdent(&(cplAttribute[ATTRIBUTE_TYPE]),		"type");

   /* Set compiler attributes values */
    cplStartIdent(&(cplAtValue[ATVALUE_0]),				"");
    cplStartIdent(&(cplAtValue[ATVALUE_ACTAB]),			"actab");
    cplStartIdent(&(cplAtValue[ATVALUE_ADD]),				"add");
    cplStartIdent(&(cplAtValue[ATVALUE_APPEND]),			"append");
    cplStartIdent(&(cplAtValue[ATVALUE_BUFFERSIZE]),	"buffersize");
    cplStartIdent(&(cplAtValue[ATVALUE_CGI]),				"cgi");
    cplStartIdent(&(cplAtValue[ATVALUE_CGISFT]),			"^t");
    cplStartIdent(&(cplAtValue[ATVALUE_CGISFU]),			"^u");
    cplStartIdent(&(cplAtValue[ATVALUE_CGISFW]),			"^w");
    cplStartIdent(&(cplAtValue[ATVALUE_CHECK]),			"check");
    cplStartIdent(&(cplAtValue[ATVALUE_CIPAR]),			"cipar");
    cplStartIdent(&(cplAtValue[ATVALUE_CLOSE]),			"close");
    cplStartIdent(&(cplAtValue[ATVALUE_CONVERT]),		"convert");
    cplStartIdent(&(cplAtValue[ATVALUE_COPY]),			"copy");
    cplStartIdent(&(cplAtValue[ATVALUE_COUNT]),			"count");
    cplStartIdent(&(cplAtValue[ATVALUE_CREATE]),			"create");
    cplStartIdent(&(cplAtValue[ATVALUE_DATABASE]),		"database");
    cplStartIdent(&(cplAtValue[ATVALUE_DB]),				"db");
    cplStartIdent(&(cplAtValue[ATVALUE_DECOD]),			"decod");
    cplStartIdent(&(cplAtValue[ATVALUE_DEFINE]),			"define");
    cplStartIdent(&(cplAtValue[ATVALUE_DELETE]),			"delete");
    cplStartIdent(&(cplAtValue[ATVALUE_DELIMITER]),		"delimiter");
    cplStartIdent(&(cplAtValue[ATVALUE_DISPLAY]),		"display");
    cplStartIdent(&(cplAtValue[ATVALUE_EXIT]),			"exit");
    cplStartIdent(&(cplAtValue[ATVALUE_EXPIRE]),			"expire");
    cplStartIdent(&(cplAtValue[ATVALUE_EXPORT]),			"export");
    cplStartIdent(&(cplAtValue[ATVALUE_EXPRESSION]),	"expression");
    cplStartIdent(&(cplAtValue[ATVALUE_EXTRACT]),		"extract");
    cplStartIdent(&(cplAtValue[ATVALUE_FILE]),			"file");
    cplStartIdent(&(cplAtValue[ATVALUE_FLAG]),			"flag");
    cplStartIdent(&(cplAtValue[ATVALUE_FLDDIR]),			"flddir");
    cplStartIdent(&(cplAtValue[ATVALUE_FREQ]),			"freq");
    cplStartIdent(&(cplAtValue[ATVALUE_FREQSUM]),		"freqsum");
    cplStartIdent(&(cplAtValue[ATVALUE_FROM]),   		"from");
    cplStartIdent(&(cplAtValue[ATVALUE_FST]),				"fst");
    cplStartIdent(&(cplAtValue[ATVALUE_FULLINV]),		"fullinvertion");
    cplStartIdent(&(cplAtValue[ATVALUE_GIZMO]),			"gizmo");
    cplStartIdent(&(cplAtValue[ATVALUE_HL]),				"hl");
    cplStartIdent(&(cplAtValue[ATVALUE_IMPORT]),			"import");
    cplStartIdent(&(cplAtValue[ATVALUE_INDEXLIST]), 	"indexlist");
    cplStartIdent(&(cplAtValue[ATVALUE_INVERTED]),		"inverted");
    cplStartIdent(&(cplAtValue[ATVALUE_INVLOAD]),		"invertedload");
    cplStartIdent(&(cplAtValue[ATVALUE_JUMP]),   		"jump");
    cplStartIdent(&(cplAtValue[ATVALUE_KEY]), 			"key");
    cplStartIdent(&(cplAtValue[ATVALUE_KEYFIELD]),		"keyfield");
    cplStartIdent(&(cplAtValue[ATVALUE_KEYLENGTH]),		"keylength");
    cplStartIdent(&(cplAtValue[ATVALUE_KEYRANGE]),		"keyrange");
    cplStartIdent(&(cplAtValue[ATVALUE_KEYS]), 			"keys");
    cplStartIdent(&(cplAtValue[ATVALUE_KEYSDB]),			"keysdb");
    cplStartIdent(&(cplAtValue[ATVALUE_LIST]), 			"list");
    cplStartIdent(&(cplAtValue[ATVALUE_LOAD]),			"load");
    cplStartIdent(&(cplAtValue[ATVALUE_LOCKID]),			"lockid");
    cplStartIdent(&(cplAtValue[ATVALUE_MASTER]),			"master");
    cplStartIdent(&(cplAtValue[ATVALUE_MAXLK]),			"maxlk");
    cplStartIdent(&(cplAtValue[ATVALUE_MFN]),				"mfn");
    cplStartIdent(&(cplAtValue[ATVALUE_MFNRANGE]),		"mfnrange");
    cplStartIdent(&(cplAtValue[ATVALUE_MSTSORT]),		"mastersort");
    cplStartIdent(&(cplAtValue[ATVALUE_OCC]),				"occ");
    cplStartIdent(&(cplAtValue[ATVALUE_OUTPUT]),			"output");
    cplStartIdent(&(cplAtValue[ATVALUE_POSTING]),   	"posting");
    cplStartIdent(&(cplAtValue[ATVALUE_POSTTAG]),   	"posttag");
    cplStartIdent(&(cplAtValue[ATVALUE_PREFIX]), 		"prefix");
    cplStartIdent(&(cplAtValue[ATVALUE_RELOAD]),			"reload");
    cplStartIdent(&(cplAtValue[ATVALUE_REPLACE]),		"replace");
    cplStartIdent(&(cplAtValue[ATVALUE_RESET]),			"reset");
    cplStartIdent(&(cplAtValue[ATVALUE_REVERSE]),   	"reverse");
    cplStartIdent(&(cplAtValue[ATVALUE_SEARCH]),			"search");
    cplStartIdent(&(cplAtValue[ATVALUE_SKIP]),			"skip");
    cplStartIdent(&(cplAtValue[ATVALUE_SORT]),			"sort");
    cplStartIdent(&(cplAtValue[ATVALUE_STATUSDB]),		"statusdb");
    cplStartIdent(&(cplAtValue[ATVALUE_STATUSFILE]),	"statusfile");
    cplStartIdent(&(cplAtValue[ATVALUE_STW]),				"stw");
    cplStartIdent(&(cplAtValue[ATVALUE_SUFFIX]), 		"suffix");
    cplStartIdent(&(cplAtValue[ATVALUE_TEMPFILE]),		"tempfile");
    cplStartIdent(&(cplAtValue[ATVALUE_TASK]),        	"task");
    cplStartIdent(&(cplAtValue[ATVALUE_TO]),        	"to");
    cplStartIdent(&(cplAtValue[ATVALUE_TYPE]),			"type");
    cplStartIdent(&(cplAtValue[ATVALUE_UCTAB]),			"uctab");
    cplStartIdent(&(cplAtValue[ATVALUE_UNLOCK]),			"unlock");
    cplStartIdent(&(cplAtValue[ATVALUE_UPDATE]),			"update");
    cplStartIdent(&(cplAtValue[ATVALUE_WRITE]),  		"write");
    /* 27.Nov.2000 */
    cplStartIdent(&(cplAtValue[ATVALUE_PREFIX_HTMLPFT]),"prefix htmlpft");
    cplStartIdent(&(cplAtValue[ATVALUE_SUFFIX_HTMLPFT]),"suffix htmlpft");
    /* 28.Nov.2000 */
    cplStartIdent(&(cplAtValue[ATVALUE_FIRST_LINE]),	"first line");
    /* 22.Feb.2001 */
    cplStartIdent(&(cplAtValue[ATVALUE_ISISXML_STYLE]),"isisxml style");
    cplStartIdent(&(cplAtValue[ATVALUE_ISISXML_TABLE]),"isisxml table");
    /* 05.Apr.2001 */
    cplStartIdent(&(cplAtValue[ATVALUE_LOG]),				"log");

   /* Set compiler comment */
    cplStartIdent(&(cplCommentBegin[COMMENT_NORMAL]),	"!--");
    cplStartIdent(&(cplCommentEnd[COMMENT_NORMAL]),		"-->");
    cplStartIdent(&(cplCommentBegin[COMMENT_PROCESS]),	"?");
    cplStartIdent(&(cplCommentEnd[COMMENT_PROCESS]),	"?>");
    cplStartIdent(&(cplCommentBegin[COMMENT_DECLARE]),	"!");
    cplStartIdent(&(cplCommentEnd[COMMENT_DECLARE]),	">");

   /* Set as started */
    program->cplStarted = TRUE;

} /* cplStart */

/* ============================================================== cplCompile */
CPL_ERROR cplCompile(char *script,				/* instructions to be compiled */
                            CPL_STRUCT *program)		/* program structure */
{
    char *p;						/* script instruction pointer */
   CPL_CMD_LIST cmdList;	/* command list */
    CPL_ERROR errorCode;		/* error code */

   /* Start compiler variables */
    cplStart(program);
//printf("antes do cplLoad\n");
   /* Load script */
   p = cplLoad(script,program,ELEMENT_LIST_QTT,&(program->cmd),&cmdList,&errorCode);

   /* Print compiled */
    if (program->printSource)
    if (errorCode) {
        *p = '\0';
        printf("%s",script);
      } else
        cplPrint(program->cmd,0);

   /* Return error code */
   return errorCode;

}  /* cplCompile */

/* ============================================================ cplWriteText */
CPL_ERROR cplWriteText(CPL_STRUCT *program,	/* program structure */
                              FILE *saveFile,			/* save file pointer */
                       char *text)				/* write text */
{
   LONGX writeTextLen;		/* write text lenght */

   /* write the text lenght */
    writeTextLen = (LONGX)strlen(text);
    if (fwrite(&writeTextLen,sizeof(LONGX),1,saveFile) != 1) {
        return cplError(program,CPL_ERROR_WRITE,text);
   }

   /* write the text string */
   if (fwrite(text,writeTextLen,1,saveFile) != 1) {
        return cplError(program,CPL_ERROR_WRITE,text);
    }

   /* return no error */
   return CPL_ERROR_OK;

} /* cplWriteText */

/* ========================================================= cplWriteCommand */
CPL_ERROR cplWriteCommand(CPL_STRUCT *program,		/* program structure */
                                  FILE *saveFile,				/* save file pointer */
                          CPL_COMMAND *cmd,			/* command instruction */
                          CPL_COMPILED_PART part)	/* write part */
{
   CPL_COMPILED_HEADER compiledHeader;		/* compiled header information */
   CPL_COMMAND cmdCompiled;				/* command save compiled area */
    CPL_ATTRIBUTE attrIndex;					/* attribute index */
    CPL_ERROR errorCode;							/* error code */

   /* select which part is to written */
    switch (part) {

   case PART_HEADER:
    /* write compiled file header */
    compiledHeader.versionNumber = 0L;		/* 0L - first compiled version */
      compiledHeader.aux1 = 0L;					/* reserved for any future use */
      compiledHeader.aux2 = 0L;					/* reserved for any future use */
    if (fwrite(&compiledHeader,sizeof(CPL_COMPILED_HEADER),1,saveFile) != 1) {
            return cplError(program,CPL_ERROR_WRITE,cplElement[cmd->element].text);
       }
      break;

   case PART_COMMAND:
    /* write command instruction */
      memmove(&cmdCompiled,cmd,sizeof(CPL_CMD_COMPILED));
    if (fwrite(&cmdCompiled,sizeof(CPL_COMMAND),1,saveFile) != 1) {
            return cplError(program,CPL_ERROR_WRITE,cplElement[cmd->element].text);
       }
      break;

   case PART_TEXT:
    /* write attribute texts */
    for (attrIndex = 0; attrIndex < ATTRIBUTE_LIST_QTT; attrIndex++) {
        if (cmd->attributeList[attrIndex].text) {
            errorCode = cplWriteText(program,saveFile,cmd->attributeList[attrIndex].text);
            if (errorCode) return errorCode;
        }
      }
    /* write command text */
    if (cmd->text) {
        errorCode = cplWriteText(program,saveFile,cmd->text);
         if (errorCode) return errorCode;
      }
        break;

   } /* switch */

   /* Return no error */
   return CPL_ERROR_OK;

} /* cplWriteCommand */

/* ================================================================ cplWrite */
CPL_ERROR cplWrite(CPL_STRUCT *program,		/* program structure */
                   FILE *saveFile,				/* save file pointer */
                   CPL_COMMAND *cmd,			/* command instruction */
                   CPL_COMPILED_PART part)	/* write part */
{
    CPL_ERROR errorCode;		/* error code */

   /* All command list */
    for ( ; cmd; cmd = cmd->next) {

    /* Write command */
    errorCode = cplWriteCommand(program,saveFile,cmd,part);
      if (errorCode) return errorCode;

    /* Write sub-command */
    if (cmd->sub) {
        errorCode = cplWrite(program,saveFile,cmd->sub,part);
         if (errorCode) return errorCode;
      }

   } /* for */

   /* Return no error */
    return CPL_ERROR_OK;

} /* cplWrite */

/* ========================================================= cplSaveCompiled */
CPL_ERROR cplSaveCompiled(CPL_STRUCT *program,	/* program structure */
                          char *saveFileName)	/* save file name */
{
   FILE *saveFile;			/* file pointer */
    CPL_ERROR errorCode;		/* error code */

   /* Open for binary write */
    saveFile = fopen(saveFileName,"wb");
    if (!saveFile) return cplError(program,CPL_ERROR_FILE,saveFileName);

   /* Write compiled header */
    errorCode = cplWriteCommand(program,saveFile,NULL,PART_HEADER);

   /* Write command instructions */
   if (!errorCode) {
    errorCode = cplWrite(program,saveFile,program->cmd,PART_COMMAND);
   }

   /* Write text string list */
   if (!errorCode) {
    errorCode = cplWrite(program,saveFile,program->cmd,PART_TEXT);
   }

   /* Close written file */
    fclose(saveFile);

   /* Return error code */
   return errorCode;

}  /* cplSaveCompiled */

/* ============================================================= cplReadText */
CPL_ERROR cplReadText(CPL_STRUCT *program,	/* program structure */
                             FILE *loadFile,			/* load file pointer */
                     char **readText,			/* read text return pointer */
                      BOOLEAN *readEOF)		/* enf of file flag */
{
   LONGX loadTextLen;		/* text lenght */
   char *text;				/* text string pointer */

    /* read text lenght */
   *readEOF = (fread(&loadTextLen,sizeof(LONGX),1,loadFile) != 1);
    if (*readEOF) {
        return CPL_ERROR_OK;
   }

    /* read text string */
//printf("cplReadText/antes do efc_new\n");
    text = (char *)efc_new(loadTextLen+1);
//printf("cplReadText/antes do efc_new\n");
   if (!text) return cplError(program,CPL_ERROR_ALLOC,"cplReadText");
   if (loadTextLen) *readEOF = (fread(text,loadTextLen,1,loadFile) != 1);
   *(text+loadTextLen) = '\0';
    if (*readEOF) {
        return CPL_ERROR_OK;
    }

   /* Return text pointer */
    *readText = text;

   /* Return no error */
   return CPL_ERROR_OK;

} /* cplReadText */

/* ========================================================== cplReadCommand */
CPL_ERROR cplReadCommand(CPL_STRUCT *program,		/* program structure */
                                 FILE *loadFile,				/* load file pointer */
                         CPL_COMPILED_PART part,	/* write part */
                         CPL_COMMAND *cmd,			/* command instruction */
                         BOOLEAN *readEOF)			/* enf of file flag */
{
   CPL_COMPILED_HEADER compiledHeader;		/* compiled header information */
   CPL_COMMAND cmdCompiled;				/* command save compiled area */
    CPL_ATTRIBUTE attrIndex;					/* attribute index */
    CPL_ERROR errorCode;							/* error code */

   /* select which part is to read */
    switch (part) {

   case PART_HEADER:
    /* read compiled header */
       *readEOF = (fread(&compiledHeader,sizeof(CPL_COMPILED_HEADER),1,loadFile) != 1);
    if (*readEOF) {
            return CPL_ERROR_OK;
       }
      break;

   case PART_COMMAND:
    /* read command instruction */
       *readEOF = (fread(&cmdCompiled,sizeof(CPL_COMMAND),1,loadFile) != 1);
      memset(cmd,0x00,sizeof(CPL_COMMAND));
      memmove(cmd,&cmdCompiled,sizeof(CPL_CMD_COMPILED));
      cmd->isCompiled = TRUE;
    if (*readEOF) {
            return CPL_ERROR_OK;
       }
      break;

   case PART_TEXT:
    /* read attribute text list */
    for (attrIndex = 0; attrIndex < ATTRIBUTE_LIST_QTT; attrIndex++) {
        if (cmd->attributeList[attrIndex].text) {
            errorCode = cplReadText(program,loadFile,&(cmd->attributeList[attrIndex].text),readEOF);
            if (errorCode) return errorCode;
        }
      }
    /* read command text */
      if (cmd->text) {
            errorCode = cplReadText(program,loadFile,&(cmd->text),readEOF);
        if (errorCode) return errorCode;
      }
        break;

   } /* switch */

   /* Return no error */
   return CPL_ERROR_OK;

} /* cplReadCommand */

/* ================================================================= cplRead */
CPL_ERROR cplRead(CPL_STRUCT *program,		/* program structure */
                        FILE *loadFile,			/* load file pointer */
                    CPL_COMMAND **first,		/* first command of the list */
                        CPL_CMD_LIST *cmdList,	/* command list */
                  BOOLEAN *readEOF)			/* enf of file flag */
{
   CPL_COMMAND cmd;					/* command structure */
   CPL_CMD_LIST subCmdList;		/* sub-command list */
   CPL_COMMAND *loadedCmd;			/* loaded command structure pointer */
    CPL_ERROR errorCode;				/* error code */

   /* Start variables */
   *first = NULL;
    memset(&cmd,0x00,sizeof(CPL_COMMAND));
    memset(cmdList,0x00,sizeof(CPL_CMD_LIST));

   /* All list commands */
    do {

    /* Read command */
    errorCode = cplReadCommand(program,loadFile,1,&cmd,readEOF);
      if (errorCode) return errorCode;
      if (*readEOF) break;

    /* Load command */
      loadedCmd = cplLoadCommand(&cmd,cmdList);
        if (errorCode) return errorCode;

        /* Load sub-command */
      if (loadedCmd->sub) {
            errorCode = cplRead(program,loadFile,&(loadedCmd->sub),&subCmdList,readEOF);
        if (errorCode) return errorCode;
      }

   } while (!(*readEOF) && loadedCmd->next);

   /* Set return command */
   *first = cmdList->first;

   /* Return no error */
    return CPL_ERROR_OK;

} /* cplRead */

/* ========================================================= cplReadTextList */
CPL_ERROR cplReadTextList(CPL_STRUCT *program,	/* program structure */
                                  FILE *loadFile,			/* load file pointer */
                          CPL_COMMAND *cmd,		/* command structure */
                          BOOLEAN *readEOF)		/* enf of file flag */
{
    CPL_ERROR errorCode;				/* error code */

   /* All list commands */
    for ( ; cmd; cmd = cmd->next) {

    /* Read command */
    errorCode = cplReadCommand(program,loadFile,PART_TEXT,cmd,readEOF);
      if (errorCode) return errorCode;
      if (*readEOF) break;

    /* Load sub-command */
      if (cmd->sub) {
        errorCode = cplReadTextList(program,loadFile,cmd->sub,readEOF);
        if (errorCode) return errorCode;
        if (*readEOF) break;
      }

   } /* for */

   /* Return no error */
    return CPL_ERROR_OK;

} /* cplReadTextList */

/* ========================================================= cplLoadCompiled */
CPL_ERROR cplLoadCompiled
(
    char *loadFileName,		/* load file name */
    CPL_STRUCT *program,		/* program structure */
    CPL_COMMAND **cmd,		/* command structure */
    CPL_CMD_LIST *cmdList	/* command list */
)
{
   FILE *loadFile;			/* file pointer */
    BOOLEAN readEOF;			/* enf of file flag */
    CPL_ERROR errorCode;		/* error code */

   /* Open for binary read */
    loadFile = fopen(loadFileName,"rb");
    if (!loadFile) return cplError(program,CPL_ERROR_FILE,loadFileName);

   /* Start compiler variables */
    cplStart(program);

   /* Read compiled header */
    errorCode = cplReadCommand(program,loadFile,PART_HEADER,NULL,&readEOF);
   if (!errorCode && readEOF) errorCode = cplError(program,CPL_ERROR_INVALID,loadFileName);

   /* Read command instructions */
   if (!errorCode) {
       errorCode = cplRead(program,loadFile,cmd,cmdList,&readEOF);
    if (!errorCode && readEOF) errorCode = cplError(program,CPL_ERROR_INVALID,loadFileName);
    }

   /* Read text strings */
   if (!errorCode) {
       errorCode = cplReadTextList(program,loadFile,*cmd,&readEOF);
   }

   /* Close writed file */
    fclose(loadFile);

   /* Return error code */
   return errorCode;

}  /* cplLoadCompiled */

/* ================================================================= cplFree */
void cplFree(CPL_COMMAND *first)		/* first command */
{
   CPL_COMMAND *cmd;		/* current command */
   CPL_COMMAND *next;	/* next command */
   CPL_ATTRIBUTE i;		/* auxiliary loop index */

   /* Garbage collector */
    for (cmd = first; cmd; cmd = next) {
        next = cmd->next;
      for (i = 0; i < ATTRIBUTE_LIST_QTT; i++)
        efc_free(cmd->attributeList[i].text);
      efc_free(cmd->text);
      if (cmd->sub) cplFree(cmd->sub);
      efc_free(cmd);
   } /* for */

} /* cplFree */

