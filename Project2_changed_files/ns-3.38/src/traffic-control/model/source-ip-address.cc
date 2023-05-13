#include "source-ip-address.h"

#include "ns3/ipv4-header.h"
#include "ns3/log.h"
#include "ns3/point-to-point-net-device.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("SourceIpAddress");

NS_OBJECT_ENSURE_REGISTERED(SourceIpAddress);

TypeId
SourceIpAddress::GetTypeId(void)
{
    static TypeId tid =
        TypeId("ns3::SourceIpAddress").SetParent<FilterElement>().SetGroupName("QoS");
    return tid;
}

SourceIpAddress::SourceIpAddress()
{
    NS_LOG_FUNCTION(this);
}

SourceIpAddress::SourceIpAddress(Ipv4Address ipV4Address)
{
    NS_LOG_FUNCTION(this);
    this->value = ipV4Address;
}

SourceIpAddress::~SourceIpAddress()
{
    NS_LOG_FUNCTION(this);
}

/**
 * Compares source ip address of packet against filter element
 * @param Packet
 * @return bool
 */
bool
SourceIpAddress::match(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this << packet);

    Ptr<Packet> packet_copy = packet->Copy();
    PppHeader pppHeader;
    packet_copy->RemoveHeader(pppHeader);

    Ipv4Header ipv4Header;
    packet_copy->PeekHeader(ipv4Header);

    Ipv4Address Ipv4Address = ipv4Header.GetSource();
    if (Ipv4Address == (this->value))
    {
        return true;
    }
    else
    {
        return false;
    }
}

} // namespace ns3