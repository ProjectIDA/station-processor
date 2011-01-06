#pragma ident "$Id: email.c,v 1.2 2001/05/07 22:40:12 dec Exp $"
/*======================================================================
 *
 *  Use a coprocess to send a file via email.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "util.h"

#ifndef MAXBYTES
#define MAXBYTES 128
#endif

#if (!defined MAILER)
#if (defined SOLARIS || defined BSD)
#define MAILER "/usr/ucb/Mail"
#else
#define MAILER "/usr/bin/mail"
#endif /* if (defined SOLARIS || defined BSD) */
#endif /* if (!defined MAILER) */

int util_email(char *address, char *subject, char *fname)
{
FILE *ifp;
char line[MAXBYTES];
pid_t pid;
int fd[2], ibytes, obytes, status;
static char *fid = "util_email";
    
    if (fname == NULL) fname = "/dev/null";

    if ((ifp = fopen(fname, "r")) == NULL) return -1;
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fd) < 0) {
        fclose(ifp);
        return -2;
    }
    if ((pid = fork()) < 0) return -3;

    if (pid > 0) { /* parent */

        while ((ibytes = fread(line, sizeof(char), MAXBYTES, ifp)) > 0) {
            if ((obytes = write(fd[0], line, ibytes)) != ibytes) {
                util_log(1, "write: %d != %d %s", 
                    ibytes, obytes, syserrmsg(errno)
                );
                return -4;
            }
        }

        status = ferror(ifp);

        fclose(ifp);
        close(fd[0]);
        close(fd[1]);

        if (status) return -5;
        
        if (waitpid(pid, &status, 0) != pid) return -6;
        if (WIFEXITED(status)) return WEXITSTATUS(status);

        return -7;

    } else {

        close(fd[0]);

        if (fd[1] != STDIN_FILENO) {
            if (dup2(fd[1], STDIN_FILENO) != STDIN_FILENO) {
                util_log(1, "%s: dup2: %s", fid, syserrmsg(errno));
                exit(1);
            }
        }

        if (fd[1] != STDOUT_FILENO) {
            if (dup2(fd[1], STDOUT_FILENO) != STDOUT_FILENO) {
                util_log(1, "%s: dup2: %s", fid, syserrmsg(errno));
                exit(1);
            }
        }

        if (subject != NULL) {
            execl(MAILER, MAILER, "-s", subject, address, NULL);
        } else {
            execl(MAILER, MAILER, address, NULL);
        }
    }
}

#ifdef DEBUG_TEST

void help(myname)
char *myname;
{
    fprintf(stderr, "usage: %s [-s subject] address\n", myname);
    exit(1);
}

main(argc, argv)
int argc;
char *argv[];
{
int i, status;
char *address = NULL;
char *subject = NULL;
static char *file  = NULL;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-s") == 0) {
            if (++i == argc) help(argv[0]);
            subject = argv[i];
        } else if (strcmp(argv[i], "-f") == 0) {
            if (++i == argc) help(argv[0]);
            file = argv[i];
        } else {
            address = argv[i];
        }
    }

    if (address != NULL) {
        printf("To:      %s\n", address);
    } else {
        help(argv[0]);
        exit(1);
    }

    if (subject != NULL) {
        printf("Subject: %s\n", subject);
    } else {
        printf("Subject: <none>\n");
    }

    if (file != NULL) {
        printf("File:    %s\n", file);
    } else {
        printf("File:    <none>\n");
    }

    status = util_email(address, subject, file);
    printf("util_email(%s, %s, %s) returns %d\n", address,subject,file,status);
    exit(status);
}

#endif /* DEBUG_TEST */

/* Revision History
 *
 * $Log: email.c,v $
 * Revision 1.2  2001/05/07 22:40:12  dec
 * ANSI function declarations
 *
 * Revision 1.1.1.1  2000/02/08 20:20:41  dec
 * import existing IDA/NRTS sources
 *
 */
