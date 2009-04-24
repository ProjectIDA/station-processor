/* -*- mode: C; c-basic-offset: 8; indent-tabs-mode: nil; tab-width: 8 -*- */

#include <config.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <pwd.h>
#include "url.h"
#include "http.h"
#include "options.h"
#include "util.h"
#include "llist.h"

extern int default_opts;

int redirect_count = 0;
#define REDIRECT_MAX 10

#define USER_AGENT "snarf/" VERSION " (http://www.xach.com/snarf)"
#define MOZILLA_USER_AGENT "Mozilla/4.0 (X11; Unix; Hi-mom)"
#define MSIE_USER_AGENT "Mozilla/4.0 (Compatible; MSIE 4.0)"

typedef struct _HttpHeader 	HttpHeader;
typedef struct _HttpHeaderEntry HttpHeaderEntry;

struct _HttpHeader {
        List *header_list;
};

struct _HttpHeaderEntry {
        char *key;
        char *value;
};


static HttpHeader *
make_http_header(char *r)
{
        HttpHeader *h		= NULL;
        HttpHeaderEntry	*he 	= NULL;
        char *s			= NULL;
        char *raw_header	= NULL;
        char *raw_header_head	= NULL;

        raw_header = safe_strdup(r);
        /* Track this to free at the end */
        raw_header_head = raw_header;
        
        h = (HttpHeader*)malloc(sizeof(HttpHeader));
        h->header_list = list_new();

        /* Skip the first line: "HTTP/1.X NNN Comment\r?\n" */
        s = raw_header;
        while (*s != '\0' && *s != '\r' && *s != '\n') {
                s++;
        }
        while (*s != '\0' && isspace(*s)) {
                s++;
        }

        raw_header = s;

        s = strstr(raw_header, ": ");
        while (s) {
                /* Set ':' to '\0' to terminate the key */
                *s++ = '\0'; 
                he = (HttpHeaderEntry*)malloc(sizeof(HttpHeaderEntry));
                he->key = safe_strdup(raw_header);
                /* Make it lowercase so we can lookup case-insensitive */
                string_lowercase(he->key);

                /* Now get the value */
                s++;
                raw_header = s;
                while (*s != '\0' && *s != '\r' && *s != '\n') {
                        s++;
                }
                *s++ = '\0';
                he->value = safe_strdup(raw_header);
                list_add(h->header_list, he);

                /* Go to the next line */
                while (*s != '\0' && isspace(*s)) {
                        s++;
                }
                raw_header = s;
                s = strstr(raw_header, ": ");
        }

        safe_free(raw_header_head);
        return h;
}


static void
free_http_header(HttpHeader *h)
{
        List *l;
        List *l1;
        HttpHeaderEntry *he;

        if (h == NULL) {
                return;
        }

        l = h->header_list;
        while (l && l->data) {
                he = l->data;
                safe_free(he->key);
                safe_free(he->value);
                safe_free(l->data);
                l = l->next;
        }

        l = h->header_list;
        while (l) {
                l1 = l->next;
                safe_free(l);
                l = l1;
        }

        safe_free(h);
}
                        

static char *
get_header_value(char *key, HttpHeader *header)
{
        List *l			 = NULL;
        HttpHeaderEntry *he	= NULL;

        l = header->header_list;

        while (l && l->data) {
                he = l->data;
                if (strcmp(he->key, key) == 0) {
                        return safe_strdup(he->value);
                }
                l = l->next;
        }

        return NULL;
}


static char *
get_raw_header(int fd)
{
        char *header = NULL;
        char *temp = NULL;
        char buf[BUFSIZE]; 	/* this whole function is pathetic. please
                                   rewrite it for me. */
        int bytes_read = 0;
        int total_read = 0;

        header = safe_strdup("");

        buf[0] = buf[1] = buf[2] = '\0';

        while( (bytes_read = read(fd, buf, 1)) ) {
                total_read += bytes_read;

                temp = header;
                header = strconcat(header, buf, NULL);
                safe_free(temp);
                if( total_read > 1) {
                        if( strcmp(header + (total_read - 2), "\n\n") == 0 )
                                break;
                }

                if( total_read > 3 ) {
                        if( strcmp(header + (total_read - 4), "\r\n\r\n") 
                            == 0 )
                                break;
                }
        }

        return header;
}

