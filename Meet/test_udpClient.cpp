#pragma once
#include "../include/meet/Meet.hpp"
#include <iostream>


// 向预设发送数据包
void sendToPreset() {
    meet::UDPClient uc;

    std::string host = "au.hivebedrock.network";
    //host = "255.255.255.255";
    //host = "fd91:5d75:8210:0:dca:68ef:639c:1d9c";//0054
    //host = "fe80::45a4:261:f46e:6943%10";//0054
    //host = "fd91:5d75:8210::2";//0054
    u_short port = 19132;

    uc.setIPv6Support(true);
    uc.setBlockingMode(false);
    uc.allowbroadcast(true);

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
        std::cout << "[" << ip.toString() << "/" << std::to_string(port) << "] ";
        std::cout << d << std::endl;
        });

    uc.OnRemoteSockClose([](meet::IP ip, u_short port) {
        std::cout << "远程主机:" << ip.toString() << "/" << std::to_string(port) << " 的SOCK已经关闭";
        });

    uc.Init();

    char pack[] = { (char)0x01, (char)0x00, (char)0x00, (char)0x00, (char)0x00, (char)0x24, (char)0x0d, (char)0x12, (char)0xd3, (char)0x00, (char)0xff, (char)0xff, (char)0x00, (char)0xfe, (char)0xfe, (char)0xfe, (char)0xfe, (char)0xfd, (char)0xfd, (char)0xfd, (char)0xfd, (char)0x12, (char)0x34, (char)0x56, (char)0x78 };
    meet::Error err = uc.sendData(host, port, pack, sizeof(pack) / sizeof(char));
    if ((int)err) {
        std::cout << meet::getString(err) << std::endl;
    }
    std::ignore = getchar();
    uc.close();
}


void sendToRemote() {
    meet::UDPClient uc;
    meet::IP ip;
    u_short port = 0;

    uc.setIPv6Support(true);
    uc.OnRecvData([](ULONG64 len, const char* data, meet::IP ip, u_short port) {
        std::cout << "[" << ip.toString() << "/" << std::to_string(port) << "] ";
        std::cout << data << std::endl;
        });

    uc.OnRemoteSockClose([](meet::IP ip, u_short port) {
        std::cout << "远程主机:" << ip.toString() << "/" << std::to_string(port) << " 的SOCK已经关闭";
        });

    uc.Init();

    for (;;) {
        std::string userinput;
        // 没有IP或端口 要求输入
        if (!ip.isValid() || port == 0) {
            std::cout << "请输入IP地址(默认127.0.0.1):";
            std::getline(std::cin, userinput);
            if (userinput.empty()) userinput = "127.0.0.1";
            ip = userinput;
            if (!ip.isValid()) {
                std::cout << "IP地址非有效IP..." << std::endl;
                continue;
            }

            std::cout << "请输入端口:";
            std::getline(std::cin, userinput);
            auto p = atoi(userinput.c_str());
            if (p >= 65535 || p <= 0) {
                std::cout << "端口错了" << std::endl;
                continue;
            }
            port = p;
        }

        std::cout << "请输入要发送的数据(输入0退出,-1重新输入ip端口):";
        std::getline(std::cin, userinput);
        if (userinput == "0") {
            std::cout << "退出" << std::endl;
            break;
        }
        if (userinput == "-1") {
            ip.Clear();
            port = 0;
            continue;
        }
        meet::Error err = uc.sendData(ip, port, userinput.c_str(), static_cast<int>(userinput.length()));
        if ((int)err) {
            std::cout << "sendData 错误: ";
            std::cout << meet::getString(err) << std::endl;
        }
    }
    std::ignore = getchar();
    uc.close();
}


void startUDPClient() {
    for (;;) {
        
        std::cout << "UDPClient 控制台" << std::endl;
        std::cout << "0 ---- 退出" << std::endl;
        std::cout << "1 ---- 向预设发送数据包" << std::endl;
        std::cout << "2 ---- 向目的地址发送数据包" << std::endl;
        std::cout << "3 ---- ???" << std::endl;
        std::cout << "4/cls ---- 清屏" << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
        std::cout << "请输入一个选项:";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        std::string userinput;
        std::getline(std::cin, userinput);
        if (userinput == "0") {
            break;
        }
        else if (userinput == "1") {
            sendToPreset();
        }
        else if (userinput == "2") {
            sendToRemote();                     // 开启Socket 发送数据
        }
        else if (userinput == "3") {
            
        }
        else if (userinput == "4" || userinput == "cls") {
            system("cls");
        }
    }
}