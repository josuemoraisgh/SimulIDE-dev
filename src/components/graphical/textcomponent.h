/***************************************************************************
 *   Copyright (C) 2017 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "component.h"
#include "linkercomponent.h"

class LibraryItem;

class TextComponent : public LinkerComponent
{
    public:
        TextComponent( QString type, QString id );
        ~TextComponent();
        
        QRectF boundingRect() const override
        { 
            return QRectF( m_area.x()-m_border/2-1, m_area.y()-m_border/2-1, 
                           m_area.width()+m_border+2, m_area.height()+m_border+2 ); 
        }

 static Component* construct( QString type, QString id );
 static LibraryItem* libraryItem();

        void updateStep() override;

        int  margin();
        void setMargin( int margin );
        
        int  border() { return m_border; }
        void setBorder( int border ) { m_border = border; update(); }
        
        QString getFont() { return m_font; }
        void    setFont( QString font );

        int  fontSize() { return m_fontSize; }
        void setFontSize( int size );

        QString fontColor() { return m_fontColor; }
        void setFontColor( QString n );

        QString colorStr() { return m_color.name(); }
        void setColorStr( QString n );
        
        bool fixedW() { return m_fixedW; }
        void setFixedW( bool fixedW );

        QString getText();
        void    setText( QString text );

        qreal opac() { return m_opac; }
        void setOpac( qreal op );

        void createLinks( QList<Component*>*compList ) override;
        void compSelected( Component* comp ) override;
        void setLinkedString( QString str, int i=0 ) override;
        void setLinkedValue( double v, int i=0 ) override;

        void updateGeometry(int, int, int);

        bool freeMove( bool ) override { return true; }

        void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;

    private:
        QGraphicsTextItem* m_text;
        QString m_textString;

        qreal m_opac;

        bool m_changed;
        
        int  m_fontSize;
        int  m_docMargin;
        int  m_margin;
        int  m_border;

        bool m_fixedW;
        bool m_context;
        
        QString m_font;
        QString m_fontColor;
};
