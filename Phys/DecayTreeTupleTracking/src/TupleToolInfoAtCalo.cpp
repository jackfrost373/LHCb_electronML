/*****************************************************************************\
* (c) Copyright 2010-2019 CERN for the benefit of the LHCb Collaboration      *
*                                                                             *
* This software is distributed under the terms of the GNU General Public      *
* Licence version 3 (GPL Version 3), copied verbatim in the file "COPYING".   *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/
// Include files

// from Gaudi
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"

#include "Event/Track.h"
#include "Event/CaloCluster.h"
#include "Event/StateParameters.h"


// local
#include "TupleToolInfoAtCalo.h"

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include "LHCbMath/ValueWithError.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolInfoAtCalo
//
// 2018-11-07 : Jacco de Vries
//              j.devries@cern.ch
//
// Inspired by Phys/DaVinciTools BremAdder
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_COMPONENT( TupleToolInfoAtCalo )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolInfoAtCalo::TupleToolInfoAtCalo( const std::string& type, const std::string& name, const IInterface* parent )
    : TupleToolBase( type, name, parent ) {
  declareInterface<IParticleTupleTool>( this );
}

//=============================================================================
// Destructor
//=============================================================================
TupleToolInfoAtCalo::~TupleToolInfoAtCalo() {}

StatusCode TupleToolInfoAtCalo::initialize() {
  StatusCode sc = TupleToolBase::initialize();
  if ( sc.isFailure() ) return sc;
  m_calo = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal ) ;
  m_extrapolator = tool<ITrackExtrapolator>( m_extrapolatorType, this );

  return sc;
}

//=============================================================================
StatusCode TupleToolInfoAtCalo::fill( const LHCb::Particle*, const LHCb::Particle* P, const std::string& head,
                                     Tuples::Tuple& tuple ) {
  if ( msgLevel( MSG::VERBOSE ) ) verbose() << " filling particle " << endmsg;
  const std::string prefix = fullName( head );
  bool              test   = true;

  if ( !P ) return StatusCode::FAILURE;

  // first just return if the particle isn't supposed to have a track
  if ( !P->isBasicParticle() ) return StatusCode::SUCCESS;

  const LHCb::ProtoParticle* protop = P->proto();
  if ( !protop ) {
    if ( msgLevel( MSG::DEBUG ) ) debug() << "Protoparticle not found for this particle" << endmsg;
    return StatusCode::SUCCESS;
  }

  const LHCb::Track* track = protop->track();
  if ( !track ) {
    if ( msgLevel( MSG::DEBUG ) ) debug() << "Track not found for this particle" << endmsg;
    return StatusCode::SUCCESS;
  }
 
  // Get the position of the track at the Ecal
  const auto caloXYZ = TupleToolInfoAtCalo::positionAtEcal( track );
  test &= tuple -> column( prefix + "_ECAL_x", caloXYZ.X() );
  test &= tuple -> column( prefix + "_ECAL_y", caloXYZ.Y() );


  // Get position of Velo and TT extrapolated track at ECAL
  Gaudi::Plane3D plane = m_calo->plane( CaloPlane::ShowerMax );
  double         Hesse = plane.HesseDistance();
  double m_z0          = -Hesse / plane.Normal().Z();

  std::pair<Gaudi::XYZPoint, Gaudi::SymMatrix3x3> fromV = getPos( protop, LHCb::State::ClosestToBeam, m_z0 );
  std::pair<Gaudi::XYZPoint, Gaudi::SymMatrix3x3> fromT = getPos( protop, LHCb::State::AtTT, m_z0, (double)StateParameters::ZEndTT );
  if (!( fromV.first.z() == -1 * Gaudi::Units::km || fromT.first.z() == -1 * Gaudi::Units::km )) {
  
    Gaudi::XYZPoint     pfromVelo = fromV.first;
    Gaudi::XYZPoint     pfromTT   = fromT.first;
    Gaudi::SymMatrix3x3 cfromVelo = fromV.second;
    Gaudi::SymMatrix3x3 cfromTT   = fromT.second;

    test &= tuple -> column( prefix + "_ECAL_velotrack_x", pfromVelo.X() );
    test &= tuple -> column( prefix + "_ECAL_velotrack_y", pfromVelo.Y() );
    test &= tuple -> column( prefix + "_ECAL_velotrack_sprx", cfromVelo(0,0) );
    test &= tuple -> column( prefix + "_ECAL_velotrack_spry", cfromVelo(1,1) );
    test &= tuple -> column( prefix + "_ECAL_TTtrack_x",   pfromTT.X() );
    test &= tuple -> column( prefix + "_ECAL_TTtrack_y",   pfromTT.Y() );
    test &= tuple -> column( prefix + "_ECAL_TTtrack_sprx", cfromTT(0,0) );
    test &= tuple -> column( prefix + "_ECAL_TTtrack_spry", cfromTT(1,1) );

  }


  return StatusCode::SUCCESS;
}


Gaudi::XYZPoint TupleToolInfoAtCalo::positionAtEcal( const LHCb::Track* track ) {
  // Obtain position of track at ECAL. Inspired by CaloTools / Track2Calo

  Gaudi::XYZPoint caloXYZpoint;
  
  CaloPlane::Plane plane = CaloPlane::ShowerMax;
  ROOT::Math::Plane3D refPlane = m_calo->plane( plane );
  LHCb::State calostate( track->closestState( refPlane ) );
  LHCb::ParticleID pid = LHCb::ParticleID(11);
  double m_tolerance;
  
  StatusCode sc = m_extrapolator -> propagate( calostate, refPlane, m_tolerance, pid );
  if ( sc.isFailure() ) return caloXYZpoint;

  caloXYZpoint = Gaudi::XYZPoint( calostate.x(), calostate.y(), calostate.z() ); 

  return caloXYZpoint; 

}



const LHCb::State* TupleToolInfoAtCalo::usedState( const LHCb::Track* track ) const {
  if ( !track ) Exception( "NULL track" );
  const LHCb::State* uState = 0;
  if ( msgLevel( MSG::VERBOSE ) ) { verbose() << "ChargedParticleMakerBase::usedState :: Looking for State" << endmsg; }
  // default: closest to the beam:
  if ( !uState ) { uState = track->stateAt( LHCb::State::ClosestToBeam ); }
  // if not availabel: first measurementr
  if ( !uState ) { uState = track->stateAt( LHCb::State::FirstMeasurement ); }
  // backup
  if ( !uState ) {
    Warning( "No state closest to beam or at first measurement for track. Using first state instead", StatusCode{10},
             1 )
        .ignore();
    uState = &track->firstState();
  }
  if ( msgLevel( MSG::VERBOSE ) ) {
    verbose() << "Using '" << uState->location() << "' state at " << uState->position() << endmsg;
  }
  return uState;
}





const std::pair<Gaudi::XYZPoint, Gaudi::SymMatrix3x3> TupleToolInfoAtCalo::getPos( const LHCb::ProtoParticle*   proto,
                                                                         const LHCb::State::Location& lstate,
                                                                         double zcalo, double def ) const {
  // shamelessly stolen from https://gitlab.cern.ch/lhcb/Phys/blob/run2-patches/Phys/DaVinciNeutralTools/src/BremAdder.cpp 

  const LHCb::Track* tr = proto->track();
  LHCb::State        nstate;
  //-->ET get first available State instead of ClosestToBeam
  if ( lstate == LHCb::State::ClosestToBeam ) {
    nstate = *( usedState( tr ) );
  } else {
    const LHCb::State* state = tr->stateAt( lstate );
    if ( state == NULL && def > 0 ) {
      nstate        = LHCb::State();
      StatusCode sc = m_extrapolator->propagate( *tr, def, nstate, LHCb::ParticleID( 11 ) );
      if ( msgLevel( MSG::DEBUG ) ) debug() << "Extrapolating to: " << def << endmsg;
      nstate.setLocation( lstate );
      if ( sc.isFailure() ) {
        Warning( "Extrapolator failed" ).ignore();
        return std::make_pair( Gaudi::XYZPoint( 0, 0, -1 * Gaudi::Units::km ), Gaudi::SymMatrix3x3() );
      }
    } else if ( state == NULL ) {
      Warning( "State points to NULL" ).ignore();
      return std::make_pair( Gaudi::XYZPoint( 0, 0, -1 * Gaudi::Units::km ), Gaudi::SymMatrix3x3() );
    } else
      nstate = *state;
  }

  if ( msgLevel( MSG::DEBUG ) ) debug() << "State " << lstate << " " << nstate << endmsg;
  double              tx    = nstate.tx();
  double              ty    = nstate.ty();
  double              x     = nstate.x() + tx * ( zcalo - nstate.z() );
  double              y     = nstate.y() + ty * ( zcalo - nstate.z() );
  Gaudi::XYZPoint     point = Gaudi::XYZPoint( x, y, zcalo );
  Gaudi::SymMatrix3x3 cov = nstate.errPosition() + ( zcalo - nstate.z() ) * ( zcalo - nstate.z() ) * nstate.errSlopes();
  return std::make_pair( point, cov );
}





