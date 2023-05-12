#include "drr.h"

#include "ns3/log.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("DRR");
NS_OBJECT_TEMPLATE_CLASS_DEFINE(DRR, Packet);

template <typename Packet>
TypeId
DRR<Packet>::GetTypeId(void)
{
    static TypeId tid = TypeId(GetTemplateClassName<DRR<Packet>>())
                            .SetParent<DiffServ<Packet>>()
                            .SetGroupName("QoS")
                            .template AddConstructor<DRR<Packet>>();
    return tid;
}

template <typename Packet>
DRR<Packet>::DRR()
    : DiffServ<Packet>(),
      NS_LOG_TEMPLATE_DEFINE("DRR")
{
    NS_LOG_FUNCTION(this);
}

template <typename Packet>
DRR<Packet>::DRR(QueueMode mode, std::vector<TrafficClass*> trafficClassList)
    : NS_LOG_TEMPLATE_DEFINE("DRR")
{
    this->m_mode = mode;
    this->q_class = trafficClassList;
    NS_LOG_FUNCTION(this);
}

template <typename Packet>
DRR<Packet>::~DRR()
{
    NS_LOG_FUNCTION(this);
}

template <typename Packet>
std::queue<Ptr<Packet>>*
DRR<Packet>::GetServiceQueue()
{
    return &service_queue;
}

template <typename Packet>
bool
DRR<Packet>::Enqueue(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this << packet);
    return DoEnqueue(packet);
}

template <typename Packet>
Ptr<Packet>
DRR<Packet>::Dequeue()
{
    NS_LOG_FUNCTION(this);
    Ptr<Packet> packet = DoDequeue();
    NS_LOG_LOGIC("Popped " << packet);

    return packet;
}

template <typename Packet>
Ptr<Packet>
DRR<Packet>::Remove()
{
    NS_LOG_FUNCTION(this);
    Ptr<Packet> packet = DoRemove();

    NS_LOG_LOGIC("Removed " << packet);

    return packet;
}

template <typename Packet>
Ptr<const Packet>
DRR<Packet>::Peek() const
{
    NS_LOG_FUNCTION(this);
    return DoPeek();
}

template <typename Packet>
bool
DRR<Packet>::DoEnqueue(Ptr<Packet> packet)
{
    uint32_t index = Classify(packet);
    if (index >= 0 && index < q_class.size())
    {
        if (std::find(active_list.begin(), active_list.end(), index) != active_list.end())
        {
            active_list.push_back(index);
            q_class[index]->SetDeficitCounter(0);
        }
        if (q_class[index]->GetPacketCount() < q_class[index]->GetMaxPacket())
        {
            q_class[index]->Enqueue(packet);

            return true;
        }
    }
    return false;
}

template <typename Packet>
Ptr<Packet>
DRR<Packet>::DoDequeue()
{
    std::queue<Ptr<Packet>>* temp_service_queue = Schedule();
    if (!temp_service_queue->empty())
    {
        Ptr<Packet> packet = temp_service_queue->front();
        temp_service_queue->pop();
        return packet;
    }
    return nullptr;
}

template <typename Packet>
Ptr<Packet>
DRR<Packet>::DoRemove()
{
    std::queue<Ptr<Packet>>* temp_service_queue = Schedule();
    if (!temp_service_queue->empty())
    {
        Ptr<Packet> packet = temp_service_queue->front();
        temp_service_queue->pop();
        return packet;
    }
    return nullptr;
}

template <typename Packet>
Ptr<const Packet>
DRR<Packet>::DoPeek() const
{
    return q_class[active_queue_index]->Peek();
}

template <typename Packet>
uint32_t
DRR<Packet>::Classify(Ptr<Packet> p)
{
    NS_LOG_FUNCTION(this << p);
    uint32_t index = -1;
    for (uint32_t i = 0; i < q_class.size(); i++)
    {
        if (q_class[i]->match(p))
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

template <typename Packet>
std::queue<Ptr<Packet>>*
DRR<Packet>::Schedule()
{
    if (GetServiceQueue()->empty())
    {
        uint32_t attempt = 0;
        while (GetServiceQueue()->empty() && attempt < 2)
        {
            for (uint32_t i = 0; i < q_class.size(); i++)
            {
                // Update dificit counter by adding quantum
                q_class[i]->SetDeficitCounter(q_class[i]->GetDeficitCounter() +
                                              q_class[i]->GetQuantumSize());
                
                while (q_class[i]->GetPacketCount() != 0)
                {
                    Ptr<Packet> packet = q_class[i]->Peek();
                    uint32_t sizeOfPacket = packet->GetSize();
                    if (q_class[i]->GetDeficitCounter() >= sizeOfPacket)
                    {
                        q_class[i]->SetDeficitCounter(q_class[i]->GetDeficitCounter() - sizeOfPacket);
                        active_queue_index = i;
                        q_class[i]->Dequeue();
                        GetServiceQueue()->push(packet);
                    }
                    else
                    {
                        break;
                    }
                }
                if (q_class[i]->IsEmpty())
                {
                    q_class[i]->SetDeficitCounter(0);
                }
            }
            attempt++;
        }
    }
    return GetServiceQueue();
}
} // namespace ns3