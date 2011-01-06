#pragma ident "$Id: ff.c,v 1.14 2008/01/15 19:36:32 dechavez Exp $"
/*======================================================================
 *
 *  IDA "flatfile" database support.
 *
 * NOTE:  This library is deprecated, and exists only to bridge the
 *        transition to a real database.
 *
 *====================================================================*/
#include "util.h"
#include "ffdb.h"
#include "isi.h"

#define MAXTEXT 128
#define GETLINE(comment) util_getline(fp, buffer, MAXTEXT, comment, &lineno)

static int CloseFp(FILE *fp, int status)
{
    if (fp != NULL) fclose(fp);
    return status;
}

static int LoadChanmap(IDAFF_CHANMAP *map, char *path, char *buffer)
{
FILE *fp;
int i, status;
int lineno = 0;
char *token;
static char *fid = "idaffInit:LoadChanmap";

    if ((fp = fopen(path, "r")) == (FILE *) NULL) {
        util_log(1, "%s: fopen: %s: %s", fid, path, syserrmsg(errno));
        return CloseFp(fp, -1);
    }

/*  Read once to count how many entries it has and allocate for them  */

    map->nstream = 0;
    while ((status = GETLINE('#')) == 0) ++map->nstream;
    if (status != 1) return CloseFp(fp, -2);

    map->stream = (IDAFF_STREAM *) malloc(map->nstream*sizeof(IDAFF_STREAM));

    if (map->stream == NULL) return CloseFp(fp, -3);

/*  Read again, loading entries into map structure  */

    rewind(fp);

    i = 0;
    while ((status = GETLINE('#')) == 0) {

    /*  Get filter code  */

        if ((token = strtok(buffer, " ")) == NULL) return CloseFp(fp, -4);
        map->stream[i].filt = atoi(token);

    /*  Get channel code  */

        if ((token = strtok(NULL, " ")) == NULL) return CloseFp(fp, -5);
        map->stream[i].chan = atoi(token);

    /*  Get trigger flag  */

        if ((token = strtok(NULL, " ")) == NULL) return CloseFp(fp, -6);
        map->stream[i].trig = atoi(token);

    /*  ignore internal stream code  */

        if ((token = strtok(NULL, " ")) == NULL) return CloseFp(fp, -7);

    /*  Get chnloc name */

        if ((token = strtok(NULL, " ")) == NULL) return CloseFp(fp, -8);
        strlcpy(map->stream[i].chname, token, IDA_CNAMLEN);

    /*  ignore analog channel code  */

        if ((token = strtok(NULL, " ")) == NULL) return CloseFp(fp, -9);

    /*  Rest is description, ignore it  */

        i++;
    }
    if (status != 1) return CloseFp(fp, -11);

    return CloseFp(fp, 0);
}

#ifdef WIN32

static int LoadAllMaps(IDAFF *ff, char *top, char *buffer)
{
int i;
char path[MAXPATHLEN+1];
struct stat st;
long hFile;
struct _finddata_t all_files;
char bufpath[_MAX_PATH];
static char *fid = "idaffInit:load_allmaps";

/* Get the current working directory: */

   if( _getcwd( bufpath, _MAX_PATH ) == NULL ) return -1;
      
    _chdir(top);

/* Read directory once, to find out how many files are present */

    if( (hFile = _findfirst( "*.*", &all_files )) == -1L ) return -1;
    ff->nmap = 0;
    while( _findnext( hFile, &all_files ) == 0 )
        {
        if (all_files.name[0] != '.' && strlen(all_files.name) < IDA_MNAMLEN) {
            snprintf(path, MAXPATHLEN+1, "%s\\%s", top, all_files.name);
            

            if (stat(path, &st) != 0) {
                util_log(1,"%s: dirls: stat: %s: %s", path, syserrmsg(errno));
                chdir(bufpath);
                return -2;
            }
            if (!(all_files.attrib&_A_SUBDIR)) ++ff->nmap;
        }
    }

/* Allocate space for all files, and load them in */

    ff->chanmap = (IDAFF_CHANMAP *)
                   malloc(ff->nmap * sizeof(IDAFF_CHANMAP));
    if (ff->chanmap == (IDAFF_CHANMAP *) NULL) return -3;

    _findclose( hFile);

    if( (hFile = _findfirst( "*.*", &all_files )) == -1L ) return -1;

    i = 0;
    while( _findnext( hFile, &all_files ) == 0 )
        {
        if (all_files.name[0] != '.' && strlen(all_files.name) < IDA_MNAMLEN) {
            snprintf(path, MAXPATHLEN+1, "%s\\%s", top, all_files.name);
            if (stat(path, &st) != 0) {
                util_log(1,"%s: dirls: stat: %s: %s",
                    path, syserrmsg(errno)
                );
                chdir(bufpath);
                return -4;
            }
            if (!(all_files.attrib&_A_SUBDIR)) {
                strlcpy(ff->chanmap[i].name, all_files.name, IDA_MNAMLEN);
                if (LoadChanmap(ff->chanmap + i, path, buffer) == 0) i++;
            }
        }
    }
    _findclose( hFile);
    ff->nmap = i;
    chdir(bufpath);
    return 0;
}

