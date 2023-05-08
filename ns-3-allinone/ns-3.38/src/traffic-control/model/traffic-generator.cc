#include "traffic-generator.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("TrafficGenerator");
/* 
    Ref: 
        https://www.nsnam.org/docs/release/3.18/doxygen/fifth_8cc_source.html 
        https://www.nsnam.org/docs/release/3.9/tutorial/tutorial_31.html
*/

TrafficGenerator::TrafficGenerator()
    : m_socket(nullptr),
      m_peer(),
      m_packetSize(0),
      m_nPackets(0),
      m_dataRate(0),
      m_sendEvent(),
      m_running(false),
      m_packetsSent(0),
      m_portNumber(0)
{
}

TrafficGenerator::~TrafficGenerator()
{
    m_socket = nullptr;
}

TypeId
TrafficGenerator::GetTypeId()
{
    static TypeId tid = TypeId("UdpApplication")
                            .SetParent<Application>()
                            .SetGroupName("Udp")
                            .AddConstructor<TrafficGenerator>();
    return tid;
}

void
TrafficGenerator::Setup(Ptr<Socket> socket,
                   Address address,
                   uint32_t packetSize,
                   uint32_t nPackets,
                   DataRate dataRate, 
                   uint32_t portNumber)
{               
    m_socket = socket;
    m_peer = address;
    m_packetSize = packetSize;
    m_nPackets = nPackets;
    m_dataRate = dataRate;
    m_portNumber = portNumber;                           
}

void
TrafficGenerator::StartApplication()
{
    m_running = true;
    m_packetsSent = 0;
    m_socket->Bind();
    m_socket->Connect(m_peer);
    SendPacket();
}

void
TrafficGenerator::StopApplication()
{
    m_running = false;

    if (m_sendEvent.IsRunning())
    {
        Simulator::Cancel(m_sendEvent);
    }

    if (m_socket)
    {
        m_socket->Close();
    }
}

void
TrafficGenerator::SendPacket()
{
    Ptr<Packet> packet = Create<Packet>(m_packetSize);
    UdpHeader header;
    header.SetSourcePort(m_portNumber);
    packet->AddHeader(header);
    m_socket->Send(packet);

    if (++m_packetsSent < m_nPackets)
    {
        ScheduleTx();
    }
}

void
TrafficGenerator::ScheduleTx()
{
    if (m_running)
    {
        Time tNext(Seconds(m_packetSize * 8 / static_cast<double>(m_dataRate.GetBitRate())));
        m_sendEvent = Simulator::Schedule(tNext, &TrafficGenerator::SendPacket, this);
    }
}