#include <tokens.h>
#include <record.h>
#include <jio.h>

token_link* create_tk_link( uint8_t token, uint8_t* buffer, size_t length, bool copy_buffer ) {
    token_link* link = NULL;
    uint8_t* buffer_ptr = NULL;

    if ( buffer ) {
        if ( copy_buffer ) {
            buffer_ptr = malloc( length );
            if ( buffer_ptr ) {
                memcpy( buffer_ptr, buffer, length );
            }
        } else {
            buffer_ptr = buffer;
        }
        if ( buffer_ptr ) {
            link = malloc( sizeof( token_link ) );
            if ( link ) {
                link->next = NULL;
                link->token = token;
                link->buffer = buffer_ptr;
                link->length = length;
                link->own_buffer = copy_buffer;
            }
        }
    }

    return link;
}

void push_tk_link( token_link** chain, token_link* link ) {
    if ( chain && link ) {
        if ( *chain ) {
            link->next = *chain;
        }
        *chain = link;
    } 
}

token_link* pop_tk_link( token_link** chain ) {
    token_link *link = NULL;
    if ( chain ) {
        if ( *chain ) {
            link = *chain;
            *chain = (*chain)->next;
            link->next = NULL;
        }
    }
    return link;
}

void destroy_tk_link( token_link** link ) {
    if ( link ) {
        if ( *link ) {
            if ( (*link)->own_buffer && (*link)->buffer ) {
                free( (*link)->buffer );
            }
            free( *link );
        }
        *link = NULL;
    }
}

void join_tk_chains( token_link** chain_a, token_link **chain_b ) {
    token_link* link = NULL;
    if ( chain_a && chain_b ) {
        if ( !(*chain_a) ) {
            *chain_a = *chain_b;
        } else {
            if ( *chain_b ) {
                link = *chain_a;
                while ( link->next ) {
                    link = link->next;
                }
                link->next = *chain_b;
            }
            *chain_b = *chain_a;
        }
    }
}

size_t count_tk_links( token_link *chain ) {
    size_t length = 0;
    while( chain ) {
        chain = chain->next;
        length++;
    }
    return length;
}

void destroy_tk_chain( token_link** chain ) {
    token_link *link = NULL;
    if ( chain ) {
        while ( link = pop_tk_link(chain) ) {
            destroy_tk_link( &link );
        }
        *chain = NULL;
    }
}

token_link* populate_token_chain( token_context *context ) {
    token_link* chain = NULL;
    token_link* link  = NULL;
    uint8_t *p;
    uint8_t *pend;
    uint8_t token;
    int len = 0;

    if ( !context ) {
        p = NULL;
        pend = NULL;
    } else {
        p = context->buffer;
        pend = p + context->buff_used;
    }

    while ( p < pend ) {
        token = *p;
        p += sizeof(token);
        if (token < 128) {
            switch( token ) {
                case TF_NOP:
                    p = pend;
                    break;
                case TF_VERSION:
                    if ( (byte)*p != T_VER )
                        p = pend;
                    else
                        len = 1;
                    break;
                case TF_NET_STAT:
                    len = 7;
                    break;
                case TF_NETSERV:
                    len = sizeof(unsigned short);
                    break;
                case TF_DSS:
                    len = 40;
                    break;
                case TF_WEB:
                    len = sizeof(unsigned short);
                    break;
                case TF_CLOCK:
                    len = 16;
                    break;
                case TF_MT:
                    len = 10;
                    break;
                case TF_CFG:
                    len = 8;
                    break;
                case TF_DATASERV:
                    len = sizeof(unsigned short);
                    break;
                default:
                    p = pend;
                    break;
            }
        } else if (token < 192) {
            len = (int)*(p);
        } else {
            len = (int)ntohs(*(uint16_t *)(p));
        }

        if ( len ) {
            link = create_tk_link( token, p, len, true );
            push_tk_link( &chain, link );
        }
        p += len;
    }
    return chain;
}


