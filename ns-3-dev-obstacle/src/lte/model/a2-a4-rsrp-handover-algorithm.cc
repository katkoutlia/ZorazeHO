/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 * Copyright (c) 2013 Budiarto Herman
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
 * Author: Zoraze Ali(zoraze.ali@cttc.cat) 
 */ 

#include "a2-a4-rsrp-handover-algorithm.h"
#include <ns3/log.h>
#include <ns3/uinteger.h>
#include <ns3/double.h>
#include <ns3/lte-common.h>

NS_LOG_COMPONENT_DEFINE ("A2A4RsrpHandoverAlgorithm");


namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (A2A4RsrpHandoverAlgorithm);


///////////////////////////////////////////
// Handover Management SAP forwarder
///////////////////////////////////////////


A2A4RsrpHandoverAlgorithm::A2A4RsrpHandoverAlgorithm ()
  : m_a2MeasId (0),
    m_a4MeasId (0),
    m_servingCellThreshold (50),
    m_handoverManagementSapUser (0)
{
  NS_LOG_FUNCTION (this);
  m_handoverManagementSapProvider = new MemberLteHandoverManagementSapProvider<A2A4RsrpHandoverAlgorithm> (this);
}


A2A4RsrpHandoverAlgorithm::~A2A4RsrpHandoverAlgorithm ()
{
  NS_LOG_FUNCTION (this);
}


TypeId
A2A4RsrpHandoverAlgorithm::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::A2A4RsrpHandoverAlgorithm")
    .SetParent<LteHandoverAlgorithm> ()
    .AddConstructor<A2A4RsrpHandoverAlgorithm> ()
    .AddAttribute ("ServingCellThreshold",
                   "If the RSRP of the serving cell is worse than this threshold, "
                   "neighbour cells are consider for handover",
                   UintegerValue (50),
                   MakeUintegerAccessor (&A2A4RsrpHandoverAlgorithm::SetServingCellThreshold),
                   MakeUintegerChecker<uint8_t> (0, 97)) // RSRP range is [0..97] as per Section 9.1.7 of 3GPP TS 36.133
    .AddAttribute ("Hysteresis",
                   "Handover margin (hysteresis) in dB "
                   "(rounded to the nearest multiple of 0.5 dB)",
                   DoubleValue (0.0),
                   MakeDoubleAccessor (&A2A4RsrpHandoverAlgorithm::m_hysteresisDb),
                   MakeDoubleChecker<uint8_t> (0.0, 15.0)) // Hysteresis IE value range is [0..30] as per Section 6.3.5 of 3GPP TS 36.331
    
   .AddAttribute ("TargetCellid",
                   "User defined cell id",
                   DoubleValue (-1),
                   MakeDoubleAccessor (&A2A4RsrpHandoverAlgorithm::SetTargetCellid),
                   MakeDoubleChecker<double>()) // this attribute is added to do the handover to user defined cell id 




  ;
  return tid;
}


void
A2A4RsrpHandoverAlgorithm::SetLteHandoverManagementSapUser (LteHandoverManagementSapUser* s)
{
  NS_LOG_FUNCTION (this << s);
  m_handoverManagementSapUser = s;
}


LteHandoverManagementSapProvider*
A2A4RsrpHandoverAlgorithm::GetLteHandoverManagementSapProvider ()
{
  NS_LOG_FUNCTION (this);
  return m_handoverManagementSapProvider;
}


