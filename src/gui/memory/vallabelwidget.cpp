/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

//#include <math.h>

#include "vallabelwidget.h"
#include "watched.h"
#include "utils.h"

ValLabelWidget::ValLabelWidget( QString name, QString type, QString unit, Watched* core, QWidget* parent )
              : QWidget( parent )
{
    setupUi(this);

    m_name = name;
    m_type = type.toLower();
    m_watched = core;

    if( unit.isEmpty() ) unit = m_type;

    QFont font;
    font.setFamily("Ubuntu Mono");
    font.setBold( true );
    font.setPixelSize( 9 );

    nameLabel->setFont( font );
    nameLabel->setText( m_name );
    nameLabel->setFixedHeight( 11 );

    typeLabel->setFont( font );
    typeLabel->setText( unit );
    typeLabel->setFixedHeight( 11 );
    typeLabel->setMinimumWidth( 11 );

    valueLabel->setFont( font );
    valueLabel->setFixedWidth( 50 );
    valueLabel->setFixedHeight( 11 );
    valueLabel->setText("...");
}

void ValLabelWidget::updateValue()
{
    if( !m_watched ) return;

    if( m_type == "string" ) setValueStr( m_watched->getStrReg( m_name ) );
    if( m_type == "double" ) setValueDbl( m_watched->getDblReg( m_name ) );
    else                     setValueInt( m_watched->getIntReg( m_name ) );
}

void ValLabelWidget::setValueDbl( double val )
{
    m_strVal = QString::number(val);
    valueLabel->setText( m_strVal );
}

void ValLabelWidget::setValueInt( int val )
{
    m_strVal = QString::number(val);
    valueLabel->setText( m_strVal );
}

void ValLabelWidget::setValueStr( QString str )
{
    valueLabel->setText( str );
}
