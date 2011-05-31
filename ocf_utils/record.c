#include <lib330.h>

#include <simclist.h>
#include <record.h>
#include <jio.h>
#include <dump_hex.h>

int metadata_compare( const void *data1, const void *data2 ) {
    return cmp_times( &((list_metadata*)data1)->time, &((list_metadata*)data2)->time );
}

int cmp_times( tseed_time* time1, tseed_time* time2 ) {
    int result = 0;

    /*
     * It appears that some times are stored as floating-point
     * rather than the separated values
     */

    if ( time1->STUNION.STGREG.yr < time2->STUNION.STGREG.yr ) {
        result = -1;
    } else if ( time1->STUNION.STGREG.yr > time2->STUNION.STGREG.yr ) {
        result = 1;
    } else if ( time1->STUNION.STGREG.jday < time2->STUNION.STGREG.jday ) {
        result = -1;
    } else if ( time1->STUNION.STGREG.jday > time2->STUNION.STGREG.jday ) {
        result = 1;
    } else if ( time1->STUNION.STGREG.hr < time2->STUNION.STGREG.hr ) {
        result = -1;
    } else if ( time1->STUNION.STGREG.hr > time2->STUNION.STGREG.hr ) {
        result = 1;
    } else if ( time1->STUNION.STGREG.minute < time2->STUNION.STGREG.minute ) {
        result = -1;
    } else if ( time1->STUNION.STGREG.minute > time2->STUNION.STGREG.minute ) {
        result = 1;
    } else if ( time1->STUNION.STGREG.seconds < time2->STUNION.STGREG.seconds ) {
        result = -1;
    } else if ( time1->STUNION.STGREG.seconds > time2->STUNION.STGREG.seconds ) {
        result = 1;
    } else {
        result = 0;
    }

    return result;
}

void diff_list_global( list_t* list, bool print_diff ) {
    list_metadata* data_original = NULL;
    list_metadata* data_updated = NULL;

    list_iterator_stop( list );
    list_iterator_start( list );
    while ( list_iterator_hasnext( list ) ) {
        data_updated = (list_metadata*)list_iterator_next( list );
        if ( data_original && data_updated ) {
            print_seed_time( "Differences from Global records dated ", 
                             &data_original->time, "");
            print_seed_time( " and ", &data_updated->time, " :\n");
            if ( diff_global( data_original->data, 
                              data_updated->data, print_diff ) ) {
                if ( !print_diff ) {
                    printf( "  Found modified records\n" );
                }
            } else {
                printf( "  NONE\n" );
            }
        }
        data_original = data_updated;
    }
    list_iterator_stop( list );
}

void diff_list_fixed( list_t* list, bool print_diff ) {
    list_metadata* data_original = NULL;
    list_metadata* data_updated = NULL;

    list_iterator_stop( list );
    list_iterator_start( list );
    while ( list_iterator_hasnext( list ) ) {
        data_updated = (list_metadata*)list_iterator_next( list );
        if ( data_original && data_updated ) {
            print_seed_time( "Differences from Fixed records dated ", 
                             &data_original->time, "");
            print_seed_time( " and ", &data_updated->time, " :\n");
            if ( diff_fixed( data_original->data, 
                             data_updated->data, print_diff ) ) {
                if ( !print_diff ) {
                    printf( "  Found modified records\n" );
                }
            } else {
                printf( "  NONE\n" );
            }
        }
        data_original = data_updated;
    }
    list_iterator_stop( list );
}

void diff_list_log( list_t* list, bool print_diff ) {
    list_metadata* data_original = NULL;
    list_metadata* data_updated = NULL;

    list_iterator_stop( list );
    list_iterator_start( list );
    while ( list_iterator_hasnext( list ) ) {
        data_updated = (list_metadata*)list_iterator_next( list );
        if ( data_original && data_updated ) {
            print_seed_time( "Differences from Log records dated ", 
                             &data_original->time, "");
            print_seed_time( " and ", &data_updated->time, " :\n");
            if ( diff_log( data_original->data, 
                           data_updated->data, print_diff ) ) {
                if ( !print_diff ) {
                    printf( "  Found modified records\n" );
                }
            } else {
                printf( "  NONE\n" );
            }
        }
        data_original = data_updated;
    }
    list_iterator_stop( list );
}

void diff_list_tokens( list_t* list, bool print_diff ) {
    int diff_count = 0;

    list_metadata* data_original = NULL;
    list_metadata* data_updated = NULL;

    token_context* context_tokens_original;
    token_context* context_tokens_updated;

    token_link* tk_link_original;
    token_link* tk_link_updated;

    token_mod_link* token_differences;
    token_mod_link* diff_pair;

    list_iterator_stop( list );
    list_iterator_start( list );
    while ( list_iterator_hasnext( list ) ) {
        data_updated = (list_metadata*)list_iterator_next( list );
        if ( data_original && data_updated ) {
            tk_link_original = populate_token_chain( (token_context*)data_original->data );
            tk_link_updated  = populate_token_chain( (token_context*)data_updated->data );

            token_differences = populate_token_mod_chain( &tk_link_original, 
                                                          &tk_link_updated );

            print_seed_time( "Differences from Token records dated ", 
                             &data_original->time, "");
            print_seed_time( " and ", &data_updated->time, " :\n");
            while( (diff_pair = pop_tm_link( &token_differences )) != NULL ) {
                diff_count++;
                if ( print_diff ) {
                    printf( "  Difference found on token [%d]\n",
                            diff_pair->original ?
                            (unsigned short)diff_pair->original->token :
                            diff_pair->modified ?
                            (unsigned short)diff_pair->modified->token :
                            (unsigned short)-1 );
                    printf( "    Original:\n" );
                    if ( diff_pair->original ) {
                        dump_hex( diff_pair->original->buffer,
                                  diff_pair->original->length,
                                  (size_t)DUMP_BYTES, 
                                  (size_t)16 );
                    } else {
                        printf( "      NULL\n" ); 
                    }
                    printf( "    Modified:\n" );
                    if ( diff_pair->modified ) {
                        dump_hex( diff_pair->modified->buffer,
                                  diff_pair->modified->length,
                                  (size_t)DUMP_BYTES, 
                                  (size_t)16 );
                    } else {
                        printf( "      NULL\n" ); 
                    }
                }
            }
            if ( !diff_count ) {
                printf( "  NONE\n" );
            }
            else if ( diff_count && !print_diff ) {
                printf( "  Found %d modified tokens\n", diff_count );
            }

            destroy_tm_chain( &token_differences );
            destroy_tk_chain( &tk_link_original );
            destroy_tk_chain( &tk_link_updated );
            tk_link_original = populate_token_chain( context_tokens_updated );
            tk_link_updated = NULL;
        }
        data_original = data_updated;
    }
    list_iterator_stop( list );
}

int diff_global( tglobal* original, tglobal* updated, bool print_diff ) {
    int result = 0;
    int depth  = 1;
    if ( print_diff ) {
        if ( original->clock_to != updated->clock_to ) {
            jprintf( depth, "Clock Timeout: [%lu] ===> [%lu]\n",
                     (unsigned long)original->clock_to,
                     (unsigned long)updated->clock_to );
            result = 1;
        }
        if ( original->initial_vco != updated->initial_vco ) {
            jprintf( depth, "Initial VCO Value: [%lu] ===> [%lu]\n",
                     (unsigned long)original->initial_vco,
                     (unsigned long)updated->initial_vco );
            result = 1;
        }
        if ( original->gps_backup != updated->gps_backup ) {
            jprintf( depth, "Initial VCO Value: [%s] ===> [%s]\n",
                     (unsigned short)original->gps_backup ? "ON": "OFF",
                     (unsigned short)updated->gps_backup ? "ON": "OFF" );
            result = 1;
        }
        if ( original->samp_rates != updated->samp_rates ) {
            jprintf( depth, "Sample Rates: [%lu] ===> [%lu]\n",
                     (unsigned long)original->samp_rates,
                     (unsigned long)updated->samp_rates );
            result = 1;
        }
        if ( original->gain_map != updated->gain_map ) {
            jprintf( depth, "Gain Bitmap: [0x%04lx] ===> [0x%04lx]\n",
                     (unsigned long)original->gain_map,
                     (unsigned long)updated->gain_map );
            result = 1;
        }
        if ( original->filter_map != updated->filter_map ) {
            jprintf( depth, "Filter Bitmap: [0x%04lx] ===> [0x%04lx]\n",
                     (unsigned long)original->filter_map,
                     (unsigned long)updated->filter_map );
            result = 1;
        }
        if ( original->input_map != updated->input_map ) {
            jprintf( depth, "Input Bitmap: [0x%04lx] ===> [0x%04lx]\n",
                     (unsigned long)original->input_map,
                     (unsigned long)updated->input_map );
            result = 1;
        }
        if ( original->web_port != updated->web_port ) {
            jprintf( depth, "Web Server Port: [%lu] ===> [%lu]\n",
                     (unsigned long)original->web_port,
                     (unsigned long)updated->web_port );
            result = 1;
        }
        if ( original->server_to != updated->server_to ) {
            jprintf( depth, "Server Timeout: [%lu] ===> [%lu]\n",
                     (unsigned long)original->server_to,
                     (unsigned long)updated->server_to );
            result = 1;
        }
        if ( original->drift_tol != updated->drift_tol ) {
            jprintf( depth, "Drift Tolerance: [%lu] ===> [%lu]\n",
                     (unsigned long)original->drift_tol,
                     (unsigned long)updated->drift_tol );
            result = 1;
        }
        if ( original->jump_filt != updated->jump_filt ) {
            jprintf( depth, "Jump Filter: [%lu] ===> [%lu]\n",
                     (unsigned long)original->jump_filt,
                     (unsigned long)updated->jump_filt );
            result = 1;
        }
        if ( original->jump_thresh != updated->jump_thresh ) {
            jprintf( depth, "Jump Threshold: [%lu] ===> [%lu]\n",
                     (unsigned long)original->jump_thresh,
                     (unsigned long)updated->jump_thresh );
            result = 1;
        }
        if ( original->cal_offset != updated->cal_offset ) {
            jprintf( depth, "Cal. D/A Offset: [%d] ===> [%d]\n",
                     (unsigned long)original->cal_offset,
                     (unsigned long)updated->cal_offset );
            result = 1;
        }
        if ( original->sensor_map != updated->sensor_map ) {
            jprintf( depth, "Sensor Control Bitmal: [0x%04lx] ===> [0x%04lx]\n",
                     (unsigned long)original->sensor_map,
                     (unsigned long)updated->sensor_map );
            result = 1;
        }
        if ( original->sampling_phase != updated->sampling_phase ) {
            jprintf( depth, "Sampling Phase: [%lu] ===> [%lu]\n",
                     (unsigned long)original->sampling_phase,
                     (unsigned long)updated->sampling_phase );
            result = 1;
        }
        if ( original->gps_cold != updated->gps_cold ) {
            jprintf( depth, "GPS Coldstart: [%lu] ===> [%lu]\n",
                     (unsigned long)original->gps_cold,
                     (unsigned long)updated->gps_cold );
            result = 1;
        }
        if ( original->user_tag != updated->user_tag ) {
            jprintf( depth, "User Tag: [%lu] ===> [%lu]\n",
                     (unsigned long)original->user_tag,
                     (unsigned long)updated->user_tag );
            result = 1;
        }
        if ( memcmp( original->scaling, updated->scaling,
                     sizeof(original->scaling) ) ) {
            jprintf( depth, "Scaling frequencies have changed.\n" );
            result = 1;
        }
        if ( memcmp( original->offsets, updated->offsets,
                     sizeof(original->offsets) ) ) {
            jprintf( depth, "Channel offsets have changed.\n" );
            result = 1;
        }
        if ( memcmp( original->gains, updated->gains,
                     sizeof(original->gains) ) ) {
            jprintf( depth, "Channel gains have changed.\n" );
            result = 1;
        }
        if ( original->msg_map != updated->msg_map ) {
            jprintf( depth, "Message Bitmap: [0x%04lx] ===> [0x%04lx]\n",
                     (unsigned long)original->msg_map,
                     (unsigned long)updated->msg_map );
            result = 1;
        }
    } else {
        if ( original->clock_to       != updated->clock_to       ||
             original->initial_vco    != updated->initial_vco    ||
             original->gps_backup     != updated->gps_backup     ||
             original->samp_rates     != updated->samp_rates     ||
             original->gain_map       != updated->gain_map       ||
             original->filter_map     != updated->filter_map     ||
             original->input_map      != updated->input_map      ||
             original->web_port       != updated->web_port       ||
             original->server_to      != updated->server_to      ||
             original->drift_tol      != updated->drift_tol      ||
             original->jump_filt      != updated->jump_filt      ||
             original->jump_thresh    != updated->jump_thresh    ||
             original->cal_offset     != updated->cal_offset     ||
             original->sensor_map     != updated->sensor_map     ||
             original->sampling_phase != updated->sampling_phase ||
             original->gps_cold       != updated->gps_cold       ||
             original->user_tag       != updated->user_tag       ||
             memcmp( original->scaling, updated->scaling, 
                     sizeof(original->scaling) )                 ||
             memcmp( original->offsets, updated->offsets, 
                     sizeof(original->offsets) )                 ||
             memcmp( original->gains, updated->gains, 
                     sizeof(original->gains) )                   ||
             original->msg_map        != updated->msg_map ) {
            result = 1;
        }
    }
    return result;
}

