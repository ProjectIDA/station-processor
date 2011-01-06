#include "sanio.h"
#include <errno.h>
#include <ctype.h>
#include "ida10.h"

int main(int argc, char **argv)
{
int status, RecordType;
UINT8 buf[IDA10_MAXRECLEN];
IDA10_LM lm;
long recno;

    recno = 0;
    while ((status = ida10ReadRecord(stdin, buf, IDA10_MAXRECLEN, &RecordType)) != IDA10_EOF) {
        ++recno;
        if (status != IDA10_OK) {
            fprintf(stderr, "ida10ReadRecord error %d: %s\n", status, ida10ErrorString(status));
        } else if (RecordType == IDA10_TYPE_LM) {
            if (!ida10UnpackLM(buf, &lm)) {
            	perror("ida10UnpackLM");
			} else {
				printf("%s", lm.text);
			}
        }
    }
    exit(0);
}
