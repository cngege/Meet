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
            system("title UDP Client / 客户端");
            meet::UDPClient uc;
            uc.Init();
            uc.OnRecvData([](ULONG64 len, const char* data, meet::IP ip, u_short port) {
                std::string d;
                for (int i = 0; i < len; i++) {
                    if (len - i > 4) {
                        if (std::string(data + i).substr(0, 4) == "MCPE") {
                            d = std::string(data + i);
                            break;
                        }
                    }
                }
                std::cout << d << std::endl;
                //size_t mcpepos = std::string(data).find("MCPE", 0);
                //std::cout << std::string(data).substr(mcpepos) << std::endl;
                });
            //uc.Init();
            char pack[] = { (char)0x01, (char)0x00, (char)0x00, (char)0x00, (char)0x00, (char)0x24, (char)0x0d, (char)0x12, (char)0xd3, (char)0x00, (char)0xff, (char)0xff, (char)0x00, (char)0xfe, (char)0xfe, (char)0xfe, (char)0xfe, (char)0xfd, (char)0xfd, (char)0xfd, (char)0xfd, (char)0x12, (char)0x34, (char)0x56, (char)0x78 };
            uc.sendData(std::string("au.hivebedrock.network"), 19132, pack, 25);
            Sleep(1000);
            uc.close();
        }
        else if (userinput == "5" || userinput == "cls") {
            system("cls");
        }
    }
}
