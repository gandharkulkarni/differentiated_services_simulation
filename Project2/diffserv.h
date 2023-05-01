#ifndef DIFF_SERV_H_
#define DIFF_SERV_H_

#include "queue.h"
#include <vector>
#include <queue>
#include "traffic-class.h"

using namespace std;

namespace ns3 {
class DiffServ : public Queue<Packet>
{
public:
  DiffServ ();
  DiffServ (uint32_t numQueue);
  virtual ~DiffServ ();

  //QUEUE_MODE_PACKETS = 0; QUEUE_MODE_BYTES = 1;
  typedef enum QueueMode {
    QUEUE_MODE_PACKETS,
    QUEUE_MODE_BYTES,
  } QueueMode;

  /**
   * setMode()/getMode() - the accessor and modifier for private variable m mode.
   */
  void SetMode (QueueMode mode);
  QueueMode GetMode ();

  // vector<TrafficClass*> q_class;
  vector<TrafficClass *> *GetQueuesPtr ();
  const vector<TrafficClass *> *GetQueuesPtrForPeek () const;

  /**
   * \return Returns a queue index whose queue has the next packet to be transmitted.
   */
  virtual uint32_t Schedule ();
  virtual uint32_t ScheduleForPeek () const;

  /**
   * \brief Takes a packet and returns an integer.
   * \return An integer
   */
  virtual uint32_t Classify (Ptr<Packet> p);

  bool Enqueue (Ptr<Packet> p);
  Ptr<Packet> Dequeue (void);
  Ptr<Packet> Remove (void);
  Ptr<const Packet> Peek (void) const;

private:
  /**
   * The QueueMode specifies whether service is in byte mode or packet mode.
   */
  QueueMode m_mode;

  /**
   * The vector (array) of TrafficClass pointers. 
   */
  vector<TrafficClass *> q_class;

  virtual bool DoEnqueue (Ptr<Packet> p);
  virtual Ptr<Packet> DoDequeue ();
  Ptr<Packet> DoRemove ();
  Ptr<const Packet> DoPeek (void) const;
};
} // namespace ns3

#endif