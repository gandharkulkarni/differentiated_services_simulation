#include "utils/cJSON.c"
#include "utils/cJSON.h"

#include "ns3/applications-module.h"
#include "ns3/config-store-module.h"
#include "ns3/core-module.h"
#include "ns3/destination-ip-address.h"
#include "ns3/destination-mask.h"
#include "ns3/destination-port-number.h"
#include "ns3/drr.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-net-device.h"
#include "ns3/protocol-number.h"
#include "ns3/source-ip-address.h"
#include "ns3/source-mask.h"
#include "ns3/source-port-number.h"
#include "ns3/spq.h"
#include <vector>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Quality of Service");

/**
 * Reads JSON config file
 * @param String path
 * @return cJSON*
*/
cJSON*
read_config(std::string path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Error opening file: " << path << std::endl;
        return nullptr;
    }

    std::string buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    cJSON* json = cJSON_Parse(buffer.c_str());
    file.close();
    return json;
}

/**
 * Method to create filter objects based on filters defined in config json
 * @param cJSON* filters
 * @return Filter
*/
static Filter*
createFilter(const cJSON* filters)
{
    Filter* filterWithElement = new Filter();

    if (cJSON_GetObjectItem(filters, "source_address"))
    {
        Ipv4Address address(cJSON_GetObjectItem(filters, "source_address")->valuestring);
        filterWithElement->elements.push_back(new SourceIpAddress(address));
    }
    if (cJSON_GetObjectItem(filters, "source_mask") && cJSON_GetObjectItem(filters, "source_address"))
    {
        Ipv4Address address(cJSON_GetObjectItem(filters, "source_address")->valuestring);
        Ipv4Mask mask(cJSON_GetObjectItem(filters, "source_mask")->valuestring);
        filterWithElement->elements.push_back(new SourceMask(address, mask));
    }
    if (cJSON_GetObjectItem(filters, "source_port"))
    {
        uint32_t port = atoi(cJSON_GetObjectItem(filters, "source_port")->valuestring);
        filterWithElement->elements.push_back(new SourcePortNumber(port));
    }
    if (cJSON_GetObjectItem(filters, "destination_address"))
    {
        Ipv4Address address(cJSON_GetObjectItem(filters, "destination_address")->valuestring);
        filterWithElement->elements.push_back(new DestinationIpAddress(address));
    }
    if (cJSON_GetObjectItem(filters, "destination_mask") && cJSON_GetObjectItem(filters, "destination_address"))
    {
        Ipv4Address address(cJSON_GetObjectItem(filters, "destination_address")->valuestring);
        Ipv4Mask mask(cJSON_GetObjectItem(filters, "destination_mask")->valuestring);
        filterWithElement->elements.push_back(new DestinationMask(address, mask));
    }
    if (cJSON_GetObjectItem(filters, "destination_port"))
    {
        uint32_t port = atoi(cJSON_GetObjectItem(filters, "destination_port")->valuestring);
        filterWithElement->elements.push_back(new DestinationPortNumber(port));
    }
    if (cJSON_GetObjectItem(filters, "protocol"))
    {
        uint32_t protocol = atoi(cJSON_GetObjectItem(filters, "protocol")->valuestring);
        filterWithElement->elements.push_back(new ProtocolNumber(protocol));
    }
    return filterWithElement;
}

