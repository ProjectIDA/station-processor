#pragma ident "$Id: view.h,v 1.2 2002/04/30 16:24:23 dec Exp $"
#ifndef view_h_included
#define view_h_included

#include <sys/param.h>
#ifdef SCO_SV
#include <sys/socket.h>
#endif
#ifdef SOLARIS
#include <netdb.h>
#endif

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

typedef struct {
  String geometry;
  String inputfile;
  String home;
  String log;
  String sort;
  String debug;
  String decim;
  String timeout;
  String keep_up;
  String keeparg;
  String beg;
  String end;
  String compress;
  String port;
  String ygain;
  Boolean help;
  Boolean confirmExit;
  int plotWidth;
  int plotHeight;
  String inputEncoding;
  String outputEncoding;
  String textLang;
  String helpPath;
  String helpFile;
} AppData;


typedef struct {
  char *home;
  char *log;
  char *syscode;
  char *sort;
  double beg;
  double end;
  int timeout;
  int keep_up;
  int compress;
  int port;
  int debug;
  char keeparg[16][80];
  int decim;
  float ygain;
  int nchn;
  char cname[OLD_XFER_MAXCHN][NRTS_CNAMLEN+1];
  char input[16][1024];
  int nsys;
} User_info;



typedef struct {
    int chan;
    int sys;
} Index;

typedef struct {
  int p, p_x;
  double p_time;
  int s, s_x;
  double s_time;
} Phases;

typedef struct {
    float  value;
    Widget arrows_label;
    Index index;
} Ygain;

typedef struct {
    Widget plot_form;
    Widget draw_form;
    Widget label;
    Widget drawing_a;
    Pixmap pixmap;
    Pixmap *pixmap_ptr;
    Widget time_form;
    Widget beg_time_w;
    Widget end_time_w;
    Widget max_min_w;
    Widget event_w;
    Widget popup_menu;
    Widget erase_menu;
    Widget filter_menu;
    Widget arrows_form;
    Widget arrow_up;
    Widget arrow_down;
    struct nrts_packet *packet;
    int p_nsamp;
    int packet_index;
    XPoint *array;
    XSegment *segs;
    char label_string[20];
    int selected;
    long *long_value;
    Ygain ygain;
    int s;
    int calculate;
    double beg;
    double end;
    float sint;
    long seconds;
    int index;
    int decim;
    int to_save;
    float scale;
    long max_value;
    long min_value;
    float mean_value;
    float ytp;
    int points;
    int first_point;
    Phases phases;
    Dimension width, height;
    Dimension prev_width;
} Plot_stuff;

    
typedef struct {
  struct {
    Widget widget;
    Index index;
  } plot[16*OLD_XFER_MAXCHN];
  int nplots;
} Layout;

typedef struct {
  int keeparg;
  int times;
  int keep_up;
  int timeout;
  int compress;
  int port;
} DefaultSet;

typedef struct { 
    struct nrts_tapinfo info;
    struct nrts_sys *sys;
    char sysname[MAXHOSTNAMELEN];
    int nchn;
    char *begstr;
    char *endstr;
    char *keeparg;
    char *timeout;
    char *keep_up;
    char *port;
    char cname[OLD_XFER_MAXCHN][NRTS_CNAMLEN+1];
    DefaultSet defaultset;
    int sd;
    XtInputId InputId;
    int pause;
    int status;
    int connected;
    time_t time;
} Sys_stuff;

typedef struct {
  Widget toplevel;
  Widget top_comm_menu_bar;
  Widget top_main_w;
  Widget top_file_menu;
  Widget top_file_menu_btns[3];
  Widget options_menu;
  Widget options_menu_btns[5];
  Widget list_menu;
  Widget list_menu_btns[5];
  Widget top_help_menu;
  Widget top_help_menu_btns[5];
  Widget syslist_w;
  Widget add_w;
  Widget viewer_scrolled_w;
  Widget main_w;
  Widget comm_menu_bar;
  Widget file_menu;
  Widget file_menu_btns[5];
  Widget snapshot_menu;
  Widget snapshot_menu_btns[5];
  Widget help_menu;
  Widget help_menu_btns[5];
  Widget view_menu;
  Widget view_menu_btns[5];
  Widget sort_menu;
} MenuStuff;

typedef struct {
  XtAppContext app;
  AppData *app_data;
  MenuStuff menu_stuff;
  User_info *userinfo;
  Index index;
  Sys_stuff sys_stuff[16];
  Plot_stuff plot_stuff[16][OLD_XFER_MAXCHN];
  /*
  Layout layout;
  Layout new_layout;
  Widget origin_w_list[100];
  Cardunal num_orig_w;
  */
} GatherStuff;

typedef struct {
  int rows;
  int columns;
  String *help_text;
  String chapter;
  String title;
  String help_path;
  String help_file;
} HelpInfo;

void init(), help();
int Nrts_tapinit();
int Nrts_tap_connect();
int Nrts_packettocss();
int Nrts_packettosac();
int Nrts_tocss_end();
int Nrts_tosac_end();
int Nrts_packettogse();
int Nrts_togse_end();
void draw(), redraw(), set_color(), quit(), clear_it(), draw_cb();
void top_file_menu_cb();
void file_menu_cb(), help_menu_cb();
void arrow_up_cb(), arrow_down_cb(), die();
void handle_input(), resize(), input();
void redraw_plot(), view_menu_cb(), sort_menu_cb();
void popup_cb(), erase_cb(), toggled();
void input1(), snapshot_menu_cb();
void write_to_any();
void read_value(), help_cb();
void dialog_done(), map_dialog();
void menu_create(), wcreate(), create_gc(), add_cbs();


#endif

/* Revision History
 *
 * $Log: view.h,v $
 * Revision 1.2  2002/04/30 16:24:23  dec
 * fixed string lengths
 *
 * Revision 1.1.1.1  2000/02/08 20:20:15  dec
 * import existing IDA/NRTS sources
 *
 */
