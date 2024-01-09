#pragma once
#include "../include/Meet.hpp"
#include <iostream>


#define NUM 100

meet::TCPClient clients[NUM];

void pause() { system("pause"); }



void startClientx100000() {
	//询问连接建立情况
	std::string ip_input;
	USHORT port;
    int failCount = 0;

    std::cout << "请分别输入要连接服务端的地址,端口,协议(4/6)(默认127.0.0.1:3000)" << std::endl;
    std::cout << "地址(请确保可以解析,否则会崩溃):";
    std::string port_input;
    std::getline(std::cin, ip_input);
    if (ip_input == "" || ip_input == "0") {
        //按默认的来连接
        ip_input = "127.0.0.1";
        port = 3000;
    }
    else {
        std::cout << "端口:";
        std::getline(std::cin, port_input);

        auto p = atoi(port_input.c_str());
        if (p >= 65535 || p <= 0) {
            system("cls");
            std::cout << "端口错了" << std::endl;
            std::cout << "结束" << std::endl;
            return;
        }
        port = p;
    }
    meet::IP connIp = ip_input;
    if (!connIp.isValid()) {
        std::cout << "地址解析错误: 你输入的域名可能无效 " << ip_input << std::endl;
        return;
    }

	for (size_t i = 0; i < NUM; i++)
	{
		//clients[i] = new meet::TCPClient;
        clients[i].setBlockingMode(true);
        meet::Error connect_error = clients[i].connect(connIp, port);
        if (connect_error != meet::Error::noError) {
            failCount++;
            std::cout << "第" + std::to_string(i) << "号客户端连接失败 Err:" << meet::getString(connect_error);
            continue;
        }
        else {
            std::cout << i << std::endl;
        }
	}

    std::cout << "连接失败的客户端数: "<< failCount << std::endl;
    std::cout << "按下回车, 发送消息 " << std::endl;
    pause();

    for (size_t i = 0; i < NUM; i++)
    {
        if (clients[i].isConnected()) {
            clients[i].sendText("客户端" + std::to_string(i) + "发送消息");
        }
    }

    std::cout << "按下回车, 断开连接 " << std::endl;
    pause();

    for (size_t i = 0; i < NUM; i++)
    {
        if (clients[i].isConnected()) {
            clients[i].disConnect();
        }
        //delete clients[i];
    }
}