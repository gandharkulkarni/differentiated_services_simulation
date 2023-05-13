#include "destination-mask.h"

#include "ns3/ipv4-header.h"
#include "ns3/log.h"
#include "ns3/point-to-point-net-device.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("DestinationMask");

NS_OBJECT_ENSURE_REGISTERED(DestinationMask);

TypeId
DestinationMask::GetTypeId(void)
{
    static TypeId tid =
        TypeId("ns3::DestinationMask").SetParent<FilterElement>().SetGroupName("QoS");
    return tid;
}

DestinationMask::DestinationMask()
{
    NS_LOG_FUNCTION(this);
}

DestinationMask::~DestinationMask()
{
    NS_LOG_FUNCTION(this);
}

DestinationMask::DestinationMask(Ipv4Address destination_ip_address, Ipv4Mask ipv4Mask)
{
    NS_LOG_FUNCTION(this);
    this->destination_ip_address = destination_ip_address;
    this->value = ipv4Mask;
}


/**
 * Compares destination mask of packet with filter element
 * @return bool
*/
bool
DestinationMask::match(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this << packet);

    Ptr<Packet> packet_copy = packet->Copy();
    PppHeader pppHeader;
    packet_copy->RemoveHeader(pppHeader);

    Ipv4Header ipv4Header;
    packet_copy->PeekHeader(ipv4Header);
    Ipv4Address ipv4Address = ipv4Header.GetDestination();

    if (this->value.IsMatch(ipv4Address, this->destination_ip_address))
    {
        return true;
    }
    else
    {
        return false;
    }
}

} // namespace ns3