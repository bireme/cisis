/* ------------------------------------------------------------------- XIS.C */

/* /////////////////////////////////////////////////////////////////////////

   [ Version 0.6 ]
   03.Mar.1999 - XML Isis Script.

   [ Version 0.7 ]
   22.Mar.1999 - "programPath": To find files in the same path of the program.

   [ Version 0.8 ]
   29.Mar.1999 - "cpl_Load": Check close element couple;
   30.Mar.1999 - "efc_split_lines_char": CR/LF problem;
   31.Mar.1999 - "exePrint": check command parameter presence.

   [ Version 0.8a ]
   05.Apr.1999 - "exeFieldOcc": Get specific occurrence.

   [ Version 0.9 ]
   12.Apr.1999 - "main": Check license after xisArgRead and xisCgi;
   13.Apr.1999 - "path_source": Set environment variable;
   14.Apr.1999 - "ATVALUE_PREFIX": substitute for ATVALUE_PFXTAG;
   14.Apr.1999 - "PATH_WXIS": substitute for PATH_SOURCE;
   14.Apr.1999 - "exeCgiTable": allowing CGI prefix tag.

   [ Version 0.9a ]
   15.Apr.1999 - "exeCall": previous index is set with the caller index.

   [ Version 0.9b ]
   15.Apr.1999 - "exePft": htmlpft convertion;

   [ Version 0.9c ]
   16.Apr.1999 - "exeScopeClose": parameter scopeVar -> localScopeVar;
   16.Apr.1999 - "efc_split_lines_char": avoid empty buffer memory fault;
   16.Apr.1999 - "exeTaskFullInvertion": internal key sort format and length;
   16.Apr.1999 - "PRECISE_GETNEW": GetNew argument value, useful for appending new records.

   [ Version 0.9d ]
   26.Apr.1999 - "exeLoop": accepts <parm name=task>...</parm>;
   27.Apr.1999 - "ELEMENT_RETURN": exit from current function;
   28.Apr.1999 - "ELEMENT_DISPL": synonymous for ELEMENT_DISPLAY;
   04.May.1999 - "exeTaskInvertedLoad": need iflind.

   [ Version 0.9e ]
   11.May.1999 - "cib62.c": b53_writx/b53_seek, fixed;
   12.May.1999 - "cib51.c, cib62.c": "$" search fixed.

   [ Version 0.9f ]
   20.May.1999 - "svdifload": mfnread--, first LE2 missing key fixed.

   [ Version 0.9g ]
   26.May.1999 - "exeTaskDo": exePos() sets Isis_Current value;
   28.May.1999 - "exeLock": expired lock need to change the lock id field;
   31.May.1999 - "exeExpire": gap time <= expire value returns not expired;
   31.May.1999 - "ATVALUE_RESET": ifupdat, ifload -reset flag;
   31.May.1999 - "PRECISE_WNOUNLOCK": write but not unlock (keep locked);
   31.May.1999 - "exeTaskUpdate": PRECISE_LOCK, PRECISE_STATUS don't propagate.

   [ Version 1.0 ]
   08.Jun.1999 - "cplToken": XML instructions as comments <? ... ?> and <! ... >;
   15.Jun.1999 - "xisLicense": license checks the new version expire limit date;
   16.Jun.1999 - "RLINE": sequential import type;
   16.Jun.1999 - "exeTaskFullInvertion": fast, fix extract, mastersort via field and fix inverted load;
   17.Jun.1999 - "exePft": allows format syntax check;
   17.Jun.1999 - "exeFieldTransf": import and export actions with tag list, respect previous attribute;
   29.Jun.1999 - "exeParm": allows fst syntax check;
   29.Jun.1999 - "exeParm": buffer size attribute value;
   06.Jul.1999 - "exeFieldTransf": if (keep) { ... while (isdigit(*(p+1))) p++; };
   13.Jul.1999 - "PRECISE_ITEMS": PRECISE_ITENS spelling mistake;
   15.Jul.1999 - "xisArgRead": call xisCgi for all in file parameters;
   30.Jul.1999 - "exeTaskList": interprets "from=" and "to=" parameters;
   04.Aug.1999 - "exeExtract": efc_free(lnk1p) and efc_free(lnk2p);
   23.Sep.1999 - "exeCgiSubfld": avoid empty value;
   27.Sep.1999 - "exeFieldSplit": attribute type=list.

   [ Version 1.1 ]
   28.Sep.1999 - "xisLicense": use REMOTE_ADDR and REMOTE_HOST instead of SERVER_NAME;
   28.Sep.1999 - "exeFieldDir": attribute type=delete.

   [ Version 1.2 ]
   26.Oct.1999 - "exeCopyFile": copy file functionality;
   04.Nov.1999 - "exeTaskMfnrange": the Isis_Status can handle the RCLOCK;
   12.Nov.1999 - "exeFileTemp": temporary file can be at CI_TEMPDIR directory;
   17.Nov.1999 - "exeTaskSearch": dbxtmpnm() instead of tmpnam();
   13.Dec.1999 - "exeHtmlPft": entity must remain untouched;
   07.Jan.2000 - "cifm3.c": date(DATEONLY) and date(DATETIME) bug.

   [ Version 4.0 ]
   07.Jan.2000 - WXIS will be first released as WWWISIS version 4.0;
   22.Feb.2000 - "ELEMENT_INCLUDE": the include functionality;
   23.Feb.2000 - "xisFileContent": first try is the original file name;
   23.Feb.2000 - "xisFileContent": not found message is the original file name;
   28.Feb.2000 - "exeJump": jump to a label inside a <include> element;
   09.Mar.2000 - Compiled save;
   10.Mar.2000 - Compiled run;
   13.Mar.2000 - Avoid trace over compiled instruction;
   20.Mar.2000 - "xisLicense": remoteAddr,remoteHost use cgi_getenv instead of getenv;
   08.Jun.2000 - "xisLicense": allows script name based lincense;
   26.Jun.2000 - "exeExport": export direct to the standard output export;
   13.Jul.2000 - "exeExport": export ISO forcing CRLF;
   14.Ago.2000 - "stwTable": PARM_STW correct load;
   25.Ago.2000 - "exeCmd/ELEMENT_TRACE": activate/deactivate dbxtrace & Cia;
   08.Sep.2000 - "arglist.c/arg_lines": avoid memory invasion;
   08.Sep.2000 - "exeExtractSave": allow in-place field add via FST;
   26.Sep.2000 - "cirec.c/mstsetup": avoid fatal("mstsetup/ALLOMAXV/.xrf");
   27.Nov.2000 - "PARM_PREFIX_HTMLPFT/PARM_SUFFIX_HTMLPFT": to be different from the hl prefix and suffix;
    27.Nov.2000 - "exeFile/ATVALUE_DELETE" uses the cipar file name translation;

   [ Version 4.1 ]
    27.Nov.2000 - "<include>" accepts compiled script;
    28.Nov.2000 - 'tag="first line"': allows to specify the tag via a format for the <field> element;

   [ Version 4.2 ]
    20.Dez.2000 - "cib7": New search engine;
    30.Jan.2001 - "PARM_POSTTAG": accepts tag list;
    15.Feb.2001 - "exeField": check action= attribute presence.

   [ Version 4.3 ]
    23.Feb.2001 - "<isisxml>": generate XML data;
    28.Feb.2001 - "cib7p": moved from global to SEARCH_VAR to allow multiple search;
    13.Mar.2001 - "exeSearchCisisPropagateQualifier": iah style feature.

   [ Version 5.0 ]
   14.Mar.2001 - WXIS with the b7 search will be released as WWWISIS version 5.0;
   25.Mar.2001 - ISISXML_STYLE_EDIT;
   05.Apr.2001 - <parm name="log">: on task search;
   05.Apr.2001 - <field action="define" tag="1098">Isis_SetNumber</field>: on task search;
   27.Apr.2001 - "cixml.c" module;

   [ Version 5.1 ]
   03.May.2001 - "xisVar.argList[ARG_SCRIPT].text = getenv("PATH_TRANSLATED")";
   22.Jun.2001 - "cib72.c": if (tmp) - 29 quoted search key error;
   21.Sep.2001 - "cib71.c": b7_cib7_delete implemented.

   [ Version 5.2 ]
   03.Oct.2001 - "cib72.c/b7_run": FREE(vtrmp[b7itrm]);
   29.Nov.2001 - "exePft()": set isisxml_style before isisxml_load;
   05.Dec.2001 - "cixmlLoad": new parameter: default_style;
   18.Mar.2002 - "xisFileContent": not found message is "IsisScript".

   [ Version 5.3 ]
   07.Aug.2002 - "cib72.c": if (tmp) - 29 quoted search key error; (AGAIN in CISIS)
   07.Aug.2002 - "cisis 4.1r1": #define CIFFI 0.

   [ Version 5.4 ]
   11.Feb.2003 - "cib72.c": (G), (F) and related operators OK.
   14.Feb.2003 - "cisis 4.2r0": #define CIFFI 0.
   27.Feb.2003 - "cib72.c": b7_pfx - if ( aspas )...

   [ Version 6.0 ]
   07.Aug.2002 - "cisis 4.1r1": #define CIFFI 1.
   14.Feb.2003 - "cisis 4.2r0": #define CIFFI 1.
   27.May.2003 - "cisis 4.3a": record update with FFI (linux)

   [ Version 7.0 ]
   02.Sep.2003 - "LICENSED_VERSION": may be compiled without license

   [ Version 7.0a ]
   02.Feb.2006 - iso/marc import and export

   [ Version 7.1 ]
   02.Feb.2007 - from now wxis will not be compiled with license.
                 CIFII and isis compilation will have the same version number.
                 the release date is generated automatically

   [ Version 7.1a ]
   20.Mar.2007 - temporary search files are not longer created.

   [ Version 7.1b ]
   16.May.2007 - .iso export marc bug fixed

   [ Version 7.1c ]
   19.Jul.2007 - the release date can not be generated automatically

   [ Version 7.1d ]
   26.Oct.2007 - security issue - now wxis check if the isis script file
                 name ends with *.xis or *.xic

   [ Version 7.1e ]
   06.Oct.2009 - long type changed to LONGX and seek to LSEEK64

   [ Version 7.1f ]
   11.Apr.2012 - increased buffer size areas during index generation(EXE_REC_SIZE, EXE_BUFF_SIZE)
   ///////////////////////////////////////////////////////////////////////// */

