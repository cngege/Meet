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
#include <future>
#include <utility>
#include <format>

#define LOG(x,...) if(_printCLog){printf(std::format((std::string(x)+"\n"),__VA_ARGS__).data());}
#define LOGNON(...) if(_printCLog){printf(std::format(__VA_ARGS__).data());}

namespace meet {
    /// <summary>
    /// 
    /// </summary>
    class TCPServer {
        using ushort = unsigned short;
        using ulong = unsigned long;
    public:
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
            bool printCLog=false,
            int maxcou = 1,
            SOCKADDR_IN sockaddrin = {},
            SOCKET socket = NULL,
            const std::function<Error(const IN_ADDR&, ushort, const clientInfo&, const TCPServer*)>& onNewClientConnect = [](const IN_ADDR&, ushort, const clientInfo&, const TCPServer*) ->Error {return Error::noError; }
        ) :_onNewClientConnect(onNewClientConnect)
            , _socket(socket)
            , _Clinum(0)
            , _Maxcou(maxcou)
            , _endServer(false)
            , _printCLog(printCLog) {
            _sin.sin_family = 0;
            _sin.sin_port = htons(0);
            _sin.sin_addr.S_un.S_addr = 0;
        }
        ~TCPServer() {
            _endServer = true;
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
            bool printCLog,
            WSADATA& wsaDat,
            int maxcou = 1,
            ulong S_addr = INADDR_ANY,
            ushort port = rand() % 65536,
            int iptype = AF_INET,
            ushort versionRequested = MAKEWORD(2, 2),
            const std::function<Error(const IN_ADDR&, ushort, const clientInfo&,const TCPServer*)>& onNewClientConnect = [](const IN_ADDR&, ushort, const clientInfo&, const TCPServer*) ->Error {return Error::noError; }
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
            if (::bind(_socket, (LPSOCKADDR)&_sin, sizeof(_sin)) == SOCKET_ERROR) {
                return Error::bindError;
            }
            return Error::noError;
        }//Error initServer(WSADATA& wsaDat,int maxcou = 1,ulong S_addr = INADDR_ANY,ushort port = rand() % 65536,int iptype = AF_INET,ushort versionRequested = MAKEWORD(2, 2),const std::function<Error(const IN_ADDR&, ushort, const TCPServer&)>& onNewClientConnect = [](const IN_ADDR&, ushort, const TCPServer&) ->Error {return Error::noError; })

        /// <summary>
        /// 
        /// </summary>
        /// <param name="backlog"></param>
        /// <returns></returns>
        Error startServer(int backlog = 5) {
            LOG("strating server.");
            LOG("Ready to listen,backlog={0}.", backlog);
            if (::listen(_socket, backlog) == SOCKET_ERROR) {
                LOG("listen Error!");
                return Error::listenError;
            }
            clientInfo* ci = new clientInfo[_Maxcou];
            int nAddrlen;
            LOG("Start a loop to accept connections...");
            while (true) {
                sockaddr_in remoteAddr;
                if (_endServer) {
                    LOG("END server!");
                    return Error::noError;
                }
                nAddrlen = sizeof(remoteAddr);
                LOG("start accept,addrlen={0}.", nAddrlen);
                SOCKET c_socket = ::accept(_socket, (SOCKADDR*)&remoteAddr, &nAddrlen);
                char Buf[20] = { '\0' };
                LOG("A connection was accepted!IP:{0},Port:{1},", ::inet_ntop(AF_INET, (void*)&remoteAddr.sin_addr, Buf, 16), remoteAddr.sin_port);
                if (_Clinum < _Maxcou) {
                    if (c_socket == INVALID_SOCKET) {
                        continue;
                    }
                    ci[_Clinum].clientSocket = c_socket;
                    LOG("Ready to start threads for {0}:{1}!", ::inet_ntop(AF_INET, (void*)&remoteAddr.sin_addr, Buf, 16), remoteAddr.sin_port);
                    
                    std::jthread t{
                        [&](TCPServer* _this,const IN_ADDR& addr, ushort port,const clientInfo& client) {
                    _onNewClientConnect(addr,port,client,_this);
                    closesocket(client.clientSocket);
                    _this->setClinum((_this->getClinum() - 1));
                    char sendBuf[20] = { '\0' };
                    LOG("End of thread {0}:{1}", inet_ntop(AF_INET, (void*)&addr, sendBuf, 16),port);
                    },
                        this,
                        remoteAddr.sin_addr,
                        remoteAddr.sin_port,
                        ci[_Clinum]
                    };
                    t.detach();
                    LOG("Already for {0}:{1} start thread! ", ::inet_ntop(AF_INET, (void*)&remoteAddr.sin_addr, Buf, 16), remoteAddr.sin_port);
                    _Clinum++;
                    LOG("Number of current connections:{0}", _Clinum);
                }//if (_Clinum < _Maxcou)
                else {
                    LOG("Error!The number of connections exceeded the maximum value:{0}", _Maxcou);
                    //return Error::theMaximumNumberOfConnectionsHasBeenReached;
                }
            }//while (true)
            LOG("unkError!");
            return Error::unkError;
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
        std::function<Error(const IN_ADDR&, ushort, const clientInfo&, const TCPServer*)> getonNewClientConnect() const {
            return _onNewClientConnect;
        }

        /// <summary>
        /// set get onNewClientConnect function
        /// </summary>
        /// <param name="onNewClientConnect">new get onNewClientConnect function</param>
        void setonNewClientConnect(std::function<Error(const IN_ADDR&, ushort, const clientInfo&, const TCPServer*)> onNewClientConnect) {
            _onNewClientConnect = onNewClientConnect;
        }

        /// <summary>
        /// get _Clinum
        /// </summary>
        /// <returns>_Clinum</returns>
        int getClinum() const {
            return _Clinum;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="Clinum"></param>
        void setClinum(int Clinum) {
            _Clinum = Clinum;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        bool isEnd() const{
            return _endServer;
        }
    public:
        /// <summary>
        /// 
        /// </summary>
        /// <param name="str"></param>
        /// <param name="s"></param>
        /// <returns></returns>
        static int send(std::string str, SOCKET s, int f = 0) {
            return ::send(s, str.data(), str.length(), f);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="data"></param>
        /// <param name="s"></param>
        /// <param name="buffsize"></param>
        /// <param name="f"></param>
        /// <returns></returns>
        static Error recv(std::string& data, SOCKET s, int buffsize = 255,int f = 0) {
            char* buff = new char[buffsize];
            int ret = ::recv(s, buff, buffsize, f);
            if (ret > 0){
                data = buff;
            }
            else {
                return Error::theClientIsDisconnected;
            }
            return Error::noError;
        }

    protected:
    private:
    private:
        std::function<Error(const IN_ADDR&, ushort, const clientInfo&,const TCPServer*)> _onNewClientConnect;
        SOCKADDR_IN _sin;
        SOCKET _socket;
        int _Clinum;
        int _Maxcou;
        bool _endServer;
        bool _printCLog;
    };//class TCPServer

}//namespace meet

#undef LOG

#endif //!___MIRACLEFOREST_MEET_TCPSERVER___