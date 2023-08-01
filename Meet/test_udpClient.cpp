#pragma once
#include "../include/Meet.hpp"
#include <iostream>

void startUDPClient() {
    system("cls");
    std::string host = "au.hivebedrock.network";
    //host = "255.255.255.255";
    //host = "fd91:5d75:8210:0:dca:68ef:639c:1d9c";//0054
    //host = "fe80::45a4:261:f46e:6943%10";//0054
    //host = "fd91:5d75:8210::2";//0054

    u_short port = 19132;

    meet::UDPClient uc;
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
    uc.Init();

    char pack[] = { (char)0x01, (char)0x00, (char)0x00, (char)0x00, (char)0x00, (char)0x24, (char)0x0d, (char)0x12, (char)0xd3, (char)0x00, (char)0xff, (char)0xff, (char)0x00, (char)0xfe, (char)0xfe, (char)0xfe, (char)0xfe, (char)0xfd, (char)0xfd, (char)0xfd, (char)0xfd, (char)0x12, (char)0x34, (char)0x56, (char)0x78 };
    meet::Error err = uc.sendData(host, port, pack, sizeof(pack) / sizeof(char));
    if ((int)err) {
        std::cout << meet::getString(err) << std::endl;
    }
    std::ignore = getchar();
    uc.close();
}