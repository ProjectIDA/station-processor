package uforsplot;

public class SeedRawRecord
{
	// Pass in a miniseed header record to generate space for full record
	SeedRawRecord(byte frame[])
	{
		record = new byte[DEFAULT_SEED_RECORD_SIZE];
		System.arraycopy(frame, 0, record, 0, record.length);
	} // constructor SeedRawRecord()
	
  public byte record[];
  
  public final int DEFAULT_SEED_RECORD_SIZE = 512;
  
  byte [] GetRecord()
  {
  	return record;
  }
  
} // class SeedRawRecord
