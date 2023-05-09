#ifndef TRAFFIC_CLASS_H
#define TRAFFIC_CLASS_H

#include "filter.h"

#include "ns3/log.h"
#include "ns3/point-to-point-net-device.h"
#include "ns3/queue.h"

#include <queue>
#include <vector>

namespace ns3
{

class TrafficClass : public Object
{
  public:
    static TypeId GetTypeId(void);

    TrafficClass();
    TrafficClass(uint32_t maxPackets,
                 uint32_t maxBytes,
                 double_t weight,
                 uint32_t priority_level,
                 bool isDefault,
                 std::vector<Filter*> filters);
    TrafficClass(uint32_t maxPackets,
                 uint32_t quantum_size,
                 uint32_t deficit_counter,
                 bool isDefault,
                 std::vector<Filter*> filters);
    ~TrafficClass();

    // project spec
    std::vector<Filter*> filters;
    bool Enqueue(Ptr<ns3::Packet> packet);
    Ptr<ns3::Packet> Dequeue();
    bool match(Ptr<ns3::Packet> packet);

    // extra feature
    bool IsEmpty();
    Ptr<ns3::Packet> Remove();
    Ptr<ns3::Packet> Peek();
    uint32_t priority_level;

    bool isDefault; // only one traffic class should have isDefault value=1
    // std::queue<Ptr<Packet>> m_queue;
    void SetQuantumSize(uint32_t q);
    uint32_t GetQuantumSize();
    void SetDeficitCounter(uint32_t d);
    uint32_t GetDeficitCounter();
    uint32_t GetMaxPacket();
    uint32_t GetPacketCount();

  private:
    uint32_t bytes;
    uint32_t packets;    // related SPQ
    uint32_t maxPackets; // related SPQ
    uint32_t maxBytes;   // related DDR
    double_t weight;     // related DDR
    uint32_t prioritylevel;
    std::queue<Ptr<Packet>> m_queue;
    uint32_t quantum_size;
    uint32_t deficit_counter;
};

} // namespace ns3

#endif /* TRAFFICCLASS_H */