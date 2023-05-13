#ifndef SOURCE_MASK_H
#define SOURCE_MASK_H


#include "ns3/log.h"
#include "filter-element.h"
#include "ns3/ipv4-address.h" 

namespace ns3 {

/**
 * Class to compare Source Mask of packet
 * @author Gandhar Kulkarni
*/
class SourceMask: public FilterElement {
public:

  static TypeId GetTypeId (void);

  SourceMask ();
  SourceMask (Ipv4Address source_ip_address, Ipv4Mask ipv4Mask);  
  ~SourceMask ();

  
  bool match (Ptr<Packet> packet) override;


private:
  Ipv4Address source_ip_address;
  Ipv4Mask value;   

};

} // namespace ns3

#endif /* SOURCE_MASK_H */