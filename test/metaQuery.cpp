#include <Eris/Metaserver.h>
#include <Eris/Log.h>
#include <Eris/PollDefault.h>
#include <Eris/ServerInfo.h>

#include <iostream>
#include <sigc++/slot.h>

using std::endl;
using std::cout;

bool queryDone = false;

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

int main(int argc, char* argv[])
{
    Eris::setLogLevel(Eris::LOG_WARNING);
    Eris::Logged.connect(SigC::slot(&erisLog));
    
    std::string metaServer = "metaserver.worldforge.org";
    if (argc > 1)
        metaServer = argv[1];
        
    // maximum of 5 simultaneous queries
    Eris::Meta meta(metaServer, 5);
    meta.CompletedServerList.connect(SigC::slot(&gotServerList));
    meta.AllQueriesDone.connect(SigC::slot(&queriesDone));
    meta.ReceivedServerInfo.connect(SigC::slot(&gotServer));
    
    cout << "querying " << metaServer << endl;
    meta.refresh();
    
    while (!queryDone)
    {
        Eris::PollDefault::poll(10);
    }
    
    cout << "final list contains " << meta.getGameServerCount() << " servers." << endl;
    
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
    
    return EXIT_SUCCESS;
}