/* ----------------------------------------------------------- C HEADER FILE */
#include <time.h>
#include <stdio.h>
#include <string.h>
/* #include <mem.h> */
/* ------------------------------------------------------------- HEADER FILE */
#include "../cisis.h"
#include "../cirun.h"
#include "easyfc.h"
#include "arglist.h"
#include "cgilist.h"
#include "freqlist.h"
#include "xis_comp.h"
#include "xis_exec.h"
#include "hide.h"
/* #include "chronometer.h" */
/* ------------------------------------------------------------------ define */

#define LICENSED_VERSION    0
#define XIS_VERSION         "?XML IsisScript WWWISIS 7.1f"  /* xis version */
#define XIS_RELEASE_DATE     __DATE__ /*"20080925"*/

#define XIS_LICENSE         "wxis.lic"  /* xis license file */
#define XIS_LICDATE         20030525L   /* xis license date */
#define XIS_LICTIME         10000L      /* xis license time (one year) */
#define XIS_LICMIN          19990600L   /* xis license smaller date */
#define XIS_COMPILED_EXT    ".xic"      /* xis compiled file extension */
#define MAX_IN_ARGV         512         /* limit of arguments */

#ifndef XIS_SERVER
#define GEN_MAIN 1
#define XIS_ERR(x,y,z)  exit(xisError(x,y))
#define XIS_ERR2(x,y,z) xisError(x,y)
#else
#define GEN_MAIN 0
#define XIS_ERR(x,y,z)  return xisServerError(x,y,z)
#define XIS_ERR2(x,y,z) xisServerError(x,y,z)
#endif /* XIS_SERVER */

