#include "filter.h"

namespace ns3 {

// Constructor
Filter::Filter ()
{
}
// Destructor
Filter::~Filter ()
{
}

// Return true if all the FilterElement is true, otherwise false.
bool
Filter::match (Ptr<Packet> p)
{
  printf ("match() in Filter start.\n");

  bool isMatchedFilterElement = false;
  printf ("filterElements.size(): %lu\n", filterElements.size ());
}
} // namespace ns3