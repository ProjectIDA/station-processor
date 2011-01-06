#pragma ident "$Id: status.h,v 1.2 2007/12/20 23:07:21 dechavez Exp $"
/*======================================================================
 *
 * Q330 Status
 *
 *====================================================================*/
#ifndef qdp_status_h_included
#define qdp_status_h_included

#include "platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/* QDP_C1_STAT status bitmap */

#define QDP_SRB_GBL   0x00000001 /* bit  0 = Global Status */
#define QDP_SRB_GPS   0x00000002 /* bit  1 = GPS Status */
#define QDP_SRB_PWR   0x00000004 /* bit  2 = Power supply Status */
#define QDP_SRB_BOOM  0x00000008 /* bit  3 = Boom positions ... */
#define QDP_SRB_THR   0x00000010 /* bit  4 = Thread Status */
#define QDP_SRB_PLL   0x00000020 /* bit  5 = PLL Status */
#define QDP_SRB_GSV   0x00000040 /* bit  6 = GPS Satellites */
#define QDP_SRB_ARP   0x00000080 /* bit  7 = ARP Status */
#define QDP_SRB_DP1   0x00000100 /* bit  8 = Data Port 1 Status */
#define QDP_SRB_DP2   0x00000200 /* bit  9 = Data Port 2 Status */
#define QDP_SRB_DP3   0x00000400 /* bit 10 = Data Port 3 Status */
#define QDP_SRB_DP4   0x00000800 /* bit 11 = Data Port 4 Status */
#define QDP_SRB_SP1   0x00001000 /* bit 12 = Serial Port 1 Status */
#define QDP_SRB_SP2   0x00002000 /* bit 13 = Serial Port 2 Status */
#define QDP_SRB_SP3   0x00004000 /* bit 14 = Serial Port 3 Status */
#define QDP_SRB_ETH   0x00008000 /* bit 15 = Ethernet Status */
#define QDP_SRB_BALER 0x00010000 /* bit 16 = Baler Status */
#define QDP_SRB_DYN   0x00020000 /* bit 17 = Dynamic IP Address */
#define QDP_SRB_AUX   0x00040000 /* bit 18 = Aux Board Status */
#define QDP_SRB_SS    0x00080000 /* bit 19 = Serial Sensor Status */
#define QDP_SRB_TOKEN 0x20000000 /* bit 29 = DP Tokens have changed */
#define QDP_SRB_LCHG  0x40000000 /* bit 30 = Logical Port programming change */
#define QDP_SRB_UMSG  0x80000000 /* bit 31 = User Message */

/* Global status */

typedef struct {
    UINT16 aqctrl;          /* Acquisition Control */
    UINT16 clock_qual;      /* Current clock quality */
    UINT16 clock_loss;      /* Minutes since lock */
    UINT16 current_voltage; /* Current AMB DAC control value */
    UINT32 sec_offset;      /* Seconds offset + data seq + 2000 = time */
    UINT32 usec_offset;     /* Usec offset for data */
    UINT32 total_time;      /* Total time in seconds */
    UINT32 power_time;      /* Total power on time in seconds */
    UINT32 last_resync;     /* Time of last resync */
    UINT32 resyncs;         /* Total number of resyncs */
    UINT16 gps_stat;        /* Gps status */
    UINT16 cal_stat;        /* Calibrator Status */
    UINT16 sensor_map;      /* Sensor control bitmap */
    UINT16 cur_vco;         /* Current VCO Value */
    UINT16 data_seq;        /* Data sequence number */
    UINT16 pll_flag;        /* PLL enabled */
    UINT16 stat_inp;        /* Status Inputs */
    UINT16 misc_inp;        /* Misc. Input */
    UINT32 cur_sequence ;   /* latest digitizer sequence */
} QDP_STAT_GBL;

/* GPS status */

typedef struct {
    UINT16 gpstime;   /* GPS Power on/off time in seconds */
    UINT16 gpson;     /* GPS power on if non-zero */
    UINT16 sat_used;  /* Number of satellite used */
    UINT16 sat_view;  /* Number of satellites in view */
    char time[10];    /* GPS Time */
    char date[12];    /* GPS Date */
    char fix[6];      /* GPS Fix type */
    char height[12];  /* GPS Height */
    char lat[14];     /* GPS Latitude */
    char lon[14];     /* GPS Longitude */
    UINT32 last_good; /* Time of last good 1PPS */
    UINT32 check_err; /* Number of checksum errors */
} QDP_STAT_GPS;

/* Power supply status */

typedef struct {
    UINT16 phase;      /* Charging Phase */
    UINT16 battemp;    /* Battery temperature */
    UINT16 capacity;   /* Battery capacity */
    UINT16 depth;      /* Depth of discharge */
    UINT16 batvolt;    /* Battery Voltage */
    UINT16 inpvolt;    /* Input Voltage */
    UINT16 batcur;     /* Battery current */
    UINT16 absorption; /* Absorption setpoint */
    UINT16 floatsp;    /* float setpoint */
    UINT8 alerts;      /* bitmap of loads about to be turned off */
    UINT8 loads_off;   /* bitmap of loads currently on */
} QDP_STAT_PWR;

