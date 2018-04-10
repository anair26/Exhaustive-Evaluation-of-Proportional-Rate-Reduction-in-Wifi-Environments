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

static void
RxDrop (Ptr<const Packet> p)
{
  NS_LOG_UNCOND ("RxDrop at " << Simulator::Now ().GetSeconds ());
}

int 
main (int argc, char *argv[])
{
  bool verbose = true;
  //uint32_t nCsma = 3;
  uint32_t nWifi = 1;
  bool tracing = true;

  CommandLine cmd;
  //cmd.AddValue ("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
  cmd.AddValue ("nWifi", "Number of wifi STA devices", nWifi);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
  cmd.AddValue ("tracing", "Enable pcap tracing", tracing);

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
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pNodes);

  //NodeContainer csmaNodes;
  //csmaNodes.Add (p2pNodes.Get (1));
  //csmaNodes.Create (nCsma);

  //CsmaHelper csma;
  //csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  //csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  //NetDeviceContainer csmaDevices;
  //csmaDevices = csma.Install (csmaNodes);

  NodeContainer wifiStaNodes;
  wifiStaNodes.Create (nWifi);
  NodeContainer wifiApNode = p2pNodes.Get (0);

  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
  phy.SetChannel (channel.Create ());

  WifiHelper wifi;
  wifi.SetRemoteStationManager ("ns3::AarfWifiManager");

  WifiMacHelper mac;
  Ssid ssid = Ssid ("ns-3-ssid");
  mac.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid),
               "ActiveProbing", BooleanValue (false));

  NetDeviceContainer staDevices;
  staDevices = wifi.Install (phy, mac, wifiStaNodes);

  mac.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid));

  NetDeviceContainer apDevices;
  apDevices = wifi.Install (phy, mac, wifiApNode);
  
//Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
  //em->SetAttribute ("ErrorRate", DoubleValue (0.00001));
 // devices.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em));
 //#include "ns3/yans-wifi-phy.h"


  
 /* Ptr<WifiPhy> myWifiPhy = CreateObject<WifiPhy> ();
  Ptr<WifiNetDevice> myWifiNetDevice = DynamicCast<WifiNetDevice> (staDevices.Get (0));
  myWifiPhy = myWifiNetDevice->GetPhy();
  //Ptr<YansWifiPhy> yp = DynamicCast<YansWifiPhy> (phy)
  myWifiPhy->SetAttribute ("RxNoiseFigure", DoubleValue(0.00001));
  staDevices.Get (0)->SetAttribute ("Phy", PointerValue(myWifiPhy)); */


//Suppose you have a Ptr<NetDevice> dev given by Node::GetDevice
//(uint32_t) or NetDeviceContainer::Get. You know that dev is actually a
//WifiNetDevice.

//chan
//cha
 Ptr<WifiNetDevice> myWifiNetDevice = DynamicCast<WifiNetDevice> (staDevices.Get (0)); //
//Converts a Ptr<NetDevice> to a Ptr<WifiNetDevice>
 Ptr<WifiPhy> myWifiPhy1 = myWifiNetDevice->GetPhy (); //works. dev->GetPhy () doesn't

 Ptr<YansWifiPhy> myWifiPhy = DynamicCast<YansWifiPhy> (myWifiPhy1); // phy
 myWifiPhy->SetAttribute ("RxNoiseFigure", DoubleValue(2.00));
  staDevices.Get (0)->SetAttribute ("Phy", PointerValue(myWifiPhy));
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
  address.Assign (apDevices);

  //UdpEchoServerHelper echoServer (9);
  uint16_t sinkPort = 8080;
  Address sinkAddress (InetSocketAddress (stInterface.GetAddress (0),sinkPort));
  PacketSinkHelper sink ("ns3::TcpSocketFactory",
                    InetSocketAddress (Ipv4Address::GetAny (), sinkPort));
  ApplicationContainer sinkApps = sink.Install (wifiStaNodes);
  sinkApps.Start (Seconds (0.0));
  sinkApps.Stop (Seconds (10.0));


  /*ApplicationContainer serverApps = echoServer.Install (csmaNodes.Get (nCsma));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));*/

  //UdpEchoClientHelper echoClient (csmaInterfaces.GetAddress (nCsma), 9);
  OnOffHelper onOffHelper ("ns3::TcpSocketFactory", sinkAddress);
  onOffHelper.SetAttribute ("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
  onOffHelper.SetAttribute ("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
  onOffHelper.SetAttribute ("DataRate",StringValue ("10Mbps"));
  onOffHelper.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer source;

  source.Add (onOffHelper.Install (p2pNodes.Get (1)));
  source.Start (Seconds (1.0));
  source.Stop (Seconds (10.0));
  /*echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = 
    echoClient.Install (wifiStaNodes.Get (nWifi - 1));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));*/

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  
  //change
  myWifiNetDevice->TraceConnectWithoutContext ("PhyRxDrop", MakeCallback (&RxDrop));
 // Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyRxDrop", MakeCallback(&RxDrop));

  Simulator::Stop (Seconds (11.0));

  if (tracing == true)
    {
      pointToPoint.EnablePcapAll ("third");
      phy.EnablePcap ("third", apDevices.Get (0));
      //csma.EnablePcap ("third", csmaDevices.Get (0), true);
    }

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
