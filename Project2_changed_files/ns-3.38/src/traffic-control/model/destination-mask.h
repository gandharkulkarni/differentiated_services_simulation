#ifndef DESTINATION_MASK_H
#define DESTINATION_MASK_H

#include "filter-element.h"

#include "ns3/ipv4-address.h"
#include "ns3/log.h"

namespace ns3
{

/**
 * Class to match Destination mask of packet
 * @author Gandhar Kulkarni
 */
class DestinationMask : public FilterElement
{
  public:
    static TypeId GetTypeId(void);

    DestinationMask();
    DestinationMask(Ipv4Address destination_ip_address, Ipv4Mask ipv4Mask);
    ~DestinationMask();

    bool match(Ptr<Packet> packet) override;

  private:
    Ipv4Address destination_ip_address;
    Ipv4Mask value;
};

} // namespace ns3

#endif /* DESTINATION_MASK_H */