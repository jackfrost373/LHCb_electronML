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


  return StatusCode::SUCCESS;
}



