#include "traffic-class.h"

#include "ns3/log.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("TrafficClass");
NS_OBJECT_ENSURE_REGISTERED(TrafficClass);

TypeId
TrafficClass::GetTypeId(void)
{
    static TypeId tid =
        TypeId("ns3::TrafficClass").SetParent<Object>().SetGroupName("QoS");
    return tid;
}

TrafficClass::TrafficClass()
{
    NS_LOG_FUNCTION(this);
}

TrafficClass::TrafficClass(uint32_t max_packets,
                           uint32_t max_bytes,
                           uint32_t priority_level,
                           bool is_default,
                           std::vector<Filter*> filters)
{
    this->max_packets = max_packets;
    this->max_bytes = max_bytes;
    this->priority_level = priority_level;
    this->is_default = is_default;
    this->filters = filters;
    NS_LOG_FUNCTION(this);
}

TrafficClass::TrafficClass(uint32_t maxPackets,
                           uint32_t quantum_size,
                           bool is_default,
                           std::vector<Filter*> filters)
{
    this->max_packets = maxPackets;
    this->quantum_size = quantum_size;
    this->deficit_counter = 0;
    this->is_default = is_default;
    this->filters = filters;
    NS_LOG_FUNCTION(this);
}

TrafficClass::~TrafficClass()
{
    NS_LOG_FUNCTION(this);
}

// for each filter in vector of filter call match on each filter
//  even if one match -- return true
bool
TrafficClass::match(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this << packet);

    for (Filter* filter : filters)
    {
        if (filter->match(packet))
        {
            return true;
        }
    }
    return false;
}

bool
TrafficClass::IsEmpty()
{
    return m_queue.empty();
}

/***
 * To Enqueue the packet
 * */
bool
TrafficClass::Enqueue(Ptr<Packet> packet)
{
    bytes += packet->GetSize();
    m_queue.push(packet);

    return true;
}

/***
 * To Dequeue the packet
 * */
Ptr<ns3::Packet>
TrafficClass::Dequeue()
{
    NS_LOG_FUNCTION(this);

    if (m_queue.empty())
    {
        NS_LOG_LOGIC("Queue empty");
        return 0;
    }

    Ptr<Packet> p = m_queue.front();
    m_queue.pop();
    bytes -= p->GetSize();
    NS_LOG_LOGIC("Popped " << p);
    NS_LOG_LOGIC("Number bytes " << bytes);

    return p;
}

/***
 * To Remove the packet
 * */
Ptr<ns3::Packet>
TrafficClass::Remove()
{
    if (!m_queue.empty())
    {
        m_queue.pop();
    }
    return 0;
}

/***
 * To Peek the packet
 * */
Ptr<ns3::Packet>
TrafficClass::Peek()
{
    NS_LOG_FUNCTION(this);

    if (m_queue.empty())
    {
        NS_LOG_LOGIC("Queue empty");

        return 0;
    }

    Ptr<Packet> p = m_queue.front();
    packets = m_queue.size();
    bytes = bytes;

    return p;
}

void
TrafficClass::SetQuantumSize(uint32_t q)
{
    quantum_size = q;
}

uint32_t
TrafficClass::GetQuantumSize()
{
    return quantum_size;
}

void
TrafficClass::SetDeficitCounter(uint32_t d)
{
    deficit_counter = d;
}

uint32_t
TrafficClass::GetDeficitCounter()
{
    return deficit_counter;
}

uint32_t
TrafficClass::GetMaxPacket()
{
    return max_packets;
}

uint32_t
TrafficClass::GetPacketCount()
{
    return m_queue.size();
}

uint32_t
TrafficClass::GetPriorityLevel()
{
    return priority_level;
}

bool
TrafficClass::GetDefault()
{
    return is_default;
}

} // namespace ns3