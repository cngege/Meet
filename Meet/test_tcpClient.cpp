#pragma once
#include "../include/Meet.hpp"

#include <iostream>
#include <fstream>
#include <chrono>

bool ClientWriteFile = false;
std::ofstream ClientWriteFileIO;

// 开启一个客户端
void startClient(meet::TCPClient& c) {

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
            ClientWriteFileIO.write(data, len);
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
            break;
        }
        std::cout << "端口:";
        std::getline(std::cin, port_input);

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
    //meet::IP connIp = meet::IP::getaddrinfo(ip_input);
    meet::IP connIp = ip_input;

    if (!connIp.isValid()) {
        std::cout << "地址解析错误: 你输入的域名可能无效 " << ip_input << std::endl;
        return;
    }

    std::cout << "IPAddress: " << connIp.toString() << std::endl;

    // 连接服务端 并对连接结果进行判断
    if ((connect_error = c.connect(connIp, port)) != meet::Error::noError) {
        std::cout << "连接错误:" << meet::getString(connect_error) << std::endl;
        std::cout << "debug Family: ";
        std::cout << ((connIp.IPFamily == meet::Family::IPV4) ? "IPV4" : "IPV6") << std::endl;
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
            c.close();
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
                return;
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

            while (true) {
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