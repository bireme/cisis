/* PROGRAM: Hybrid sort (polyphase merge with quicksort presorter) */
/* AUTHOR: Kelly McTiernan, Feb. 4, 1990                           */
/* USAGE:  psort reclen src dest: reclen--record length in bytes;  */
/* src--source file name; dest--destination file name              */
/* NOTES: Sort routine is attached to a test driver for your con-  */
/* venience.  Functions main() and compare() illustrate calling    */
/* conventions.  This test driver is not very forgiving, the sort  */
/* is intended for more flexible usage as part of a larger         */
/* package.  This has been implemented in Turbo C. This is         */
/* configured for large memory model.                              */
/*
   Changed in may/2001 for CISIS Interface purposes.
   AOT, 19/05/2001
*/

#include <stdio.h>            
#include <string.h>

#if CIAPI
#include "ciapi.h"
#else /* CIAPI */
#include "cisis.h"                  /* CISIS Interface header file */
#endif /* CIAPI */

#define      MAX_STK         512 /*128*/         /* Local stack size */
#define      TMP_FILES       33 /*9*/           /* Maximum number of merge files */
#define      HEAP_SAVE    8192           /* Amount to save for FILE'S */
#define      MAX_BUF (unsigned)  0xFFFF  /* largest possible buffer */

#define      OPEN_ERR        1           /* Local error codes */
#define      OPEN_ERR_OUT        11           /* cannot create output file */
#define      OPEN_ERR_INP        12           /* cannot read input file */
#define      OPEN_ERR_TMP        13           /* cannot create tmp file */
#define      READ_ERR        2
#define      WRITE_ERR       3
#define      ALLOC_ERR       4
/* #define      DATA_ERR        5 */
#define      PARM_ERR         6
#define      MOVE_ERR        998
#define      PROG_ERR        999


typedef struct mys_stru {
int     last_file;                   /* Actual number of files */
int     last_minus_one;              /* Eliminates major invariant */
int     actual_run_count[TMP_FILES]; /* Ideal number of runs */
int     dummy_run_count[TMP_FILES];  /* Difference from ideal */
int     least_runs;             /* Number of runs at a level */
int     select;                 /* Next file to receive run */
int     level;                  /* Number of passes needed */
void    *last_item[TMP_FILES];  /* Buffered I/O variables */
void    *next;
FILE    *tmp_files[TMP_FILES];
char    fnames[TMP_FILES][FILENAME_MAX+1]; /* Filenames for temp files */
int      end_data[TMP_FILES]; /* EOF for local buffers */
int      end_of_data;
size_t   max_items;   /* Maximum records a buffer can hold */
size_t   num_ptrs;    /* Number of items for presort */
void     *buf;        /* R/W pointer */
void     *bs;         /* last item read */
void     **buf_ptrs;  /* Array of pointers to records */
size_t   rd_count;    /* Static read counter, last number read */
size_t     rsize;     /* Global for record size */
void     *spos[TMP_FILES];      /* Store position in buffer */
void     *rpos[TMP_FILES];      /* Retrieve position pointer */
void     *top[TMP_FILES];       /* Buffer tops */
void     *next_item[TMP_FILES]; /* Next item to be read */
size_t   buf_cnt[TMP_FILES];    /* Buffer counter */
int      myserror; /* program error set by funserror(xmysp, ) */
int      rec_len;     /* For our driver routine */
int      index[TMP_FILES];         /* Index to merge file buffers */
char     *gidbnp; /* dbn.par */
FILE     *src; /* now global and fclose(src) in funserror() - AOT 19/09/2001 */
#if CIAPI
    void *ciapip;
#endif /* CIAPI */
} MYS_STRU;

