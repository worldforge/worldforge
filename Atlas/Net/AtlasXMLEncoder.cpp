/*
        AtlasXMLEncoder.cpp
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#include "AtlasXMLEncoder.h"

#include <map.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <string>


void AXMLEncoder::walkTree(int nest, string name, AObject& list)
{
	int	i;
	string	buf;
	string	pre;

	for (int j=0; j<nest; j++) {
		pre.append("    ");
	}


	if (list.isList()) {
		// precheck types here
		string pfix("");

// 		if (list.getListType() == AObject::AIntList)		pfix.append("int_");
// 		else if (list.getListType() == AObject::AFloatList)	pfix.append("float_");
// 		else if (list.getListType() == AObject::AStringList)	pfix.append("string_");

		if (name.length() > 0) {
			printf("%s<%slist name=\"%s\">\n", pre.c_str(), pfix.c_str(), name.c_str());
		} else {
			printf("%s<%slist>\n", pre.c_str(), pfix.c_str());
		}
		for (i=0; i<list.length(); i++) {
			AObject tmp;
			list.get(i, tmp);
			walkTree(nest+1, "", tmp);
		}
		printf("%s</list>\n",pre.c_str());
	} 
	if (list.isMap()) {
		AObject keys = list.keys();
		if (name.length() > 0) {
			printf("%s<map name=\"%s\">\n",pre.c_str(), name.c_str());
		} else {
			printf("%s<map>\n", pre.c_str());
		}
		for (i=0; i<keys.length(); i++) {
			AObject key;
			keys.get(i, key);
			AObject tmp;
			list.get(key.asString(), tmp);
			walkTree(nest+1, key.asString(), tmp);
		}
		printf("%s</map>\n",pre.c_str());
	} 

	if (list.isString()) {
		if (name.length() > 0) {
			printf("%s<string name=\"%s\">%s</string>\n",
				pre.c_str(), name.c_str(),list.asString().c_str()
			);
		} else {
			printf("%s<string>%s</string>\n",pre.c_str(), list.asString().c_str());
		}
	}
	if (list.isLong()) {
		if (name.length() > 0) {
			printf("%s<int name=\"%s\">%li</int>\n",
				pre.c_str(), name.c_str(),list.asLong()
			);
		} else {
			printf("%s<int>%li</int>\n", pre.c_str(), list.asLong());
		}
	}
	if (list.isFloat()) {
		if (name.length() > 0) {
			printf("%s<float name=\"%s\">%.2f</float>\n",
				pre.c_str(), name.c_str(),list.asFloat()
			);
		} else {
			printf("%s<float>%.2f</float>\n",pre.c_str(), list.asFloat());
		}
	}

}


string AXMLEncoder::encodeMessage(AObject& msg)
{
	int	i;

	// start a new message
	buffer = "";
	// format the message header
	printf("<obj>\n");
	walkTree(1, "", msg);
	// and close off the message
	printf("</obj>\n");
	// return result
	return buffer;
}





