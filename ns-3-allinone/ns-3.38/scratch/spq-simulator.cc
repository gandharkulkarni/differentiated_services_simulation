#include "ns3/spq.h"
#include "ns3/traffic-class.h"
#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ppp-header.h"
#include "ns3/traffic-generator.h"

#include <queue>
#include <string>
#include <vector>
int
main(int argc, char* argv[])
{
    CommandLine cmd(__FILE__);
    cmd.Parse(argc, argv);

    // LogComponentEnable("UdpApplication", LOG_LEVEL_INFO);
    // LogComponentEnable("UdpServerApplication", LOG_LEVEL_INFO);

    // NS_LOG_INFO("Create nodes.");
    NodeContainer nodes;
    nodes.Create(3);

    ns3::NodeContainer n0r = NodeContainer(nodes.Get(0), nodes.Get(1));
    ns3::NodeContainer n1r = NodeContainer(nodes.Get(1), nodes.Get(2));

    InternetStackHelper internet;
    internet.Install(nodes);

    NetDeviceContainer d0r;
    Ptr<PointToPointNetDevice> devA = CreateObject<PointToPointNetDevice>();
    devA->SetAttribute("DataRate", StringValue("10Mbps"));
    devA->SetAddress(Mac48Address::Allocate());
    n0r.Get(0)->AddDevice(devA);
    Ptr<SPQ<Packet>> queueA = CreateObject<SPQ<Packet>>();
    devA->SetQueue(queueA);

    Ptr<PointToPointNetDevice> devB = CreateObject<PointToPointNetDevice>();
    devB->SetAttribute("DataRate", StringValue("10Mbps"));
    devB->SetAddress(Mac48Address::Allocate());
    n0r.Get(1)->AddDevice(devB);
    Ptr<SPQ<Packet>> queueB = CreateObject<SPQ<Packet>>();
    devB->SetQueue(queueB);

    Ptr<PointToPointChannel> channel1 = CreateObject<PointToPointChannel>();
    devA->Attach(channel1);
    devB->Attach(channel1);
    d0r.Add(devA);
    d0r.Add(devB);

    NetDeviceContainer d1r;
    Ptr<PointToPointNetDevice> devC = CreateObject<PointToPointNetDevice>();
    devC->SetAttribute("DataRate", StringValue("10Mbps"));
    devC->SetAddress(Mac48Address::Allocate());
    n1r.Get(0)->AddDevice(devC);
    Ptr<SPQ<Packet>> queueC = CreateObject<SPQ<Packet>>();
    devC->SetQueue(queueC);

    Ptr<PointToPointNetDevice> devD = CreateObject<PointToPointNetDevice>();
    devD->SetAttribute("DataRate", StringValue("10Mbps"));
    devD->SetAddress(Mac48Address::Allocate());
    n1r.Get(1)->AddDevice(devD);
    Ptr<SPQ<Packet>> queueD = CreateObject<SPQ<Packet>>();
    devD->SetQueue(queueD);

    Ptr<PointToPointChannel> channel2 = CreateObject<PointToPointChannel>();
    devC->Attach(channel2);
    devD->Attach(channel2);
    d1r.Add(devC);
    d1r.Add(devD);

    // eNS_LOG_INFO("Assign IP Addresses.");
    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer i0r = ipv4.Assign(d0r);
    ipv4.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer i1r = ipv4.Assign(d1r);

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    UdpServerHelper Server(9);

    ApplicationContainer serverApps = Server.Install(nodes.Get(2));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(20.0));

    Ptr<Socket> socket1 = Socket::CreateSocket(n0r.Get(0), UdpSocketFactory::GetTypeId());
    socket1->Bind();
    Address address1(InetSocketAddress(i1r.GetAddress(1), 9));

    Ptr<Socket> socket2 = Socket::CreateSocket(n0r.Get(0), UdpSocketFactory::GetTypeId());
    socket2->Bind();
    Address address2(InetSocketAddress(i1r.GetAddress(1), 9));

    Ptr<TrafficGenerator> clientApps1 = CreateObject<TrafficGenerator>();
    nodes.Get(0)->AddApplication(clientApps1);
    clientApps1->SetStartTime(Seconds(1.0));
    clientApps1->SetStopTime(Seconds(20.0));
    clientApps1->Setup(socket1, address1, 1000, 1000000, DataRate("10Mbps"), 8888);

    Ptr<TrafficGenerator> clientApps2 = CreateObject<TrafficGenerator>();
    nodes.Get(0)->AddApplication(clientApps2);
    clientApps2->SetStartTime(Seconds(10.0));
    clientApps2->SetStopTime(Seconds(20.0));
    clientApps2->Setup(socket2, address2, 1000, 1500000, DataRate("10Mbps"), 9999);

    PointToPointHelper pointToPoint0;
    pointToPoint0.EnablePcapAll("udp_new");

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}