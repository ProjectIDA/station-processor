/****************************************************************************
** MainFrame meta object code from reading C++ file 'MainFrame.h'
**
** Created: ?? 1. ??? 13:19:27 2005
**      by: The Qt MOC ($Id: moc_crtdqtui.cpp,v 1.1 2005/05/18 00:06:44 dechavez Exp $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "MainFrame.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *MainFrame::className() const
{
    return "MainFrame";
}

QMetaObject *MainFrame::metaObj = 0;
static QMetaObjectCleanUp cleanUp_MainFrame( "MainFrame", &MainFrame::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString MainFrame::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "MainFrame", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString MainFrame::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "MainFrame", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* MainFrame::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QMainWindow::staticMetaObject();
    static const QUMethod slot_0 = {"helpIndex", 0, 0 };
    static const QUMethod slot_1 = {"helpContents", 0, 0 };
    static const QUMethod slot_2 = {"helpAbout", 0, 0 };
    static const QUMethod slot_3 = {"configButton_clicked", 0, 0 };
    static const QUMethod slot_4 = {"newSlot", 0, 0 };
    static const QUMethod slot_5 = {"runButton_clicked", 0, 0 };
    static const QUMethod slot_6 = {"autoButton_clicked", 0, 0 };
    static const QUMethod slot_7 = {"autoButton1_clicked", 0, 0 };
    static const QUMethod slot_8 = {"expandButton_clicked", 0, 0 };
    static const QUMethod slot_9 = {"contractButton_clicked", 0, 0 };
    static const QUMethod slot_10 = {"upButton_clicked", 0, 0 };
    static const QUMethod slot_11 = {"downButton_clicked", 0, 0 };
    static const QUMethod slot_12 = {"exitButton_clicked", 0, 0 };
    static const QUMethod slot_13 = {"stopButton_clicked", 0, 0 };
    static const QUMethod slot_14 = {"aboutButton_clicked", 0, 0 };
    static const QUMethod slot_15 = {"languageChange", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "helpIndex()", &slot_0, QMetaData::Public },
	{ "helpContents()", &slot_1, QMetaData::Public },
	{ "helpAbout()", &slot_2, QMetaData::Public },
	{ "configButton_clicked()", &slot_3, QMetaData::Public },
	{ "newSlot()", &slot_4, QMetaData::Public },
	{ "runButton_clicked()", &slot_5, QMetaData::Public },
	{ "autoButton_clicked()", &slot_6, QMetaData::Public },
	{ "autoButton1_clicked()", &slot_7, QMetaData::Public },
	{ "expandButton_clicked()", &slot_8, QMetaData::Public },
	{ "contractButton_clicked()", &slot_9, QMetaData::Public },
	{ "upButton_clicked()", &slot_10, QMetaData::Public },
	{ "downButton_clicked()", &slot_11, QMetaData::Public },
	{ "exitButton_clicked()", &slot_12, QMetaData::Public },
	{ "stopButton_clicked()", &slot_13, QMetaData::Public },
	{ "aboutButton_clicked()", &slot_14, QMetaData::Public },
	{ "languageChange()", &slot_15, QMetaData::Protected }
    };
    metaObj = QMetaObject::new_metaobject(
	"MainFrame", parentObject,
	slot_tbl, 16,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_MainFrame.setMetaObject( metaObj );
    return metaObj;
}

void* MainFrame::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "MainFrame" ) )
	return this;
    return QMainWindow::qt_cast( clname );
}

bool MainFrame::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: helpIndex(); break;
    case 1: helpContents(); break;
    case 2: helpAbout(); break;
    case 3: configButton_clicked(); break;
    case 4: newSlot(); break;
    case 5: runButton_clicked(); break;
    case 6: autoButton_clicked(); break;
    case 7: autoButton1_clicked(); break;
    case 8: expandButton_clicked(); break;
    case 9: contractButton_clicked(); break;
    case 10: upButton_clicked(); break;
    case 11: downButton_clicked(); break;
    case 12: exitButton_clicked(); break;
    case 13: stopButton_clicked(); break;
    case 14: aboutButton_clicked(); break;
    case 15: languageChange(); break;
    default:
	return QMainWindow::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool MainFrame::qt_emit( int _id, QUObject* _o )
{
    return QMainWindow::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool MainFrame::qt_property( int id, int f, QVariant* v)
{
    return QMainWindow::qt_property( id, f, v);
}

bool MainFrame::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
