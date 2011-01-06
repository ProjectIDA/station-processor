// RTDisplay.h: interface for the CRTDisplay class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_RTDISPLAY_H_INCLUDED_)
#define _RTDISPLAY_H_INCLUDED_

#include "platform.h"
#include <qwidget.h>
#include <qpixmap.h>
#include <qptrlist.h> 
#include <qvector.h>
#include "MWindow.h"
#include <qmutex.h>


class CRTDisplay : public QWidget  
{
public:
	CRTDisplay(QWidget * parent , const char * name , WFlags f);
	virtual ~CRTDisplay();
	enum
		{
		AutoScale0		=	QEvent::User+1,
		AutoScale1		=	QEvent::User+2,
		ExpandVert		=	QEvent::User+3,
		ContractVert	=	QEvent::User+4,
		ShiftUp			=	QEvent::User+5,
		ShiftDown		=	QEvent::User+6,
		Update          =	QEvent::User+7,
		Draw			=	QEvent::User+8
		};

public:
	QPixmap Bitmap; // off screen bitmap
	QPtrList<CMWindow> WaveformInfo; // array of all waveforms 
	QPtrList<int> tmmarks; // time marks array 

	QMutex Critical;
	QMutex ScreenBitmapCriticalSection;

	int MaxX, MaxY;
	BOOL bRepaint;

	double t1,t2;
	double tB1,tB2;
	int nTimerCount;
	int nActiveScreen;
	int TimeBarYsize;

// Operations
public:

	void AddWindow(QString Sta, QString Chan, QString LCODE, double dSpS);
	void RemoveWindow(QString Sta, QString Chan, QString LCODE);
	void Run();
	void Stop();
	void DrawWaveforms();
	void OnTimer();
	void RemoveOldData();
	void OnAutoScale(int nType);
	void OnExpandVert();
	void OnContractVert();
	void OnShiftUpDown(int nDirection);

	void DrawTimeMark(int x, double tt, QPainter *pDC);
	void DrawTimeBounds(QPainter *pDC);
	int nWindowFromPoint(int x, int y);
	void resizeEvent(QResizeEvent * );
	void paintEvent (QPaintEvent  * );
	void customEvent(QCustomEvent *event);

private:
	int nLastMarkRightPos; 
	bool bDrawBitmap;
	int WindowInfoXsize;

protected:
	void SetTime();
	void CalculateWindowsSize();
	void ClearWaveformInfo();
	void DrawFocused(QPainter  *pDC);
	void GetTextRectangle(const char *lpText, QRect &rc);
	void DrawTimePanel(QPainter *pDC);
	void DrawWindow(int nWindow, QPainter  *pDC);
	void ClearDisplay();
	void ApplyAutoscale(int nWindow, int nType);
	void DrawGaps(int nWindow, QPainter *pDC);
	void ApplyExpand(int nWindow);
	void ApplyContract(int nWindow);
	void mousePressEvent (QMouseEvent *e );
	void mouseMoveEvent ( QMouseEvent *e );

};

#endif // !defined(_RTDISPLAY_H_INCLUDED_)
