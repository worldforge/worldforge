// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#include "MorphObject.h"

using namespace Atlas::Generic;
using namespace std;

namespace Atlas { namespace Message {

Token<MorphObject::Int> MorphObject::IntToken;
Token<MorphObject::Float> MorphObject::FloatToken;
Token<MorphObject::String> MorphObject::StringToken;
Token<MorphObject::Map> MorphObject::MapToken;
Token<MorphObject::List> MorphObject::ListToken;

} } // namespace Atlas::Message
