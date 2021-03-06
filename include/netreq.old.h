/* Include file for the NETREQ program */

#define SEEDRECSIZE 4096
#define FRAME_SIZE 64

typedef struct {
  short yr ;
  short jday ;
  char hr ;
  char minute ;
  char seconds ;
  char unused ;
  short tenth_millisec ;
} tseed_time ;

typedef struct {
  char sequence[6] ;                   /* record number */
  char seed_record_type ;                     /* D for data record */
  char continuation_record ;                  /* space normally */
  char station_id_call_letters[5] ;         /* last char must be space */
  char location_id[2] ;                     /* non aligned! */
  char channel_id[3] ;                     /* non aligned! */
  char seednet[2] ;                         /* space filled */
  tseed_time starting_time ;
  short samples_in_record ;
  short sample_rate_factor ;
  short sample_rate_multiplier ;              /* always 1 */
  char activity_flags ;                       /* ?I?LEBTC */
  char io_flags ;                             /* ??L????? */
  char data_quality_flags ;                   /* ???G???? */
  char number_of_following_blockettes ;       /* normally 2 for data */
  int tenth_msec_correction ;             /* always 0 */
  short first_data_byte ;                      /* 0, 56, or 64 - data starts in frame 1 */
  short first_blockette_byte ;                 /* 48 */
} seed_header ;


int client_connected;

