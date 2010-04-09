Documents changes to operational software and scripts for q330 ASP processor

README Updated:  May 6, 2009

Release June 30, 2008
Initial release

Release November 17, 2008
q330arch        -- Fixes problem where a hard slate crash would caused cached ISI data to never get written to disk.

Release March 19, 2009
netseed         -- Implements station retrieve capability
init.d/netseed
q330serv        -- Continuity fix for q330ser, fix growth /var/tmp/q330serv
/etc/init.d/q330serv  -- Shutdown waits for q330serv to finish shutting down
/home/maint/install (sudo enabled) -- Allows installation of new binaries
dlutil          -- Gives gap information whith time range argument
isirstat        -- Gives status about last seed record on ISI diskloop
30 minute timeout on logouts for data port server security issue.

Release May 4, 2009
isid            -- Fixed memory leak each time a new connection is made

Release June 1, 2009
All programs now have a way to get a version/release number
dispstatus      -- Will display clock status for each Q330
New IDA release -- rebuilt all software using IDA release 2009-05-14
                -- with official fixes for isid memory leak

Release July 13, 2009
q330arch        -- Removed debug messages from IDA log level
ida2liss        -- Removed debug messages from IDA log level

Release 1.7, December 2, 2009
/etc/init.d/ida2liss /etc/q330/ida2liss.config used to autostart ida2liss

Release 1.8, February 9,2009
falcon          -- New program to create seed records of falcon status
lib330_100327   -- Supports new environmental processor (microbarograph)
q330serv (1.3)  -- new diskloop.config keywords for falcon, lib330_100327
q330arch (1.4)  -- new diskloop.config keywords for falcon, NoIDA keyword
netseed (1.2)   -- new diskloop.config keywords for falcon
netreq (1.2)    -- new diskloop.config keywords for falcon
log330 (1.2)    -- new diskloop.config keywords for falcon
dlutil (1.3)    -- new diskloop.config keywords for falcon
Seneca (1.07)   -- Supports new environmental processor (microbarograph)
isirstat (1.2)  -- Add beg= and end= option arguments
dispstatus(1.3) -- new lib330
NOTE:  seneca.config is not compatible with earlier releases
Built with svn revision 216