OUT_BUFFER *outBuff;
int firstTime = 0;

/* ------------------------------------------------------------- enumeration */
typedef enum {
   XIS_ERROR_OK,
   XIS_ERROR_ALLOC,
   XIS_ERROR_ARGS,
   XIS_ERROR_CGI,
   XIS_ERROR_COMPILED,
   XIS_ERROR_COMPILER,
   XIS_ERROR_EXECUTION,
   XIS_ERROR_FATAL,
   XIS_ERROR_FILE,
   XIS_ERROR_MISSING,
   XIS_ERROR_LICENSE,
   XIS_ERROR_QTT
} XIS_ERROR;                    /* error */

typedef enum {
   ARG_ARGS,
   ARG_COMPILE,
   ARG_HELLO,
   ARG_IN,
   ARG_SCRIPT,
   ARG_SOURCE,
   ARG_WHAT,
   ARG_LIST_QTT
} ARG_LIST;                 /* argument */

typedef enum {
   LICERROR_OK,
   LICERROR_FILE,
   LICERROR_INVALID,
   LICERROR_EXPIRED,
   LICERROR_UNAUTHO,
   LICERROR_MISSING,
   LICERROR_LIST_QTT
} LICERROR_LIST;           /* license error */
/* --------------------------------------------------------------- structure */
typedef struct {
   char            *programPath;
   STRUCT_ARGUMENT argList[ARG_LIST_QTT];
   EFC_ERROR       error;
   CGI_PARAM       *cgiList;
   char            *licenseText;
   char            *pathTranslated;
   char            *script;
   CPL_STRUCT      xisProgram;
} XIS_VAR;                     /* variables */
/* ------------------------------------------------------------------ global */
char *xisVersion = XIS_VERSION;                 /* xis version */
char *xisLicenseFile = XIS_LICENSE;             /* xis license file */
char path_source[PATH_NAME_LEN+1];              /* PATH_SOURCE putenv */

/* ============================================================ xisErrorText */
char *xisErrorText(XIS_ERROR errorCode)     /* XIS error code */
{

    /* Return corresponding error text */
   switch (errorCode) {
   case XIS_ERROR_OK:           return "no error";
   case XIS_ERROR_ALLOC:        return "memory allocation";
   case XIS_ERROR_ARGS:         return "argument";
   case XIS_ERROR_CGI:          return "cgi parameter";
   case XIS_ERROR_COMPILED: return "compiled";
   case XIS_ERROR_COMPILER: return "compiler";
   case XIS_ERROR_EXECUTION:    return "execution";
   case XIS_ERROR_FATAL:        return "fatal";
   case XIS_ERROR_FILE:         return "file";
   case XIS_ERROR_MISSING:      return "missing";
   case XIS_ERROR_LICENSE:      return "license";
   } /* switch */

    /* Return undefined error code message */
   return "undefined message";

} /* xisErrorText */

