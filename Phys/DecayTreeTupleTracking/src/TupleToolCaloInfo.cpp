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
// Inspired by Phys/DaVinciTools BremAdder
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_COMPONENT( TupleToolCaloInfo )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolCaloInfo::TupleToolCaloInfo( const std::string& type, const std::string& name, const IInterface* parent )
    : TupleToolBase( type, name, parent ) {
  declareInterface<IParticleTupleTool>( this );
}

//=============================================================================
// Destructor
//=============================================================================
TupleToolCaloInfo::~TupleToolCaloInfo() {}

StatusCode TupleToolCaloInfo::initialize() {
  StatusCode sc = TupleToolBase::initialize();
  if ( sc.isFailure() ) return sc;
  m_calo = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal ) ;
  m_extrapolator = tool<ITrackExtrapolator>( m_extrapolatorType, this );

  return sc;
}

//=============================================================================
StatusCode TupleToolCaloInfo::fill( const LHCb::Particle*, const LHCb::Particle* P, const std::string& head,
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
  const auto caloXYZ = TupleToolCaloInfo::positionAtEcal( track );
  const auto x_at_calo = caloXYZ.X();
  const auto y_at_calo = caloXYZ.Y();

  test &= tuple -> column( prefix + "_ECAL_x", x_at_calo );
  test &= tuple -> column( prefix + "_ECAL_y", y_at_calo );


  // Get all Ecal clusters
  std::vector<double> cluster_x, cluster_y, cluster_e;
  const auto caloclusters = get<LHCb::CaloClusters>( LHCb::CaloClusterLocation::Ecal );
  for (auto it = caloclusters -> begin(); it != caloclusters -> end(); it++) {
    auto position = (*it) -> position();
    cluster_x.push_back( position.x() );
    cluster_y.push_back( position.y() );
    cluster_e.push_back( position.e() );
  }

  test &= tuple -> farray ( "ECAL_cluster_x_arr", cluster_x, "N_ECAL_clusters", 500 );
  test &= tuple -> farray ( "ECAL_cluster_y_arr", cluster_y, "N_ECAL_clusters", 500 );
  test &= tuple -> farray ( "ECAL_cluster_e_arr", cluster_e, "N_ECAL_clusters", 500 );


  return StatusCode::SUCCESS;
}


Gaudi::XYZPoint TupleToolCaloInfo::positionAtEcal( const LHCb::Track* track ) {
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