/* MYS_STRU */
#define last_file       xmysp->last_file
#define last_minus_one  xmysp->last_minus_one
#define actual_run_count xmysp->actual_run_count
#define dummy_run_count xmysp->dummy_run_count
#define least_runs      xmysp->least_runs
#define select          xmysp->select
#define level           xmysp->level
#define last_item       xmysp->last_item
#define next            xmysp->next
#define tmp_files       xmysp->tmp_files
#define fnames          xmysp->fnames
#define end_data        xmysp->end_data
#define end_of_data     xmysp->end_of_data
#define max_items       xmysp->max_items
#define num_ptrs        xmysp->num_ptrs
#define buf             xmysp->buf
#define bs              xmysp->bs
#define buf_ptrs        xmysp->buf_ptrs
#define rd_count        xmysp->rd_count
#define rsize           xmysp->rsize
#define spos            xmysp->spos
#define rpos            xmysp->rpos
#define top             xmysp->top
#define next_item       xmysp->next_item
#define buf_cnt         xmysp->buf_cnt
#define myserror        xmysp->myserror
#define rec_len         xmysp->rec_len
#define index           xmysp->index
#define gidbnp          xmysp->gidbnp
#define src             xmysp->src
#define ciapip          xmysp->ciapip

#define serror(x)       { funserror(xmysp,x); return; }
#define serrorX(x)      { funserror(xmysp,x); return(x); }

int   funserror(MYS_STRU *xmysp, int e);
void  select_file(MYS_STRU *xmysp);
void  generate_run(MYS_STRU *xmysp, FILE *fp);
void  b_set_rd(MYS_STRU *xmysp, int n);
void  b_set_wr(MYS_STRU *xmysp, int b);
void  bflush(MYS_STRU *xmysp, int b);
void  buf_free(MYS_STRU *xmysp);
void  fill_buffer(MYS_STRU *xmysp, FILE *fp);
void  quick_sort(MYS_STRU *xmysp, void *a[], int l, int r);
void  bmove(MYS_STRU *xmysp, int n, int m);
void  bfill(MYS_STRU *xmysp, int n);
void  buf_init(MYS_STRU *xmysp, FILE *fp);
int   poly_sort(MYS_STRU *xmysp, char *srcname, int rec_size, char *name);


