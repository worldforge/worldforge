/*
        AtlasPackedAsciiEncoder.cpp
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#include "AtlasPackedAsciiEncoder.h"

#include <map.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void APackedAsciiEncoder::walkTree(int nest, int names, AObject *list)
{
	int	i;
	char	buf[80];
	char	pre[80];

	*pre = 0;
	for (int j=0; j<nest; j++) {
		strcat(pre,"\t");
	}

	if (list->isList()) {
		char* name = "";
		if (names) name = list->getName();
		printf("(%s=", name);
		for (i=0; i<list->length(); i++) {
			AObject* tmp;
			list->get(i,tmp);
			walkTree(0, 0, tmp);
		}
		printf(")");
	} 
	if (list->isMap()) {
		AObject* keys = list->keys();
		char* name = "";
		if (names) name = list->getName();
		printf("[%s=", name);
		for (i=0; i<keys->length(); i++) {
			char* key; AObject* tmp;
			keys->get(i, key);
			list->get(key, tmp);
			walkTree(0, 1, tmp);
		}
		printf("]");
	} 

	if (list->isString()) {
		char* name = "";
		if (names) name = list->getName();
		printf("$%s=%s", name, list->asString());
	}
	if (list->isLong()) {
		char* name = "";
		if (names) name = list->getName();
		printf("%%%s=%li", name, list->asLong());
	}
	if (list->isFloat()) {
		char* name = "";
		if (names) name = list->getName();
		printf("#%s=%.2f", name, list->asFloat());
	}

}

char* APackedAsciiEncoder::encodeMessage(AObject *msg)
{
	int	i;

	// start a new message
	*buffer = 0;
	// format the message header
	printf("{%s=", msg->getName());
	// walk the tree

	AObject* keys = msg->keys();
	for (i=0; i<keys->length(); i++) {
		char* key; AObject* tmp;
		keys->get(i, key);
		msg->get(key, tmp);
		walkTree(0, 1, tmp);
	}
	// and close off the message
	printf("}");
	// return result
	return buffer;
}





