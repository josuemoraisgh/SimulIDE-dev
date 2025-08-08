/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef EJFET_H
#define EJFET_H

#include "e-resistor.h"

class eJfet : public eResistor
{
    public:
        eJfet( QString id );
        ~eJfet();

        virtual void initialize() override;
        virtual void stamp() override;
        virtual void voltChanged() override;

        // bool pChannel() { return m_Pchannel; }
        // virtual void setPchannel( bool p ) { m_Pchannel = p; m_changed = true; }

        double Idss() { return m_Idss; }
        void setIdss( double Idss );

        double Vp() { return m_Vp; }
        void setVp( double Vp );

        double LambdaInv() { return m_LambdaInv; }
        void setLambdaInv( double LambdaInv );
        
    protected:
        void updateValues();

        double m_accuracy;
        double m_lastCurrent;
        double m_lastAdmit;
        double m_gateV;
        double m_beta;
        // double m_admit; // Field from e-resistor

        // Component properties
        bool m_Pchannel;    // Not user defineable yet
        double m_Idss;      // Idss = Ids when gate "shorted" to source (i.e., nearly maxCurrent). Idss is specified in datasheets.
        double m_Vp;        // Pinch off voltage or Vgs(off), specified in datasheets
        double m_LambdaInv;    // Not specified in datasheets but (nearly) identical to lambda in LTSpice accounting for channel length
};

#endif
