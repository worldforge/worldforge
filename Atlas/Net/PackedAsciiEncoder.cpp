/*
        AtlasPackedAsciiEncoder.cpp
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#include "PackedAsciiEncoder.h"

//#include <map.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <string>

void APackedAsciiEncoder::walkTree(int nest, const char* xname,
                                   const AObject& list)
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
			AObject tmp;
			list.get(i,tmp);
			walkTree(0, "", tmp);
		}
		printf(")");
	} 
	if (list.isURIList()) {
		printf("<!%s=", name);
		for (i=0; i<list.length(); i++) {
			AObject tmp;
			list.get(i,tmp);
			walkTree(0, "", tmp);
		}
		printf(">");
	} 
	if (list.isIntList()) {
		printf("<@%s=", name);
		for (i=0; i<list.length(); i++) {
			AObject tmp;
			list.get(i,tmp);
			walkTree(0, "", tmp);
		}
		printf(">");
	} 
/*	if (list.isLongList()) {
		printf("<%%%s=", name);
		for (i=0; i<list.length(); i++) {
			AObject tmp;
			list.get(i,tmp);
			walkTree(0, "", tmp);
		}
		printf(">");
	} 
*/	if (list.isFloatList()) {
		printf("<#%s=", name);
		for (i=0; i<list.length(); i++) {
			AObject tmp;
			list.get(i,tmp);
			walkTree(0, "", tmp);
		}
		printf(">");
	} 
	if (list.isStringList()) {
		printf("<$%s=", name);
		for (i=0; i<list.length(); i++) {
			AObject tmp;
			list.get(i,tmp);
			walkTree(0, "", tmp);
		}
		printf(">");
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
		string tmp = hexEncodeString(list.asString(), 
						'+', "{[(<#!@%>)]}");
		printf("$%s=%s", name, tmp.c_str());
	}
	if (list.isURI()) {
		string tmp = hexEncodeString(list.getURIPath().asString(), 
						'+', "{[(<#!@%>)]}");
		printf("!%s=%li", name, tmp.c_str());
	}
	if (list.isInt()) {
		printf("@%s=%li", name, list.asInt());
	}
/*	if (list.isLong()) {
		printf("%%%s=%li", name, list.asLong());
	}
*/	if (list.isFloat()) {
		printf("#%s=%.2f", name, list.asFloat());
	}

}

string APackedAsciiEncoder::encodeMessage(const AObject& msg)
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