#else

static int LoadAllMaps(IDAFF *ff, char *top, char *buffer)
{
int i;
DIR *dirp;
char path[MAXPATHLEN+1];
struct dirent *dp;
struct stat st;
static char *fid = "idaffInit:LoadAllMaps";

/* Read directory once, to find out how many files are present */

    if ((dirp = opendir(top)) == NULL) {
        util_log(1, "%s: opendir: %s: %s", top, syserrmsg(errno));
        return -1;
    }

    ff->nmap = 0;
    for (dp = readdir(dirp); dp != NULL; dp = readdir(dirp)) {
        if (dp->d_name[0] != '.' && strlen(dp->d_name) < IDA_MNAMLEN) {
            snprintf(path, MAXPATHLEN+1, "%s/%s", top, dp->d_name);
            if (stat(path, &st) != 0) {
                util_log(1,"%s: dirls: stat: %s: %s",
                    path, syserrmsg(errno)
                );
                return -2;
            }
            if (S_ISREG(st.st_mode)) ++ff->nmap;
        }
    }

/* Allocate space for all files, and load them in */

    ff->chanmap = (IDAFF_CHANMAP *)
                   malloc(ff->nmap * sizeof(IDAFF_CHANMAP));
    if (ff->chanmap == (IDAFF_CHANMAP *) NULL) return -3;

    rewinddir(dirp);
    i = 0;
    for (dp = readdir(dirp); dp != NULL; dp = readdir(dirp)) {
        if (dp->d_name[0] != '.' && strlen(dp->d_name) < IDA_MNAMLEN) {
            snprintf(path, MAXPATHLEN+1, "%s/%s", top, dp->d_name);
            if (stat(path, &st) != 0) {
                util_log(1,"%s: dirls: stat: %s: %s",
                    path, syserrmsg(errno)
                );
                return -4;
            }
            if (S_ISREG(st.st_mode)) {
                strlcpy(ff->chanmap[i].name, dp->d_name, IDA_MNAMLEN);
                if (LoadChanmap(ff->chanmap + i, path, buffer) == 0) i++;
            }
        }
    }
    closedir(dirp);
    ff->nmap = i;

    return 0;
}
#endif

static int LoadSint(IDAFF *ff, char *path, char *buffer)
{
FILE *fp;
int i, status;
int lineno = 0;
char *token[5];
static char *fid = "idaffInit:LoadSint";

    if ((fp = fopen(path, "r")) == (FILE *) NULL) {
        util_log(1, "%s: fopen: %s: %s", fid, path, syserrmsg(errno));
        return CloseFp(fp, -1);
    }

/*  Read once to count how many entries it has and allocate for them  */

    ff->nsint = 0;
    while ((status = GETLINE('#')) == 0) ++ff->nsint;
    if (status != 1) return CloseFp(fp, -2);

    ff->sint = (IDAFF_SINTMAP *)
               malloc(ff->nsint*sizeof(IDAFF_SINTMAP));
    if (ff->sint == (IDAFF_SINTMAP *) NULL) return CloseFp(fp, -3);

/*  Read again, loading entries into map structure  */

    rewind(fp);

    i = 0;
    while ((status = GETLINE('#')) == 0) {
        if (util_sparse(buffer, token, " ", 5) != 5) {
            util_log(1, "%s: error at %s line %d", fid, path, lineno);
            return CloseFp(fp, -4);
        }
        strlcpy(ff->sint[i].sta, token[0], IDA_SNAMLEN);
        strlcpy(ff->sint[i].chn, token[1], IDA_CNAMLEN);
        ff->sint[i].sint = atoi(token[2]);
        ff->sint[i].datlen = atoi(token[3]);
        ff->sint[i].wrdsiz = atoi(token[4]);
        i++;
    }
    if (status != 1) return CloseFp(fp, -5);

    return CloseFp(fp, 0);
}