int poly_sort(xmysp, srcname,rec_size,name)
MYS_STRU *xmysp;
char *srcname;      /* File to sort, open for reading */
int  rec_size;  /* Length of records in bytes */
char *name;     /* Name of sorted file */
{
#if BEFORE20010919
FILE *src;      /* File to sort, open for reading */
#endif
	/*register*/ int     i;
	int      tmp_idx,  tmp_d;
	int      min_index;
	int      active_index[TMP_FILES];  /* Index to active files in merge */
	void     *min;
	int      active_count;             /* Number files currently active */
	void     *tmp;
/* MYSFUN */
/* FILE     *fp; */
   int      wffd;
	if (!srcname || !name || rec_size < 0)          /* Parse command line */
		return(PARM_ERR);
   /* create empty output file */
#if 0
   /* remove(name); */
   fp=fopen(name, "wb");
   if (fp) fclose(fp); else return(OPEN_ERR_OUT);
	if((src = fopen(srcname,"rb")) == NULL)
#endif
   dbxopenc(gidbnp,dbxcipar(gidbnp,name,'='),&wffd,NULL,NULL,1,0); /*..force create= */
   if (wffd <= 0) return(OPEN_ERR_OUT); else CLOSE(wffd);
	if((src = fopen(dbxcipar(gidbnp,srcname,'='),"rb")) == NULL)
		return(OPEN_ERR_INP);
/* MYSFUN */
	rec_len = rec_size;
	rsize = rec_size;
	rewind(src);
	buf_init(xmysp, src);      /* Allocate memory, init buffer */
	if (end_of_data)
		serrorX(0/*DATA_ERR*/);
	/* Initialize, distribute one run to each merge file */
	for (i = 0; i < last_minus_one; i++) {
		actual_run_count[i] = 1;
		dummy_run_count[i] = 1;
		tmpnam(fnames[i]);
		tmp_files[i] = fopen(fnames[i], "wb+");
		if (tmp_files[i] == NULL)
			serrorX(OPEN_ERR_TMP);
	}
	level = 1;
	select = 0;
	actual_run_count[last_minus_one] = 0;
	dummy_run_count[last_minus_one] = 0;
	do {
		select_file(xmysp);
		generate_run(xmysp, src);
	} while (!end_of_data && select != last_file - 2);
	/* Distribute runs, account for next run being continuation of the */
	/* previous run or an out-of-data condition */
	while (!end_of_data) {
		select_file(xmysp);
		while (memcmp(last_item[select], next, rec_len) <= 0) {
			generate_run(xmysp, src);          /* Continue old run */
			if (end_of_data)
				break;
		}
		if (end_of_data)                    /* Then use a null run */
			dummy_run_count[select]++;
		else
			generate_run(xmysp, src);         /* Otherwise generate new run */
	}

	for (i = 0; i < last_minus_one; i++) {
		b_set_rd(xmysp, i);
		index[i] = i;
	}
	index[last_minus_one] = last_minus_one;
	tmpnam(fnames[last_minus_one]);
	tmp_files[last_minus_one] = fopen(fnames[last_minus_one], "wb+");
	if (tmp_files[last_minus_one] == NULL)
		serrorX(OPEN_ERR_TMP);
	do {            /* Begin merge operation */
		least_runs = actual_run_count[last_file - 2];
		dummy_run_count[last_minus_one] = 0;
		b_set_wr(xmysp, index[last_minus_one]);
		do {         /* Merge one run */
			active_count = 0;
			for (i = 0; i < last_minus_one; i++){
				if (dummy_run_count[i] > 0)
					dummy_run_count[i]--;	/* Exclude from merge */
				else {                       /* Participate in merge */
					active_index[active_count] = index[i];
					active_count++;
				}
			}
			if (!active_count)           /* All dummy runs, do dummy merge */
				dummy_run_count[last_minus_one]++;
			else /* Do real merge */
				do {   /* Find smallest item */
					min_index = 0;
					min = next_item[active_index[0]];
					for (i = 1; i < active_count; i++) {
						tmp = next_item[active_index[i]];
						if (memcmp(tmp, min, rec_len) < 0) {
							min = tmp;
							min_index = i;
						}
					}
					/* Move min item to test buffer */
					tmp_idx = active_index[min_index];
					bmove(xmysp, index[last_minus_one], tmp_idx);
					if (memcmp(buf, next_item[tmp_idx], rec_len) > 0 || end_data[tmp_idx]) {
						active_count--;
						active_index[min_index] = active_index[active_count];
					}
				} while (active_count); /* Do until one run from each merged */
			/* Continue merging until file with least runs is empty */
		} while (--least_runs);
		bflush(xmysp, index[last_minus_one]);		/* Rotate files */
		b_set_rd(xmysp, index[last_minus_one]);
		tmp_idx = index[last_minus_one];
		tmp_d = dummy_run_count[last_minus_one];
		least_runs = actual_run_count[last_file - 2];
		for (i = last_minus_one; i > 0; i--) {
			dummy_run_count[i] = dummy_run_count[i - 1];
			actual_run_count[i] = actual_run_count[i - 1] - least_runs;
			index[i] = index[i - 1];
		}
		index[0] = tmp_idx;
		dummy_run_count[0] = tmp_d;
		actual_run_count[0] = least_runs;
	} while (--level);               /* Do until all runs merge to one */
	/* Merge sort complete, index[0] is sorted file */
	/* Delete all temporary files and rename sorted file */
	for (i = 1; i < last_file; i++) {
		fclose(tmp_files[index[i]]); tmp_files[index[i]]=NULL;
		remove(fnames[index[i]]);
	}
	fclose(tmp_files[index[0]]); tmp_files[index[0]]=NULL;
	fclose(src); src=NULL; /* AOT 19/09/2001 */

   name=dbxcipar(gidbnp,name,'=');
   remove(name);
#if UNIX
   sprintf(fnames[0],"mv %s %s",fnames[index[0]],name);
   if (system(fnames[0])) serrorX(MOVE_ERR);
#else
   if (rename(fnames[index[0]],name)) serrorX(MOVE_ERR);
#endif
	buf_free(xmysp);
/*
	fp = fopen(name, "r+");
	if (fp == NULL)
		return(OPEN_ERR_END);
   fclose(fp);
*/
	return(0);
}

