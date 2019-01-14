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

#include "handover-stats-calculator.h"
#include "ns3/string.h"
#include <vector>
#include <ns3/simulator.h>
#include <ns3/log.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("HandoverStatsCalculator");

NS_OBJECT_ENSURE_REGISTERED (HandoverStatsCalculator);


std::vector<uint16_t>HandoverStatsCalculator::m_measResultVec;
HandoverStatsCalculator::HandoverStatsCalculator  ()
  :  m_RecvMeasFirstWrite (true),m_prevlogtime(0)
    
{
  NS_LOG_FUNCTION (this);

}

HandoverStatsCalculator::~HandoverStatsCalculator ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
HandoverStatsCalculator::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::HandoverStatsCalculator")
    .SetParent<LteStatsCalculator> ()
    .AddConstructor<HandoverStatsCalculator> ()
    .AddAttribute ("RecvMeasReport",
                   "Name of the file where the RecvMeasReport statistics will be saved.",
                   StringValue ("RecvMeasReport.txt"),
                   MakeStringAccessor (&HandoverStatsCalculator::SetRecvMeasurementReportTraceFilename),MakeStringChecker ())

    .AddAttribute ("DataBaseFormat",
                   "Format of the file where the RecvMeasReport statistics will be saved.If true file will be written in database format",
                   BooleanValue (false),
                   MakeBooleanAccessor (&HandoverStatsCalculator::m_dataBase_format),MakeBooleanChecker ());

;

  return tid;
}

void
HandoverStatsCalculator::SetRecvMeasurementReportTraceFilename(std::string filename)
{
  m_RecvMeasFilename = filename;
}

std::string
HandoverStatsCalculator::GetRecvMeasurementReportTraceFilename(void)
{
  return m_RecvMeasFilename;
}

std::vector<uint16_t>
HandoverStatsCalculator::GetRecvMeasurementVector(void)
{
  return m_measResultVec;
}


void
HandoverStatsCalculator::ReportRecvMeasurementReport (uint64_t imsi,uint16_t cellId,uint16_t rnti,LteRrcSap::MeasurementReport msg)

{

  NS_LOG_INFO ("Write RecvMeasurementReport trace in " << GetRecvMeasurementReportTraceFilename().c_str ());

if(!m_dataBase_format)
{

if ((msg.measResults.measId==1||msg.measResults.measId==2)&&(Simulator::Now ().GetNanoSeconds () / (double) 1e9>m_prevlogtime ))

//if ((Simulator::Now ().GetNanoSeconds () / (double) 1e9>m_prevlogtime ))
{

NS_LOG_INFO ("measId" <<(uint16_t)msg.measResults.measId);
  std::ofstream outFile;
//std::cout<<msg.measResults.measResultListEutra.size ()<<std::endl;
  if ( m_RecvMeasFirstWrite == true )
    {
      outFile.open (GetRecvMeasurementReportTraceFilename().c_str ());
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetRecvMeasurementReportTraceFilename().c_str ());
          return;
        }
      m_RecvMeasFirstWrite = false;
      outFile << "% time\tScellid\tIMSI\tRNTI\tSrsrp\tSrsrq\tNiebCellids\tNiebCellsRSRP\tNiebCellsRSRQ\tMeasId";
      outFile << std::endl;

    }
  else
    {
      outFile.open (GetRecvMeasurementReportTraceFilename ().c_str (),  std::ios_base::app);
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetRecvMeasurementReportTraceFilename().c_str ());
          return;
        }
    }

  outFile << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t";
  m_prevlogtime=Simulator::Now ().GetNanoSeconds () / (double) 1e9;
  outFile << cellId << "\t";
  outFile << imsi << "\t";
  outFile << rnti << "\t";
  outFile<<(uint16_t)msg.measResults.rsrpResult<< "\t";
  outFile<< (uint16_t)msg.measResults.rsrqResult<< "\t";
 // outFile<< "S-RSRP " << (uint16_t)msg.measResults.rsrpResult<< "\t";
  //outFile<< "S-RSRQ " << (uint16_t)msg.measResults.rsrqResult<< "\t";

