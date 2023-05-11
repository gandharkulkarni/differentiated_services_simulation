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
                                .SetGroupName("trafficControl")
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
    DRR<Packet>::DRR(QueueMode mode, std::vector<TrafficClass *> trafficClassList)
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
    std::queue<Ptr<Packet>> *
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
        return DiffServ<Packet>::Peek();
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
        std::queue<Ptr<Packet>> *temp_service_queue = Schedule();
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
        std::queue<Ptr<Packet>> *temp_service_queue = Schedule();
        if (!temp_service_queue->empty())
        {
            Ptr<Packet> packet = temp_service_queue->front();
            temp_service_queue->pop();
            return packet;
        }
        return nullptr;
    }

    // template <typename Packet>
    // Ptr<const Packet>
    // DRR<Packet>::DoPeek() const
    // {
    //     if (GetServiceQueue()->empty())
    //     {
    //         for (uint32_t i = 0; i < q_class.size(); i++) // todo: read '3' from config file
    //         {
    //             if (q_class[i]->GetPacketCount() != 0)
    //             {
    //                 q_class[i]->SetDeficitCounter(q_class[i]->GetDeficitCounter() +
    //                                               q_class[i]->GetQuantumSize());
    //                 Ptr<Packet> packet = q_class[i]->Peek();
    //                 uint32_t sizeOfPacket = packet->GetSize();
    //                 if (q_class[i]->GetDeficitCounter() >= sizeOfPacket)
    //                 {
    //                     q_class[i]->SetDeficitCounter(q_class[i]->GetDeficitCounter() -
    //                     sizeOfPacket); q_class[i]->Dequeue(); GetServiceQueue()->push(packet);
    //                 }
    //             }
    //         }
    //     }
    //     return GetServiceQueue()->front();
    // }

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
    std::queue<Ptr<Packet>> *
    DRR<Packet>::Schedule()
    {
        // while(true){
        //     if(!active_list->empty()){
        //         uint32_t i = active_list.at(0);
        //         active_list.erase(0);
        //         q_class[i]->SetDeficitCounter(q_class[i]->GetDeficitCounter() + q_class[i]->GetQuantumSize());
        //         while(q_class[i]->GetDeficitCounter()>0 && q_class[i]->GetPacketCount!=0){
        //             std::cout<<"Packet available in queue "<<i<<std::endl;
        //             Ptr<Packet> packet = q_class[i]->Peek();
        //             uint32_t sizeOfPacket = packet->GetSize();
        //             std::cout<<"Packet size "<<sizeOfPacket<<" =< "<<" Deficit counter "<< q_class[i]->GetDeficitCounter() <<" ? :: "<<(q_class[i]->GetDeficitCounter() >= sizeOfPacket)<<std::endl;
        //             if (q_class[i]->GetDeficitCounter() >= sizeOfPacket)
        //             {
        //                 std::cout<<"Packet can be serviced "<<std::endl;
        //                 q_class[i]->SetDeficitCounter(q_class[i]->GetDeficitCounter() - sizeOfPacket);
        //                 q_class[i]->Dequeue();
        //                 GetServiceQueue()->push(packet);
        //             }
        //             else
        //             {
        //                 break;
        //             }
        //         }
        //         if(q_class[i]->IsEmpty()){
        //             q_class[i]->SetDeficitCounter(0);
        //         }
        //         else{
        //             active_list.push_back(i);
        //         }

        //     }
        // }
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

                    std::cout << "Schedule called. Getting packet from queue " << i << std::endl;
                    while (q_class[i]->GetPacketCount() != 0)
                    // if (q_class[i]->GetPacketCount() != 0)
                    {
                        std::cout << "Packet available in queue " << i << std::endl;
                        Ptr<Packet> packet = q_class[i]->Peek();
                        uint32_t sizeOfPacket = packet->GetSize();
                        std::cout << "Packet size " << sizeOfPacket << " =< "
                                  << " Deficit counter " << q_class[i]->GetDeficitCounter() << " ? :: " << (q_class[i]->GetDeficitCounter() >= sizeOfPacket) << std::endl;
                        if (q_class[i]->GetDeficitCounter() >= sizeOfPacket)
                        {
                            std::cout << "Packet can be serviced " << std::endl;
                            q_class[i]->SetDeficitCounter(q_class[i]->GetDeficitCounter() - sizeOfPacket);
                            q_class[i]->Dequeue();
                            GetServiceQueue()->push(packet);
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                attempt++;
            }
        }
        return GetServiceQueue();
    }
} // namespace ns3