void select_file(xmysp)
MYS_STRU *xmysp;
{                         /* Selects file for run distribution */
	int    i, mn;
	if (dummy_run_count[select] < dummy_run_count[select + 1])
		select++;            /* Select next file */
	else {
		if (dummy_run_count[select] == 0) {   /* Ideal reached */
			level++;                  /* Generate next level */
			mn = actual_run_count[0];
			for (i = 0; i < last_minus_one; i++) {
				dummy_run_count[i] = mn + actual_run_count[i + 1] - actual_run_count[i];
				actual_run_count[i] = mn + actual_run_count[i + 1];
			}  /* Actual run count = ideal, dummy runs = number to add */
		}     /* to the previous level to reach that ideal */
		select = 0;                  /* Select for a run */
	}
	dummy_run_count[select]--;
}
void buf_init(xmysp, fp)
MYS_STRU *xmysp;
FILE *fp;               /* Get memory, init buffers for generate_run(xmysp, ) */
{
	void      *tmp;         /* Allocate, then free to save */
	int       i;
	tmp = malloc(HEAP_SAVE);
	bs = malloc(rsize);     /* Emergency item save buffer */
	for (i = 0; i < TMP_FILES; i++) {
		last_item[i] = malloc(rsize);
		if (!last_item[i])
			serror(ALLOC_ERR);
	}
	max_items = MAX_BUF/rsize;
	num_ptrs = MAX_BUF/sizeof(void *);
	buf_ptrs = (void **)malloc(MAX_BUF);
	if (!(buf_ptrs && bs))
		serror(ALLOC_ERR);
	for (i = 0; i < TMP_FILES - 1; i++) {
		top[i] = malloc(MAX_BUF);
		if (top[i] == NULL)
			break;
	}
	top[i=TMP_FILES - 1] = (void *)buf_ptrs; /* Gets us an extra buffer without realloc */
	last_minus_one = i;
	i++;
	if (i < 3)                /* At least three files needed to merge */
		serror(ALLOC_ERR);
	free(tmp);                /* Return to system rool */
	last_file = i;
	fill_buffer(xmysp, fp);
	--rd_count;
	switch (rd_count) {
	case -1:
		end_of_data = 1;
		return;
	case 0:
		break;
	default:
		quick_sort(xmysp, buf_ptrs, 0, rd_count);
		break;
	}
	next = buf_ptrs[0];
	end_of_data = 0;
}
void generate_run(xmysp, srcx)
MYS_STRU *xmysp;
FILE *srcx;      /* Use avail memory to gen maximally large runs */
{               /* init_buf() loaded and presorted buffer */
	size_t  i;
	size_t  w;
	if (end_of_data)
		return;
	memmove(last_item[select], buf_ptrs[rd_count], rsize);
	for (i = 0; i <= rd_count; i++) {
		w = fwrite(buf_ptrs[i],rsize,(size_t)1,tmp_files[select]);
		if (w != 1)
			serror(WRITE_ERR);
	}
	fflush(tmp_files[select]);
	fill_buffer(xmysp, srcx);
	--rd_count;
	switch (rd_count) {
	case -1:                /* Nothing read */
		end_of_data = 1;
		break;
	case 0:                 /* One item read */
		break;
	default:
		quick_sort(xmysp, buf_ptrs, 0, rd_count);
		break;
	}
	next = buf_ptrs[0];
}
void fill_buffer(xmysp, fp)
MYS_STRU *xmysp;
FILE *fp;           /* Fill sort buffer from source file */
{
	void  **p;
	void  *bp;
	int     i;
	size_t  j, cnt, num_to_read;
	if (feof(fp)) {
		rd_count = 0;
		return;
	}
	p = buf_ptrs;
	rd_count = 0;
	num_to_read = max_items;                /* Try for a full buffer */
	for (i = 0; i < last_minus_one; i++) {  /* Read across boundaries */
		cnt = fread(top[i], rsize, num_to_read, fp);
		rd_count += cnt;
		bp = top[i];
		for (j = 0; j < cnt; j++) {  /* Set pointers to items for sort */
			*p++ = bp;
			/* (char *)bp += rsize; */
			bp = (void *)((char *)bp + rsize);

		}
		if (cnt < max_items)        /* EOF */
			break;
		if (rd_count + max_items >= num_ptrs)  /* Don't read too much */
			num_to_read = num_ptrs - rd_count;
	}
}
void b_set_rd(xmysp, n)
MYS_STRU *xmysp;
int  n;
{
	rewind(tmp_files[n]);
	bfill(xmysp, n);                  /* Fill buffer */
}
void bfill(xmysp, n)
MYS_STRU *xmysp;
int  n;
{
	size_t  cnt;
	rpos[n] = spos[n] = top[n];
	if (feof(tmp_files[n])) {
		end_data[n] = 1;
		return;
	}
	cnt = fread(top[n], rsize, max_items, tmp_files[n]);
	if (cnt == 0) {
		end_data[n] = 1;
		return;
	}
	/* (char *)spos[n] += cnt * rsize; */
	spos[n] = (char *)spos[n] + cnt * rsize;
	buf_cnt[n] = 0;
	end_data[n] = 0;
	next_item[n] = top[n];
}
void b_set_wr(xmysp, n)
MYS_STRU *xmysp;
int  n;
{
	spos[n] = rpos[n] = top[n];
	fclose(tmp_files[n]);                  /* Truncate file */
	tmp_files[n] = fopen(fnames[n], "wb+");
	if (tmp_files[n] == NULL)
		serror(OPEN_ERR);
	buf_cnt[n] = 0;
}
void bflush(xmysp, n)
MYS_STRU *xmysp;
int  n;         /* Flush write buffer */
{
	size_t w;
	memmove(bs, buf, rsize);     /* Save! We are going to flush */
	buf = bs;
	w = fwrite(rpos[n], rsize, buf_cnt[n], tmp_files[n]);
	if (w != buf_cnt[n])
		serror(WRITE_ERR);
	fflush(tmp_files[n]);
	buf_cnt[n] = 0;
	spos[n] = rpos[n] = top[n];          /* Buffer empty */
}
void bmove(xmysp, n, m)
MYS_STRU *xmysp;
int  n;         /* Index of destination buffer */
int  m;         /* Index of source buffer */
{
	if (end_data[m])
		return;
	memmove(spos[n], next_item[m], rsize);
	buf = spos[n];
	/* (char *)spos[n] += rsize; */
	spos[n] = (char *)spos[n] + rsize;
	buf_cnt[n]++;
	if (buf_cnt[n] == max_items)
		bflush(xmysp, n);
	/* (char *)rpos[m] += rsize; */
	rpos[m] = (char *)rpos[m] + rsize;
	buf_cnt[m]++;
	if(rpos[m] >= spos[m])
		bfill(xmysp, m);
	else
		next_item[m] = rpos[m];
}

