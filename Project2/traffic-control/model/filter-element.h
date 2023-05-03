#ifndef FILTER_ELEMENT_H_
#define FILTER_ELEMENT_H_

#include "ns3/pointer.h"
#include "ns3/packet.h"
#include "ipv4-address.h"

namespace ns3 {
class FilterElement
{
public:
  FilterElement ();
  virtual ~FilterElement ();
  virtual bool match (Ptr<ns3::Packet> p) = 0;
};
} // namespace ns3

#endif