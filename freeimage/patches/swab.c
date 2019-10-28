#ifdef NO_SWAB

#include "swab.h"

/** swab copies pair of bytes from 'src' pointer to 'dst' pointer, for-each pair
it invert the order of bytes. If 'n' is an odd number, the last number from 'src'
is ignored (does not copy on dst).
*/
void swab(const void *src, void *dst, size_t n){
    const char * srcBytes = (const char *)src;
    char * dstBytes = (char *)dst;

    size_t i =0;
    while(i + 1 < n){
        dstBytes[i]     = srcBytes[i + 1];
        dstBytes[i + 1] = srcBytes[i];
        
        i += 2;
    }
}
#endif
