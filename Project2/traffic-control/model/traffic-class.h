#ifndef TRAFFIC_CLASS_H
#define TRAFFIC_CLASS_H

#include "queue.h"
#include <vector>
#include <queue>
#include "filter.h"

using namespace std;
namespace ns3 {
class TrafficClass
{
public:
  TrafficClass ();
  TrafficClass (const TrafficClass &tc);
  virtual ~TrafficClass ();

  /**
   * A collection of conditions as Filters, any of which should match in order to trigger match.
   */
  vector<Filter *> filters;

  bool Enqueue (Ptr<Packet> p);
  Ptr<Packet> Dequeue ();
  Ptr<Packet> Peek ();

  /**
   * \return Return true if at least one Filter is true, otherwise false. 
   */
  bool match (Ptr<Packet> p);

  void SetPacketsCount (uint32_t packetsCount);
  uint32_t GetPacketsCount ();
  void SetWeight (double weight);
  double GetWeight ();
  void SetPriorityLevel (uint32_t plevel);
  uint32_t GetPriorityLevel ();
  void SetIsDefault (bool b);
  bool GetIsDefault ();

private:
  /**
   * the count of bytes in the queue.
   */
  uint32_t bytes;
  /**
   * the count of packets in the queue.
   */
  uint32_t packets;
  /**
   * the maximum number of packets allowed in the queue.
   */
  uint32_t maxPackets;
  /**
   * the maximum number of bytes allowed in the queue. 
   */
  uint32_t maxBytes;
  /**
   * applicable if the QoS mechanism uses weight.
   */
  double weight;
  /**
   * applicable if the QoS mechanism uses priority level.
   */
  uint32_t priority_level;
  /**
   * true if the queue is default, where all the packets that does not match the conditions go. 
   */
  bool isDefault;
  /**
   * Actual queue that packets are queued in. 
   */
  queue<Ptr<Packet>> m_queue;
};
} // namespace ns3

#endif