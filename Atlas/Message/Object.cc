// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#include "Object.h"

using namespace Atlas::Generic;
using namespace std;

namespace Atlas { namespace Message {

Token<Object::IntType> Object::Int;
Token<Object::FloatType> Object::Float;
Token<Object::StringType> Object::String;
Token<Object::MapType> Object::Map;
Token<Object::ListType> Object::List;

} } // namespace Atlas::Message
