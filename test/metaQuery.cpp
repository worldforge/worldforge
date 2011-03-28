#include <Eris/Metaserver.h>
#include <Eris/Log.h>
#include <Eris/PollDefault.h>
#include <Eris/ServerInfo.h>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <sigc++/functors/ptr_fun.h>

using std::endl;
using std::cout;
using std::cerr;

bool queryDone = false,
    failure = false,
	exactTime = false;

void erisLog(Eris::LogLevel, const std::string& msg)
{
    cerr << "ERIS: " << msg << endl;
}

void gotServerList(int count)
{
    cerr << "metaserver knows about " << count << " servers." << endl;
}

void gotServer(const Eris::ServerInfo& info)
{
    cerr << "got info for server: " << info.getServername() << '/'
        << info.getHostname() << endl;
}

void queriesDone()
{
    cerr << "query complete" << endl;
    queryDone = true;
}

void queryFailed(const std::string& msg)
{
    cerr << "got query failure: " << msg << endl;
    failure = true;
}

std::string timeFormat(double time)
{
	static const std::string times[] = { "secs", "mins", "hours", "days", "weeks" };
	// int precs[] = { 0, 1, 1, 2, 2, 2 };
	// int divi = 0;
	std::stringstream result;
	std::string timestring;
	// double orig = time;
	
	if(exactTime == false)
	{
		static const int divs[] = { 60, 60, 24, 7, 0 };
		
#if 0
		while((divs[divi] > 0) && (time > divs[divi]))
		{
			std::stringstream division;
			division << (int)time % divs[divi] << " "
                                 << times[divi];
			if (!timestring.empty()) {
				timestring = ", " + timestring;
			}
			timestring = division.str() + timestring;
			time /= divs[divi++];
		}
#else
	if(exactTime == false) {
		for (int i = 0; i < 5; ++i) {
			std::stringstream division;
			int interval;
			if (i == 4) {
				interval = (int)time;
			} else {
				interval = (int)time % divs[i];
			}
			if (interval != 0) {
				division << interval << " " << times[i];
				if (!timestring.empty()) {
					timestring = ", " + timestring;
				}
				timestring = division.str() + timestring;
			}
			time /= divs[i];
			if (time < 1) {
				break;
			}
		}
	}
#endif
	}
	// result << std::fixed << std::setprecision(precs[divi]) << orig << "," << time << ' ' << times[divi];
	
	return timestring;
}

void dumpToScreen(const Eris::Meta& meta)
{
    for (unsigned int S=0; S < meta.getGameServerCount(); ++S)
    {
        const Eris::ServerInfo& sv = meta.getInfoForServer(S);
        cout << S << ": " << sv.getServername() << '/'<< sv.getHostname() << endl;
        
        switch (sv.getStatus())
        {
        case Eris::ServerInfo::VALID:
            cout << "\tserver: " << sv.getServer() << " " << sv.getVersion() << " (builddate " << sv.getBuildDate() << ")" << endl;
            cout << "\truleset:" << sv.getRuleset() << endl;
            cout << "\tuptime:" << timeFormat(sv.getUptime()) << endl;
            cout << "\tping:" << sv.getPing() << endl;
            cout << "\tconnected clients:" << sv.getNumClients() << endl;
            break;
            
        case Eris::ServerInfo::TIMEOUT:
            cout << "Timed out." << endl;
            break;
            
        case Eris::ServerInfo::QUERYING:
            cout << "Something is broken, all queries should be done" << endl;
            break;
            
        default:
            cout << "Query failed" << endl;
        }
    } // of server iteration
}