int funserror(xmysp, e)
MYS_STRU *xmysp;
int  e;         /* Minimal error handler */
{
   int i;
   myserror=e;
	if (src) {
        fclose(src); src=NULL; /* AOT 19/09/2001 */
    }
	for (i = 0; i < last_file; i++) {
		if (tmp_files[index[i]]) {
          fclose(tmp_files[index[i]]); tmp_files[index[i]]=NULL;
		    remove(fnames[index[i]]);
	   }
   }
	buf_free(xmysp);
	return(e);
}

void buf_free(xmysp)
MYS_STRU *xmysp;
{
	/*register*/ int    i;
	if (bs) { free(bs); bs=NULL; }
	for (i = 0; i < TMP_FILES; i++)
		if (last_item[i]) { free(last_item[i]); last_item[i]=NULL; }
	for (i = 0; i < TMP_FILES - 1; i++) /* MYSFUN !! */
		if (top[i]) { free(top[i]); top[i]=NULL; }

	if (buf_ptrs) { free(buf_ptrs); buf_ptrs=NULL; }
}
void quick_sort(xmysp, a, l, r)
MYS_STRU *xmysp;
void *a[];      /* Array of pointers to sort */
int  l, r;      /* Left and right index */
{
	/*register*/ int i;
	/*register*/ int j;
	void     *mid;
	void     *tmp;
	struct stack {
		int l;
		int r;
	};
	struct stack s[MAX_STK];    /* Local stack variable */
	struct stack *sp;
   if (xmysp) if (!xmysp) return; /* no warning */
	sp = s;
	sp->l = l;                  /* Stack initial request */
	(sp++)->r = r;
	do {
		l = (--sp)->l;         /* Retrieve partition bounds from stack */
		r = sp->r;
		do {
			i = l;              /* Split into two halves */
			j = r;
			mid = a[(l + r)/2];
			do {
				while(memcmp(a[i], mid, rec_len) < 0) i++;
				while(memcmp(a[j], mid, rec_len) > 0) j--;
				if (i <= j) {        /* Swap */
					tmp = a[i];
					a[i++] = a[j];
					a[j--] = tmp;
				}
			} while (i <= j);
			if (j - l < r - i) {    /* Left partition smaller? */
				if (i < r) {         /* And bigger than one item? */
					sp->l = i;        /* Stack right partition */
					(sp++)->r = r;
				}
				r = j;               /* Sort left partition */
			} else {                /* Right partition = left */
				if (l < j) {         /* Bigger than one item? */
					sp->l = l;        /* Stack left partition */
					(sp++)->r = j;
				}
				l = i;               /* Sort right partition */
			}
		} while (l < r);           /* Repeat until partition size is zero */
	} while (sp != s);        /* Continue until stack is empty */
}


