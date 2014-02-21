#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include "Types.h"
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
    
    MapType::iterator dit;
    MapType::const_iterator sit = srcMap.begin();
    MapType::const_iterator sitend = srcMap.end(); 
    
    for (; sit != sitend; ++sit) { 
        dit = dstMap.find(sit->first);
        if (dit == dstMap.end()) {
            // only in source, insert
            dstMap[sit->first] = sit->second;
        } else {
            mergeOrCopyElement(sit->second, dit->second);
        }
    } // of source map iteration
}

} // of namespace