static int LoadStamap(IDAFF *ff, char *path, char *buffer)
{
FILE *fp;
int i, status, lineno = 0;
char *token;
static char *fid = "idaffInit:LoadStamap";

    if ((fp = fopen(path, "r")) == (FILE *) NULL) {
        util_log(1, "%s: fopen: %s: %s", fid, path, syserrmsg(errno));
        return CloseFp(fp, -1);
    }

/*  Read once to count how many entries it has and allocate for them  */

    ff->nstamap = 0;
    while ((status = GETLINE('#')) == 0) ++ff->nstamap;
    if (status != 1) return CloseFp(fp, -2);

    ff->stamap = (IDAFF_STAMAP *) malloc(ff->nstamap*sizeof(IDAFF_STAMAP));
    if (ff->stamap == NULL) return CloseFp(fp, -3);

/*  Read again, loading entries into map structure  */

    rewind(fp);

    i = 0;
    while ((status = GETLINE('#')) == 0) {

    /*  Get station name */

        if ((token = strtok(buffer, " ")) == NULL) return CloseFp(fp, -4);
        strlcpy(ff->stamap[i].sta, token, IDA_SNAMLEN);

    /*  Get map name */

        if ((token = strtok(NULL, " ")) == NULL) return CloseFp(fp, -5);
        strlcpy(ff->stamap[i].name, token, IDA_MNAMLEN);

        i++;
    }
    if (status != 1) return CloseFp(fp, -6);

    return CloseFp(fp, 0);
}

static int LoadTqual(IDAFF *ff, char *path, char *buffer)
{
FILE *fp;
int i, status;
int lineno = 0;
char *token;
static char *fid = "idaffInit:LoadTqual";

    if ((fp = fopen(path, "r")) == (FILE *) NULL) {
        util_log(1, "%s: fopen: %s: %s", fid, path, syserrmsg(errno));
        return CloseFp(fp, -1);
    }

/*  Read once to count how many entries it has and allocate for them  */

    ff->nqual = 0;
    while ((status = GETLINE(';')) == 0) ++ff->nqual;
    if (status != 1) return CloseFp(fp, -2);

    ff->tqual = (IDAFF_TQUAL *) malloc(ff->nqual*sizeof(IDAFF_TQUAL));
    if (ff->tqual == (IDAFF_TQUAL *) NULL) return CloseFp(fp, -3);

/*  Read again, loading entries into map structure  */

    rewind(fp);

    i = 0;
    while ((status = GETLINE(';')) == 0) {

        if ((token = strtok(buffer, " ")) == NULL) return CloseFp(fp, -4);
        ff->tqual[i].factory = (strlen(token) == 1) ? token[0] :
                               (char) strtol(token, (char **) NULL, 0);

        if ((token = strtok(NULL, " ")) == NULL) return CloseFp(fp, -5);
        ff->tqual[i].internal = atoi(token);

        i++;
    }
    if (status != 1) return CloseFp(fp, -6);

    return CloseFp(fp, 0);
}

static int LoadRevs(IDAFF *ff, char *path, char *buffer)
{
FILE *fp;
int i, j, k, status, nblank, oldlen, newlen;
int lineno = 0;
char *token;
static char *fid = "idaffInit:LoadRevs";

    if ((fp = fopen(path, "r")) == (FILE *) NULL) {
        util_log(1, "%s: fopen: %s: %s", fid, path, syserrmsg(errno));
        return CloseFp(fp, -1);
    }

/*  Read once to count how many entries it has and allocate for them  */

    ff->nrev = 0;
    while ((status = GETLINE('#')) == 0) ++ff->nrev;
    if (status != 1) return CloseFp(fp, -2);

    ff->rev = (IDAFF_REVMAP *) malloc(ff->nrev*sizeof(IDAFF_REVMAP));
    if (ff->rev == NULL) return CloseFp(fp, -3);

/*  Read again, loading entries into map structure  */

    rewind(fp);

    i = 0;
    while ((status = GETLINE('#')) == 0) {

        if ((token = strtok(buffer, " ")) == NULL) return CloseFp(fp, -4);
        ff->rev[i].code = atoi(token);

    /*  Rest is description  */

        token += strlen(token) + 1;
        oldlen = strlen(token);
        nblank = 0;
        while (token[nblank] == ' ' && nblank < oldlen) nblank++;
        newlen = oldlen - nblank;
        for (j=0, k=nblank; j < newlen; j++,k++) token[j] = token[k];
        token[newlen] = 0;
        if (token[newlen-1] == 0x0d) token[newlen-1] = 0;

        if ((ff->rev[i].text = malloc(strlen(token)+1)) == NULL) return CloseFp(fp, -5);
        strlcpy(ff->rev[i].text, token, strlen(token)+1);

        i++;
    }
    if (status != 1) return CloseFp(fp, -6);

    return CloseFp(fp, 0);
}

