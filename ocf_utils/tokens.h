#ifndef __TOKENS_H__
#define __TOKENS_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

#include <lib330.h>

#define BUFF_OWN  1
#define BUFF_COPY 2

typedef struct tk_link {
    struct tk_link *next;
    uint8_t token;
    uint8_t *buffer;
    size_t length;
    bool own_buffer;
} token_link;

typedef struct tm_link {
    struct tm_link *next;
    token_link *original;
    token_link *modified;
    bool own_links;
} token_mod_link;

typedef struct {
    bool started;
    bool complete;
    size_t segments;
    uint8_t *buffer;
    size_t buff_len;
    size_t buff_used;
    /* These are managed by the user to tweak functionality */
    bool summary_only; /* Print only intrinsic types */
} token_context;

/* Store the token data in a chain */
token_link* create_tk_link( uint8_t token, uint8_t* buffer, size_t length, bool copy_buffer );
void push_tk_link( token_link** chain, token_link* link );
token_link* pop_tk_link( token_link** chain );
void destroy_tk_link( token_link** link );
void destroy_tk_chain( token_link** chain );
void join_tk_chains( token_link** dst, token_link **src );
size_t count_tk_links( token_link *chain );
/* Populate a chain of tokens from a token context */
token_link* populate_token_chain( token_context *context );

/* These chains are intended to store modified tokens */
token_mod_link* create_tm_link( token_link* original, token_link* modified, bool own_links );
void push_tm_link( token_mod_link** chain, token_mod_link* link );
token_mod_link* pop_tm_link( token_mod_link** chain );
void destroy_tm_link( token_mod_link** link );
void destroy_tm_chain( token_mod_link** chain );
void join_tm_chains( token_mod_link** dst, token_mod_link **src );
size_t count_tm_links( token_mod_link *chain );
/* Populate a chain of token modifications based on two token chains */
token_mod_link* populate_token_mod_chain( token_link** original, token_link** modified );

/* Print a list of tokens for this context */
void parse_tokens ( token_context *context, bool print_raw, size_t depth );
int token_fixed ( uint8_t token, uint8_t *data, size_t max, bool print_raw, size_t depth );
int token_8 ( uint8_t token, uint8_t *data, size_t max, bool print_raw, size_t depth );
int token_16 ( uint8_t token, uint8_t *data, size_t max, bool print_raw, size_t depth );
void append_tokens( token_context *context, void *buff, size_t len, uint8_t flags );
bool tokens_complete( token_context *context );
void reset_tokens( token_context *context );
void dump_tokens( token_context *context );

#endif
