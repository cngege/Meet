﻿/*
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

meet::TCPServer s;
meet::TCPClient c;

bool ClientWriteFile = false;
std::ofstream ClientWriteFileIO;

bool ServerWriteFile = false;
std::string ServerWriteFileIP;
USHORT ServerWriteFilePort;
std::ofstream ServerWriteFileIO;

// 开启一个TCP的服务
void startServer(){
    
    meet::Family fa = meet::Family::IPV4;
    USHORT port = 3000;
    int maxconn = 5;
    std::cout << "请输入你要监听地址的协议(4/6):";
    std::string fa_input;
    std::getline(std::cin, fa_input);
    if (fa_input == "" || fa_input == "0") {
        std::cout << "将使用默认项(0.0.0.0 : 3000  maxconnect 5)" << std::endl;
    }
    else {
        if (fa_input == "6") {
            fa = meet::Family::IPV6;
        }
        std::cout << "请输入你要监听端口:";
        std::string port_input;
        std::getline(std::cin, port_input);
        auto p = atoi(port_input.c_str());
        if (p >= 65535 || p <= 0) {
            std::cout << "输入的端口有问题" << std::endl;
            return;
        }
        port = p;

        std::cout << "请输入最大允许连接的客户端数量:";
        std::string count_input;
        std::getline(std::cin, count_input);
        auto count = atoi(count_input.c_str());
        if (count <= 0) {
            std::cout << "你乱输什么啊,我替你选吧,就……10个吧" << std::endl;
            maxconn = 10;
        }
        else {
            maxconn = count;
        }

    }

    // 设置为阻塞模式
    s.setBlockingMode(true);

    // 监听客户端断开连接的消息
    s.onClientDisConnect([](meet::TCPServer::MeetClient meetClient) {
        printf("\n[%s -:- %d][连接] 断开连接\n", meetClient.addr.toString().c_str(), meetClient.port);
    });

    // 监听客户端连接的消息
    s.onNewClientConnect([](meet::TCPServer::MeetClient meetClient /*meet::IP ip, USHORT port, SOCKET socket*/) {
        printf("\n[%s -:- %d][连接] 连接成功\n", meetClient.addr.toString().c_str(), meetClient.port);
    });

    // 当有数据到达时监听消息
    s.onRecvData([](meet::TCPServer::MeetClient meetClient /*meet::IP ip, USHORT port, SOCKET socket*/,ULONG64 len,const char* data) {
        if (ServerWriteFile && ServerWriteFileIP == meetClient.addr.toString() && ServerWriteFilePort == meetClient.port) {
            ServerWriteFileIO.write(data, len);
            ServerWriteFileIO.flush();
        }
        else {
            printf("\n[%s -:- %d][数据][%I64d字节]:", meetClient.addr.toString().c_str(), meetClient.port, len);
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9);
            std::cout << std::string(data) << std::endl;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        }

    });

    // 监听地址 对监听v4地址和 v6地址做出区分
    if (fa == meet::Family::IPV6) {
        s.setListenAddress("::");
    }

    // 设置服务端的客户端连接队列的最大值
    s.setMaxConnectCount(maxconn);

    // 开始监听
    meet::Error listen_err = s.Listen(port);
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
            
            for (auto &c : clientList) {
                if (s.disClientConnect(c.addr, c.port) == meet::Error::noError) {
                    discount++;
                }
            }
            std::cout << "共成功断开了 " << discount << "/" << clientcount << " 个客户端的连接" << std::endl;
            s.~TCPServer();
            break;
        }
        else if (sinput == "1") {
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
                std::cout << clientList.size() << " ========== [返回上一页] " << std::endl;
                std::cout << "请输入一个序号选择一个客户端:";
                std::string sinput_client;
                std::getline(std::cin, sinput_client);

                int x = atoi(sinput_client.c_str());
                if (x == 0 && sinput_client != "0") {
                    system("cls");
                    continue;
                }
                if (x == clientList.size()) {
                    break;
                }
                if (x >= 0 && x < clientList.size()) {
                    // 控制台输入选择了一个客户端
                    meet::TCPServer::MeetClient client = clientList.at(x);
                    // 对选择的客户端进行操作
                    for (;;) {
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
                        std::cout << "当前选择的客户端是:[" << client.addr.toString() << ":" << client.port << "]" << std::endl;
                        std::cout << "0 ---- 断开这个客户端,并返回上一页" << std::endl;
                        std::cout << "1 ---- 返回上一页" << std::endl;
                        std::cout << "2 ---- 发送消息" << std::endl;
                        std::cout << "3 ---- 发送文件" << std::endl;
                        std::cout << "4 ---- 接收文件" << std::endl;
                        std::cout << "5 ---- 停止接收文件" << std::endl;
                        std::cout << "6/cls ---- 清屏" << std::endl;
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
                        std::cout << "请输入一个选项:";
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                        std::string sinput_setup;
                        std::getline(std::cin, sinput_setup);
                        if (clientList.at(x).clientSocket != client.clientSocket) {
                            std::cout << "该客户端的连接早已断开" << std::endl;
                            break;
                        }
                        if (sinput_setup == "0") {
                            meet::Error err = s.disClientConnect(client.addr, client.port);
                            if (err != meet::Error::noError) {
                                std::cout << "客户端断开失败:" << meet::getString(err) << std::endl;
                            }
                            c.~TCPClient();
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
                            if ((err = s.sendText(client.clientSocket, ssendtext)) != meet::Error::noError) {
                                std::cout << "发送消息出现错误" << meet::getString(err) << std::endl;
                            }
                        }
                        else if (sinput_setup == "3") {
                            std::cout << "请确保对方准备好接收文件" << std::endl;
                            std::cout << "请输入文件名:";
                            std::string ssendfile;
                            std::getline(std::cin, ssendfile);

                            //发送文件 以二进制方式读取
                            std::ifstream sf(ssendfile.c_str(), std::ios::binary | std::ios::in);
                            if (!sf.good()) {
                                std::cout << "文件不存在,请仔细确认" << std::endl;
                                continue;
                            }

                            while (true) {

                                if (sf.eof()) {
                                    std::cout << "文件发送完成。" << std::endl;
                                    break;
                                }
                                char* tempStr = new char[1024];
                                sf.read(tempStr, 1024);
                                int readsize = static_cast<int>(sf.gcount());

                                meet::Error sendFileErr = s.sendData(client.clientSocket,tempStr, readsize);
                                if (sendFileErr != meet::Error::noError) {
                                    std::cout << "发送文件发生错误:" << meet::getString(sendFileErr) << std::endl;
                                    break;
                                }
                            }
                            sf.close();
                        }
                        else if (sinput_setup == "4") {

                            std::cout << "请输入将保存的文件名:";
                            std::string ssavefile;
                            std::getline(std::cin, ssavefile);


                            ServerWriteFileIO.open(ssavefile,std::ios::binary);
                            if (ServerWriteFileIO.is_open()) {
                                ServerWriteFile = true;
                                ServerWriteFileIP = client.addr.toString();
                                ServerWriteFilePort = client.port;
                                std::cout << "开始接收文件" << std::endl;
                            }
                        }
                        else if (sinput_setup == "5") {
                            if (ServerWriteFile) {
                                ServerWriteFileIO.close();
                                ServerWriteFile = false;
                                std::cout << "文件已经保存" << std::endl;
                            }
                        }
                        else if (sinput_setup == "6" || sinput_setup == "cls") {
                            system("cls");
                        }

                    }

                    break;
                }

            }

        }

    }

}

