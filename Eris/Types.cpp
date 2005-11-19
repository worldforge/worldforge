#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <Eris/Types.h>
#include <Atlas/Message/Element.h>

using namespace Atlas::Message;

namespace Eris
{

void mergeOrCopyElement(const Element& src, Element& dst)
{
    if (!src.isMap() || !dst.isMap()) {
        dst = src;
        return;
    }
    
    const MapType& srcMap(src.asMap());
    MapType& dstMap(dst.asMap());
    
    MapType::iterator dit = dstMap.begin();
    MapType::const_iterator sit = srcMap.begin();
    
    for (; sit != srcMap.end(); ++sit) {   
        // advance destination to us / past us
        while (dit->first < sit->first) ++dit;
        if (dit == dstMap.end()) break;
        
        if (dit->first == sit->first) {
            // exists in destination, merge
            mergeOrCopyElement(sit->second, dit->second);
        } else {
            // only in source, insert
            dstMap.insert(dit, *sit);
        }
    } // of source map iteration
    
    // copy over remanining attrs
    while (sit != srcMap.end()) dstMap.insert(dit, *sit++);
}

} // of namespace
