//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#ifndef STUB_Link_Link
#define STUB_Link_Link
Link::Link(CommSocket & commSocket, RouterId id)
    : Router(id)
    , m_encoder(nullptr)
    ,m_commSocket(commSocket)
{

}
#endif //STUB_Link_Link
