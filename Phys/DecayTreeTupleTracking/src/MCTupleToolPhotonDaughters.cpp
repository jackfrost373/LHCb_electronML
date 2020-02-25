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
    
  std::vector<LHCb::MCParticle*> photondaughters;

  if ( mcp ) {

    // Find all photon daughters of particle
    const auto mcparticles = get<LHCb::MCParticles>( LHCb::MCParticleLocation::Default );
    for(auto  mcparticle = mcparticles -> begin(); mcparticle != mcparticles -> end(); ++mcparticle) {

      if (!( (*mcparticle) -> particleID() == LHCb::ParticleID(22) )) continue; 
      //info() << " - is photon" << endmsg;
      
      auto mother = (*mcparticle) -> mother();
      if (!mother) continue;
      //info() << "  - is secondary" << endmsg;
      
      if ( (*mcparticle) -> originVertex() -> position().Z() > 9000 ) continue;
      // Because B-field is small, and past T3 we won't have any useful strack states anyway
      
      auto motherorivx = mother -> originVertex();
      auto mcporivx = mcp -> originVertex();

      if( motherorivx -> position().Z() == mcporivx -> position().Z() && 
          motherorivx -> position().X() == mcporivx -> position().X() &&
          motherorivx -> position().Y() == mcporivx -> position().Y() &&
          //mother -> p()                 == mcp -> p() && 
          //mother -> pseudoRapidity()    == mcp -> pseudoRapidity() && 
          mother -> particleID()        == mcp -> particleID()   ) { 

        //info() << "    - is daughter! " << (*mcparticle) -> particleID() << endmsg;
        photondaughters.push_back( *mcparticle );

      }
    } // end of loop over mcparticles 
  }

  //test &= tuple->column( prefix + "_MCphotondaughters_N", photondaughters.size() );
  test &= tuple->farray( prefix + "_MCphotondaughters_P"  , std::mem_fun(&LHCb::MCParticle::p),
                         prefix + "_MCphotondaughters_PT" , std::mem_fun(&LHCb::MCParticle::pt),
                         prefix + "_MCphotondaughters_ETA", std::mem_fun(&LHCb::MCParticle::pseudoRapidity),
                         photondaughters.begin(), photondaughters.end(), prefix + "_MCphotondaughters_N", 20 );

  std::vector<double> orivxX, orivxY, orivxZ;
  std::transform(photondaughters.begin(), photondaughters.end(), std::back_inserter(orivxX), 
      [](LHCb::MCParticle* daughter) { return daughter -> originVertex() -> position().X(); } );
  std::transform(photondaughters.begin(), photondaughters.end(), std::back_inserter(orivxY), 
      [](LHCb::MCParticle* daughter) { return daughter -> originVertex() -> position().Y(); } );
  std::transform(photondaughters.begin(), photondaughters.end(), std::back_inserter(orivxZ), 
      [](LHCb::MCParticle* daughter) { return daughter -> originVertex() -> position().Z(); } );
  test &= tuple->farray( prefix + "_MCphotondaughters_orivx_X", orivxX, prefix + "_MCphotondaughters_N", 20 );
  test &= tuple->farray( prefix + "_MCphotondaughters_orivx_Y", orivxY, prefix + "_MCphotondaughters_N", 20 );
  test &= tuple->farray( prefix + "_MCphotondaughters_orivx_Z", orivxZ, prefix + "_MCphotondaughters_N", 20 );

  std::vector<double> caloX, caloY;
  std::transform(photondaughters.begin(), photondaughters.end(), std::back_inserter(caloX), 
      [&](LHCb::MCParticle* daughter) { return MCTupleToolPhotonDaughters::positionAtEcal(daughter).X(); } );
  std::transform(photondaughters.begin(), photondaughters.end(), std::back_inserter(caloY), 
      [&](LHCb::MCParticle* daughter) { return MCTupleToolPhotonDaughters::positionAtEcal(daughter).Y(); } );
  test &= tuple->farray( prefix + "_MCphotondaughters_ECAL_X", caloX, prefix + "_MCphotondaughters_N", 20 );
  test &= tuple->farray( prefix + "_MCphotondaughters_ECAL_Y", caloY, prefix + "_MCphotondaughters_N", 20 );

  return StatusCode( test );
}


Gaudi::XYZPoint MCTupleToolPhotonDaughters::positionAtEcal( const LHCb::MCParticle* photon ) {
  // Obtain position of photon at ECAL

  Gaudi::XYZPoint caloXYZpoint;

  if (!( photon -> particleID() == LHCb::ParticleID(22) )) return caloXYZpoint;
  
  float zPos = 12500.;

  auto orivx = photon -> originVertex() -> position();
  auto lorentzvec = photon -> momentum();
  auto zdiff = zPos - orivx.Z();
  auto calox = orivx.X() + lorentzvec.Px() / lorentzvec.Pz() * zdiff;
  auto caloy = orivx.Y() + lorentzvec.Py() / lorentzvec.Pz() * zdiff;

  caloXYZpoint = Gaudi::XYZPoint( calox, caloy, zPos );

  return caloXYZpoint; 

}


