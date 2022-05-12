/*
*
* Copyright(C) 2022 MiracleForest Studio. All Rights Reserved.
*
* @filename:tcpserver.hpp
* @creation time:2022.5.10.12:49
* @created by:Lovelylavender4
* -----------------------------------------------------------------------------
*
*
* -----------------------------------------------------------------------------
* If you have contact or find bugs,
* you can go to Github or email (MiracleForest@Outlook.com) to give feedback.
* We will try to do our best!
*/
#ifndef ___MIRACLEFOREST_MEET_TCPSERVER___
#define ___MIRACLEFOREST_MEET_TCPSERVER___

#include "ip.hpp"
#include <map>
#include <functional>

namespace meet {
    /// <summary>
    /// 
    /// </summary>
    class TCPServer {
        using ushort = unsigned short;
        using ulong = unsigned long;
    public:
        TCPServer(
            const std::function<Error(const IP&, ushort, const TCPServer&)>& onNewClientConnect = [](const IP&, ushort, const TCPServer&) ->Error {return Error::noError; }
        ):_onNewClientConnect(onNewClientConnect) {}
        ~TCPServer(){}
    public:

        Error initServer(
            WSADATA& wsaDat,
            ulong S_addr = INADDR_ANY,
            ushort port = rand() % 65536,
            int iptype = AF_INET,
            ushort versionRequested = MAKEWORD(2, 2),
            const std::function<Error(const IP&, ushort, const TCPServer&)>& onNewClientConnect = [](const IP&, ushort, const TCPServer&) ->Error {return Error::noError; }
        ){
            _onNewClientConnect = onNewClientConnect;
            if (WSAStartup(versionRequested, &wsaDat) != 0) {
                return Error::initializationWinsockFailed;
            }
            int af;
            if (iptype == AF_INET) {
                af = AF_INET;
            }
            else if (iptype == AF_INET6) {
                af = AF_INET6;
            }
            slisten = socket(af, SOCK_STREAM, IPPROTO_TCP);
            if (slisten == INVALID_SOCKET) {
                return Error::socketError;
            }
            sin.sin_family = af;
            sin.sin_port = htons(port);
            sin.sin_addr.S_un.S_addr = S_addr;
            if (bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR){
                return Error::bindError;
            }
        }
    public:

    protected:
    private:
        std::function<Error(const IP&,ushort,const TCPServer&)> _onNewClientConnect;
        SOCKADDR_IN sin;
        SOCKET slisten;
    };//class TCPServer

}//namespace meet

#endif //!___MIRACLEFOREST_MEET_TCPSERVER___