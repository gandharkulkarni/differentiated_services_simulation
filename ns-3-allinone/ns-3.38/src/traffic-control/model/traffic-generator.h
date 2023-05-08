#ifndef TRAFFICGENERATOR_H
#define TRAFFICGENERATOR_H

#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"

using namespace ns3;
/* 
    Ref: 
        https://www.nsnam.org/docs/release/3.18/doxygen/fifth_8cc_source.html 
        https://www.nsnam.org/docs/release/3.9/tutorial/tutorial_31.html
*/
class TrafficGenerator : public Application
{
    public:
        TrafficGenerator ();
        virtual ~TrafficGenerator ();
        static TypeId GetTypeId();
        void Setup(Ptr<Socket> socket,
                   Address address,
                   uint32_t packetSize,
                   uint32_t nPackets,
                   DataRate dataRate,
                   uint32_t portNumber);

    private:
        void StartApplication() override;
        void StopApplication() override;
        void ScheduleTx();
        void SendPacket();
        Ptr<Socket> m_socket;   //!< The transmission socket.
        Address m_peer;         //!< The destination address.
        uint32_t m_packetSize;  //!< The packet size.
        uint32_t m_nPackets;    //!< The number of packets to send.
        DataRate m_dataRate;    //!< The data rate to use.
        EventId m_sendEvent;    //!< Send event.
        bool m_running;         //!< True if the application is running.
        uint32_t m_packetsSent; //!< The number of packets sent.
        uint16_t m_portNumber;
}; 

#endif