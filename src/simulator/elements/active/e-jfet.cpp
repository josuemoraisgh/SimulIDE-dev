/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QtMath>   // qFabs(x,y)
//#include <QDebug>

#include "e-jfet.h"
#include "simulator.h"
#include "e-pin.h"
#include "e-node.h"
#include "utils.h"

eJfet::eJfet( QString id )
     : eResistor( id )
{
    m_Pchannel  = false;// Currently not allowed to change.
    
    m_Idss      = 0.050;// [A]
    m_Vp        = -3;   // [V]
    m_LambdaInv    = 1000;// [Vds] (i.e., slope of Idss in saturation region that also impacts triode region accordingly)

    m_ePin.resize(3);
}
eJfet::~eJfet(){}

void eJfet::initialize()
{
    m_step = 0;
    m_gateV = eElement::cero_doub;
    m_lastCurrent = eElement::cero_doub;
    m_accuracy = 1;
    m_beta = m_Idss/(qPow(m_Vp,2));
    m_admit = eElement::cero_doub;
}

void eJfet::stamp()
{
    updateValues();
    if( (m_ePin[0]->isConnected())
      &&(m_ePin[1]->isConnected()) )
    {
        eResistor::stamp();

        m_ePin[0]->createCurrent();
        m_ePin[1]->createCurrent();

        m_ePin[0]->getEnode()->addToNoLinList(this);
        m_ePin[1]->getEnode()->addToNoLinList(this);
    }
    if( m_ePin[2]->isConnected() )
    {
        m_ePin[2]->getEnode()->addToNoLinList(this);
    }
    // if( m_ePin[3]->isConnected() )
    // {
    //     m_ePin[3]->getEnode()->addToNoLinList(this);
    //     m_ePin[3]->createCurrent();
    //     m_ePin[3]->stampAdmitance(1000);
    // }
}

void eJfet::voltChanged()
{
    int direction = 1;

    double Vd = m_ePin[0]->getVoltage();
    double Vs = m_ePin[1]->getVoltage();

    // if( m_Pchannel ){ Vgs = -Vgs; Vds = -Vds; } // Unused for now. Forced to N-channel for now with m_Pchannel=False
    if(Vd < Vs)
    {
        // Use same approach as LTSpice. https://www.reddit.com/r/ECE/comments/md5l0i/nchannel_jfet_gate_drain_voltage_vgd_question/
        // Virtually switch which pin is considered the source for calculations
        // if the drain voltage is lower than the source voltage
        Vd = m_ePin[1]->getVoltage();
        Vs = m_ePin[0]->getVoltage();
        direction = -1;
    }
    double Vg = m_ePin[2]->isConnected() ? m_ePin[2]->getVoltage() : Vs;
    double Vgs = Vg-Vs;
    double Vds = Vd-Vs; // Should always be positive using virtual switching method above
    double gateV = Vgs - m_Vp; // Vgs - Vgs(off)
    double tempAdmit = eElement::cero_doub;

    double current = eElement::cero_doub; // initialize variable

    // This channel length correction taken from (https://www.allaboutcircuits.com/textbook/semiconductors/chpt-5/simulating-jfet-circuits-using-ltspice/)
    double correction = ( 1+Vds/m_LambdaInv );

    if( Vgs <= m_Vp )
    {
        gateV = eElement::cero_doub;
        current = eElement::cero_doub;
        tempAdmit = eElement::cero_doub;
    }
    else //(i.e., Vgs > m_Vp)
    {
        if( Vds < gateV)
        {
            current = m_beta*( 2*( gateV )*Vds-qPow( Vds , 2 ))*correction;
            tempAdmit = current/Vds;
        }
        else //(i.e., Vds >= gateV
        {
            current = m_beta*qPow( Vgs-m_Vp, 2 )*correction;
            tempAdmit = current/Vds;
        }
    }

    // if( m_Pchannel ) current = -current;



    if( qFabs(current-m_lastCurrent)<m_accuracy )
    {
        if( tempAdmit != m_admit ){
            eResistor::setAdmit( tempAdmit );
            eResistor::stamp();
        }
        return; // Converged
    }

    Simulator::self()->notCorverged();
    if( tempAdmit != m_admit ){
        eResistor::setAdmit( m_admit + 0.001*(tempAdmit-m_admit));
        eResistor::stamp();
    }


    m_gateV = gateV;
    m_lastCurrent = current;
    m_ePin[0]->stampCurrent( (current-m_lastCurrent)*direction );
    m_ePin[1]->stampCurrent(-1*(current-m_lastCurrent)*direction );
    // m_ePin[3]->stampCurrent(m_admit*1000); // Pin admittance is 1000. Setting 'voltage' equal to Jfet admittance to watch with oscope
}

void eJfet::setIdss( double Idss )
{
    Idss = qFabs(Idss);
    if( Idss > 1000 )
    {
        Idss = 1000;
    }
    m_Idss = Idss;
    m_changed = true;
}

void eJfet::setVp( double Vp )
{
    // if(Vp < 0)
    // {
    //     if(m_Pchannel)
    //     {
    //         Vp = -1*Vp;
    //     }
    // }
    // else
    // {
    //     if(!m_Pchannel)
    //     {
    //         Vp = -1*Vp;
    //     }
    // }
    m_Vp = Vp;
    m_changed = true;
}

void eJfet::setLambdaInv( double LambdaInv )
{
    if(LambdaInv < 1)
    {
        LambdaInv = 1;
    }
    m_LambdaInv = LambdaInv;
    m_changed = true;
}

void eJfet::updateValues()
{}
