/*
        AtlasXMLEncoder.cpp
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#include "XMLEncoder.h"

//#include <map.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <string>


void AXMLEncoder::walkTree(int nest, string name, const AObject& list)
{
        /* note that printf if member method, not normal printf function */
	int	i;
	string	buf;
	string	pre;
	string	nam;

	for (int j=0; j<nest; j++) {
		pre.append("    ");
	}

	if (name.length() > 0) {
		char buf[80];
		sprintf(buf, " name=\"%s\"", name.c_str());
		nam.append(buf);
	}

	if (list.isList()) {
		printf("%s<list%s>\n", pre.c_str(), nam.c_str());
		for (i=0; i<list.length(); i++) {
			AObject tmp;
			list.get(i, tmp);
			walkTree(nest+1, "", tmp);
		}
		printf("%s</list>\n",pre.c_str());
	} 

	if (list.isURIList()) {
		printf("%s<uri_list%s>\n", pre.c_str(), nam.c_str());
		for (i=0; i<list.length(); i++) {
			AObject tmp;
			list.get(i, tmp);
			walkTree(nest+1, "", tmp);
		}
		printf("%s</uri_list>\n",pre.c_str());
	} 
	if (list.isIntList()) {
		printf("%s<int_list%s>\n", pre.c_str(), nam.c_str());
		for (i=0; i<list.length(); i++) {
			AObject tmp;
			list.get(i, tmp);
			walkTree(nest+1, "", tmp);
		}
		printf("%s</int_list>\n",pre.c_str());
	} 
	if (list.isLongList()) {
		printf("%s<long_list%s>\n", pre.c_str(), nam.c_str());
		for (i=0; i<list.length(); i++) {
			AObject tmp;
			list.get(i, tmp);
			walkTree(nest+1, "", tmp);
		}
		printf("%s</long_list>\n",pre.c_str());
	} 
	if (list.isFloatList()) {
		printf("%s<float_list%s>\n", pre.c_str(), nam.c_str());
		for (i=0; i<list.length(); i++) {
			AObject tmp;
			list.get(i, tmp);
			walkTree(nest+1, "", tmp);
		}
		printf("%s</float_list>\n",pre.c_str());
	} 
	if (list.isStringList()) {
		printf("%s<string_list%s>\n", pre.c_str(), nam.c_str());
		for (i=0; i<list.length(); i++) {
			AObject tmp;
			list.get(i, tmp);
			walkTree(nest+1, "", tmp);
		}
		printf("%s</string_list>\n",pre.c_str());
	} 

	if (list.isMap()) {
		AObject keys = list.keys();
		printf("%s<map%s>\n", pre.c_str(), nam.c_str());
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
		printf("%s<string%s>%s</string>\n",
			pre.c_str(), nam.c_str(),list.asString().c_str()
		);
	}
	if (list.isURI()) {
		printf("%s<uri%s>%s</uri>\n",
			pre.c_str(), nam.c_str(),list.getURIPath().asString().c_str()
		);
	}
	if (list.isInt()) {
		printf("%s<int%s>%li</int>\n",
			pre.c_str(), nam.c_str(),list.asInt()
		);
	}
	if (list.isLong()) {
		printf("%s<long%s>%li</long>\n",
			pre.c_str(), nam.c_str(),list.asLong()
		);
	}
	if (list.isFloat()) {
		printf("%s<float%s>%.2f</float>\n",
			pre.c_str(), nam.c_str(),list.asFloat()
		);
	}
}


string AXMLEncoder::encodeMessage(const AObject& msg)
{
	//int	i;

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





