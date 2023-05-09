#include "utils/cJSON.c"
#include "utils/cJSON.h"

#include "ns3/applications-module.h"
#include "ns3/config-store-module.h"
#include "ns3/core-module.h"
#include "ns3/destination-ip-address.h"
#include "ns3/destination-mask.h"
#include "ns3/destination-port-number.h"
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

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <vector>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Quality of Service");

// Cjson Code start
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

static Filter*
createFilter(const cJSON* filters)
{
    Filter* filterWithElement = new Filter();

    xmlNode* point = nullptr;
    
            if (cJSON_GetObjectItem(filters,"source_address"))
            {
                Ipv4Address address(
                  cJSON_GetObjectItem(filters,"source_address")->valuestring
                );
                filterWithElement->elements.push_back(new SourceIpAddress(address));
            }
            if (cJSON_GetObjectItem(filters,"source_mask"))
            {
                Ipv4Mask mask(
                  cJSON_GetObjectItem(filters,"source_mask")->valuestring
                );
                filterWithElement->elements.push_back(new SourceMask(mask));
            }
            if (cJSON_GetObjectItem(filters, "source_port"))
            {
                uint32_t port = atoi(cJSON_GetObjectItem(filters,"source_port")->valuestring);
                filterWithElement->elements.push_back(new SourcePortNumber(port));
            }
            if (cJSON_GetObjectItem(filters, "destination_address"))
            {
                Ipv4Address address(cJSON_GetObjectItem(filters, "destination_address")->valuestring);
                filterWithElement->elements.push_back(new DestinationIpAddress(address));
            }
            if (cJSON_GetObjectItem(filters, "destination_mask"))
            {
                Ipv4Mask mask(cJSON_GetObjectItem(filters, "destination_mask")->valuestring);
                filterWithElement->elements.push_back(new DestinationMask(mask));
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

void
populate_traffic_vector(cJSON* json, std::string config_file, std::vector<TrafficClass*>& result)
{
        const cJSON* q1 = NULL;
        const cJSON* q2 = NULL;
        const cJSON* maxPackets = NULL;
        const cJSON* maxBytes = NULL;
        const cJSON* weight = NULL;
        const cJSON* priorityLevel = NULL;
        const cJSON* isDefault = NULL;
        const cJSON* filters = NULL;
        
        q1 = cJSON_GetObjectItemCaseSensitive(json, "q1");
        maxPackets = cJSON_GetObjectItem(q1, "max_packets");
        maxBytes = cJSON_GetObjectItem(q1, "max_bytes");
        weight = cJSON_GetObjectItem(q1, "weight");
        priorityLevel = cJSON_GetObjectItem(q1, "priority_level");
        isDefault = cJSON_GetObjectItem(q1, "is_default");
        filters = cJSON_GetObjectItem(q1, "filters");
        Filter* filter = createFilter(filters);
        std::vector<Filter*> filters_list;
        if(filter->elements.size()!=0){
          filters_list.push_back(filter);
        }
        //TC object for first queue
        TrafficClass* tc1 = new TrafficClass(
          atoi(maxPackets->valuestring),
          atoi(maxBytes->valuestring),
          atoi(weight->valuestring),
          atoi(priorityLevel->valuestring),
          atoi(isDefault->valuestring),
          filters_list
        ); 
        result.push_back(tc1);

        q2 = cJSON_GetObjectItemCaseSensitive(json, "q2");

        maxPackets = cJSON_GetObjectItem(q2, "max_packets");
        maxBytes = cJSON_GetObjectItem(q2, "max_bytes");
        weight = cJSON_GetObjectItem(q2, "weight");
        priorityLevel = cJSON_GetObjectItem(q2, "priority_level");
        isDefault = cJSON_GetObjectItem(q2, "is_default");
        filters = cJSON_GetObjectItem(q2, "filters");
        filter = createFilter(filters);
        if(filter->elements.size()!=0){
          filters_list.push_back(filter);
        }
        //TC object for first queue
        TrafficClass* tc2 = new TrafficClass(
          atoi(maxPackets->valuestring),
          atoi(maxBytes->valuestring),
          atoi(weight->valuestring),
          atoi(priorityLevel->valuestring),
          atoi(isDefault->valuestring),
          filters_list
        ); 
        result.push_back(tc2);
}

// cjson code end

int
main(int argc, char* argv[])
{
    // std::string fileName = "";
    std::string config_file = "";
    CommandLine cmd;
    cmd.AddValue("filename", "Name of the configuration file", config_file);
    cmd.Parse(argc, argv);

    //Cjson code start
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
        // initialize traffic vector
        populate_traffic_vector(json, config_file, traffics);
    }
    else
    {
        std::cout << "No config file detected" << std::endl;
        return 0;
    }

    // Cjson code end

    NodeContainer nodes;
    nodes.Create(3);

    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("4Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("2ms"));
    NetDeviceContainer node01 = p2p.Install(nodes.Get(0), nodes.Get(1));
    p2p.SetDeviceAttribute("DataRate", StringValue("1Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("2ms"));

    NetDeviceContainer node12 = p2p.Install(nodes.Get(1), nodes.Get(2));

    Ptr<PointToPointNetDevice> router_send = DynamicCast<PointToPointNetDevice>(node12.Get(0));

    Ptr<SPQ<Packet>> queue2 = new SPQ<Packet>(QueueMode::QUEUE_MODE_PACKETS, traffics);
    router_send->SetQueue(queue2);

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

    UdpServerHelper echoServer2(10);
    ApplicationContainer serverApps2 = echoServer2.Install(nodes.Get(2));
    serverApps2.Start(Seconds(1.0));
    serverApps2.Stop(Seconds(10000.0));

    // 1st sender wıll have source port 49153
    // 2nd sender wıll have source port 49154
    // 49153
    UdpClientHelper echoClient(interfaces2.GetAddress(1), 9);
    echoClient.SetAttribute("MaxPackets", UintegerValue(50000));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(0.001)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1000));

    ApplicationContainer client1 = echoClient.Install(nodes.Get(0));
    client1.Start(Seconds(3.000));
    client1.Stop(Seconds(10000.0));

    // 49154
    UdpClientHelper echoClient2(interfaces2.GetAddress(1), 10);
    echoClient2.SetAttribute("MaxPackets", UintegerValue(75000));
    echoClient2.SetAttribute("Interval", TimeValue(Seconds(0.001)));
    echoClient2.SetAttribute("PacketSize", UintegerValue(1000));

    ApplicationContainer client2 = echoClient2.Install(nodes.Get(0));
    client2.Start(Seconds(30.000)); // 5.101
    client2.Stop(Seconds(10000.0));

    // AnimationInterface anim ("spq_topology.xml");
    // anim.SetConstantPosition (nodes.Get(0), 0, 0);
    // anim.SetConstantPosition (nodes.Get(1), 10, 10);
    // anim.SetConstantPosition (nodes.Get(2), 20, 20);

    p2p.EnablePcapAll("spq");

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}