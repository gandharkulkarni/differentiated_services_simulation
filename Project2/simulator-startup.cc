#include "utils/cJSON.h"
#include "utils/cJSON.c"

#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

#include <climits>
#include <algorithm>
#include <functional>
#include <stdio.h>
#include <cstring>
#include <iomanip>
#include <map>

#include "ns3/ptr.h"
#include "ns3/object.h"
#include "ns3/event-id.h"
#include "ns3/histogram.h"
#include "ns3/flow-classifier.h"
#include "ns3/flow-probe.h"
#include "ns3/nstime.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/ipv4-flow-classifier.h"
#include "ns3/flow-monitor.h"

using std::cout;
using std::endl;

using namespace ns3;
std::string fileName = "";

struct spq_config
{
  int queue_count;
  int *queue_priority;
};
struct spq_config *config_spq;
struct drr_config
{
  int queue_count;
  int *queue_quantum;
};
struct drr_config *config_drr;

NS_LOG_COMPONENT_DEFINE ("DifferentiatedServicesSimulator");

cJSON *
read_config (std::string path)
{
  std::ifstream file (path);
  if (!file.is_open ())
    {
      std::cerr << "Error opening file: " << path << std::endl;
      return nullptr;
    }

  std::string buffer ((std::istreambuf_iterator<char> (file)), std::istreambuf_iterator<char> ());
  cJSON *json = cJSON_Parse (buffer.c_str ());
  file.close ();
  return json;
}
void
populate_config_struct (cJSON *json, std::string config_file)
{
  if (config_file == "DRR")
    {
      config_drr = new drr_config ();
      const cJSON *queue_count = NULL;
      const cJSON *queue_quantum = NULL;
      queue_count = cJSON_GetObjectItemCaseSensitive (json, "QueueCount");
      queue_quantum = cJSON_GetObjectItemCaseSensitive (json, "QueueQuantum");
      config_drr->queue_count = queue_count->valueint;

      int array_size = cJSON_GetArraySize (queue_quantum);

      config_drr->queue_quantum = new int[array_size];

      for (int i = 0; i < array_size; ++i)
        {
          cJSON *item = cJSON_GetArrayItem (queue_quantum, i);
          config_drr->queue_quantum[i] = item->valueint;
          printf ("Queue Quantum : %d\n", config_drr->queue_quantum[i]);
        }

      printf ("Queue count : %d\n", config_drr->queue_count);
    }
  else if (config_file == "SPQ")
    {
      config_spq = new spq_config ();
      const cJSON *queue_count = NULL;
      const cJSON *queue_priority = NULL;
      queue_count = cJSON_GetObjectItemCaseSensitive (json, "QueueCount");
      queue_priority = cJSON_GetObjectItemCaseSensitive (json, "QueuePriority");
      config_spq->queue_count = queue_count->valueint;

      int array_size = cJSON_GetArraySize (queue_priority);

      config_spq->queue_priority = new int[array_size];

      for (int i = 0; i < array_size; ++i)
        {
          cJSON *item = cJSON_GetArrayItem (queue_priority, i);
          config_spq->queue_priority[i] = item->valueint;
          printf ("Queue Priority : %d\n", config_spq->queue_priority[i]);
        }

      printf ("Queue count : %d\n", config_spq->queue_count);
    }
  else
    {
      cout << "Invalid config file input" << endl;
    }
}

int
main (int argc, char *argv[])
{
  uint32_t packetSize = 1000;
  uint32_t maxPacketCount = 3000;
  
  std::string dataRate = "40Mbps";
  std::string outputDataRate = "10Mbps";

  std::string config_file;
  //Read type of config from command line
  CommandLine cmd;

  cmd.AddValue ("cnfg", "Enter the config file name", config_file);
  cmd.Parse (argc, argv);

  if (!(config_file == "DRR" || config_file == "SPQ"))
    {
      std::cout << "You must specify the configuration file (SPQ/DRR)" << endl;
      return 0;
    }

  std::string config_path = "./scratch/Project2/config/" + config_file + ".json";
  std::ifstream cFile (config_path);

  //Check if config file exists
  if (cFile.is_open ())
    {
      cout << "Config file detected" << endl;
      //Read respective json file configuration
      cJSON *json = read_config (config_path);
      //initialize struct variables
      populate_config_struct (json, config_file);
    }
  else
    {
      cout << "No config file detected" << endl;
      return 0;
    }

  NS_LOG_INFO ("Create nodes.");
  
  NodeContainer node_container;
  node_container.Create (3);


  NodeContainer node_0_1 = NodeContainer (node_container.Get (0), node_container.Get (1));
  NodeContainer node_1_2 = NodeContainer (node_container.Get (1), node_container.Get (2));

  //install internet
  InternetStackHelper internet;
  internet.Install (node_container);

  NS_LOG_INFO ("Create channels.");
  PointToPointHelper p2p_helper;
  p2p_helper.SetDeviceAttribute ("DataRate", StringValue (dataRate));
  // p2p_helper.SetChannelAttribute ("Delay", StringValue ("2ms"));
  NetDeviceContainer device_0_1 = p2p_helper.Install (node_0_1);

  p2p_helper.SetDeviceAttribute ("DataRate", StringValue (outputDataRate));
  // p2p_helper.SetChannelAttribute ("Delay", StringValue ("2ms"));
  NetDeviceContainer device_1_2 = p2p_helper.Install (node_1_2);

  //Add IP addresses
  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interface_0_1 = ipv4.Assign (device_0_1);

  ipv4.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer interface_1_2 = ipv4.Assign (device_1_2);

  // Create router nodes, initialize routing database and set up the routing tables in the nodes.
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  NS_LOG_INFO ("Create Applications.");
  if (config_file == "SPQ") {
    
  }
  else { 
   }

  UdpServerHelper server_app1(9);

  ApplicationContainer serverApps = server_app1.Install (node_container.Get (2));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10000.0));

  UdpServerHelper server_app2 (10);
  ApplicationContainer serverApps2 = server_app2.Install (node_container.Get (2));
  serverApps2.Start (Seconds (1.0));
  serverApps2.Stop (Seconds (10000.0));

  UdpClientHelper echoClient (interface_1_2.GetAddress (1), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1000));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (0.001)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1000));

  ApplicationContainer client_app1 = echoClient.Install (node_container.Get (0));
  client_app1.Start (Seconds (3.000));
  client_app1.Stop (Seconds (10000.0));

  UdpClientHelper client_app2 (interface_1_2.GetAddress (1), 10);
  client_app2.SetAttribute ("MaxPackets", UintegerValue (1000));
  client_app2.SetAttribute ("Interval", TimeValue (Seconds (0.001)));
  client_app2.SetAttribute ("PacketSize", UintegerValue (1000));

  ApplicationContainer client2 = client_app2.Install (node_container.Get (0));
  client2.Start (Seconds (3.101)); 
  client2.Stop (Seconds (10000.0));

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;

}

