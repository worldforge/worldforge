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

#include <string>

void APackedAsciiEncoder::walkTree(int nest, const char* name, AObject& list)
{
	int	i;
	string	buf;

	if (list.isList()) {
		printf("(%s=", name);
		for (i=0; i<list.length(); i++) {
			AObject tmp;
			list.get(i,tmp);
			walkTree(0, "", tmp);
		}
		printf(")");
	} 
	if (list.isMap()) {
		AObject keys = list.keys();
		printf("[%s=", name);
		for (i=0; i<keys.length(); i++) {
			AObject key;
			AObject	tmp;
			keys.get(i, key);
			list.get(key.asString(), tmp);
			walkTree(0, key.asString().c_str(), tmp);
		}
		printf("]");
	} 

	if (list.isString()) {
		printf("$%s=%s", name, list.asString().c_str());
	}
	if (list.isLong()) {
		printf("%%%s=%li", name, list.asLong());
	}
	if (list.isFloat()) {
		printf("#%s=%.2f", name, list.asFloat());
	}

}

string APackedAsciiEncoder::encodeMessage(AObject& msg)
{
	int	i;

	// start a new message
	buffer = "";
	// format the message header
	printf("{");
	// walk the tree
	walkTree(0, "", msg);
	// and close off the message
	printf("}");
	// return result
	return buffer;
}





