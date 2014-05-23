/* -------------------------------------------------------------- FREQLIST.H */

/* ------------------------------------------------------------- enumeration */
typedef enum {
    FREQ_ERROR_OK,
    FREQ_ERROR_ALLOC,
    FREQ_ERROR_QTT
} FREQ_ERROR;				/* error codes */
/* --------------------------------------------------------------- structure */
typedef struct stru_freq_list {
   char *item;                  	/* frequency sorted item */
   LONGX qtt;                     /* quantity of itens found */
   char *sort;                  	/* sort key for display */
   struct stru_freq_list *prev;	/* previous list item */
   struct stru_freq_list *next;  /* next list item */
} STRUCT_FREQLIST;               /* frequency list */

typedef struct {
   STRUCT_FREQLIST *head;
   STRUCT_FREQLIST *tail;
   LONGX item_tot;
   LONGX freq_tot;
   BOOLEAN check_all;				/* TRUE if the list is not sorted by item */
} FREQLIST_VAR;               	/* frequency list variables */
/* -------------------------------------------------------------- prototypes */
void 		freq_free	(STRUCT_FREQLIST *freq_pos);
BOOLEAN 	freq_load	(FREQLIST_VAR *freq_var,char *itens,LONGX value,BOOLEAN list_style,BOOLEAN sort_style);
void 		freq_sort	(FREQLIST_VAR *freq_var);