/**
 * Populates traffic class vector
 * @param cJSON*
 * @param string
 * @param vector<TrafficClass*>
*/
void
populate_traffic_class_vector(cJSON* json, std::string config_file, std::vector<TrafficClass*>& result)
{
    const cJSON* q1 = NULL;
    const cJSON* q2 = NULL;
    const cJSON* q3 = NULL;
    const cJSON* maxPackets = NULL;
    const cJSON* maxBytes = NULL;
    const cJSON* priorityLevel = NULL;
    const cJSON* quantumSize = NULL;
    const cJSON* isDefault = NULL;
    const cJSON* filters = NULL;

    q1 = cJSON_GetObjectItemCaseSensitive(json, "q1");
    maxPackets = cJSON_GetObjectItem(q1, "max_packets");
    maxBytes = cJSON_GetObjectItem(q1, "max_bytes");
    priorityLevel = cJSON_GetObjectItem(q1, "priority_level");
    quantumSize = cJSON_GetObjectItem(q1, "quantum_size");
    isDefault = cJSON_GetObjectItem(q1, "is_default");
    filters = cJSON_GetObjectItem(q1, "filters");

    Filter* filter = createFilter(filters);

    std::vector<Filter*> filters_list;
    if (filter->elements.size() != 0)
    {
        filters_list.push_back(filter);
    }

    if (config_file == "SPQ")
    {
        
        // TC object for first queue
        TrafficClass* tc1 = new TrafficClass(atoi(maxPackets->valuestring),
                                             atoi(maxBytes->valuestring),
                                             atoi(priorityLevel->valuestring),
                                             atoi(isDefault->valuestring),
                                             filters_list);
        result.push_back(tc1);

        q2 = cJSON_GetObjectItemCaseSensitive(json, "q2");

        maxPackets = cJSON_GetObjectItem(q2, "max_packets");
        maxBytes = cJSON_GetObjectItem(q2, "max_bytes");
        priorityLevel = cJSON_GetObjectItem(q2, "priority_level");
        isDefault = cJSON_GetObjectItem(q2, "is_default");
        filters = cJSON_GetObjectItem(q2, "filters");
        filter = createFilter(filters);
        filters_list.clear();
        if (filter->elements.size() != 0)
        {
            filters_list.push_back(filter);
        }
        // TC object for first queue
        TrafficClass* tc2 = new TrafficClass(atoi(maxPackets->valuestring),
                                             atoi(maxBytes->valuestring),
                                             atoi(priorityLevel->valuestring),
                                             atoi(isDefault->valuestring),
                                             filters_list);
        result.push_back(tc2);
    }
    else if (config_file == "DRR")
    {
        // TC object for first queue
        TrafficClass* tc1 = new TrafficClass(atoi(maxPackets->valuestring),
                                             atoi(quantumSize->valuestring),
                                             atoi(isDefault->valuestring),
                                             filters_list);
        result.push_back(tc1);

        q2 = cJSON_GetObjectItemCaseSensitive(json, "q2");

        maxPackets = cJSON_GetObjectItem(q2, "max_packets");
        maxBytes = cJSON_GetObjectItem(q2, "max_bytes");
        quantumSize = cJSON_GetObjectItem(q2, "quantum_size");
        isDefault = cJSON_GetObjectItem(q2, "is_default");
        filters = cJSON_GetObjectItem(q2, "filters");
        filter = createFilter(filters);
        filters_list.clear();
        if (filter->elements.size() != 0)
        {
            filters_list.push_back(filter);
        }
        // TC object for second queue
        TrafficClass* tc2 = new TrafficClass(atoi(maxPackets->valuestring),
                                             atoi(quantumSize->valuestring),
                                             atoi(isDefault->valuestring),
                                             filters_list);
        result.push_back(tc2);

        q3 = cJSON_GetObjectItemCaseSensitive(json, "q3");

        maxPackets = cJSON_GetObjectItem(q3, "max_packets");
        maxBytes = cJSON_GetObjectItem(q3, "max_bytes");
        quantumSize = cJSON_GetObjectItem(q3, "quantum_size");
        isDefault = cJSON_GetObjectItem(q3, "is_default");
        filters = cJSON_GetObjectItem(q3, "filters");
        filter = createFilter(filters);
        filters_list.clear();
        if (filter->elements.size() != 0)
        {
            filters_list.push_back(filter);
        }

        // TC object for third queue
        TrafficClass* tc3 = new TrafficClass(atoi(maxPackets->valuestring),
                                             atoi(quantumSize->valuestring),
                                             atoi(isDefault->valuestring),
                                             filters_list);
        result.push_back(tc3);
    }
}


