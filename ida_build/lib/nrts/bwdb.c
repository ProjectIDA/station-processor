#pragma ident "$Id: bwdb.c,v 1.3 2008/01/07 22:34:55 dechavez Exp $"
/*======================================================================
 *
 *  Database hook for wfdisc managment.  Called from bwd.c if the
 *  NRTS_DL flags have the NRTS_DL_FLAGS_USE_BWD_DB bit set.
 *
 *====================================================================*/
#include "nrts/dl.h"

#ifdef INCLUDE_NRTS_BWD_DB

void getOldWfid(DBIO *db, WFDISC *wfdisc, int begend);

static void CreateWfdisc(DBIO *db, WFDISC *wfdisc)
{
_int64 id;
char query[1024]; 
MYSQL_RES *result; 

sprintf(query,"insert into wfdisc (sta, chan, time, jdate, endtime," 
		"nsamp, samprate, calib, calper, instype, datatype, dir, dfile, foff, lddate, owner)"
		" values ('%s', '%s', %17.3f, %8ld, %17.3f, %8ld, %11.7f, %16.6f, %16.6f,'%s','%s','%s', '%s',%10ld, SYSDATE(),'nrts')", 
		wfdisc->sta, wfdisc->chan, wfdisc->time, wfdisc->jdate, wfdisc->endtime, wfdisc->nsamp, wfdisc->smprate,
		wfdisc->calib, wfdisc->calper, wfdisc->instype, wfdisc->datatype, wfdisc->dir, wfdisc->dfile, wfdisc->foff); 


	if (!dbioExecuteQuery(db, query)) return;

	id = mysql_insert_id(&db->mysql);

	wfdisc->wfid = id;

    result = mysql_store_result(&db->mysql); 

	mysql_free_result(result);
    return;
}

static void ReduceWfdisc(DBIO *db, WFDISC *wfdisc)
{
char query[1024]; 
MYSQL_RES *result; 

if(wfdisc->wfid==-1)
	{
	getOldWfid(db, wfdisc, 1);
	if(wfdisc->wfid==-1)
		{
		CreateWfdisc(db, wfdisc);
		return ;
		}
	}

if(wfdisc->nsamp==0)
	{
	sprintf(query, "delete from wfdisc where wfid=%ld", wfdisc->wfid);
	if(!dbioExecuteQuery(db, query)) return;
    result = mysql_store_result(&db->mysql); 
	mysql_free_result(result);
	return ;
	}

sprintf(query,"update wfdisc set  time=%.3f, nsamp=%ld, foff=%ld lddate= SYSDATE() where wfid=%ld", 
		wfdisc->time, wfdisc->nsamp, wfdisc->foff, wfdisc->wfid);

if (!dbioExecuteQuery(db, query)) return;
    result = mysql_store_result(&db->mysql); 

	mysql_free_result(result);

    return ;
}

static void ExtendWfdisc(DBIO *db, WFDISC *wfdisc)
{
char query[1024]; 
MYSQL_RES *result; 

if(wfdisc->wfid==-1)
	{
	getOldWfid(db, wfdisc, 0);
	if(wfdisc->wfid==-1)
		{
		CreateWfdisc(db, wfdisc);
		return;
		}
	}


sprintf(query,"update wfdisc set  endtime=%17.3f, nsamp=%8ld, lddate= SYSDATE() where wfid=%ld", 
		wfdisc->endtime, wfdisc->nsamp, wfdisc->wfid);

if (!dbioExecuteQuery(db, query)) return;
    result = mysql_store_result(&db->mysql); 

	mysql_free_result(result);

    return ;
}

void nrtsUpdateBwdDb(DBIO *db, WFDISC *wfdisc, int action)
{
    switch (action) {
      case NRTS_CREATE: CreateWfdisc(db, wfdisc); break;
      case NRTS_REDUCE: ReduceWfdisc(db, wfdisc); break;
      case NRTS_EXTEND: ExtendWfdisc(db, wfdisc); break;
    }
}

static void getOldWfid(DBIO *db, WFDISC *wfdisc, int begend)
	{
	MYSQL_RES *result;
	_int64 id;
	char query[1024]; 
	MYSQL_ROW row; 

	if(begend)
		{
		sprintf(query, "select wfid from wfdisc where sta='%s' and chan='%s' and endtime=%.3f",
			wfdisc->sta, wfdisc->chan, wfdisc->endtime);
		}
	else
		{
		sprintf(query, "select wfid from wfdisc where sta='%s' and chan='%s' and time=%.3f",
			wfdisc->sta, wfdisc->chan, wfdisc->time);
		}

	if (!dbioExecuteQuery(db, query)) return;
    result = mysql_store_result(&db->mysql); 

	if ((row = mysql_fetch_row(result)) != NULL) {
        id = atol(row[0]);
		wfdisc->wfid = id;
        mysql_free_result(result);
        return ;
    } 

    mysql_free_result(result);
    return ;

	}

BOOL InitTableWfdisc(NRTS_DL *dl)
	{
	int i;
	char query[1024];
	WFDISC *wfd;
	LNKLST_NODE *crnt;
	LNKLST *list;

	sprintf(query,"delete from wfdisc where sta='%s' and owner='nrts'", &dl->sys->sta[0]);

	if (!dbioExecuteQuery(dl->db, query)) return FALSE;


    for (i = 0; i < dl->sys->sta[0].nchn; i++) {
       crnt = listFirstNode(&dl->bwd.list[i]);
       while (crnt != NULL) {
	   		wfd = (WFDISC *) crnt->payload;
			CreateWfdisc(dl->db, wfd);
            crnt = listNextNode(crnt);
            }
        }

	return TRUE;
	}

#else

void nrtsUpdateBwdDb(DBIO *db, WFDISC *wfdisc, int action)
{
    return;
}

#endif /* !INCLUDE_NRTS_BWD_DB */

/* Revision History
 *
 * $Log: bwdb.c,v $
 * Revision 1.3  2008/01/07 22:34:55  dechavez
 * Added 12/24/2007 Akimov functions to replace stubs
 *
 * Revision 1.2  2007/06/01 19:21:52  dechavez
 * made default (dummy) action functions static
 *
 * Revision 1.1  2007/03/26 21:09:52  dechavez
 * initial (stubbed) release
 *
 */
