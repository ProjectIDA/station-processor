SCSI* mtopen(char *dev, char *cpMode);
int mtread(SCSI *tp, unsigned char *buffer, int count);
void mtclose(SCSI *pSCSI);


