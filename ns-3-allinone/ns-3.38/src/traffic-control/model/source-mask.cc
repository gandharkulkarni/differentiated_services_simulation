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
    static TypeId tid =
        TypeId("ns3::SourceMask").SetParent<FilterElement>().SetGroupName("trafficControl");
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

SourceMask::SourceMask(Ipv4Mask ipv4Mask)
{
    NS_LOG_FUNCTION(this);
    this->value = ipv4Mask;
}

bool
SourceMask::match(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this << packet);
    Ipv4Header ipv4Header;
    packet->PeekHeader(ipv4Header);
    uint32_t prefix_length = value.GetPrefixLength();
    Ipv4Mask mask = Ipv4Mask((Ipv4Mask::GetOnes().Get() << (32 - prefix_length)));

    if ((ipv4Header.GetDestination().Get() & mask.Get()) == (value.Get() & mask.Get()))
    {
        return true;
    }
    else
    {
        return false;
    }
    /*
    Ipv4Address ipv4Address =ipv4Header.GetSource();

  if (ipv4Address == (ipv4Address.CombineMask(this->value))){
      std::cout<<"Mask:"<< value << " Matched with Ipv4Address:" << ipv4Address <<std::endl;
      return true;
    }else{
      std::cout<<"Mask:"<< value << " Not Matched Ipv4Address:" << ipv4Address <<std::endl;
      return false;
    }
    */
}

} // namespace ns3