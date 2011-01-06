#pragma ident "$Id: iso9660.h,v 1.2 2008/03/10 20:50:25 dechavez Exp $"
#ifndef iso9660_h_included
#define iso9660_h_included

#include "platform.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ISO9660_IMAGE_CREATION_DATE					813
#define ISO9660_DATE_FIELD_SIZE						17

#define ISO9660_DIRECTORY_RECORD_LENGTH             0
#define ISO9660_DIRECTORY_RECORD_EXT_ATTR_LENGTH    1
#define ISO9660_DIRECTORY_RECORD_EXTENT             2
#define ISO9660_DIRECTORY_RECORD_SIZE               10
#define ISO9660_DIRECTORY_RECORD_DATE               18
#define ISO9660_DIRECTORY_RECORD_FLAGS              25
#define ISO9660_DIRECTORY_RECORD_FILE_UNIT_SIZE     26
#define ISO9660_DIRECTORY_RECORD_INTERLEAVE			27
#define ISO9660_DIRECTORY_RECORD_VOLUME_SEQUENCE_NUMBER			28
#define ISO9660_DIRECTORY_RECORD_NAME_LEN			32
#define ISO9660_DIRECTORY_RECORD_NAME				33

#define PRIMARY_DESCRIPTOR_PATH_TABLE_SIZE			132
#define PRIMARY_DESCRIPTOR_ROOT_DIRECTORY_RECORD_OFFSET				156
#define PRIMARY_DESCRIPTOR_FILE_STRUCTURE_VERSION					881


#define ISO9660_DEFAULT_BLOCK_SIZE		2048
#define PATH_TABLE_SECTOR           21
#define ROOT_DIRECTORY_EXTENT       23
#define PRIMARY_VOLUME_DESCRIPTOR	16

/* Root directory size in sectors */

#define ROOT_DIRECTORY_SIZE              60


/* Error code                            */

#define ISO9660_OK							0
#define ISO9660_IMAGE_FULL					-1000
#define ISO9660_DIRECTORY_FULL				-2000
#define ISO9660_IMAGE_READ_ERROR			-1
#define ISO9660_IMAGE_WRITE_ERROR			-2
#define ISO9660_IMAGE_OPEN_ERROR			-3
#define ISO9660_IMAGE_BAD_SIGNATURE			-4
#define ISO9660_IMAGE_CORRUPTED				-5
#define ISO9660_INPUT_FILE_OPEN_ERROR		-6
#define ISO9660_INPUT_FILE_READ_ERROR		-7
#define ISO9660_MEMORY_ALLOC_ERROR			-8


#define ISO9660_DATETIME_YEAR					0
#define ISO9660_DATETIME_MONTH					1
#define ISO9660_DATETIME_DAY					2
#define ISO9660_DATETIME_HOUR					3
#define ISO9660_DATETIME_MINUTE					4
#define ISO9660_DATETIME_SECOND					5
#define ISO9660_DATETIME_GMT_OFF				6



#define CDMAXSIZE                       681984000L

typedef struct
	{
	char path[MAXPATHLEN];
	int handle;
	}ISO9660;

/* iso9660.c */
int iso9660AddFile(ISO9660 *pISO, const char *pname, void *pdata, long nsize, struct tm filetm);
ISO9660* iso9660Open(const char *pFileName, int *pError);
int iso9660Append(ISO9660 *p, const char *pFileName);
ISO9660* iso9660Create(const char *pFileName, const char *VolumeIdentifier);
int iso9660Close(ISO9660 *p);

/* string.c */
char *iso9660ErrorString(int code);

/* version.c */
char *iso9660VersionString();
VERSION *iso9660Version();

#ifdef __cplusplus
}
#endif

#endif /* iso9660_h_included */

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2008 Andrei Akimov                                      |
 |                                                                       |
 | This software is provided 'as-is', without any express or implied     |
 | warranty.  In no event will the authors be held liable for any        |
 | damages arising from the use of this software.                        |
 |                                                                       |
 | Permission is granted to anyone to use this software for any purpose, |
 | including commercial applications, and to alter it and redistribute   |
 | it freely, subject to the following restrictions:                     |
 |                                                                       |
 | 1. The origin of this software must not be misrepresented; you must   |
 |    not claim that you wrote the original software. If you use this    |
 |    software in a product, an acknowledgment in the product            |
 |    documentation of the contribution by Andrei Akimov would be        |
 |    appreciated but is not required.                                   |
 | 2. Altered source versions must be plainly marked as such, and must   |
 |    not be misrepresented as being the original software.              |
 | 3. This notice may not be removed or altered from any source          |
 |    distribution.                                                      |
 |                                                                       |
 +-----------------------------------------------------------------------*/

/* Revision History
 *
 * $Log: iso9660.h,v $
 * Revision 1.2  2008/03/10 20:50:25  dechavez
 * added iso9660ErrorString() prototype
 *
 * Revision 1.1  2008/03/04 19:07:42  akimov
 * initial release
 *
 */