#if !MYSFUN
int cisis_mysfunc(char *xgidbnp, int xparmlen, char *xparmin, char *xparmout, LONGX xparmtell);
#endif /* MYSFUN */

/* MYSFUN */
int cisis_mysfunc(parmgidbnp, parmlen, parmin, parmout, parmtell)
char *parmgidbnp; /* dbn.par */
int parmlen;
char *parmin;
char *parmout;
LONGX parmtell;
{
   MYS_STRU *xmysp;
   int rc;

/*
#if CICPP
   xmysp=(MYS_STRU *)new char[sizeof(MYS_STRU)];
#else
*/
   xmysp=(MYS_STRU *)ALLOC((ALLOPARM)sizeof(MYS_STRU));
/*
#endif
*/
   if (xmysp == (MYS_STRU *)NULL) return(PROG_ERR);
   memset(xmysp,0x00,sizeof(MYS_STRU));
   gidbnp=parmgidbnp;

	if (parmtell) printf("starting sort.. (%d,%s,%s)\n",rec_len,parmin,parmout);

#if CIAPI
    ciapip = cisisApplication( MAXNREC,MAXNTRM );/* mandatory for CIAPI definitions */
#endif /* CIAPI */

	rc=poly_sort(xmysp, parmin, parmlen, parmout);

#if CIAPI
    cisisApplicationDelete( ciapip );
#endif /* CIAPI */

	if (parmtell)
       if (rc)
           printf("sort error #%d\n",rc);
       else
           printf("sort completed (files used: %d)\n",last_file);

/*
#if CICPP
   delete [] (char *)xmysp;
#else
*/
   FREE(xmysp);
/*
#endif
*/
   return(rc);
}
/* MYSFUN */


#if MYSMAIN || !MYSFUN
#include "cirun.h"
void main(argc, argv)
int argc;
char *argv[];
{
	int parmlen;
	char *parmin=argv[2];
	char *parmout=argv[3];
	LONGX parmtell=1;
   int rc;

   int link1len=LE1+1+8+(LIND||ciiflfim?0:1+5+1+4+1+4)+(CRLF?2:1); /* 27: key %8"_LD_" %5d %4d %4d */
   int link2len=LE2+1+8+(LIND||ciiflfim?0:1+5+1+4+1+4)+(CRLF?2:1); /* 57: key %8"_LD_" %5d %4d %4d */

	if (argc < 4) {          /* Parse command line */
        printf("%s",cicopyr("Utility MYS"));
        printf("\n");
        printf("mys {link1|link2|<preclen>} <fileln> <filelk> [<option> [...]] \n");
        printf("\n");
        printf("options:  tell=<n> \n");
        /*printf("          +fix/m \n");*/
        printf("\n");
        printf("\n");
        printf(" Ex: mys %d x.ln1 x.lk1 \n",link1len);
        printf("     mys link1 x.ln1 x.lk1 tell=0 \n");
        printf(" \n");
        printf(" Ex: mys %d x.ln1 x.lk1 \n",link2len);
        printf("     mys link2 x.ln2 x.lk2 tell=0 \n");
        printf("\n");
		  exit(0);
   }

	parmlen = atoi(argv[1]);
   if (strcmp(argv[1],"link1") == 0) parmlen=link1len;
   if (strcmp(argv[1],"link2") == 0) parmlen=link2len;

   if (argc > 4) {
      if (strncmp(argv[4],"tell=",5) == 0) {
         if (sscanf(argv[4]+5,"%"_LD_,&parmtell) != 1) exit(PARM_ERR);
      }
      else exit(PARM_ERR);
   }

   rc=cisis_mysfunc(NULL,parmlen,parmin,parmout,parmtell);
   exit(rc);
}
#endif /* MYSMAIN || !MYSFUN*/