int diff_fixed( tfixed* original, tfixed* updated, bool print_diff ) {
    int result = 0;
    int depth = 1;
    string31 str31_a;
    string31 str31_b;
    string31 str31;

    if ( print_diff ) {
        if ( memcmp( original->sys_num, updated->sys_num,
                     sizeof(original->sys_num) ) ) {
            jprintf( depth, "Q330 Serial Number: [%s] ===> [%s]\n",
                     showsn((t64*)&original->sys_num, &str31_a),
                     showsn((t64*)&updated->sys_num, &str31_b) );
            result = 1;
        }
        if ( memcmp( original->amb_num, updated->amb_num,
                     sizeof(original->amb_num) ) ) {
            jprintf( depth, "AMB Serial Number: [%s] ===> [%s]\n",
                     showsn((t64*)&original->amb_num, &str31_a),
                     showsn((t64*)&updated->amb_num, &str31_b) );
            result = 1;
        }
        if ( memcmp( original->seis1_num, updated->seis1_num,
                     sizeof(original->seis1_num) ) ) {
            jprintf( depth, "Seismo 1 Serial Number: [%s] ===> [%s]\n",
                     showsn((t64*)&original->seis1_num, &str31_a),
                     showsn((t64*)&updated->seis1_num, &str31_b) );
            result = 1;
        }
        if ( memcmp( original->seis2_num, updated->seis2_num,
                     sizeof(original->seis2_num) ) ) {
            jprintf( depth, "Seismo 2 Serial Number: [%s] ===> [%s]\n",
                     showsn((t64*)&original->seis2_num, &str31_a),
                     showsn((t64*)&updated->seis2_num, &str31_b) );
            result = 1;
        }
        if ( original->qapchp1_num != updated->qapchp1_num ) {
            jprintf( depth, "QAPCHP 1 Serial Number: [%d] ===> [%d]\n",
                     (long)original->qapchp1_num,
                     (long)updated->qapchp1_num );
            result = 1;
        }
        if ( original->qapchp2_num != updated->qapchp2_num ) {
            jprintf( depth, "QAPCHP 2 Serial Number: [%d] ===> [%d]\n",
                     (long)original->qapchp2_num,
                     (long)updated->qapchp2_num );
            result = 1;
        }
        if ( original->property_tag != updated->property_tag ) {
            jprintf( depth, "KMI Property Tag Number: [%lu] ===> [%lu]\n",
                     (unsigned long)original->property_tag,
                     (unsigned long)updated->property_tag );
            result = 1;
        }
        if ( original->sys_ver != updated->sys_ver ) {
            jprintf( depth, "System Software Version: [%d.%d] ===> [%d.%d]\n",
                     original->sys_ver >> 8, (long)(original->sys_ver & 255),
                     updated->sys_ver  >> 8, (long)(updated->sys_ver  & 255) );
            result = 1;
        }
        if ( original->sp_ver != updated->sp_ver ) {
            jprintf( depth, "Slave Processor Version: [%d.%d] ===> [%d.%d]\n",
                     original->sp_ver >> 8, (long)(original->sp_ver & 255),
                     updated->sp_ver  >> 8, (long)(updated->sp_ver  & 255) );
            result = 1;
        }
        if ( original->cal_type != updated->cal_type ) {
            jprintf( depth, "Calibrator Type: [%s] ===> [%s]\n",
                     original->cal_type == 33 ? "QCAL330" : "Unknown",
                     updated->cal_type  == 33 ? "QCAL330" : "Unknown" );
            result = 1;
        }
        if ( original->cal_ver != updated->cal_ver ) {
            jprintf( depth, "Calibrator Version: [%d.%d] ===> [%d.%d]\n",
                     original->cal_ver >> 8, (long)(original->cal_ver & 255),
                     updated->cal_ver  >> 8, (long)(updated->cal_ver  & 255) );
            result = 1;
        }
        if ( original->aux_type != updated->aux_type ) {
            jprintf( depth, "Auxillary Board Type: [%s] ===> [%s]\n",
                     original->aux_type == AUXAD_ID ? "AUXAD" : "None",
                     updated->aux_type  == AUXAD_ID ? "AUXAD" : "None" );
            result = 1;
        }
        if ( original->aux_ver != updated->aux_ver ) {
            jprintf( depth, "Auxillary Board Version: [%d.%d] ===> [%d.%d]\n",
                     original->aux_ver >> 8, (long)(original->aux_ver & 255),
                     updated->aux_ver  >> 8, (long)(updated->aux_ver  & 255) );
            result = 1;
        }
        if ( original->clk_type != updated->clk_type ) {
            jprintf( depth, "Clock Type: [%s] ===> [%s]\n",
                     original->clk_type == 1 ? "Motorolla M12" :
                     original->clk_type == 2 ? "Seascan"       : "None",
                     updated->clk_type  == 1 ? "Motorolla M12" :
                     updated->clk_type  == 2 ? "Seascan"       : "None" );
            result = 1;
        }
        if ( original->pld_ver != updated->pld_ver ) {
            jprintf( depth, "PLD Version: [%d.%d] ===> [%d.%d]\n",
                     original->pld_ver >> 8, (long)(original->pld_ver & 255),
                     updated->pld_ver  >> 8, (long)(updated->pld_ver  & 255) );
            result = 1;
        }
           /*original->last_reboot != updated->last_reboot */
           /*original->reboots != updated->reboots */
        if ( original->backup_map != updated->backup_map ) {
            jprintf( depth, "Backup Bitmap: [0x%08lx] ===> [0x%08lx]\n",
                     (unsigned long)original->backup_map,
                     (unsigned long)updated->backup_map );
            result = 1;
        }
        if ( original->default_map != updated->default_map ) {
            jprintf( depth, "Default Bitmap: [0x%08lx] ===> [0x%08lx]\n",
                     (unsigned long)original->default_map,
                     (unsigned long)updated->default_map );
            result = 1;
        }
        if ( original->flags != updated->flags ) {
            jprintf( depth, "Ethernet Installed: [0x%04lx] ===> [0x%04lx]\n",
                     (unsigned long)original->default_map,
                     (unsigned long)updated->default_map );
            result = 1;
        }
        if ( original->mem_block != updated->mem_block ) {
            jprintf( depth, "Memory Block Size: [%lu] ===> [%lu]\n",
                     (unsigned long)original->mem_block,
                     (unsigned long)updated->mem_block );
            result = 1;
        }
        if ( original->int_sz != updated->int_sz ) {
            jprintf( depth, "Internal Data Memory Size: [%lu kB] ===> [%lu kB]\n",
                     (unsigned long)original->int_sz,
                     (unsigned long)updated->int_sz );
            result = 1;
        }
           /*original->int_used != updated->int_used */
        if ( original->ext_sz != updated->ext_sz ) {
            jprintf( depth, "External Data Memory Size: [%lu kB] ===> [%lu kB]\n",
                     (unsigned long)original->ext_sz,
                     (unsigned long)updated->ext_sz );
            result = 1;
        }
           /*original->ext_used != updated->ext_used */
        if ( original->flash_sz != updated->flash_sz ) {
            jprintf( depth, "Flash Memory Size: [%lu kB] ===> [%lu kB]\n",
                     (unsigned long)original->flash_sz,
                     (unsigned long)updated->flash_sz );
            result = 1;
        }
           /*memcmp( original->log_sz, updated->log_sz, 
                     sizeof(original->log_sz ) */
        if ( original->freq7 != updated->freq7 ) {
            jprintf( depth, "Bit 7 Frequency: [%lu] ===> [%lu]\n",
                     (unsigned long)original->freq7,
                     (unsigned long)updated->freq7 );
            result = 1;
        }
        if ( original->freq6 != updated->freq6 ) {
            jprintf( depth, "Bit 6 Frequency: [%lu] ===> [%lu]\n",
                     (unsigned long)original->freq6,
                     (unsigned long)updated->freq6 );
            result = 1;
        }
        if ( original->freq5 != updated->freq5 ) {
            jprintf( depth, "Bit 5 Frequency: [%lu] ===> [%lu]\n",
                     (unsigned long)original->freq5,
                     (unsigned long)updated->freq5 );
            result = 1;
        }
        if ( original->freq4 != updated->freq4 ) {
            jprintf( depth, "Bit 4 Frequency: [%lu] ===> [%lu]\n",
                     (unsigned long)original->freq4,
                     (unsigned long)updated->freq4 );
            result = 1;
        }
        if ( original->freq3 != updated->freq3 ) {
            jprintf( depth, "Bit 3 Frequency: [%lu] ===> [%lu]\n",
                     (unsigned long)original->freq3,
                     (unsigned long)updated->freq3 );
            result = 1;
        }
        if ( original->freq2 != updated->freq2 ) {
            jprintf( depth, "Bit 2 Frequency: [%lu] ===> [%lu]\n",
                     (unsigned long)original->freq2,
                     (unsigned long)updated->freq2 );
            result = 1;
        }
        if ( original->freq1 != updated->freq1 ) {
            jprintf( depth, "Bit 1 Frequency: [%lu] ===> [%lu]\n",
                     (unsigned long)original->freq1,
                     (unsigned long)updated->freq1 );
            result = 1;
        }
        if ( original->freq0 != updated->freq0 ) {
            jprintf( depth, "Bit 0 Frequency: [%lu] ===> [%lu]\n",
                     (unsigned long)original->freq0,
                     (unsigned long)updated->freq0 );
            result = 1;
        }
        if ( memcmp( original->ch13_delay, updated->ch13_delay, 
                     sizeof(original->ch13_delay) ) ) {
            jprintf( depth, "Channel delays 1-3 have changed.\n" );
            result = 1;
        }
        if ( memcmp( original->ch46_delay, updated->ch46_delay, 
                     sizeof(original->ch46_delay) ) ) {
            jprintf( depth, "Channel delays 4-6 have changed.\n" );
            result = 1;
        }
    } else {
        if ( /*original->last_reboot != updated->last_reboot    ||*/
             /*original->reboots     != updated->reboots        ||*/
             original->backup_map    != updated->backup_map   ||
             original->default_map   != updated->default_map  ||
             original->cal_type      != updated->cal_type     ||
             original->cal_ver       != updated->cal_ver      ||
             original->aux_type      != updated->aux_type     ||
             original->clk_type      != updated->clk_type     ||
             original->flags         != updated->flags        ||
             original->sys_ver       != updated->sys_ver      ||
             original->sp_ver        != updated->sp_ver       ||
             original->pld_ver       != updated->pld_ver      ||
             original->mem_block     != updated->mem_block    ||
             original->property_tag  != updated->property_tag ||
             memcmp( original->sys_num, updated->sys_num,
                     sizeof(original->sys_num) )              ||
             memcmp( original->amb_num, updated->amb_num,
                     sizeof(original->amb_num) )              ||
             memcmp( original->seis1_num, updated->seis1_num,
                     sizeof(original->seis1_num) )            ||
             memcmp( original->seis2_num, updated->seis2_num,
                     sizeof(original->seis2_num) )            ||
             original->qapchp1_num   != updated->qapchp1_num  ||
             original->int_sz        != updated->int_sz       ||
             /*original->int_used    != updated->int_used       ||*/
             original->ext_sz        != updated->ext_sz       ||
             original->flash_sz      != updated->flash_sz     ||
             /*original->ext_used    != updated->ext_used      ||*/
             original->qapchp2_num   != updated->qapchp2_num  ||
             memcmp( original->log_sz, updated->log_sz,
                     sizeof(original->log_sz) )               ||
             original->freq7         != updated->freq7        ||
             original->freq6         != updated->freq6        ||
             original->freq5         != updated->freq5        ||
             original->freq4         != updated->freq4        ||
             original->freq3         != updated->freq3        ||
             original->freq2         != updated->freq2        ||
             original->freq1         != updated->freq1        ||
             original->freq0         != updated->freq0        ||
             memcmp( original->ch13_delay, updated->ch13_delay, 
                     sizeof(original->ch13_delay) )           ||
             memcmp( original->ch46_delay, updated->ch46_delay, 
                     sizeof(original->ch46_delay) ) ) {
            result = 1;
        }
    }
    return result;
}

