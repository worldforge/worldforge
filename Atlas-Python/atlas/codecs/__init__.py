#codec factory, list of codecs, etc..

#Copyright (C) 2000,2001 by Aloril
#Copyright (C) 2002 by AIR-IX SUUNNITTELU/Ahiplan Oy

#This library is free software; you can redistribute it and/or
#modify it under the terms of the GNU Lesser General Public
#License as published by the Free Software Foundation; either
#version 2.1 of the License, or (at your option) any later version.

#This library is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#Lesser General Public License for more details.

#You should have received a copy of the GNU Lesser General Public
#License along with this library; if not, write to the Free Software
#Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


#ids: list of codec ids
#factory_dict: dictionary of codec factories
#factory_list: list of codec factories
#example usage:
#to get codec:
#codec = codec_dict["XML"]()
#to encode object:
#output_string = codec.encode(obj)
#to decode string (returns list of decodec objects)
#obj_list = codec.decode(some_string)

import xml
import xml2
import packed
import binary1
import binary2
import bach

class Codec:
    def __init__(self, id, decoder, encoder):
        self.id = id
        self.decoder = decoder
        self.encoder = encoder
    def encode(self, obj):
        return self.encoder(obj)
    def decode(self, str):
        return self.decoder(str)
    def close(self):
        return self.encoder.close()
    def set_stream_mode(self, mode=1):
        self.decoder.set_stream_mode(mode)
        self.encoder.set_stream_mode(mode)

class CodecFactory: 
    def __init__(self, id, decoder_factory, encoder_factory):
        self.id = id
        self.decoder_factory = decoder_factory
        self.encoder_factory = encoder_factory
    def __call__(self):
        return apply(Codec, (self.id, self.decoder_factory(),
                             self.encoder_factory()))

factory_list = [
    CodecFactory("Bach_beta2",
                 bach.get_decoder,
                 bach.get_encoder),
    CodecFactory("XML",
                 xml.get_decoder,
                 xml.get_encoder),
    CodecFactory("XML2_test",
                 xml2.get_parser,
                 lambda :xml2.gen_xml2),
    CodecFactory("Packed",
                 packed.get_parser,
                 packed.get_encoder),
    CodecFactory("Binary1_beta",
                 binary1.get_parser,
                 lambda :binary1.gen_binary1),
    CodecFactory("Binary2_test",
                 binary2.get_parser,
                 lambda :binary2.gen_binary2)
    ]

factory_dict = {}
ids = []
for factory in factory_list:
    factory_dict[factory.id] = factory
    ids.append(factory.id)
    def _get_codec(factory=factory):
        return factory()
    globals()["get_" + factory.id] = _get_codec

del _get_codec

def get_codec(name):
    return factory_dict[name]()

def get_default_codec():
    return factory_list[0]()
