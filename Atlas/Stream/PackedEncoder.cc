// This file may be redistributed and modified under the terms of the
// GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

// $Log$
// Revision 1.3  2000-02-22 05:31:22  mike
// Fixed a quick namespace bug relating to use of unqualified Atlas::Object.
//
// Revision 1.2  2000/02/22 04:22:19  mike
// Begun implementation of PackedEncoder based on new Encoder interface.
//

#include "PackedEncoder.h"

/*
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
*/

void Begin(Container);
void Begin(const std::string& name, Container);

void Send(int);
void Send(float);
void Send(const std::string&);
void Send(const Atlas::Object&);

void Send(const std::string& name, int);
void Send(const std::string& name, float);
void Send(const std::string& name, const std::string&);
void Send(const std::string& name, const Atlas::Object&);

void End();