int diff_log( tlog* original, tlog* updated, bool print_diff ) {
    int result = 0;
    int depth = 1;

    if ( print_diff ) {
        if ( original->lport != updated->lport ) {
            jprintf( depth, "Logical Port Number: [%lu] ===> [%lu]\n",
                     (unsigned long)original->lport,
                     (unsigned long)updated->lport );
            result = 1;
        }
        if ( original->flags != updated->flags ) {
            jprintf( depth, "Logical Port Flags: [0x%04lx] ===> [0x%04lx]\n",
                     (unsigned long)original->flags,
                     (unsigned long)updated->flags );
            result = 1;
        }
             /* original->perc != updated->perc ) { } */
        if ( original->mtu != updated->mtu ) {
            jprintf( depth, "Size of Data Packet: [%lu] ===> [%lu]\n",
                     (unsigned long)original->mtu,
                     (unsigned long)updated->mtu );
            result = 1;
        }
             /* original->grp_cnt != updated->grp_cnt ) { } */
        if ( original->rsnd_max != updated->rsnd_max ) {
            jprintf( depth, "Maximum Resend Timeout: [%lu] ===> [%lu]\n",
                     (unsigned long)original->rsnd_max,
                     (unsigned long)updated->rsnd_max );
            result = 1;
        }
        if ( original->grp_to != updated->grp_to ) {
            jprintf( depth, "Group Timeout: [%lu] ===> [%lu]\n",
                     (unsigned long)original->grp_to,
                     (unsigned long)updated->grp_to );
            result = 1;
        }
        if ( original->rsnd_min != updated->rsnd_min ) {
            jprintf( depth, "Minimum Resend Timeout: [%lu] ===> [%lu]\n",
                     (unsigned long)original->rsnd_min,
                     (unsigned long)updated->rsnd_min );
            result = 1;
        }
        if ( original->window != updated->window ) {
            jprintf( depth, "Window Size: [%lu] ===> [%lu]\n",
                     (unsigned long)original->window,
                     (unsigned long)updated->window );
            result = 1;
        }
             /*original->dataseq != updated->dataseq*/
        if ( memcmp( original->freqs, updated->freqs, 
                     sizeof(original->freqs) ) ) {
            jprintf( depth, "Channel frequencies have changed.\n" );
            result = 1;
        }
            /*original->ack_cnt != updated->ack_cnt*/
        if ( original->ack_to != updated->ack_to ) {
            jprintf( depth, "Acknowledge Timeout: [%lu] ===> [%lu]\n",
                     (unsigned long)original->ack_to,
                     (unsigned long)updated->ack_to );
            result = 1;
        }
        if ( original->olddata != updated->olddata ) {
            jprintf( depth, "Flush Threshold: [%lu] ===> [%lu]\n",
                     (unsigned long)original->olddata,
                     (unsigned long)updated->olddata );
            result = 1;
        }
        if ( original->eth_throttle != updated->eth_throttle ) {
            jprintf( depth, "Ethernet Throttling: [%lu] ===> [%lu]\n",
                     (unsigned long)original->eth_throttle,
                     (unsigned long)updated->eth_throttle );
            result = 1;
        }
        if ( original->full_alert != updated->full_alert ) {
            jprintf( depth, "Data Fault Threshold: [%lu] ===> [%lu]\n",
                     (unsigned long)original->full_alert,
                     (unsigned long)updated->full_alert );
            result = 1;
        }
        if ( original->auto_filter != updated->auto_filter ) {
            jprintf( depth, "SEED (Auto) Channel Filters: [%lu] ===> [%lu]\n",
                     (unsigned long)original->auto_filter,
                     (unsigned long)updated->auto_filter );
            result = 1;
        }
        if ( original->man_filter != updated->man_filter ) {
            jprintf( depth, "Manual Channel Filters: [%lu] ===> [%lu]\n",
                     (unsigned long)original->man_filter,
                     (unsigned long)updated->man_filter );
            result = 1;
        }
        if ( original->spare != updated->spare ) {
            jprintf( depth, "Spare: [%lu] ===> [%lu]\n",
                     (unsigned long)original->spare,
                     (unsigned long)updated->spare );
            result = 1;
        }
    } else {
        if ( original->lport        != updated->lport        ||
             original->flags        != updated->flags        ||
             /*original->perc         != updated->perc         ||*/
             original->mtu          != updated->mtu          ||
             /*original->grp_cnt      != updated->grp_cnt      ||*/
             original->rsnd_max     != updated->rsnd_max     ||
             original->grp_to       != updated->grp_to       ||
             original->rsnd_min     != updated->rsnd_min     ||
             original->window       != updated->window       ||
             /*original->dataseq      != updated->dataseq      ||*/
             memcmp( original->freqs, updated->freqs, 
                     sizeof(original->freqs) )               ||
             /*original->ack_cnt      != updated->ack_cnt      ||*/
             original->ack_to       != updated->ack_to       ||
             original->olddata      != updated->olddata      ||
             original->eth_throttle != updated->eth_throttle ||
             original->full_alert   != updated->full_alert   ||
             original->auto_filter  != updated->auto_filter  ||
             original->man_filter   != updated->man_filter   ||
             original->spare        != updated->spare ) {
            result = 1;
        }
    }
    return result;
}