token_mod_link* create_tm_link( token_link* original, token_link* modified, bool own_links ) {
    token_mod_link* link = NULL;
    if ( original || modified ) {
        link = malloc( sizeof( token_mod_link ) );
        if ( link ) {
            link->next = NULL;
            link->original = original;
            link->modified = modified;
            link->own_links = own_links;
        }
    }
    return link;
}

void push_tm_link( token_mod_link** chain, token_mod_link* link ) {
    if ( chain && link ) {
        if ( *chain )
            link->next = (*chain);
        *chain = link;
    } 
}

token_mod_link* pop_tm_link( token_mod_link** chain ) {
    token_mod_link *link = NULL;
    if ( chain ) {
        if ( *chain ) {
            link = *chain;
            *chain = (*chain)->next;
            link->next = NULL;
        }
    }
    return link;
}

void destroy_tm_link( token_mod_link** link ) {
    if ( link ) {
        if ( *link ) {
            if ( (*link)->own_links ) {
                destroy_tk_link( &(*link)->original );
                destroy_tk_link( &(*link)->modified );
            }
            free( *link );
        }
        *link = NULL;
    }
}

size_t count_tm_links( token_mod_link *chain ) {
    size_t length = 0;
    while( chain ) {
        chain = chain->next;
        length++;
    }
    return length;
}

void destroy_tm_chain( token_mod_link** chain ) {
    token_mod_link *link = NULL;
    if ( chain ) {
        while ( link = pop_tm_link(chain) ) {
            destroy_tm_link( &link );
        }
        *chain = NULL;
    }
}

void join_tm_chains( token_mod_link** chain_a, token_mod_link **chain_b ) {
    token_mod_link* link = NULL;
    if ( chain_a && chain_b ) {
        if ( !(*chain_a) ) {
            *chain_a = *chain_b;
        } else {
            if ( *chain_b ) {
                link = *chain_a;
                while ( link->next ) {
                    link = link->next;
                }
                link->next = *chain_b;
            }
            *chain_b = *chain_a;
        }
    }
}

token_mod_link* populate_token_mod_chain( token_link** original, token_link** modified ) {
    token_mod_link* diff_list = NULL;
    token_mod_link* diff_pair = NULL;
    token_link* tk_original = NULL;
    token_link* tk_modified = NULL;
    token_link* save_original = NULL;
    token_link* save_modified = NULL;
    bool match_found = false;

    if ( original && modified ) {
        while ( tk_original = pop_tk_link( original ) ) {
            /*
            printf( "A=%d B=%d\n", 
                    count_tk_links(original),
                    count_tk_links(modified) );
            */
            match_found = false;
            save_modified = NULL;
            while ( !match_found && (tk_modified = pop_tk_link( modified )) ) {
                if ( tk_original->token == tk_modified->token ) {
                    if ( (tk_original->length != tk_modified->length ) ||
                         memcmp(tk_original->buffer, tk_modified->buffer, 
                                tk_original->length) ) {
                        switch ( tk_original->token ) {
                            case TF_NOP:
                            case TF_VERSION:
                            case TF_NET_STAT:
                            case TF_NETSERV:
                            case TF_DSS:
                            case TF_WEB:
                            case TF_CLOCK:
                            case TF_MT:
                            case TF_CFG:
                            case TF_DATASERV:
                            case T2_CNAMES:
                                match_found = true;
                                diff_pair = create_tm_link( tk_original, 
                                                            tk_modified, 
                                                            true );
                                push_tm_link( &diff_list, diff_pair );
                                break;
                            default:
                                push_tk_link( &save_modified, tk_modified );
                                break;
                        }
                    }
                    else {
                        match_found = true;
                        destroy_tk_link( &tk_original );
                        destroy_tk_link( &tk_modified );
                    }
                } else {
                    push_tk_link( &save_modified, tk_modified );
                }
            }
            if ( !match_found ) {
                push_tk_link( &save_original, tk_original );
            }
            join_tk_chains( modified, &save_modified );
        }
        while ( tk_modified = pop_tk_link( &save_modified ) ) {
            diff_pair = create_tm_link( NULL, tk_modified, true );
            push_tm_link( &diff_list, diff_pair );
        }
        while ( tk_original = pop_tk_link( &save_original ) ) {
            diff_pair = create_tm_link( tk_original, NULL, true );
            push_tm_link( &diff_list, diff_pair );
        }
    }

    return diff_list;
}


