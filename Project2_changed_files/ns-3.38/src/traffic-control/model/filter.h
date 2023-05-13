#ifndef FILTER_H
#define FILTER_H

 
#include "ns3/log.h"
#include "filter-element.h"

namespace ns3 {

/**
 * Class to store filters
 * @author Gandhar Kulkarni
*/
class Filter: public Object {
  public:


  Filter ();
  ~Filter ();


  std::vector<ns3::FilterElement*> elements;
  bool match (Ptr<ns3::Packet> packet);  

};

} // namespace ns3

#endif /* FILTER_H */