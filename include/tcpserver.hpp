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
#include <thread>
#include <list>

namespace meet {

    /// <summary>
    /// 
    /// </summary>
    class TCPServer {
        using ushort = unsigned short;
        using ulong = unsigned long;

        using NewClientConnectEvent = void(__fastcall*)(IP, ushort, SOCKET);
        using ClientDisConnectEvent = void(__fastcall*)(IP, ushort);
        using RecvDataEvent = void(__fastcall*)(IP, ushort, SOCKET, ULONG64, const char*);
        using RecvErrorEvent = void(__fastcall*)(IP, ushort, SOCKET, int);
    public:
        typedef struct ClientS
        {
            SOCKET clientSocket;
            IP addr;
            ushort port;
        }ClientList;

    public:
        //TODO 监听地址 监听端口 最大连接数量
        TCPServer(IP addr, ushort port, int maxConnect) :_listenAddr(addr), _listenPort(port), _maxCount(maxConnect) {}
        ~TCPServer() {
            if (_socket) {
                shutdown(_socket, SD_BOTH);
                closesocket(_socket);
                WSACleanup();
            }
            _serverRunning = false;
        }
    public:


        Error Listen() {
            if (_serverRunning) {
                return Error::serverIsStarted;
            }

            if (WSAStartup(_versionRequested, &_wsaDat) != 0) {
                return Error::initializationWinsockFailed;
            }
            memset(&_sock, 0, sizeof(_sock));
            _sock.sin_family = (_listenAddr.IPFamily == Family::IPV4) ? AF_INET : AF_INET6;

            _socket = socket(_sock.sin_family, SOCK_STREAM, IPPROTO_TCP);
            if (_socket == INVALID_SOCKET) {
                return Error::socketError;
            }
            _sock.sin_port = htons(_listenPort);
            _sock.sin_addr = _listenAddr.InAddr;

            if (::bind(_socket, (LPSOCKADDR)&_sock, sizeof(_sock)) == SOCKET_ERROR) {
                return Error::bindError;
            }

            if (::listen(_socket, _maxCount) == SOCKET_ERROR) {
                return Error::listenError;
            }

            _serverRunning = true;

            if (!_blockingMode) {
                u_long iMode = 1;
                ::ioctlsocket(_socket, FIONBIO, &iMode);
            }

            //创建一个线程 接收客户端的连接
            auto _connect_thread = std::thread([this]() {
                while (_serverRunning) {
                    sockaddr_in remoteAddr;
                    int nAddrlen = sizeof(remoteAddr);
                    SOCKET c_socket = ::accept(_socket, (SOCKADDR*)&remoteAddr, &nAddrlen); //默认应该会在这里阻塞
                    if (c_socket == INVALID_SOCKET) {   //无效的套接字
                        continue;
                    }

                    if (_clientList.size() < _maxCount) {

                        ClientList newClient = { .clientSocket = c_socket, .addr = IP(remoteAddr) , .port = remoteAddr.sin_port };
                        _clientList.push_back(newClient);

                        //创建线程 监听客户端传来的消息
                        auto _recv_thread = std::thread([this, c_socket, remoteAddr]() {
                            char buffer[1024];
                            memset(buffer, '\0', sizeof(buffer));
                            IP ipaddr(remoteAddr.sin_addr);
                            ushort port = remoteAddr.sin_port;

                            while (_serverRunning) {
                                int recvbytecount;
                                if ((recvbytecount = ::recv(c_socket, buffer, sizeof(buffer), 0)) <= 0) {
                                    //Return 0 Network Outage
                                    if (recvbytecount == 0) {
                                        removeClientFromClientList(ipaddr,port);
                                        if (_onClientDisConnectEvent != NULL) {
                                            _onClientDisConnectEvent(ipaddr,port);
                                        }
                                        break;
                                    }

                                    //阻塞模式
                                    if (_blockingMode) {
                                        if (recvbytecount == SOCKET_ERROR) {
                                            if (_recvErrorEvent != NULL) {
                                                _recvErrorEvent(ipaddr,port,c_socket,recvbytecount);
                                            }
                                        }
                                    }

                                }//if ((recvbytecount = recv(c->sockfd, buffer, sizeof(buffer), 0)) <= 0)

                                //接收数据 触发回调
                                else {
                                    if (_recvDataEvent != NULL) {
                                        _recvDataEvent(ipaddr,port,c_socket,recvbytecount, buffer);
                                    }
                                    memset(buffer, '\0', sizeof(buffer));
                                }
                            }
                        });
                        _recv_thread.detach();

                        //触发有客户端连接的回调
                        if (_onNewClientConnectEvent != NULL) {
                            IP ipaddr(remoteAddr.sin_addr);
                            _onNewClientConnectEvent(ipaddr, remoteAddr.sin_port, c_socket);
                        }
                        

                    }
                    else {
                        // 连接总数超过限制 断开这个客户端的连接
                        shutdown(c_socket, SD_BOTH);
                        closesocket(c_socket);
                    }
                }
            });
            _connect_thread.detach();
            return Error::noError;
        }

    public:

        /// <summary>
        /// 设置阻塞模式,在连接前设置 默认为阻塞模式
        /// </summary>
        /// <param name="blocking">是否设置成阻塞模式</param>
        /// <returns></returns>
        Error setBlockingMode(bool blocking) {
            if (!_serverRunning) {
                _blockingMode = blocking;
                return Error::noError;
            }
            return Error::serverIsStarted;
        }

        /// <summary>
        /// 从客户端列表中移除客户端信息
        /// </summary>
        /// <param name="addr"></param>
        /// <param name="port"></param>
        /// <returns></returns>
        Error removeClientFromClientList(IP addr,ushort port) {
            for (auto it = _clientList.begin(); it != _clientList.end(); it++) {
                if ((*it).addr.toString() == addr.toString() && (*it).port == port) {		// 条件语句
                    _clientList.erase(it);		// 移除他
                    it--;		                    // 让该迭代器指向前一个
                    return Error::noError;
                }
            }
            return Error::noFoundClient;
        }



        /// <summary>
        /// 断开客户端的连接
        /// </summary>
        /// <param name="addr"></param>
        /// <param name="port"></param>
        /// <returns></returns>
        Error disClientConnect(IP addr,ushort port) {
            if (!_serverRunning) {
                return Error::serverNotStarted;
            }
            for (auto c : _clientList) {
                if (addr.toString() == c.addr.toString() && port == c.port) {
                    shutdown(c.clientSocket, SD_BOTH);
                    if (closesocket(c.clientSocket) == 0) {
                        removeClientFromClientList(addr, port);
                        //循环 
                        return Error::noError;
                    }
                    return Error::unkError;
                }
            }
            return Error::noFoundClient;
        };


        /// <summary>
        /// 注册客户端连接时事件
        /// </summary>
        /// <param name="onNewClientConnectEvent"></param>
        void onNewClientConnect(NewClientConnectEvent onNewClientConnectEvent) {
            _onNewClientConnectEvent = onNewClientConnectEvent;
        };

        /// <summary>
        /// 注册客户端断开连接时事件
        /// </summary>
        /// <param name="onClientDisConnectEvent"></param>
        void onClientDisConnect(ClientDisConnectEvent onClientDisConnectEvent) {
            _onClientDisConnectEvent = onClientDisConnectEvent;
        };

        /// <summary>
        /// 注册接收到客户端消息时回调
        /// </summary>
        /// <param name="recvDataEvent"></param>
        void onRecvData(RecvDataEvent recvDataEvent) {
            _recvDataEvent = recvDataEvent;
        }

        /// <summary>
        /// 阻塞模式 接收客户端消息时错误回调
        /// </summary>
        /// <param name="recvErrorEvent"></param>
        void onRecvError(RecvErrorEvent recvErrorEvent) {
            _recvErrorEvent = recvErrorEvent;
        }

    public:
        
        /// <summary>
        ///  Send text
        /// </summary>
        /// <param name="text">Text to be sent</param>
        /// <returns></returns>
        Error sendText(SOCKET socket,std::string text) {
            if (!_serverRunning) {
                return Error::serverNotStarted;
            }
            auto textlen = text.length();
            auto sendcount = send(socket, text.data(), text.length(), 0);
            if (sendcount <= 0) {
                return Error::sendFailed;
            }
            return Error::noError;
        };

        /// <summary>
        /// Sending bytes
        /// </summary>
        /// <param name="data"></param>
        /// <returns></returns>
        Error sendData(SOCKET socket, const char* data) {
            if (!_serverRunning) {
                return Error::serverNotStarted;
            }
            auto sendcount = send(socket, data, strlen(data), 0);
            if (sendcount <= 0) {
                return Error::sendFailed;
            }
            return Error::noError;
        };

        /// <summary>
        /// 获取所有已连接的客户端
        /// </summary>
        /// <returns></returns>
        std::vector<ClientList> GetALLClient() {
            return _clientList;
        }
        

    protected:
    private:
    private:
        /// <summary>
        /// 监听地址
        /// </summary>
        IP _listenAddr;
        /// <summary>
        /// 监听端口
        /// </summary>
        ushort _listenPort;
        /// <summary>
        /// 最大客户端连接数量
        /// </summary>
        int _maxCount;

        WSADATA _wsaDat;
        ushort _versionRequested = MAKEWORD(2, 2);

        sockaddr_in _sock = {};

        /// <summary>
        /// 服务端监听会话
        /// </summary>
        SOCKET _socket;

        /// <summary>
        /// 接收消息阻塞模式
        /// </summary>
        bool _blockingMode = true;

        /// <summary>
        /// 连接的客户端列表
        /// </summary>
        std::vector<ClientList> _clientList;
        
        /// <summary>
        /// 标志，防止重复初始化， 服务端是否启动
        /// </summary>
        bool _serverRunning = false;

        // 回调
        NewClientConnectEvent _onNewClientConnectEvent = NULL;
        ClientDisConnectEvent _onClientDisConnectEvent = NULL;
        RecvDataEvent _recvDataEvent = NULL;
        RecvErrorEvent _recvErrorEvent = NULL;
    };//class TCPServer

}//namespace meet

#endif //!___MIRACLEFOREST_MEET_TCPSERVER___