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

        inline void stampAdmitance( double a ) { if( m_enode ) m_enode->stampAdmitance( this, a ); }

        void addSingAdm( eNode* node, double admit );
        void stampSingAdm( double admit );

        void createCurrent();
        inline void stampCurrent( double c ) { if( m_enode ) { m_sourceCurrent = c; m_enode->stampCurrent( this, c ); } }
        
        QString getId()  { return m_id; }
        void setId( QString id );

        virtual Pin* getPin(){ return nullptr; }

        void setIndex( int i ) { m_index = i; }

        void setCircuitPin( ePin* p ) { m_circuitPin = p; }

        virtual double getCurrent() { return m_current; }
        void setCurrent( double c ) { m_current = c; }

        virtual bool hasCurrent() { return m_hasCurrent; }
        void setHasCurrent( bool h ) { m_hasCurrent = h; }

        void resetCurrent() { m_hasCurrent = false; m_current = 0; }

    protected:
        eNode* m_enode;     // My eNode
        eNode* m_enodeComp; // eNode at other side of my component

        QString m_id;
        int m_index;

        bool m_inverted;
        bool m_hasCurrent;

         ePin* m_circuitPin;

        double m_current;
        double m_sourceCurrent;
};
