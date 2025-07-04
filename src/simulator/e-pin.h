/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include <QString>

#include "e-node.h"

class eElement;
class Pin;

class ePin
{
    friend class eNode;

    public:
        ePin( QString id, int index );
        virtual ~ePin();

        bool isConnected() { return (m_enode!=nullptr); }

        virtual double getVoltage();

        eNode* getEnode() { return m_enode; }
        void   setEnode( eNode* enode );
        void   setEnodeComp( eNode* enode ); // The enode at other side of component

        void changeCallBack( eElement* el , bool cb=true );

        bool inverted() { return m_inverted; }
        virtual void setInverted( bool i ) { m_inverted = i; }

        inline void stampAdmitance( double data ) { if( m_enode ) m_enode->stampAdmitance( this, data ); }

        void addSingAdm( eNode* node, double admit );
        void stampSingAdm( double admit );

        void createCurrent();
        inline void stampCurrent( double data ) { if( m_enode ) m_enode->stampCurrent( this, data ); }
        
        QString getId()  { return m_id; }
        void setId( QString id );

        virtual Pin* getPin(){ return nullptr; }

        void setIndex( int i ) { m_index = i; }

        virtual double getCurrent() { return m_current; }
        void setCurrent( double c ) { m_current = c; }

        virtual bool hasCurrent() { return m_hasCurrent; }
        void setHasCurrent( bool h ) { m_hasCurrent = h; }

    protected:
        eNode* m_enode;     // My eNode
        eNode* m_enodeComp; // eNode at other side of my component

        QString m_id;
        int m_index;

        bool m_inverted;
        bool m_hasCurrent;

        double m_current;
};
