#ifndef TEST_UTILS_H
#define TEST_UTILS_H

std::string getType(const Atlas::Message::Element &obj);

const Atlas::Message::Element
getArg(const Atlas::Message::Element &op, unsigned int i);	

/// assume that args[0] is a map, and then lookup the named value
const Atlas::Message::Element
getArg(const Atlas::Message::Element &op, const std::string &nm);

bool hasArg(const Atlas::Message::Element &op, const std::string &nm);	

const Atlas::Message::Element&
getMember(const Atlas::Message::Element &obj, unsigned int i);	

const Atlas::Message::Element&
getMember(const Atlas::Message::Element &obj, const std::string &nm);

#endif
