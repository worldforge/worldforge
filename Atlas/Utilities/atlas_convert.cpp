#include <fstream>
#include <iostream>

#include "../src/Codecs/XML.h"
#include "../src/Codecs/Bach.h"
#include "../src/Codecs/Packed.h"
//#include "../src/Codecs/Binary.h"
#include "../src/Message/QueuedDecoder.h"
#include "../src/Message/MEncoder.h"

Atlas::Codec<std::iostream>* getCodec(std::string type, iostream &stream, Atlas::Message::DecoderBase* decoder)
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
        cout << "Unknown CODEC required!" << endl;
        exit(0);
    }
}

void convert(std::string file_in, std::string codec_in, std::string file_out, std::string codec_out)
{
    cout << "CONVERT " << codec_in << " to " << codec_out << endl;

    filebuf fb_in, fb_out;

    fb_in.open( file_in.c_str(), ios::in );
    fb_out.open( file_out.c_str(), ios::out );

    std::iostream in( &fb_in );
    std::iostream out( &fb_out );

    cout << "Reading... ";

    Atlas::Message::QueuedDecoder decoder;
    Atlas::Codec<std::iostream> *inCodec = getCodec(codec_in, in, &decoder);
    while (!in.eof())
        inCodec->poll(true);

    cout << "done." << endl;
    cout << "Writing... ";

    Atlas::Codec<std::iostream> *outCodec = getCodec(codec_out, out, NULL);
    Atlas::Message::Encoder encoder(outCodec);
    encoder.streamBegin();
    while (decoder.queueSize() > 0 ) {
        Atlas::Message::Object msg(decoder.pop());
        encoder.streamMessage(msg);
    }
    encoder.streamEnd();

    cout << "done." << endl;

    fb_out.close();
    fb_in.close();
}

int main( int argc, char** argv )
{
    // parse command line here
    if (argc!=4)
    {
        cout << "usage: atlas_convert <OPTION> <input file> <output file>" << endl << endl;
        cout << "options: --xml2bach" << endl;
        cout << "         --xml2packed" << endl;
        cout << "         --xml2binary" << endl;
        cout << "         --bach2xml" << endl;
        cout << "         --bach2packed" << endl;
        cout << "         --bach2binary" << endl;
        cout << "         --packed2xml" << endl;
        cout << "         --packed2bach" << endl;
        cout << "         --packed2binary" << endl;
        cout << "         --binary2xml" << endl;
        cout << "         --binary2bach" << endl;
        cout << "         --binary2packed" << endl;
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
