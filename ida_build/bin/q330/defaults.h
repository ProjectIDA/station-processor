#pragma ident "$Id: defaults.h,v 1.2 2009/02/23 22:13:06 dechavez Exp $"
#ifndef q330_defaults_included
#define q330_defaults_included

#define QDP_DEFAULT_C1_SPP_MAX_MAIN_CURRENT   350
#define QDP_DEFAULT_C1_SPP_MIN_OFF_TIME        60
#define QDP_DEFAULT_C1_SPP_MIN_PS_VOLTAGE      60
#define QDP_DEFAULT_C1_SPP_MAX_ANTENNA_CURRENT 34
#define QDP_DEFAULT_C1_SPP_MIN_TEMP           -40
#define QDP_DEFAULT_C1_SPP_MAX_TEMP            65
#define QDP_DEFAULT_C1_SPP_TEMP_HYSTERESIS      5
#define QDP_DEFAULT_C1_SPP_VOLD_HYSTERESIS     13
#define QDP_DEFAULT_C1_SPP_DEFAULT_VCO       2048

#define QDP_DEFAULT_TYPE_C1_SPP {           \
    QDP_DEFAULT_C1_SPP_MAX_MAIN_CURRENT,    \
    QDP_DEFAULT_C1_SPP_MIN_OFF_TIME,        \
    QDP_DEFAULT_C1_SPP_MIN_PS_VOLTAGE,      \
    QDP_DEFAULT_C1_SPP_MAX_ANTENNA_CURRENT, \
    QDP_DEFAULT_C1_SPP_MIN_TEMP,            \
    QDP_DEFAULT_C1_SPP_MAX_TEMP,            \
    QDP_DEFAULT_C1_SPP_TEMP_HYSTERESIS,     \
    QDP_DEFAULT_C1_SPP_VOLD_HYSTERESIS,     \
    QDP_DEFAULT_C1_SPP_DEFAULT_VCO          \
}

#define QDP_DEFAULT_C2_GPS_MODE_INTERNAL   1
#define QDP_DEFAULT_C2_GPS_MODE_EXTERNAL   8
#define QDP_DEFAULT_C2_GPS_FLAGS           0
#define QDP_DEFAULT_C2_GPS_OFF_TIME      180
#define QDP_DEFAULT_C2_GPS_RESYNC          0
#define QDP_DEFAULT_C2_GPS_MAX_ON        120
#define QDP_DEFAULT_C2_GPS_LOCK_USEC       5
#define QDP_DEFAULT_C2_GPS_INTERVAL       10
#define QDP_DEFAULT_C2_GPS_INITIAL_PLL     3
#define QDP_DEFAULT_C2_GPS_PFRAC         150.000
#define QDP_DEFAULT_C2_GPS_VCO_SLOPE      -0.175
#define QDP_DEFAULT_C2_GPS_VCO_INTERCEPT   7.110
#define QDP_DEFAULT_C2_GPS_MAX_IKM_RMS     0.250
#define QDP_DEFAULT_C2_GPS_IKM_WEIGHT      0.250
#define QDP_DEFAULT_C2_GPS_KM_WEIGHT       0.120
#define QDP_DEFAULT_C2_GPS_BEST_WEIGHT     0.100
#define QDP_DEFAULT_C2_GPS_KM_DELTA      600.000

#define QDP_DEFAULT_TYPE_C2_GPS_INTERNAL { \
    QDP_DEFAULT_C2_GPS_MODE_INTERNAL,      \
    QDP_DEFAULT_C2_GPS_FLAGS,              \
    QDP_DEFAULT_C2_GPS_OFF_TIME,           \
    QDP_DEFAULT_C2_GPS_RESYNC,             \
    QDP_DEFAULT_C2_GPS_MAX_ON,             \
    QDP_DEFAULT_C2_GPS_LOCK_USEC,          \
    QDP_DEFAULT_C2_GPS_INTERVAL,           \
    QDP_DEFAULT_C2_GPS_INITIAL_PLL,        \
    QDP_DEFAULT_C2_GPS_PFRAC,              \
    QDP_DEFAULT_C2_GPS_VCO_SLOPE,          \
    QDP_DEFAULT_C2_GPS_VCO_INTERCEPT,      \
    QDP_DEFAULT_C2_GPS_MAX_IKM_RMS,        \
    QDP_DEFAULT_C2_GPS_IKM_WEIGHT,         \
    QDP_DEFAULT_C2_GPS_KM_WEIGHT,          \
    QDP_DEFAULT_C2_GPS_BEST_WEIGHT,        \
    QDP_DEFAULT_C2_GPS_KM_DELTA            \
}

#define QDP_DEFAULT_TYPE_C2_GPS_EXTERNAL { \
    QDP_DEFAULT_C2_GPS_MODE_EXTERNAL,      \
    QDP_DEFAULT_C2_GPS_FLAGS,              \
    QDP_DEFAULT_C2_GPS_OFF_TIME,           \
    QDP_DEFAULT_C2_GPS_RESYNC,             \
    QDP_DEFAULT_C2_GPS_MAX_ON,             \
    QDP_DEFAULT_C2_GPS_LOCK_USEC,          \
    QDP_DEFAULT_C2_GPS_INTERVAL,           \
    QDP_DEFAULT_C2_GPS_INITIAL_PLL,        \
    QDP_DEFAULT_C2_GPS_PFRAC,              \
    QDP_DEFAULT_C2_GPS_VCO_SLOPE,          \
    QDP_DEFAULT_C2_GPS_VCO_INTERCEPT,      \
    QDP_DEFAULT_C2_GPS_MAX_IKM_RMS,        \
    QDP_DEFAULT_C2_GPS_IKM_WEIGHT,         \
    QDP_DEFAULT_C2_GPS_KM_WEIGHT,          \
    QDP_DEFAULT_C2_GPS_BEST_WEIGHT,        \
    QDP_DEFAULT_C2_GPS_KM_DELTA            \
}

#endif /* q330_defaults_included */

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2009 Regents of the University of California            |
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
 * $Log: defaults.h,v $
 * Revision 1.2  2009/02/23 22:13:06  dechavez
 * permit setting individual or default values
 *
 * Revision 1.1  2009/02/23 22:11:24  dechavez
 * initial release with QDP_DEFAULT_TYPE_C1_SPP, QDP_DEFAULT_TYPE_C2_GPS_INTERNAL,
 * and QDP_DEFAULT_TYPE_C2_GPS_EXTERNAL
 *
 */
