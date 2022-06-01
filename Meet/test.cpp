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


void startServer(){
    
    meet::IP listenAddr(meet::Family::IPV4, "0.0.0.0");
    meet::TCPServer s(listenAddr,3000,2);

    s.setBlockingMode(false);

    s.onClientDisConnect([](meet::IP ip,USHORT port) {
        printf("\n[%s:%d][连接] 断开连接\n", ip.toString().c_str(), port);
    });

    s.onNewClientConnect([](meet::IP ip, USHORT port, SOCKET socket) {
        printf("\n[%s:%d][连接] 连接成功\n", ip.toString().c_str(), port);
    });

    s.onRecvData([](meet::IP ip, USHORT port, SOCKET socket,ULONG64 len,const char* data) {
        printf("\n[%s:%d][数据][%I64d字节]:", ip.toString().c_str(),port,len);
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9);
        std::cout << std::string(data) << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    });

    meet::Error listen_err = s.Listen();
    if (listen_err != meet::Error::noError) {
        std::cout << "监听错误:" << meet::getString(listen_err) << std::endl;
        return;
    }

    for (;;) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
        std::cout << "0 ---- 断开所有连接并退出服务端" << std::endl;
        std::cout << "1 ---- 选择一个客户端,并进行操作" << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
        std::cout << "请输入一个选项:";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        std::string sinput;
        std::getline(std::cin, sinput);


    }

}

void startClient() {
    meet::TCPClient c;
    //关闭阻塞模式
    c.setBlockingMode(false);

    //注册断开连接 回调
    c.onDisConnect([]() {
        std::cout << "服务端断开了连接" << std::endl;
        });

    //注册接收消息 回调
    c.onRecvData([](ULONG64 len, const char* data) {
        std::cout << "\n[来自服务端 " << len << "字节]:";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9);
        std::cout << std::string(data) << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        });

    meet::Error connect_error;
    if ((connect_error = c.connectV4("127.0.0.1", 3000)) != meet::Error::noError) {
        std::cout << meet::getString(connect_error) << std::endl;
        return;
    }
    else {
        std::cout << "connectV4->127.0.0.1:3000" << std::endl;
    }

    //根据输入来执行相应功能
    for (;;) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
        std::cout << "0 ---- 断开连接并退出客户端" << std::endl;
        std::cout << "1 ---- 发送文本" << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
        std::cout << "请输入一个选项:";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        std::string cinput;
        std::getline(std::cin, cinput);

        if (cinput == "0") {
            c.disConnect();
            return;
        }
        else if (cinput == "1") {
            std::cout << "请输入你要发送的文本:";
            std::string csendtext;
            std::getline(std::cin, csendtext);

            //发送文本
            meet::Error send_error;
            if ((send_error = c.sendText(csendtext)) != meet::Error::noError) {
                std::cout << meet::getString(send_error) << std::endl;
                return;
            }
            else {
                std::cout << "sendData->127.0.0.1:3000" << std::endl;
            }
        }

    }
}



int main() {

    for (;;) {
        system("title Menu / 主菜单");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
        std::cout << "0 ---- 退出" << std::endl;
        std::cout << "1 ---- 开启服务端" << std::endl;
        std::cout << "2 ---- 开启客户端" << std::endl;
        std::cout << "3/cls ---- 清屏" << std::endl;
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
            system("title Server / 服务端");
            startServer();
        }
        //客户端
        else if (userinput == "2") {
            system("title Client / 客户端");
            startClient();
        }
        else if (userinput == "3" || userinput == "cls") {
            system("cls");
        }
    }
}
