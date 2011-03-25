#ifndef ERIS_POLL_WINSOCK_H
#define ERIS_POLL_WINSOCK_H 

#include <Eris/Poll.h>

#include <skstream/skstreamconfig.h>

#include <map>

namespace Eris {

class PollWinsock : public Poll, virtual public sigc::trackable
{
public:
	PollWinsock();
	virtual ~PollWinsock();

	virtual void addStream(const basic_socket*, Check);
	virtual void changeStream(const basic_socket*, Check);
	virtual void removeStream(const basic_socket*);

	static void poll(unsigned long timeout = 0);

	typedef std::map<const basic_socket*,Check> MapType;
private:
	MapType _streams;
	typedef MapType::iterator _iter;

	void doPoll(unsigned long timeout);
};

class PollDataWinsock : public PollData
{
public:
        PollDataWinsock(const PollWinsock::MapType&, bool&, unsigned long);

        virtual bool isReady(const basic_socket*);
private:
        typedef PollWinsock::MapType::const_iterator _iter;
};

} // namespace Eris

#if 0
#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdio.h>

// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#define DATA_BUFSIZE 4096

int main()
{
    //-----------------------------------------
    // Declare and initialize variables
    WSADATA wsaData = { 0 };
    int iResult = 0;
    BOOL bResult = TRUE;

    WSABUF DataBuf;
    char buffer[DATA_BUFSIZE];

    DWORD EventTotal = 0;
    DWORD RecvBytes = 0;
    DWORD Flags = 0;
    DWORD BytesTransferred = 0;

    WSAEVENT EventArray[WSA_MAXIMUM_WAIT_EVENTS];
    WSAOVERLAPPED AcceptOverlapped;
    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET AcceptSocket = INVALID_SOCKET;

    DWORD Index;

    //-----------------------------------------
    // Initialize Winsock
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        wprintf(L"WSAStartup failed: %d\n", iResult);
        return 1;
    }
    //-----------------------------------------
    // Create a listening socket bound to a local
    // IP address and the port specified
    ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ListenSocket == INVALID_SOCKET) {
        wprintf(L"socket failed with error = %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    u_short port = 27015;
    char *ip;
    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_port = htons(port);
    hostent *thisHost;

    thisHost = gethostbyname("");
    if (thisHost == NULL) {
        wprintf(L"gethostbyname failed with error = %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    ip = inet_ntoa(*(struct in_addr *) *thisHost->h_addr_list);

    service.sin_addr.s_addr = inet_addr(ip);

    //-----------------------------------------
    // Bind the listening socket to the local IP address
    // and port number
    iResult = bind(ListenSocket, (SOCKADDR *) & service, sizeof (SOCKADDR));
    if (iResult != 0) {
        wprintf(L"bind failed with error = %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    //-----------------------------------------
    // Set the socket to listen for incoming
    // connection requests
    iResult = listen(ListenSocket, 1);
    if (iResult != 0) {
        wprintf(L"listen failed with error = %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    wprintf(L"Listening...\n");

    //-----------------------------------------
    // Accept and incoming connection request
    AcceptSocket = accept(ListenSocket, NULL, NULL);
    if (AcceptSocket == INVALID_SOCKET) {
        wprintf(L"accept failed with error = %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    wprintf(L"Client Accepted...\n");

    //-----------------------------------------
    // Create an event handle and setup an overlapped structure.
    EventArray[EventTotal] = WSACreateEvent();
    if (EventArray[EventTotal] == WSA_INVALID_EVENT) {
        wprintf(L"WSACreateEvent failed with error = %d\n", WSAGetLastError());
        closesocket(AcceptSocket);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    ZeroMemory(&AcceptOverlapped, sizeof (WSAOVERLAPPED));
    AcceptOverlapped.hEvent = EventArray[EventTotal];

    DataBuf.len = DATA_BUFSIZE;
    DataBuf.buf = buffer;

    EventTotal++;

    //-----------------------------------------
    // Call WSARecv to receive data into DataBuf on 
    // the accepted socket in overlapped I/O mode
    if (WSARecv(AcceptSocket, &DataBuf, 1, &RecvBytes, &Flags, &AcceptOverlapped, NULL) ==
        SOCKET_ERROR) {
        iResult = WSAGetLastError();
        if (iResult != WSA_IO_PENDING)
            wprintf(L"WSARecv failed with error = %d\n", iResult);
    }
    //-----------------------------------------
    // Process overlapped receives on the socket
    while (1) {

        //-----------------------------------------
        // Wait for the overlapped I/O call to complete
        Index = WSAWaitForMultipleEvents(EventTotal, EventArray, FALSE, WSA_INFINITE, FALSE);

        //-----------------------------------------
        // Reset the signaled event
        bResult = WSAResetEvent(EventArray[Index - WSA_WAIT_EVENT_0]);
        if (bResult == FALSE) {
            wprintf(L"WSAResetEvent failed with error = %d\n", WSAGetLastError());
        }
        //-----------------------------------------
        // Determine the status of the overlapped event
        bResult =
            WSAGetOverlappedResult(AcceptSocket, &AcceptOverlapped, &BytesTransferred, FALSE,
                                   &Flags);
        if (bResult == FALSE) {
            wprintf(L"WSAGetOverlappedResult failed with error = %d\n", WSAGetLastError());
        }
        //-----------------------------------------
        // If the connection has been closed, close the accepted socket
        if (BytesTransferred == 0) {
            wprintf(L"Closing accept Socket %d\n", AcceptSocket);
            closesocket(ListenSocket);
            closesocket(AcceptSocket);
            WSACloseEvent(EventArray[Index - WSA_WAIT_EVENT_0]);
            WSACleanup();
            return 1;
        }
        //-----------------------------------------
        // If data has been received, echo the received data
        // from DataBuf back to the client
        iResult =
            WSASend(AcceptSocket, &DataBuf, 1, &RecvBytes, Flags, &AcceptOverlapped, NULL);
        if (iResult != 0) {
            wprintf(L"WSASend failed with error = %d\n", WSAGetLastError());
        }
        //-----------------------------------------         
        // Reset the changed flags and overlapped structure
        Flags = 0;
        ZeroMemory(&AcceptOverlapped, sizeof (WSAOVERLAPPED));

        AcceptOverlapped.hEvent = EventArray[Index - WSA_WAIT_EVENT_0];

        //-----------------------------------------
        // Reset the data buffer
        DataBuf.len = DATA_BUFSIZE;
        DataBuf.buf = buffer;
    }

    closesocket(ListenSocket);
    closesocket(AcceptSocket);
    WSACleanup();

    return 0;
}

#endif

#if 0
   WSAEventSelect(m_listeningSocket, m_acceptEvent.GetEvent(), FD_ACCEPT);

   do
   {
      for (size_t i = 0; i < numAcceptsToPost; ++i)
      {
         Accept();
      }

      m_postMoreAcceptsEvent.Reset();
      m_acceptEvent.Reset();

      HANDLE handlesToWaitFor[2];

      handlesToWaitFor[0] = m_postMoreAcceptsEvent.GetEvent();
      handlesToWaitFor[1] = m_acceptEvent.GetEvent();

      waitResult = ::WaitForMultipleObjects(2, handlesToWaitFor, false, INFINITE);

      if (waitResult != WAIT_OBJECT_0 &&
          waitResult != WAIT_OBJECT_0 + 1)
      {
         OnError(_T("CSocketServerEx::Run() - WaitForMultipleObjects: ") 
                 + GetLastErrorMessage(::GetLastError()));
      }

      if (waitResult == WAIT_OBJECT_0 + 1)
      {
         Output(_T("Accept..."));
      }
   }
   while (waitResult == WAIT_OBJECT_0 || waitResult == WAIT_OBJECT_0 + 1);
#endif

#endif // ERIS_POLL_WINSOCK_H
