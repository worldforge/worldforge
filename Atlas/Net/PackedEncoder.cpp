/*
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#include "PackedEncoder.h"

//#include <map.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <string>

namespace Atlas
{

void PackedEncoder::walkTree(int nest, const char* xname,
                                   const Object& list)
{
	int	i;
	string	buf;
	string  yname(xname);
	char*	name;

	yname = hexEncodeString(xname, '+', "{[(<#!@%>)]}");
	name = const_cast<char*>(yname.c_str());
	
	if (list.isList()) {
		printf("(%s=", name);
		for (i=0; i<list.length(); i++) {
			Object tmp;
			list.get(i,tmp);
			walkTree(0, "", tmp);
		}
		printf(")");
	} 
	if (list.isMap()) {
		Object keys = list.keys();
		printf("[%s=", name);
		for (i=0; i<keys.length(); i++) {
			Object key;
			Object	tmp;
			keys.get(i, key);
			list.get(key.asString(), tmp);
			walkTree(0, key.asString().c_str(), tmp);
		}
		printf("]");
	} 

	if (list.isString()) {
		string tmp = hexEncodeString(list.asString(), 
						'+', "{[(<#!@%>)]}");
		printf("$%s=%s", name, tmp.c_str());
	}
	if (list.isInt()) {
		printf("@%s=%li", name, list.asInt());
	}
	if (list.isFloat()) {
		printf("#%s=%.2f", name, list.asFloat());
	}

}

string PackedEncoder::encodeMessage(const Object& msg)
{
	//int	i;

	// start a new message
	buffer = "";
	// format the message header
	printf("{");
	// walk the tree
	walkTree(0, "", msg);
	// and close off the message
	printf("}\n");
	// return result
	return buffer;
}


} // namespace Atlas


