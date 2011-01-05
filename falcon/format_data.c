#include <format_data.h>
#include <stdlib.h>

void format_data( const uint8_t *data, const size_t len, size_t bytes_per_line, size_t lines_per_section )
{
    if ( !data || !len ) {
        return;
    }

    if ( !bytes_per_line ) {
        bytes_per_line = 16; 
    }

    const uint8_t *pos = NULL;
    const uint8_t *pend = NULL;
    char *summary = NULL;
    size_t summary_count = 0;
    size_t section_lines = 0;
    size_t line_tally = 0;
    uint8_t value;
    int i;

    pos = data;
    pend = data + len;
    summary = calloc( 1, bytes_per_line + 1 );

    if ( summary ) {
        while (pos < pend) {
            fprintf( stdout, "%08lx ", (unsigned long)(line_tally * bytes_per_line) );
            /*
            fprintf( stdout, "%010lu ", line_tally * bytes_per_line );
            */
            line_tally++;
            for (i = 0; i < bytes_per_line; i++) {
                if ( pos < pend ) {
                    fprintf( stdout, " %02lx", (unsigned long)(*pos) );
                    value = *pos;
                    pos++;
                    if (value < 32 || value > 126 ) {
                        value = '.';
                    }

                    sprintf( (summary + summary_count), "%c", *(char *)&value );
                    summary_count++;
                } else {
                    fprintf( stdout, "   " );
                }
            }
            fprintf( stdout, "  |%s|\n", summary );
            summary_count = 0;
            section_lines++;
            if ( lines_per_section && section_lines >= lines_per_section ) {
                fprintf( stdout, "\n" );
                section_lines = 0;
            }
        }
        fprintf( stdout, "\n" );
        free(summary);
    }
}

