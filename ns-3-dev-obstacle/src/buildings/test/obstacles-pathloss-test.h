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
 */

#ifndef OBSTACLES_PATHLOSS_TEST_H
#define OBSTACLES_PATHLOSS_TEST_H

#include <ns3/test.h>
#include <ns3/obstacles-propagation-loss-model.h>


using namespace ns3;

class ObstaclesPathlossTestSuite : public TestSuite
{
public:
  ObstaclesPathlossTestSuite ();
};


class ObstaclesPathlossTestCase : public TestCase
{
public:
  ObstaclesPathlossTestCase (double txpower, uint16_t m1, uint16_t m2,double refValue, std::string status);
  virtual ~ObstaclesPathlossTestCase ();

private:
  virtual void DoRun (void);
  Ptr<MobilityModel> CreateMobilityModel (uint16_t index);

  double m_txpower;
  uint16_t m_mobilityModelIndex1;
  uint16_t m_mobilityModelIndex2;
  double m_RxPowerRef;

};


#endif /* Obstacles_PATHLOSS_TEST_H */
