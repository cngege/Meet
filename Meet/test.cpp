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


int main() {

    meet::TCPClient c;
    meet::Error connect_error;
    if ((connect_error = c.connectV4("127.0.0.1", 3000)) != meet::Error::noError) {
        std::cout << meet::getString(connect_error) << std::endl;
        system("pause");
        return 0;
    }
    meet::Error send_error;
    if ((send_error = c.sendData(new char[] { 0x10, 0x11, 0x12, 0x13, 0x14 })) != meet::Error::noError) {
        std::cout << meet::getString(connect_error) << std::endl;
        system("pause");
        return 0;
    }
    system("pause");
    return 0;

    WSADATA wsaDat;
    meet::TCPServer s;
    std::cout <<"initServer->" << meet::getString(
        s.initServer(true,wsaDat, 5, INADDR_ANY, 3000, 2, MAKEWORD(2, 2),
            [](
                const IN_ADDR& addr,
                unsigned short port,
                const meet::TCPServer::clientInfo& client
                )->meet::Error {
                    char sendBuf[20] = { '\0' };
                    std::cout << "[" << std::this_thread::get_id() << "]" << inet_ntop(AF_INET, (void*)&addr, sendBuf, 16) << std::endl;
                    std::cout << "[" << std::this_thread::get_id() << "]" << port << std::endl;
                    return meet::Error::noError;
            })) << std::endl;
    std::cout << "startServer->" << meet::getString(s.startServer()) << std::endl;
    
    system("pause");
    return 0;
}
