#ifndef __AtlasStringList_h__
#define __AtlasStringList_h__

#define X(postfix) String##postfix
#define X2(prefix,postfix) prefix##String##postfix

#define TYPE_STR "String_list"

#define StringList_Check(op) ((op)->ob_type == &StringList_Type)

/* Macro, trading safety for speed */
#define StringList_GET_ITEM(op, i)	(((StringListObject *)(op))->ob_item[i])
#define StringList_SET_ITEM(op, i, v)	(((StringListObject *)(op))->ob_item[i] = (v))
#define StringList_GET_SIZE(op)		(((StringListObject *)(op))->ob_size)

#include "AtlasXList_h.inc"

#endif


