/****************************************************************************
** Set_All_Parameters meta object code from reading C++ file 'set_all_parameters.h'
**
** Created: Tue May 17 16:59:00 2005
**      by: The Qt MOC ($Id: moc_set_all_parameters.cpp,v 1.1 2005/05/18 00:06:44 dechavez Exp $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "set_all_parameters.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *Set_All_Parameters::className() const
{
    return "Set_All_Parameters";
}

QMetaObject *Set_All_Parameters::metaObj = 0;
static QMetaObjectCleanUp cleanUp_Set_All_Parameters( "Set_All_Parameters", &Set_All_Parameters::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString Set_All_Parameters::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "Set_All_Parameters", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString Set_All_Parameters::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "Set_All_Parameters", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* Set_All_Parameters::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QDialog::staticMetaObject();
    static const QUMethod slot_0 = {"languageChange", 0, 0 };
    static const QUMethod slot_1 = {"OnOk", 0, 0 };
    static const QUMethod slot_2 = {"OnLoadList", 0, 0 };
    static const QUParameter param_slot_3[] = {
	{ "row", &static_QUType_int, 0, QUParameter::In },
	{ "col", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_3 = {"OnClickStaListTableItem", 2, param_slot_3 };
    static const QUParameter param_slot_4[] = {
	{ "row", &static_QUType_int, 0, QUParameter::In },
	{ "col", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_4 = {"OnClickChanListTableItem", 2, param_slot_4 };
    static const QMetaData slot_tbl[] = {
	{ "languageChange()", &slot_0, QMetaData::Protected },
	{ "OnOk()", &slot_1, QMetaData::Protected },
	{ "OnLoadList()", &slot_2, QMetaData::Protected },
	{ "OnClickStaListTableItem(int,int)", &slot_3, QMetaData::Protected },
	{ "OnClickChanListTableItem(int,int)", &slot_4, QMetaData::Protected }
    };
    metaObj = QMetaObject::new_metaobject(
	"Set_All_Parameters", parentObject,
	slot_tbl, 5,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_Set_All_Parameters.setMetaObject( metaObj );
    return metaObj;
}

void* Set_All_Parameters::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "Set_All_Parameters" ) )
	return this;
    return QDialog::qt_cast( clname );
}

bool Set_All_Parameters::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: languageChange(); break;
    case 1: OnOk(); break;
    case 2: OnLoadList(); break;
    case 3: OnClickStaListTableItem((int)static_QUType_int.get(_o+1),(int)static_QUType_int.get(_o+2)); break;
    case 4: OnClickChanListTableItem((int)static_QUType_int.get(_o+1),(int)static_QUType_int.get(_o+2)); break;
    default:
	return QDialog::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool Set_All_Parameters::qt_emit( int _id, QUObject* _o )
{
    return QDialog::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool Set_All_Parameters::qt_property( int id, int f, QVariant* v)
{
    return QDialog::qt_property( id, f, v);
}

bool Set_All_Parameters::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
