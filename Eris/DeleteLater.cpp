#include <Eris/DeleteLater.h>
#include <vector>

namespace Eris
{

static std::vector<BaseDeleteLater*> global_deleteLaterQueue;

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
        
        BaseDeleteLater* dl = global_deleteLaterQueue.back();
        global_deleteLaterQueue.pop_back();
        delete dl;
    }
}

} // of namespace
