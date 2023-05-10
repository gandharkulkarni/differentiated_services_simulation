#include "ns3/log.h"
#include "spq.h"

namespace ns3 {
NS_LOG_COMPONENT_DEFINE ("SPQ");
NS_OBJECT_TEMPLATE_CLASS_DEFINE (SPQ, Packet);

template <typename Packet>
TypeId
SPQ<Packet>::GetTypeId (void)
{
  static TypeId tid =
      TypeId ((GetTemplateClassName<SPQ<Packet>> () ))
          .SetParent<DiffServ<Packet>> ()
          .SetGroupName ("trafficControl")
          .template AddConstructor<SPQ<Packet>> () 
      ;
  return tid;
}

template <typename Packet>
SPQ<Packet>::SPQ () : DiffServ<Packet> (), NS_LOG_TEMPLATE_DEFINE ("SPQ")
{
  NS_LOG_FUNCTION (this);
}

template <typename Packet>
SPQ<Packet>::SPQ (QueueMode mode, std::vector<TrafficClass *> trafficClassList)
    : NS_LOG_TEMPLATE_DEFINE ("SPQ")
{
  this->m_mode = mode;
  this->q_class = trafficClassList;
  std::cout << "SPQ.q_class.size: " << q_class.size () << std::endl;
  NS_LOG_FUNCTION (this);
}

template <typename Packet>
SPQ<Packet>::~SPQ ()
{
  NS_LOG_FUNCTION (this);
}

/***
 * To Enqueue the packet
 * */
template <typename Packet>
bool
SPQ<Packet>::Enqueue (Ptr<Packet> p)
{
  NS_LOG_FUNCTION (this << p);

  printf ("Test.SPQ.Enqueue\n");
  u_int32_t index = Classify (p);

  q_class[index]->Enqueue (p);

  DiffServ<Packet>::Enqueue(p);

  return true;
}

/***
 * To Dequeue the packet
 * */
template <typename Packet>
Ptr<Packet>
SPQ<Packet>::Dequeue (void)
{
  NS_LOG_FUNCTION (this);
  printf ("Test.SPQ.Dequeue\n");
  Ptr<Packet> p;
  p = Schedule ();
  std::cout << "Schedule p in final Dequeue" << &p << std::endl;
  DiffServ<Packet>::Dequeue();
  return p;
}

/***
 * To Remove the packet
 * */
template <typename Packet>
Ptr<Packet>
SPQ<Packet>::Remove (void)
{
  NS_LOG_FUNCTION (this);
  Ptr<Packet> item = DiffServ<Packet>::Remove ();
  NS_LOG_LOGIC ("Removed " << item);
  return item;
}

/***
 * To Peek the packet
 * */
template <typename Packet>
Ptr<const Packet>
SPQ<Packet>::Peek (void) const
{
  NS_LOG_FUNCTION (this);

  return DiffServ<Packet>::Peek ();
}

/**
 * The classify function utilizes filter aspect to sort the traffic
 *  packets into appropriate trafficqueues
 * 
 * */
template <typename Packet>
uint32_t
SPQ<Packet>::Classify (Ptr<Packet> p)
{
  NS_LOG_FUNCTION (this << p);

  uint32_t index = -1;

  for (uint32_t i = 0; i < q_class.size (); i++)
    {
      if ((q_class[i]->match (p)))
        {
          return index = i;
        }
      else
        {
          if (q_class[i]->GetDefault())
            { 
              index = i; 
            }
        }
    }

  

  return index;
}

/** 
 * The schedule function carries out designed Quality-of-Service (QoS) algorithm to schedule 
 * whichtraffic queue to be served at the time.
 * */
template <typename Packet>
Ptr<Packet>
SPQ<Packet>::Schedule ()
{
  NS_LOG_FUNCTION (this);
  Ptr<Packet> p;
  for (uint32_t priority = 0; priority < 100; priority++)
    {
      for (uint32_t i = 0; i < q_class.size (); i++)
        {
          if (q_class[i]->GetPriorityLevel() == priority &&
              q_class[i]->IsEmpty () != true)
            {
              p = q_class[i]->Dequeue ();
              return p;
            }
        }
    }
    return 0;
}

} // namespace ns3