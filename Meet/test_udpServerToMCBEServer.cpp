#pragma once
#include "../include/Meet.hpp"
#include <iostream>


void StartUDPServerToMCBEServer(meet::IP remoteIp, u_short remotePort) {

	meet::UDPServer us;
	meet::UDPClient uc;

	meet::IP localIp;
	u_short localPort = 0;



	// ��������ݽ��� ���Ա��ص�����
	us.OnRecvData([&](UINT64 len, const char* data, meet::IP ip, u_short port) {
		//if (localPort == 0) {
			localIp = ip;
			localPort = port;
		//}
		if (localIp == ip && localPort == port) {
			meet::Error err = uc.sendData(remoteIp, remotePort, data, static_cast<int>(len));
			if ((int)err) {
				std::cout << "���ط���Զ�˴���: " << meet::getString(err) << std::endl;
			}
		}
		});

	//�ͻ������ݽ��� ����Զ�˵�����
	uc.OnRecvData([&](UINT64 len, const char* data, meet::IP ip, u_short port) {
		if (localPort != 0) {
			meet::Error err = us.sendData(localIp, localPort, data, static_cast<int>(len));	   // �������Ҫ���ֽ� ����UINT64 ������ǿת
			if ((int)err) {
				std::cout << "Զ�˷��ͱ��ش���: " << meet::getString(err) << std::endl;
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

	std::cout << " �������ⰴ�� ֹͣ����. " << std::endl;
	std::ignore = getchar();
	uc.close();
	us.close();
}