#include <Eris/DeleteLater.h>
#include <deque>

namespace Eris
{

static std::deque<BaseDeleteLater*> global_deleteLaterQueue;

BaseDeleteLater::~BaseDeleteLater()
{
}

void pushDeleteLater(BaseDeleteLater* bl)
{
    global_deleteLaterQueue.push_back(bl);
}

void execDeleteLaters()
{
    while (!global_deleteLaterQueue.empty()) {
        // ordering here is important, in cases where deleting 'foo' causes
        // other objects to be 'deleteLater'ed; we want to clean them up in
        // this same cycle, not leave the hanging around.
        
        BaseDeleteLater* dl = global_deleteLaterQueue.front();
        global_deleteLaterQueue.pop_front();
        delete dl;
    }
}

} // of namespace
