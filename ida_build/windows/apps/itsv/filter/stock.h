/* stock set of macros and declarations for DSAP programs danq 6/5/92 */
/* $Name $Revision: 1.1 $ $Date: 2000/07/29 01:10:38 $ */

#ifndef _stock_
#define _stock_


#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>



#include <string.h>

 
//typedef Tbl     Bitvector;
#define BITS_PER_INT   	(8*sizeof(int))


#define HOOK_MAGIC 814615491

typedef struct Hook {
    int magic ; 
    void (*free_hook)(void *) ;
    void *p ;
} Hook ; 

typedef struct Xlat {
        char *name ;
        int num ;
} Xlat ;

#define PL_(x) ( )
extern Hook *new_hook( void (*hookfree)(void *));
extern void free_hook(Hook **hookp);

extern char **dsap_glob PL_(( char *v ));
extern char *ask PL_(( char *fmt, ... ));
extern char * getdsap ( void );
extern int newdata PL_(( char *returnpath, char *envname, char *dirname, char *filename, char *suffix ));
extern char *datafile PL_(( char *envname, char *filename ));
extern char *datapath PL_(( char *envname, char *dirname, char *filename, char *suffix ));
extern char *newcs PL_(( char *f_s, int n_s )) ;
extern char * expand_env ( char *s );

//extern Tbl *split( char *s, int c );
extern FILE *zopen( char *filename, char *permissions );
extern FILE *gz_open( char *filename, char *permissions );
extern FILE *gz_cat( char *filename, char *permissions );
extern int abspath(char *relp, char *absp );
extern int askyn( char *fmt, ... );
extern int blank( char *s );
extern void compress( char *path );
extern void dirbase( char *path, char *dir, char *base );
extern int fixup_env( char *envsetup[], int nenv, int force );
extern int envfile ( char *filename );
extern int gethdir( char *user, char *home );
extern void isort( char *pbase, int total_elems, int size, int (*cmp)(), void *prvt );
extern int makedir( char *dir );
extern int mappath( char *spath, char *upath );
extern void quicksort( char *pbase, size_t total_elems, size_t size, int (*cmp)(), void *prvt );
extern void shellsort( char *a, int n, int size, int (*compare)(), void *prvt );
extern int whitespace( char *s );
extern int blank( char *s );
extern int whitespace( char *s );
//extern Tbl *split( char *s, int c );
extern void sncopy( char *dest, char *source, int n );
extern void szcopy( char *dest, char *source, int n );
extern void copystrip( char *dest, char *source, int n );
extern void *memdup( void *a, int n );

extern int bitset PL_(( Bitvector *bvec, int index ));
extern int bitclr PL_(( Bitvector *bvec, int index ));
extern int bittst PL_(( Bitvector *bvec, int index ));
extern int bitmax PL_(( Bitvector *b ));
//extern Bitvector * bitand PL_(( Bitvector *b1, Bitvector *b2 ));
//extern Bitvector * bitor PL_(( Bitvector *b1, Bitvector *b2 ));
//extern Bitvector * bitnot PL_(( Bitvector *b ));
//extern Bitvector * bitxor PL_(( Bitvector *b1, Bitvector *b2 ));
/*
extern int fdkey PL_(( int fd ));
extern int fdwait PL_(( int fd, int msec ));

extern void hexdump PL_(( FILE *file, void *memory, int l ));
extern void asciidump PL_(( FILE *file, char *memory, int l ));
extern int hex2int PL_(( char c ));
extern void read_asciidump PL_(( FILE *file, char *memory, int l ));
extern void read_hexdump PL_(( FILE *file, char *memory, int l ));

extern void banner PL_(( char *program, char *version ));

extern double htond PL_(( double val ));
extern double ntohd PL_(( double val ));
extern void htondp PL_(( double * valp_from, double * valp_to ));
extern void ntohdp PL_(( double * valp_from, double * valp_to ));
extern float htonf PL_(( float val ));
extern float ntohf PL_(( float val ));
extern void htonfp PL_(( float * valp_from, float * valp_to ));
extern void ntohfp PL_(( float * valp_from, float * valp_to ));
extern short int revshort PL_(( short int val ));
extern int revint PL_(( int val ));
extern int rev4 PL_(( unsigned char *from, unsigned char *to, int n ));
extern int rev8 PL_(( unsigned char *from, unsigned char *to, int n ));
extern int isnetorder PL_(( void ));

extern int gencompress PL_(( unsigned char **out, int *nout, int *size, int *in, int nsamp, int length ));
extern int genuncompress PL_(( int **out, int *nout, int *size, unsigned char *in, int nbytes ));

extern char * xlatnum PL_(( int def, Xlat *xlat, int nxlat ));
extern int xlatname PL_(( char *name, Xlat *xlat, int nxlat ));

extern int runcmd ( char *argv[], char **result );
extern int verbose_exists (void) ;

extern int set_random ( int intcount );
extern void fill_random ( char *packet, int nchar, int *pp );
extern int check_random ( char *packet, int nchar );

extern void ignoreSIGPIPE ( void );*/



