#include <fstream>
#include <iostream>

#include "../src/Codecs/XML.h"
#include "../src/Codecs/Bach.h"
#include "../src/Codecs/Packed.h"
//#include "../src/Codecs/Binary.h"
#include "../src/Message/QueuedDecoder.h"
#include "../src/Message/MEncoder.h"

Atlas::Codec<std::iostream>* getCodec(std::string type, std::iostream &stream, Atlas::Message::DecoderBase* decoder)
{
    if (type == "XML")
        return new Atlas::Codecs::XML(stream, decoder);
    else if (type == "Bach")
        return new Atlas::Codecs::Bach(stream, decoder);
    else if (type == "Packed")
        return new Atlas::Codecs::Packed(stream, decoder);
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

void convert(std::string file_in, std::string codec_in, std::string file_out, std::string codec_out)
{
    std::cout << "CONVERT " << codec_in << " to " << codec_out << std::endl;

    std::filebuf fb_in, fb_out;

    fb_in.open( file_in.c_str(), std::ios::in );
    fb_out.open( file_out.c_str(), std::ios::out );

    std::iostream in( &fb_in );
    std::iostream out( &fb_out );

    std::cout << "Reading... ";

    Atlas::Message::QueuedDecoder decoder;
    Atlas::Codec<std::iostream> *inCodec = getCodec(codec_in, in, &decoder);
    while (!in.eof())
        inCodec->poll(true);

    std::cout << "done." << std::endl;
    std::cout << "Writing... ";

    Atlas::Codec<std::iostream> *outCodec = getCodec(codec_out, out, NULL);
    Atlas::Message::Encoder encoder(outCodec);
    encoder.streamBegin();
    while (decoder.queueSize() > 0 ) {
        Atlas::Message::Element msg(decoder.pop());
        encoder.streamMessage(msg);
    }
    encoder.streamEnd();

    std::cout << "done." << std::endl;

    fb_out.close();
    fb_in.close();
}

int main( int argc, char** argv )
{
    // parse command line here
    if (argc!=4)
    {
        std::cout << "usage: atlas_convert <OPTION> <input file> <output file>" << std::endl << std::endl;
        std::cout << "options: --xml2bach" << std::endl;
        std::cout << "         --xml2packed" << std::endl;
        std::cout << "         --xml2binary" << std::endl;
        std::cout << "         --bach2xml" << std::endl;
        std::cout << "         --bach2packed" << std::endl;
        std::cout << "         --bach2binary" << std::endl;
        std::cout << "         --packed2xml" << std::endl;
        std::cout << "         --packed2bach" << std::endl;
        std::cout << "         --packed2binary" << std::endl;
        std::cout << "         --binary2xml" << std::endl;
        std::cout << "         --binary2bach" << std::endl;
        std::cout << "         --binary2packed" << std::endl;
        return 0;
    }

    std::string file_in(argv[2]);
    std::string file_out(argv[3]);
    std::string codec_in, codec_out ;

    for (int i = 0; i < 3; i++)
    {
        std::string temp(argv[i]);

        if (temp=="--xml2bach")
        {
            codec_in = "XML";
            codec_out= "Bach";
        }
        else if (temp=="--xml2packed")
        {
            codec_in = "XML";
            codec_out = "Packed";
        }
        else if (temp=="--xml2binary")
        {
            codec_in = "XML";
            codec_out = "Binary";
        }
        else if (temp=="--bach2xml")
        {
            codec_in = "Bach";
            codec_out = "XML";
        }
        else if (temp=="--bach2packed")
        {
            codec_in = "Bach";
            codec_out = "Packed";
        }
        else if (temp=="--bach2binary")
        {
            codec_in = "Bach";
            codec_out = "Binary";
        }
        else if (temp=="--packed2xml")
        {
            codec_in = "Packed";
            codec_out = "XML";
        }
        else if (temp=="--packed2bach")
        {
            codec_in = "Packed";
            codec_out = "Bach";
        }
        else if (temp=="--packed2binary")
        {
            codec_in = "Packed";
            codec_out = "Binary";
        }
        else if (temp=="--binary2xml")
        {
            codec_in = "Binary";
            codec_out = "XML";
        }
        else if (temp=="--binary2bach")
        {
            codec_in = "Binary";
            codec_out = "Bach";
        }
        else if (temp=="--binary2packed")
        {
            codec_in = "Binary";
            codec_out = "Packed";
        }
    }

    // convert file
    convert(file_in, codec_in, file_out, codec_out);

    return 0;
}
