#pragma ident "$Id: cd2.c,v 1.8 2003/12/10 06:31:22 dechavez Exp $"

/* Dump CD1.1 CSF packet headers */

#include <assert.h>
#include "platform.h"

#define MAXDAT 2048

int DumpData = 0;

void DumpDataValues(long *ptr, int nsamp)
{
int i;

printf("dump %d samples", nsamp);
    for (i = 0; i < nsamp; i++) {
        if (i % 10 == 0) printf("\n");
        printf("%ld ", ntohl(ptr[i]));
    }
    printf("\n");
}

int unpacklong(char *ptr, long *out)
{
unsigned long lval;

    memcpy(&lval, ptr, 4);
    *out = ntohl(lval);

    return 4;
}

int unpackfloat(char *ptr, float *out)
{
union {
    float f;
    long  l;
} val;
unsigned long lval;

    memcpy(&lval, ptr, 4);
    val.l = htonl(lval);
    *out = val.f;
    return 4;
}

static char *tostring(char *ptr, int count)
{
static char tmp[21];

    memcpy(tmp, ptr, count);
    tmp[count] = 0;
    return tmp;
}

static char *binary(char value)
{
static char output[] = "0 0 0 0 0 0 0 0 (0x00) and some more";
long diff;
char mask;
int i;

    output[0] = 0;
    for (i = 7; i >= 0; i--) {
        mask = 1 << i;
        sprintf(output+strlen(output), "%d ", (value & mask) ? 1 : 0);
    }
    sprintf(output+strlen(output), "(0x%02x)", value & 0x00ff);

    return output;
}

void decodechannelstatus(char *start)
{
long val;
int format;
char *ptr;

    ptr = start;

    printf("                               "
        "       format = %d\n", (int) *ptr
    );
    ptr += 1;

    printf("                               "
        "         data = %s\n", binary(*ptr)
    );
    ptr += 1;

    printf("                               "
        "     security = %s\n", binary(*ptr)
    );
    ptr += 1;

    printf("                               "
        "         misc = %s\n", binary(*ptr)
    );
    ptr += 1;

    printf("                               "
        "      voltage = %s\n", binary(*ptr)
    );
    ptr += 1;

    ptr += 3;

    printf("                               "
        "     GPS sync = %s\n", tostring(ptr, 20)
    );
    ptr += 20;

    ptr += unpacklong(ptr, &val);
    printf("                               "
        " differential = %ld (0x%08x)\n", val, (unsigned long) val
    );
}

int process(int count)
{
long val, i;
float calib, calper;
static char buf[MAXDAT], *ptr, *save, byte4, compression;

    if (fread(buf, 1, 4, stdin) != 4) exit(0);
    unpacklong(buf, &val);
    if (val > MAXDAT) {
        printf("increase MAXDAT (len=%d)\n", val);
        exit(1);
    }

    if (fread(buf, 1, val, stdin) != val) {
        perror("fread");
        exit(1);
    }

    printf("Record %d, len=%d\n", count, val);
    
    ptr = buf;
    ptr += unpacklong(ptr, &val);
    printf("\tauthentication offset = %d\n", val);

    printf("\t       authentication = %d\n", *(ptr++));
    printf("\t          compression = %d\n", (compression = *(ptr++)));
    printf("\t          sensor type = %d\n", *(ptr++));
    byte4 = *(ptr++);
    printf("\t               byte 4 = %d\n", byte4);
    printf("\t            site name = `%s'\n", tostring(ptr, 5)); ptr += 5;
    printf("\t         channel name = `%s'\n", tostring(ptr, 3)); ptr += 3;
    printf("\t        location name = `%s'\n", tostring(ptr, 2)); ptr += 2;
    printf("\t  uncompressed format = `%s'\n", tostring(ptr, 2)); ptr += 2;
    if (byte4 == 1) {
        ptr += unpackfloat(ptr, &calib);
        ptr += unpackfloat(ptr, &calper);
        printf("\t       calib & calper = %11.4e %11.4e\n", calib, calper);
    } else {
        ptr += 8;
    }
    printf("\t           time stamp = %s\n", tostring(ptr, 20)); ptr += 20;
    ptr += unpacklong(ptr, &val);
    printf("\t subframe time length = %d\n", val);
    ptr += unpacklong(ptr, &val);
    printf("\t    number of samples = %d\n", val);
    ptr += unpacklong(ptr, &val);
    printf("\t  channel status size = %d\n", val);

    if (val > 0) {
        save = ptr;
        printf("\t       channel status =");
        for (i = 0; i < val; i++) {
            if (i  && (i % 16 == 0)) printf("\n\t                       ");
            printf(" %02x", (0x00ff & ptr[i]));
        }
        ptr += val;
        printf("\n");
        decodechannelstatus(save);
    }

    ptr += unpacklong(ptr, &val);
    printf("\t            data size = %d\n", val);
    if (compression == 0 && DumpData) DumpDataValues((long *) ptr, val/4);
    ptr += val;

    ptr += unpacklong(ptr, &val);
    printf("\t       subframe count = %d\n", val);

    ptr += unpacklong(ptr, &val);
    printf("\tauthentication key id = 0x%04x\n", val);

    ptr += unpacklong(ptr, &val);
    printf("\t  authentication size = %d\n", val);

    if (val > 0) {
        printf("\t            signature =");
        for (i = 0; i < val; i++) {
            if (i && (i % 10 == 0)) printf("\n\t                       ");
            printf(" %02x", (0x00ff & ptr[i]));
        }
        ptr += val;
        printf("\n");
    }
}

int main(int argc, char **argv)
{
int i = 0;
    while (process(++i));
}
