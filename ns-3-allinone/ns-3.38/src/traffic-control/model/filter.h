#ifndef FILTER_H
#define FILTER_H

#include "filter-element.h"

#include "ns3/log.h"

// #include "ns3/integer.h"
// #include "ns3/ppp-header.h"
// #include "ns3/pointer.h"
// #include "queue-disc.h"

namespace ns3
{

class Filter : public Object
{
  public:
    Filter();
    ~Filter();

    // static TypeId GetTypeId (void);
    std::vector<ns3::FilterElement*> elements; // TODO check for ns3::FilterElement*
    bool match(Ptr<ns3::Packet> packet);
};

} // namespace ns3

#endif /* FILTER_H */