/* ========================================================== xisErrorDetail */
char *xisErrorDetail(EFC_ERROR *error,          /* error structure */
                     XIS_ERROR errorCode)       /* XIS error code */
{

    /* Return corresponding error text */
   switch (errorCode) {
   case XIS_ERROR_OK:           return "success";
   case XIS_ERROR_ALLOC:        return "not enough";
   case XIS_ERROR_ARGS:
   case XIS_ERROR_CGI:
   case XIS_ERROR_FILE:
    switch (error->code) {
    case FILE_ERROR_SIZE:
    case -FILE_ERROR_SIZE:  return "file size";
    case FILE_ERROR_OPEN:
    case -FILE_ERROR_OPEN:  return "file open";
    case FILE_ERROR_ALLOC:
    case -FILE_ERROR_ALLOC: return "memory allocation";
    case FILE_ERROR_READ:
    case -FILE_ERROR_READ:  return "file read";
    case ARG_ERROR_MAX:
    case -ARG_ERROR_MAX:        sprintf(error->info,"%d",MAX_IN_ARGV);
                                    return "argument limit exceeded";
      } /* switch */
      break;
   case XIS_ERROR_COMPILED:
    switch (error->code) {
    case CPL_ERROR_READ:                return "read";
      } /* switch */
      break;
   case XIS_ERROR_COMPILER:
    switch (error->code) {
    case CPL_ERROR_ALLOC:           return "memory allocation";
    case CPL_ERROR_INVALID:         return "invalid identifier";
    case CPL_ERROR_MISSING:         return "missing";
    case CPL_ERROR_PATHNAMELEN: return "path name lenght";
    case CPL_ERROR_NOTFOUND:        return "file not found";
    case CPL_ERROR_FILE:                return "file";
    case CPL_ERROR_WRITE:           return "write";
      } /* switch */
      break;
   case XIS_ERROR_EXECUTION:
    switch (error->code) {
    case EXE_ERROR_ACTAB:   return "actab";
    case EXE_ERROR_ALLOC:   return "memory allocation";
    case EXE_ERROR_EXTRACT: return "extract";
    case EXE_ERROR_FIELD:   return "field";
    case EXE_ERROR_FILE:        return "file";
    case EXE_ERROR_FORMAT:  return "format syntax";
    case EXE_ERROR_FST:     return "fst";
    case EXE_ERROR_IMPORT:  return "import";
    case EXE_ERROR_INVALID: return "invalid value";
    case EXE_ERROR_MAXNREC: return "record index overflow";
    case EXE_ERROR_MAXNTRM: return "term index overflow";
    case EXE_ERROR_MISSING: return "missing";
    case EXE_ERROR_MSRT:        return "master sort";
    case EXE_ERROR_PROC:        return "proc";
    case EXE_ERROR_RETURN:  return "return";
    case EXE_ERROR_SORT:        return "sort";
    case EXE_ERROR_STW:     return "stop word";
    case EXE_ERROR_SEARCH:  return "search";
        case EXE_ERROR_TARGET:  return "label not found (same scope)";
    case EXE_ERROR_UCTAB:   return "uctab";
    case EXE_ERROR_UNDEFINED:
                                    return "undefined instruction";
      } /* switch */
      break;
   case XIS_ERROR_FATAL:        return "unavoidable";
   case XIS_ERROR_MISSING:      return "parameter";
   case XIS_ERROR_LICENSE:      return "unauthorized";
   } /* switch */

    /* Return undefined error code message */
    sprintf(error->info,"%d/%d",errorCode,error->code);
    return "undefined message";

} /* xisErrorDetail */

/* ================================================================ xisError */
XIS_ERROR xisError(EFC_ERROR *error,        /* error structure */
                   XIS_ERROR errorCode)     /* WIS error code */
{
   char *text;         /* error message text */
   char *detail;        /* error message detail */

    /* Return exit command code, without any message */
    if (errorCode == XIS_ERROR_EXECUTION && error->code == EXE_ERROR_EXIT)
        return atoi(error->info);

    /* Prepare message */
    text = xisErrorText(errorCode);
    detail = xisErrorDetail(error,errorCode);

    /* Show XIS error line */
   printf("\nWXIS|%s error|%s|%s|\n",text,detail,error->info);

    /* Return error code */
   return errorCode;

} /* xisError */

#ifdef XIS_SERVER
/* ========================================================== xisServerError */
XIS_ERROR xisServerError(EFC_ERROR *error,          /* error structure */
                         XIS_ERROR errorCode,       /* WIS error code */
                         XIS_VAR *xisVar)
{
    char *text;         /* error message text */
    char *detail;       /* error message detail */
    char mess[256];

    /* Return exit command code, without any message */
    if (errorCode == XIS_ERROR_EXECUTION && error->code == EXE_ERROR_EXIT)
      return atoi(error->info);

    /* Prepare message */
    text = xisErrorText(errorCode);
    detail = xisErrorDetail(error,errorCode);

    /* Show XIS error line */
    sprintf(mess, "\nWXIS_SERVER|%s error|%s|%s|\n",text,detail,error->info);
    writeBuff(mess, outBuff, 0);

    /* Return error code */
   return errorCode;

} /* xisError */
#endif /* XIS_SERVER */

/* ============================================================== wwwi_fatal */
#ifdef XIS_SERVER
void wwwi_fatal(char *fatal_msg) {
    printf("FATRAP flag should be on.\n");
    exit(1);
}
#else
void wwwi_fatal(char *fatal_msg)
{
    EFC_ERROR err;

    /* Set fatal message */
   efc_error(&err,fatal_errcod,fatal_msg);

    /* Show message, exit with error code */
    exit(xisError(&err,XIS_ERROR_FATAL));

} /* wwwi_fatal */
#endif