int
main(int argc, char* argv[])
{
    std::string config_file = "";
    CommandLine cmd;
    cmd.AddValue("filename", "Name of the configuration file", config_file);
    cmd.Parse(argc, argv);

    if (!(config_file == "DRR" || config_file == "SPQ"))
    {
        std::cout << "You must specify the configuration file (SPQ/DRR)" << std::endl;
        return 0;
    }

    std::string config_path = "./scratch/config/" + config_file + ".json";
    std::ifstream cFile(config_path);
    std::vector<TrafficClass*> traffics;
    // Check if config file exists
    if (cFile.is_open())
    {
        std::cout << "Config file detected" << std::endl;
        // Read respective json file configuration
        cJSON* json = read_config(config_path);
        // initialize struct variables
        populate_traffic_class_vector(json, config_file, traffics);
    }
    else
    {
        std::cout << "No config file detected" << std::endl;
        return 0;
    }

    if (config_file == "SPQ")
    {
        NodeContainer nodes;
        nodes.Create(3);
        std::cout<<"Nodes created"<<std::endl;
        PointToPointHelper p2p;
        p2p.SetDeviceAttribute("DataRate", StringValue("4Mbps"));
        p2p.SetChannelAttribute("Delay", StringValue("10ms"));
        NetDeviceContainer node01 = p2p.Install(nodes.Get(0), nodes.Get(1));
 
        p2p.SetDeviceAttribute("DataRate", StringValue("1Mbps"));
        p2p.SetChannelAttribute("Delay", StringValue("10ms"));

        NetDeviceContainer node12 = p2p.Install(nodes.Get(1), nodes.Get(2));

        Ptr<PointToPointNetDevice> router = DynamicCast<PointToPointNetDevice>(node12.Get(0));

        std::cout<<"SPQ simulation started"<<std::endl;

        Ptr<SPQ<Packet>> queue = new SPQ<Packet>(QueueMode::QUEUE_MODE_PACKETS, traffics);
        router->SetQueue(queue);

        InternetStackHelper stack;
        stack.Install(nodes);

        Ipv4AddressHelper address;
        address.SetBase("10.1.1.0", "255.255.255.0");
        Ipv4InterfaceContainer interfaces1 = address.Assign(node01);
        address.SetBase("10.1.2.0", "255.255.255.0");
        Ipv4InterfaceContainer interfaces2 = address.Assign(node12);

        Ipv4GlobalRoutingHelper::PopulateRoutingTables();
        UdpServerHelper echoServer(9);

        ApplicationContainer serverApps = echoServer.Install(nodes.Get(2));
        serverApps.Start(Seconds(1.0));
        serverApps.Stop(Seconds(10000.0));

        // 1st sender wıll have source port 49153
        // 2nd sender wıll have source port 49154 and so on
        // 49153
        UdpClientHelper echoClient(interfaces2.GetAddress(1), 9);
        echoClient.SetAttribute("MaxPackets", UintegerValue(50000));
        echoClient.SetAttribute("Interval", TimeValue(Seconds(0.001)));
        echoClient.SetAttribute("PacketSize", UintegerValue(1000));

        ApplicationContainer client1 = echoClient.Install(nodes.Get(0));
        client1.Start(Seconds(3.000));
        client1.Stop(Seconds(10000.0));

        // 49154
        UdpClientHelper echoClient2(interfaces2.GetAddress(1), 9);
        echoClient2.SetAttribute("MaxPackets", UintegerValue(75000));
        echoClient2.SetAttribute("Interval", TimeValue(Seconds(0.001)));
        echoClient2.SetAttribute("PacketSize", UintegerValue(1000));

        ApplicationContainer client2 = echoClient2.Install(nodes.Get(0));
        client2.Start(Seconds(30.000)); 
        client2.Stop(Seconds(10000.0));

        p2p.EnablePcapAll("spq");

        Simulator::Run();
        Simulator::Destroy();
        std::cout<<"Pcap file generated"<<std::endl;
        return 0;
    }
    else if (config_file == "DRR")
    {
        NodeContainer nodes;
        nodes.Create(3);
        std::cout<<"Nodes created"<<std::endl;
        PointToPointHelper p2p;
        p2p.SetDeviceAttribute("DataRate", StringValue("4Mbps"));
        p2p.SetChannelAttribute("Delay", StringValue("10ms"));
        NetDeviceContainer node01 = p2p.Install(nodes.Get(0), nodes.Get(1));
 
        p2p.SetDeviceAttribute("DataRate", StringValue("1Mbps"));
        p2p.SetChannelAttribute("Delay", StringValue("10ms"));

        NetDeviceContainer node12 = p2p.Install(nodes.Get(1), nodes.Get(2));

        Ptr<PointToPointNetDevice> router = DynamicCast<PointToPointNetDevice>(node12.Get(0));

        std::cout<<"DRR simulation started"<<std::endl;

        Ptr<DRR<Packet>> queue = new DRR<Packet>(QueueMode::QUEUE_MODE_PACKETS, traffics);
        router->SetQueue(queue);

        InternetStackHelper stack;
        stack.Install(nodes);

        Ipv4AddressHelper address;
        address.SetBase("10.1.1.0", "255.255.255.0");
        Ipv4InterfaceContainer interfaces1 = address.Assign(node01);
        address.SetBase("10.1.2.0", "255.255.255.0");
        Ipv4InterfaceContainer interfaces2 = address.Assign(node12);

        Ipv4GlobalRoutingHelper::PopulateRoutingTables();
        UdpServerHelper echoServer(9);

        ApplicationContainer serverApps = echoServer.Install(nodes.Get(2));
        serverApps.Start(Seconds(1.0));
        serverApps.Stop(Seconds(10000.0));

        // 1st sender wıll have source port 49153
        // 2nd sender wıll have source port 49154 and so on
        // 49153
        UdpClientHelper echoClient(interfaces2.GetAddress(1), 9);
        echoClient.SetAttribute("MaxPackets", UintegerValue(3000));
        echoClient.SetAttribute("Interval", TimeValue(Seconds(0.001)));
        echoClient.SetAttribute("PacketSize", UintegerValue(1000));

        ApplicationContainer client1 = echoClient.Install(nodes.Get(0));
        client1.Start(Seconds(3.000));
        client1.Stop(Seconds(10000.0));

        // 49154
        UdpClientHelper echoClient2(interfaces2.GetAddress(1), 9);
        echoClient2.SetAttribute("MaxPackets", UintegerValue(2000));
        echoClient2.SetAttribute("Interval", TimeValue(Seconds(0.001)));
        echoClient2.SetAttribute("PacketSize", UintegerValue(1000));

        ApplicationContainer client2 = echoClient2.Install(nodes.Get(0));
        client2.Start(Seconds(3.000));
        client2.Stop(Seconds(10000.0));

        // 49155
        UdpClientHelper echoClient3(interfaces2.GetAddress(1), 9);
        echoClient3.SetAttribute("MaxPackets", UintegerValue(1000));
        echoClient3.SetAttribute("Interval", TimeValue(Seconds(0.001)));
        echoClient3.SetAttribute("PacketSize", UintegerValue(1000));

        ApplicationContainer client3 = echoClient3.Install(nodes.Get(0));
        client3.Start(Seconds(3.000));
        client3.Stop(Seconds(10000.0));

        p2p.EnablePcapAll("drr");

        Simulator::Run();
        Simulator::Destroy();
        std::cout<<"Pcap file generated"<<std::endl;
        return 0;
    }
    return 0;
}