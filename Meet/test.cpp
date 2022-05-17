/*
*
* Copyright(C) 2022 MiracleForest Studio. All Rights Reserved.
*
* @filename:test.cpp
* @creation time:2022.5.10.12:42
* @created by:Lovelylavender4
* -----------------------------------------------------------------------------
* 
* -----------------------------------------------------------------------------
* If you have contact or find bugs,
* you can go to Github or email (MiracleForest@Outlook.com) to give feedback.
* We will try to do our best!
* 
*/

#include "../include/meet"

#include <iostream>
#include <chrono>

#define _S_ 1

struct AAA {
    std::string a;
};

int main() {
#if _S_
    WSADATA wsaDat;
    meet::TCPServer s(true);
    std::cout <<"initServer->" << meet::getString(
        s.initServer(true,wsaDat, 5, INADDR_ANY, 3000, 2, MAKEWORD(2, 2),
            [](
                const IN_ADDR& addr,
                unsigned short port,
                const meet::TCPServer::clientInfo& client,
                const meet::TCPServer* server
                )->meet::Error {
                    char sendBuf[20] = { '\0' };
                    std::cout << "[" << std::this_thread::get_id() << "]" << inet_ntop(AF_INET, (void*)&addr, sendBuf, 16) << std::endl;
                    std::cout << "[" << std::this_thread::get_id() << "]" << port << std::endl;
                    //meet::TCPServer::send("Hiiiiiii", client.clientSocket);

                    std::string dat;
                    for (;;){
                        if (meet::TCPServer::recv(dat, client.clientSocket,1024) == meet::Error::theClientIsDisconnected) {
                            std::cout << "[" << std::this_thread::get_id() << "]" << "Client " << inet_ntop(AF_INET, (void*)&addr, sendBuf, 16) <<":"<<port << " is disconnected" << std::endl;
                            return meet::Error::noError;
                        }
                        if (server->isEnd()) {
                            std::cout << "[" << std::this_thread::get_id() << "]" << "server is end" << std::endl;
                            return meet::Error::noError;
                        }
                        std::cout << "[" << std::this_thread::get_id() << "]" << dat << std::endl;
                    }
                    return meet::Error::noError;
            })) << std::endl;
        std::cout << "startServer->" << meet::getString(s.startServer()) << std::endl;
#else
    meet::TCPClient c;
    meet::Error connect_error;
    if ((connect_error = c.connectV4("127.0.0.1", 3000)) != meet::Error::noError) {
        std::cout << meet::getString(connect_error) << std::endl;
        system("pause");
        return 0;
    }
    else {
        std::cout << "connectV4->127.0.0.1:3000" << std::endl;
    }
    meet::Error send_error;
    if ((send_error = c.sendData(new char[] { 0x10, 0x11, 0x12, 0x13, 0x14 })) != meet::Error::noError) {
        std::cout << meet::getString(connect_error) << std::endl;
        system("pause");
        return 0;
    }
    else {
        std::cout << "sendData->127.0.0.1:3000" << std::endl;
    }
#endif
    system("pause");
    return 0;
}
