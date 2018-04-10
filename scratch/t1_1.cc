/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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
 */

#include <fstream>
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/yans-wifi-phy.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/wifi-net-device.h"
#include "ns3/wifi-phy.h"
#include "ns3/packet.h"

// Default Network Topology
//
//   Wifi 10.1.3.0
//                 AP
//  *    *    *    *
//  |    |    |    |    10.1.1.0
// n5   n6   n7   n0 -------------- n1   n2   n3   n4
//                   point-to-point  |    |    |    |
//                                   ================
//                                     LAN 10.1.2.0

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ThirdScriptExample");

/*static void
RxDrop (Ptr<const Packet> p)
{
  NS_LOG_UNCOND ("RxDrop at " << Simulator::Now ().GetSeconds ());
}*/

Ptr<PacketSink> sink;                         /* Pointer to the packet sink application */
uint64_t lastTotalRx = 0;                     /* The value of the last total received bytes */

void
CalculateThroughput ()
{
  Time now = Simulator::Now ();                                         /* Return the simulator's virtual time. */
  double cur = (sink->GetTotalRx () - lastTotalRx) * (double) 8 / 1e5;     /* Convert Application RX Packets to MBits. */
  std::cout << now.GetSeconds () << "s: \t" << cur << " Mbit/s" << std::endl;
  lastTotalRx = sink->GetTotalRx ();
  Simulator::Schedule (MilliSeconds (100), &CalculateThroughput);
}



int 
main (int argc, char *argv[])
{
bool verbose = true;
  //uint32_t nCsma = 3;
  uint32_t nWifi = 1;
  bool tracing = true;
 std::string phyRate = "HtMcs7";
uint32_t payloadSize = 1472; 
double simulationTime = 10;  
  CommandLine cmd;
  cmd.AddValue ("payloadSize", "Payload size in bytes", payloadSize);
  //cmd.AddValue ("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
  cmd.AddValue ("nWifi", "Number of wifi STA devices", nWifi);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
  cmd.AddValue ("tracing", "Enable pcap tracing", tracing);
  cmd.AddValue ("simulationTime", "Simulation time in seconds", simulationTime);
cmd.AddValue ("phyRate", "Physical layer bitrate", phyRate);

  cmd.Parse (argc,argv);

  // The underlying restriction of 18 is due to the grid position
  // allocator's configuration; the grid layout will exceed the
  // bounding box if more than 18 nodes are provided.
  if (nWifi > 18)
    {
      std::cout << "nWifi should be 18 or less; otherwise grid layout exceeds the bounding box" << std::endl;
      return 1;
    }

  if (verbose)
    {
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

  NodeContainer p2pNodes;
  p2pNodes.Create (2);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("1ms"));

  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pNodes);


/* Configure TCP Options */
  Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (payloadSize));

  WifiMacHelper wifiMac;
  WifiHelper wifiHelper;
  wifiHelper.SetStandard (WIFI_PHY_STANDARD_80211n_5GHZ);

  
  NodeContainer wifiStaNodes;
  wifiStaNodes.Create (nWifi);
  NodeContainer wifiApNode = p2pNodes.Get (0);