/* Boom positions */

typedef struct {
    INT16 pos[QDP_NCHAN]; /* boom positions */
    UINT16 amb_pos;       /* analog mother board positive - 10mv */
    UINT16 amb_neg;       /* analog mother board negative - 10mv */
    UINT16 supply;        /* input voltage - 150mv */
    INT16 sys_temp;       /* system temperature - celsius */
    UINT16 main_cur;      /* main current - 1ma */
    UINT16 ant_cur;       /* GPS antenna current - 1ma */
    INT16 seis1_temp;     /* seismo 1 temperature - celsius */
    INT16 seis2_temp;     /* seimso 2 temperature - celsius */
    UINT32 cal_timeouts;  /* calibrator timeouts */
} QDP_STAT_BOOM;

/* Thread status */

typedef struct {
    UINT64 runtime; /* total run time in ms */
    UINT64 since;   /* time since last run in ms */
    UINT8 priority; /* priority */
    UINT8 counter;  /* priority counter */
    UINT16 flags;   /* flags */
} QDP_STAT_TASK;

typedef struct {
    UINT64 stime;        /* total system time in ms */
    UINT16 nentry;       /* number of entries to follow */
    QDP_STAT_TASK *task; /* per-thread status */
} QDP_STAT_THR;

/* PLL status */

typedef struct {
    struct {
        REAL32 initial;  /* initial VCO */
        REAL32 rms;      /* RMS VCO */
        REAL32 best;     /* Best VCO */
    } vco;
    REAL32 time_error;
    UINT32 ticks_track_lock; /* ticks since last track or lock */
    INT16 km;
    UINT16 state;            /* hold/track/lock */
} QDP_STAT_PLL;

/* GPS satellites */

typedef struct {
    UINT16 num;       /* satellite number */
    INT16 elevation; /* elevation in meters */
    INT16 azimuth;   /* azimuth in degrees */
    UINT16 snr;      /* signal to noise ratio */
} QDP_STAT_SATL;

typedef struct {
    UINT16 nentry;       /* number of entries to follow */
    QDP_STAT_SATL *satl; /* per-satellite status */
} QDP_STAT_GSV;

/* ARP status */

typedef struct {
    UINT32 ip;      /* IP address */
    UINT8  mac[6];  /* MAC address */
    UINT16 timeout; /* timeout in seconds */
} QDP_STAT_ARPADDR;

typedef struct {
    UINT16 nentry;          /* number of entries to follow */
    QDP_STAT_ARPADDR *addr; /* per-mac address data */
} QDP_STAT_ARP;

/* Logical port status */

typedef struct {
    UINT32 sent;      /* Total Data Packets Sent */
    UINT32 resends;   /* Total Packets re-sent */
    UINT32 fill;      /* Total Fill Packets sent */
    UINT32 seq;       /* Receive Sequence errors */
    UINT32 pack_used; /* Bytes of packet buffer used */
    UINT32 last_ack;  /* time of last packet acked */
    UINT16 phy_num;   /* physical port number used */
    UINT16 log_num;   /* logical port we are reporting */
    UINT16 retran;    /* retransmission timer */
    UINT16 flags;     /* LPSF_xxx flags */
} QDP_STAT_DPORT;

/* Serial port status */

typedef struct {
    UINT32 check;    /* Receive Checksum errors */
    UINT32 ioerrs;   /* Total I/O errors */
    UINT16 phy_num;  /* Physical port we are reporting */
    UINT32 unreach;  /* Destination Unreachable ICMP Packets Received */
    UINT32 quench;   /* Source Quench ICMP Packets Received */
    UINT32 echo;     /* Echo Request ICMP Packets Received */
    UINT32 redirect; /* Redirection Packets Received */
    UINT32 over;     /* Total overrun errors */
    UINT32 frame;    /* Total framing errors */
} QDP_STAT_SPORT;

/* Ethernet status */

typedef struct {
    UINT32 check;    /* Receive Checksum errors */
    UINT32 ioerrs;   /* Total I/O errors */
    UINT16 phy_num;  /* Physical port we are reporting */
    UINT32 unreach;  /* Destination Unreachable ICMP Packets Received */
    UINT32 quench;   /* Source Quench ICMP Packets Received */
    UINT32 echo;     /* Echo Request ICMP Packets Received */
    UINT32 redirect; /* Redirection Packets Received */
    UINT32 runt;     /* Total runt frames */
    UINT32 crc_err;  /* CRC errors */
    UINT32 bcast;    /* Broadcast frames */
    UINT32 ucast;    /* Unicast frames */
    UINT32 good;     /* Good frames */
    UINT32 jabber;   /* Jabber errors */
    UINT32 outwin;   /* Out the window */
    UINT32 txok;     /* Transmit OK */
    UINT32 miss;     /* Receive packets missed */
    UINT32 collide;  /* Transmit collisions */
    UINT16 linkstat; /* Link status */
} QDP_STAT_ETH;

