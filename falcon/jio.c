#include <jio.h>

void jprintf( size_t depth, const char *str, ... ) {
    va_list arglist;
    char *indent = NULL;
    size_t indent_spaces = 0;

    va_start(arglist, str);

    indent_spaces = depth * INDENT_WIDTH;
    indent = malloc( indent_spaces + 1 );
    if ( !indent )
        return;
    memset( indent, ' ', indent_spaces );
    indent[indent_spaces] = '\0';

    printf( "%s", indent );
    vprintf( str, arglist );
    free( indent );
}

