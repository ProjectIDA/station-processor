#pragma ident "$Id"

#define E$NoErr 0           /* No Error */
#define E$NoAspi -1         /* No Aspi Manager Installed */
#define E$Busy -2           /* Aspi Module is busy (this should never occur */
#define E$AspiErr -3        /* Undefined Aspi error */
#define E$NoMem -4          /* Can't allocate memory */
#define E$NoDevice -5       /* The device is not installed */
#define E$AspiInval -6      /* Invalid ASPI request */
#define E$NoAdapter -7      /* Invalid Host Adapter Number */
#define E$Abort -8          /* ASPI request aborted by Host */
#define E$SelTimeout -9     /* Selection Timeout */
#define E$DataOverrun -10   /* Data over-run/under-run */
#define E$BusFree -11       /* Unexpected Bus Free */
#define E$BusFail -12       /* Target bus phase sequence failure */
#define E$TargetBusy -13    /* The specified Target/LUN is busy */
#define E$Reservation -14   /* Reservation conflict */
#define E$NotReady -15      /* lun cannot be accessed */
#define E$Medium -16        /* Medium Error */
#define E$Hardware -17      /* Non recoverable hardware error */
#define E$IllegalReq -18    /* Illegal Request */
#define E$UnitAttention -19 /* Unit Attention */
#define E$DataProtect -20   /* The Block is protected */
#define E$BlankCheck -21    /* Encounterd non blank data */
#define E$TargetAbort -22   /* The Target Aborted the command */
#define E$VolOverflow -23   /* Volume overflow */
#define E$Miscompare -24    /* Source and data did nor match */
#define E$IOErr -25         /* An IO Error of unknown type */

#define E$LockLMem -26      /* Can not lock local memory */
#define E$LockGMem -27      /* Can not lock global memory */
#define E$Fatal    -28      /* Fatal Error - unemploy the programmer :-) */
#define E$NoChannel -29     /* The logical channel is not initialized */
#define E$NotActive -30     /* The logical channel is not active */
#define E$ChannelOverflow -31 /* Logical Channel overflow */
#define E$StreamerInUse -32 /* streamer is already configured for a channel */
#define E$BlockLimit -33    /* Illegal blocksize for a streamer */
#define E$Offset -34        /* Illegal offset allowed for this operation */
#define E$Position -35      /* Illegal position for this operarion */
#define E$BlockLen -36      /* Illegal block length for this operation */


#define E$EndOfMedium -37   /* End of Medium detected, not really an error */


int SCSIRead(int adapter, int target_id, int lun, BYTE *buf, int len);
int SCSIWrite(int adapter, int target_id, int lun, BYTE *buf, int len);
int SCSIRewind(int adapter, int target_id, int lun);
static int ScsiIoError(int aspi_status, int adapter_status, int target_status, BYTE *sense);
int SCSIReset(int adapter, int target_id, int lun);
BOOL InitASPI();
void CloseASPI();

/*$ Revision History
 *
 * $Log: scsitape.h,v $
 * Revision 1.1  2005/05/13 17:16:54  dechavez
 * AAP 05-11 update
 *
 * Revision 1.1  2005/02/15 15:15:28  dechavez
 * added
 *
 */
