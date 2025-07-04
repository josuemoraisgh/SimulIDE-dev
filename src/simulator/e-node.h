/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include<QHash>

class ePin;
class Node;
class eElement;

class eNode
{
    friend class IoPin;

    public:
        eNode( QString id );
        ~eNode();

        QString itemId() { return m_id; }

        void addEpin( ePin* epin );
        void remEpin( ePin* epin );
        void clear();

        void voltChangedCallback( eElement* el );
        void remFromChangedCallback( eElement* el );

        void addToNoLinList( eElement* el );
        //void remFromNoLinList( eElement* el );

        void addConnection( ePin* epin, eNode* node );
        void stampAdmitance( ePin* epin, double admit );

        void addSingAdm( ePin* epin, eNode* node, double admit );
        void stampSingAdm( ePin* epin, double admit );

        void createCurrent( ePin* epin );
        void stampCurrent( ePin* epin, double current );

        int  getNodeNumber() { return m_nodeNum; }
        void setNodeNumber( int n ) { m_nodeNum = n; }

        void setNodeGroup( int n ){ m_nodeGroup = n; }

        double getVolt() { return m_volt; }
        void   setVolt( double volt );

        void initialize();
        void stampMatrix();

        void setSingle( bool single ) { m_single = single; } // This eNode can calculate it's own Volt

        void updateConnectors();
        void updateCurrents();

        void addNodeComp( Node* n );

        QList<ePin*> getEpins() { return m_ePinList; }

        QList<int> getConnections();

        eNode* nextCH;

    private:
        class Connection
        {
            public:
                Connection( ePin* e, eNode* n=nullptr, double v=0 )
                { epin = e; node = n; value = v;
                  if( n ) nodeNum = n->getNodeNumber(); }
                ~Connection(){;}

                Connection* next = nullptr;
                ePin*  epin = nullptr;
                eNode* node = nullptr;
                int    nodeNum = 0;
                double value;
        };
        class CallBackElement
        {
            public:
                CallBackElement( eElement* el ) { element = el; }
                ~CallBackElement(){;}

                CallBackElement* next;
                eElement* element;
        };

        inline void changed();

        inline void solveSingle();

        void clearElmList( CallBackElement* first );
        void clearConnList( Connection* first );

        QString m_id;

        QList<ePin*> m_ePinList;

        CallBackElement* m_voltChEl;
        CallBackElement* m_nonLinEl;

        Connection* m_firstAdmit;   // Stamp full admitance in Admitance Matrix
        Connection* m_firstSingAdm; // Stamp single value   in Admitance Matrix
        Connection* m_firstCurrent; // Stamp value in Current Vector
        Connection* m_nodeAdmit;

        QList<int> m_nodeList;
        QList<Node*> m_nodeCompList;

        double m_totalCurr;
        double m_totalAdmit;
        double m_volt;

        int m_nodeNum;
        int m_nodeGroup;

        bool m_currChanged;
        bool m_admitChanged;
        bool m_voltChanged;
        bool m_changed;
        bool m_single;
};
