/*  mergeopaque.c
 *
 *  Author: Frank Shelly
 *          fshelly@usgs.gov
 *
 *  Merges two seed files containing opaque seed records
 *  New records from the first file are added to second file
 *  Records are assumed to be presorted by time
 *  Any records with the same timetag are assumed to be duplicates
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <inttypes.h>


/////////////////////////////////////////////////////////////////////////////
void ShowUsage()
{

  fprintf(stderr,
"Usage:  mergeopaque <seed file> <output seed file> ]\n"
"   Records from the first seed file are appended to the output seed file\n"
"   if they contain later timestamps.\n"
  );
  return;
} // ShowUsage()

/////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
  FILE *SeedF1;
  FILE *SeedOut = stdout;

  char store_header[64];
  char seed_record[8192];
  char str_recnum[8];

  char *errstr;
  int  save_errno;
  int  iRecordNum=0;
  int  iRecordSize = 0;
  int  iSeek;
  int  iPower;

  int  iYearF1,iYearOut;
  int  iDoyF1,iDoyOut;
  int  iHourF1,iHourOut;
  int  iMinuteF1,iMinuteOut;
  int  iSecondF1,iSecondOut;
  int  iTMsecF1,iTMsecOut;

  if (argc != 3)
  {
    ShowUsage();
    exit(1);
  }

  if ((SeedF1=fopen(argv[1], "r")) == NULL)
  {
    save_errno = errno;
    errstr = strerror(save_errno);
    fprintf(stderr, "Unable to open first input seed file '%s'\n", argv[1]);
    fprintf(stderr, "ERRNO %d: %s\n", errno, errstr);
    exit(1);
  }

  if ((SeedOut=fopen(argv[2], "a+")) == NULL)
  {
    save_errno = errno;
    errstr = strerror(save_errno);
    fprintf(stderr, "Unable to create output seed file '%s'\n", argv[2]);
    fprintf(stderr, "ERRNO %d: %s\n", errno, errstr);
    exit(1);
  }

  // Find out what size seed records we are using
  fseek(SeedF1, 0, SEEK_SET);
  if (fread(store_header, 64, 1, SeedF1) != 1)
  {
    // Unable to read any data from the new file so no appending
    fprintf(stderr, "Unable to read data from %s, done!\n", argv[1]);
    exit(0);
  }
  fseek(SeedF1, 0, SEEK_SET);

  if (ntohs(*(short *)&store_header[48]) == 1000)
  {
    iPower = store_header[54];  
    iRecordSize = 1 << iPower;
    if (iRecordSize < 256 || iRecordSize > 8192)
    {
      fprintf(stderr, "Seed record sizes of %d are not supported\n",
              iRecordSize);
      exit(1);
    }
  }

  // Find the timestamp of the last record in destination file
  iSeek = ftell(SeedOut);
  iRecordNum = iSeek / iRecordSize;
  fseek(SeedOut, -iRecordSize, SEEK_END);
  if (fread(store_header, 64, 1, SeedOut) != 1)
  {
    // Output file appears empty so assume zero timegag
    iYearOut = 0;
    iDoyOut = 0;
    iHourOut = 0;
    iMinuteOut = 0;
    iSecondOut = 0;
    iTMsecOut = 0;
  }
  else
  {
    iYearOut = ntohs(*(short *)&store_header[20]);
    iDoyOut = ntohs(*(short *)&store_header[22]);
    iHourOut = store_header[23];
    iMinuteOut = store_header[24];
    iSecondOut = store_header[25];
    iTMsecOut = ntohs(*(short *)&store_header[26]);
  }
//fprintf(stderr, "DEBUG Oldest timetag = %d,%d,%02d:%02d:%02d.%04d\n",
//iYearOut, iDoyOut, iHourOut, iMinuteOut, iSecondOut, iTMsecOut);

  // Now loop through F1 records until we find one after the output timetag
  while (1)
  {
    if (fread(seed_record, iRecordSize, 1, SeedF1) != 1)
    {
      // Reached end of input file
//fprintf(stderr, "DEBUG reached end of input file with no new records\n");
      exit(0);
    }

    // See if this record is older than output file last record
    memcpy(store_header, seed_record, 64);
    iYearF1 = ntohs(*(short *)&store_header[20]);
    iDoyF1 = ntohs(*(short *)&store_header[22]);
    iHourF1 = store_header[23];
    iMinuteF1 = store_header[24];
    iSecondF1 = store_header[25];
    iTMsecF1 = ntohs(*(short *)&store_header[26]);

    if (iYearF1 < iYearOut) continue;
    if (iYearF1 > iYearOut) break;
    if (iDoyF1 < iDoyOut) continue;
    if (iDoyF1 > iDoyOut) break;
    if (iHourF1 < iHourOut) continue;
    if (iHourF1 > iHourOut) break;
    if (iMinuteF1 < iMinuteOut) continue;
    if (iMinuteF1 > iMinuteOut) break;
    if (iSecondF1 < iSecondOut) continue;
    if (iSecondF1 > iSecondOut) break;
    if (iTMsecF1 < iTMsecOut) continue;
    if (iTMsecF1 > iTMsecOut) break;
    
  } // loop until newer record found (or end of file)

  // Loop until we have appended the rest of the input file records
  fseek(SeedOut, 0, SEEK_END);
  while (1)
  {
    iRecordNum = (iRecordNum % 999999) + 1;
    sprintf(str_recnum, "%06d", iRecordNum);
    memcpy(seed_record, str_recnum, 6);
    if (fwrite(seed_record, iRecordSize, 1, SeedOut) != 1)
    {
      save_errno = errno;
      errstr = strerror(save_errno);
      fprintf(stderr, "Error appending record to output seed file '%s'\n",
              argv[2]);
      fprintf(stderr, "ERRNO %d: %s\n", errno, errstr);
      exit(1);
    }

    // Read the next record
    if (fread(seed_record, iRecordSize, 1, SeedF1) != 1)
    {
      break;
    }
  } // read until input file exhausted

  fclose(SeedF1);
  fclose(SeedOut);
  return 0;
} // main()

