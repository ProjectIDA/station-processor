#include <qpopupmenu.h>
#include <qpixmap.h>
#include <qapplication.h>
#include <qmainwindow.h> 
#include <qmenubar.h>
#include <qvbox.h> 
#include <qlineedit.h> 
#include <qsettings.h>
#include "isitapdlg.h"
#include "platform.h"

int main(int argc,char *argv[])
	{

	QApplication a( argc, argv );

	int nFontSize = 18;

	QFont f("times", nFontSize);
//	a.setFont ( f, nFontSize );



	isiTapDlg dlg;
    dlg.show();
	a.connect( &a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()) );
    a.setMainWidget( &dlg );

	a.exec();

	return 0;
	}
