#ifndef __AtlasLongList_h__
#define __AtlasLongList_h__

#define X(postfix) Long##postfix
#define X2(prefix,postfix) prefix##Long##postfix

#define TYPE_STR "Long_list"

#define LongList_Check(op) ((op)->ob_type == &LongList_Type)

/* Macro, trading safety for speed */
#define LongList_GET_ITEM(op, i)	(((LongListObject *)(op))->ob_item[i])
#define LongList_SET_ITEM(op, i, v)	(((LongListObject *)(op))->ob_item[i] = (v))
#define LongList_GET_SIZE(op)		(((LongListObject *)(op))->ob_size)

#include "AtlasXList_h.inc"

#endif


