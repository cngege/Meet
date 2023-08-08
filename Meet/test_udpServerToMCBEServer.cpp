#pragma once
#include "../include/Meet.hpp"
#include <iostream>


void StartUDPServerToMCBEServer(meet::IP remoteIp, u_short remotePort) {

	meet::UDPServer us;
	meet::UDPClient uc;

	meet::IP localIp;
	u_short localPort = 0;



	// 服务端数据接收 来自本地的数据
	us.OnRecvData([&](UINT64 len, const char* data, meet::IP ip, u_short port) {
		//if (localPort == 0) {
			localIp = ip;
			localPort = port;
		//}
		if (localIp == ip && localPort == port) {
			meet::Error err = uc.sendData(remoteIp, remotePort, data, static_cast<int>(len));
			if ((int)err) {
				std::cout << "本地发送远端错误: " << meet::getString(err) << std::endl;
			}
		}
		});

	//客户端数据接收 来自远端的数据
	uc.OnRecvData([&](UINT64 len, const char* data, meet::IP ip, u_short port) {
		if (localPort != 0) {
			meet::Error err = us.sendData(localIp, localPort, data, static_cast<int>(len));	   // 这里后面要做分解 兼容UINT64 而不是强转
			if ((int)err) {
				std::cout << "远端发送本地错误: " << meet::getString(err) << std::endl;
			}
		}
		});

	//us.setBlockingMode(false);
	//uc.setBlockingMode(false);

	meet::Error usinitErr = us.InitAndBind("0.0.0.0", 19130);
	if ((int)usinitErr) {
		std::cout << "InitAndBind Error: " << meet::getString(usinitErr) << std::endl;
	}
	
	meet::Error ucinitErr = uc.Init();
	if ((int)ucinitErr) {
		std::cout << "ucinitErr: " << meet::getString(ucinitErr) << std::endl;
	}

	std::cout << " 输入任意按键 停止代理. " << std::endl;
	std::ignore = getchar();
	uc.close();
	us.close();
}