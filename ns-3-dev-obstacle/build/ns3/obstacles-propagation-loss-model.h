
/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011, 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author:
 *
 */

#ifndef OBSTACLES_PROPAGATION_LOSS_MODEL_H_
#define OBSTACLES_PROPAGATION_LOSS_MODEL_H_

#include "ns3/nstime.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/random-variable-stream.h"
#include <ns3/building.h>
#include <ns3/mobility-building-info.h>
#include <ns3/building-list.h>
#include <ns3/vector.h>



namespace ns3 {

 class ObstaclesPropagationLossModel : public PropagationLossModel

 {
  public :
     static TypeId GetTypeId (void);
     ObstaclesPropagationLossModel ();
    ~ObstaclesPropagationLossModel ();
  /**
   * \param a the mobility model of the source
   * \param b the mobility model of the destination
   * \returns the propagation loss (in dBm)
   */
    double GetLoss (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const;

   // inherited from PropagationLossModel
   virtual double DoCalcRxPower (double txpowerDbm, Ptr<MobilityModel> a, Ptr<MobilityModel> b) const;
   virtual int64_t DoAssignStreams (int64_t stream);
   
   void SetObsatcleLoss (double ObsLoss);

   private :  
      
       double m_ObsLoss;  
  
 };

}

#endif /* OBSTACLES_PROPAGATION_LOSS_MODEL_H_ */

