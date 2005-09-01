#include "viewTest.h"
#include "testUtils.h"
#include "controller.h"
#include "signalHelpers.h"
#include "setupHelpers.h"

#include <Eris/Account.h>
#include <Eris/Avatar.h>
#include <Eris/Entity.h>
#include <Eris/View.h>
#include <Eris/PollDefault.h>
#include <sigc++/object_slot.h>
#include <sigc++/object.h>

using std::cout;
using std::endl;

class ViewObserver : public SigC::Object
{
public:
    ViewObserver(Eris::View* v)
    {
        v->Appearance.connect(SigC::slot(*this, &ViewObserver::onAppear));
        v->Disappearance.connect(SigC::slot(*this, &ViewObserver::onDisappear));
    }

    int getAppearCount(Eris::Entity* e)
    {
        return m_appears[e];
    }
    
    int getDisappearCount(Eris::Entity* e)
    {
        return m_disappears[e];
    }
private:
    void onAppear(Eris::Entity* e)
    {
        m_appears[e]++;
    }
    
    void onDisappear(Eris::Entity* e)
    {
        m_disappears[e]++;
    }
    
    std::map<Eris::Entity*, int> m_appears, 
        m_disappears;
};

class CharacterGetter : public SigC::Object
{
public:
    CharacterGetter(Eris::Avatar* av) :
        m_done(false)
    {
        av->GotCharacterEntity.connect(SigC::slot(*this, &CharacterGetter::onGetCharacter));
    }
    
    void run()
    {
        while (!m_done) Eris::PollDefault::poll();
    }
private:
    void onGetCharacter(Eris::Entity*)
    {
        m_done = true;
    }
    
    bool m_done;
};


WaitForAppearance::WaitForAppearance(Eris::View* v, const std::string& eid) : 
    m_view(v)
{
    waitFor(eid);
    v->Appearance.connect(SigC::slot(*this, &WaitForAppearance::onAppear));
}

void WaitForAppearance::waitFor(const std::string& eid)
{
    Eris::Entity* e = m_view->getEntity(eid);
    if (e && e->isVisible()) {
        return;
    }
    
    m_waiting.insert(eid);
}

void WaitForAppearance::run()
{
    while (!m_waiting.empty()) Eris::PollDefault::poll();
}

void WaitForAppearance::onAppear(Eris::Entity* e)
{
    m_waiting.erase(e->getId());
}


WaitForDisappearance::WaitForDisappearance(Eris::View* v, const std::string& eid) : 
    m_view(v)
{
    waitFor(eid);
    v->Disappearance.connect(SigC::slot(*this, &WaitForDisappearance::onDisappear));
}

void WaitForDisappearance::waitFor(const std::string& eid)
{
    Eris::Entity* e = m_view->getEntity(eid);
    if (!e || !e->isVisible()) {
        std::cerr << "wait on invisible entity " << eid << std::endl;
        return;
    }
    
    m_waiting.insert(eid);
}

void WaitForDisappearance::run()
{
    while (!m_waiting.empty()) Eris::PollDefault::poll();
}

void WaitForDisappearance::onDisappear(Eris::Entity* e)
{
    m_waiting.erase(e->getId());
}

#pragma mark -

void testCharacterInitialVis(Controller& ctl)
{
    AutoConnection con = stdConnect();
    AutoAccount player = stdLogin("account_B", "sweede", con.get());
    ctl.setEntityVisibleToAvatar("_hut_01", "acc_b_character");
    
    AvatarGetter ag(player.get());
    ag.returnOnSuccess();
    AutoAvatar av = ag.take("acc_b_character");
    
    Eris::View* v = av->getView();
    ViewObserver vob(v);
    
    CharacterGetter cg(av.get());
    cg.run();
    
    assert(vob.getAppearCount(av->getEntity()) == 1);
    assert(vob.getDisappearCount(av->getEntity()) == 0);
}

void testAppearance(Controller& ctl)
{
    AutoConnection con = stdConnect();
    AutoAccount acc = stdLogin("account_B", "sweede", con.get());
    
    ctl.setEntityVisibleToAvatar("_hut_01", "acc_b_character");
    AutoAvatar av = AvatarGetter(acc.get()).take("acc_b_character");
    
    Eris::View* v = av->getView();
    
    ctl.setEntityVisibleToAvatar("_potato_2", av.get());
    ctl.setEntityVisibleToAvatar("_pig_01", av.get());
    ctl.setEntityVisibleToAvatar("_field_01", av.get());
        
    {
        WaitForAppearance wf(v, "_field_01");
        wf.waitFor("_pig_01");
        wf.waitFor("_potato_2");
        wf.run();
    }
    
    ctl.setEntityInvisibleToAvatar("_field_01", av.get());
    WaitForDisappearance df(v, "_field_01");
    df.waitFor("_pig_01");
    df.waitFor("_potato_2");
    
    df.run();
    
    ctl.setEntityVisibleToAvatar("_potato_1", av.get());
    //
    ctl.setEntityVisibleToAvatar("_pig_02", av.get());
    ctl.setEntityVisibleToAvatar("_field_01", av.get());
    
    {
        WaitForAppearance af2(v, "_field_01");
        af2.waitFor("_potato_1");
        af2.run();
    }
}
