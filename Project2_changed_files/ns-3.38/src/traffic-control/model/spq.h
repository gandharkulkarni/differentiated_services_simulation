#ifndef SPQ_H
#define SPQ_H

#include "diff-serv.h"
#include "ns3/queue.h"
#include "ns3/log.h"

namespace ns3 {

template <typename Packet>
class SPQ : public DiffServ<Packet> 
{

public:
  SPQ ();

  SPQ (QueueMode mode, std::vector<TrafficClass *> trafficClassVector);

  static TypeId GetTypeId (void);

  virtual ~SPQ ();

  bool Enqueue (Ptr<Packet> item) override;

  Ptr<Packet> Dequeue () override;

  Ptr<Packet> Remove () override;

  Ptr<const Packet> Peek (void) const override;

  uint32_t Classify (Ptr<Packet> item);

  Ptr<Packet> Schedule ();

  QueueMode m_mode; 
  std::vector<TrafficClass*> q_class; 

private:
  uint32_t active_queue_index;
  NS_LOG_TEMPLATE_DECLARE;
};


extern template class SPQ<Packet>;
} // namespace ns3

#endif /* SPQ_H */