void
A2A4RsrpHandoverAlgorithm::DoInitialize ()
{
  NS_LOG_FUNCTION (this);

uint8_t hysteresisIeValue = EutranMeasurementMapping::ActualHysteresis2IeValue (m_hysteresisDb);

  NS_LOG_LOGIC (this << " requesting Event A2 measurements"
                     << " (threshold=" << (uint16_t) m_servingCellThreshold << ")"<< " (hysteresis=" << (uint16_t) hysteresisIeValue << ")");

  LteRrcSap::ReportConfigEutra reportConfigA2;
  reportConfigA2.eventId = LteRrcSap::ReportConfigEutra::EVENT_A2;
  reportConfigA2.threshold1.choice = LteRrcSap::ThresholdEutra::THRESHOLD_RSRP;
  reportConfigA2.threshold1.range = m_servingCellThreshold;
  reportConfigA2.hysteresis = hysteresisIeValue;
  reportConfigA2.triggerQuantity = LteRrcSap::ReportConfigEutra::RSRP;
  reportConfigA2.reportInterval = LteRrcSap::ReportConfigEutra::MS240;
  m_a2MeasId = m_handoverManagementSapUser->AddUeMeasReportConfigForHandover (reportConfigA2);

  NS_LOG_LOGIC (this << " requesting Event A4 measurements"
                     << " (threshold=0)");
  LteRrcSap::ReportConfigEutra reportConfigA4;
  reportConfigA4.eventId = LteRrcSap::ReportConfigEutra::EVENT_A4;
  reportConfigA4.threshold1.choice = LteRrcSap::ThresholdEutra::THRESHOLD_RSRP;
  reportConfigA4.threshold1.range = 0; // intentionally very low threshold
  reportConfigA4.triggerQuantity = LteRrcSap::ReportConfigEutra::RSRP;
  reportConfigA4.reportInterval = LteRrcSap::ReportConfigEutra::MS240; 
  m_a4MeasId = m_handoverManagementSapUser->AddUeMeasReportConfigForHandover (reportConfigA4);

  LteHandoverAlgorithm::DoInitialize ();
}


void
A2A4RsrpHandoverAlgorithm::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  delete m_handoverManagementSapProvider;
}


void
A2A4RsrpHandoverAlgorithm::DoReportUeMeas (uint16_t rnti,
                                           LteRrcSap::MeasResults measResults)
{
  NS_LOG_FUNCTION (this << rnti << (uint16_t) measResults.measId);

  if (measResults.measId == m_a2MeasId)
    {
      NS_ASSERT_MSG (measResults.rsrpResult <= m_servingCellThreshold,
                     "Invalid UE measurement report");
      EvaluateHandover (rnti, measResults.rsrpResult);
    }
  else if (measResults.measId == m_a4MeasId)
    {
      if (measResults.haveMeasResultNeighCells
          && !measResults.measResultListEutra.empty ())
        {
          for (std::list <LteRrcSap::MeasResultEutra>::iterator it = measResults.measResultListEutra.begin ();
               it != measResults.measResultListEutra.end ();
               ++it)
            {
              NS_ASSERT_MSG (it->haveRsrpResult == true,
                             "RSRP measurement is missing from cellId " << it->physCellId);
              UpdateNeighbourMeasurements (rnti, it->physCellId, it->rsrpResult);
            }
        }
      else
        {
          NS_LOG_WARN (this << " Event A4 received without measurement results from neighbouring cells");
        }
    }
  else
    {
      NS_LOG_WARN ("Ignoring measId " << (uint16_t) measResults.measId);
    }

} // end of DoReportUeMeas


