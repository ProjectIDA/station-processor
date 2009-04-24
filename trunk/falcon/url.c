/* -*- mode: C; c-basic-offset: 8; indent-tabs-mode: nil; tab-width: 8 -*- */

#include "config.h"

#include <stdlib.h>
#include <string.h>
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#include <ctype.h>
#include "url.h"
#include "util.h"

#include <mem_profiler.h>

/*
void
safe_free(void *ptr)
{
    if( ptr ) {
        debug("safe_free [%p]", ptr);
        free(ptr);
        ptr = NULL;
    }
}
*/

int
is_probably_an_url(char *string)
{
    if( strstr(string, "://") )
        return 1;
    return 0;
}

const char *
get_service_type(const char *string, Url *u)
{
    /* fixme: what if the string isn't at the beginning of the 
       *string? */
    
    if( strstr(string, "http://") ) {
        string += 7;		/* skip past that part */
        u->service_type = SERVICE_HTTP;
        return string;
    }

    if( strstr(string, "ftp://") ) {
        string += 6;
        u->service_type = SERVICE_FTP;
        return string;
    }

    if( strstr(string, "gopher://") ) {
        string += 9;
        u->service_type = SERVICE_GOPHER;
        return string;
    }

    if( strncasecmp(string, "www", 3) == 0 ) {
        u->service_type = SERVICE_HTTP;
        u->full_url = strconcat("http://", u->full_url, NULL);
        return string;
    }

    if( strncasecmp(string, "ftp", 3) == 0 ) {
        u->service_type = SERVICE_FTP;
        u->full_url = strconcat("ftp://", u->full_url, NULL);
        return string;
    }

    /* default to browser-style serviceless http URL */
    u->full_url = strconcat("http://", u->full_url, NULL);
    u->service_type = SERVICE_HTTP;
    return string;
}


const char *
get_username(const char *string, Url *u)
{
    int i;
    char *username;
    char *at;
    char *slash;
    
    at = strchr(string, '@');
    slash = strchr(string, '/');

    if( (!at) || (slash && (at >= slash)) )
        return string;
    
    for( i = 0; string[i] && string[i] != ':' &&
         string[i] != '@' && string[i] != '/'; i++);

    if( string[i] != '@' && string[i] != ':' ) {
        return string;
    }
 
    username = (char*)malloc(i);
    memcpy(username, string, i + 1);

    username[i] = '\0';

    string += i + 1;

    u->username = username;
    return string;
}


const char *
get_password(const char *string, Url *u)
{
    int i;
    char *password;
    char *at;
    char *slash;
    
    at = strchr(string, '@');
    slash = strchr(string, '/');

    if( (!at) || (slash && (at >= slash)) )
        return string;

    /* skipping to the end of the host portion.
       this is kinda messy for the (rare) cases where someone
       has a slash and/or at in their password. It's not perfect;
       but it catches easy cases. 
       
       If you know of a better way to do this, be my guest. I do not
       feel a particular paternal instinct towards my ugly code.

       I guess that applies to this whole program.
    */

    for(i = 0 ; string[i] != '@'; i++);
    
    password = (char*)malloc(i);

    /* and finally, get the password portion */
    memcpy(password, string, i);
    password[i] = '\0';

    string += i + 1;

    u->password = password;
    
    return string;
}


const char *
get_hostname(const char *url, Url *u)
{
    char *hostname;
    int i;

    /* skip to end, slash, or port colon */
    for( i = 0; url[i] && url[i] != '/' && url[i] != ':'; i++ );

    hostname = (char*)malloc(i + 1);

    memcpy(hostname, url, i);

    hostname[i] = '\0';

    /* if there's a port */
    if(url[i] == ':')
        url += i + 1;
    else
        url += i;

    u->host = hostname;
    return url;
}

const char *
get_port(const char *url, Url *u)
{
    char *port_string;
    int i;

    for(i = 0; isdigit(url[i]); i++);

    if(i == 0)
        return url;

    port_string = (char*)malloc(i + 1);
    memcpy(port_string, url, i + 1);

    port_string[i] = '\0';

    url += i;
    
    u->port = atoi(port_string);
    safe_free(port_string);

    return url;
}


const char *
get_path(const char *url, Url *u)
{
    int i;
    char *path;

    /* find where the last slash is */
    for(i = strlen(url); i > 0 && url[i] != '/'; i--);

    if(url[i] != '/')
        return url;

    path = (char*)malloc(i + 2);
    memcpy(path, url, i + 1);
    path[i] = '/';
    path[i + 1] = '\0';

    url += i + 1;
    u->path = path;

    return url;
}


const char *
get_file(const char *string, Url *u)
{
  char *file;

  if( !string[0] ) 
    return NULL;

  file = (char*)malloc(strlen(string) + 1);

  memcpy(file, string, strlen(string) + 1);

  u->file = file;

  return string;
}


Url *
url_new(void)
{
  Url *new_url;

  new_url = (Url*)malloc(sizeof(Url));

  new_url->full_url	= NULL;
  new_url->service_type 	= 0;
  new_url->username 	= NULL;
  new_url->password 	= NULL;
  new_url->host 		= NULL;
  new_url->port 		= 0;
  new_url->path 		= NULL;
  new_url->file		= NULL;

  return new_url;
}


void 
url_destroy(Url *u)
{
    if( u ) {
        safe_free(u->full_url);
        safe_free(u->username);
        safe_free(u->password);
        safe_free(u->host);
        safe_free(u->path);
        safe_free(u->file);
        memset(u, 0, sizeof(Url));
        safe_free(u);
    }
}
        

UrlResource *
url_resource_new(void)
{
    UrlResource *new_resource;
    
    new_resource = (UrlResource*)malloc(sizeof(UrlResource));
    
    new_resource->url 		= NULL;
    new_resource->outfile		= NULL;
    new_resource->proxy		= NULL;
    new_resource->proxy_username	= NULL;
    new_resource->proxy_password	= NULL;
    new_resource->options		= 0;
    new_resource->outfile_size	= 0;
    new_resource->outfile_offset	= 0;

    return new_resource;
}


void 
url_resource_destroy(UrlResource *rsrc)
{
    if (rsrc) {
        if (rsrc->url) {
            url_destroy(rsrc->url);
            rsrc->url = NULL;
        }
        safe_free(rsrc->outfile);
        safe_free(rsrc);
    }
}


Url *
url_init(Url *u, const char *string)
{
	const char *sp;	/* since we're going to walk through string,
			               use a copy instead. */

  sp = string;

  u->full_url = (char *)safe_strdup(string);

	if( ! (sp = get_service_type(sp, u)) )
		return 0;

    /* only get username/password if they are not null,
       allows us to handle redirects properly */

    if( !u->username ) 
            sp = get_username(sp, u);
    if( !u->password )
            sp = get_password(sp, u);

    sp = get_hostname(sp, u);

    if( ! (u->host && *(u->host)) )
            return NULL;

    sp = get_port(sp, u);

    sp = get_path(sp, u);
    sp = get_file(sp, u);

	return u;
}




		

