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

void APackedAsciiEncoder::walkTree(int nest, int names, AObject& list)
{
	int	i;
	string	buf;

	if (list.isList()) {
		//::printf("encode list\n");
		//fflush(stdout);
		string name;
		if (names) name = list.getName();
		printf("(%s=", name.c_str());
		for (i=0; i<list.length(); i++) {
			AObject tmp;
			list.get(i,tmp);
			walkTree(0, 0, tmp);
		}
		printf(")");
	} 
	if (list.isMap()) {
		//::printf("encode map\n");
		//fflush(stdout);
		AObject keys = list.keys();
		string name;
		if (names) name = list.getName();
		printf("[%s=", name.c_str());
		for (i=0; i<keys.length(); i++) {
			AObject key;
			AObject	tmp;
			keys.get(i, key);
			list.get(key.asString(), tmp);
			walkTree(0, 1, tmp);
		}
		printf("]");
	} 

	if (list.isString()) {
		//::printf("encode string\n");
		//fflush(stdout);
		string name;
		if (names) name = list.getName();
		printf("$%s=%s", name.c_str(), list.asString().c_str());
	}
	if (list.isLong()) {
		//::printf("encode long\n");
		//fflush(stdout);
		string name;
		if (names) name = list.getName();
		printf("%%%s=%li", name.c_str(), list.asLong());
	}
	if (list.isFloat()) {
		//::printf("encode float\n");
		//fflush(stdout);
		string name;
		if (names) name = list.getName();
		printf("#%s=%.2f", name.c_str(), list.asFloat());
	}

}

string APackedAsciiEncoder::encodeMessage(AObject& msg)
{
	int	i;

	// start a new message
	buffer = "";
	// format the message header
	printf("{%s=", msg.getName().c_str());
	// walk the tree

	AObject keys = msg.keys();
	for (i=0; i<keys.length(); i++) {
		AObject	key;
		AObject	tmp;
		keys.get(i, key);
		//::printf("\n **** %i = %s\n", i, key.asString().c_str());
		fflush(stdout);
		msg.get(key.asString(), tmp);
		walkTree(0, 1, tmp);
	}
	// and close off the message
	printf("}");
	// return result
	return buffer;
}





