#ifndef __AtlasIntList_h__
#define __AtlasIntList_h__

#define X(postfix) Int##postfix
#define X2(prefix,postfix) prefix##Int##postfix

#define TYPE_STR "Int_list"

#define IntList_Check(op) ((op)->ob_type == &IntList_Type)

/* Macro, trading safety for speed */
#define IntList_GET_ITEM(op, i)	(((IntListObject *)(op))->ob_item[i])
#define IntList_SET_ITEM(op, i, v)	(((IntListObject *)(op))->ob_item[i] = (v))
#define IntList_GET_SIZE(op)		(((IntListObject *)(op))->ob_size)

#include "XList_h.inc"

#endif


