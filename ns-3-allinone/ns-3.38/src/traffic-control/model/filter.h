#ifndef FILTER_H
#define FILTER_H

#include "filter-element.h"

#include "ns3/log.h"
#include "filter-element.h"
#include "destination-ip-address.h"
#include "destination-mask.h"
#include "destination-port-number.h"
#include "protocol-number.h"
#include "source-ip-address.h"
#include "source-mask.h"
#include "source-port-number.h"

namespace ns3
{

class Filter : public Object
{
  public:
    Filter();
    Filter(DestinationIpAddress* destIpAddr, DestinationMask* destMask, DestinationPortNumber* destPortNum, 
         ProtocolNumber* protNum, SourceIpAddress* sourIpAddr, SourceMask* sourMask, SourcePortNumber* sourPortNum);
    ~Filter();

    // static TypeId GetTypeId (void);
    std::vector<ns3::FilterElement*> elements; // TODO check for ns3::FilterElement*
    bool match(Ptr<ns3::Packet> packet);
};

} // namespace ns3

#endif /* FILTER_H */