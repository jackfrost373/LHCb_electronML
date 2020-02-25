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
#ifndef DecayTreeTupleTracking_TupleToolInfoAtCalo_H
#define DecayTreeTupleTracking_TupleToolInfoAtCalo_H 1

// Include files
// from Gaudi
#include "DecayTreeTupleBase/TupleToolBase.h"
#include "GaudiKernel/ToolHandle.h"
#include "Kernel/IParticleTupleTool.h" // Interface
#include "LHCbMath/ValueWithError.h"
#include "CaloDet/DeCalorimeter.h"
#include "Event/CaloCluster.h"

#include "TrackInterfaces/ITrackExtrapolator.h"

/** @class TupleToolInfoAtCalo TupleToolInfoAtCalo.h
 *  \brief TupleTool to extrapolate track position to ECAL
 *
 *  @author Jacco de Vries
 *  @date   2019-11-07
 */

// Forward declarations
class ITrackExtrapolator;

class TupleToolInfoAtCalo : public TupleToolBase, virtual public IParticleTupleTool {

public:
  /// Standard constructor
  TupleToolInfoAtCalo( const std::string& type, const std::string& name, const IInterface* parent );

  virtual ~TupleToolInfoAtCalo(); ///< Destructor

  StatusCode initialize() override;

public:
  StatusCode fill( const LHCb::Particle*, const LHCb::Particle*, const std::string&, Tuples::Tuple& ) override;

private:
  DeCalorimeter* m_calo;  
  ITrackExtrapolator*  m_extrapolator;
  std::string m_extrapolatorType = "TrackMasterExtrapolator";

  Gaudi::XYZPoint positionAtEcal( const LHCb::Track* ) ;

};

#endif // DecayTreeTupleTracking_TupleToolInfoAtCalo_H
