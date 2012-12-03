#ifndef TEST_SETUP_HELPERS_H
#define TEST_SETUP_HELPERS_H

#include <Eris/Connection.h>
#include <Eris/Account.h>
#include <Eris/Avatar.h>

typedef std::unique_ptr<Eris::Connection> AutoConnection;
typedef std::unique_ptr<Eris::Account> AutoAccount;
typedef std::unique_ptr<Eris::Avatar> AutoAvatar;

AutoConnection stdConnect();

AutoAccount stdLogin(const std::string& uname, 
                    const std::string& pwd, Eris::Connection* con);

class AvatarGetter
{
public:
    AvatarGetter(Eris::Account* acc);
        
    void expectFailure()
    {
        m_expectFail = true;
    }
    
    void returnOnSuccess()
    {
        m_earlyReturn = true;
    }
    
    AutoAvatar take(const std::string& charId);
    AutoAvatar create(const Atlas::Objects::Entity::RootEntity& charEnt);
    
private:
    void success(Eris::Avatar* av);    
    void failure(const std::string& msg);
    
    bool m_waiting;
    Eris::Account* m_acc;
    AutoAvatar m_av;
    bool m_expectFail, m_failed;
    bool m_earlyReturn;
};

#endif // of TEST_SETUP_HELPERS_H