/* ========================================================== xisFileContent */
char *xisFileContent(XIS_VAR *xisVar,   /* XIS variables */
                     char *fileName)    /* file name */
{
    char *p;                        /* file content buffer pointer */
    char pathName[PATH_NAME_LEN+1]; /* auxiliary file name complement */

   /* [4.1] Get file content, try with the original file name */
   p = efc_std_filecontent(&(xisVar->error),fileName);
    xisVar->xisProgram.pathSource = "";
    if (p) return p;

   /* Get file content, try with the program path */
    sprintf(pathName,"%s%s",xisVar->programPath,fileName);
   p = efc_std_filecontent(&(xisVar->error),pathName);
    xisVar->xisProgram.pathSource = xisVar->programPath;
    if (p) return p;

   /* If file not found, try with the CGI path translated */
   if (!p && xisVar->error.code == -FILE_ERROR_OPEN) {
        if (strlen(xisVar->pathTranslated) + strlen(fileName) > PATH_NAME_LEN)
            fatal("PATH_NAME_LEN");
        sprintf(pathName,"%s%s",xisVar->pathTranslated,fileName);
        p = efc_std_filecontent(&(xisVar->error),pathName);
    xisVar->xisProgram.pathSource = xisVar->pathTranslated;
    }

   /* If error, exit */
   if (!p) {
    efc_error(&(xisVar->error),FILE_ERROR_OPEN,"Isis_Script");//fileName);
    XIS_ERR2(&(xisVar->error),XIS_ERROR_FILE, xisVar);
   }

   /* Return allocated buffer */
   return p;

} /* xisFileContent */

/* ========================================================== xisLicenseInfo */
void xisLicenseInfo(void)
{
    time_t tim = time(NULL);
    struct tm *now = localtime(&tim);

    printf("\nWXIS release date: %s\n",  XIS_RELEASE_DATE);
}

/* =========================================================== xisArgInstant */
void xisArgInstant(XIS_VAR *xisVar)     /* XIS variables */
{

    /* Interpret instant parameters */
   if (xisVar->argList[ARG_HELLO].text) {
    printf("Content-type: text/plain\n\n%s",cicopyr(xisVersion));
      xisLicenseInfo();
   }
   if (xisVar->argList[ARG_WHAT].text) {
    printf("%s",cicopyr(xisVersion));
      xisLicenseInfo();
   }

} /* xisArgInstant */

