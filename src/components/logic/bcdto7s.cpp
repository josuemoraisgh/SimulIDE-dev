/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "bcdto7s.h"
#include "itemlibrary.h"
#include "simulator.h"
#include "iopin.h"
#include "propdialog.h"

#include "boolprop.h"
#include "stringprop.h"

#define tr(str) simulideTr("BcdTo7S",str)

Component* BcdTo7S::construct( QString type, QString id )
{ return new BcdTo7S( type, id ); }

LibraryItem* BcdTo7S::libraryItem()
{
    return new LibraryItem(
        tr("Bcd To 7S."),
        "Converters",
        "2to3g.png",
        "BcdTo7S",
        BcdTo7S::construct );
}

BcdTo7S::BcdTo7S( QString type, QString id )
       : BcdBase( type, id )
{
    m_width  = 4;
    m_height = 8;

    m_useReset = false;
    m_tristate = true;

    init({         // Inputs:
            "IL03S0",
            "IL04S1",
            "IL05S2",
            "IL06S3",
                    // Outputs:
            "OR01a",
            "OR02b",
            "OR03c",
            "OR04d",
            "OR05e",
            "OR06f",
            "OR07g"
        });

    createOePin("IU01OE ", id+"-in4");

    m_resetPin = new IoPin( 180, QPoint(-24, 8 ), m_id+"-Pin_reset", 0, this, input );
    setupPin( m_resetPin,"L02RST");
    m_resetPin->setVisible( false );
    m_resetPin->setInverted( true ); //setResetInv( true );    // Invert Reset Pin
    m_otherPin.emplace_back( m_resetPin );

    Simulator::self()->addToUpdateList( this );

    addPropGroup( { tr("Main"), {
        new StrProp<BcdTo7S>("Custom_Segments", tr("Custom Characters CSV"),""
                            , this, &BcdTo7S::customChars, &BcdTo7S::setCustomChars, propNoCopy ),

        new BoolProp<BcdTo7S>("UseRS", tr("Reset Pin"),""
                             , this, &BcdTo7S::pinReset, &BcdTo7S::setPinReset, propNoCopy ),
    },0} );

    appendPropGroup( tr("Main"), IoComponent::familyProps() );

    addPropGroup( { tr("Inputs")
        , IoComponent::inputProps()
        + QList<ComProperty*>({
            new BoolProp<BcdTo7S>("Invert_Inputs", tr("Invert Inputs"),""
                                 , this, &BcdTo7S::invertInps, &BcdTo7S::setInvertInps, propNoCopy )})
    ,0 } );

    addPropGroup( { tr("Outputs")
        , IoComponent::outputProps()
        + IoComponent::outputType()
        + QList<ComProperty*>({
            new BoolProp<BcdTo7S>("Tristate", tr("Tristate"),""
                                  , this, &BcdTo7S::tristate, &BcdTo7S::setTristate ),
        })
    ,0 } );

    addPropGroup( { tr("Timing"), IoComponent::edgeProps(),0 } );
}
BcdTo7S::~BcdTo7S(){}

bool BcdTo7S::propNotFound( QString prop, QString val )
{
    if( prop =="Reset_Inverted" ) // Old circuits
    {
        bool invert = (val == "true");
        m_resetPin->setInverted( invert );
        return true;
    }
    return false;
}

void BcdTo7S::stamp()
{
    BcdBase::stamp();

    m_resetPin->changeCallBack( this, m_useReset );
}

void BcdTo7S::updateStep()
{
    if( !m_changed ) return;
    m_changed = false;

    if( !Simulator::self()->isRunning() ) m_digit = 0;
    else                                  voltChanged();

    update();
}

void BcdTo7S::voltChanged()
{
    LogicComponent::updateOutEnabled();
    BcdBase::voltChanged();
    if( m_useReset && m_resetPin->getInpState() ) m_nextOutVal = 0;
    else                                          m_nextOutVal = m_digit;
    scheduleOutPuts( this );
}

void BcdTo7S::setPinReset( bool r )
{
    m_useReset = r;
    m_resetPin->setVisible( r );
    if( !r ) m_resetPin->removeConnector();

    updtProperties();
}
