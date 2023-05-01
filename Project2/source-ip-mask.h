#include "ns3/ipv4-header.h"
#include "ns3/ipv4-address.h"
#include "ns3/packet.h"
namespace ns3 {
class SourceIpMask
{
private:
  ns3::Ipv4Address m_srcIpAddressValue;
  ns3::Ipv4Mask m_srcIpMaskValue;

public:
  SourceIpMask (ns3::Ipv4Address address, ns3::Ipv4Mask mask)
      : m_srcIpAddressValue (address), m_srcIpMaskValue (mask)
  {
  }
  virtual ~SourceIpMask ()
  {

  }
  bool
  match (ns3::Ptr<ns3::Packet> packet)
  {
    ns3::Ipv4Header ipv4Header;
    packet->PeekHeader (ipv4Header);
    ns3::Ipv4Address packetSourceIp = ipv4Header.GetSource ();
    uint32_t ipAddressInt = m_srcIpAddressValue.Get ();
    uint32_t ipMaskInt = m_srcIpMaskValue.Get ();
    uint32_t packetSourceIpInt = packetSourceIp.Get ();
    return (packetSourceIpInt) == (ipAddressInt & ipMaskInt);
  }
};
} // namespace ns3