void parse_tokens ( token_context *context, bool print_raw, size_t depth ) {
    uint8_t *p;
    uint8_t *pend;
    uint8_t tok;
    int len = 0;

    p = context->buffer;
    pend = p + context->buff_used;
    while ( p < pend ) {
        tok = *p;
        if (tok < 128) {
            len = token_fixed( tok, p + sizeof(tok), 
                               pend - p - sizeof(tok), print_raw, depth );
            if ( !len ) {
                jprintf( depth, "Encountered an error while parsing tokens\n" );
                p = pend;
            }
        } else if (tok < 192) {
            len = token_8( tok, p + sizeof(tok), 
                           pend - p - sizeof(tok), print_raw, depth );
        } else {
            len = token_16( tok, p + sizeof(tok), 
                            pend - p - sizeof(tok), print_raw, depth );
        }
        p += len + sizeof(tok);
    }
}

int token_fixed ( uint8_t token, uint8_t *data, size_t max, bool print_raw, size_t depth ) {
    int len = 0;
    uint8_t *p = NULL;
    char password[9];

    p = data;

    switch (token) {
        case TF_NOP:
            jprintf( depth, "No more data\n" );
            break;
        case TF_VERSION:
            if ( (byte)*p != T_VER ) {
                jprintf( depth, "Invalid token version %d\n", (unsigned short)*p );
            } else {
                jprintf( depth, "Token version is %d\n", (unsigned short)*p) ;
                len = 1;
            }
            break;
        case TF_NET_STAT:
            len = 7;
            jprintf( depth, "Token: NET_STAT\n" );
            jprintf( depth + 1, "Network = %c%c\n", 
                     *(char *)p, 
                     *(char *)(p + 1) );
            p += 2;
            jprintf( depth + 1, "Station = %s\n", (char *)p );
            break;
        case TF_NETSERV:
            len = sizeof(unsigned short);
            jprintf( depth, "Token: NETSERV\n" );
            jprintf( depth + 1, "Netserv Port = %d\n", ntohs(*(unsigned short *)p) );
            break;
        case TF_DSS:
            len = 40;
            jprintf( depth, "Token: DSS\n" );
            *(uint64_t *)password = *(uint64_t *)p; p += 8; password[8] = '\0';
            jprintf( depth + 1, "High Priority Password = %s\n", 
                     password );
            *(uint64_t *)password = *(uint64_t *)p; p += 8; password[8] = '\0';
            jprintf( depth + 1, "Mid Priority Password = %s\n", 
                     password );
            *(uint64_t *)password = *(uint64_t *)p; p += 8; password[8] = '\0';
            jprintf( depth + 1, "Low Priority Password = %s\n", 
                     password );
            jprintf( depth + 1, "Timeout = %d\n",
                     ntohl(*(long *)p) ); p += 4;
            jprintf( depth + 1, "Max BPS = %d\n",
                     ntohl(*(long *)p) ); p += 4;
            jprintf( depth + 1, "Verbosity = %d\n",
                     (unsigned short)*p ); p += 1;
            jprintf( depth + 1, "Max CPU Percentage = %d\n",
                     (unsigned short)*p ); p += 1;
            jprintf( depth + 1, "Port Number = %d\n",
                     ntohs(*(unsigned short *)p) ); p += 2;
            jprintf( depth + 1, "Max Memory = %d kB\n",
                     ntohs(*(unsigned short *)p) ); p += 2;
            jprintf( depth + 1, "Reserved = %d\n",
                     ntohs(*(unsigned short *)p) ); p += 2;
            break;
        case TF_WEB:
            len = sizeof(unsigned short);
            jprintf( depth, "Token: WEB\n" );
            jprintf( depth + 1, "Web Server Port = %d\n", ntohs(*(unsigned short *)p) );
            break;
        case TF_CLOCK:
            len = 16;
            jprintf( depth, "Token: CLOCK\n" );
            jprintf( depth + 1, "Zone is UTC %+d seconds\n", 
                     ntohl(*(long *)p) ); p += 4;
            jprintf( depth + 1, "Degrade 1%% once off by %d minutes\n", 
                     ntohs(*(unsigned short *)p) ); p += 1;
            jprintf( depth + 1, "PLL Locked Quality = %d\n", 
                     (unsigned short)*p ); p += 1;
            jprintf( depth + 1, "PLL Tracking Quality = %d\n", 
                     (unsigned short)*p ); p += 1;
            jprintf( depth + 1, "PLL Holding Quality = %d\n", 
                     (unsigned short)*p ); p += 1;
            jprintf( depth + 1, "Currently Locked (PLL Off) = %d\n", 
                     (unsigned short)*p ); p += 1;
            jprintf( depth + 1, "Has been locked Highest Quality = %d\n", 
                     (unsigned short)*p ); p += 1;
            jprintf( depth + 1, "Has been locked Lowest Quality = %d\n", 
                     (unsigned short)*p ); p += 1;
            jprintf( depth + 1, "Never been locked Quality = %d\n", 
                     (unsigned short)*p ); p += 1;
            jprintf( depth + 1, "Currently Locked = %d\n", 
                     (unsigned short)*p ); p += 1;
            jprintf( depth + 1, "Min. between clock messages = %d\n", 
                     ntohs(*(unsigned short *)p) ); p += 1;
            break;
        case TF_MT:
            len = 10;
            jprintf( depth, "Token: MT\n" );
            jprintf( depth + 1, "Log = %c%c_%c%c%c\n",
                    *(char *)(p), 
                    *(char *)(p + 1),
                    *(char *)(p + 2), 
                    *(char *)(p + 3), 
                    *(char *)(p + 4) );
            p += 5;
            jprintf( depth + 1, "Timing = %c%c_%c%c%c\n",
                    *(char *)(p), 
                    *(char *)(p + 1),
                    *(char *)(p + 2), 
                    *(char *)(p + 3), 
                    *(char *)(p + 4) );
            break;
        case TF_CFG:
            len = 8;
            jprintf( depth, "Token: CFG\n" );
            jprintf( depth + 1, "Config = %c%c_%c%c%c\n",
                    *(char *)(p), 
                    *(char *)(p + 1),
                    *(char *)(p + 2), 
                    *(char *)(p + 3), 
                    *(char *)(p + 4) );
            p += 5;
            jprintf( depth + 1, "Write Configuration:\n" );
            jprintf( depth + 2, "At session start [%s]\n", 
                     *p & 0x01 ? "YES" : "NO" );
            jprintf( depth + 2, "At session end   [%s]\n", 
                     *p & 0x02 ? "YES" : "NO" );
            jprintf( depth + 2, "At set interval  [%s]", 
                     *p & 0x04 ? "YES" : "NO" );
            if ( *p & 0x04 )
                printf( " (every %d minutes)\n", 
                         ntohs(*(unsigned short *)(p + 1)) );
            else
                printf( "\n" );
                    break;
        case TF_DATASERV:
            len = sizeof(unsigned short);
            jprintf( depth, "Token: DATASERV\n" );
            jprintf( depth + 1, "Data Server Port = %d\n", ntohs(*(unsigned short *)p) );
            break;
        default:
            jprintf( depth, "Token: UNKNOWN [%d]\n", len );
            break;
    }
    if ( print_raw && len ) {
        dump_hex( data, len, (size_t)DUMP_BYTES, (size_t)16 );
    }
    return len;
}

