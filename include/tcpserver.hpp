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

        typedef struct clientInfo
        {
            SOCKET clientSocket;
        }clientInfo;

    public:
        /// <summary>
        /// 
        /// </summary>
        /// <param name="maxcou"></param>
        /// <param name="sockaddrin"></param>
        /// <param name="socket"></param>
        /// <param name="onNewClientConnect"></param>
        TCPServer(
            int maxcou = 1,
            SOCKADDR_IN sockaddrin = {},
            SOCKET socket = NULL,
            const std::function<Error(const IN_ADDR&, ushort, const TCPServer&)>& onNewClientConnect = [](const IN_ADDR&, ushort, const TCPServer&) ->Error {return Error::noError; }
        ) :_onNewClientConnect(onNewClientConnect)
            , _socket(socket)
            , _Clinum(0)
            , _Maxcou(maxcou) {
            _sin.sin_family = 0;
            _sin.sin_port = htons(0);
            _sin.sin_addr.S_un.S_addr = 0;
        }
        ~TCPServer() {
            closesocket(_socket);
            WSACleanup();
        }
    public:
        /// <summary>
        /// 
        /// </summary>
        /// <param name="wsaDat"></param>
        /// <param name="maxcou"></param>
        /// <param name="S_addr"></param>
        /// <param name="port"></param>
        /// <param name="iptype"></param>
        /// <param name="versionRequested"></param>
        /// <param name="onNewClientConnect"></param>
        /// <returns></returns>
        Error initServer(
            WSADATA& wsaDat,
            int maxcou = 1,
            ulong S_addr = INADDR_ANY,
            ushort port = rand() % 65536,
            int iptype = AF_INET,
            ushort versionRequested = MAKEWORD(2, 2),
            const std::function<Error(const IN_ADDR&, ushort, const TCPServer&)>& onNewClientConnect = [](const IN_ADDR&, ushort, const TCPServer&) ->Error {return Error::noError; }
        ) {
            _Maxcou = maxcou;
            _onNewClientConnect = onNewClientConnect;
            if (WSAStartup(versionRequested, &wsaDat) != 0) {
                return Error::initializationWinsockFailed;
            }
            int af = AF_INET;
            if (iptype == AF_INET6) {
                af = AF_INET6;
            }
            _socket = socket(af, SOCK_STREAM, IPPROTO_TCP);
            if (_socket == INVALID_SOCKET) {
                return Error::socketError;
            }
            _sin.sin_family = af;
            _sin.sin_port = htons(port);
            _sin.sin_addr.S_un.S_addr = S_addr;
            if (bind(_socket, (LPSOCKADDR)&_sin, sizeof(_sin)) == SOCKET_ERROR) {
                return Error::bindError;
            }
            return Error::noError;
        }//Error initServer(WSADATA& wsaDat,int maxcou = 1,ulong S_addr = INADDR_ANY,ushort port = rand() % 65536,int iptype = AF_INET,ushort versionRequested = MAKEWORD(2, 2),const std::function<Error(const IN_ADDR&, ushort, const TCPServer&)>& onNewClientConnect = [](const IN_ADDR&, ushort, const TCPServer&) ->Error {return Error::noError; })

        Error startServer(int backlog = 5) {
            if (listen(_socket, backlog) == SOCKET_ERROR) {
                return Error::listenError;
            }
            clientInfo* ci = new clientInfo[_Maxcou];

            while (true) {
                sockaddr_in remoteAddr;
                int nAddrlen = sizeof(remoteAddr);
                SOCKET c_socket = accept(_socket, (SOCKADDR*)&remoteAddr, &nAddrlen);;
                if (_Clinum < _Maxcou) {
                    if (c_socket == INVALID_SOCKET) {
                        continue;
                    }
                    ci[_Clinum].clientSocket = c_socket;
                    std::thread t(
                        _onNewClientConnect,
                        (remoteAddr.sin_addr),
                        (remoteAddr.sin_port),
                        (ci[_Clinum].clientSocket)
                    );
                    t.detach();
                    _Clinum++;
                }//if (_Clinum < _Maxcou)
                else {
                    return Error::theMaximumNumberOfConnectionsHasBeenReached;
                }
            }//while (true)

        }//Error startServer(int backlog = 5)

    public:
        /// <summary>
        /// get SOCKADDR_IN
        /// </summary>
        /// <returns>SOCKADDR_IN</returns>
        SOCKADDR_IN getSin() const {
            return _sin;
        }
        /// <summary>
        /// set SOCKADDR_IN
        /// </summary>
        /// <param name="sin">new SOCKADDR_IN</param>
        void setSin(SOCKADDR_IN sin) {
            _sin = sin;
        }
        /// <summary>
        /// get SOCKET
        /// </summary>
        /// <returns>SOCKET</returns>
        SOCKET getSocket() const {
            return _socket;
        }
        /// <summary>
        /// set SOCKET
        /// </summary>
        /// <returns>new SOCKET</returns>
        int getMaxcou() const {
            return _Maxcou;
        }
        /// <summary>
        /// set Maxcou
        /// </summary>
        /// <param name="Maxcou">new Maxcou</param>
        void setMaxcou(int Maxcou) {
            _Maxcou = Maxcou;
        }
        /// <summary>
        /// get onNewClientConnect function
        /// </summary>
        /// <returns>onNewClientConnect function</returns>
        std::function<Error(const IN_ADDR&, ushort, const TCPServer&)> getonNewClientConnect() const {
            return _onNewClientConnect;
        }
        /// <summary>
        /// set get onNewClientConnect function
        /// </summary>
        /// <param name="onNewClientConnect">new get onNewClientConnect function</param>
        void setonNewClientConnect(std::function<Error(const IN_ADDR&, ushort, const TCPServer&)> onNewClientConnect) {
            _onNewClientConnect = onNewClientConnect;
        }
        /// <summary>
        /// get _Clinum
        /// </summary>
        /// <returns>_Clinum</returns>
        int getClinum() {
            return _Clinum;
        }
    protected:
    private:
        std::function<Error(const IN_ADDR&, ushort, const TCPServer&)> _onNewClientConnect;
        SOCKADDR_IN _sin;
        SOCKET _socket;
        int _Clinum;
        int _Maxcou;
    };//class TCPServer

}//namespace meet

#endif //!___MIRACLEFOREST_MEET_TCPSERVER___