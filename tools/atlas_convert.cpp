// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2002 Michael Koch

#include <Atlas/Codecs/XML.h>
#include <Atlas/Codecs/Bach.h>
#include <Atlas/Codecs/Packed.h>
//#include <Atlas/Codecs/Binary.h>
#include <Atlas/Message/QueuedDecoder.h>
#include <Atlas/Message/MEncoder.h>
#include <Atlas/Message/Element.h>
#include <Atlas/Formatter.h>

#include <fstream>
#include <iostream>
#include <cstdlib>

#include <unistd.h>

int option_format = 0;
int option_spacing = -1;

Atlas::Codec * getCodec(std::string type, std::iostream &stream, Atlas::Message::DecoderBase& decoder)
{
    if (type == "XML")
        return new Atlas::Codecs::XML(stream, stream, decoder);
    else if (type == "Bach")
        return new Atlas::Codecs::Bach(stream, stream, decoder);
    else if (type == "Packed")
        return new Atlas::Codecs::Packed(stream, stream, decoder);
/*
    else if (type == "Binary")
        return new Atlas::Codecs::Binary(stream, decoder);
*/
    else
    {
        std::cout << "Unknown CODEC required!" << std::endl;
        exit(0);
    }
}

int convert(const std::string & file_in, const std::string & codec_in,
            const std::string & file_out, const std::string & codec_out)
{
    std::cout << "Convert " << codec_in << " to " << codec_out << std::endl;

    std::fstream in, out;

    in.open( file_in.c_str(), std::ios::in );

    if (!in.is_open()) {
        std::cerr << "Unable to open " << file_in << " for input"
                  << std::endl << std::flush;
        return 1;
    }

    out.open( file_out.c_str(), std::ios::out );

    if (!out.is_open()) {
        std::cerr << "Unable to open " << file_out << " for output"
                  << std::endl << std::flush;
        return 1;
    }

    std::cout << "Reading... ";

    Atlas::Message::QueuedDecoder decoder;
    Atlas::Codec *inCodec = getCodec(codec_in, in, decoder);
    while (!in.eof()) {
        inCodec->poll();
    }

    std::cout << "done." << std::endl;
    std::cout << "Writing... ";

    Atlas::Codec * outCodec = getCodec(codec_out, out, decoder);
    Atlas::Bridge * bridge;

    if (option_format) {
        Atlas::Formatter * format;
        bridge = format = new Atlas::Formatter(out, *outCodec);
        if (option_spacing != -1) {
            format->setSpacing(option_spacing);
        }
    } else {
        bridge = outCodec;
    }

    Atlas::Message::Encoder encoder(*bridge);
    encoder.streamBegin();
    while (decoder.queueSize() > 0 ) {
        Atlas::Message::MapType msg(decoder.popMessage());
        encoder.streamMessageElement(msg);
    }
    encoder.streamEnd();

    std::cout << "done." << std::endl;

    out.close();
    in.close();

    return 0;
}

void usage(const char * program)
{
    std::cout << "usage: " << program
              << " [-i infile] [-o outfile] <input file> <output file>"
              << std::endl;
    std::cout << "Supported Codecs: XML Back Packed"
              << std::endl << std::flush;
    return;
}

int main( int argc, char** argv )
{
    std::string codec_in("XML"),
                codec_out("Bach");

    while (1) {
        int c = getopt(argc, argv, "fs:i:o:");
        if (c == -1) {
            break;
        } else if (c == 'f') {
            option_format = 1;
        } else if (c == 's') {
            option_spacing = strtol(optarg, NULL, 0);
        } else if (c == 'i') {
            codec_in = optarg;
        } else if (c == 'o') {
            codec_out = optarg;
        }
    }

    if ((argc - optind) != 2) {
        usage(argv[0]);
        return 1;
    }

    std::string file_in(argv[argc - 2]);
    std::string file_out(argv[argc - 1]);

    std::cout << "Reading from " << file_in << " to " << file_out << std::endl << std::flush;

    // convert file
    return convert(file_in, codec_in, file_out, codec_out);
}
