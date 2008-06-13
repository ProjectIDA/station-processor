
//////////////////////////////////////////////////////////////////////////////
// Routine that processes data request
int TransferData(
  const char  *rq_station,   // station name
  const char  *rq_chan,      // Channel ID
  const char  *rq_loc,       // Location ID
  STDTIME2    rq_tBeginTime, // Start time
  int         rq_iSeconds    // Seconds of data desired
  )                          // Returns the number of records actually sent
{
  int   indexFirst;
  int   indexLast;
  int   iCount;
  int   iLoopSize;
  int   i;
  int   iSeek;
  int   iRecord;
  STDTIME2    rq_tEndTime;
  char  str_header[FRAME_SIZE];
  char  buf_filename[2*MAXCONFIGLINELEN+2];
  char  *errmsg;
  FILE  *fp_buf;

  // Get ending time of data request
  rq_tEndTime = ST_AddToTime2(rq_tBeginTime, 0, 0, 0, rq_iSeconds, 0);

  // Get index of first and last record for this data request
  errmsg = GetRecordRange(rq_station, rq_chan, rq_loc, 
             rq_tBeginTime, rq_tEndTime,
             &indexFirst, &indexLast, &iCount, &iLoopSize);
  if (errmsg != NULL)
  {
    fprintf(stderr, "%s\n", errmsg);
    return 0;
  }

  // Make sure there are data records to return
  if (iCount < 1)
    return 0;

  // Get name of buffer file
  // If blank location code, leave off leading location code in filename
  if (rq_loc[0] == ' ' || rq_loc[0] == 0)
  {
    sprintf(buf_filename, "%s/%s/%s.buf",
        loopDir, rq_station, rq_chan);
  }
  else
  {
    sprintf(buf_filename, "%s/%s/%s_%s.buf",
        loopDir, rq_station, rq_loc, rq_chan);
  }

  // Open the buffer file
  if ((fp_buf=fopen(buf_filename, "r")) == NULL)
  {
    // Buffer file does not exist so no records to return
    return 0;
  }

  // Loop through all data records
  for (i=0; i <= iCount; i++)
  {
    iRecord = (i + indexFirst) % iLoopSize;
    iSeek = iRecord * iLoopRecordSize;

    // Seek to the record position
    fseek(fp_buf, iSeek, SEEK_SET);
    if (iSeek != ftell(fp_buf))
    {
      // If seek failed, we hit end of file, set iHigh
      sprintf(looperrstr, "TransferData: Unable to seek to %d in %s",
              iSeek, buf_filename);
      fclose(fp_buf);
      return i;
    } // Failed to seek to required file buffer position

    // Read in the header only
    if (fread(str_header, FRAME_SIZE, 1, fp_buf) != 1)
    {
      sprintf(looperrstr, "TransferData: Unable to read record %d in %s",
              iRecord, buf_filename);
      fclose(fp_buf);
      return i;
    } // Failed to read record

    // Transfer the data to the requester
    printf("TODO data transfer in TransferData\n");
/*
    if (!xxx)
    {
      // Connection has closed before all of the data was sent
      sprintf(looperrstr, "TransferData: Connection closed on record %d",
              iRecord);
      fclose(fp_buf);
      return i;
      return i;
    }

    netdatasendroutine(str_header);
*/
  } // loop through each record index

  // Close buffer file
  fclose(fp_buf);

  return iCount;
} // TransferData()
