#include "spq.h"
#include "traffic-class.h"
#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ppp-header.h"

#include <math.h>
#include <queue>
#include <string>
#include <vector>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Spq");

template <typename Packet>
SPQ<Packet>::SPQ()
{
    // todo: read parameters from config file
    TrafficClass* tc_high = new TrafficClass(100000,
                                         2,
                                         false,
                                         "10.1.2.0",
                                         "255.255.255.0",
                                         9,
                                         17,
                                         "10.1.1.0",
                                         "255.255.255.0",
                                         9999);
    q_class.push_back(tc_high);

    TrafficClass* tc_low = new TrafficClass(100000,
                                         1,
                                         true,
                                         "10.1.2.0",
                                         "255.255.255.0",
                                         9,
                                         17,
                                         "10.1.1.0",
                                         "255.255.255.0",
                                         8888);
    q_class.push_back(tc_low);
}

template <typename Packet>
SPQ<Packet>::~SPQ()
{
    NS_LOG_FUNCTION (this);
}

template <typename Packet>
TypeId
SPQ<Packet>::GetTypeId(void)
{
    static TypeId tid = TypeId("SPQ<Packet>")
                            .SetParent<Queue<Packet>>()
                            .SetGroupName("SPQ")
                            .template AddConstructor<SPQ<Packet>>();
    return tid;
}

template <typename Packet>
bool
SPQ<Packet>::Enqueue(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this << packet);

    return DoEnqueue(packet);
}

template <typename Packet>
Ptr<Packet>
SPQ<Packet>::Dequeue()
{
    NS_LOG_FUNCTION(this);

    Ptr<Packet> packet = DoDequeue();

    NS_LOG_LOGIC("Popped " << packet);

    return packet;
}

template <typename Packet>
Ptr<Packet>
SPQ<Packet>::Remove()
{
    NS_LOG_FUNCTION(this);

    Ptr<Packet> packet = DoRemove();

    NS_LOG_LOGIC("Removed " << packet);

    return packet;
}

template <typename Packet>
Ptr<const Packet>
SPQ<Packet>::Peek() const
{
    NS_LOG_FUNCTION(this);

    return DoPeek();
}

template <typename Packet>
bool
SPQ<Packet>::DoEnqueue(Ptr<Packet> packet)
{
    uint32_t index_of_qclass = Classify(packet);
    if (index_of_qclass >= 0)
    {
        Ptr<Packet> copy = packet->Copy();
        PppHeader pppHeader;
        Ipv4Header ipHeader;
        UdpHeader header;
        copy->RemoveHeader(pppHeader);
        copy->RemoveHeader(ipHeader);
        copy->RemoveHeader(header);
        copy->PeekHeader(header);

        if (q_class[index_of_qclass]->GePackets() < q_class[index_of_qclass]->GetMaxPackets())
        {
            q_class[index_of_qclass]->getMqueue()->push(packet);
            return true;
        }
    }
    else
    {
        for (TrafficClass* tc : q_class)
        {
            if (tc->GetDefault() == true)
            {
                if (tc->GePackets() < tc->GetMaxPackets())
                {
                    tc->getMqueue()->push(packet);
                }
            }
        }
    }
    return false;
}

template <typename Packet>
Ptr<Packet>
SPQ<Packet>::DoDequeue()
{
    TrafficClass* tc = Schedule();
    if(tc != nullptr) {
        Ptr<Packet> packet = tc->getMqueue()->front();
        tc->getMqueue()->pop();
        return packet;
    } 
    return nullptr;
}

template <typename Packet>
Ptr<Packet>
SPQ<Packet>::DoRemove()
{
    TrafficClass* tc = Schedule();
    if(tc != nullptr) {
        Ptr<Packet> packet = tc->getMqueue()->front();
        tc->getMqueue()->pop();
        return packet;
    } 
    return nullptr;
}

template <typename Packet>
Ptr<const Packet>
SPQ<Packet>::DoPeek() const
{
    for(int i = 0; i < 2; i++) {          //todo: read '2' from config file
        if(!q_class[i]->IsEmpty()) {
            return q_class[i]->getMqueue()->front();
        }
    }
    return nullptr;
}

template <typename Packet>
uint32_t
SPQ<Packet>::Classify(Ptr<Packet> p)
{
    for (int i = 0; i < 2; i++)
    { // todo: read '2' from config
        if (q_class[i]->match(p))
        {
            return i;
        }
    }
    return -1;
}

template <typename Packet>
TrafficClass*
SPQ<Packet>::Schedule()
{
    for(int i = 0; i < 2; i++) {          //todo: read '2' from config file
        if(!q_class[i]->IsEmpty()) {
            return q_class[i];
        }
    }
    return nullptr;
}