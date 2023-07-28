#pragma once
#include "../include/Meet.hpp"

#include <iostream>
#include <fstream>
#include <chrono>


bool ServerWriteFile = false;
std::string ServerWriteFileIP;
USHORT ServerWriteFilePort;
std::ofstream ServerWriteFileIO;

// ����һ��TCP�ķ���
void startServer(meet::TCPServer& s) {

    meet::Family fa = meet::Family::IPV4;
    USHORT port = 3000;
    int maxconn = 5;
    std::cout << "��������Ҫ������ַ��Э��(4/6):";
    std::string fa_input;
    std::getline(std::cin, fa_input);
    if (fa_input == "" || fa_input == "0") {
        std::cout << "��ʹ��Ĭ����(0.0.0.0 : 3000  maxconnect 5)" << std::endl;
    }
    else {
        if (fa_input == "6") {
            fa = meet::Family::IPV6;
        }
        std::cout << "��������Ҫ�����˿�:";
        std::string port_input;
        std::getline(std::cin, port_input);
        auto p = atoi(port_input.c_str());
        if (p >= 65535 || p <= 0) {
            std::cout << "����Ķ˿�������" << std::endl;
            return;
        }
        port = p;

        std::cout << "����������������ӵĿͻ�������:";
        std::string count_input;
        std::getline(std::cin, count_input);
        auto count = atoi(count_input.c_str());
        if (count <= 0) {
            std::cout << "������ʲô��,������ѡ��,�͡���10����" << std::endl;
            maxconn = 10;
        }
        else {
            maxconn = count;
        }

    }

    // ����Ϊ����ģʽ
    s.setBlockingMode(true);

    // �����ͻ��˶Ͽ����ӵ���Ϣ
    s.onClientDisConnect([](meet::TCPServer::MeetClient meetClient) {
        printf("\n[%s -:- %d][����] �Ͽ�����\n", meetClient.addr.toString().c_str(), meetClient.port);
        });

    // �����ͻ������ӵ���Ϣ
    s.onNewClientConnect([](meet::TCPServer::MeetClient meetClient /*meet::IP ip, USHORT port, SOCKET socket*/) {
        printf("\n[%s -:- %d][����] ���ӳɹ�\n", meetClient.addr.toString().c_str(), meetClient.port);
        });

    // �������ݵ���ʱ������Ϣ
    s.onRecvData([](meet::TCPServer::MeetClient meetClient /*meet::IP ip, USHORT port, SOCKET socket*/, ULONG64 len, const char* data) {
        if (ServerWriteFile && ServerWriteFileIP == meetClient.addr.toString() && ServerWriteFilePort == meetClient.port) {
            ServerWriteFileIO.write(data, len);
            ServerWriteFileIO.flush();
        }
        else {
            printf("\n[%s -:- %d][����][%I64d�ֽ�]:", meetClient.addr.toString().c_str(), meetClient.port, len);
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9);
            std::cout << std::string(data) << std::endl;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        }

        });

    // ������ַ �Լ���v4��ַ�� v6��ַ��������
    if (fa == meet::Family::IPV6) {
        s.setListenAddress("::");
    }

    // ���÷���˵Ŀͻ������Ӷ��е����ֵ
    s.setMaxConnectCount(maxconn);

    // ��ʼ����
    meet::Error listen_err = s.Listen(port);
    if (listen_err != meet::Error::noError) {
        std::cout << "��������:" << meet::getString(listen_err) << std::endl;
        return;
    }

    for (;;) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
        std::cout << "0 ---- �Ͽ��������Ӳ��˳������" << std::endl;
        std::cout << "1 ---- ѡ��һ���ͻ���,�����в���" << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
        std::cout << "������һ��ѡ��:";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        std::string sinput;
        std::getline(std::cin, sinput);

        if (sinput == "0") {
            auto& clientList = s.GetALLClient();
            int discount = 0;
            size_t clientcount = clientList.size();

            for (int i = clientcount - 1; i >= 0; i--) {
                if (s.disClientConnect(clientList[i].addr, clientList[i].port) == meet::Error::noError) {
                    discount++;
                }
            }
            std::cout << "���ɹ��Ͽ��� " << discount << "/" << clientcount << " ���ͻ��˵�����" << std::endl;
            std::cout << "debug clientList size: " << clientList.size() << std::endl;
            s.Close();
            break;
        }
        else if (sinput == "1") {
            for (;;) {
                int a = 0;
                auto& clientList = s.GetALLClient();
                if (clientList.size() == 0) {
                    std::cout << "��ǰû�пͻ�������" << std::endl;
                    break;
                }

                std::cout << std::endl;
                for (int i = 0; i < clientList.size(); i++) {
                    std::cout << i << " ========== [" << clientList.at(i).addr.toString() << ":" << clientList.at(i).port << "]" << std::endl;
                }
                std::cout << clientList.size() << " ========== [������һҳ] " << std::endl;
                std::cout << "������һ�����ѡ��һ���ͻ���:";
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
                    // ����̨����ѡ����һ���ͻ���
                    meet::TCPServer::MeetClient client = clientList.at(x);
                    // ��ѡ��Ŀͻ��˽��в���
                    for (;;) {
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
                        std::cout << "��ǰѡ��Ŀͻ�����:[" << client.addr.toString() << ":" << client.port << "]" << std::endl;
                        std::cout << "0 ---- �Ͽ�����ͻ���,��������һҳ" << std::endl;
                        std::cout << "1 ---- ������һҳ" << std::endl;
                        std::cout << "2 ---- ������Ϣ" << std::endl;
                        std::cout << "3 ---- �����ļ�" << std::endl;
                        std::cout << "4 ---- �����ļ�" << std::endl;
                        std::cout << "5 ---- ֹͣ�����ļ�" << std::endl;
                        std::cout << "6/cls ---- ����" << std::endl;
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
                        std::cout << "������һ��ѡ��:";
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                        std::string sinput_setup;
                        std::getline(std::cin, sinput_setup);

                        if (x >= clientList.size() || clientList.at(x).clientSocket != client.clientSocket) {
                            std::cout << "�ÿͻ��˵��������ѶϿ�" << std::endl;
                            break;
                        }
                        if (sinput_setup == "0") {
                            meet::Error err = s.disClientConnect(client.addr, client.port);
                            if (err != meet::Error::noError) {
                                std::cout << "�ͻ��˶Ͽ�ʧ��:" << meet::getString(err) << std::endl;
                            }
                            s.Close();
                            break;
                        }
                        else if (sinput_setup == "1") {
                            break;
                        }
                        else if (sinput_setup == "2") {
                            std::cout << "��������Ҫ���͵��ı�:";
                            std::string ssendtext;
                            std::getline(std::cin, ssendtext);

                            meet::Error err;
                            if ((err = s.sendText(client.clientSocket, ssendtext)) != meet::Error::noError) {
                                std::cout << "������Ϣ���ִ���" << meet::getString(err) << std::endl;
                            }
                        }
                        else if (sinput_setup == "3") {
                            std::cout << "��ȷ���Է�׼���ý����ļ�" << std::endl;
                            std::cout << "�������ļ���:";
                            std::string ssendfile;
                            std::getline(std::cin, ssendfile);

                            //�����ļ� �Զ����Ʒ�ʽ��ȡ
                            std::ifstream sf(ssendfile.c_str(), std::ios::binary | std::ios::in);
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

                                meet::Error sendFileErr = s.sendData(client.clientSocket, tempStr, readsize);
                                if (sendFileErr != meet::Error::noError) {
                                    std::cout << "�����ļ���������:" << meet::getString(sendFileErr) << std::endl;
                                    break;
                                }
                            }
                            sf.close();
                        }
                        else if (sinput_setup == "4") {

                            std::cout << "�����뽫������ļ���:";
                            std::string ssavefile;
                            std::getline(std::cin, ssavefile);


                            ServerWriteFileIO.open(ssavefile, std::ios::binary);
                            if (ServerWriteFileIO.is_open()) {
                                ServerWriteFile = true;
                                ServerWriteFileIP = client.addr.toString();
                                ServerWriteFilePort = client.port;
                                std::cout << "��ʼ�����ļ�" << std::endl;
                            }
                        }
                        else if (sinput_setup == "5") {
                            if (ServerWriteFile) {
                                ServerWriteFileIO.close();
                                ServerWriteFile = false;
                                std::cout << "�ļ��Ѿ�����" << std::endl;
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