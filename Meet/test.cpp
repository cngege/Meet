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


struct AAA {
    std::string a;
};


void startServer(){
    WSADATA wsaDat;
    meet::TCPServer s(true);
    std::cout << "initServer->" << meet::getString(
        s.initServer(true, wsaDat, 5, INADDR_ANY, 3000, 2, MAKEWORD(2, 2),
            [](
                const IN_ADDR& addr,
                unsigned short port,
                const meet::TCPServer::clientInfo& client,
                const meet::TCPServer* server
                )->meet::Error {
                    char sendBuf[20] = { '\0' };
                    std::cout << "[" << std::this_thread::get_id() << "]" << inet_ntop(AF_INET, (void*)&addr, sendBuf, 16) << std::endl;
                    std::cout << "[" << std::this_thread::get_id() << "]" << port << std::endl;
                    meet::TCPServer::send("Hiiiiiii", client.clientSocket);

                    std::string dat;
                    for (;;) {
                        if (meet::TCPServer::recv(dat, client.clientSocket, 1024) == meet::Error::theClientIsDisconnected) {
                            std::cout << "[" << std::this_thread::get_id() << "]" << "Client " << inet_ntop(AF_INET, (void*)&addr, sendBuf, 16) << ":" << port << " is disconnected" << std::endl;
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
}

void startClient() {
    meet::TCPClient c;

    //注册断开连接 回调
    c.onDisConnect([]() {
        std::cout << "服务端断开了连接" << std::endl;
        });

    //注册接收消息 回调
    c.onRecvData([](ULONG64 len, const char* data) {
        std::cout << "接收到了来自服务端的数据，共:" << len << "字节" << std::endl;
        std::cout << std::string(data) << std::endl;
        });

    meet::Error connect_error;
    if ((connect_error = c.connectV4("127.0.0.1", 3000)) != meet::Error::noError) {
        std::cout << meet::getString(connect_error) << std::endl;
        system("pause");
        return;
    }
    else {
        std::cout << "connectV4->127.0.0.1:3000" << std::endl;
    }

    //根据输入来执行相应功能
    for (;;) {
        std::cout << "0 ---- 断开连接并退出客户端" << std::endl;
        std::cout << "1 ---- 发送文本" << std::endl;
        std::cout << "请输入一个选项:";
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
                std::cout << meet::getString(connect_error) << std::endl;
                system("pause");
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
        std::cout << "0 ---- 退出" << std::endl;
        std::cout << "1 ---- 开启服务端" << std::endl;
        std::cout << "2 ---- 开启客户端" << std::endl;
        std::cout << "3/cls ---- 清屏" << std::endl;
        std::cout << "请输入一个选项:";
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
