#include <Eris/Metaserver.h>
#include <Eris/Log.h>
#include <Eris/PollDefault.h>
#include <Eris/ServerInfo.h>

#include <iostream>
#include <sigc++/slot.h>

using std::endl;
using std::cout;
using std::cerr;

bool queryDone = false,
    failure = false;

void erisLog(Eris::LogLevel, const std::string& msg)
{
    cout << "ERIS: " << msg << endl;
}

void gotServerList(int count)
{
    cout << "metaserver knows about " << count << " servers." << endl;
}

void gotServer(const Eris::ServerInfo& info)
{
    cout << "got info for server: " << info.getServername() << '/'
        << info.getHostname() << endl;
}

void queriesDone()
{
    cout << "query complete" << endl;
    queryDone = true;
}

void queryFailed(const std::string& msg)
{
    cerr << "got query failure: " << msg << endl;
    failure = true;
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
            cout << "\truleset:" << sv.getRuleset() << endl;
            cout << "\tuptime:" << sv.getUptime() << endl;
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
            cout << "Up: " << sv.getUptime() << " ("  << sv.getPing() << " ping)<br/>" << endl;
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
    Eris::setLogLevel(Eris::LOG_DEBUG);
    Eris::Logged.connect(SigC::slot(&erisLog));

    bool htmlDump = false;
    
    std::string metaServer = "metaserver.worldforge.org";
    if (argc > 1)
    {
       if (strcmp(argv[1], "--html") == 0)
           htmlDump = true;
       else
       {
         metaServer = argv[1];

         if ((argc > 2) && (strcmp(argv[2], "--html") == 0))
             htmlDump = true;
       }
    }
    
    // maximum of 5 simultaneous queries
    Eris::Meta meta(metaServer, 5);
    meta.CompletedServerList.connect(SigC::slot(&gotServerList));
    meta.AllQueriesDone.connect(SigC::slot(&queriesDone));
    meta.ReceivedServerInfo.connect(SigC::slot(&gotServer));
    meta.Failure.connect(SigC::slot(&queryFailed));
    
    cout << "querying " << metaServer << endl;
    meta.refresh();
    
    while (!queryDone && !failure)
    {
        Eris::PollDefault::poll(10);
    }
    
    if (failure) {
        cerr << "querying meta server at " << metaServer << " failed" << endl;
        return EXIT_FAILURE;
    }
    
    cout << "final list contains " << meta.getGameServerCount() << " servers." << endl;
  
    if (htmlDump == true)
        dumpToHTML(meta);
    else
        dumpToScreen(meta);
    
    return EXIT_SUCCESS;
}
