/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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

#include <ns3/simulator.h>

#include <ns3/log.h>

#include <ns3/buildings-helper.h>
#include <ns3/string.h>
#include <ns3/double.h>
#include <ns3/building.h>
#include <ns3/enum.h>
#include <ns3/constant-position-mobility-model.h>
#include <ns3/mobility-building-info.h>
#include "obstacles-pathloss-test.h"
#include <iomanip>
#include <ios>



NS_LOG_COMPONENT_DEFINE ("ObstaclesPathlossTest");

using namespace ns3;




/**
 * This TestSuite tests the ObstaclesPathlossModel by reproducing
 * several test positions of source and destination node 
 */


ObstaclesPathlossTestSuite::ObstaclesPathlossTestSuite ()
  : TestSuite ("obstacles-pathloss-test", SYSTEM)
{

  LogComponentEnable ("ObstaclesPathlossTest", LOG_LEVEL_ALL);

  double txpower = 46.00;  // in dbm
// Test #1    // In this configuration nodes will not be blocked as one node is above the top of obstacleand hence RxPower=TxPower

  AddTestCase (new ObstaclesPathlossTestCase (txpower,3, 6,46.00,"Nodes Not Blocked"), TestCase::QUICK);

// Test #2   // In this configuration nodes will be blocked as both the nodes are inside the obstacle and hence Txpower-3dB=43dbm

   AddTestCase (new ObstaclesPathlossTestCase (txpower,2, 1,43.00, "Nodes Blocked"), TestCase::QUICK);

  
// Test #3   // In this configuration nodes will be blocked as one node is still inside the obstacle and hence Txpower-3dB=43dbm
   AddTestCase (new ObstaclesPathlossTestCase (txpower,3,1,43.00, "Nodes Blocked"), TestCase::QUICK);

// Test #4   // In this configuration nodes will be blocked as obstacle in between the two nodes and hence Txpower-3dB=43dbm 
   AddTestCase (new ObstaclesPathlossTestCase (txpower, 4,5,43.00, "Nodes Blocked"), TestCase::QUICK);

// Test #5   // In this configuration nodes will be blocked as there are two obstacles between the two nodes and hence    (3*2=6dB)and Txpower-6dB=40dbm 
   AddTestCase (new ObstaclesPathlossTestCase (txpower, 4, 7,40.0, "Nodes Blocked by two Obstacles"), TestCase::QUICK);
  
}

static ObstaclesPathlossTestSuite obstaclesPathlossTestSuite;


/**
 * TestCase
 */

ObstaclesPathlossTestCase::ObstaclesPathlossTestCase (double txpower, uint16_t m1, uint16_t m2,double refValue, std::string status)
  : TestCase ("Receive Power calculation: " + status),
    m_txpower (txpower),
    m_mobilityModelIndex1 (m1),
    m_mobilityModelIndex2 (m2),
    m_RxPowerRef (refValue)
{
}

ObstaclesPathlossTestCase::~ObstaclesPathlossTestCase ()
{
}

void
ObstaclesPathlossTestCase::DoRun (void)
{
  NS_LOG_FUNCTION (this);

  // Two buildings in positive x-plane to demostrate the obstacle block 
  Ptr<Building> building1 = CreateObject<Building> ();
  building1->SetBoundaries (Box (890.0, 990.0,840.0,870.0, 0.0,6.0));
  Ptr<Building> building2 = CreateObject<Building> ();
  building2->SetBoundaries (Box (590.0, 690.0,840.0,870.0, 0.0,6.0));
  
  Ptr<MobilityModel> mma = CreateMobilityModel (m_mobilityModelIndex1);
  Ptr<MobilityModel> mmb = CreateMobilityModel (m_mobilityModelIndex2);

  Ptr<ObstaclesPropagationLossModel> propagationLossModel = CreateObject<ObstaclesPropagationLossModel> ();

   propagationLossModel->SetAttribute ("ObstacleLoss", DoubleValue (3));
  
  double RxPower = propagationLossModel->DoCalcRxPower (m_txpower,mma,mmb);
  

  NS_LOG_INFO ("Calculated Receive Power: " << RxPower);
  NS_LOG_INFO ("Theoretical Receive Power: " << m_RxPowerRef);
 
  NS_TEST_ASSERT_MSG_EQ (RxPower,m_RxPowerRef, "Wrong Receive Power !");
  Simulator::Destroy ();
}

Ptr<MobilityModel>
ObstaclesPathlossTestCase::CreateMobilityModel (uint16_t index)
{

  /*
   * The purpose of this method is to defer the creation of the
   * MobilityModel instances to when DoRun() is called. In a previous
   * version, MobilityModel instances where created directly in the
   * constructor of the test suite, which caused subtle bugs due to
   * "static initialization order fiasco". An example of such a subtle
   * bug is that logging via NS_LOG failed for some modules.
   * 
   */

  double hm = 1.5;
  //double hb = 30;
  double henbHeight = 1.5;

  Ptr<MobilityModel> mm;
  
  switch (index)
    {
    case 1:
      mm = CreateObject<ConstantPositionMobilityModel> ();
      mm->SetPosition (Vector (934.0, 852.0, henbHeight));
      break;

    case 2:
      mm = CreateObject<ConstantPositionMobilityModel> ();
      mm->SetPosition (Vector (931.0,861.0, hm));
      break;

    case 3:
      mm = CreateObject<ConstantPositionMobilityModel> ();
      mm->SetPosition (Vector (484.0, 298.0, hm));
      break;

    case 4:
      mm = CreateObject<ConstantPositionMobilityModel> ();
      mm->SetPosition (Vector (1000, 850, hm));
      break;

   
    case 5:
      mm = CreateObject<ConstantPositionMobilityModel> ();
      mm->SetPosition (Vector (850, 850, henbHeight));
      break;

   
    case 6:
      mm = CreateObject<ConstantPositionMobilityModel> ();
      mm->SetPosition (Vector (934.0, 852.0, (henbHeight+13.5)));
      break;

    case 7:
      mm = CreateObject<ConstantPositionMobilityModel> ();
      mm->SetPosition (Vector (484.0, 850.0,henbHeight));
      break;

      default:
      mm = 0;
      break;
    }
  Ptr<MobilityBuildingInfo> buildingInfo = CreateObject<MobilityBuildingInfo> ();
  mm->AggregateObject (buildingInfo); // operation usually done by BuildingsHelper::Install
  BuildingsHelper::MakeConsistent (mm); 
  return mm;
}
