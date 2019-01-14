/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011, 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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
 * 
 */

#ifndef A2_A4_RSRP_HANDOVER_ALGORITHM_H
#define A2_A4_RSRP_HANDOVER_ALGORITHM_H

#include <ns3/lte-handover-algorithm.h>
#include <ns3/lte-handover-management-sap.h>
#include <ns3/lte-rrc-sap.h>
#include <ns3/simple-ref-count.h>
#include <ns3/ptr.h>
#include <map>

namespace ns3 {


/**
 * \brief Handover algorithm implementation based on RSRP measurements, Event
 *        A2 and Event A4.
 *
 * Handover decision made by this algorithm is based on Event A2
 * measurements (serving cell's RSRP becomes worse than threshold). When the
 * event is triggered, the first condition of handover is fulfilled.
 *
 * Event A4 measurements (neighbour cell's RSRP becomes better than threshold)
 * are used to detect neighbouring cells and their respective RSRP. 
 *
 * 
 *
 * The threshold for Event A2 can be configured in the `ServingCellThreshold`
 * attribute. 
 *
 * The following code snippet is an example of using and configuring the
 * handover algorithm in a simulation program:
 *
 *     Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
 *
 *     NodeContainer enbNodes;
 *     // configure the nodes here...
 *
 *     lteHelper->SetHandoverAlgorithmType ("ns3::A2A4RsrpHandoverAlgorithm");
 *     lteHelper->SetHandoverAlgorithmAttribute ("ServingCellThreshold",
 *                                               UintegerValue (30));
 *     lteHelper->SetHandoverAlgorithmAttribute ("TargetCellid",DoubleValue (targetCellId));
 *    
 *     NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice (enbNodes);
 *
 * \note Setting the handover algorithm type and attributes after the call to
 *       LteHelper::InstallEnbDevice does not have any effect to the devices
 *       that have already been installed.
 */
class A2A4RsrpHandoverAlgorithm : public LteHandoverAlgorithm
{
public:
  /**
   * \brief Creates an A2-A4-RSRP handover algorithm instance.
   */
  A2A4RsrpHandoverAlgorithm ();

  virtual ~A2A4RsrpHandoverAlgorithm ();


void SetTargetCellid(double cellid);
void SetServingCellThreshold (uint8_t th);

  // inherited from Object
  static TypeId GetTypeId ();

  // inherited from LteHandoverAlgorithm
  virtual void SetLteHandoverManagementSapUser (LteHandoverManagementSapUser* s);
  virtual LteHandoverManagementSapProvider* GetLteHandoverManagementSapProvider ();

  // let the forwarder class access the protected and private members
  friend class MemberLteHandoverManagementSapProvider<A2A4RsrpHandoverAlgorithm>;

protected:
  // inherited from Object
  virtual void DoInitialize ();
  virtual void DoDispose ();

  // inherited from LteHandoverAlgorithm as a Handover Management SAP implementation
  void DoReportUeMeas (uint16_t rnti, LteRrcSap::MeasResults measResults);

private:
  // Internal methods
  void EvaluateHandover (uint16_t rnti, uint8_t servingCellRsrp);
  bool IsValidNeighbour (uint16_t cellId);
  void UpdateNeighbourMeasurements (uint16_t rnti, uint16_t cellId,
                                    uint8_t rsrp);

  // The expected measurement identities
  uint8_t m_a2MeasId;
  uint8_t m_a4MeasId;
  

  /**
   * \brief Measurements reported by a UE for a cell ID.
   *
   * The values are quantized according 3GPP TS 36.133 section 9.1.4 and 9.1.7.
   */
  class UeMeasure : public SimpleRefCount<UeMeasure>
  {
  public:
    uint16_t m_cellId;
    uint8_t m_rsrp;
    uint8_t m_rsrq;
  };

  //               cellId
  typedef std::map<uint16_t, Ptr<UeMeasure> > MeasurementRow_t;
  //               rnti
  typedef std::map<uint16_t, MeasurementRow_t> MeasurementTable_t;
  MeasurementTable_t m_neighbourCellMeasures;

  // Class attributes
  uint8_t m_servingCellThreshold;
  double m_TargetCellid;
 /**
   * The `Hysteresis` attribute. Handover margin (hysteresis) in dB (rounded to
   * the nearest multiple of 0.5 dB).
   */
  double m_hysteresisDb;


  // Handover Management SAPs
  LteHandoverManagementSapUser* m_handoverManagementSapUser;
  LteHandoverManagementSapProvider* m_handoverManagementSapProvider;

}; // end of class A2A4RsrpHandoverAlgorithm


} // end of namespace ns3


#endif /* A2_A4_RSRP_HANDOVER_ALGORITHM_H */
