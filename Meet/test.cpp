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

    s.setBlockingMode(true);

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

        if (sinput == "0") {
            auto clientList = s.GetALLClient();
            int discount = 0;
            size_t clientcount = clientList.size();
            
            for (auto c : clientList) {
                if (s.disClientConnect(c.addr, c.port) == meet::Error::noError) {
                    discount++;
                }
            }
            std::cout << "共成功断开了 " << discount << "/" << clientcount << " 个客户端的连接" << std::endl;
            break;
        }
        else if (sinput == "1") {

            meet::TCPServer::ClientList* client = nullptr;
            for (;;) {
                int a = 0;
                auto clientList = s.GetALLClient();
                if (clientList.size() == 0) {
                    std::cout << "当前没有客户端连接" << std::endl; 
                    break;
                }

                std::cout << std::endl;
                for (int i = 0; i < clientList.size(); i++) {
                    std::cout << i << " ========== [" << clientList.at(i).addr.toString() << ":" << clientList.at(i).port << "]" << std::endl;
                }
                std::cout << "请输入一个序号选择一个客户端:";
                std::string sinput_client;
                std::getline(std::cin, sinput_client);

                int x = atoi(sinput_client.c_str());
                if (x == 0 && sinput_client != "0") {
                    system("cls");
                    continue;
                }
                if (x >= 0 && x < clientList.size()) {
                    client = &clientList.at(x);
                    break;
                }

            }

            for (;;) {
                if (s.GetALLClient().size() == 0) {
                    break;
                }

                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
                std::cout << "当前选择的客户端是:[" << client->addr.toString() << ":" << client->port << "]" << std::endl;
                std::cout << "0 ---- 断开这个客户端,并返回上一页" << std::endl;
                std::cout << "1 ---- 返回上一页" << std::endl;
                std::cout << "2 ---- 发送消息" << std::endl;
                std::cout << "3/cls ---- 清屏" << std::endl;
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
                std::cout << "请输入一个选项:";
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                std::string sinput_setup;
                std::getline(std::cin, sinput_setup);
                if (sinput_setup == "0") {
                    meet::Error err  = s.disClientConnect(client->addr, client->port);
                    if (err != meet::Error::noError) {
                        std::cout << "客户端断开失败:" << meet::getString(err) << std::endl;
                    }
                    break;
                }
                else if (sinput_setup == "1") {
                    break;
                }
                else if (sinput_setup == "2") {
                    std::cout << "请输入你要发送的文本:";
                    std::string ssendtext;
                    std::getline(std::cin, ssendtext);

                    meet::Error err;
                    if ((err = s.sendText(client->clientSocket,ssendtext)) != meet::Error::noError) {
                        std::cout << "发送消息出现错误" << meet::getString(err) << std::endl;
                    }
                }
                else if (sinput_setup == "3" || sinput_setup == "cls") {
                    system("cls");
                }

            }


        }

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
