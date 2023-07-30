#pragma once
#include "../include/Meet.hpp"

#include <iostream>
#include <fstream>
#include <chrono>

bool ClientWriteFile = false;
std::ofstream ClientWriteFileIO;

// ����һ���ͻ���
void startClient(meet::TCPClient& c) {

    //�ر�����ģʽ
    //c.setBlockingMode(false);

    //ע��Ͽ����� �ص�
    c.onDisConnect([]() {
        std::cout << "����˶Ͽ�������" << std::endl;
        });

    //ע�������Ϣ �ص�
    c.onRecvData([](ULONG64 len, const char* data) {
        if (!ClientWriteFile) {
            std::cout << "\n[���Է���� " << len << "�ֽ�]:";
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9);
            std::cout << std::string(data) << std::endl;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        }
        else {
            ClientWriteFileIO.write(data, len);
            ClientWriteFileIO.flush();
        }
        });

    // �ֶ��������ӷ���˵ĵ�ַ�˿�
    std::string ip_input, fm_input;
    USHORT port;
    for (;;) {
        std::cout << "��ֱ�����Ҫ���ӷ���˵ĵ�ַ,�˿�,Э��(4/6)(Ĭ��127.0.0.1:3000)" << std::endl;
        std::cout << "��ַ(��ȷ�����Խ���,��������):";
        std::string port_input;
        std::getline(std::cin, ip_input);
        if (ip_input == "" || ip_input == "0") {
            //��Ĭ�ϵ�������
            ip_input = "127.0.0.1";
            port = 3000;
            break;
        }
        std::cout << "�˿�:";
        std::getline(std::cin, port_input);

        auto p = atoi(port_input.c_str());
        if (p >= 65535 || p <= 0) {
            system("cls");
            std::cout << "�˿ڴ���" << std::endl;
            continue;
        }
        port = p;
        break;
    }


    meet::Error connect_error;
    //meet::IP connIp = meet::IP::getaddrinfo(ip_input);
    meet::IP connIp = ip_input;

    if (!connIp.isValid()) {
        std::cout << "��ַ��������: �����������������Ч " << ip_input << std::endl;
        return;
    }

    std::cout << "IPAddress: " << connIp.toString() << std::endl;

    // ���ӷ���� �������ӽ�������ж�
    if ((connect_error = c.connect(connIp, port)) != meet::Error::noError) {
        std::cout << "���Ӵ���:" << meet::getString(connect_error) << std::endl;
        std::cout << "debug Family: ";
        std::cout << ((connIp.IPFamily == meet::Family::IPV4) ? "IPV4" : "IPV6") << std::endl;
        return;
    }

    //����������ִ����Ӧ����
    for (;;) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
        std::cout << "0 ---- �Ͽ����Ӳ��˳��ͻ���" << std::endl;
        std::cout << "1 ---- �����ı�" << std::endl;
        std::cout << "2 ---- �����ļ�" << std::endl;
        std::cout << "3 ---- �����ļ�" << std::endl;
        std::cout << "4 ---- ֹͣ�����ļ�" << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
        std::cout << "������һ��ѡ��:";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        std::string cinput;
        std::getline(std::cin, cinput);

        if (!c.Connected) {
            std::cout << "�������ѶϿ�" << std::endl;
            return;
        }

        if (cinput == "0") {
            c.disConnect();
            c.close();
            return;
        }
        else if (cinput == "1") {
            std::cout << "��������Ҫ���͵��ı�:";
            std::string csendtext;
            std::getline(std::cin, csendtext);

            if (!c.Connected) {
                std::cout << "�����Ѿ��Ͽ�" << std::endl;
                return;
            }

            //�����ı�
            meet::Error send_error;
            if ((send_error = c.sendText(csendtext)) != meet::Error::noError) {
                std::cout << meet::getString(send_error) << std::endl;
                return;
            }
        }
        else if (cinput == "2") {
            std::cout << "��ȷ���Է�׼���ý����ļ�" << std::endl;
            std::cout << "�������ļ���:";
            std::string csendfile;
            std::getline(std::cin, csendfile);

            if (!c.Connected) {
                std::cout << "�����Ѿ��Ͽ�" << std::endl;
                return;
            }

            //�����ļ� �Զ����Ʒ�ʽ��ȡ
            std::ifstream sf(csendfile.c_str(), std::ios::binary | std::ios::in);
            if (!sf.good()) {
                std::cout << "�ļ�������,����ϸȷ��" << std::endl;
                continue;
            }

            while (true) {
                if (sf.eof()) {
                    std::cout << "�ļ�������ɡ�" << std::endl;
                    break;
                }
                char* tempStr = new char[1024];
                sf.read(tempStr, 1024);
                int readsize = static_cast<int>(sf.gcount());

                meet::Error sendFileErr = c.sendData(tempStr, readsize);
                delete[] tempStr;
                if (sendFileErr != meet::Error::noError) {
                    std::cout << "�����ļ���������:" << meet::getString(sendFileErr) << std::endl;
                    break;
                }
            }
            sf.close();

        }
        else if (cinput == "3") {
            std::cout << "�����뽫������ļ���:";
            std::string csavefile;
            std::getline(std::cin, csavefile);

            if (!c.Connected) {
                std::cout << "�����Ѿ��Ͽ�" << std::endl;
                return;
            }
            // ����һ������  ʹ������Ϣ�ص� ȷ���������������ļ��ı���

            ClientWriteFileIO.open(csavefile.c_str(), std::ios::binary);
            if (ClientWriteFileIO.is_open()) {
                ClientWriteFile = true;
                std::cout << "���ڿ�ʼ�����ļ�" << std::endl;
            }

        }
        else if (cinput == "4") {
            if (ClientWriteFile) {
                ClientWriteFileIO.close();
                ClientWriteFile = false;
                std::cout << "�ļ��ѱ���" << std::endl;
            }
        }

    }
}