static int LoadSystems(IDAFF *ff, char *path, char *buffer)
{
FILE *fp;
int unused, len;
ISI_SYSTEM entry;

    listInit(&ff->systems);
    if ((fp = fopen(path, "r")) == NULL) return -1;
    while (utilGetLine(fp, buffer, MAXPATHLEN, '#', &unused) == 0) {
        len = strlen(buffer) <= ISI_STALEN ? strlen(buffer) + 1 : ISI_STALEN;
        strlcpy(entry.sta, buffer, len);
        entry.flags.sf_private = 0;
        if (!listAppend(&ff->systems, &entry, sizeof(ISI_SYSTEM))) {
            listDestroy(&ff->systems);
            fclose(fp);
            return -2;
        }
    }
    if (!listSetArrayView(&ff->systems)) return -3;
    return 0;
}

/* Read in the flatfiles */

static BOOL LoadFlatfiles(IDAFF *ff, char *spec)
{
char path[MAXPATHLEN+1];
char buf[MAXPATHLEN+1];
char *isicfg;

    snprintf(path, MAXPATHLEN+1, "%s%cetc%crevs", spec, PATH_DELIMITER, PATH_DELIMITER);
    if (LoadRevs(ff, path, buf) != 0) return FALSE;

    snprintf(path, MAXPATHLEN+1, "%s%cetc%cmaps", spec, PATH_DELIMITER, PATH_DELIMITER);
    if (LoadAllMaps(ff, path, buf) != 0) return FALSE;

    snprintf(path, MAXPATHLEN+1, "%s%cetc%ctqual", spec, PATH_DELIMITER, PATH_DELIMITER);
    if (LoadTqual(ff, path, buf) != 0) return FALSE;

    snprintf(path, MAXPATHLEN+1, "%s%cetc%csint", spec, PATH_DELIMITER, PATH_DELIMITER);
    if (LoadSint(ff, path, buf) != 0) return FALSE;

    snprintf(path, MAXPATHLEN+1, "%s%cetc%cmaps%cAssignments", spec, PATH_DELIMITER, PATH_DELIMITER, PATH_DELIMITER);
    if (LoadStamap(ff, path, buf) != 0) return FALSE;

    snprintf(path, MAXPATHLEN+1, "%s%cetc%cSystems", spec, PATH_DELIMITER, PATH_DELIMITER);
    if (LoadSystems(ff, path, buf) != 0) return FALSE;

    snprintf(path, MAXPATHLEN+1, "%s%cetc%cisi.cfg", spec, PATH_DELIMITER, PATH_DELIMITER);
    isicfg = utilFileExists(path) ? path : NULL;
    if (!idaffReadGlobalInitFile(isicfg, &ff->glob)) return FALSE;

    return TRUE;
}

static void FreeRevs(IDAFF *ff)
{
int i;

    for (i = 0; i < ff->nrev; i++) free(ff->rev[i].text);
    free(ff->rev);
}

static void FreeChanmap(IDAFF_CHANMAP *map)
{
    if (map == NULL) return;
    if (map->nstream < 1) return;
    free(map->stream);
}

static void FreeAllMaps(IDAFF *ff)
{
int i;

    for (i = 0; i < ff->nmap; i++) FreeChanmap(&ff->chanmap[i]);
    free(ff->chanmap);
}

void idaffDestroy(IDAFF *ff)
{
    if (ff == NULL) return;
    FreeRevs(ff);
    FreeAllMaps(ff);
    free(ff->tqual);
    free(ff->sint);
    free(ff->stamap);
    free(ff);
}

static void InitalizeFF(IDAFF *ff)
{
    ff->nmap = 0;
    ff->chanmap = NULL;

    ff->nqual = 0;
    ff->tqual = NULL;

    ff->nrev = 0;
    ff->rev = NULL;

    ff->nsint = 0;
    ff->sint = NULL;

    ff->nstamap = 0;
    ff->stamap = NULL;

    ff->nserial = 0;
    ff->serial = NULL;

    listInit(&ff->systems);
}