void
A2A4RsrpHandoverAlgorithm::EvaluateHandover (uint16_t rnti,
                                             uint8_t servingCellRsrp)
{
  NS_LOG_FUNCTION (this << rnti << (uint16_t) servingCellRsrp);

  MeasurementTable_t::iterator it1;
  it1 = m_neighbourCellMeasures.find (rnti);

  if (it1 == m_neighbourCellMeasures.end ())
    {
      NS_LOG_WARN ("Skipping handover evaluation for RNTI " << rnti << " because neighbour cells information is not found");
    }
  else
    {
      // Find the best neighbour cell (eNB)
      NS_LOG_LOGIC ("Number of neighbour cells = " << it1->second.size ());
      uint16_t bestNeighbourCellId = 0;
      uint8_t bestNeighbourRsrp = 0;
      MeasurementRow_t::iterator it2;
      MeasurementRow_t::iterator it3;

     if (m_TargetCellid!= -1)
         {
           it3 = it1->second.find (m_TargetCellid);
           if (it3 != it1->second.end ())
              {
               NS_LOG_LOGIC ("User triggered Handover to cellId " << it3->first);
               NS_LOG_LOGIC ("User selected target cell RSRP " << (uint16_t) it3->second->m_rsrp);
                 bestNeighbourCellId = it3->first;
                 bestNeighbourRsrp = it3->second->m_rsrp;

                }
            }
    
    else{
      for (it2 = it1->second.begin (); it2 != it1->second.end (); ++it2)
        {
          if ((it2->second->m_rsrp > bestNeighbourRsrp)
              && IsValidNeighbour (it2->first))
            {
              bestNeighbourCellId = it2->first;
              bestNeighbourRsrp = it2->second->m_rsrp;
            }
        }
     }

      // Trigger Handover, if needed
      if (bestNeighbourCellId > 0)
        {
          NS_LOG_LOGIC ("Best neighbour cellId " << bestNeighbourCellId);

        
              NS_LOG_LOGIC ("Trigger Handover to cellId " << bestNeighbourCellId);
              NS_LOG_LOGIC ("target cell RSRP " << (uint16_t) bestNeighbourRsrp);
              NS_LOG_LOGIC ("serving cell RSRP " << (uint16_t) servingCellRsrp);

            
              m_handoverManagementSapUser->TriggerHandover (rnti,
                                                            bestNeighbourCellId);
                 
           
        }

    } // end of else of if (it1 == m_neighbourCellMeasures.end ())

} // end of EvaluateMeasurementReport


bool
A2A4RsrpHandoverAlgorithm::IsValidNeighbour (uint16_t cellId)
{
  NS_LOG_FUNCTION (this << cellId);

  /**
   * \todo In the future, this function can be expanded to validate whether the
   *       neighbour cell is a valid target cell, e.g., taking into account the
   *       NRT in ANR and whether it is a CSG cell with closed access.
   */

  return true;
}


void
A2A4RsrpHandoverAlgorithm::UpdateNeighbourMeasurements (uint16_t rnti,
                                                        uint16_t cellId,
                                                        uint8_t rsrp)
{
  NS_LOG_FUNCTION (this << rnti << cellId << (uint16_t) rsrp);
  MeasurementTable_t::iterator it1;
  it1 = m_neighbourCellMeasures.find (rnti);

  if (it1 == m_neighbourCellMeasures.end ())
    {
      // insert a new UE entry
      MeasurementRow_t row;
      std::pair<MeasurementTable_t::iterator, bool> ret;
      ret = m_neighbourCellMeasures.insert (std::pair<uint16_t, MeasurementRow_t> (rnti, row));
      NS_ASSERT (ret.second);
      it1 = ret.first;
    }

  NS_ASSERT (it1 != m_neighbourCellMeasures.end ());
  Ptr<UeMeasure> neighbourCellMeasures;
  std::map<uint16_t, Ptr<UeMeasure> >::iterator it2;
  it2 = it1->second.find (cellId);

  if (it2 != it1->second.end ())
    {
      neighbourCellMeasures = it2->second;
      neighbourCellMeasures->m_cellId = cellId;
      neighbourCellMeasures->m_rsrp = rsrp;
      neighbourCellMeasures->m_rsrq = 0;
    }
  else
    {
      // insert a new cell entry
      neighbourCellMeasures = Create<UeMeasure> ();
      neighbourCellMeasures->m_cellId = cellId;
      neighbourCellMeasures->m_rsrp = rsrp;
      neighbourCellMeasures->m_rsrq = 0;
      it1->second[cellId] = neighbourCellMeasures;
    }

} // end of UpdateNeighbourMeasurements


void 
A2A4RsrpHandoverAlgorithm::SetTargetCellid(double cellid)
 {
   NS_LOG_FUNCTION (this << double (cellid));
   m_TargetCellid=cellid;
 }


void 
A2A4RsrpHandoverAlgorithm::SetServingCellThreshold (uint8_t th)
 {
    NS_LOG_FUNCTION (this << uint16_t (th));
    m_servingCellThreshold=th;
 }




} // end of namespace ns3
