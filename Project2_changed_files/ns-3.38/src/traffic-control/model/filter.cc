#include "ns3/log.h"
#include "source-ip-address.h"
#include "traffic-class.h"
#include "filter.h"


namespace ns3 {
    NS_LOG_COMPONENT_DEFINE ("Filter");
    NS_OBJECT_ENSURE_REGISTERED (Filter);

    Filter::Filter ()
    {
      NS_LOG_FUNCTION (this);
    }
    
    Filter::~Filter()
    {
      NS_LOG_FUNCTION (this); 
    }

    /**
     * Match the packet attributes against all filters for queue.
     * @return bool
     * */    
    bool Filter::match (Ptr<ns3::Packet> packet){
      auto iter = elements.begin();     
      for(;iter!=elements.end();iter++){
          // std::cout<<*iter<<" "; 
          if ((**iter).match(packet)==false){              
              return false;
          }                             
      }
      return true;      
    }
}// namespace ns3