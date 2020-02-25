/*****************************************************************************\
* (c) Copyright 2000-2019 CERN for the benefit of the LHCb Collaboration      *
*                                                                             *
* This software is distributed under the terms of the GNU General Public      *
* Licence version 3 (GPL Version 3), copied verbatim in the file "COPYING".   *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/
// Include files
#include "gsl/gsl_sys.h"

// from Gaudi
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/Vector3DTypes.h"

#include "Event/Track.h"
#include "Event/CaloCluster.h"
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"

// local
#include "MCTupleToolPhotonDaughters.h"

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"


using namespace LHCb;
//-----------------------------------------------------------------------------
// Implementation file for class : MCTupleToolPhotonDaughters
//
// 2009-01-19 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_COMPONENT( MCTupleToolPhotonDaughters )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCTupleToolPhotonDaughters::MCTupleToolPhotonDaughters( const std::string& type, const std::string& name, const IInterface* parent )
    : TupleToolBase( type, name, parent ) {
  declareInterface<IMCParticleTupleTool>( this );

  //declareProperty( "StoreKineticInfo", m_storeKinetic = true );

}
//=============================================================================
// Destructor
//=============================================================================
MCTupleToolPhotonDaughters::~MCTupleToolPhotonDaughters() {}

//=============================================================================
// initialize
//=============================================================================

StatusCode MCTupleToolPhotonDaughters::initialize() {
  const StatusCode sc = TupleToolBase::initialize();
  if ( sc.isFailure() ) return StatusCode::FAILURE;

  m_calo = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal ) ;
  m_extrapolator = tool<ITrackExtrapolator>( m_extrapolatorType, this );

  return sc;
}

//=============================================================================
// Fill
//=============================================================================
StatusCode MCTupleToolPhotonDaughters::fill( const LHCb::MCParticle*, const LHCb::MCParticle* mcp, const std::string& head,
                                       Tuples::Tuple& tuple ) {
  const std::string prefix = fullName( head );
  bool              test   = true;

  if ( mcp ) {

    // Find all photon daughters of particle
    std::vector<LHCb::MCParticle*> photondaughters;
    const auto mcparticles = get<LHCb::MCParticles>( LHCb::MCParticleLocation::Default );
    for(auto  mcparticle = mcparticles -> begin(); mcparticle != mcparticles -> end(); ++mcparticle) {

      if (not (*mcparticle) -> particleID() == LHCb::ParticleID(22) ) continue; 
      //info() << " - is photon" << endmsg;
      
      mother = (*mcparticle) -> mother();
      if (not mother) continue;
      //info() << "  - is secondary" << endmsg;
      
      if ( (*mcparticle) -> originVertex() -> position().Z() > 10000 ) continue;
      //info() << "   - created before calo" << endmsg;
      
      motherorivx = mother -> originVertex();
      mcporivx = mcp -> originVertex();

      //if (!( mother -> particleID() == LHCb::ParticleID(11) )) continue;
      //info() << "    - created by electron" << endmsg;
      //info() << "     mcp    orivx: " << mcporivx -> position()   << endmsg;
      //info() << "     mother orivx: " << motherorivx ->position() << endmsg;

      if( motherorivx -> position().Z() == mcporivx -> position().Z() && 
          motherorivx -> position().X() == mcporivx -> position().X() &&
          motherorivx -> position().Y() == mcporivx -> position().Y() &&
          mother -> p()                 == mcp -> p() && 
          mother -> pseudoRapidity()    == mcp -> pseudoRapidity() && 
          mother -> particleID()        == mcp -> particleID()   ) { 

        info() << "    - is daughter! " << (*mcparticle) -> particleID() << endmsg;
        photondaughters.push_back( *mcparticle );
        info() << "P is " << (*mcparticle) -> p() << endmsg;
        info() << "PT is " << (*mcparticle) -> pt() << endmsg;
        info() << "eta is " << (*mcparticle) -> pseudoRapidity() << endmsg;
        info() << "orivx_z is " << (*mcparticle) -> originVertex() -> position().Z() << endmsg;
      }
    } // end of loop over mcparticles 

  }

  //test &= tuple->column( prefix + "_TRUEP_", 1 );
  //test &= tuple->column( prefix + "_TRUEPT", 1 );

  return StatusCode( test );
}


Gaudi::XYZPoint MCTupleToolPhotonDaughters::positionAtEcal( const LHCb::Track* track ) {
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


