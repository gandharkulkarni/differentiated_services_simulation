#include "ns3/log.h"
#include "ns3/ipv4-header.h"
#include "ns3/point-to-point-net-device.h"
#include "destination-ip-address.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("DestinationIpAddress");

NS_OBJECT_ENSURE_REGISTERED (DestinationIpAddress);

TypeId
DestinationIpAddress::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DestinationIpAddress")
    .SetParent<FilterElement> ()
    .SetGroupName ("QoS");
  return tid;
}

DestinationIpAddress::DestinationIpAddress ()
{
  NS_LOG_FUNCTION (this);
}

DestinationIpAddress::DestinationIpAddress (Ipv4Address ipV4Address)
{
  NS_LOG_FUNCTION (this);
  this->value = ipV4Address;
}

DestinationIpAddress::~DestinationIpAddress ()
{
  NS_LOG_FUNCTION (this);
}

bool
DestinationIpAddress::match (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this << packet);
  
  Ptr<Packet> packet_copy = packet->Copy();
  PppHeader pppHeader;
  packet_copy->RemoveHeader(pppHeader);

  Ipv4Header ipv4Header;
  packet_copy->PeekHeader (ipv4Header);

  Ipv4Address Ipv4Address = ipv4Header.GetDestination();

  if (Ipv4Address == (value))
    {
      return true;
    }
  else
    {
      return false;
    }
}

} // namespace ns3