#if !defined(_PREVIEW_H_INCLUDED_)
#define _PREVIEW_H_INCLUDED_

#include "platform.h"
#include "RTDisplay.h"
#include <qwidget.h>
#include <qpixmap.h>
#include <qptrlist.h> 
#include <qvector.h>

/////////////////////////////////////////////////////////////////////////////
// CPreview window

class CPreview : public QWidget
{
public:
	CPreview(CRTDisplay *pDisplay, QWidget * parent, const char * , WFlags f);
	virtual ~CPreview();

public:
	void DrawPreviewBitmap();
	void Stop();

private:
	void DrawWaveform(QPainter &DC);
	QMutex ScreenBitmapCriticalSection;
	void paintEvent(QPaintEvent *);
	void resizeEvent(QResizeEvent *);



public:
	QPixmap Bitmap;
	int MaxX, MaxY;
	CRTDisplay *pDisplay;
	int x1,x2,y1,y2,xw1, xw2, yw1, yw2;

	enum
		{
		DrawPreview	=	QEvent::User+1
		};


public:

public:
protected:
	bool bDrawBitmap;
	void mouseMoveEvent ( QMouseEvent * e );
	void mousePressEvent ( QMouseEvent *e );  
	void customEvent(QCustomEvent *event);


};


#endif // _PREVIEW_H_INCLUDED_
