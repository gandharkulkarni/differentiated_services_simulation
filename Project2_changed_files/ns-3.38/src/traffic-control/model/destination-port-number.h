#ifndef DESTINATION_PORT_NUMBER_H
#define DESTINATION_PORT_NUMBER_H

#include "filter-element.h"

#include "ns3/log.h"

namespace ns3
{

/**
 * Class to match destionation port number of packet
 * @author Gandhar Kulkarni
 */
class DestinationPortNumber : public FilterElement
{
  public:
    static TypeId GetTypeId(void);

    DestinationPortNumber(uint32_t);
    ~DestinationPortNumber();

    bool match(Ptr<ns3::Packet> packet) override;

  private:
    uint32_t value;
};
} // namespace ns3
#endif /*DESTINATION_PORT_NUMBER_H */