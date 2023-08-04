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

//#include "../include/meet"
#include "../include/Meet.hpp"

#include <iostream>
#include <fstream>
#include <chrono>

//#include "test_tcpClient.cpp"
//#include "test_tcpServer.cpp"

void startClient(meet::TCPClient& c);
void startServer(meet::TCPServer& s);
void startUDPClient();

int main() {
    for (;;) {
        meet::TCPServer s;
        meet::TCPClient c;


        system("title Menu / 主菜单");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
        std::cout << "0 ---- 退出" << std::endl;
        std::cout << "1 ---- 开启TCP服务端" << std::endl;
        std::cout << "2 ---- 开启TCP客户端" << std::endl;
        std::cout << "3 ---- 开启UDP服务端" << std::endl;
        std::cout << "4 ---- 开启UDP客户端" << std::endl;
        std::cout << "5/cls ---- 清屏" << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
        std::cout << "请输入一个选项:";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        std::string userinput;
        std::getline(std::cin, userinput);
        if (userinput == "0") {
            return 0;
        }
        //服务端
        else if (userinput == "1") {
            system("title TCP Server / 服务端");
            startServer(s);
        }
        //客户端
        else if (userinput == "2") {
            system("title TCP Client / 客户端");
            startClient(c);
        }
        // UDP Server
        else if (userinput == "3") {
            system("title UDP Server / 服务端");
            
        }
        // UDP Client
        else if (userinput == "4") {
            system("title UDP Client / 客户端");           // 已证实 A通过出口Socket A' 连接B， C就可以通过发送数据到A' 而连接A
            startUDPClient();
        }
        else if (userinput == "5" || userinput == "cls") {
            system("cls");
        }
    }
}