for (std::list <LteRrcSap::MeasResultEutra>::iterator it = msg.measResults.measResultListEutra.begin ();
  it != msg.measResults.measResultListEutra.end ();
  ++it)
  {
if(it != msg.measResults.measResultListEutra.begin ())
{
   outFile<<",";
   outFile<< it->physCellId;

}
else{
outFile<< it->physCellId;
}

}

outFile << "\t";

for (std::list <LteRrcSap::MeasResultEutra>::iterator it1 = msg.measResults.measResultListEutra.begin ();
  it1 != msg.measResults.measResultListEutra.end ();
  ++it1)
  {

if(it1 != msg.measResults.measResultListEutra.begin ())
{
   outFile<<",";
  outFile<< (it1->haveRsrpResult ? (uint16_t) it1->rsrpResult : 255);

}
else{
outFile<< (it1->haveRsrpResult ? (uint16_t) it1->rsrpResult : 255);
}

}

outFile << "\t";

for (std::list <LteRrcSap::MeasResultEutra>::iterator it2 = msg.measResults.measResultListEutra.begin ();
  it2 != msg.measResults.measResultListEutra.end ();
  ++it2)
  {
if(it2 != msg.measResults.measResultListEutra.begin ())
{
   outFile<<",";
  outFile<< (it2->haveRsrqResult ? (uint16_t) it2->rsrqResult : 255);

}
else{
outFile<< (it2->haveRsrqResult ? (uint16_t) it2->rsrqResult : 255);
}

}
outFile << "\t";

outFile<<(uint16_t)msg.measResults.measId<< "\t";

  outFile << std::endl;

  outFile.close ();

   }
 }//if(!dataBase_format) ends here

 else
    {
        if ((msg.measResults.measId==1||msg.measResults.measId==2)&&(Simulator::Now ().GetNanoSeconds () / (double) 1e9>m_prevlogtime )&&cellId==1)

        {

NS_LOG_INFO ("measId" <<(uint16_t)msg.measResults.measId);
  std::ofstream outFile;
  
  if ( m_RecvMeasFirstWrite == true )
    {
      outFile.open (GetRecvMeasurementReportTraceFilename().c_str ());
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetRecvMeasurementReportTraceFilename().c_str ());
          return;
        }
      m_RecvMeasFirstWrite = false;
      
    }
  else
    {
      outFile.open (GetRecvMeasurementReportTraceFilename ().c_str (),std::ios_base::app);
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetRecvMeasurementReportTraceFilename().c_str ());
          return;
        }
    }

 
for (std::list <LteRrcSap::MeasResultEutra>::reverse_iterator it1 = msg.measResults.measResultListEutra.rbegin ();
  it1 != msg.measResults.measResultListEutra.rend ();
  ++it1)
  {

  outFile<< (it1->haveRsrqResult ? (uint16_t) it1->rsrqResult : 255)<<" ";
   m_measResultVec.push_back((it1->haveRsrqResult ? (uint16_t) it1->rsrqResult : 255));
  outFile<< (it1->haveRsrpResult ? (uint16_t) it1->rsrpResult : 255)<<" ";
  m_measResultVec.push_back((it1->haveRsrpResult ? (uint16_t) it1->rsrpResult : 255));
  

}

  outFile<< (uint16_t)msg.measResults.rsrqResult<< " ";
  m_measResultVec.push_back(msg.measResults.rsrqResult);
  outFile<<(uint16_t)msg.measResults.rsrpResult<< " ";
  m_measResultVec.push_back(msg.measResults.rsrpResult);

 outFile << std::endl;
 outFile.close ();
  }

   } //else of if(!dataBase_format) ends here

 }


void
HandoverStatsCalculator::RecvMeasurementReportCallback (Ptr<HandoverStatsCalculator> hndStats,std::string path,
uint64_t imsi,uint16_t cellId,uint16_t rnti,LteRrcSap::MeasurementReport msg)
{
  NS_LOG_FUNCTION (hndStats << path);
 
  hndStats->ReportRecvMeasurementReport (imsi,cellId,rnti,msg);
}


} // namespace ns3