/* Baler status */

typedef struct {
    UINT32 last_on;      /* Time last turned on */
    UINT32 powerups;     /* Total number of power ups since reset */
    UINT16 baler_status; /* BS_XXXX and timeouts */
    UINT16 baler_time;   /* minutes since baler was activated */
} QDP_STAT_BALER;

/* Dynamic IP addresses */

typedef struct {
    UINT32 serial[QDP_NSERIAL]; /* serial IP addresses */
    UINT32 ethernet;  /* ethernet IP address */
} QDP_STAT_DYN;

/* Aux board status */

typedef struct {
    UINT16 size;     /* Size in bytes */
    UINT16 packver;  /* Version */
    UINT16 aux_type; /* Aux. Type */
    UINT16 aux_ver;  /* Aux. Version */
} QDP_STAT_AUXHDR;

typedef struct {
    QDP_STAT_AUXHDR hdr;
#define QDP_STAT_AUXAD_MAXCONVERT 8
    INT32 conversion[QDP_STAT_AUXAD_MAXCONVERT];
} QDP_STAT_AUX;

/* Serial sensor status */

#define QDP_STAT_SS_TYPE_PARO 1

typedef struct {
    UINT16 port;    /* serial interface index */
    UINT16 sps;     /* seconds per sample */
    UINT16 units;   /* see Paroscientific manual */
    UINT16 inttime; /* integration time in ms */
    UINT16 frac;    /* fractional digits */
    struct {
        BOOL valid;  /* TRUE if value field contains valid  data */
        INT32 value; /* measurement */
    } pressure, temperature;
} QDP_STAT_PARO;

typedef struct {
    UINT16 type;
    QDP_STAT_PARO paro; /* for sensor type = QDP_STAT_SS_TYPE_PARO */
} QDP_STAT_SS;

/* Unsoliciated alerts */

#define QDP_STAT_USRMSG_MAXLEN 79

typedef struct {
    struct {
        UINT32 ip;
        char txt[QDP_STAT_USRMSG_MAXLEN+1]; /* the message */
    } usrmsg;
    BOOL port;    /* data port programming has changed */
    BOOL tokens;  /* DP tokens have changed */
} QDP_STAT_ALERT;

/* Complete status */

typedef struct {
    UINT32 bitmap;                     /* status bitmap */
    QDP_STAT_ALERT alert;              /* status alerts */
    QDP_STAT_GBL gbl;                  /* global status */
    QDP_STAT_GPS gps;                  /* GPS status */
    QDP_STAT_PWR pwr;                  /* power status */
    QDP_STAT_BOOM boom;                /* boom positions, and other stuff */
    QDP_STAT_THR thr;                  /* thread status */
    QDP_STAT_PLL pll;                  /* PLL status */
    QDP_STAT_GSV gsv;                  /* GPS satellites */
    QDP_STAT_ARP arp;                  /* ARP table */
    QDP_STAT_DPORT dport[QDP_NLP];     /* data port status */
    QDP_STAT_SPORT sport[QDP_NSERIAL]; /* serial port status */
    QDP_STAT_ETH eth;                  /* ethernet status */
    struct {
        QDP_STAT_BALER serial[2];      /* serial 1 and 2 Baler status */
        QDP_STAT_BALER special;        /* special Baler status */
        QDP_STAT_BALER ethernet;       /* ethernet Baler status */
    } baler;
    QDP_STAT_DYN dyn;                  /* dynamic IP addresses */
    QDP_STAT_AUX aux;                  /* auxiliary board status */
    QDP_STAT_SS ss[QDP_NSERIAL];       /* serial sensor status */
} QDP_TYPE_C1_STAT;

#ifdef __cplusplus
}
#endif

#endif /* qdp_status_h_included */

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2007 Regents of the University of California            |
 |                                                                       |
 | This software is provided 'as-is', without any express or implied     |
 | warranty.  In no event will the authors be held liable for any        |
 | damages arising from the use of this software.                        |
 |                                                                       |
 | Permission is granted to anyone to use this software for any purpose, |
 | including commercial applications, and to alter it and redistribute   |
 | it freely, subject to the following restrictions:                     |
 |                                                                       |
 | 1. The origin of this software must not be misrepresented; you must   |
 |    not claim that you wrote the original software. If you use this    |
 |    software in a product, an acknowledgment in the product            |
 |    documentation of the contribution by Project IDA, UCSD would be    |
 |    appreciated but is not required.                                   |
 | 2. Altered source versions must be plainly marked as such, and must   |
 |    not be misrepresented as being the original software.              |
 | 3. This notice may not be removed or altered from any source          |
 |    distribution.                                                      |
 |                                                                       |
 +-----------------------------------------------------------------------*/

/* Revision History
 *
 * $Log: status.h,v $
 * Revision 1.2  2007/12/20 23:07:21  dechavez
 * added IP address to user message
 *
 * Revision 1.1  2007/12/20 22:44:06  dechavez
 * created
 *
 */
