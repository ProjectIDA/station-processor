#pragma ident "$Id: ReleaseNotes.c,v 1.16 2009/05/14 18:13:55 dechavez Exp $"

char *VersionIdentString = "Release 1.4.2";

/* Release notes

1.4.2  03/12/2009
       Fixed bug in reporting calibration amplitude

1.4.1  02/23/2009
       Permit setting GPS parameters via internal/external option to
       gps cnf, and setting of operational limits (either default or
       specific values)

1.4.0  02/04/2009
       Added "checkout", "spp", "dcp", and "man" commands, and "gps"
       subcommands "id" and "cnf".  Added "reboot" option to "save"
       command.
       Use new libqdp qdp_C1_RQx commands to retrieve info from Q330..

1.3.1  01/26/2009
       Fixed bug setting pulse duration, changed default pulse width to
       one second, fixed bug specifying sensor b recentering, changed
       flag for recenter line from mass to mp. Added glob command.

1.3.0  01/23/2009
       Added amass, pulse, and sc commands

1.2.0  01/05/2009
       Extended ifconfig command to allow setting IP addresses
       Added save command for saving current configuration to eeprom

1.1.2  10/03/2008
       Allow specifying an arbitrary number of status selectors

1.1.1  10/03/2008
       Added gsv command, removed satellites from gps selection

1.1.0  10/02/2008
       Added ifconfig, status, and fixvar commands

1.0.2  03/08/2008
       Changed default calibration duration to hardware max (16383 sec)
       and silenty ensure that custom durations do not exceed this

1.0.1  01/05/2008
       Fixed bug reporting calibration frequency

1.0.0  12/20/2007
       Initial release
/*

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