IDAFF *idaffInit(char *spec)
{
IDAFF *ff;
static char *fid = "idaffInit";

    if (spec == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    if ((ff = (IDAFF *) malloc(sizeof(IDAFF))) == NULL) return NULL;
    InitalizeFF(ff);
    if (!LoadFlatfiles(ff, spec)) {
        idaffDestroy(ff);
        return NULL;
    }

    return ff;
}

/* Given factory clock quality code, return numerical equivalent */

BOOL idaffLookupTqual(IDAFF *ff, char factory, int *internal)
{
int i;

    *internal = IDA_UNDEFINED_TQUAL;

    if (ff == (IDAFF *) NULL) {
        errno = EINVAL;
        return FALSE;
    }

    for (i = 0; i < ff->nqual; i++) {
        if (ff->tqual[i].factory == factory) {
            *internal = ff->tqual[i].internal;
            return TRUE;
        }
    }

    return TRUE;
}

/* Given station and channel return sint, datlen, wrdsiz */

BOOL idaffLookupSintmap(IDAFF *ff, IDAFF_SINTMAP *map)
{
int i;
static char *fid = "idaffLookupSintmap";

    if (ff  == (IDAFF *) NULL || map == (IDAFF_SINTMAP *) NULL) {
        errno = EINVAL;
        return FALSE;
    }

    util_strtrm(map->chn);
    for (i = 0; i < ff->nsint; i++) {
        if (strcasecmp(ff->sint[i].chn, map->chn) == 0) {
            if (strcasecmp(ff->sint[i].sta, map->sta) == 0) {
                map->sint   = ff->sint[i].sint;
                map->datlen = ff->sint[i].datlen;
                map->wrdsiz = ff->sint[i].wrdsiz;
                return TRUE;
            }
        }
    }

    for (i = 0; i < ff->nsint; i++) {
        if (strcasecmp(ff->sint[i].chn, map->chn) == 0) {
            if (strcasecmp(ff->sint[i].sta, "-") == 0) {
                map->sint   = ff->sint[i].sint;
                map->datlen = ff->sint[i].datlen;
                map->wrdsiz = ff->sint[i].wrdsiz;
                return TRUE;
            }
        }
    }

    return FALSE;
}

/* Given station name, return name of channel map */

char *idaffLookupStamap(IDAFF *ff, char *sta)
{
int i;

    if (ff  == NULL || sta == NULL) {
        errno = EINVAL;
        return NULL;
    }

    for (i = 0; i < ff->nstamap; i++) {
        if (strcasecmp(ff->stamap[i].sta, sta) == 0) return ff->stamap[i].name;
    }

    return NULL;
}

/* Revision History
 *
 * $Log: ff.c,v $
 * Revision 1.14  2008/01/15 19:36:32  dechavez
 * fixed convoluted logic in idaffLookupSintmap
 *
 * Revision 1.13  2007/10/30 21:58:59  dechavez
 * replaced string memcpy with strlcpy
 *
 * Revision 1.12  2007/03/07 14:24:26  dechavez
 * fixed insane FreeChanmap()
 *
 * Revision 1.11  2007/01/11 20:19:27  dechavez
 * snprintf instead of sprintf
 *
 * Revision 1.10  2007/01/08 15:48:03  dechavez
 * strlcpy() instead of strncpy()
 *
 * Revision 1.9  2006/11/10 06:52:03  dechavez
 * fixed reserved name bug
 *
 * Revision 1.8  2006/08/30 18:16:22  judy
 * added support for ISI_SYSTEM
 *
 * Revision 1.7  2006/06/26 22:18:54  dechavez
 * removed unreferenced local variables
 *
 * Revision 1.6  2006/03/15 20:48:57  dechavez
 * check for existence of isi.cfg before loading IDAFF_GLOB.  Setting path
 * to NULL will force load the default parameters.
 *
 * Revision 1.5  2006/03/15 20:43:48  dechavez
 * check for NULL map in FreeChanmap()
 *
 * Revision 1.4  2006/03/14 20:35:14  dechavez
 * fixed typo generating isi.cfg path name
 *
 * Revision 1.3  2006/03/13 22:16:54  dechavez
 * added IDAFF_GLOB support
 *
 * Revision 1.2  2006/02/08 23:58:28  dechavez
 * removed tabs
 *
 * Revision 1.1  2006/02/08 23:53:54  dechavez
 * initial release
 *
 */
