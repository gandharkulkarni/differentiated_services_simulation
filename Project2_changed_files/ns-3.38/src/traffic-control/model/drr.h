#ifndef DRR_H
#define DRR_H

#include "diff-serv.h"
#include "ns3/log.h"
#include "ns3/queue.h"

namespace ns3
{

template <typename Packet>
class DRR : public DiffServ<Packet>
{
  public:
    DRR();
    ~DRR();
    DRR(QueueMode mode, std::vector<TrafficClass*> trafficClassVector);
    static TypeId GetTypeId(void);
    Ptr<Packet> Dequeue();
    bool Enqueue(Ptr<Packet> packet);
    Ptr<Packet> Remove();
    Ptr<const Packet> Peek(void) const;
    uint32_t Classify(Ptr<Packet> p);
    std::queue<Ptr<Packet>>* Schedule();
    std::queue<Ptr<Packet>>* GetServiceQueue();

  protected:
    QueueMode m_mode; 
    std::vector<TrafficClass*> q_class;
    bool DoEnqueue(Ptr<Packet> packet);
    Ptr<Packet> DoDequeue();
    Ptr<Packet> DoRemove();
    Ptr<const Packet> DoPeek() const;

  private:
    using DiffServ<Packet>::Enqueue;
    using DiffServ<Packet>::Dequeue;
    using DiffServ<Packet>::Remove;
    using DiffServ<Packet>::Peek;

    using DiffServ<Packet>::Schedule;
    using DiffServ<Packet>::Classify;

    std::queue<Ptr<Packet>> service_queue;

    NS_LOG_TEMPLATE_DECLARE;
};
extern template class DRR<Packet>;
} // namespace ns3

#endif