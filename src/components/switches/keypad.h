/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "e-element.h"
#include "push_base.h"
#include "e-diode.h"
class LibraryItem;

class KeyPad : public Component, public eElement
{
    public:
        KeyPad( QString type, QString id );
        ~KeyPad();

        void initialize() override;

 static Component* construct( QString type, QString id );
 static LibraryItem* libraryItem();

        int rows() { return m_rows; }
        void setRows( int rows );

        int cols() { return m_cols; }
        void setCols( int cols );
        
        QString keyLabels() { return m_keyLabels; }
        void setKeyLabels( QString keyLabels );
        
        void stamp() override;
        void remove() override;

        bool hasDiodes() { return m_hasDiodes; }
        void setHasDiodes( bool d );

        bool direction(){ return m_direction; }
        void setDirection( bool dir );
       
        void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;

    protected:
        void setflip() override;
        void setupButtons( int newRows, int newCols );

        int m_rows;
        int m_cols;
        
        bool m_hasDiodes;
        bool m_direction;
        
        QString m_keyLabels;
        
        QList<PushBase*> m_buttons;
        QList<eDiode*>   m_diodes;
        QList<eNode*>    m_enodes;

        std::vector<Pin*> m_rowPins;
        std::vector<Pin*> m_colPins;
};
