/* -------------------------------------------------------------- FREQLIST.C */

/* /////////////////////////////////////////////////////////////////////////

   [ Version 1.0 ]
   17.Nov.1998 - Frequency list.

   ///////////////////////////////////////////////////////////////////////// */

/* ---------------------------------------------------------- C HEADER FILES */
#include <string.h>
/* ------------------------------------------------------------ HEADER FILES */
/* include "cisis.h" */
#include "easyfc.h"
#include "freqlist.h"

/* --------------------------------------------------------------- freq_free */
void freq_free(STRUCT_FREQLIST *freq_pos)
{
   STRUCT_FREQLIST *freq_next;

   for ( ; freq_pos; freq_pos = freq_next) {
      freq_next = freq_pos->next;
      efc_free(freq_pos->item);
      efc_free(freq_pos->sort);
      efc_free(freq_pos);
   }
}
/* ---------------------------------------------------------------- freq_new */
STRUCT_FREQLIST *freq_new(STRUCT_FREQLIST *prev,
                          STRUCT_FREQLIST *next,
                          char *item)
{
   STRUCT_FREQLIST *new_item;

   new_item = (STRUCT_FREQLIST *)efc_new(sizeof(STRUCT_FREQLIST));
   if (!new_item) return NULL;
   new_item->item = strdup(item);
   if (!new_item->item) return NULL;
   new_item->qtt = 0L;
   new_item->sort = NULL;
   new_item->prev = prev;
   new_item->next = next;
   if (prev) prev->next = new_item;
   if (next) next->prev = new_item;
   return new_item;
}
/* --------------------------------------------------------------- freq_find */
STRUCT_FREQLIST *freq_find(FREQLIST_VAR *freq_var,
                                    STRUCT_FREQLIST *freq_pos,
                           char *item,
                           BOOLEAN *is_equal)
{
   *is_equal = FALSE;
   for ( ; freq_pos; freq_pos = freq_pos->next) {
      if (strcmp(item,freq_pos->item) > 0) continue;
      if (strcmp(item,freq_pos->item) == 0) {
        *is_equal = TRUE;
         break;
      }
      if (!freq_var->check_all) break;
   }
   return freq_pos;
}
/* ---------------------------------------------------------------- freq_add */
BOOLEAN freq_add(FREQLIST_VAR *freq_var,
                      char *item,
                 LONGX freq_count,
                 BOOLEAN list_style,
                 BOOLEAN sort_style)
{
   STRUCT_FREQLIST *freq_pos;
   STRUCT_FREQLIST *prev;
   STRUCT_FREQLIST *next;
   BOOLEAN is_equal = FALSE;

   if (!*item) return TRUE;
   freq_pos = list_style ? NULL : freq_find(freq_var,freq_var->head,item,&is_equal);
   if (!is_equal || list_style || sort_style) {
      prev = freq_var->tail;
      next = NULL;
      if (freq_pos) {
         prev = freq_pos->prev;
         next = freq_pos;
      }
      freq_pos = freq_new(prev,next,item);
      if (!freq_pos) return FALSE;
      if (!prev) freq_var->head = freq_pos;
      if (!next) freq_var->tail = freq_pos;
      freq_var->item_tot++;
   }
   freq_pos->qtt += freq_count;
   freq_var->freq_tot += freq_count;
   return TRUE;
}
/* --------------------------------------------------------------- freq_load */
BOOLEAN freq_load(FREQLIST_VAR *freq_var,
                    char *itens,
                    LONGX value,
                  BOOLEAN list_style,
                  BOOLEAN sort_style)
{
   char *p;
   char *pbeg;

   for (p = pbeg = itens; ; p++) {
      if (*p && *p != '\r' && *p != '\n') continue;
      while (*p == '\r' || *p == '\n') *p++ = '\0';
      if (!*pbeg) break;
      if (!freq_add(freq_var,pbeg,value,list_style,sort_style)) return FALSE;
      pbeg = p--;
   }

   return TRUE;
}
/* ---------------------------------------------------------- freq_sort_find */
STRUCT_FREQLIST *freq_sort_find(STRUCT_FREQLIST *freq_pos,
                                char *item)
{
   for ( ; freq_pos; freq_pos = freq_pos->next)
      if (strcmp(item,freq_pos->sort) < 0) break;
   return freq_pos;
}
/* --------------------------------------------------------------- freq_sort */
void freq_sort(FREQLIST_VAR *freq_var)
{
   STRUCT_FREQLIST *freq_pos;
   STRUCT_FREQLIST *freqnext;
   STRUCT_FREQLIST *sort_head = NULL;
   STRUCT_FREQLIST *sort_tail = NULL;
   STRUCT_FREQLIST *sort_pos;
   STRUCT_FREQLIST *prev;
   STRUCT_FREQLIST *next;

   for (freq_pos = freq_var->head; freq_pos; freq_pos = freqnext) {
      sort_pos = freq_sort_find(sort_head,freq_pos->sort);
      freqnext = freq_pos->next;

      prev = sort_tail;
      next = NULL;
      if (sort_pos) {
         prev = sort_pos->prev;
         next = sort_pos;
      }

      freq_pos->prev = prev;
      freq_pos->next = next;
      if (prev) prev->next = freq_pos;
      if (next) next->prev = freq_pos;

      if (!prev) sort_head = freq_pos;
      if (!next) sort_tail = freq_pos;
   }
   freq_var->head = sort_head;
   freq_var->tail = sort_tail;
   freq_var->check_all = TRUE;
}