bool done_reading( file_context* context ) {
    bool complete = false;
    if ( context ) {
        complete = context->eof_reached;
    }
    return complete;
}

bool data_complete( file_context* context ) {
    bool complete = false;
    if ( context ) {
        complete = context->flags == PF_COMPLETE;
    }
    return complete;
}

void get_seed_file_info( char *file_name, seed_file_info* file_info, 
                         bool print_file_info ) {
    FILE *file_handle = NULL;
    uint8_t *buffer = NULL;
    long end_offset;
    long file_length;
    long records_read;
    seed_header header_seed; /* SEED header struct*/
    uint8_t* header_ptr = NULL;

    file_info->good = false;

    if ( print_file_info ) {
        printf( "Getting file info for %s\n", file_name );
    }

    if ( !file_name ) {
        fprintf( stderr, "No file name supplied.\n" );
        goto clean;
    }

    buffer = malloc( (size_t)(RECORD_SIZE) );
    if ( !buffer ) {
        fprintf( stderr, "Could not allocate memory for read buffer.\n" );
        goto clean;
    }

    file_handle = fopen( file_name, "rb" );
    if ( !file_handle ) {
        fprintf( stderr, "Unable to open file '%s' for reading.\n", file_name );
        goto clean;
    }

    fseek( file_handle, 0, SEEK_END );
    file_length = ftell( file_handle );
    if ( file_length < (long)RECORD_SIZE ) {
        fprintf( stderr, "File is too small to contain seed records.\n" );
        goto clean;
    }
    end_offset = file_length % (long)RECORD_SIZE
                    ? file_length / (long)RECORD_SIZE 
                        ? file_length - (file_length % (long)RECORD_SIZE) - (long)RECORD_SIZE
                        : (long)0
                    : file_length - (long)RECORD_SIZE;
    if ( print_file_info ) {
        printf( "File size is %li bytes.\n", file_length );
    }

    if ( print_file_info ) {
        printf( "Reading at position [%li]\n", (long)0 );
    }
    fseek( file_handle, 0, SEEK_SET ); 
    records_read = fread( buffer, (size_t)RECORD_SIZE, 1, file_handle );
    header_ptr = buffer;
    loadseedhdr( (pbyte *)&header_ptr, &header_seed, false );
    if ( print_file_info ) {
        print_seed_header( &header_seed );
    }
    if ( !records_read ) {
        goto clean;
    }
    file_info->start_time = header_seed.starting_time;

    if ( print_file_info ) {
        printf( "Reading at position [%li]\n", end_offset );
    }
    fseek( file_handle, end_offset, SEEK_SET ); 
    records_read = fread( buffer, (size_t)RECORD_SIZE, 1, file_handle );
    header_ptr = buffer;
    loadseedhdr( (pbyte *)&header_ptr, &header_seed, false );
    if ( print_file_info ) {
        print_seed_header( &header_seed );
    }
    if ( !records_read ) {
        goto clean;
    }
    file_info->end_time = header_seed.starting_time;

    fclose( file_handle );

    file_info->length = file_length;
    file_info->good = true;
clean:
    if ( buffer ) {
        free( buffer );
    }
}

void populate_records ( char *file_name, 
                        list_t* list_global, 
                        list_t* list_fixed,
                        list_t* list_log, 
                        list_t* list_token_context,
                        bool print_file_info ) {
    FILE *file_handle = NULL;

    uint8_t *buffer = NULL;
    size_t records_read = 0;
    size_t header_len = 0;
    size_t i = 0, j = 0;
    uint8_t *current_record = NULL; /* the selected 512-byte record */
    uint8_t *current_position = NULL; /* position within selected record */
    uint8_t *header_ptr = NULL; /* temporary pointer for header parsing functions*/

    seed_header header_seed; /* SEED header struct*/
    topaque_hdr header_opaque; /* Opaque blockette header struct*/
    char rec_type_str[REC_TYPE_MAX_LEN + 1];

    int16_t blockette_type;
    int16_t next_blockette;
    int16_t blockette_length;
    bool    first_read = true;

    tglobal* ocf_global = NULL;
    tfixed* ocf_fixed = NULL;
    tlog* ocf_log = NULL;
    token_context* ocf_token_context = NULL;
    list_metadata* metadata = NULL;

    file_context f_context;
    file_context* context_file = &f_context;

    memset( context_file, 0, sizeof(file_context) );

    if ( !file_name || !context_file || context_file->eof_reached )
        return;

    buffer = malloc( (size_t)(BUFFER_SIZE) );
    if ( !buffer ) {
        fprintf( stderr, "Could not allocate memory for read buffer.\n" );
        goto clean;
    }

    file_handle = fopen( file_name, "rb" );
    if ( !file_handle ) {
        fprintf( stderr, "Unable to open file '%s' for reading.\n", file_name );
        goto clean;
    }


    /* interpret header type, and break into opaque block types */
    while ( !context_file->eof_reached ) {
        records_read = fread( buffer, (size_t)RECORD_SIZE, (size_t)MAX_RECORDS, file_handle );
        context_file->offset = ftell( file_handle );
        if ( feof(file_handle) || ferror(file_handle) ) {
            context_file->eof_reached = true;
        }

        /* populate the header structs */
        current_record = buffer;
        for (i = 0; i < records_read; i++ ) {
            header_ptr = current_record;
            loadseedhdr( (pbyte *)&header_ptr, &header_seed, false );
            if ( print_file_info ) {
                print_seed_header( &header_seed );
            }
            next_blockette = header_seed.first_blockette_byte;
            blockette_type = 0;
            current_position = current_record + next_blockette;

            if ( first_read ) {
                first_read = false;
                memcpy( &context_file->timestamp, &header_seed.starting_time,
                        sizeof( tseed_time ) );
            }

            while ( next_blockette ) {
                blockette_type = ntohs(*(int16_t *)(current_position + 0));
                next_blockette = ntohs(*(int16_t *)(current_position + 2));
                if (blockette_type == 2000) {
                    blockette_length = ntohs(*(int16_t *)(current_position + 4));
                    header_ptr = current_position;
                    loadopaquehdr( (pbyte *)&header_ptr, &header_opaque );

                    header_ptr = current_position + (size_t)header_opaque.data_off;
                    header_len = (size_t)header_opaque.blk_lth - (size_t)header_opaque.data_off;
                    metadata = (list_metadata*)malloc( sizeof(list_metadata) );
                    if ( metadata ) {
                        memcpy( &metadata->time, &header_seed.starting_time, sizeof(tseed_time) );
                    }
                    if ( header_opaque.rec_type[0] == 'G' && 
                         header_opaque.rec_type[1] == 'L' ) {
                        ocf_global = (tglobal*)malloc( sizeof(tglobal) );
                        if ( ocf_global && metadata ) {
                            loadglob( (pbyte *)&header_ptr, ocf_global );
                            metadata->data = ocf_global;
                            list_append( list_global, metadata );
                            metadata = NULL;
                        }
                    }
                    else if ( header_opaque.rec_type[0] == 'F' && 
                              header_opaque.rec_type[1] == 'X' ) {
                        ocf_fixed = (tfixed*)malloc(sizeof(tfixed));
                        if ( ocf_fixed) {
                            loadfix( (pbyte *)&header_ptr, ocf_fixed ); 
                            metadata->data = ocf_fixed;
                            list_append( list_fixed, metadata );
                            metadata = NULL;
                        }
                    }
                    else if ( header_opaque.rec_type[0] == 'L' ) {
                        ocf_log = (tlog*)malloc(sizeof(tlog));
                        if ( ocf_log) {
                            loadlog( (pbyte *)&header_ptr, ocf_log );
                            metadata->data = ocf_log;
                            list_append( list_log, metadata );
                            metadata = NULL;
                        }
                    }
                    else if ( header_opaque.rec_type[0] == 'T' ) {
                        if ( !ocf_token_context ) {
                            ocf_token_context = (token_context*)calloc(sizeof(token_context), 1);
                        }
                        if ( ocf_token_context ) {
                            append_tokens( ocf_token_context,
                                           header_ptr,
                                           header_len,
                                           header_opaque.opaque_flags );
                        }
                        if ( tokens_complete( ocf_token_context ) ) {
                            metadata->data = ocf_token_context;
                            list_append( list_token_context, metadata );
                            ocf_token_context = NULL;
                            metadata = NULL;
                        }
                    }
                    if ( metadata ) {
                        free( metadata );
                    }
                }
                else if (blockette_type == 1000) {
                    blockette_length = ntohs((uint16_t)(*(current_position + 6)));
                }
                current_position = current_record + next_blockette;
            }
            current_record += (size_t)RECORD_SIZE;
        }

    } 
clean:
    if ( buffer ) {
        free( buffer );
    }
}

void print_seed_header( seed_header* header ) {
    printf( "SEED Record:" );
    printf( " %c%c_%c%c%c%c%c %c%c-%c%c%c",
            header->seednet[0],
            header->seednet[1], 
            header->station_id_call_letters[0],
            header->station_id_call_letters[1],
            header->station_id_call_letters[2],
            header->station_id_call_letters[3],
            header->station_id_call_letters[4],
            header->location_id[0],
            header->location_id[1],
            header->channel_id[0],
            header->channel_id[1],
            header->channel_id[2] );
    print_seed_time( " ", &header->starting_time, "\n" );
}

void print_seed_time( const char* prefix_msg, tseed_time* time, 
                      const char* postfix_msg ) {
    if ( time ) {
        printf( "%s[%04d-%03d %02d:%02d:%02d.%04d]%s",
                prefix_msg,
                time->seed_yr, 
                time->seed_jday, 
                time->seed_hr, 
                time->seed_minute, 
                time->seed_seconds,
                time->seed_tenth_millisec,
                postfix_msg );
    }
}