static char *
get_request(UrlResource *rsrc)
{
        char *request = NULL;
        char *auth = NULL;
        char *temp = NULL;
        char buf[BUFSIZE];
        Url *u;
        off_t file_size;

        u = rsrc->url;

        request = strconcat("GET ", u->path, u->file, " HTTP/1.0\r\n", 
                            "Host: ", u->host, "\r\n", NULL);

        if( u->username && u->password ) {
                auth = strconcat( u->username, ":", u->password, NULL);
                temp = auth;
                auth = base64(auth, strlen(auth));
                safe_free(temp);
                temp = request;
                request = strconcat(request, "Authorization: Basic ",
                                    auth, "\r\n", NULL);
                safe_free(temp);
                safe_free(auth);
        }

        if( rsrc->proxy_username && rsrc->proxy_password ) {
                auth = strconcat( rsrc->proxy_username, ":", 
                                  rsrc->proxy_password, NULL);
                temp = auth;
                auth = base64(auth, strlen(auth));
                safe_free(temp);
                temp = request;
                request = strconcat(request, "Proxy-Authorization: Basic ",
                                    auth, "\r\n", NULL);
                safe_free(temp);
                safe_free(auth);
        }

        if( (rsrc->options & OPT_RESUME) 
            && (file_size = get_file_size(rsrc->outfile)) ) {
                sprintf(buf, "%ld-", (long int )file_size);
                temp = request;
                request = strconcat(request, "Range: bytes=", buf, "\r\n",
                                    NULL);
                safe_free(temp);
        }

        /* Use user's SNARF_HTTP_USER_AGENT env. var if present,
           as they might want to spoof some discriminant sites.
           (Or just increase the hit count for their favorite
           browser.)  Alternately, use Mozilla or MSIE's User-Agent
           strings based on options set.  */

        temp = request;
        request = strconcat(request, "User-Agent: ", NULL);
        safe_free(temp);

        if (getenv("SNARF_HTTP_USER_AGENT")) {
            temp = request;
            request = strconcat(request, getenv("SNARF_HTTP_USER_AGENT"), 
                                NULL);
            safe_free(temp);
        } else if (rsrc->options & OPT_BE_MOZILLA) {
            temp = request;
            request = strconcat(request, MOZILLA_USER_AGENT, NULL);
            safe_free(temp);
        } else if (rsrc->options & OPT_BE_MSIE) {
            temp = request;
            request = strconcat(request, MSIE_USER_AGENT, NULL);
            safe_free(temp);
        } else {            /* let snarf be snarf :-) */
            temp = request;
            request = strconcat(request, USER_AGENT, NULL);
            safe_free(temp);
        }
        temp = request;
        /* This CRLF pair closes the User-Agent key-value set. */
        request = strconcat(request, "\r\n", NULL);
        safe_free(temp);

        /* If SNARF_HTTP_REFERER is set, spoof it. */
        if (getenv("SNARF_HTTP_REFERER")) {
                temp = request;
                request = strconcat(request, "Referer: ",
                                    getenv("SNARF_HTTP_REFERER"),
                                    "\r\n", NULL);
                safe_free(temp);
        }
        
        temp = request;
        request = strconcat(request, "\r\n", NULL);
        safe_free(temp);

        return request;
}


