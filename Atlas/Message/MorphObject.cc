// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#include "MorphObject.h"

using namespace Atlas::Generic;
using namespace std;

namespace Atlas { namespace Message {

static Token<MorphObject::Int> MorphObject::IntToken;
static Token<MorphObject::Float> MorphObject::FloatToken;
static Token<MorphObject::String> MorphObject::StringToken;
static Token<MorphObject::Map> MorphObject::MapToken;
static Token<MorphObject::List> MorphObject::ListToken;

} } // namespace Atlas::Message
