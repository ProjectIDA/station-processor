// DateTimeEdit.h: interface for the DateTimeEdit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_DATETIMEEDIT_H_)
#define _DATETIMEEDIT_H_

#include <qlineedit.h>

class DateTimeEdit : public QLineEdit  
{
public:
	DateTimeEdit(QWidget * parent, const char * name = 0);
	virtual ~DateTimeEdit();
	void SetFormat(int nFormat);
	int nTimeFormat;
	void GetDateTime(int &yr, int &mnth, int &day, int &hh, int &mm, int &ss);
	void SetDateTime(int yr, int mnth, int day, int hh, int mm, int ss);
	bool Validate();


};

#endif // !defined(_DATETIMEEDIT_H_)
