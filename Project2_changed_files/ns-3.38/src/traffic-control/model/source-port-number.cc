
#include "source-port-number.h"
#include "ns3/packet.h"
#include "ns3/log.h"
#include "ns3/ipv4-header.h"
#include "ns3/ipv4-address.h"
#include "ns3/udp-header.h"

namespace ns3{

    NS_LOG_COMPONENT_DEFINE ("SourcePortNumber");
    NS_OBJECT_ENSURE_REGISTERED (SourcePortNumber);

    TypeId
    SourcePortNumber::GetTypeId (void)
    {
        static TypeId tid = TypeId ("ns3::SourcePortNumber")
                .SetParent<FilterElement> ()
                .SetGroupName ("QoS")
        ;
        return tid;
    }

    SourcePortNumber::SourcePortNumber (uint32_t val)
    {
        this->value = val;
    }


    SourcePortNumber::~SourcePortNumber()
    {
        NS_LOG_FUNCTION (this);
    }

    /**
     * Compares source port number of packet with filter element
     * @param Packet
     * @return bool
    */
    bool 
    SourcePortNumber::match (Ptr<Packet> p)
    {
       NS_LOG_FUNCTION (this);
	   Ptr<Packet> copyPacket = p -> Copy();
       PppHeader pppHeader;
	   Ipv4Header ipv4Header;
	   UdpHeader udpHeader;
       copyPacket -> RemoveHeader(pppHeader);
	   copyPacket -> RemoveHeader(ipv4Header);
	   copyPacket -> RemoveHeader(udpHeader);
	   uint32_t sourcePort = udpHeader.GetSourcePort();       
       
	   return sourcePort == value;
    }

}
