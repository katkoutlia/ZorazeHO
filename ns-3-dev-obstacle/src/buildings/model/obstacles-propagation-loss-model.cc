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


#include "ns3/propagation-loss-model.h"
#include "ns3/log.h"
#include "ns3/mobility-model.h"
#include "ns3/double.h"
#include "ns3/pointer.h"
#include <cmath>
#include "obstacles-propagation-loss-model.h"
#include <ns3/building.h>
#include <ns3/mobility-building-info.h>
#include <ns3/building-list.h>
#include <ns3/vector.h>
#include "ns3/enum.h"
#include <ns3/simulator.h>


NS_LOG_COMPONENT_DEFINE ("ObstaclesPropagationLossModel");

namespace ns3{

NS_OBJECT_ENSURE_REGISTERED (ObstaclesPropagationLossModel);

ObstaclesPropagationLossModel::ObstaclesPropagationLossModel()
{
}

ObstaclesPropagationLossModel::~ObstaclesPropagationLossModel()
{
}

TypeId
ObstaclesPropagationLossModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ObstaclesPropagationLossModel")
    .SetParent<PropagationLossModel> ()
    .AddConstructor<ObstaclesPropagationLossModel> ()

    .AddAttribute ("ObstacleLoss", "Loss value for Obstacle in [dB].",
                   DoubleValue(3.0),
                   MakeDoubleAccessor(&ObstaclesPropagationLossModel::SetObsatcleLoss),
                   MakeDoubleChecker<double>())

    ;
  return tid;
}

double
ObstaclesPropagationLossModel::GetLoss(Ptr<MobilityModel> a, Ptr<MobilityModel> b) const
{
    Vector Pos_a= a->GetPosition();
    Vector Pos_b= b->GetPosition();
    bool intersect;
    double loss=0.0;
    for (BuildingList::Iterator it = BuildingList::Begin (); it != BuildingList::End (); ++it)
    {

      Ptr<Building> building= (*it);
      intersect=building->IsIntersect(Pos_a,Pos_b);
      

      if (intersect)
      {
          loss= loss+m_ObsLoss;
        NS_LOG_INFO ("At "<<Simulator::Now ().GetSeconds ()<< "Sec Obstacle between " <<"("<<Pos_a.x<<","<<Pos_a.y<<","<<Pos_a.z<<")"<<" and "<<"("<<Pos_b.x<<","<<Pos_b.y<<","<<Pos_b.z<<")"<<" Loss "<< loss);
      }

    }
 loss = std::max (loss, 0.0);
    return loss;
}

double
ObstaclesPropagationLossModel::DoCalcRxPower (double txPowerDbm, Ptr<MobilityModel> a, Ptr<MobilityModel> b) const
{

return (txPowerDbm - GetLoss (a,b));
}

void
ObstaclesPropagationLossModel::SetObsatcleLoss( double ObsLoss)
{
  m_ObsLoss=ObsLoss;

}

int64_t
ObstaclesPropagationLossModel::DoAssignStreams(int64_t stream)
{
    return 0;
}

}