// 开启一个客户端
void startClient() {
    
    //关闭阻塞模式
    //c.setBlockingMode(false);

    //注册断开连接 回调
    c.onDisConnect([]() {
        std::cout << "服务端断开了连接" << std::endl;
        });

    //注册接收消息 回调
    c.onRecvData([](ULONG64 len, const char* data) {
        if (!ClientWriteFile) {
            std::cout << "\n[来自服务端 " << len << "字节]:";
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9);
            std::cout << std::string(data) << std::endl;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        }
        else {
            ClientWriteFileIO.write(data,len);
            ClientWriteFileIO.flush();
        }
        });

    // 手动设置连接服务端的地址端口
    std::string ip_input, fm_input;
    USHORT port;
    for (;;) {
        std::cout << "请分别输入要连接服务端的地址,端口,协议(4/6)(默认127.0.0.1:3000)" << std::endl;
        std::cout << "地址(请确保可以解析,否则会崩溃):";
        std::string port_input;
        std::getline(std::cin, ip_input);
        if (ip_input == "" || ip_input == "0") {
            //按默认的来连接
            ip_input = "127.0.0.1";
            port = 3000;
            fm_input = "4";
            break;
        }
        std::cout << "端口:";
        std::getline(std::cin, port_input);
        std::cout << "协议:";
        std::getline(std::cin, fm_input);
        
        auto p = atoi(port_input.c_str());
        if (p >= 65535 || p <= 0) {
            system("cls");
            std::cout << "端口错了" << std::endl;
            continue;
        }
        port = p;
        break;
    }


    meet::Error connect_error;
    meet::IP connIp = meet::IP::getaddrinfo((fm_input == "6") ? meet::Family::IPV6 : meet::Family::IPV4, ip_input);

    // 连接服务端 并对连接结果进行判断
    if ((connect_error = c.connect(connIp, port)) != meet::Error::noError) {
        std::cout << "连接错误:" << meet::getString(connect_error) << std::endl;
        return;
    }

    //根据输入来执行相应功能
    for (;;) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
        std::cout << "0 ---- 断开连接并退出客户端" << std::endl;
        std::cout << "1 ---- 发送文本" << std::endl;
        std::cout << "2 ---- 发送文件" << std::endl;
        std::cout << "3 ---- 接收文件" << std::endl;
        std::cout << "4 ---- 停止接收文件" << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
        std::cout << "请输入一个选项:";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        std::string cinput;
        std::getline(std::cin, cinput);

        if (!c.Connected) {
            std::cout << "连接早已断开" << std::endl;
            return;
        }

        if (cinput == "0") {
            c.disConnect();
            return;
        }
        else if (cinput == "1") {
            std::cout << "请输入你要发送的文本:";
            std::string csendtext;
            std::getline(std::cin, csendtext);

            if (!c.Connected) {
                std::cout << "连接已经断开" << std::endl;
                return;
            }

            //发送文本
            meet::Error send_error;
            if ((send_error = c.sendText(csendtext)) != meet::Error::noError) {
                std::cout << meet::getString(send_error) << std::endl;
            }
        }
        else if (cinput == "2") {
            std::cout << "请确保对方准备好接收文件" << std::endl;
            std::cout << "请输入文件名:";
            std::string csendfile;
            std::getline(std::cin, csendfile);

            if (!c.Connected) {
                std::cout << "连接已经断开" << std::endl;
                return;
            }

            //发送文件 以二进制方式读取
            std::ifstream sf(csendfile.c_str(), std::ios::binary | std::ios::in);
            if (!sf.good()) {
                std::cout << "文件不存在,请仔细确认" << std::endl;
                continue;
            }

            while(true) {
                if (sf.eof()) {
                    std::cout << "文件发送完成。" << std::endl;
                    break;
                }
                char* tempStr = new char[1024];
                sf.read(tempStr, 1024);
                int readsize = static_cast<int>(sf.gcount());

                meet::Error sendFileErr = c.sendData(tempStr, readsize);
                delete[] tempStr;
                if (sendFileErr != meet::Error::noError) {
                    std::cout << "发送文件发生错误:" << meet::getString(sendFileErr) << std::endl;
                    break;
                }
            }
            sf.close();

        }
        else if (cinput == "3") {
            std::cout << "请输入将保存的文件名:";
            std::string csavefile;
            std::getline(std::cin, csavefile);

            if (!c.Connected) {
                std::cout << "连接已经断开" << std::endl;
                return;
            }
            // 设置一个变量  使接收消息回调 确认这个变量后进行文件的保存

            ClientWriteFileIO.open(csavefile.c_str(), std::ios::binary);
            if (ClientWriteFileIO.is_open()) {
                ClientWriteFile = true;
                std::cout << "现在开始接收文件" << std::endl;
            }

        }
        else if (cinput == "4") {
            if (ClientWriteFile) {
                ClientWriteFileIO.close();
                ClientWriteFile = false;
                std::cout << "文件已保存" << std::endl;
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
