// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day, Stefanus Du Toit

// $Id$

#ifndef ATLAS_MESSAGE_LAYER_H
#define ATLAS_MESSAGE_LAYER_H

#include <Atlas/Message/Encoder.h>

namespace Atlas { namespace Message {

/** Layer that combines Atlas::Message::Encoder and a decoder.
 *
 * This class simply combines Atlas::Message::Encoder and a decoder of your
 * choice (derived from DecoderBase) into a single layer.
 * 
 * @see DecoderBase
 * @see Encoder
 * @see Object
*/

template <class Decoder>
class Layer : public Encoder, public Decoder
{
public:
    Layer(Atlas::Bridge* b) : Encoder(b), Decoder() { }
    virtual ~Layer() { }
};

} } // namespace Atlas::Message

#endif
