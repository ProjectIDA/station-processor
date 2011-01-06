#pragma ident "$Id: kill.c,v 1.1 2005/09/30 16:29:27 dechavez Exp $"

#include "platform.h"

/* this is not the real win32 kill. It's just a temporary plug 
 to avoid linking problem */

int kill(pid_t pid, int sig)
	{
	return -1;
	}

/* Revision History
 *
 * $Log: kill.c,v $
 * Revision 1.1  2005/09/30 16:29:27  dechavez
 * initial release (09-30-2005-aap)
 *
 */
