/* --------------------------------------------------------------- CGILIST.H */

/* ------------------------------------------------------------- enumeration */
typedef enum {
	CGI_ERROR_OK,
	CGI_ERROR_ALLOC,
	CGI_ERROR_POST,
	CGI_ERROR_QTT
} CGI_ERROR_LIST;
/* -------------------------------------------------------------- structures */
typedef struct cgi_param_stru {
   char *name;
   char *value;
   struct cgi_param_stru *next;
} CGI_PARAM;
/* -------------------------------------------------------------- prototypes */
void			cgi_free		(CGI_PARAM *cgiparam);
char 		  *cgi_getenv	(char *env_name);
CGI_PARAM  *cgi_newparam (EFC_ERROR *err,CGI_PARAM *cgi_pos,char *cgicouple);
CGI_PARAM  *cgi_read		(EFC_ERROR *err);
CGI_PARAM  *cgi_split	(EFC_ERROR *err,char *query_string);
