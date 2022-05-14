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
    WSADATA wsaDat;
    meet::TCPServer s;
    s.initServer(wsaDat, 10, INADDR_ANY, 3000, 2, MAKEWORD(2, 2),
        [](
            const IN_ADDR& addr,
            unsigned short port,
            const meet::TCPServer& server
            )->meet::Error {
                char sendBuf[20] = { '\0' };
                std::cout << inet_ntop(AF_INET, (void*)&addr, sendBuf, 16) << std::endl;;
                std::cout << port << std::endl;
                return meet::Error::noError;
        });
    s.startServer();
    
    system("pause");
    return 0;
}
