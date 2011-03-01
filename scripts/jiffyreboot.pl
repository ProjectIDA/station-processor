#!/usr/bin/perl
#==============================================================================
# Script:  jiffyreboot.pl
#
# The slate linux jiffy time counter rolls over after 496 days causing
# q330serv data acquisition program to hang, along with some other bugs.
# This script will get run weekly as a cron job.  If it's not a holiday, or
# the last two weeks of the year, and the machine has been up 400 days,
# then reboot the machine
#==============================================================================

use strict;

my $cmd;
my $uptime;
my $days=0;
my $today;
my $month;
my $dayofmonth;
my $hostname;

$uptime = `uptime`
  or die "uptime command failed: $?\n";

if ($uptime =~ /.*\s+up\s+(\d+)\s+days,$*/)
{
  $days = $1;
} # pull out days up

$hostname = `hostname`;
chomp($hostname);

$today = `date`;
if ($today =~ /^(\w+)\s+(\w+)\s+(\d+)\s+.*$/)
{
  $month = $2;
  $dayofmonth = $3;
}

# Don't reboot if we haven't been up for 400 days
if ($days >= 400)
{
  print "Station $hostname has been up $days days\n";

  # Don't reboot last two weeks of December
  if ($month eq "Dec" && $dayofmonth > 18)
  {
    print "Not rebooting during last two weeks of December\n";
    exit 0;
  }

  # Don't reboot on January 1
  if ($month eq "Jan" && $dayofmonth == 1)
  {
    print "Not rebooting on January 1st\n";
    exit 0;
  }

  # Don't reboot on July 4
  if ($month eq "Jul" && $dayofmonth == 4)
  {
    print "Not rebooting on July 4th\n";
    exit 0;
  }

  # Reboot the machine
  system("/sbin/reboot")
    or die "/sbin/reboot command failed: $?\n";
} # if up 400 days

