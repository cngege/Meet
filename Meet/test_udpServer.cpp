#pragma once
#include <iostream>

#include "../include/Meet.hpp"


bool setUplisten(meet::UDPServer& us) {
	meet::IP ip;
	u_short port = 0;

	for (;;) {
		std::cout << "UDPServer 监听设置" << std::endl;
		std::string userinput;

		// 输入监听地址
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
		std::cout << "设置监听地址(默认0.0.0.0): ";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
		std::getline(std::cin, userinput);
		if (userinput.empty()) userinput = "0.0.0.0";
		ip = userinput;
		if (!ip.isValid()) {
			std::cout << "输入的IP地址无效" << std::endl;
			continue;
		}

		// 输入监听端口
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
		std::cout << "设置端口(输入0退出): ";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
		std::getline(std::cin, userinput);
		auto p = atoi(userinput.c_str());
		if (p >= 65535 || p < 0) {
			std::cout << "端口非法." << std::endl;
			continue;
		}
		if (p == 0) {
			std::cout << "退出." << std::endl;
			return false;
		}
		port = p;

		meet::Error listenErr = us.InitAndBind(ip, port);
		if ((int)listenErr) {
			std::cout << "InitAndBind 错误: " << meet::getString(listenErr) << " WSAGetLastError: " << WSAGetLastError() << std::endl;
			DebugBreak();
			return false;
		}
		break;
	}
	return true;
}

void manageClients(meet::UDPServer& us, std::vector<std::pair<meet::IP, u_short>>& clients) {

}



void startUDPServer() {
	
	bool autocls = false;
	std::string userinput;
	std::vector < std::pair<meet::IP, u_short>>Clients;
	
	meet::UDPServer us;
	us.OnRecvData([&](UINT64 len, const char* data, meet::IP ip, u_short port) {

		for (int i = 0; i < Clients.size(); i++) {
			if (Clients.at(i).first == ip && Clients.at(i).second == port) {
				break;
			}
			if (i == Clients.size() - 1) {
				Clients.push_back({ ip, port });
			}
		}
		if (Clients.size() == 0) {
			Clients.push_back({ ip, port });
		}

		std::cout << "[recv][" << ip.toString();
		std::cout << "/" << std::to_string(port) << "]";
		std::cout << "(" << std::to_string(len) << ") ";
		std::cout << data << std::endl;
		});

	if (!setUplisten(us)) {
		us.close();
		return;
	}

	for (;;) {
		if (autocls) { system("cls"); }

		std::cout << "UDPServer - 控制台" << std::endl;
		if (Clients.size()) {
			std::cout << "连接记录:(" << Clients.size() << ") " << std::endl;
			for (int i = 0; i < Clients.size(); i++) {
				std::cout << "  -  " << Clients.at(i).first.toString() << " / " << Clients.at(i).second << std::endl;
			}
		}
		std::cout << "0. 关闭并退出" << std::endl;
		std::cout << "1. 管理连接成员" << Clients.size() << std::endl;
		std::cout << "2. 自动清屏(" << autocls << ")" << std::endl;
		std::cout << "3. 转发MC服务器(预设)" << std::endl;
		std::cout << "4/cls. 清屏" << std::endl;

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
		std::cout << "请输入: ";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
		std::getline(std::cin, userinput);

		if (userinput == "0") {
			us.close();
			break;
		}
		if (userinput == "1") {
			manageClients(us, Clients);
		}
		if (userinput == "2") {
			autocls = !autocls;
		}
		if (userinput == "3") {
			
		}
		if (userinput == "4" || userinput == "cls") {
			system("cls");
			continue;
		}
	}
}