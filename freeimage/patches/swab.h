/** 
File created to supply swab function (required by LibRawLite) on systems that do 
not implement swab in unistd, like android.
Define 'NO_SWAB' to enable this file.
*/

#ifdef NO_SWAB 

#ifndef CUSTOM_SWAB_H
#define CUSTOM_SWAB_H

#include <stdlib.h>


#ifdef __cplusplus
extern "C" {
#endif

/** (description extracted from swab manpage)
The swab() function copies n bytes from the array pointed to by from to
the array pointed to by to, exchanging adjacent  even  and  odd  bytes.
This  function is used to exchange data between machines that have different 
low/high byte ordering.
This function does nothing when n is negative.  When n is positive  and
odd, it handles n-1 bytes as above, and does something unspecified with
the last byte.  (In other words, n should be even.)
*/
void swab(const void *from, void *to, size_t n);


#ifdef __cplusplus
}
#endif

#endif //CUSTOM_SWAB_H

#endif //NO_SWAB 
