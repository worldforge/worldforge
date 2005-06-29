#ifndef TEST_SETUP_HELPERS_H
#define TEST_SETUP_HELPERS_H

#include <Eris/Connection.h>
#include <Eris/Account.h>
#include <Eris/Avatar.h>

#include <sigc++/object.h>

typedef std::auto_ptr<Eris::Connection> AutoConnection;
typedef std::auto_ptr<Eris::Account> AutoAccount;
typedef std::auto_ptr<Eris::Avatar> AutoAvatar;

AutoConnection stdConnect();

AutoAccount stdLogin(const std::string& uname, 
                    const std::string& pwd, Eris::Connection* con);

class AvatarGetter : public SigC::Object
{
public:
    AvatarGetter(Eris::Account* acc);
        
    void expectFailure()
    {
        m_expectFail = true;
    }
    
    AutoAvatar take(const std::string& charId);    
private:
    void success(Eris::Avatar* av);    
    void failure(const std::string& msg);
    
    bool m_waiting;
    Eris::Account* m_acc;
    AutoAvatar m_av;
    bool m_expectFail, m_failed;
};

#endif // of TEST_SETUP_HELPERS_H