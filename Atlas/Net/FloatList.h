#ifndef __AtlasFloatList_h__
#define __AtlasFloatList_h__

#define X(postfix) Float##postfix
#define X2(prefix,postfix) prefix##Float##postfix

#define TYPE_STR "float_list"

#define FloatList_Check(op) ((op)->ob_type == &FloatList_Type)

/* Macro, trading safety for speed */
#define FloatList_GET_ITEM(op, i)	(((FloatListObject *)(op))->ob_item[i])
#define FloatList_SET_ITEM(op, i, v)	(((FloatListObject *)(op))->ob_item[i] = (v))
#define FloatList_GET_SIZE(op)		(((FloatListObject *)(op))->ob_size)

#include "XList_h.inc"

#endif