void print_global( tglobal *opaque_global, size_t depth, uint32_t pr_flags ) {
    int j, k;

    if (!opaque_global)
        return;

    if ( PRINT_WILLARD(pr_flags) ) {
        jprintf( depth, "[Willard Menu]->Configuration->Global Setup\n" );
    }
    jprintf( depth, "GPS Coldstart Timeout -- %lu sec.\n",
            (unsigned long)opaque_global->gps_cold);
    jprintf( depth, "Clock Timeout ---------- %lu sec.\n",
            (unsigned long)opaque_global->clock_to);
    jprintf( depth, "Deregistration Timeout - %lu\n",
            (unsigned long)opaque_global->server_to);
    jprintf( depth, "Initial VCO ------------ %lu\n",
            (unsigned long)opaque_global->initial_vco);
    jprintf( depth, "Calibrator Offset ------ %d\n",
            (short)opaque_global->cal_offset);
    jprintf( depth, "Phase Tolerance -------- %lu usec.\n",
            (unsigned long)opaque_global->drift_tol);
    jprintf( depth, "Jump Filter ------------ %lu\n",
            (unsigned long)opaque_global->jump_filt);
    jprintf( depth, "Jump Threshold --------- %lu usec.\n",
            (unsigned long)opaque_global->jump_thresh);
    jprintf( depth, "User Tag Number -------- %lu\n",
            (unsigned long)opaque_global->user_tag);
    jprintf( depth, "GPS Backup Power ------- %s\n",
            (unsigned short)opaque_global->gps_backup ?
            "ON" : "OFF" );
    jprintf( depth, "Web Server Port -------- %lu\n",
            (unsigned long)opaque_global->web_port);
    jprintf( depth, "\n" );

    jprintf( depth, "Sample Rates ----------- %lu\n",
            (unsigned long)opaque_global->samp_rates);
    jprintf( depth, "Sampling Phase --------- %lu\n",
            (unsigned long)opaque_global->sampling_phase);
    jprintf( depth, "Gain Bitmap ------------ 0x%04lx\n",
            (unsigned long)opaque_global->gain_map);
    jprintf( depth, "Filter Bitmap ---------- 0x%04lx\n",
            (unsigned long)opaque_global->filter_map);
    jprintf( depth, "Input Bitmap ----------- 0x%04lx\n",
            (unsigned long)opaque_global->input_map);
    jprintf( depth, "Semsor Ctl Bitmap ------ 0x%04lx\n",
            (unsigned long)opaque_global->sensor_map);
    jprintf( depth, "Message Bitmap --------- 0x%04lx\n",
            (unsigned long)opaque_global->msg_map);

    jprintf( depth, "Channel | Offset | Gain  | Frequencies\n" );
    jprintf( depth, "--------|--------|-------|--------------\n" );
    for (j = 0; j < CHANNELS; j++) {
        jprintf( depth, "% 7d | % 6d | % 5d |", (short)(j+1),
                (short)opaque_global->offsets[j],
                (short)opaque_global->gains[j] );
        for (k = 0; k < FREQS; k++) {
            printf( " % 5d",
                    (short)opaque_global->scaling[j][k] );
        }
        printf( "\n" );
    }
}

void print_fixed( tfixed *opaque_fixed, size_t depth, uint32_t pr_flags ){
    int j;
    string31 str31;
    unsigned long packet_memory_total = 0;

    if (!opaque_fixed)
        return;

    if ( PRINT_WILLARD(pr_flags) ) {
        jprintf( depth, "[Willard Menu]->Information->Hardware & Software\n" );
    }
    jprintf( depth, "Q330 Serial Number -------- %s\n",
            showsn((t64 *)&opaque_fixed->sys_num, &str31) );
    jprintf( depth, "AMB Serial Number --------- %s\n",
            showsn((t64 *)&opaque_fixed->amb_num, &str31) );
    jprintf( depth, "Seismo 1 Serial Number ---- %s\n",
            showsn((t64 *)&opaque_fixed->seis1_num, &str31) );
    jprintf( depth, "Seismo 2 Serial Number ---- %s\n",
            showsn((t64 *)&opaque_fixed->seis2_num, &str31) );
    jprintf( depth, "QAPCHP 1 Serial Number ---- %d\n",
            (long)opaque_fixed->qapchp1_num);
    jprintf( depth, "QAPCHP 2 Serial Number ---- %d\n",
            (long)opaque_fixed->qapchp2_num);
    jprintf( depth, "KMI Property Tag Number --- %lu\n",
            (unsigned long)opaque_fixed->property_tag);
    jprintf( depth, "System Software Version --- %d.%d\n",
            opaque_fixed->sys_ver >> 8, 
            (long)(opaque_fixed->sys_ver & 255));
    jprintf( depth, "Slave Processor Version --- %d.%d\n",
            opaque_fixed->sp_ver >> 8,
            (long)(opaque_fixed->sp_ver & 255));
    jprintf( depth, "Calibrator Type ----------- %s\n",
            opaque_fixed->cal_type == 33 ? "QCAL330" : "Unknown" );
    jprintf( depth, "Calibrator Version -------- %d.%d\n",
            opaque_fixed->cal_ver >> 8,
            (long)(opaque_fixed->cal_ver & 255));
    jprintf( depth, "Auxillary Board Type ------ %s\n",
            opaque_fixed->aux_type == AUXAD_ID ? "AUXAD" : "None" );
    switch( opaque_fixed->aux_type ) {
        case AUXAD_ID:
            jprintf( depth, "Auxillary Board Version --- %d.%d\n",
                     opaque_fixed->aux_ver >> 8,
                     (long)(opaque_fixed->aux_ver & 255) );
    }
    jprintf( depth, "Clock Type ---------------- %s\n",
            opaque_fixed->clk_type == 1 ? "Motorolla M12" :
            opaque_fixed->clk_type == 2 ? "Seascan" :
                                          "None" );
    jprintf( depth, "PLD Version --------------- %d.%d\n",
            opaque_fixed->pld_ver >> 8,
            (long)(opaque_fixed->pld_ver & 255));
    jprintf( depth, "\n" );

    if ( PRINT_WILLARD(pr_flags) ) {
        jprintf( depth, "[Willard Menu]->Information->Memory\n" );
    }
    jprintf( depth, "Flash Memory Size --------- %lu kB\n",
            (unsigned long)opaque_fixed->flash_sz);
    jprintf( depth, "Internal Data Memory Size - %lu kB\n",
            (unsigned long)opaque_fixed->int_sz);
    jprintf( depth, "Internal Data Memory Used - %lu kB\n",
            (unsigned long)opaque_fixed->int_used);
    jprintf( depth, "External Data Memory Size - %lu kB\n",
            (unsigned long)opaque_fixed->ext_sz);
    jprintf( depth, "External Data Memory Used - %lu kB\n",
            (unsigned long)opaque_fixed->ext_used);
    jprintf( depth, "Packet | Memory Size (bytes)\n" );
    jprintf( depth, "-------|----------------\n" );
    for (j = 0; j < (sizeof(opaque_fixed->log_sz) / sizeof(longword)); j++) {
        jprintf( depth, "% 6d | % 10lu\n", (short)(j + 1),
                (unsigned long)opaque_fixed->log_sz[j] );
        packet_memory_total += (unsigned long)opaque_fixed->log_sz[j];
    }
    jprintf( depth, "EEPROM Block Size --------- %lu\n",
            (unsigned long)opaque_fixed->mem_block);
    jprintf( depth, "\n" );

    if ( PRINT_WILLARD(pr_flags) ) {
        jprintf( depth, "[Willard Menu]->Configuration->Restricted->Manufacturer Data...\n" );
    }
    jprintf( depth, "Packet Memory Total ------- %lu\n",
             packet_memory_total );
    jprintf( depth, "\n" );

    jprintf( depth, "Last Reboot --------------- %s\n",
            jul_string((longint)opaque_fixed->last_reboot, (pchar)&str31) );
    jprintf( depth, "Reboot Count -------------- %lu\n",
            (unsigned long)opaque_fixed->reboots);
    jprintf( depth, "\n" );

    jprintf( depth, "Backup Bitmap ------------- 0x%08lx\n",
            (unsigned long)opaque_fixed->backup_map);
    jprintf( depth, "Default Bitmal ------------ 0x%08lx\n",
            (unsigned long)opaque_fixed->default_map);
    jprintf( depth, "Ethernet Installed -------- 0x%04lx\n",
            (unsigned long)opaque_fixed->flags);
    jprintf( depth, "\n" );

    jprintf( depth, "Bit 7 Frequency -- %lu\n",
            (unsigned long)opaque_fixed->freq7);
    jprintf( depth, "Bit 6 Frequency -- %lu\n",
            (unsigned long)opaque_fixed->freq6);
    jprintf( depth, "Bit 5 Frequency -- %lu\n",
            (unsigned long)opaque_fixed->freq5);
    jprintf( depth, "Bit 4 Frequency -- %lu\n",
            (unsigned long)opaque_fixed->freq4);
    jprintf( depth, "Bit 3 Frequency -- %lu\n",
            (unsigned long)opaque_fixed->freq3);
    jprintf( depth, "Bit 2 Frequency -- %lu\n",
            (unsigned long)opaque_fixed->freq2);
    jprintf( depth, "Bit 1 Frequency -- %lu\n",
            (unsigned long)opaque_fixed->freq1);
    jprintf( depth, "Bit 0 Frequency -- %lu\n",
            (unsigned long)opaque_fixed->freq0);

    jprintf( depth, "Channels | Delays\n" );
    jprintf( depth, "---------|----------\n" );
    jprintf( depth,"      1-3 | " );
    for (j = 0; j < FREQS; j++) {
        printf( "% 10d",
                (int)opaque_fixed->ch13_delay[j] );
    }
    printf( "\n" );
    jprintf( depth,"     4-6 | " );
    for (j = 0; j < FREQS; j++) {
        printf( "% 10d",
                (int)opaque_fixed->ch46_delay[j] );
    }
    printf( "\n" );
}

