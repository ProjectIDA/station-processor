#pragma ident "$Id: sacecho.c,v 1.2 2003/12/10 06:31:25 dechavez Exp $"
/*======================================================================
 *
 *  Echo the specified SAC header variable.
 *
 *  Exits status = 0 if last file read was OK
 *                 1 if last file read had system error
 *                 2 if unrecognized variable name was given
 *
 *====================================================================*/
#include <stdio.h>
#include "sacio.h"
#include "util.h"

char *fname;
char *var;
int status = 0;

static int echo_var(fp, print_name)
FILE *fp;
int  print_name;
{
struct sac_header hdr;
double beg, end;

    if (print_name) printf("%s: ", fname);

    if (sacio_rbh(fp, &hdr) != 0) {
        perror("sacio_rbh");
        return status = 1;
    }

    if (strcmp(var, "delta")     == 0) {
        printf("%f\n", hdr.delta);
        return status = 0;
    }
    if (strcmp(var, "depmin")    == 0) {
        printf("%f\n", hdr.depmin);
        return status = 0;
    }
    if (strcmp(var, "depmax")    == 0) {
        printf("%f\n", hdr.depmax);
        return status = 0;
    }
    if (strcmp(var, "scale")     == 0) {
        printf("%f\n", hdr.scale);
        return status = 0;
    }
    if (strcmp(var, "odelta")    == 0) {
        printf("%f\n", hdr.odelta);
        return status = 0;
    }
    if (strcmp(var, "b")         == 0) {
        printf("%f\n", hdr.b);
        return status = 0;
    }
    if (strcmp(var, "e")         == 0) {
        printf("%f\n", hdr.e);
        return status = 0;
    }
    if (strcmp(var, "o")         == 0) {
        printf("%f\n", hdr.o);
        return status = 0;
    }
    if (strcmp(var, "a")         == 0) {
        printf("%f\n", hdr.a);
        return status = 0;
    }
    if (strcmp(var, "internal1") == 0) {
        printf("%f\n", hdr.internal1);
        return status = 0;
    }
    if (strcmp(var, "t0")        == 0) {
        printf("%f\n", hdr.t0);
        return status = 0;
    }
    if (strcmp(var, "t1")        == 0) {
        printf("%f\n", hdr.t1);
        return status = 0;
    }
    if (strcmp(var, "t2")        == 0) {
        printf("%f\n", hdr.t2);
        return status = 0;
    }
    if (strcmp(var, "t3")        == 0) {
        printf("%f\n", hdr.t3);
        return status = 0;
    }
    if (strcmp(var, "t4")        == 0) {
        printf("%f\n", hdr.t4);
        return status = 0;
    }
    if (strcmp(var, "t5")        == 0) {
        printf("%f\n", hdr.t5);
        return status = 0;
    }
    if (strcmp(var, "t6")        == 0) {
        printf("%f\n", hdr.t6);
        return status = 0;
    }
    if (strcmp(var, "t7")        == 0) {
        printf("%f\n", hdr.t7);
        return status = 0;
    }
    if (strcmp(var, "t8")        == 0) {
        printf("%f\n", hdr.t8);
        return status = 0;
    }
    if (strcmp(var, "t9")        == 0) {
        printf("%f\n", hdr.t9);
        return status = 0;
    }
    if (strcmp(var, "f")         == 0) {
        printf("%f\n", hdr.f);
        return status = 0;
    }
    if (strcmp(var, "resp0")     == 0) {
        printf("%f\n", hdr.resp0);
        return status = 0;
    }
    if (strcmp(var, "resp1")     == 0) {
        printf("%f\n", hdr.resp1);
        return status = 0;
    }
    if (strcmp(var, "resp2")     == 0) {
        printf("%f\n", hdr.resp2);
        return status = 0;
    }
    if (strcmp(var, "resp3")     == 0) {
        printf("%f\n", hdr.resp3);
        return status = 0;
    }
    if (strcmp(var, "resp4")     == 0) {
        printf("%f\n", hdr.resp4);
        return status = 0;
    }
    if (strcmp(var, "resp5")     == 0) {
        printf("%f\n", hdr.resp5);
        return status = 0;
    }
    if (strcmp(var, "resp6")     == 0) {
        printf("%f\n", hdr.resp6);
        return status = 0;
    }
    if (strcmp(var, "resp7")     == 0) {
        printf("%f\n", hdr.resp7);
        return status = 0;
    }
    if (strcmp(var, "resp8")     == 0) {
        printf("%f\n", hdr.resp8);
        return status = 0;
    }
    if (strcmp(var, "resp9")     == 0) {
        printf("%f\n", hdr.resp9);
        return status = 0;
    }
    if (strcmp(var, "stla")      == 0) {
        printf("%f\n", hdr.stla);
        return status = 0;
    }
    if (strcmp(var, "stlo")      == 0) {
        printf("%f\n", hdr.stlo);
        return status = 0;
    }
    if (strcmp(var, "stel")      == 0) {
        printf("%f\n", hdr.stel);
        return status = 0;
    }
    if (strcmp(var, "stdp")      == 0) {
        printf("%f\n", hdr.stdp);
        return status = 0;
    }
    if (strcmp(var, "evla")      == 0) {
        printf("%f\n", hdr.evla);
        return status = 0;
    }
    if (strcmp(var, "evlo")      == 0) {
        printf("%f\n", hdr.evlo);
        return status = 0;
    }
    if (strcmp(var, "evel")      == 0) {
        printf("%f\n", hdr.evel);
        return status = 0;
    }
    if (strcmp(var, "evdp")      == 0) {
        printf("%f\n", hdr.evdp);
        return status = 0;
    }
    if (strcmp(var, "unused1")   == 0) {
        printf("%f\n", hdr.unused1);
        return status = 0;
    }
    if (strcmp(var, "user0")     == 0) {
        printf("%f\n", hdr.user0);
        return status = 0;
    }
    if (strcmp(var, "user1")     == 0) {
        printf("%f\n", hdr.user1);
        return status = 0;
    }
    if (strcmp(var, "user2")     == 0) {
        printf("%f\n", hdr.user2);
        return status = 0;
    }
    if (strcmp(var, "user3")     == 0) {
        printf("%f\n", hdr.user3);
        return status = 0;
    }
    if (strcmp(var, "user4")     == 0) {
        printf("%f\n", hdr.user4);
        return status = 0;
    }
    if (strcmp(var, "user5")     == 0) {
        printf("%f\n", hdr.user5);
        return status = 0;
    }
    if (strcmp(var, "user6")     == 0) {
        printf("%f\n", hdr.user6);
        return status = 0;
    }
    if (strcmp(var, "user7")     == 0) {
        printf("%f\n", hdr.user7);
        return status = 0;
    }
    if (strcmp(var, "user8")     == 0) {
        printf("%f\n", hdr.user8);
        return status = 0;
    }
    if (strcmp(var, "user9")     == 0) {
        printf("%f\n", hdr.user9);
        return status = 0;
    }
    if (strcmp(var, "dist")      == 0) {
        printf("%f\n", hdr.dist);
        return status = 0;
    }
    if (strcmp(var, "az")        == 0) {
        printf("%f\n", hdr.az);
        return status = 0;
    }
    if (strcmp(var, "baz")       == 0) {
        printf("%f\n", hdr.baz);
        return status = 0;
    }
    if (strcmp(var, "gcarc")     == 0) {
        printf("%f\n", hdr.gcarc);
        return status = 0;
    }
    if (strcmp(var, "internal2") == 0) {
        printf("%f\n", hdr.internal2);
        return status = 0;
    }
    if (strcmp(var, "internal3") == 0) {
        printf("%f\n", hdr.internal3);
        return status = 0;
    }
    if (strcmp(var, "depmen")    == 0) {
        printf("%f\n", hdr.depmen);
        return status = 0;
    }
    if (strcmp(var, "cmpaz")     == 0) {
        printf("%f\n", hdr.cmpaz);
        return status = 0;
    }
    if (strcmp(var, "cmpinc")    == 0) {
        printf("%f\n", hdr.cmpinc);
        return status = 0;
    }
    if (strcmp(var, "unused2")   == 0) {
        printf("%f\n", hdr.unused2);
        return status = 0;
    }
    if (strcmp(var, "unused3")   == 0) {
        printf("%f\n", hdr.unused3);
        return status = 0;
    }
    if (strcmp(var, "unused4")   == 0) {
        printf("%f\n", hdr.unused4);
        return status = 0;
    }
    if (strcmp(var, "unused5")   == 0) {
        printf("%f\n", hdr.unused5);
        return status = 0;
    }
    if (strcmp(var, "unused6")   == 0) {
        printf("%f\n", hdr.unused6);
        return status = 0;
    }
    if (strcmp(var, "unused7")   == 0) {
        printf("%f\n", hdr.unused7);
        return status = 0;
    }
    if (strcmp(var, "unused8")   == 0) {
        printf("%f\n", hdr.unused8);
        return status = 0;
    }
    if (strcmp(var, "unused9")   == 0) {
        printf("%f\n", hdr.unused9);
        return status = 0;
    }
    if (strcmp(var, "unused10")  == 0) {
        printf("%f\n", hdr.unused10);
        return status = 0;
    }
    if (strcmp(var, "unused11")  == 0) {
        printf("%f\n", hdr.unused11);
        return status = 0;
    }
    if (strcmp(var, "unused12")  == 0) {
        printf("%f\n", hdr.unused12);
        return status = 0;
    }

    if (strcmp(var, "nzyear")    == 0) {
        printf("%ld\n", hdr.nzyear);
        return status = 0;
    }
    if (strcmp(var, "nzjday")    == 0) {
        printf("%ld\n", hdr.nzjday);
        return status = 0;
    }
    if (strcmp(var, "nzhour")    == 0) {
        printf("%ld\n", hdr.nzhour);
        return status = 0;
    }
    if (strcmp(var, "nzmin")     == 0) {
        printf("%ld\n", hdr.nzmin);
        return status = 0;
    }
    if (strcmp(var, "nzsec")     == 0) {
        printf("%ld\n", hdr.nzsec);
        return status = 0;
    }
    if (strcmp(var, "nzmsec")    == 0) {
        printf("%ld\n", hdr.nzmsec);
        return status = 0;
    }
    if (strcmp(var, "internal4") == 0) {
        printf("%ld\n", hdr.internal4);
        return status = 0;
    }
    if (strcmp(var, "internal5") == 0) {
        printf("%ld\n", hdr.internal5);
        return status = 0;
    }
    if (strcmp(var, "internal6") == 0) {
        printf("%ld\n", hdr.internal6);
        return status = 0;
    }
    if (strcmp(var, "npts")      == 0) {
        printf("%ld\n", hdr.npts);
        return status = 0;
    }
    if (strcmp(var, "internal7") == 0) {
        printf("%ld\n", hdr.internal7);
        return status = 0;
    }
    if (strcmp(var, "internal8") == 0) {
        printf("%ld\n", hdr.internal8);
        return status = 0;
    }
    if (strcmp(var, "unused13")  == 0) {
        printf("%ld\n", hdr.unused13);
        return status = 0;
    }
    if (strcmp(var, "unused14")  == 0) {
        printf("%ld\n", hdr.unused14);
        return status = 0;
    }
    if (strcmp(var, "unused15")  == 0) {
        printf("%ld\n", hdr.unused15);
        return status = 0;
    }
    if (strcmp(var, "iftype")    == 0) {
        printf("%ld\n", hdr.iftype);
        return status = 0;
    }
    if (strcmp(var, "idep")      == 0) {
        printf("%ld\n", hdr.idep);
        return status = 0;
    }
    if (strcmp(var, "iztype")    == 0) {
        printf("%ld\n", hdr.iztype);
        return status = 0;
    }
    if (strcmp(var, "unused16")  == 0) {
        printf("%ld\n", hdr.unused16);
        return status = 0;
    }
    if (strcmp(var, "iinst")     == 0) {
        printf("%ld\n", hdr.iinst);
        return status = 0;
    }
    if (strcmp(var, "istreg")    == 0) {
        printf("%ld\n", hdr.istreg);
        return status = 0;
    }
    if (strcmp(var, "ievreg")    == 0) {
        printf("%ld\n", hdr.ievreg);
        return status = 0;
    }
    if (strcmp(var, "ievtyp")    == 0) {
        printf("%ld\n", hdr.ievtyp);
        return status = 0;
    }
    if (strcmp(var, "iqual")     == 0) {
        printf("%ld\n", hdr.iqual);
        return status = 0;
    }
    if (strcmp(var, "isynth")    == 0) {
        printf("%ld\n", hdr.isynth);
        return status = 0;
    }
    if (strcmp(var, "unused17")  == 0) {
        printf("%ld\n", hdr.unused17);
        return status = 0;
    }
    if (strcmp(var, "unused18")  == 0) {
        printf("%ld\n", hdr.unused18);
        return status = 0;
    }
    if (strcmp(var, "unused19")  == 0) {
        printf("%ld\n", hdr.unused19);
        return status = 0;
    }
    if (strcmp(var, "unused20")  == 0) {
        printf("%ld\n", hdr.unused20);
        return status = 0;
    }
    if (strcmp(var, "unused21")  == 0) {
        printf("%ld\n", hdr.unused21);
        return status = 0;
    }
    if (strcmp(var, "unused22")  == 0) {
        printf("%ld\n", hdr.unused22);
        return status = 0;
    }
    if (strcmp(var, "unused23")  == 0) {
        printf("%ld\n", hdr.unused23);
        return status = 0;
    }
    if (strcmp(var, "unused24")  == 0) {
        printf("%ld\n", hdr.unused24);
        return status = 0;
    }
    if (strcmp(var, "unused25")  == 0) {
        printf("%ld\n", hdr.unused25);
        return status = 0;
    }
    if (strcmp(var, "unused26")  == 0) {
        printf("%ld\n", hdr.unused26);
        return status = 0;
    }
    if (strcmp(var, "leven")     == 0) {
        printf("%ld\n", hdr.leven);
        return status = 0;
    }
    if (strcmp(var, "lpspol")    == 0) {
        printf("%ld\n", hdr.lpspol);
        return status = 0;
    }
    if (strcmp(var, "lovrok")    == 0) {
        printf("%ld\n", hdr.lovrok);
        return status = 0;
    }
    if (strcmp(var, "lcalda")    == 0) {
        printf("%ld\n", hdr.lcalda);
        return status = 0;
    }
    if (strcmp(var, "unused27")  == 0) {
        printf("%ld\n", hdr.unused27);
        return status = 0;
    }

    if (strcmp(var, "kstnm")     == 0) {
        printf("%s\n", util_strtrm(hdr.kstnm));
        return status = 0;
    }
    if (strcmp(var, "kevnm")     == 0) {
        printf("%s\n", util_strtrm(hdr.kevnm));
        return status = 0;
    }
    if (strcmp(var, "khole")     == 0) {
        printf("%s\n", util_strtrm(hdr.khole));
        return status = 0;
    }
    if (strcmp(var, "ko")        == 0) {
        printf("%s\n", util_strtrm(hdr.ko));
        return status = 0;
    }
    if (strcmp(var, "ka")        == 0) {
        printf("%s\n", util_strtrm(hdr.ka));
        return status = 0;
    }
    if (strcmp(var, "kt0")       == 0) {
        printf("%s\n", util_strtrm(hdr.kt0));
        return status = 0;
    }
    if (strcmp(var, "kt1")       == 0) {
        printf("%s\n", util_strtrm(hdr.kt1));
        return status = 0;
    }
    if (strcmp(var, "kt2")       == 0) {
        printf("%s\n", util_strtrm(hdr.kt2));
        return status = 0;
    }
    if (strcmp(var, "kt3")       == 0) {
        printf("%s\n", util_strtrm(hdr.kt3));
        return status = 0;
    }
    if (strcmp(var, "kt4")       == 0) {
        printf("%s\n", util_strtrm(hdr.kt4));
        return status = 0;
    }
    if (strcmp(var, "kt5")       == 0) {
        printf("%s\n", util_strtrm(hdr.kt5));
        return status = 0;
    }
    if (strcmp(var, "kt6")       == 0) {
        printf("%s\n", util_strtrm(hdr.kt6));
        return status = 0;
    }
    if (strcmp(var, "kt7")       == 0) {
        printf("%s\n", util_strtrm(hdr.kt7));
        return status = 0;
    }
    if (strcmp(var, "kt8")       == 0) {
        printf("%s\n", util_strtrm(hdr.kt8));
        return status = 0;
    }
    if (strcmp(var, "kt9")       == 0) {
        printf("%s\n", util_strtrm(hdr.kt9));
        return status = 0;
    }
    if (strcmp(var, "kf")        == 0) {
        printf("%s\n", util_strtrm(hdr.kf));
        return status = 0;
    }
    if (strcmp(var, "kuser0")    == 0) {
        printf("%s\n", util_strtrm(hdr.kuser0));
        return status = 0;
    }
    if (strcmp(var, "kuser1")    == 0) {
        printf("%s\n", util_strtrm(hdr.kuser1));
        return status = 0;
    }
    if (strcmp(var, "kuser2")    == 0) {
        printf("%s\n", util_strtrm(hdr.kuser2));
        return status = 0;
    }
    if (strcmp(var, "kcmpnm")    == 0) {
        printf("%s\n", util_strtrm(hdr.kcmpnm));
        return status = 0;
    }
    if (strcmp(var, "knetwk")    == 0) {
        printf("%s\n", util_strtrm(hdr.knetwk));
        return status = 0;
    }
    if (strcmp(var, "kdatrd")    == 0) {
        printf("%s\n", util_strtrm(hdr.kdatrd));
        return status = 0;
    }
    if (strcmp(var, "kinst")     == 0) {
        printf("%s\n", util_strtrm(hdr.kinst));
        return status = 0;
    }

    printf("'%s' is not a valid SAC header variable\n", var);
    return status = 2;
}

int main(int argc, char **argv)
{
int   i, nfiles;
FILE *fp;

    if (argc < 2) {
        fprintf(stderr, "usage: sacecho var_name file [file ...]\n");
        exit(1);
    }

    nfiles = argc - 2;
    var = argv[1];

    if (!nfiles) {
        if (SETMODE(fileno(stdin), O_BINARY) == -1) {
            perror("sacecho: setmode");
            exit(1);
        }
        echo_var(stdin, nfiles);
    } else {
        for (i = 2; i < argc; i++) {
            fname = argv[i];
            if ((fp = fopen(fname, "rb")) == NULL) {
                fprintf(stderr,"sacecho: ");
                perror(fname);
            } else {
                echo_var(fp, nfiles-1);
            }
        }
    }

    exit(status);
}

/* Revision History
 *
 * $Log: sacecho.c,v $
 * Revision 1.2  2003/12/10 06:31:25  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:20:19  dec
 * import existing IDA/NRTS sources
 *
 */
