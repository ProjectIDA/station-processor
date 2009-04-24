#include <get.h>
#include <http.h>
#include <options.h>
#include <url.h>
#include <util.h>

void get_page( const char* url_str, buffer_t* buf )
{
    UrlResource* rsc;

    rsc = (UrlResource*)url_resource_new();

    rsc->url = (Url*)url_new();
    url_init(rsc->url, url_str);
    rsc->options |= OPT_NORESUME | OPT_QUIET;

    http_transfer(rsc, buf);
    url_resource_destroy(rsc);
    rsc = NULL;
    buffer_seek_end(buf);
    buffer_terminate(buf);
}