void print_log( tlog *opaque_log, size_t depth, uint32_t pr_flags ){
    if (! opaque_log)
        return;
    int j;
    jprintf( depth, "Logical Port Number --------- %lu\n",
            (unsigned long)opaque_log->lport);
    jprintf( depth, "Logical Port Flags ---------- 0x%04lx\n",
            (unsigned long)opaque_log->flags);
    jprintf( depth, "Percent of Packet Buffer ---- %lu\n",
            (unsigned long)opaque_log->perc);
    jprintf( depth, "Size of Data Packet --------- %lu\n",
            (unsigned long)opaque_log->mtu);
    jprintf( depth, "Group Count ----------------- %lu\n",
            (unsigned long)opaque_log->grp_cnt);
    jprintf( depth, "Maximum Resend Timeout ------ %lu\n",
            (unsigned long)opaque_log->rsnd_max);
    jprintf( depth, "Group Timeout --------------- %lu\n",
            (unsigned long)opaque_log->grp_to);
    jprintf( depth, "Minimum Resend Timeout ------ %lu\n",
            (unsigned long)opaque_log->rsnd_min);
    jprintf( depth, "Window Size ----------------- %lu\n",
            (unsigned long)opaque_log->window);
    jprintf( depth, "Data Sequence Number -------- %lu\n",
            (unsigned long)opaque_log->dataseq);
    jprintf( depth, "Channel | Frequency\n" );
    jprintf( depth, "--------|--------------\n" );
    for (j = 0; j < CHANNELS; j++) {
        jprintf( depth, "% 7d | % 5d\n", (short)(j+1),
                (unsigned short)opaque_log->freqs[j] );
    }
    jprintf( depth, "Acknowledge Skip Count ------ %lu\n",
            (unsigned long)opaque_log->ack_cnt);
    jprintf( depth, "Acknowledge Timeout --------- %lu\n",
            (unsigned long)opaque_log->ack_to);
    jprintf( depth, "Flush Threshold ------------- %lu\n",
            (unsigned long)opaque_log->olddata);
    jprintf( depth, "Ethernet Throttling --------- %lu\n",
            (unsigned long)opaque_log->eth_throttle);
    jprintf( depth, "Data Fault Threshold -------- %lu%%\n",
            (unsigned long)opaque_log->full_alert);
    jprintf( depth, "SEED (Auto) Channel Filters - %lu\n",
            (unsigned long)opaque_log->auto_filter);
    jprintf( depth, "Manual Channel Filters ------ %lu\n",
            (unsigned long)opaque_log->man_filter);
    jprintf( depth, "Spare ----------------------- %lu\n",
            (unsigned long)opaque_log->spare);
}

void print_tokens( token_context *context, size_t depth, uint32_t pr_flags ) {

    tq330 *q330;
    tfilter filter;
    tpar_create cfg;

    int j;

    memset( (void *)&filter, 0, sizeof(tfilter) );
    memset( (void *)&cfg, 0, sizeof(tpar_create) );

    cfg.mini_firchain = &filter;
    cfg.amini_exponent = 9; /* 512-byte records */
    lib_create_context( (tcontext *)&q330, &cfg );

    paqstruc aqs = q330->aqstruc;
    q330->cfgbuf = (pcfgbuf)context->buffer;
    q330->cfgsize = context->buff_used;
    aqs->non_comp = true;

    decode_cfg( q330 );

    /* Maybe we should just try dumping out the entire q330 structure...
     * That is a huge task, but it looks like there is a massive amount
     * of it that gets populated when we read back the contents of the
     * T?~ record.
     *
     * Actually, it may be possible to just pull all the data out of
     * the taqstruc structure, and just pull a few extras out of q330.
     */

    jprintf( depth, "Network ----------------- %c%c\n", 
            q330->network[0],
            q330->network[1] );
    jprintf( depth, "Station ----------------- %s\n", 
            q330->station );

    jprintf( depth, "Archive Record Size ----- %d\n", 
            (long)aqs->arc_size );
    jprintf( depth, "Frames per Record ------- %d\n", 
            (long)aqs->arc_frames );
    jprintf( depth, "Start of Cleared Fields - %d\n", 
            (unsigned short)aqs->first_sg );
    jprintf( depth, "Web Port ---------------- %d\n", 
            (unsigned short)aqs->webport );
    jprintf( depth, "Network Port ------------ %d\n", 
            (unsigned short)aqs->netport );
    jprintf( depth, "Server Port ------------- %d\n", 
            (unsigned short)aqs->dservport );
    jprintf( depth, "Continuity Good --------- %s\n", 
            aqs->contingood ? "true" : "false" );
    jprintf( depth, "Non-Complient DP -------- %s\n", 
            aqs->non_comp ? "true" : "false" );
    jprintf( depth, "First Data -------------- %s\n", 
            aqs->first_data ? "true" : "false" );

    jprintf( depth, "LOG TIM:\n" );
    jprintf( depth + 1, "Log Record Location: %c%c\n", 
            ((char *)aqs->log_tim.log_location)[0],
            ((char *)aqs->log_tim.log_location)[1] );
    jprintf( depth + 1, "Log Record SEED Name: %c%c%c\n", 
            ((char *)aqs->log_tim.log_seedname)[0],
            ((char *)aqs->log_tim.log_seedname)[1],
            ((char *)aqs->log_tim.log_seedname)[2] );
    jprintf( depth + 1, "Timing Record Location: %c%c\n", 
            ((char *)aqs->log_tim.tim_location)[0],
            ((char *)aqs->log_tim.tim_location)[1] );
    jprintf( depth + 1, "Timing Record SEED Name: %c%c%c\n", 
            ((char *)aqs->log_tim.tim_seedname)[0],
            ((char *)aqs->log_tim.tim_seedname)[1],
            ((char *)aqs->log_tim.tim_seedname)[2] );

    jprintf( depth, "LOG CFG:\n" );
    jprintf( depth + 1, "Config Record Location: %c%c\n", 
            ((char *)aqs->log_cfg.cfg_location)[0],
            ((char *)aqs->log_cfg.cfg_location)[1] );
    jprintf( depth + 1, "Config Record SEED Name: %c%c%c\n", 
            ((char *)aqs->log_cfg.cfg_seedname)[0],
            ((char *)aqs->log_cfg.cfg_seedname)[1],
            ((char *)aqs->log_cfg.cfg_seedname)[2] );
    jprintf( depth + 1, "Write Configuration:\n" );
    jprintf( depth + 2, "At session start [%s]\n", 
             aqs->log_cfg.flags & 0x01 ? "YES" : "NO" );
    jprintf( depth + 2, "At session end   [%s]\n", 
             aqs->log_cfg.flags & 0x02 ? "YES" : "NO" );
    jprintf( depth + 2, "At set interval  [%s]", 
             aqs->log_cfg.flags & 0x04 ? "YES" : "NO" );
    if ( aqs->log_cfg.flags & 0x04 )
        printf( " (every %d minutes)\n", 
                 (unsigned short)aqs->log_cfg.interval );
    else
        printf( "\n" );


    jprintf( depth, "Global Data Record Sequence Number - %lu\n", 
            (unsigned long)aqs->dt_data_sequence );
    
    if ( !context->summary_only ) {
        jprintf( depth, "Logical Channel Queue from this Server:\n" );
        print_lcq_list( aqs->lcqs, false/*dispatch*/, depth + 1 );
        jprintf( depth, "Logical Channel Queue for Sliding Window:\n" );
        print_lcq_list( aqs->proc_lcq, false/*dispatch*/, depth + 1 );
    }

    jprintf( depth, "Calibration Error Bitmap ----------- 0x%02lx\n", 
            (unsigned long)aqs->calerr_bitmap );
    jprintf( depth, "Highest LCQ Number from Tokens ----- %d\n", 
            (unsigned short)aqs->highest_lcqnum );
    jprintf( depth, "Last Data Quality ------------------ %d\n", 
            (unsigned short)aqs->last_data_qual );
    jprintf( depth, "Data Quality ----------------------- %d%%\n", 
            (unsigned short)aqs->data_qual );
    jprintf( depth, "Data Time Tag ---------------------- %f\n", 
            aqs->data_timetag );

    if ( !context->summary_only ) {
        jprintf( depth, "Token Definition:\n" );
        print_dss( &aqs->dss_def, depth + 1 );
    }
    jprintf( depth, "Comm. Events:\n" );
    for (j = 0; j < CE_MAX; j++) {
        if ( *aqs->commevents[j].name ) {
            jprintf( depth + 1, "[%s] %s\n",
                    aqs->commevents[j].ison ? "ON " : "OFF",
                    aqs->commevents[j].name );
        }
    }

    jprintf( depth, "Daily Timemark has Occured  ------ %s\n", 
            aqs->daily_done ? "true" : "false" );
    jprintf( depth, "Last Update ---------------------- %f\n", 
            aqs->last_update );
    jprintf( depth, "Timing Blockette Exception Count - %li\n", 
            (long)aqs->except_count );

    if ( !context->summary_only ) {
        jprintf( depth, "Logical Channel Queue [Config]:\n" );
        print_lcq_list( aqs->cfg_lcq, false/*dispatch*/, depth + 1 );
        jprintf( depth, "Logical Channel Queue [Message]:\n" );
        print_lcq_list( aqs->msg_lcq, false/*dispatch*/, depth + 1 );
        jprintf( depth, "Logical Channel Queue [TIM]:\n" );
        print_lcq_list( aqs->tim_lcq, false/*dispatch*/, depth + 1 );
        jprintf( depth, "Logical Channel Queue [CNP]:\n" );
        print_lcq_list( aqs->cnp_lcqs, false/*dispatch*/, depth + 1 );
    }

    if ( !context->summary_only ) {
        jprintf( depth, "Filter Chain:\n" );
        print_tiirdef_list( aqs->iirchain, depth + 1 );
        jprintf( depth, "Detector Chain:\n" );
        print_detector_list( aqs->defchain, depth + 1 );
        jprintf( depth, "Control Chain:\n" );
        print_cdetector_list( aqs->ctrlchain, depth + 1 );
    }

    jprintf( depth, "Configuration Last Written ------- %f\n", 
            aqs->cfg_lastwritten );
    jprintf( depth, "Total Detectors ------------------ %d\n", 
            (unsigned short)aqs->total_detectors );
    jprintf( depth, "Opaque Buffer -------------------- 0x%08lx\n", 
            (unsigned long)aqs->opaque_buf );
    jprintf( depth, "Opaque Size ---------------------- %d\n", 
            (unsigned short)aqs->opaque_size );

    jprintf( depth, "Timing Blockette:\n" );
    jprintf( depth + 1, "Blockette Type ---- %d\n",
            (unsigned short)aqs->timing_buf.blockette_type );
    jprintf( depth + 1, "Next Blockette ---- %d\n",
            (unsigned short)aqs->timing_buf.next_blockette );
    jprintf( depth + 1, "VCO Correction ---- %f%%\n",
            (float)aqs->timing_buf.vco_correction );
    jprintf( depth + 1, "Time of Exception - %d %d %d:%d:%d.%d%d\n",
            aqs->timing_buf.time_of_exception.STUNION.STGREG.yr,
            aqs->timing_buf.time_of_exception.STUNION.STGREG.jday,
            aqs->timing_buf.time_of_exception.STUNION.STGREG.hr,
            aqs->timing_buf.time_of_exception.STUNION.STGREG.minute,
            aqs->timing_buf.time_of_exception.STUNION.STGREG.seconds,
            aqs->timing_buf.time_of_exception.STUNION.STGREG.tenth_millisec,
            (unsigned short)aqs->timing_buf.usec99 );
    jprintf( depth + 1, "Reception Quality - %d\n",
            (unsigned short)aqs->timing_buf.reception_quality );
    jprintf( depth + 1, "Exception Count --- %d\n",
            (long)aqs->timing_buf.exception_count );
    jprintf( depth + 1, "Exception Type ---- %s\n",
            *aqs->timing_buf.exception_type ?
            aqs->timing_buf.exception_type : "null" );
    jprintf( depth + 1, "Clock Model ------- %s\n",
            *aqs->timing_buf.clock_model ?
            aqs->timing_buf.clock_model : "null" );
    jprintf( depth + 1, "Clock Status ------ %s\n",
            *aqs->timing_buf.clock_status ?
            aqs->timing_buf.clock_status : "null" );

    if ( !context->summary_only ) {
        jprintf( depth, "Compressed Buffer Ring:\n" );
        print_tcompressed_buffer_ring_list( &aqs->detcal_buf, depth + 1 );
    }

    jprintf( depth, "End of Cleared Fields - %d\n", 
            (unsigned short)aqs->last_sg );

    if ( !context->summary_only ) {
        jprintf( depth, "Logical Channel Queue [Statistics]:\n" );
        print_lcq_list( aqs->dplcqs, false/*dispatch*/, depth + 1 );
        jprintf( depth, "Logical Channel Queue [Data Latency]:\n" );
        print_lcq_list( aqs->data_latency_lcq, false/*dispatch*/, depth + 1 );
        jprintf( depth, "Logical Channel Queue [Status Latency]:\n" );
        print_lcq_list( aqs->status_latency_lcq, false/*dispatch*/, depth + 1 );
        jprintf( depth, "dispatch:\n" );
        print_tdispatch( &aqs->dispatch, depth + 1 );
        jprintf( depth, "mdispatch:\n" );
        print_tmdispatch( &aqs->mdispatch, depth + 1 );
        jprintf( depth, "msgqueue:\n" );
        print_mholdqtype_list( aqs->msgqueue, depth + 1 );
        jprintf( depth, "msgq_in:\n" );
        print_mholdqtype_list( aqs->msgq_in, depth + 1 );
        jprintf( depth, "msgq_out:\n" );
        print_mholdqtype_list( aqs->msgq_out, depth + 1 );
        jprintf( depth, "FIR Filter:\n" );
        print_tfilter_list( aqs->firchain, depth + 1 );
    }

    jprintf( depth, "Count-down since last config. data added - %d\n", 
            (long)aqs->cfg_timer );

}

