//////////////////////////////////////////////////////////////////////////////
// NextTr1Station()
// int bFirst  True if you want to start at the beginning
// returns next <network_station> directory string from /tr1/telemetry_days/
//////////////////////////////////////////////////////////////////////////////
char *NextTr1Station(int bFirst);

//////////////////////////////////////////////////////////////////////////////
// NextTr1StationYear()
// int bFirst  True if you want to start at the beginning
// returns next <network_station> directory string from /tr1/telemetry_days/
//////////////////////////////////////////////////////////////////////////////
char *NextTr1StationYearDay(
  int bFirst,                    // True if first time opening this directory
  const char *network_station,   // Station name
  int year,                      // Year to look at
  int doy,                       // Day of year to look at
  char **seed_dir                // returns full path name of directory
  ); // Return pointer to the name of the next file in the directory

