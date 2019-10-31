#include <boost/regex.hpp>
#include <boost/date_time.hpp>

int main()
{
    boost::gregorian::date weekstart(2002,boost::gregorian::Feb,1);
    boost::regex expression{"\\w+\\s\\w+"};
}

