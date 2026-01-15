/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include <QPointF>
#include <QStringView>

QPointF getPointF( QString p );

QString getStrPointF( QPointF p );


//---------------------------------------------------

struct propStr_t{
    QString name;
    QString value;
};

QVector<propStr_t> parseXmlProps( QString line );
QVector<propStr_t> parseProps( QString line );
propStr_t parseProp( QString token );

