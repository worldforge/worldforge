#include <Eris/DeleteLater.h>
#include <vector>

namespace Eris
{

static std::vector<BaseDeleteLater*> global_deleteLaterQueue;

void pushDeleteLater(BaseDeleteLater* bl)
{
    global_deleteLaterQueue.push_back(bl);
}

void execDeleteLaters()
{
    while (!global_deleteLaterQueue.empty()) {
        global_deleteLaterQueue.back()->execute();
        delete global_deleteLaterQueue.back();
        global_deleteLaterQueue.pop_back();
    }
}

} // of namespace
