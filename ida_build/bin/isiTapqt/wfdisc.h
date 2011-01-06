#if !defined(_WFDISC_H_)
#define _WFDISC_H_

//#include "wfdiscio.h"

#include <qstring.h>
#include "isi.h"
#include "util.h"
#include "css/3.0/wfdiscio.h"
#include <qstring.h>

class CWfdisc  
{
public:
	CWfdisc();
	virtual ~CWfdisc();
	int WriteWfdisk(const char  *pName);
	int WriteData(ISI_GENERIC_TS *ts, const char * datatype );
	void SetWorkingDir(const QString &s);
	void CloseFileHandle();
public:
	QString qsDir;
	struct wfdisc wfd;
	int hF;
};

#endif // !defined(_WFDISC_H_)
