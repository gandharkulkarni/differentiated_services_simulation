#include "source-mask.h"

#include "ns3/ipv4-header.h"
#include "ns3/log.h"
#include "ns3/point-to-point-net-device.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("SourceMask");
NS_OBJECT_ENSURE_REGISTERED(SourceMask);

TypeId
SourceMask::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::SourceMask").SetParent<FilterElement>().SetGroupName("QoS");
    return tid;
}

SourceMask::SourceMask()
{
    NS_LOG_FUNCTION(this);
}

SourceMask::~SourceMask()
{
    NS_LOG_FUNCTION(this);
}

SourceMask::SourceMask(Ipv4Address source_ip_address, Ipv4Mask ipv4Mask)
{
    NS_LOG_FUNCTION(this);
    this->source_ip_address = source_ip_address;
    this->value = ipv4Mask;
}

/**
 * Compares source mask of packet again filter elements
 * @param Packet
 * @return bool
 */
bool
SourceMask::match(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this << packet);

    Ptr<Packet> packet_copy = packet->Copy();
    PppHeader pppHeader;
    packet_copy->RemoveHeader(pppHeader);

    Ipv4Header ipv4Header;
    packet_copy->PeekHeader(ipv4Header);
    Ipv4Address ipv4Address = ipv4Header.GetSource();

    if (this->value.IsMatch(ipv4Address, this->source_ip_address))
    {
        return true;
    }
    else
    {
        return false;
    }
}

} // namespace ns3