int
http_transfer(UrlResource *rsrc, buffer_t* usr_buf)
{
        FILE *out 		= NULL;
        Url *u			= NULL;
        Url *proxy_url		= NULL;
        Url *redir_u		= NULL;
        char *request		= NULL;
        char *raw_header	= NULL;
        HttpHeader *header	= NULL;
        char *len_string 	= NULL;
        char *new_location	= NULL;
        char *prompt = NULL;
        char *temp = NULL;
        char buf[BUFSIZE];
        int sock 		= 0;
        ssize_t bytes_read	= 0;
        int retval		= 0;
        int i;

        /* make sure we haven't recursed too much */

        if( redirect_count > REDIRECT_MAX ) {
                report(ERR, "redirection max count exceeded " 
                      "(looping redirect?)");
                redirect_count = 0;
                goto cleanup;
        }


        /* make sure everything's initialized to something useful */
        u = rsrc->url;
     
        if( ! *(u->host) ) {
                report(ERR, "no host specified");
                goto cleanup;
        }

        /* fill in proxyness */
        if( !rsrc->proxy ) {
                rsrc->proxy = get_proxy("HTTP_PROXY");
        }

        if( !rsrc->outfile ) {
                if( u->file ) 
                        rsrc->outfile = safe_strdup(u->file);
                else
                        rsrc->outfile = safe_strdup("index.html");
        }

        if( !u->path )
                u->path = safe_strdup("/");

        if( !u->file )
                u->file = safe_strdup("");  /* funny looking */

        if( !u->port )
                u->port = 80;

        rsrc->options |= default_opts;
                
        /* send the request to either the proxy or the remote host */
        if( rsrc->proxy ) {
                proxy_url = url_new();
                url_init(proxy_url, rsrc->proxy);
                
                if( !proxy_url->port ) 
                        proxy_url->port = 80;

                if( !proxy_url->host ) {
                        report(ERR, "bad proxy `%s'", rsrc->proxy);
                        goto cleanup;
                }

                if( proxy_url->username )
                        rsrc->proxy_username = safe_strdup(proxy_url->username);

                if( proxy_url->password )
                        rsrc->proxy_password = safe_strdup(proxy_url->password);

                /* Prompt for proxy password if not specified */
                if( proxy_url->username && !proxy_url->password ) {
                        prompt = strconcat("Password for proxy ",
                                           proxy_url->username, "@",
                                           proxy_url->host, ": ", NULL);
                        proxy_url->password = safe_strdup(getpass(prompt));
                        safe_free(prompt);
                }

                if( ! (sock = tcp_connect(proxy_url->host, proxy_url->port)) )
                        goto cleanup;


                u->path = safe_strdup("");
                u->file = safe_strdup(u->full_url);
                request = get_request(rsrc);

                write(sock, request, strlen(request));
                
                safe_free(request);

        } else /* no proxy */ {

                if( ! (sock = tcp_connect(u->host, u->port)) )
                        goto cleanup;

                request = get_request(rsrc);
                write(sock, request, strlen(request));

                safe_free(request);
        }

        if(!usr_buf && !(out = open_outfile(rsrc)) ) {
                report(ERR, "opening %s: %s", rsrc->outfile, strerror(errno));
                goto cleanup;
        }

        /* check to see if it returned a HTTP 1.x response */
        memset(buf, '\0', 5);

        bytes_read = read(sock, buf, 8);

        if( bytes_read == 0 ) {
                close(sock);
                goto cleanup;
        }

        if( ! (buf[0] == 'H' && buf[1] == 'T' 
               && buf[2] == 'T' && buf[3] == 'P') ) {
                if ((rsrc->options & OPT_RESUME) && 
                    rsrc->outfile_offset) {
                        report(WARN,
                               "server does not support resume, "
                               "try again"
                               " with -n (no resume)");
                        retval = 0;
                        goto cleanup;
                }
                write(fileno(out), buf, bytes_read);
        } else {
                /* skip the header */
                buf[bytes_read] = '\0';
                raw_header = get_raw_header(sock);
                temp = raw_header;
                raw_header = strconcat(buf, raw_header, NULL);
                safe_free(temp);
                header = make_http_header(raw_header);

                if (rsrc->options & OPT_VERBOSE) {
                        fwrite(raw_header, 1, strlen(raw_header), stderr);
                }


                /* check for redirects */
                new_location = get_header_value("location", header);

                if (raw_header[9] == '3' && new_location ) {
                        redir_u = url_new();
                        
                        /* make sure we still send user/password along */
                        redir_u->username = safe_strdup(u->username);
                        redir_u->password = safe_strdup(u->password);

                        url_init(redir_u, new_location);
                        rsrc->url = redir_u;
                        redirect_count++;
                        retval = transfer(rsrc);
                        goto cleanup;
                }

                if (raw_header[9] == '4' || raw_header[9] == '5') {
                        for(i = 0; raw_header[i] && raw_header[i] != '\n'; i++);
                        raw_header[i] = '\0';
                        report(ERR, "HTTP error from server: %s", raw_header);
                        retval = 0;
                        goto cleanup;
                }

                safe_free(raw_header);
                        
                len_string = get_header_value("content-length", header);

                if (len_string)
                        rsrc->outfile_size = (off_t )atoi(len_string);

                if (get_header_value("content-range", header))
                        rsrc->outfile_size += rsrc->outfile_offset;

                if( (!rsrc->outfile_size) && 
                    (rsrc->options & OPT_RESUME) && 
                    !(rsrc->options & OPT_NORESUME)
                    && rsrc->outfile_offset ) {
                        report(WARN,
                             "unable to determine remote file size, try again"
                             " with -n (no resume)");
                        retval = 0;
                        goto cleanup;
                }
        }

        if( ! dump_data(rsrc, sock, out, usr_buf) )
                retval = 0;
        else
                retval = 1;
                        
 cleanup:
        free_http_header(header);
        if (!usr_buf)
            fclose(out);
        close(sock); 
        return retval;

}

