#ifndef TEST_UTILS_H
#define TEST_UTILS_H

std::string getType(const Atlas::Message::Object &obj);

const Atlas::Message::Object
getArg(const Atlas::Message::Object &op, unsigned int i);	

/// assume that args[0] is a map, and then lookup the named value
const Atlas::Message::Object
getArg(const Atlas::Message::Object &op, const std::string &nm);

bool hasArg(const Atlas::Message::Object &op, const std::string &nm);	

const Atlas::Message::Object&
getMember(const Atlas::Message::Object &obj, unsigned int i);	

const Atlas::Message::Object&
getMember(const Atlas::Message::Object &obj, const std::string &nm);

#endif
