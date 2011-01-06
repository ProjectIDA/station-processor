#include <qpopupmenu.h>
#include <qpixmap.h>
#include <qapplication.h>
#include <qmainwindow.h> 
#include <qmenubar.h>
#include <qvbox.h> 
#include <qlineedit.h> 
#include <qsettings.h>
#include "MainFrame.h"
#include "CRTDglob.h"


QString WINDOWS_REGISTRY = "/nrtsutil/CRTD/display/";
const QString APP_KEY_C = "/display/";

bool bPrintUsage=false;
void PrintUsage();
void ParseCmd(int argc,char *argv[]);

void LoadConfig()
	{
	double dSpS;
	BOOL bSelected;
	QStringList stalist, chanlist, lcodelist;
	QSettings settings;

	CNRTSSTAINFO *si;

	QString qs;



#ifdef WIN32
//	settings.insertSearchPath( QSettings::Windows, WINDOWS_REGISTRY);
#else
	//settings.insertSearchPath( QSettings::Unix, "./" );
#endif
	
	settings.beginGroup(WINDOWS_REGISTRY);
	stalist = settings.subkeyList(0);

	QStringList::Iterator it = stalist.begin();
    for( ;it != stalist.end(); ++it) 
		{
		si=new 	CNRTSSTAINFO((*it).ascii());
		qs = APP_KEY_C+*it;
		settings.beginGroup(*it);
		chanlist = settings.subkeyList(0);
		QStringList::Iterator chanit = chanlist.begin();
		while( chanit != chanlist.end() ) 
			{
			QString qChan = (*chanit).ascii();
			settings.beginGroup(*chanit);
			lcodelist = settings.subkeyList(0);
			QStringList::Iterator lcodeit = lcodelist.begin();
			while( lcodeit != lcodelist.end() ) 
				{
				settings.beginGroup(*lcodeit);
				lcodelist = settings.subkeyList(0);

				dSpS = settings.readDoubleEntry(qs,1);
				bSelected = settings.readBoolEntry(qs,0);
				si->AddChan(*chanit, *lcodeit, dSpS, bSelected);
				++lcodeit;
				settings.endGroup();
				}
			++chanit;
			settings.endGroup();
			}
		settings.endGroup();
		}
	settings.endGroup();

	}



int main(int argc,char *argv[])
	{
	QApplication a( argc, argv );

	ParseCmd(argc,argv);
	if(bPrintUsage) 
		{
		PrintUsage();
		return 0;
		}
	nFontSize = 12;

	QFont f("times", nFontSize);
	a.setFont ( f, nFontSize );

//	LoadConfig();
	LoadGlobalParam();
	NrtsInfo.LoadConfigFromFile();

	MainFrame mw( 0, "CRTD" );
    mw.show();
	a.connect( &a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()) );
    a.setMainWidget( &mw );

	a.exec();

	return 0;
	}

void ParseCmd(int argc,char *argv[])
	{
	QString qSc;
	for(int i=0; i<argc; ++i)
		{
		QString s=argv[i];
		s = s.upper();

		if(qstrncmp(s, "-FONTSIZE=",10) ==0)
			{
			s=s.remove(0,10);
			nFontSize = s.toLong();
			if(nFontSize<6||nFontSize>15) nFontSize=10;
			continue;
			}

		if(qstrncmp(s,"CHLIST=",7) ==0)
			{
			qParamFileName = s.remove(0,7); 
			continue;
			}

		if(qstrncmp(s,"-H",2) ==0)
			{
			bPrintUsage = true;
			return;
			}


		}
	}
void PrintUsage()
	{
	QMessageBox::information(NULL, "CRTDQT(ISI)",
"\nUsage: crtdqt [-help] [-chlist=<file name>] [-fontsize=<6..15>]"
"\nwhere	-help display this help message"
"\n	-chlist=file name to save/restore downloaded from NRTS server channel list"
"\n	-fonsize=desired font size. Integer value between 6 and 15. ",QMessageBox::Information
	);
	}