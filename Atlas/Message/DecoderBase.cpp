// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#include "DecoderBase.h"

namespace Atlas { namespace Message {

DecoderBase::DecoderBase()
{
}

void DecoderBase::StreamBegin()
{
}

void DecoderBase::StreamMessage(const Map&)
{
}

void DecoderBase::StreamEnd()
{
}

void DecoderBase::MapItem(const std::string& name, const Map&)
{
}

void DecoderBase::MapItem(const std::string& name, const List&)
{
}
    
void DecoderBase::MapItem(const std::string& name, int)
{
}
    
void DecoderBase::MapItem(const std::string& name, double)
{
}
    
void DecoderBase::MapItem(const std::string& name, const std::string&)
{
}

void DecoderBase::MapEnd()
{
}
  
void DecoderBase::ListItem(const Map&)
{
}
    
void DecoderBase::ListItem(const List&)
{
}
    
void DecoderBase::ListItem(int)
{
}

void DecoderBase::ListItem(double)
{
}
    
void DecoderBase::ListItem(const std::string&)
{
}
    
void DecoderBase::ListEnd()
{
}
    
} } // namespace Atlas::Message
