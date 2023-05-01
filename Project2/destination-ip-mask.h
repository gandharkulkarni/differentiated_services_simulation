#include "ns3/ipv4-header.h"
#include "ns3/ipv4-address.h"
#include "ns3/packet.h"
namespace ns3 {
class DestinationIpMask
{
private:
  ns3::Ipv4Address m_ipAddress;
  ns3::Ipv4Mask m_ipMask;

public:
  DestinationIpMask (ns3::Ipv4Address address, ns3::Ipv4Mask mask)
      : m_ipAddress (address), m_ipMask (mask)
  {
  }

  bool
  match (ns3::Ptr<ns3::Packet> packet)
  {
    ns3::Ipv4Header ipv4Header;
    packet->PeekHeader (ipv4Header);
    ns3::Ipv4Address packetDestIp = ipv4Header.GetDestination ();
    uint32_t ipAddressInt = m_ipAddress.Get ();
    uint32_t ipMaskInt = m_ipMask.Get ();
    uint32_t packetDestIpInt = packetDestIp.Get ();
    return (packetDestIpInt) == (ipAddressInt & ipMaskInt);
  }
};
} // namespace ns3