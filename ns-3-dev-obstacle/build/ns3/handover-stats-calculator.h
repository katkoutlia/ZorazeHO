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
 * Author: Zoraze Ali <zali@cttc.es>
 */

#ifndef HANDOVER_STATS_CALCULATOR_H_
#define HANDOVER_STATS_CALCULATOR_H_

#include <ns3/object.h>
#include "ns3/traced-callback.h"
#include "ns3/lte-stats-calculator.h"
#include "ns3/nstime.h"
#include "ns3/uinteger.h"
#include "ns3/spectrum-value.h"
#include <string>
#include <vector>
#include <fstream>
#include <ns3/lte-rrc-sap.h>
#include <ns3/core-module.h>

#include <map>
#include <set>

namespace ns3 {

/**
 * Takes care of measurement reports fired from EnbRrc. Metrics saved are:
 * - Source Cell Id 
 * - IMSI
 * - Source Cell RSRP
 * - Source Cell RSRQ
 * - Neighbour Cell Id
 * - Neighbour Cell RSRP
 * - Neighbour Cell RSRQ
 
 */
class HandoverStatsCalculator : public LteStatsCalculator
{
public:
  /**
   * Constructor
   */
  HandoverStatsCalculator();

  /**
   * Destructor
   */
  virtual ~HandoverStatsCalculator ();

  // Inherited from ns3::Object
  static TypeId GetTypeId (void);

  /**
   * Set the name of the file where the RecvMeasurementReport statistics will be stored.
   *
   * @param filename string with the name of the file
   */
  void SetRecvMeasurementReportTraceFilename (std::string filename);

  /**
   * Get the name of the file where the RecvMeasurementReport statistics will be stored.
   * @return the name of the file where the RecvMeasurementReport statistics will be stored
   */
  std::string GetRecvMeasurementReportTraceFilename(void);

 
  /**
   * Notifies the stats calculator that a Measurement report has occurred.
   * @param imsi IMSI of the UE
   * @param cellid cellId of the enb
   * @param rnti C-RNTI scheduled
   * @param message msg
   
   */
  void ReportRecvMeasurementReport (uint64_t imsi,uint16_t cellId,uint16_t rnti,LteRrcSap::MeasurementReport msg);

  
  /** 
   * trace sink
   * 
   * \param imsi 
   * \param cellid 
   * \param rnti 
   * \param rnti 
   * \param msg 
      */

static void RecvMeasurementReportCallback (Ptr<HandoverStatsCalculator> hndStats,std::string path,uint64_t imsi,uint16_t cellId,uint16_t rnti,LteRrcSap::MeasurementReport msg);

/** 
   * Function returning the Static vector "m_measResultVec" in which measurment are pushed in database format 
   * 
   * 
      */

static std::vector<uint16_t>GetRecvMeasurementVector(void);

bool m_RecvMeasFirstWrite;
bool m_dataBase_format;
double m_prevlogtime;
static std::vector<uint16_t> m_measResultVec;

std::string m_RecvMeasFilename;
  

};

} // namespace ns3

#endif /* HANDOVER_STATS_CALCULATOR_H_ */



