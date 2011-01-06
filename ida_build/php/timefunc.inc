<?php
/* $Id: timefunc.inc,v 1.1 2003/03/19 01:34:44 dechavez Exp $
 * Time/date related PHP utlities
 */

function isLeap($year)
{
    return (($year % 4 == 0 && $year % 100 != 0) || $year % 400 == 0) ? true : false;
}

function YearDayToYearMonthDay($yyyyddd)
{

    sscanf($yyyyddd, "%4d%3d", &$year, &$day);

    if (isLeap($year)) {
        $DayTable = array(0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31);
    } else {
        $DayTable = array(0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31);
    }

    for ($i = 1; $day > $DayTable[$i]; $i++) $day -= $DayTable[$i];
    return array($year, $i, $day);
}

function YearDayToUnixTime($yyyyddd)
{
    list($year, $month, $day) = YearDayToYearMonthDay($yyyyddd);
    return gmmktime(0, 0, 0, $month, $day, $year);
}

function UnixTimeToMySQLDateTime($unix)
{
    return gmstrftime("%Y-%m-%d %H-%M-%S", $unix);
}

function YearDayToMySQLDateTime($yyyyddd)
{
    if ($yyyyddd == -1) return "9999-12-31 23:59:59";
    return UnixTimeToMySQLDateTime(YearDayToUnixTime($yyyyddd));
}

/* Revision History
 *
 * $Log: timefunc.inc,v $
 * Revision 1.1  2003/03/19 01:34:44  dechavez
 * created
 *
 */
?>