int token_8 ( uint8_t token, uint8_t *data, size_t max, bool print_raw, size_t depth ) {
    unsigned short len = (unsigned short)*data;
    uint8_t *p = NULL;

    p = data;

    switch (token) {
        case T1_LCQ:
            jprintf( depth, "Token: LCQ [%d]\n", len );
            break;
        case T1_IIR:
            jprintf( depth, "Token: IIR [%d]\n", len );
            break;
        case T1_FIR:
            jprintf( depth, "Token: FIR [%d]\n", len );
            break;
        case T1_CTRL:
            jprintf( depth, "Token: CTRL [%d]\n", len );
            break;
        case T1_MHD:
            jprintf( depth, "Token: MHD (Murdock-Hutt Detector) [%d]\n", len );
            break;
        case T1_THRD:
            jprintf( depth, "Token: THRD (Threshold Detector) [%d]\n", len );
            break;
        case T1_NONCOMP:
            jprintf( depth, "Token: NONCOMP [%d]\n", len );
            break;
        case T1_DPLCQ:
            jprintf( depth, "Token: DPLCQ [%d]\n", len );
            break;
        default:
            jprintf( depth, "Token: UNKNOWN [%d]\n", len );
            break;
    }
    if ( print_raw && len ) {
        dump_hex( data, len, (size_t)DUMP_BYTES, (size_t)16 );
    }
    return len;
}