/* When a macro generates multiple output statements, there is a danger
 * that these statements might lead to odd results in a context where a
 * single statement is expected, eg. after an if -- STMT forces the macro
 * output to be a single statement, and the following stuff with ZERO is
 * to make lint work properly. */

#ifdef lint
extern int      ZERO;
#else
#define ZERO 0
#endif

#define STMT(stuff) do { stuff } while (ZERO)

/* The following macros simplify memory allocation and testing
 *
 * allot and reallot are convenient interfaces to malloc and realloc which
 * perform testing on the result without cluttering the code with
 * branches and messages which are seldom if ever used.
 *
 * ALLOTERROR specifies what happens when a malloc or realloc fails -- it
 * may be overridden with a special macro within the file */

/*#ifdef __STDC__
#define ALLOTDIE(ptr,size)	printf("Out of memory at line %d in '%s'\n\tCan't malloc %d bytes for " #ptr "\n", __LINE__, __FILE__, size)
#else
#define ALLOTDIE(ptr,size)	printf("Out of memory at line %d in '%s'\n\tCan't malloc %d bytes for ptr\n", __LINE__, __FILE__, size)
#endif

#ifndef BADALLOT
#define BADALLOT -1
#endif

#ifdef __STDC__
#define ALLOTREGISTER_ERROR(ptr,size) \
    STMT(register_error(1,"Out of memory at line %d in '%s'\n\tCan't malloc %d bytes for " #ptr "\n", __LINE__, __FILE__, size); return BADALLOT ; )
#else
#define ALLOTREGISTER_ERROR(ptr,size) \
    STMT(register_error(1,"Out of memory at line %d in '%s'\n\tCan't malloc %d bytes for ptr\n", __LINE__, __FILE__, size); return BADALLOT; )
#endif

#ifdef __STDC__
#define ALLOTCOMPLAIN(ptr,size) \
    STMT(complain(1,"Out of memory at line %d in '%s'\n\tCan't malloc %d bytes for " #ptr "\n", __LINE__, __FILE__, size); return BADALLOT ; )
#else
#define ALLOTCOMPLAIN(ptr,size) \
    STMT(complain(1,"Out of memory at line %d in '%s'\n\tCan't malloc %d bytes for ptr\n", __LINE__, __FILE__, size); return BADALLOT; )
#endif
*/
//#ifndef ALLOTERROR
//#define ALLOTERROR ALLOTDIE
//#endif


/* allotted can be used in an if statement when the action in allot is
 * inadequate */
#define allotted(type,ptr,size) \
  ( (ptr=(type) malloc((unsigned)((size)*sizeof(*ptr)))) != NULL )

/* allot checks the results of malloc and generates an error message in
 * the case of failures */
/*#define allot(type,ptr,size)    \
  STMT( if (!allotted(type,ptr,size)) ALLOTERROR(ptr,size);)*/

/* reallot and reallotted correspond to allot and allotted and are
 * interfaces to realloc. */
#define reallotted(type,ptr,size) \
  ( (ptr=(type) realloc((char *)ptr,(unsigned)((size)*sizeof(*ptr)))) != NULL )
#define reallot(type,ptr,size)    \
  STMT( if (!reallotted(type,ptr,size)) ALLOTERROR(ptr,size);)


/* insist provides a concise method for consistency checks which are
 * never expect to fail, avoiding cluttering the code with branches which
 * are seldom if ever taken.
 * 
 * INSISTFAIL specifies what happens when an insist fails -- it may be
 * overridden with a special macro within a file */

#ifndef INSISTFAIL
#define INSISTFAIL die
#endif

//#define insist(ex)	STMT( if (!(ex)){ INSISTFAIL(1, "Unexpected failure: file %s, line %d\n", __FILE__, __LINE__ ) ;} )


/* The meaning of the following macros is fairly obvious; however, they
 * are all dangerous in the sense that their arguments will be evaluated
 * multiple times, which can lead to erroneous results. for example,
 * sqr(a++) will give an undefined result which is probably not what you
 * wanted and may differ among compilers. */

/* The following size is judged to be large enough to avoid problems with
 * overflowing string buffers, so that the various string routines which
 * do no checking can be used without much fear. */
#define STRSZ 1024


#endif


/* $Id: stock.h,v 1.1 2000/07/29 01:10:38 akimov Exp $ */
