#include "protocol-number.h"

#include "ns3/log.h"

namespace ns3
{

    NS_LOG_COMPONENT_DEFINE("ProtocolNumber");

    NS_OBJECT_ENSURE_REGISTERED(ProtocolNumber);

    TypeId
    ProtocolNumber::GetTypeId(void)
    {
        static TypeId tid =
            TypeId("ns3::ProtocolNumber").SetParent<Object>().SetGroupName("QoS");
        return tid;
    }

    ProtocolNumber::ProtocolNumber()
    {
        NS_LOG_FUNCTION(this);
    }

    ProtocolNumber::~ProtocolNumber()
    {
        NS_LOG_FUNCTION(this);
    }

    ProtocolNumber::ProtocolNumber(uint32_t val)
    {
        value = val;
    }

    bool
    ProtocolNumber::match(Ptr<Packet> packet)
    {
        NS_LOG_FUNCTION(this << packet);

        Ptr<Packet> packet_copy = packet->Copy();
        PppHeader pppHeader;
        packet_copy->RemoveHeader(pppHeader);

        Ipv4Header ipv4Header;
        packet_copy->PeekHeader(ipv4Header);

        uint8_t protocolNumber = ipv4Header.GetProtocol();

        if (protocolNumber == value)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

} // namespace ns3