int token_16 ( uint8_t token, uint8_t *data, size_t max, bool print_raw, size_t depth ) {
    unsigned short len = ntohs(*(unsigned short *)data);
    uint8_t *p = NULL;

    p = data;

    switch (token) {
        case T2_CNAMES:
            jprintf( depth, "Token: CNAMES [%d]\n", len );
            break;
        case T2_ALERT:
            jprintf( depth, "Token: ALERT [%d]\n", len );
            break;
        case T2_OPAQUE:
            jprintf( depth, "Token: OPAQUE [%d]\n", len );
            break;
        default:
            jprintf( depth, "Token: UNKNOWN [%d]\n", len );
            break;
    }
    if ( print_raw && len ) {
        dump_hex( data, len, (size_t)DUMP_BYTES, (size_t)16 );
    }
    return len;
}

void append_tokens( token_context *context, void *buff, size_t len, uint8_t flags ) {
    uint8_t *buff_pickup = NULL;
    uint8_t *buff_new = NULL;
    size_t   new_len  = 0;
    if ( !context ) return;
    if ( !(flags & 0x04) && !(flags & 0x08) ) return;

    if ( (flags & 0x04) && !(flags & 0x08) ) {
        reset_tokens( context );
        context->started = true;
    }
    else if ( !(flags & 0x04) && (flags & 0x08) ) {
        context->complete = true;
    }

    if ( context->buffer ) {
        new_len = context->buff_used + len;
        if ( new_len > context->buff_len )
            buff_new = realloc( context->buffer, new_len );
        if ( buff_new ) {
            context->buffer = buff_new;
            context->buff_len = new_len;
        }
    }
    else {
        buff_new = malloc( len );
        if ( buff_new ) {
            context->buffer = buff_new;
            context->buff_len = len;
        }
    }
    if ( context->buffer ) {
        buff_pickup = context->buffer + context->buff_used;
        memcpy( buff_pickup, buff, len );
        context->buff_used += len;
        context->segments++;
    }

}

bool tokens_complete( token_context *context ) {
    bool completed = false;
    if ( context && context->complete )
        completed = true;
    return completed;
}

void reset_tokens( token_context *context ) {
    if ( context ) {
        context->started   = false;
        context->complete  = false;
        context->segments  = 0;
        if ( context->buffer )
            free( context->buffer );
        context->buffer    = NULL;
        context->buff_len  = 0;
        context->buff_used = 0;

        /* The following should be set by the user
         *   context->summary_only
         */
    }
}

void destroy_token_context( token_context **context_ptr ) {
    if ( context_ptr ) {
        if ( *context_ptr ) {
            if ( (*context_ptr)->buffer ) {
                free( (*context_ptr)->buffer );
            }
            free( *context_ptr );
        }
        *context_ptr = NULL;
    }
}

void dump_tokens( token_context *context ) {
    if ( !context ) return;
    dump_hex( context->buffer, context->buff_used,
              (size_t)DUMP_BYTES, (size_t)16 );
}

