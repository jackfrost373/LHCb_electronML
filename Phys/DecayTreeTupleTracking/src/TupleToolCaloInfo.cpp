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

#include "Event/CaloCluster.h"

// local
#include "TupleToolCaloInfo.h"

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include "LHCbMath/ValueWithError.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolCaloInfo
//
// 2018-11-07 : Jacco de Vries
//              j.devries@cern.ch
//
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_COMPONENT( TupleToolCaloInfo )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolCaloInfo::TupleToolCaloInfo( const std::string& type, const std::string& name, const IInterface* parent )
    : TupleToolBase( type, name, parent ) {
  declareInterface<IEventTupleTool>( this );
}

//=============================================================================
// Destructor
//=============================================================================
TupleToolCaloInfo::~TupleToolCaloInfo() {}

StatusCode TupleToolCaloInfo::initialize() {
  StatusCode sc = TupleToolBase::initialize();
  if ( sc.isFailure() ) return sc;
  m_calo = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal ) ;
  return sc;
}

//=============================================================================
StatusCode TupleToolCaloInfo::fill( Tuples::Tuple& tuple ) {
  if ( msgLevel( MSG::VERBOSE ) ) verbose() << " filling particle " << endmsg;
  bool              test   = true;

  // Get all Ecal clusters
  std::vector<double> cluster_x, cluster_y, cluster_e;
  const auto caloclusters = get<LHCb::CaloClusters>( LHCb::CaloClusterLocation::Ecal );
  for (auto it = caloclusters -> begin(); it != caloclusters -> end(); it++) {
    auto position = (*it) -> position();
    cluster_x.push_back( position.x() );
    cluster_y.push_back( position.y() );
    cluster_e.push_back( position.e() );
  }

  test &= tuple -> farray ( "ECAL_cluster_x_arr", cluster_x, "N_ECAL_clusters", 2000 );
  test &= tuple -> farray ( "ECAL_cluster_y_arr", cluster_y, "N_ECAL_clusters", 2000 );
  test &= tuple -> farray ( "ECAL_cluster_e_arr", cluster_e, "N_ECAL_clusters", 2000 );


  // Get all photon candidates
  //  inspired by https://gitlab.cern.ch/lhcb/Phys/blob/run2-patches/Phys/DaVinciNeutralTools/src/BremAdder.cpp
  std::vector<double> photon_x, photon_y, photon_sprx, photon_spry, photon_CL, photon_PT;
  const auto photons = get<LHCb::Particle::Range>( "/Event/Phys/StdVeryLooseAllPhotons/Particles" );
  for ( LHCb::Particle::Range::const_iterator ip = photons.begin(); photons.end() != ip; ++ip ) {
    const LHCb::Particle* ph = *ip;
    if ( ph->proto() == NULL ) continue;
    const SmartRefVector<LHCb::CaloHypo>& calovect = ph->proto()->calo();
    if ( calovect.empty() ) continue;
    SmartRefVector<LHCb::CaloHypo>::const_iterator icalo  = calovect.begin(); // take only first calohypo
    const LHCb::CaloHypo*                          caloph = ( *icalo );
    const LHCb::CaloHypo::Position*                gpos   = caloph->position();
    if ( gpos == NULL ) continue;

    const LHCb::CaloPosition::Spread& poscov = gpos->spread();

    photon_x.push_back(   gpos->x() );
    photon_y.push_back(   gpos->y() );
    photon_sprx.push_back( poscov( LHCb::CaloPosition::X, LHCb::CaloPosition::X ) );
    photon_spry.push_back( poscov( LHCb::CaloPosition::Y, LHCb::CaloPosition::Y ) );
    photon_CL.push_back(  ph -> confLevel() );  // BremAdder default cut is at 0.1
    photon_PT.push_back(  ph -> pt() );         // BremAdder default cut is at 75.

  }

  test &= tuple -> farray ( "ECAL_photon_x_arr",    photon_x,    "N_ECAL_photons", 2000 );
  test &= tuple -> farray ( "ECAL_photon_y_arr",    photon_y,    "N_ECAL_photons", 2000 );
  test &= tuple -> farray ( "ECAL_photon_sprx_arr", photon_sprx, "N_ECAL_photons", 2000 );
  test &= tuple -> farray ( "ECAL_photon_spry_arr", photon_spry, "N_ECAL_photons", 2000 );
  test &= tuple -> farray ( "ECAL_photon_CL_arr",   photon_CL,   "N_ECAL_photons", 2000 );
  test &= tuple -> farray ( "ECAL_photon_PT_arr",   photon_PT,   "N_ECAL_photons", 2000 );


  return StatusCode::SUCCESS;
}



