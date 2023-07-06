#pragma once

#include <iostream>
#include <string>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mutex>
#include <vector>
#include <map>
#include <set>
#pragma comment (lib, "ws2_32.lib")

class TcpServer {
    static enum STATES {
        STOPPED = 1,
        ACTIVE,
        STOPPING
    };

    STATES state = STATES::STOPPED;
    std::vector<SOCKET> sockets;
    SOCKET serverSocket;
    fd_set activeFdSet;
    int x = 0;

    void acceptClient() {
        sockaddr_in client;
        int clientSize = sizeof(client);
        SOCKET socket = accept(serverSocket, (sockaddr*)&client, &clientSize);
        if (socket == INVALID_SOCKET)
        {
            if (WSAGetLastError() != WSAEWOULDBLOCK) {
                std::cout << "accept() failed with error " << WSAGetLastError() << std::endl;
                return;
            }
        }
        u_long nonBlock = 1;
        if (ioctlsocket(socket, FIONBIO, &nonBlock) == SOCKET_ERROR) {
            std::cout << "ioctlsocket(FIONBIO) failed with error " << WSAGetLastError();
        }

        char host[NI_MAXHOST];
        ZeroMemory(host, NI_MAXHOST);
        FD_SET(socket, &activeFdSet);
        sockets.push_back(socket);
        std::cout
            << "Client connected from "
            << inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST)
            << " assigned socket=" << socket
            << std::endl;
    }

    void handleSocketRead() {
        char recvbuf[4096];
        int recvbuflen = 4096;
        int iResult;
        struct timeval tv;
        tv.tv_usec = 50;
        tv.tv_sec = 0;
        fd_set readFdSet = activeFdSet;
        iResult = select(FD_SETSIZE, &readFdSet, NULL, NULL, &tv);
        if (iResult == SOCKET_ERROR) {
            throw std::exception("Error retrieving data from sockets!");
        }

        for (int i = 0; i < sockets.size(); i++) {
            // grab the socket fd at this index 
            SOCKET currSocketFd = sockets.at(i);

            if (!FD_ISSET(currSocketFd, &readFdSet)) {
                continue;
            }
            if (currSocketFd == serverSocket) {
                acceptClient();
            }
            else {
                ZeroMemory(recvbuf, recvbuflen);
                iResult = recv(currSocketFd, recvbuf, recvbuflen, 0);
                if (iResult > 0) {
                    // TODO, DO SOMETHING WITH THE DATA
                    this->x = this->x + 1;
                    std::string out = "Hi from server" + std::to_string(x);
                    sendMessage(currSocketFd, out);
                }
                else if (iResult == 0) {
                    disconnectClient(currSocketFd);
                    i--;
                }
                else {
                    printf("recv failed: %d\n", WSAGetLastError());
                    disconnectClient(currSocketFd);
                    i--;
                }
            }
        }
    }

    static void sendMessage(SOCKET& s, std::string& message) {
        send(s, message.c_str(), message.size() + 1, 0);
    }

    void disconnectClient(SOCKET s) {
        std::cout << "Disconnecting client with socket=" << s << std::endl;
        // if you reach this point, connection needs to be shutdown 
        if (shutdown(s, SD_SEND) == SOCKET_ERROR) {
            std::cout << "" << std::endl;
        }
        closesocket(s);
        FD_CLR(s, &activeFdSet);

        // remove from sockets array
        sockets.erase(std::remove_if(sockets.begin(),
            sockets.end(),
            [s](SOCKET x) { return (SOCKET)x == s; }),
            sockets.end());
    }

public:
    ~TcpServer() {
        state = STOPPING;
        WSACleanup();
        system("pause");
    }

    void start()
    {
        if (state != STOPPED) {
            throw std::exception("TcpServer is not in a stopped state");
        }
        // Initialze winsock
        WSADATA wsData;
        WORD ver = MAKEWORD(2, 2);

        int wsOk = WSAStartup(ver, &wsData);
        if (wsOk != 0)
        {
            throw std::exception("Can't Initialize winsock! Quitting...");
        }

        // Create a socket
        serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (serverSocket == INVALID_SOCKET)
        {
            WSACleanup();
            throw std::exception("Can't create a socket");
        }
        u_long nonBlock = 1;
        if (ioctlsocket(serverSocket, FIONBIO, &nonBlock) == SOCKET_ERROR) {
            printf("ioctlsocket(FIONBIO) failed with error %d\n", WSAGetLastError());
        }

        sockaddr_in hint;
        hint.sin_family = AF_INET;
        hint.sin_port = htons(1337);
        hint.sin_addr.S_un.S_addr = INADDR_ANY;

        bind(serverSocket, (sockaddr*)&hint, sizeof(hint));
        listen(serverSocket, SOMAXCONN);
        sockets.push_back(serverSocket);

        FD_ZERO(&activeFdSet);
        FD_SET(serverSocket, &activeFdSet);
        state = ACTIVE;
    }

    STATES getState() const {
        return this->state;
    }

    void spin(int waitMs) {
        if (state != ACTIVE) {
            throw std::exception("TCP Server state must be active to spin");
        }
        while (state == ACTIVE) {
            handleSocketRead();
            Sleep(waitMs);
        }
    }
};

