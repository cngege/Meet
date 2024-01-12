#pragma once
#include "../include/Meet.hpp"

#include <iostream>
#include <fstream>
#include <chrono>


bool ServerWriteFile = false;
std::string ServerWriteFileIP;
USHORT ServerWriteFilePort;
std::ofstream ServerWriteFileIO;

// 开启一个TCP的服务
void startServer(meet::TCPServer& s) {

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
        maxconn = count;
    }

    // 设置为阻塞模式
    s.setBlockingMode(true);

    // 监听客户端断开连接的消息
    s.onClientDisConnect([&](meet::TCPServer::MeetClient meetClient) {
        //printf("\n[%s -:- %d][连接] 断开连接\n", meetClient.getIp().toString().c_str(), meetClient.getPort());
        printf("\n[%s -:- %d][连接] 断开连接\n", meetClient.getIp().toString().c_str(), meetClient.getPort());
        system(std::format("title TCP Server / 服务端 [{}/{}]", s.GetALLClient().size(), s.getMaxConnectCount()).c_str());
        });

    // 监听客户端连接的消息
    s.onNewClientConnect([&](meet::TCPServer::MeetClient meetClient /*meet::IP ip, USHORT port, SOCKET socket*/) {
        printf("\n[%s -:- %d][连接] 连接成功\n", meetClient.getIp().toString().c_str(), meetClient.getPort());
        system(std::format("title TCP Server / 服务端 [{}/{}]",s.GetALLClient().size(), s.getMaxConnectCount()).c_str());
        });

    // 当有数据到达时监听消息
    s.onRecvData([&](meet::TCPServer::MeetClient meetClient /*meet::IP ip, USHORT port, SOCKET socket*/, ULONG64 len, const char* data) {
        if (ServerWriteFile && ServerWriteFileIP == meetClient.getIp().toString() && ServerWriteFilePort == meetClient.getPort()) {
            ServerWriteFileIO.write(data, len);
            ServerWriteFileIO.flush();
        }
        else {
            // 检查是不是exit,则断开连接
            if (std::string(data) == "exit") {
                meetClient.disConnect();
                printf("\n[%s -:- %d][命令]: %s", meetClient.getIp().toString().c_str(), meetClient.getPort(), "断开连接命令");
                system(std::format("title TCP Server / 服务端 [{}/{}]", s.GetALLClient().size(), s.getMaxConnectCount()).c_str());
                return;
            }
            printf("\n[%s -:- %d][数据][%I64d字节]:", meetClient.getIp().toString().c_str(), meetClient.getPort(), len);
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
        if (WSAGetLastError() == WSAEADDRINUSE) {
            std::cout << "监听地址已被其他进程绑定 " << std::endl;
        }
        std::cout << "监听错误:" << meet::getString(listen_err) << std::endl;
        return;
    }

    // 打印 服务端配置情况
    std::cout << std::format("Address: {}:{}", s.getListenAddress().toString(), s.getListenPort()) << std::endl;
    std::cout << std::format("BlockingMode: {}", s.isBlockingMode()) << std::endl;
    std::cout << std::format("MaxClientCount: {}", s.getMaxConnectCount()) << std::endl;
    std::cout << std::endl;

    for (;;) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
        std::cout << std::format("CurrentClientCount: {}", s.GetALLClient().size()) << std::endl;
        std::cout << "0 ---- 断开所有连接并退出服务端" << std::endl;
        std::cout << "1 ---- 选择一个客户端,并进行操作" << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
        std::cout << "请输入一个选项:";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        std::string sinput;
        std::getline(std::cin, sinput);

        if (sinput == "0") {
            auto& clientList = s.GetALLClient();
            int discount = 0;
            size_t clientcount = clientList.size();

            for (INT64 i = clientcount - 1; i >= 0; i--) {
                if (s.disClientConnect(clientList[i].getIp(), clientList[i].getPort()) == meet::Error::noError) {
                    discount++;
                }
            }
            std::cout << "共成功断开了 " << discount << "/" << clientcount << " 个客户端的连接" << std::endl;
            std::cout << "debug clientList size: " << clientList.size() << std::endl;
            //s.close();
            break;
        }
        else if (sinput == "1") {
            for (;;) {
                int a = 0;
                auto& clientList = s.GetALLClient();
                if (clientList.size() == 0) {
                    std::cout << "当前没有客户端连接" << std::endl;
                    break;
                }

                std::cout << std::endl;
                for (int i = 0; i < clientList.size(); i++) {
                    std::cout << i << " ========== [" << clientList.at(i).getIp().toString() << ":" << clientList.at(i).getPort() << "]" << std::endl;
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
                        std::cout << "当前选择的客户端是:[" << client.getIp().toString() << ":" << client.getPort() << "]" << std::endl;
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

                        if (x >= clientList.size() || clientList.at(x).getSocket() != client.getSocket()) {
                            std::cout << "该客户端的连接早已断开" << std::endl;
                            break;
                        }
                        if (sinput_setup == "0") {
                            meet::Error err = s.disClientConnect(client.getIp(), client.getPort());
                            if (err != meet::Error::noError) {
                                std::cout << "客户端断开失败:" << meet::getString(err) << std::endl;
                            }
                            //s.Close();
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
                            if ((err = s.sendText(client.getSocket(), ssendtext)) != meet::Error::noError) {
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

                                meet::Error sendFileErr = s.sendData(client.getSocket(), tempStr, readsize);
                                if (sendFileErr != meet::Error::noError) {
                                    std::cout << "发送文件发生错误:" << meet::getString(sendFileErr) << std::endl;
                                    break;
                                }
                            }
                            sf.close();// 关闭读取文件的文件句柄 
                        }
                        else if (sinput_setup == "4") {

                            std::cout << "请输入将保存的文件名:";
                            std::string ssavefile;
                            std::getline(std::cin, ssavefile);


                            ServerWriteFileIO.open(ssavefile, std::ios::binary);
                            if (ServerWriteFileIO.is_open()) {
                                ServerWriteFile = true;
                                ServerWriteFileIP = client.getIp().toString();
                                ServerWriteFilePort = client.getPort();
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