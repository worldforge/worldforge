class Codec
{
public:
    /// Construct a Codec for a specific Protocol
    Codec( Protocol* );

    /// Convert an Object to a Stram
    std::string      encodeMessage( const Object& );
    /// Return the length of a Stream that may contain \0 characters
    int         encodedLength();
    /// Push streaming data to a Protocol Decoder for parsing
    void        feedStream( const std::string& );
    /// Fetch a reconstructed Object from the Protocol Decoder
    Object&     getMessage();
    /// Return TRUE if the Protocol Decoder has a complete Object
    bool        hasMessage();
    /// Release internal storage for the last Object recieved
    void        freeMessage();
};