void dumpToXML(const Eris::Meta & meta)
{
    cout << "<metaquery>" << endl;
    for(unsigned int S=0; S < meta.getGameServerCount(); ++S)
    {
        const Eris::ServerInfo& sv = meta.getInfoForServer(S);
        
        cout << "<server status=\"";
        switch (sv.getStatus())
        {
        case Eris::ServerInfo::VALID: cout << "valid"; break;
        case Eris::ServerInfo::TIMEOUT: cout << "timeout"; break;
        case Eris::ServerInfo::QUERYING: cout << "querying"; break;
        default: cout << "failed";
        }
        cout << "\">" << endl;
        cout << "<address>" << sv.getHostname() << "</address>" << endl;
        if(sv.getStatus() == Eris::ServerInfo::VALID)
        {
            cout << "<status>valid</status>" << endl;
            cout << "<name>" << sv.getServername() << "</name>" << endl;
            cout << "<servertype>" << sv.getServer() << "</servertype>" << endl;
            cout << "<ruleset>" << sv.getRuleset() << "</ruleset>" << endl;
            cout << "<uptime>" << timeFormat(sv.getUptime()) << "</uptime>" << endl;
            cout << "<ping>" << sv.getPing() << "</ping>" << endl;
            cout << "<clients>" << sv.getNumClients() << "</clients>" << endl;
			cout << "<builddate>" << sv.getBuildDate() << "</builddate>" << endl;
			cout << "<version>" << sv.getVersion() << "</version>" << endl;
        }
        cout << "</server>" << endl;
    } // of server iteration
    cout << "</metaquery>" << endl;
}

void dumpToHTML(const Eris::Meta& meta)
{
    cout << "<div class=\"metaserver\">" << endl;
    cout << "  <dl>" << endl;

    for (unsigned int S=0; S < meta.getGameServerCount(); ++S)
    {
        const Eris::ServerInfo& sv = meta.getInfoForServer(S);

        cout << "    <dt>" << sv.getHostname() << " :: " << sv.getServername() << "</dt>" << endl;
        cout << "    <dd>" << endl;
        
        switch (sv.getStatus())
        {
        case Eris::ServerInfo::VALID:
            cout << "Server: " << sv.getServer() << " " << sv.getVersion() << " (builddate " << sv.getBuildDate() << ")<br/>" << endl;
            cout << "Ruleset: " << sv.getRuleset() << "<br/>" << endl;
            cout << "Up: " << timeFormat(sv.getUptime()) << " ("  << sv.getPing() << " ping)<br/>" << endl;
            cout << "Clients: " << sv.getNumClients() << endl;
            break;
            
        case Eris::ServerInfo::TIMEOUT:
            cout << "Timed out." << endl;
            break;
            
        case Eris::ServerInfo::QUERYING:
            cout << "Something is broken, all queries should be done" << endl;
            break;
            
        default:
            cout << "Query failed" << endl;
        }

        cout << "    </dd>" << endl;
    } // of server iteration

    cout << "  </dl>" << endl;
    cout << "</div>"  << endl;
}

int main(int argc, char* argv[])
{
    std::string metaServer = "metaserver.worldforge.org";
    std::vector< std::string > args(argv, argv + argc);
    void (* dumper)(const Eris::Meta &) = dumpToScreen;
    
    Eris::setLogLevel(Eris::LOG_DEBUG);
    Eris::Logged.connect(sigc::ptr_fun(&erisLog));
    
    if(args.size() > 1)
    {
        if(args[1].substr(0, 2) != "--")
        {
            metaServer = argv[1];
        }
        if(find(args.begin(), args.end(), "--html") != args.end())
        {
            dumper = dumpToHTML;
        }
        else if(find(args.begin(), args.end(), "--xml") != args.end())
        {
            dumper = dumpToXML;
        }
		if(find(args.begin(), args.end(), "--exact") != args.end())
		{
			exactTime = true;
		}
    }
    
    // maximum of 5 simultaneous queries
    Eris::Meta meta(metaServer, 100);
    meta.CompletedServerList.connect(sigc::ptr_fun(&gotServerList));
    meta.AllQueriesDone.connect(sigc::ptr_fun(&queriesDone));
    meta.ReceivedServerInfo.connect(sigc::ptr_fun(&gotServer));
    meta.Failure.connect(sigc::ptr_fun(&queryFailed));
    
    cerr << "querying " << metaServer << endl;
    meta.refresh();
    
    while (!queryDone && !failure)
    {
        Eris::PollDefault::poll(100);
    }
    
    if (failure) {
        cerr << "querying meta server at " << metaServer << " failed" << endl;
        return EXIT_FAILURE;
    }
    
    cerr << "final list contains " << meta.getGameServerCount() << " servers." << endl;
  
    dumper(meta);
    
    return EXIT_SUCCESS;
}
