#include "spq.h"

#include "ns3/log.h"

namespace ns3
{
NS_LOG_COMPONENT_DEFINE("SPQ");
NS_OBJECT_TEMPLATE_CLASS_DEFINE(SPQ, Packet);

template <typename Packet>
TypeId
SPQ<Packet>::GetTypeId(void)
{
    static TypeId tid = TypeId((GetTemplateClassName<SPQ<Packet>>()))
                            .SetParent<DiffServ<Packet>>()
                            .SetGroupName("QoS")
                            .template AddConstructor<SPQ<Packet>>();
    return tid;
}

template <typename Packet>
SPQ<Packet>::SPQ()
    : DiffServ<Packet>(),
      NS_LOG_TEMPLATE_DEFINE("SPQ")
{
    NS_LOG_FUNCTION(this);
}

template <typename Packet>
SPQ<Packet>::SPQ(QueueMode mode, std::vector<TrafficClass*> trafficClassList)
    : NS_LOG_TEMPLATE_DEFINE("SPQ")
{
    this->m_mode = mode;
    this->q_class = trafficClassList;
    // std::cout << "SPQ.q_class.size: " << q_class.size () << std::endl;
    NS_LOG_FUNCTION(this);
}

template <typename Packet>
SPQ<Packet>::~SPQ()
{
    NS_LOG_FUNCTION(this);
}

/***
 * Enqueues the packet from queue
 * @return Packet
 * */
template <typename Packet>
bool
SPQ<Packet>::Enqueue(Ptr<Packet> p)
{
    NS_LOG_FUNCTION(this << p);

    u_int32_t index = Classify(p);

    q_class[index]->Enqueue(p);

    return true;
}

/***
 * Dequeues the packet from queue
 * @return Packet
 * */
template <typename Packet>
Ptr<Packet>
SPQ<Packet>::Dequeue(void)
{
    NS_LOG_FUNCTION(this);
    Ptr<Packet> p;
    p = Schedule();
    return p;
}

/***
 * Removes the packet from queue
 * @return Packet
 * */
template <typename Packet>
Ptr<Packet>
SPQ<Packet>::Remove(void)
{
    NS_LOG_FUNCTION(this);
    Ptr<Packet> item = DiffServ<Packet>::Remove();
    NS_LOG_LOGIC("Removed " << item);
    return item;
}

/***
 * Peek the packet from the front of the queue
 * @return Packet
 * */
template <typename Packet>
Ptr<const Packet>
SPQ<Packet>::Peek(void) const
{
    NS_LOG_FUNCTION(this);

    return q_class[active_queue_index]->Peek();
}

/**
 * Classifies packets into different queues based on filter parameters 
 * @param Packet
 * @return uint32_t index
 * */
template <typename Packet>
uint32_t
SPQ<Packet>::Classify(Ptr<Packet> p)
{
    NS_LOG_FUNCTION(this << p);

    uint32_t index = -1;

    for (uint32_t i = 0; i < q_class.size(); i++)
    {
        if ((q_class[i]->match(p)))
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
 * Decides which queue to serve
 * @return Packet
 * */
template <typename Packet>
Ptr<Packet>
SPQ<Packet>::Schedule()
{
    NS_LOG_FUNCTION(this);
    Ptr<Packet> p;
    for (uint32_t priority = 0; priority < 100; priority++)
    {
        for (uint32_t i = 0; i < q_class.size(); i++)
        {
            if (q_class[i]->GetPriorityLevel() == priority && q_class[i]->IsEmpty() != true)
            {
                active_queue_index = i;
                p = q_class[i]->Dequeue();
                return p;
            }
        }
    }
    return 0;
}

} // namespace ns3