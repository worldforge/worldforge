#ifndef __AtlasURIList_h__
#define __AtlasURIList_h__

#include "URI.h"

#define X(postfix) URI##postfix
#define X2(prefix,postfix) prefix##URI##postfix

#define TYPE_STR "URI_list"

#define URIList_Check(op) ((op)->ob_type == &URIList_Type)

/* Macro, trading safety for speed */
#define URIList_GET_ITEM(op, i)		(((URIListObject *)(op))->ob_item[i])
#define URIList_SET_ITEM(op, i, v)	(((URIListObject *)(op))->ob_item[i] = (v))
#define URIList_GET_SIZE(op)		(((URIListObject *)(op))->ob_size)

#include "XList_h.inc"

#endif

