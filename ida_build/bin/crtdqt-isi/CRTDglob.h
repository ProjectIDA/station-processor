#include "nrtsinfo.h"

extern BOOL bContinue;
extern QString server;
extern int nNrtsPort;
extern int nDisplayedTime;
extern int nBufferedTime;
extern int nRefreshTime;
extern int nDelay;
extern char *pIniFile;

void LoadGlobalParam();
void SaveGlobalParam();

extern CNRTSINFO		NrtsInfo;

extern bool bShiftButtonPressed;
extern QString qParamFileName;

extern int nFontSize;