void print_lcq_list( tlcq *head, bool dispatch, size_t depth ) {

    if ( !head ) {
        jprintf( depth, "EMPTY\n" );
        return;
    }

    while ( head ) {
        jprintf( depth, "%s 0x%08lx\n",
                dispatch ? "Link ---------------------------" :
                           "Dispatch Link ------------------",
                dispatch ? (unsigned long)head->link :
                           (unsigned long)head->dispatch_link );
        jprintf( depth, "SEED Location ------------------ %c%c\n",
                ((char *)head->location)[0],
                ((char *)head->location)[1] );
        jprintf( depth, "SEED Name ---------------------- %c%c%c\n",
                ((char *)head->seedname)[0],
                ((char *)head->seedname)[1],
                ((char *)head->seedname)[2] );
        jprintf( depth, "LCQ Reference Number ----------- %d\n", 
                (unsigned short)head->lcq_num );
        jprintf( depth, "Raw Data Source (Q330 Channel) - %d\n", 
                (unsigned short)head->raw_data_source );
        jprintf( depth, "More Raw Data (Q330 Channel) --- %d\n", 
                (unsigned short)head->raw_data_field );
        jprintf( depth, "LCQ Options -------------------- %lu\n", 
                (unsigned long)head->lcq_opt );
        jprintf( depth, "SEED Location (Dynamic) -------- %c%c\n", 
                ((char *)head->slocation)[0],
                ((char *)head->slocation)[1] );
        jprintf( depth, "SEED Name (Dynamic) ------------ %c%c%c\n", 
                ((char *)head->sseedname)[0],
                ((char *)head->sseedname)[1],
                ((char *)head->sseedname)[2] );
        if ( head->rate >= 0 ) {
            jprintf( depth, "Rate -------------------------- %.0f Hz\n", 
                     (float)head->rate );
        } else {
            jprintf( depth, "Rate -------------------------- %.3f Hz\n", 
                     -1.0 / (float)head->rate );
        }
        jprintf( depth, "Timemark Occured --------------- %s\n", 
                head->timemark_occurred ? "true" : "false" );
        jprintf( depth, "Calibration -------------------- %s\n", 
                head->cal_on ? "ON" : "OFF" );
        jprintf( depth, "Unfiltered Calibration Status -- %s\n", 
                head->calstat ? "ON" : "OFF" );
        jprintf( depth, "Variable Rate LCQ --------------- %s\n", 
                head->variable_rate_set ? "YES" : "NO" );
        jprintf( depth, "Validated ---------------------- %s\n", 
                head->validated ? "YES" : "NO" );
        jprintf( depth, "Packet Class ------------------- %s\n", 
                head->pack_class == PKC_DATA      ? "Data"      :
                head->pack_class == PKC_EVENT     ? "Event"     : 
                head->pack_class == PKC_CALIBRATE ? "Calibrate" :
                head->pack_class == PKC_TIMING    ? "Timing"    :
                head->pack_class == PKC_MESSAGE   ? "Message"   :
                head->pack_class == PKC_OPAQUE    ? "Opaque"    : "Unknown" );
        jprintf( depth, "Record Sequence Number --------- %lu\n", 
                head->dtsequence );
        jprintf( depth, "Segment Sequence Number -------- %lu\n", 
                head->seg_seq );
        /* TODO: 
         psegment_ring segbuf, pseg, seg_next
         */
        jprintf( depth, "Segment Buffer Size ------------ %d\n", 
                head->segsize );
        jprintf( depth, "Segment Count ------------------ %d\n", 
                head->seg_count );
        jprintf( depth, "Highest Segment ---------------- %d\n", 
                head->seg_high );
        /* TODO: 
         pmergedbuf mergedbuf
         */
        jprintf( depth, "OSF_xxx Bits ------------------- 0x%04lx\n", 
                (unsigned long)head->onesec_filter );
        /* TODO: 
         pidxarray idxbuf
         pdataarray databuf
         */
        jprintf( depth, "Data Size ---------------------- %d\n", 
                head->datasize );
        /* TODO: 
         tdhqp dholdq
         */
        jprintf( depth, "Time Tag ----------------------- %f\n", 
                head->timetag );
        jprintf( depth, "Backup Tag --------------------- %f\n", 
                head->backup_tag );
        jprintf( depth, "Last Time Tag ------------------ %f\n", 
                head->last_timetag );
        jprintf( depth, "Time Quality ------------------- %d\n", 
                head->timequal );
        jprintf( depth, "Backup Quality ----------------- %d\n", 
                head->backup_qual );
        jprintf( depth, "Gap Threshold ------------------ %f\n", 
                head->gap_threshold );
        jprintf( depth, "Gap Seconds -------------------- %f\n", 
                head->gap_secs );
        jprintf( depth, "Gap Offset --------------------- %f\n", 
                head->gap_offset );
        /* TODO: 
         tprecomp precomp
         */
        jprintf( depth, "Slipping ----------------------- %s\n", 
                head->slipping ? "YES" : "NO" );
        jprintf( depth, "Slip Modulus ------------------- %li\n", 
                head->slip_modulus );
        jprintf( depth, "Input Sample Rate -------------- %f\n", 
                head->input_sample_rate );
        /* TODO: 
         pdownstream_packet downstream_link
         struct tlcq *prev_link
         */
        jprintf( depth, "Average Length ----------------- %li\n", 
                head->avg_length );
        jprintf( depth, "FIR Fixing Gain ---------------- %f\n", 
                head->firfixing_gain );
        /* TODO: 
         pcom_packet com
         pcontrol_detector ctrl
         pointer det
         pfilter source_fir
         pfir_packet fir
         piirdef avg_source
         piirfilter avg_filt
         */
        jprintf( depth, "General Detector --------------- %s\n", 
                head->slipping ? "ON" : "OFF" );
        jprintf( depth, "Gen Last On -------------------- %s\n", 
                head->slipping ? "True" : "False" );
        jprintf( depth, "Data Written ------------------- %s\n", 
                head->slipping ? "True" : "False" );
        jprintf( depth, "SCD_xxx flags (event) ---------- 0x%02lx\n", 
                (unsigned long)head->scd_evt );
        jprintf( depth, "SCD_xxx flags (cont) ----------- 0x%02lx\n", 
                (unsigned long)head->scd_cont );
        jprintf( depth, "Pre Event Buffers -------------- %d\n", 
                head->pre_event_buffers );
        jprintf( depth, "Processed Stream --------------- %f\n", 
                head->processed_stream );
        jprintf( depth, "Control Detector Name ---------- %s\n", 
                head->control_detector_name );
        jprintf( depth, "Detections this Session -------- %li\n", 
                head->detections_session );
        jprintf( depth, "Calibrations this Session ------ %li\n", 
                head->calibrations_session );
        jprintf( depth, "General Next to Send ----------- %lu\n", 
                head->gen_next );
        /* TODO: 
         piirfilter stream_iir
         pavg_packet avg
         */
        jprintf( depth, "OMF_xxx Bits ------------------- 0x%04lx\n", 
                (unsigned long)head->mini_filter );
        /* TODO: 
         tarc arc
         */

        jprintf( depth, "FIR Delay (including digitizer delay) ------- %f\n", 
                head->delay );
        jprintf( depth, "Records Generated During this Session ------- %li\n", 
                head->records_generated_session );
        jprintf( depth, "Last Record Generated (seconds since 2000) -- %lu\n", 
                head->last_record_generated );
        jprintf( depth, "Delay after cal. over to turn off "
                        "detection - %d seconds\n", 
                (unsigned short)head->caldly );
        jprintf( depth, "Count-up timer for turing off "
                        "detect flag --- %d seconds\n", 
                (unsigned short)head->calinc );


        /* TODO: finish dumping this data structure */
        printf( "\n" );

        if (dispatch) {
            head = head->dispatch_link;
        } else {
            head = head->link;
        }
    }
}

