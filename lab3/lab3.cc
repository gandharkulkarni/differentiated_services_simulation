/* CS621: Lab 3 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/packet-capture-helper.h"
#include "ns3/net-device.h"

// Default Network Topology
//
//       10.1.1.0
// n0 -------------- n1
//    point-to-point
//
 
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("MyFirstScriptExample");

void classify(Ptr<QueueDisc> queueDisc, Ptr<const Packet> packet, unit16_t protocol, const Address &from, const Address &to, unit16_t len){
  if(protocol == 17){
    const UdpHeader udpHeader;
    packet->PeekHeader(udpHeader);

    if(udpHeader.GetDestinationPort() == 10000){
      queueDisc->Enqueue(packet, from, to, len);
    }
    else if(udpHeader.GetDestinationPort() == 20000){
      queueDisc->Enqueue(packet, from, to, len, 1);
    }

  }
}

bool schedule(Ptr<QueueDisc> queueDisc){
  return queueDisc->GetNBytes()>0;
}

int
main (int argc, char *argv[])
{
  CommandLine cmd (__FILE__);
  cmd.Parse (argc, argv);
  
  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  NodeContainer nodes;
  nodes.Create (3);

  PointToPointHelper p2pHelper;
  p2pHelper.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
  
  NetDeviceContainer dev1 = p2pHelper.Install(nodes.Get(0), nodes.Get(1));
  
  p2pHelper.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));

  NetDeviceContainer dev2 = p2pHelper.Install(nodes.Get(1), nodes.Get(2));


  // clientToRouter.SetChannelAttribute ("Delay", StringValue ("20ms"));

  NetDeviceContainer devices;
  devices = pointToPoint.Install (nodes);

  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");

  Ipv4InterfaceContainer interfaces = address.Assign(dev1);  

  address.SetBase("10.1.2.0", "255.255.255.0");
  address.Assign(dev2);

  // Add a router node
  NodeContainer router;
  router.Create(1);

  // Connect router to the existing nodes
  p2pHelper.SetDeviceAttribute("DataRate", StringValue("100Mbps"));

  NetDeviceContainer dev3 = p2pHelper.Install(nodes.Get(0), router.Get(0));
  p2pHelper.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
  NetDeviceContainer dev4 = p2pHelper.Install(router.Get(0), nodes.Get(2));

  //Set Up IP address for the router
  address.SetBase("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer routerInterfaces = address.Assign(dev3);

  address.SetBase("10.1.4.0", "255.255.255.0");
  address.Assign(dev4);

  //Set up router to have 2 queues
  QueueDiscContainer queueDiscs;

  // Create Queue Discs
  TrafficControlHelper trafficHelper;
  trafficHelper.SetRootQueueDisc("ns3::PfifoFastQueueDisc");
  queueDiscs = trafficHelper.Install(dev3);

  Ptr<Queue> q1 = queueDisc->GetInternalQueue(0,0);
  Ptr<Queue> q2 = queueDisc->GetInternalQueue(0,1);

  unit16_t port1 = 10000;
  unit16_t port2 = 20000;

  // Create UDP applications
  UdpServerHelper server1(port1);
  ApplicationContainer serverApps1 = server1.Install(nodes.Get(2));
  serverApps1.Start(Seconds(1.0));
  serverApps1.Stop(Seconds(10.0));

  UdpServerHelper server2(port2);
  ApplicationContainer serverApps2 = server2.Install(nodes.Get(2));
  serverApps2.Start(Seconds(1.0));
  serverApps2.Stop(Seconds(10.0));

  UdpClientHelper client1(routerInterfaces.GetAddress(0), port1);
  client1.SetAttribute("MaxPackets", UintegerValue(10000));
  client1.SetAttribute("Interval", TimeValue(MilliSeconds(1)));
  client1.SetAttribute("PacketSize", UintegerValue(1024));
  ApplicationContainer clientApps1 = client1.Install(nodes.Get(0));
  clientApps1.Start(Seconds(2.0));
  clientApps1.Stop(Seconds(10.0));

  UdpClientHelper client2(routerInterfaces.GetAddress(0), port2);
  client2.SetAttribute("MaxPackets", UintegerValue(10000));
  client2.SetAttribute("Interval", TimeValue(MilliSeconds(1)));
  client2.SetAttribute("PacketSize", UintegerValue(1024));
  ApplicationContainer clientApps2 = client2.Install(nodes.Get(0));
  clientApps2.Start(Seconds(2.0));
  clientApps2.Stop(Seconds(10.0));

  // Ipv4InterfaceContainer interfaces = address.Assign (devices);

  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (nodes.Get (1));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  UdpEchoClientHelper echoClient (interfaces.GetAddress (1), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));
  
  echoClient.SetFill(clientApps.Get(0), "Hello World");
  pointToPoint.EnablePcapAll("udp-echo");

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}