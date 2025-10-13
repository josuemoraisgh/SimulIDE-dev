/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

//#include <math.h>

#include "valuewidget.h"
#include "mainwindow.h"
#include "watched.h"
#include "utils.h"

ValueWidget::ValueWidget( QString name, QString type, QString unit, Watched* core, QWidget* parent )
           : QWidget( parent )
{
    setupUi(this);

    m_name = name;
    m_type = type.toLower();
    m_watched = core;

    if( unit.isEmpty() ) unit = m_type;

    float scale = MainWindow::self()->fontScale();
    int fontSize = round(12.5*scale);
    int valueWidth = round(120*scale);

    QFont font;
    font.setFamily("Ubuntu Mono");
    font.setBold( true );
    font.setPixelSize( fontSize );

    nameLabel->setFont( font );
    nameLabel->setText( m_name );

    typeLabel->setFont( font );
    typeLabel->setText( unit );

    valueLine->setFont( font );
    valueLine->setFixedWidth( valueWidth );
    valueLine->setReadOnly( true );
}

void ValueWidget::updateValue()
{
    if( !m_watched ) return;

    if( m_type == "string" ) setValueStr( m_watched->getStrReg( m_name ) );
    if( m_type == "double" ) setValueDbl( m_watched->getDblReg( m_name ) );
    else                     setValueInt( m_watched->getIntReg( m_name ) );
}

void ValueWidget::setValueDbl( double val )
{
    m_strVal = QString::number(val);
    valueLine->setText( m_strVal );
}

void ValueWidget::setValueInt( int val )
{
    if( m_type.endsWith("8") )        // 8 bits
    {
        val = val  & 0xFF;
        QString decStr = decToBase( val, 10, 3 );
        QString hexStr = decToBase( val, 16, 2 );
        QString binStr = decToBase( val,  2, 8 );
        m_strVal = decStr+" 0x"+hexStr+" "+binStr;
    }
    else if( m_type.endsWith("16") ) // 16 bits
    {
        val = val  & 0xFFFF;
        QString decStr = decToBase( val, 10, 5 );
        QString hexStr = decToBase( val, 16, 4 );
        m_strVal = decStr+"  0x"+hexStr;
    }
    else                            // Unknown
    {
        m_strVal = QString::number(val);
    }
    valueLine->setText( m_strVal );
}

void ValueWidget::setValueStr( QString str )
{
    valueLine->setText( str );
}