void print_dss( tdss *dss, size_t depth ) {
    if ( !dss ) {
        jprintf( depth, "EMPTY\n" );
        return;
    }
    jprintf( depth, "Password (High Prio.) - %8s\n",
            dss->high_pass);
    jprintf( depth, "Password (Mid Prio.) - %8s\n",
            dss->high_pass);
    jprintf( depth, "Password (Low Prio.) - %8s\n",
            dss->high_pass);
    jprintf( depth, "Timeout -------------- %d\n",
            (long)dss->timeout);
    jprintf( depth, "Max bandwidth -------- %d bps\n",
            (long)dss->max_bps);
    jprintf( depth, "Verbosity ------------ %d\n",
            (unsigned short)dss->verbosity);
    jprintf( depth, "Max CPU Percentage --- %d%%\n",
            (unsigned short)dss->max_cpu_perc);
    jprintf( depth, "Port Number ---------- %d\n",
            (unsigned short)dss->port_number);
    jprintf( depth, "Max Memory ----------- %d kB\n",
            (unsigned short)dss->max_mem);
    jprintf( depth, "Reserved ------------- %d kB\n",
            (unsigned short)dss->reserved);
}

void print_tiirdef_list( tiirdef *head, size_t depth ) {
    int j,k;

    if ( !head ) {
        jprintf( depth, "EMPTY\n" );
        return;
    }

    while( head ) {
        jprintf( depth, "Sects --------- %d\n",
                (unsigned short)head->sects );
        jprintf( depth, "Filter Number - %d\n",
                (unsigned short)head->iir_num );
        jprintf( depth, "Gain ---------- %d\n",
                (unsigned short)head->gain );
        jprintf( depth, "Rate ---------- %d\n",
                (unsigned short)head->rate );
        jprintf( depth, "Filter Name --- %s\n",
                head->fname );
        jprintf( depth, "Filter Info:\n" );
        for (j = 0; j <= MAXSECTIONS; j++) {
            jprintf( depth + 1, "Poles ---- %d\n",
                    (unsigned short)head->filt[j].poles );
            jprintf( depth + 1, "Highpass - %s\n",
                    head->filt[j].highpass ? "true" : "false" );
            jprintf( depth + 1, "Spare ---- %d\n",
                    (unsigned short)head->filt[j].spare );
            jprintf( depth + 1, "Ratio ---- %f\n",
                    (float)head->filt[j].ratio );
            jprintf( depth + 1, "Vectors [A]:" );
            for (k = 0; k <= MAXPOLES; k++) {
                printf( " %f", ((double *)head->filt[j].a)[k] );
            }
            printf( "\n");
            jprintf( depth + 1, "Vectors [B]:" );
            for (k = 0; k <= MAXPOLES; k++) {
                printf( " %f", ((double *)head->filt[j].b)[k] );
            }
            printf( "\n");
            printf( "\n");
        }

        head = head->link;
    }
}

void print_detector_list( tdetector *head, size_t depth ) {
    int j,k;

    if ( !head ) {
        jprintf( depth, "EMPTY\n" );
        return;
    }

    while ( head ) {
        jprintf( depth, "Detector Pre-Filter:\n" );
        print_tiirdef_list( head->detfilt, depth + 1 );
        jprintf( depth, "Detector Parameters:\n" );
        print_tdetload( &head->uconst, depth + 1 );

        jprintf( depth, "Detector Number - %d\n",
                (unsigned short)head->detector_num);
        jprintf( depth, "Detector Type --- %s\n",
                (head->dtype == MURDOCK_HUTT) ? "Murdock-Hutt" : "Threshold" );
        jprintf( depth, "Detector Name --- %s\n",
                head->detname );

        head = head->link;
    }
}

void print_cdetector_list( tcontrol_detector *head, size_t depth ) {
    int j,k;

    if ( !head ) {
        jprintf( depth, "EMPTY\n" );
        return;
    }

    while ( head ) {
        jprintf( depth, "Execution Equations:\n" );
        print_tdetector_operation_list( head->pdetop, depth + 1 );
        jprintf( depth, "Token List:\n" );
        print_tdop_list( head->token_list, depth + 1 );
        jprintf( depth, "Send Message to auxout on change - %s\n",
                head->logmsg ? "True" : "False" );
        jprintf( depth, "Current Status ------------------- %s\n", 
                head->ison ? "ON" : "OFF" );
        jprintf( depth, "Previous Status ------------------ %s\n", 
                head->wason ? "ON" : "OFF" );
        jprintf( depth, "Control Detector Number ---------- %d\n", 
                (unsigned short)head->ctrl_num );
        jprintf( depth, "Control Detector Name ------------ %s\n", 
                head->cdname );

        head = head->link;
    }
}

void print_tdop_list( tdop *head, size_t depth ) 
{
    int j,k;

    if ( !head ) {
        jprintf( depth, "EMPTY\n" );
        return;
    }

    while ( head ) {
        jprintf( depth, "Pointer - 0x%08lx\n", 
                head->point );
        jprintf( depth, "Token --- %d\n", 
                (unsigned short)head->tok );

        head = head->link;
    }
}

void print_tdetector_operation_list ( tdetector_operation *head, size_t depth)
{
    if ( !head ) {   
        jprintf( depth, "EMPTY\n" );
        return;
    }

    while ( head ) {
        jprintf( depth, "Operation - %d\n",
                 (unsigned short)head->op );
        jprintf( depth, "Temp Num -- %d\n",
                 (long)head->temp_num );
        jprintf( depth, "To SPT ---- %s\n",
                 (long)head->tospt ? "true" : "false" );
        jprintf( depth, "No SPT ---- %s\n",
                 (long)head->nospt ? "true" : "false" );

        head = head->link;
    }
}

void print_tdetload( tdetload *detector, size_t depth ) 
{

    if ( !detector ) {
        jprintf( depth, "EMPTY\n" );
        return;
    }

    jprintf( depth, "Threshold Limits -------- High=%d  Low=%d\n",
            detector->filhi, detector->fillo );
    jprintf( depth, "Windows Length ---------- %d\n",
            detector->iwin );
    jprintf( depth, "Hits -------------------- %d\n",
            detector->n_hits );
    jprintf( depth, "Coded Threshold Factors - xth1=%d xth2=%d xth3=%d xthx=%d\n",
            detector->xth1, detector->xth2, detector->xth3, detector->xthx );
    jprintf( depth, "Time Correction --------- %d\n",
            detector->def_tc );
    jprintf( depth, "Wait -------------------- %d\n",
            detector->wait_blk );
    jprintf( depth, "Num Values in tsstak ---- %d\n",
            detector->val_avg );
}

void print_tfilter_list( tfilter *head, size_t depth ) {
    int j;

    if ( !head ) {   
        jprintf( depth, "EMPTY\n" );
        return;
    }

    while ( head ) {
        jprintf( depth, "Filter Name ------ %s\n",
                 head->fname );
        jprintf( depth, "Filter Number ---- %d\n",
                 head->fir_num );
        jprintf( depth, "Coefficients:\n" );
        for (j = 0; j < FIRMAXSIZE; j++) {
            jprintf( depth + 1, "[%d] %f\n",
                     j, head->coef[j] );
        }
        jprintf( depth, "Filter Length ----- %d\n",
                 head->len );
        jprintf( depth, "Gain Factor ------- %f\n",
                 head->gain );
        jprintf( depth, "Delay in Samples -- %f\n",
                 head->dly );
        jprintf( depth, "Decimation Factor - %d\n",
                 head->dec );
        printf( "\n" );

        head = head->link;
    }
}

void print_mholdqtype_list( mholdqtype *head, size_t depth ) {
    bool has_loop = false;
    mholdqtype *head1, *head2;

    if ( !head ) {   
        jprintf( depth, "EMPTY\n" );
        return;
    }

    head1 = head;
    while ( head1 ) {
        head1 = head1->link;
        if (head1 == head) {
            has_loop = true;
            break;
        }
    }

    if ( !has_loop ) {
        head1 = head2 = head;
        while ( head1 && head2 ) {
            head1 = head1->link;
            head2 = head2->link->link;
            if ( head1 == head2 ) {
                fprintf( stderr, "LOOP IN LINKED LIST!! "
                                 "Matched on 0x%08lx\n",
                                 (unsigned long)head1 );
                return;
            }
        }
    }

    head1 = head;
    while ( head ) {
        if ( strlen( head->m ) ) {
            jprintf( depth, "%s\n", head->m );
        }

        head = head->link;
        if ( head == head1 )
            break;
    }
}

void print_tmdispatch( tmdispatch *disp, size_t depth ) {
    int j,k; 
    jprintf( depth, " Channel | Frequencies\n" );
    jprintf( depth, "---------|----------------\n" );
    for (j = 0; j < CHANNELS; j++) {
        jprintf( depth, "% 8d |", j );
        for (j = 0; j < FREQUENCIES; j++) {
            printf( " %f", disp[j][k] );
        }
        printf( "\n" );
    }
}

void print_tcompressed_buffer_ring_list( tcompressed_buffer_ring *head, size_t depth ) {

    if ( !head ) {   
        jprintf( depth, "EMPTY\n" );
        return;
    }

    while ( head ) {
        jprintf( depth, "SEED Header ------ 0x%08lx\n", 
                 (unsigned long)&head->hdr_buf );
        jprintf( depth, "Completed Record - 0x%08lx\n",
                 (unsigned long)head->rec );
        jprintf( depth, "Record Full ------ %s\n", 
                 head->full ? "true" : "false" );

        head = head->link;
    }
}

void print_tdispatch( tdispatch *disp, size_t depth ) {
    int j;

    for (j = 0; j < 96; j++) {
        jprintf( depth, "LCQ %d:\n", j );
        print_lcq_list( (*disp)[j], true /*dispatch*/, depth + 1 );
    }
}
