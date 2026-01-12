/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QStringList>
#include <QPointF>
#include <QVector>
#include "proputils.h"

QPointF getPointF( QString p )
{
    QStringList plist = p.split(",");
    QPointF point( plist.first().toDouble(), plist.last().toDouble() );
    return point;
}
QString getStrPointF( QPointF p )
{
    return QString::number( p.x())+","+QString::number(p.y() );
}

//---------------------------------------------------

QVector<propStr_t> parseXmlProps( QString line )
{
    QVector<propStr_t> properties;

    QString name;
    QStringList tokens = line.split("\"");
    tokens.removeLast();

    for( QString token : tokens )
    {
        if( name.isEmpty() ){
            int start = token.lastIndexOf(" ")+1;
            name = token.mid( start, token.length()-start-1 );
        }else{
            properties.append( { name, token } );
            name.clear();
        }
    }
    return properties;
}

QVector<propStr_t> parseProps( QString line )
{
    QVector<propStr_t> properties;

    QList<QString> tokens = line.split(';');

    for( QString token : tokens )
    {
        propStr_t property = parseProp( token );
        if( property.name.size() ) properties.append( property );
    }
    return properties;
}

propStr_t parseProp( QString token )
{
    QString name;       // Property_name
    QString value;      // Property value
    int index = token.indexOf("="); // First occurrence of "="

    if( index == -1 ) name = token;
    else{
        name  = token.left( index );
        value = token.right( token.size()-1-index );
    }
    if( name.indexOf(" ") == 0)   // Only remove leading spaces
    {
        index = name.lastIndexOf(" ");
        name = name.right( name.size()-index-1 ); // Remove spaces
    }
    return { name, value };
}

