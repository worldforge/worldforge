//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.

#include <iostream>

void logEvent(LogEvent lvl, const std::string & msg)
{
    std::cerr << "(event) " << lvl << " " << msg << std::endl;

}
