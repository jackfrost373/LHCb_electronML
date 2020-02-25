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
// $Id: MCTupleToolPhotonDaughters.h, jadevrie $

#ifndef MCTUPLETOOLPHOTONDAUGHTERS_H
#define MCTUPLETOOLPHOTONDAUGHTERS_H 1

#include "DecayTreeTupleBase/TupleToolBase.h"
#include "Event/MCParticle.h"            
#include "Kernel/IMCParticleTupleTool.h" 
#include "GaudiKernel/ToolHandle.h"
#include "CaloDet/DeCalorimeter.h"
#include "Event/CaloCluster.h"

#include "TrackInterfaces/ITrackExtrapolator.h"

class ITrackExtrapolator;

class MCTupleToolPhotonDaughters : public TupleToolBase, virtual public IMCParticleTupleTool {

public:
  /// Standard constructor
  MCTupleToolPhotonDaughters( const std::string& type, const std::string& name, const IInterface* parent );

  virtual ~MCTupleToolPhotonDaughters(); ///< Destructor
  StatusCode initialize() override;
  StatusCode fill( const LHCb::MCParticle*, const LHCb::MCParticle*, const std::string&, Tuples::Tuple& ) override;

private:
  DeCalorimeter* m_calo;  
  ITrackExtrapolator*  m_extrapolator;
  std::string m_extrapolatorType = "TrackMasterExtrapolator";

  Gaudi::XYZPoint positionAtEcal( const LHCb::MCParticle* photon ) ;

};
#endif // MCTUPLETOOLPHOTONDAUGHTERS_H