/* ================================================================== xisRun */
XIS_ERROR xisRun(XIS_VAR *xisVar)   /* XIS variables */
{
   XIS_ERROR errorCode = XIS_ERROR_OK;     /* XIS error code */
   CPL_CMD_LIST cmdList;                   /* sub-command list */
   int len;
   char * ext;

    /* Get script parameter */
    if (xisVar->argList[ARG_SCRIPT].text) {

        /* Check if the script file ends with .xis/.xic for security reasons */
      len = strlen(xisVar->argList[ARG_SCRIPT].text);
      ext = xisVar->argList[ARG_SCRIPT].text + (len - 1);

      while ((len >= 5) && (*ext == ' ')) {
        *ext-- = 0;
         len--;
      }
      if (len < 5) { /* x.xis */
            efc_free(xisVar->programPath); xisVar->programPath = NULL;// Code Guard
         XIS_ERR(&(xisVar->error),efc_error(&(xisVar->error),XIS_ERROR_MISSING,"IsisScript expected"), xisVar);
      }
      ext = xisVar->argList[ARG_SCRIPT].text + (len - 4);
#if UNIX
      if (!((strcasecmp(ext, ".xis") == 0) || (strcasecmp(ext, ".xic") == 0))) {
#else /* UNIX */
      if (!((stricmp(ext, ".xis") == 0) || (stricmp(ext, ".xic") == 0))) {
#endif /* UNUX */
            efc_free(xisVar->programPath); xisVar->programPath = NULL;// Code Guard
         XIS_ERR(&(xisVar->error),efc_error(&(xisVar->error),XIS_ERROR_MISSING,"IsisScript expected"), xisVar);
      }

        /* Not compiled */
      if (!cplIsCompiled(&(xisVar->xisProgram),xisVar->argList[ARG_SCRIPT].text)) {
        /* Get script file content, set xisVar->xisProgram.pathSource */
        xisVar->script = xisFileContent(xisVar,xisVar->argList[ARG_SCRIPT].text);
        if (!xisVar->script) {   // MUDAR !!!
            return XIS_ERROR_MISSING;
            //XIS_ERR(&(xisVar->error),efc_error(&(xisVar->error),XIS_ERROR_MISSING,xisVar->argList[ARG_SCRIPT].id), xisVar);
        }

        /* Load program instructions */
        xisVar->xisProgram.printSource = (BOOLEAN)xisVar->argList[ARG_SOURCE].num;
        if (cplCompile(xisVar->script,&(xisVar->xisProgram))) {
            errorCode = XIS_ERR2(&(xisVar->xisProgram.error),XIS_ERROR_COMPILER, xisVar);
        }

        /* Compiled */
        } else {

            /* Load compiled instructions */
         if (cplLoadCompiled(xisVar->argList[ARG_SCRIPT].text,&(xisVar->xisProgram),&(xisVar->xisProgram.cmd),&cmdList)) {
            errorCode = XIS_ERR2(&(xisVar->xisProgram.error),XIS_ERROR_COMPILER, xisVar);
         }
      }

    /* No script parameter present */
    } else {
        efc_free(xisVar->programPath); xisVar->programPath = NULL;// Code Guard
        XIS_ERR(&(xisVar->error),efc_error(&(xisVar->error),XIS_ERROR_MISSING,xisVar->argList[ARG_SCRIPT].id), xisVar);
   }

    /* No error */
   if (!errorCode) {

    /* Write compiled instructions required  */
    if (xisVar->argList[ARG_COMPILE].text) {

        /* Save compiled */
        if (cplSaveCompiled(&(xisVar->xisProgram),xisVar->argList[ARG_COMPILE].text)) {
            errorCode = XIS_ERR2(&(xisVar->xisProgram.error),XIS_ERROR_COMPILER, xisVar);
        }

    /* Normal run */
      } else {
        /* Execute program */
        if (exeRun(&(xisVar->xisProgram),xisVar->cgiList, outBuff)) {
            errorCode = XIS_ERR2(&(xisVar->xisProgram.error),XIS_ERROR_EXECUTION, xisVar);
        }
      }
   }

   /* Garbage collector */
   cplFree(xisVar->xisProgram.cmd);

   /* Return error code */
    return errorCode;

} /* xisRun */

/* ============================================================== xisArgShow */
void xisArgShow(XIS_VAR *xisVar,        /* XIS variables */
                     int argc,              /* quantity of arguments */
                     char *argv[])          /* array of arguments */
{
   int argI;        /* argument loop index */

    /* Check show flag */
    if (!xisVar->argList[ARG_ARGS].text) return;

    /* Show argument list title */
    printf("\n[%s]\n",argv[0]);

    /* Show all arguments */
    for (argI = 1; argI < argc; argI++)
        printf("%3d) %s\n",argI,argv[argI]);

} /* xisArgShow */

/* ================================================================== xisCgi */
XIS_ERROR xisCgi(XIS_VAR *xisVar,   /* XIS variables */
                 int argc,          /* quantity of arguments */
                 char *argv[])      /* array of arguments */
{
   CGI_PARAM *cgiHead = NULL;       /* cgi parameter header */
   CGI_PARAM *cgiAux = NULL;        /* auxiliary cgi parameter pointer */
   int argI;                        /* argument loop index */
   char *cgiCouple;                 /* cgi couple parameter name=value */

    /* Get CGI parameter list */
   if (!xisVar->cgiList) {
       xisVar->cgiList = cgi_read(&(xisVar->error));
       if (xisVar->error.code) return XIS_ERROR_CGI;
   }

    /* Get arguments as CGI parameter */
   for (argI = 1; argI < argc; argI++) {
      cgiCouple = strdup(argv[argI]);
      if (!cgiCouple) XIS_ERR(&(xisVar->error),XIS_ERROR_ALLOC, xisVar);
      cgiAux = cgi_newparam(&(xisVar->error),cgiAux,cgiCouple);
      if (!cgiAux) XIS_ERR(&(xisVar->error),XIS_ERROR_ALLOC, xisVar);
      if (!cgiHead) cgiHead = cgiAux;
   }

    /* Join the two list */
   if (xisVar->cgiList) {
       for (cgiAux = xisVar->cgiList; cgiAux->next; cgiAux = cgiAux->next);
       cgiAux->next = cgiHead;
   } else xisVar->cgiList = cgiHead;

    /* Return no error */
   return XIS_ERROR_OK;

} /* xisCgi */

/* ============================================================== xisArgRead */
int xisArgRead(XIS_VAR *xisVar, /* XIS variables */
               int argc,        /* quantity of arguments */
               char *argv[])    /* arrray of arguments */
{
   char *inArgV[MAX_IN_ARGV];    /* in arguments array */
   int inArgC;                          /* in arguments quantity */
   int argI;                            /* arguments list index */

    /* Access command line arguments */
    arg_read(xisVar->argList,ARG_LIST_QTT,argv,1,argc);

    /* Set instant arguments */
   xisArgInstant(xisVar);

    /* Show arguments */
    xisArgShow(xisVar,argc,argv);

    /* Loop while infile argument present */
    while (xisVar->argList[ARG_IN].text) {

        /* Set argument list title */
        inArgV[0] = xisVar->argList[ARG_IN].text;

        /* Split infile, generate argument list, on error return -ARG_ERROR_LIST */
        inArgC = arg_in(inArgV,1,MAX_IN_ARGV,xisVar->argList[ARG_IN].text);
       if (inArgC < 0) return efc_error(&(xisVar->error),inArgC,xisVar->argList[ARG_IN].text);
        xisVar->argList[ARG_IN].text = NULL;

        /* Access infile arguments */
        arg_read(xisVar->argList,ARG_LIST_QTT,inArgV,0,inArgC);

        /* Store CGI parameters, on error exit */
        if (xisCgi(xisVar,inArgC,inArgV))
            XIS_ERR(&(xisVar->error),XIS_ERROR_CGI, xisVar);

        /* Show valid arguments */
       xisArgInstant(xisVar);

        /* Show infile arguments */
        xisArgShow(xisVar,inArgC,inArgV);

   } /* while */

    /* Show valid arguments */
    if (xisVar->argList[ARG_ARGS].text) {
        printf("\n[Isis Script]\n");
        for (argI = 0; argI < ARG_LIST_QTT; argI++)
        if (xisVar->argList[argI].text) {
            printf("  %s",xisVar->argList[argI].id);
             if (xisVar->argList[argI].id_type != ARG_TYPE_BOOLEAN)
                printf("=%s",xisVar->argList[argI].text);
            printf("\n");
         }
   }

    /* Return no error */
   return 0;

} /* xisArgRead */

/* ============================================================== xisArgLoad */
void xisArgLoad(XIS_VAR *xisVar)        /* XIS variables */
{

    /* Load all arguments */
    arg_load(&(xisVar->argList[ARG_ARGS]),      "args",         ARG_TYPE_BOOLEAN);
    arg_load(&(xisVar->argList[ARG_COMPILE]),   "compile",      ARG_TYPE_TEXT);
    arg_load(&(xisVar->argList[ARG_HELLO]),     "hello",        ARG_TYPE_BOOLEAN);
    arg_load(&(xisVar->argList[ARG_IN]),        "in",           ARG_TYPE_TEXT);
    arg_load(&(xisVar->argList[ARG_SCRIPT]),    "IsisScript",   ARG_TYPE_TEXT);
    arg_load(&(xisVar->argList[ARG_SOURCE]),    "+source",      ARG_TYPE_BOOLEAN);
    arg_load(&(xisVar->argList[ARG_WHAT]),      "what",         ARG_TYPE_BOOLEAN);

} /* xisArgLoad */

/* ======================================================== xisGetIsisScript */
char *xisGetIsisScript(CGI_PARAM *cgip) /* cgi list item */
{
    char *p;            /* auxiliary loop pointer */
   int pos;         /* auxiliary loop string position */

    /* Find IsisScript */
   for (; cgip; cgip = cgip->next) {
        if (strcmp("IsisScript",cgip->name) == 0) {
        p = cgip->value;
         pos = strlen(cgip->value);
        for (p += pos; pos; p--, pos--) {
                if (*p == '\\' || *p == '/') {
                p++;
                break;
            }
         }
        return p;
      }
   } /* for */

   /* parameter IsisScript not found */
   return NULL;

} /* xisGetIsisScript */

#if LICENSED_VERSION
/* ============================================================== xisLicense */
#define LICENSE_TYPE_POS    21      /* 19993112 174009 5 365x */
                                            /* 012345678901234567890x */
LICERROR_LIST xisLicense(XIS_VAR *xisVar,       /* XIS variables */
                                 char *fileName)        /* license file name */
{
   char licenseType;                        /* license type */
   char *p;                                 /* file content buffer */
   char *remoteAddr;                        /* remote address */
   char *remoteHost;                        /* remote host */
   char *localAddr = "127.0.0.1";   /* authorized local address */
   char *localHost = "localhost";   /* authorized local host */
    EFC_SPLIT_LINES table;              /* split line structure */
   LONGX line;                               /* line index */
   LONGX licenseDate;                /* authorized license date */
   LICERROR_LIST licensed = LICERROR_UNAUTHO;   /* licensed flag */

   /* 1.1 */
    /* Check remote address or host */
   remoteAddr = cgi_getenv("REMOTE_ADDR");
   remoteHost = cgi_getenv("REMOTE_HOST");
    if (strcmp(remoteAddr,localAddr) == 0) return LICERROR_OK;
    if (strcmp(remoteAddr,localHost) == 0) return LICERROR_OK;
    if (strcmp(remoteHost,localAddr) == 0) return LICERROR_OK;
    if (strcmp(remoteHost,localHost) == 0) return LICERROR_OK;

    /* Check local server name */
/* 1.0
    if (strcmp(xisVar->licenseText,"localhost") == 0) return LICERROR_OK;
    if (strcmp(xisVar->licenseText,"127.0.0.1") == 0) return LICERROR_OK;
*/

    /* Get license file content */
   p = xisFileContent(xisVar,fileName);
   if (!*p) return LICERROR_FILE;

   /* Split lines */
    efc_split_lines(&table,p);

   /* Check license time */
   hide_expose(table.list[0].text);
   licenseDate = atol(table.list[0].text);
   if (licenseDate < XIS_LICMIN) {
    licensed = LICERROR_INVALID;
   } else {
        if (XIS_LICDATE > licenseDate + XIS_LICTIME) {
        licensed = LICERROR_EXPIRED;
        }
   }

   /* 08.Jun.2000 begin ]
   /* get the license type,
        2 types: 1 by server name
                2 by script name (IsisScript)
      to set the comparison text */

   /* lincese type is the first character after the standard time */
    licenseType = table.list[0].text[LICENSE_TYPE_POS];

   /* if license type is '\0' means the original standard type */
   if (licenseType == 's') {
    xisVar->licenseText = xisGetIsisScript(xisVar->cgiList);
   }

    /* check mandatory arguments */
   if (!xisVar->licenseText) licensed = LICERROR_MISSING;
   /* 08.Jun.2000 end ]

    /* if OK, check server names */
    if (licensed == LICERROR_UNAUTHO) {

       /* Loop all lines */
    for (line = 1L; line < table.qtt; line++) {

        /* Check licensed servers */
          hide_expose(table.list[line].text);
        if (strcmp(xisVar->licenseText,table.list[line].text) == 0) {
           licensed = LICERROR_OK;
            break;
          }

    } /* for */

   }

   /* Garbage collector */
    efc_split_lines_free(&table);

   /* Return licensed flag */
   return licensed;

} /* xisLicense */
#endif

/* ================================================================ xisStart */
void xisStart(XIS_VAR *xisVar,  /* XIS variables */
              char *argv0)      /* program itself */
{
    int auxLength;                       /* auxiliary length */
    char *p;                             /* auxiliary string buffer pointer */

    /* Set program path */
    xisVar->programPath = strdup(argv0);
    auxLength = strlen(xisVar->programPath);
    for (p = xisVar->programPath+auxLength; auxLength; p--,auxLength--) {
        if (*p == '\\' || *p == '/') {
            p++;
            break;
        }
    } /* for */
    *p = '\0';

    /* Set environment variable */
    sprintf(path_source,"PATH_WXIS=%s",xisVar->programPath);
    putenv(path_source);

    /* Call CISIS setup functions */
    if (firstTime) {
        dbxinit();
        recinit();
        trminit();
        firstTime = 0;
    }
    ifupd_wrnmsg = 0;

    /* Setup XIS variables*/
    xisVar->licenseText = getenv("SERVER_NAME");
    xisVar->pathTranslated = cgi_getenv("PATH_TRANSLATED");

    /* Setup compiler variables*/
    xisVar->xisProgram.compiledExt = XIS_COMPILED_EXT;

} /* xisStart */

/* ================================================================= xisFree */
void xisFree(XIS_VAR *xisVar)       /* XIS variables */
{

    /* Garbage collector */
   efc_free(xisVar->programPath);
#ifndef XIS_SERVER
   efc_free(xisVar->script);
#else
    //efc_delete_buffer();
#endif
   cgi_free(xisVar->cgiList);

} /* xisFree */

/* ================================================================ mainFunc */
int mainFunc(int argc,       /* quantity of arguments */
             char *argv[],   /* array of arguments */
             char *envv[],   /* array of environment variables */
             OUT_BUFFER *out)   /* server output buffer */
{
    XIS_VAR xisVar;                 /* XIS variables */
    XIS_ERROR errorCode;            /* XIS error code */
    LICERROR_LIST licenseError;     /* license error return code */
    CGI_PARAM *cgiAux;              /* auxiliary cgi parameter pointer */

/*
{
int i;
printf("Content-type: text/plain\n\n");
for (i = 0; i < argc; i++) {
    printf("argv[%d]: %s\n",i,argv[i]);
}
for (i = 0; envv[i]; i++) {
    printf("env[%d]: %s\n",i,envv[i]);
}
}
exit(0);
*/

    //chrono_start();

    outBuff = out;

#define AIX     0
#if AIX
    /* 1.2 avoid AIX behaviour [fatal: dbxopen/fd=0] */
    int fdAux;
    fdAux = open(argv[0],0);
#endif

    /* Start variables */
    memset(&xisVar,0x00,sizeof(XIS_VAR));
    xisStart(&xisVar,argv[0]);

    /* Store arguments, on error exit */
    xisArgLoad(&xisVar);
    if (xisArgRead(&xisVar,argc,argv) < 0)
        XIS_ERR(&(xisVar.error),XIS_ERROR_ARGS, &xisVar);

    /* Store CGI parameters, on error exit */
    if (xisCgi(&xisVar,argc,argv))
        XIS_ERR(&(xisVar.error),XIS_ERROR_CGI, &xisVar);

    /* Licensed server name */
    if (xisVar.licenseText) {
#if LICENSED_VERSION
        licenseError = xisLicense(&xisVar,xisLicenseFile);
#else
        licenseError = LICERROR_OK;
#endif
        switch (licenseError) {
            case LICERROR_OK :
                break;
            case LICERROR_FILE :
                XIS_ERR(&(xisVar.error),efc_error(&(xisVar.error),XIS_ERROR_LICENSE,xisLicenseFile), &xisVar);
            case LICERROR_INVALID :
                XIS_ERR(&(xisVar.error),efc_error(&(xisVar.error),XIS_ERROR_LICENSE,"invalid license"), &xisVar);
            case LICERROR_EXPIRED :
                XIS_ERR(&(xisVar.error),efc_error(&(xisVar.error),XIS_ERROR_LICENSE,"new release"), &xisVar);
            case LICERROR_UNAUTHO :
                XIS_ERR(&(xisVar.error),efc_error(&(xisVar.error),XIS_ERROR_LICENSE,xisVar.licenseText), &xisVar);
            case LICERROR_MISSING :
                XIS_ERR(&(xisVar.error),efc_error(&(xisVar.error),XIS_ERROR_LICENSE,"server name/script name"), &xisVar);
        } /* switch */
    }

    /* Find script parameter */
    for (cgiAux = xisVar.cgiList; cgiAux; cgiAux = cgiAux->next) {
        if (strcmp(cgiAux->name,xisVar.argList[ARG_SCRIPT].id) == 0) {
            xisVar.argList[ARG_SCRIPT].text = cgiAux->value;
            break;
        }
    }

    /* The WXIS is called by the server via the .xis/.xic configuration */
    if ( !xisVar.argList[ARG_SCRIPT].text ) {
        xisVar.argList[ARG_SCRIPT].text = getenv("PATH_TRANSLATED");
    }

    /* Run script */
    errorCode = xisRun(&xisVar);

    /* Garbage collector */
    xisFree(&xisVar);


    //chrono_stop("mainFunc");

    /* Return error code */
    return errorCode;
} /* mainFunc */


/* ==================================================================== main */
#if GEN_MAIN
int main(int argc,       /* quantity of arguments */
         char *argv[],   /* arrray of arguments */
         char *envv[])   /* arrray of environment variables */
{
    int errorCode =  0;

    //chrono_init("chrono_wxis.txt");

    mainFunc(argc, argv, envv, NULL);

    //chrono_end();

    /* Return error code */
    exit(errorCode);

    return 0;
}
#endif /* GEN_MAIN */
