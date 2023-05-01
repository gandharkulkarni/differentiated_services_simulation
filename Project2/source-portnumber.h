#include <stdint.h>
namespace ns3 {
class SourcePortNumber
{

private:
  uint32_t m_srcPortNumberValue;

public:

  bool match(uint32_t portNumberToMatch) {
    return m_srcPortNumberValue == portNumberToMatch;
  }
};
} // namespace ns3