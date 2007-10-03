#ifndef VIEW_TEST_H
#define VIEW_TEST_H

#include <string>
#include <set>

namespace Eris
{
    class Entity;
    class View;
}

class Controller;

void testCharacterInitialVis(Controller&);
void testAppearance(Controller& ctl);
void testLookQueue(Controller& ctl);
void testEntityCreation(Controller& ctl);
void testUnseen(Controller& ctl);
void testTasks(Controller& ctl);

class WaitForAppearance
{
public:
    WaitForAppearance(Eris::View* v, const std::string& eid);    
    void waitFor(const std::string& eid);    
    void run();
private:
    void onAppear(Eris::Entity* e);
        
    Eris::View* m_view;
    std::set<std::string> m_waiting;
};

class WaitForDisappearance
{
public:
    WaitForDisappearance(Eris::View* v, const std::string& eid);    
    void waitFor(const std::string& eid);    
    void run();

private:
    void onDisappear(Eris::Entity* e);
        
    Eris::View* m_view;
    std::set<std::string> m_waiting;
};

#endif // of VIEW_TEST_H
