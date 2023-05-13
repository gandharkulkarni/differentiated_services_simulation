#include "ns3/log.h"
#include "ns3/enum.h"
#include "ns3/uinteger.h"
#include "diff-serv.h"
#include "traffic-class.h"
#include <vector>
#include "stdio.h"
#include <iostream>
#include <ns3/object-base.h>
namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("DiffServ");
NS_OBJECT_TEMPLATE_CLASS_DEFINE (DiffServ, Packet);

template <typename Packet>
TypeId
DiffServ<Packet>::GetTypeId (void)
{
  static TypeId tid =
      TypeId ((GetTemplateClassName<DiffServ<Packet>> ()))
          .SetParent<Queue<Packet>> ()
          .SetGroupName ("QoS")
          .template AddConstructor<DiffServ<Packet>> ();
  return tid;
}

template <typename Packet>
DiffServ<Packet>::DiffServ () : Queue<Packet> (), NS_LOG_TEMPLATE_DEFINE ("DiffServ")
{
  NS_LOG_FUNCTION (this);
}

template <typename Packet>
DiffServ<Packet>::~DiffServ ()
{
  NS_LOG_FUNCTION (this);
}

/**
 * Sets mode of Queue
 * @param mode
*/
template <typename Packet>
void
DiffServ<Packet>::SetMode (QueueMode mode)
{
  NS_LOG_FUNCTION (this << mode);
  this->m_mode = mode;
}

/**
 * Get mode of Queue
 * @return QueueMode
*/
template <typename Packet>
QueueMode
DiffServ<Packet>::GetMode ()
{
  NS_LOG_FUNCTION (this);
  return m_mode;
}

/**
 * Enqueues packet into queue
 * @param Packet 
*/
template <typename Packet>
bool
DiffServ<Packet>::DoEnqueue (Ptr<ns3::Packet> p)
{
  NS_LOG_FUNCTION (this << p);

  auto iter = q_class.begin ();
  for (; iter != q_class.end (); iter++)
    {
      // std::cout<<*iter<<" ";
      if ((**iter).match (p))
        {
          return (**iter).Enqueue (p);
        }
      else
        {
        }
    }
  return false;
}


/**
 * Dequeues the packet 
 * @return Packet
 **/
template <typename Packet>
Ptr<ns3::Packet>
DiffServ<Packet>::DoDequeue (void)
{
  NS_LOG_FUNCTION (this);
  for (TrafficClass *tc : q_class)
    {
      if (tc->Peek () != 0)
        {
          return tc-> Dequeue ();
        }
    }
  return 0;
}


/**
 * Peeks the packet in front of the queue
 * @return Packet
 **/
template <typename Packet>
Ptr<const ns3::Packet>
DiffServ<Packet>::DoPeek () const
{
  NS_LOG_FUNCTION (this);
  auto iter = q_class.begin ();
  for (; iter != q_class.end (); iter++)
    {      
      if ((**iter).Peek () != 0)
        { //NULL check!
          return (**iter).Peek ();
        }
    }
  return 0;
}

/**
 * Removes the packet from the queue.
 * @return Packet
 **/
template <typename Packet>
Ptr<ns3::Packet>
DiffServ<Packet>::DoRemove (void)
{
  NS_LOG_FUNCTION (this);

  for (TrafficClass *tc : q_class)
    {
      if (tc->Peek () != 0)
        {
          return tc->Remove ();
        }
    }
  return 0;
}

/**
 * Classifies packet into different queues.
 * @param Packet
 **/
template <typename Packet>
uint32_t
DiffServ<Packet>::Classify (Ptr<ns3::Packet> p)
{
  NS_LOG_FUNCTION (this << p);

  uint32_t match_index = -1;

  return match_index;
}

/**
 * To schedule the packet and return it.
 * @return Packet
 **/
template <typename Packet>
Ptr<ns3::Packet>
DiffServ<Packet>::Schedule ()
{
  NS_LOG_FUNCTION (this);  
  return  0; 
}



/***
 * Enqueues the packet
 * @param Packet
 * @return bool
 * */
template <typename Packet>
bool
DiffServ<Packet>::Enqueue (Ptr<Packet> item)
{
  NS_LOG_FUNCTION (this << item);

  return DoEnqueue (item);
}

/***
 * Dequeues the packet
 * @param Packet
 * @return Packet
 * */
template <typename Packet>
Ptr<Packet>
DiffServ<Packet>::Dequeue (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<Packet> item = DoDequeue ();

  NS_LOG_LOGIC ("Popped " << item);

  return item;
}

/***
 * Removes the packet
 * @return Packet
 * */
template <typename Packet>
Ptr<Packet>
DiffServ<Packet>::Remove (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<Packet> item = DoRemove ();

  NS_LOG_LOGIC ("Removed " << item);

  return item;
}


/***
 * Peeks the packet in front of Queue
 * @return Packet
 * */
template <typename Packet>
Ptr<const Packet>
DiffServ<Packet>::Peek (void) const
{
  NS_LOG_FUNCTION (this);

  return DoPeek ();
}

} // namespace ns3