/* Set up Legacy Channel */
  YansWifiChannelHelper wifiChannel;
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel", "Frequency", DoubleValue (5e9));

 YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
  wifiPhy.SetChannel (wifiChannel.Create ());
  wifiPhy.Set ("TxPowerStart", DoubleValue (1.0));
  wifiPhy.Set ("TxPowerEnd", DoubleValue (1.0));
  wifiPhy.Set ("TxPowerLevels", UintegerValue (1));
  wifiPhy.Set ("TxGain", DoubleValue (0));
  wifiPhy.Set ("RxGain", DoubleValue (0));
  wifiPhy.Set ("RxNoiseFigure", DoubleValue (20));
  wifiPhy.Set ("CcaMode1Threshold", DoubleValue (-79));
  wifiPhy.Set ("EnergyDetectionThreshold", DoubleValue (-79 + 3));
  wifiPhy.SetErrorRateModel ("ns3::YansErrorRateModel");
  wifiHelper.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                      "DataMode", StringValue (phyRate),
                                      "ControlMode", StringValue ("HtMcs0"));

  WifiHelper wifi;
  wifi.SetRemoteStationManager ("ns3::AarfWifiManager");

  WifiMacHelper mac;
  Ssid ssid = Ssid ("ns-3-ssid");
  mac.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid),
               "ActiveProbing", BooleanValue (false));

  NetDeviceContainer staDevices;
  staDevices = wifi.Install ( wifiPhy, mac, wifiStaNodes);
  
  //staDevices.Get (0)->SetAttribute ("Phy", PointerValue(myWifiPhy));


  mac.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid));

  NetDeviceContainer apDevice;
  
  
  apDevice = wifi.Install ( wifiPhy, mac, wifiApNode);
  
 MobilityHelper mobility;

  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wifiStaNodes);

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wifiApNode);
  
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (p2pNodes.Get (1));

  InternetStackHelper stack;
  //stack.Install (csmaNodes);
  stack.Install (p2pNodes.Get (1));
  stack.Install (wifiApNode);
  stack.Install (wifiStaNodes);

  Ipv4AddressHelper address;

  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces, stInterface;
  p2pInterfaces = address.Assign (p2pDevices);

  //address.SetBase ("10.1.2.0", "255.255.255.0");
  //Ipv4InterfaceContainer csmaInterfaces;
  //csmaInterfaces = address.Assign (csmaDevices);

  address.SetBase ("10.1.3.0", "255.255.255.0");
  stInterface = address.Assign (staDevices);
  address.Assign (apDevice);


//UdpEchoServerHelper echoServer (9);
  uint16_t sinkPort = 8080;
  Address sinkAddress (InetSocketAddress (stInterface.GetAddress (0),sinkPort));
  PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory",InetSocketAddress (Ipv4Address::GetAny (), sinkPort));
  ApplicationContainer sinkApp = sinkHelper.Install (wifiStaNodes);
  sink = StaticCast<PacketSink> (sinkApp.Get (0));
//  sinkApp.Start (Seconds (0.0));
//  sinkApp.Stop (Seconds (10.0));
    
  OnOffHelper server ("ns3::TcpSocketFactory", sinkAddress);
  server.SetAttribute ("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
  server.SetAttribute ("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
  server.SetAttribute ("DataRate",StringValue ("10Mbps"));
  server.SetAttribute ("PacketSize", UintegerValue (1024));
  ApplicationContainer serverApp = server.Install (wifiStaNodes);


   sinkApp.Start (Seconds (0.0));
  serverApp.Start (Seconds (1.0));
   sinkApp.Stop (Seconds (10.0));

 ApplicationContainer source;

  source.Add (server.Install (p2pNodes.Get (1)));
  source.Start (Seconds (0.0));
 
 Simulator::Schedule (Seconds (1.1), &CalculateThroughput);
 source.Stop (Seconds (10.0));

Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  
Simulator::Stop (Seconds (11.0));

  if (tracing == true)
    {
      wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11_RADIO);
      wifiPhy.EnablePcap ("t1_ap", apDevice);
      wifiPhy.EnablePcap ("t1_Sta", staDevices);
    }
   Simulator::Stop (Seconds (simulationTime + 1));
  Simulator::Run ();
  Simulator::Destroy ();

  double averageThroughput = ((sink->GetTotalRx () * 8) / (1e6  * simulationTime));
  if (averageThroughput < 5)
    {
      NS_LOG_ERROR ("Obtained throughput is not in the expected boundaries!");
      exit (1);
    }
  std::cout << "\nAverage throughput: " << averageThroughput << " Mbit/s" << std::endl;


  return 0;
}
