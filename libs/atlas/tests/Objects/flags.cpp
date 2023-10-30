#include <Atlas/Objects/RootOperation.h>

int main()
{
    Atlas::Objects::Operation::RootOperation op;

    assert(!op->hasAttrFlag(Atlas::Objects::Operation::FROM_FLAG));
    op->setFrom("foo");
    assert(op->hasAttrFlag(Atlas::Objects::Operation::FROM_FLAG));
    op->removeAttr("from");
    assert(!op->hasAttrFlag(Atlas::Objects::Operation::FROM_FLAG));
    op->setFrom("bar");
    assert(op->hasAttrFlag(Atlas::Objects::Operation::FROM_FLAG));
    op->removeAttrFlag(Atlas::Objects::Operation::FROM_FLAG);
    assert(!op->hasAttrFlag(Atlas::Objects::Operation::FROM_FLAG));
}
