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

void gotServerCount(int count)
{
    cout << "metaserver knows about " << count << " servers." << endl;
}

void gotServer(const Eris::ServerInfo& info)
{
    cout << "got info for server: " << info.getServername() << '/'
        << info.getHostname() << endl;
    cout << "ruleset:" << info.getRuleset() << endl;
    cout << "uptime:" << info.getUptime() << endl;
    cout << "ping:" << info.getPing() << endl;
}

void listComplete()
{
    cout << "query complete" << endl;
    queryDone = true;
}

int main(int argc, char* argv[])
{
    Eris::setLogLevel(Eris::LOG_DEBUG);
    Eris::Logged.connect(SigC::slot(&erisLog));
    
    std::string metaServer = "metaserver.worldforge.org";
    if (argc > 1)
        metaServer = argv[1];
        
    Eris::Meta meta(metaServer, 5);
    meta.GotServerCount.connect(SigC::slot(&gotServerCount));
    meta.CompletedServerList.connect(SigC::slot(&listComplete));
    meta.ReceivedServerInfo.connect(SigC::slot(&gotServer));
    
    cout << "querying " << metaServer << endl;
    meta.refresh();
    
    while (!queryDone)
    {
        Eris::PollDefault::poll(10);
    }
    
    cout << "final list contains " << meta.getGameServerCount() << " servers." << endl;
}
