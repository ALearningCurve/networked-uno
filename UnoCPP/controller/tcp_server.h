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

/// <summary>
/// Interface that specifies an async event handler for networked server events using sockets
/// </summary>
class ServerInterface {
public:
    virtual ~ServerInterface() = default;
    /// <summary>
    /// Called when a client disconnects. 
    /// </summary>
    /// <param name="s">The socket of the disconnecting client</param>
    virtual void on_disconnect(SOCKET s) = 0;

    /// <summary>
    /// Called when a client connects.
    /// </summary>
    /// <param name="s">the socket of the connecting client</param>
    virtual void on_client_connected(SOCKET s) = 0;

    /// <summary>
    /// Called when input is recieved from a client
    /// </summary>
    /// <param name="s">the socket of the client we recieved data from</param>
    /// <param name="uinput">the msg we recieved</param>
    virtual void on_input_recieved(SOCKET s, std::string uinput) = 0;
};

/// <summary>
/// Nonblocking TCP Server that can handle sending/recieving from multiple clients simulatanously. 
/// To actually do something with the server events, the ServerInterface provided upon construction
/// can be used to handle disconnect/connect/input recieved events
/// </summary>
class TcpServer {
    enum STATES {
        STOPPED = 1,
        ACTIVE,
        STOPPING
    };

    STATES state = STATES::STOPPED;
    std::vector<SOCKET> sockets;
    SOCKET server_socket = INVALID_SOCKET;
    fd_set _active_fd_set;
    ServerInterface* _server_interface = nullptr;

    /// <summary>
    /// Using the server socket, accepts a client (blocking)
    /// </summary>
    void accept_client() {
        sockaddr_in client;
        int clientSize = sizeof(client);
        SOCKET socket = accept(server_socket, (sockaddr*)&client, &clientSize);
        if (socket == INVALID_SOCKET)
        {
            if (WSAGetLastError() != WSAEWOULDBLOCK) {
                std::cout << "[X] accept() failed with error " << WSAGetLastError() << std::endl;
                return;
            }
        }
        u_long nonBlock = 1;
        if (ioctlsocket(socket, FIONBIO, &nonBlock) == SOCKET_ERROR) {
            std::cout << "[X] ioctlsocket(FIONBIO) failed with error " << WSAGetLastError();
        }

        char host[NI_MAXHOST];
        ZeroMemory(host, NI_MAXHOST);
        FD_SET(socket, &_active_fd_set);
        sockets.push_back(socket);
        std::cout
            << "[ ] Client connected from "
            << inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST)
            << " assigned SOCKET=" << socket
            << std::endl;
        _server_interface->on_client_connected(socket);
    }

    /// <summary>
    /// For all connected clients, see if data has to be read. For the server socket,
    /// see if a client is attempting to connect. This method is non-blocking.
    /// </summary>
    void handle_socket_read() {
        char recvbuf[4096];
        int recvbuflen = 4096;
        int iResult;
        struct timeval tv;
        tv.tv_usec = 50;
        tv.tv_sec = 0;
        fd_set readFdSet = _active_fd_set;
        iResult = select(FD_SETSIZE, &readFdSet, NULL, NULL, &tv);
        if (iResult == SOCKET_ERROR) {
            throw std::exception("Error retrieving data from sockets!");
        }

        for (int i = 0; i < sockets.size(); i++) {
            SOCKET currSocketFd = sockets.at(i);
            if (!FD_ISSET(currSocketFd, &readFdSet)) {
                continue;
            }
            if (currSocketFd == server_socket) {
                accept_client();
            }
            else {
                ZeroMemory(recvbuf, recvbuflen);
                iResult = recv(currSocketFd, recvbuf, recvbuflen, 0);
                if (iResult > 0) {
                    std::string data(recvbuf);
                    std::cout << "[ ] Recieved data from SOCKET=" << currSocketFd << ": \"" << data << "\"" << std::endl;
                    _server_interface->on_input_recieved(currSocketFd, data);
                }
                else if (iResult == 0) {
                    disconnect_client(currSocketFd);
                    i--;
                }
                else {
                    std::cout << "[X] recv failed for SOCKET=" << currSocketFd << ": " << WSAGetLastError() << std::endl;
                    disconnect_client(currSocketFd);
                    i--;
                }
            }
        }
    }

    /// <summary>
    /// Disconnects a client from the server
    /// </summary>
    /// <param name="s">client socket to disconnect</param>
    void disconnect_client(SOCKET s) {
        std::cout << "[ ] Disconnecting client with SOCKET=" << s << std::endl;
        // if you reach this point, connection needs to be shutdown 
        if (shutdown(s, SD_SEND) == SOCKET_ERROR) {
            // maybe hanndle this error, but likely this means the client diconnected anyway
            // so for now do nothing
        }
        closesocket(s);
        FD_CLR(s, &_active_fd_set);

        // remove from sockets array
        sockets.erase(std::remove_if(sockets.begin(),
            sockets.end(),
            [s](SOCKET x) { return (SOCKET)x == s; }),
            sockets.end());
        _server_interface->on_disconnect(s);
    }

public:
    ~TcpServer() {
        state = STOPPING;
        WSACleanup();
        system("pause");
    }

    /// <summary>
    /// Create a server that will handle all server events via the specified server interface
    /// </summary>
    /// <param name="serverInterface">Interface to use to hook onto server events</param>
    TcpServer(ServerInterface* serverInterface): _server_interface(serverInterface) {
        if (serverInterface == nullptr) {
            throw std::invalid_argument("Must provide a pointer to a server interface");
        }
        FD_ZERO(&_active_fd_set);
    }

    /// <summary>
    /// Starts the TCP Server on port 1337 on all available network interfaces. 
    /// </summary>
    void start()
    {
        std::cout << "[ ] Starting TCP Server" << std::endl;
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
        server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (server_socket == INVALID_SOCKET)
        {
            WSACleanup();
            throw std::exception("Can't create a socket");
        }
        u_long nonBlock = 1;
        if (ioctlsocket(server_socket, FIONBIO, &nonBlock) == SOCKET_ERROR) {
            printf("ioctlsocket(FIONBIO) failed with error %d\n", WSAGetLastError());
        }

        sockaddr_in hint;
        hint.sin_family = AF_INET;
        hint.sin_port = htons(1337);
        hint.sin_addr.S_un.S_addr = INADDR_ANY;

        bind(server_socket, (sockaddr*)&hint, sizeof(hint));
        listen(server_socket, SOMAXCONN);
        sockets.push_back(server_socket);

        FD_ZERO(&_active_fd_set);
        FD_SET(server_socket, &_active_fd_set);
        state = ACTIVE;
        std::cout << "[ ] Started TCP Server on 0.0.0.0:1337" << std::endl;
    }

    /// <summary>
    /// Returns the current state of the server
    /// </summary>
    /// <returns>state of the server</returns>
    STATES get_state() const {
        return this->state;
    }

    /// <summary>
    /// While server is active, attempts to handle reading input from clients as well as 
    /// any connect and disconnect events. Will spin as long as the server is active. Each
    /// spin handles the connect/disconnect/recieved data events.
    /// </summary>
    /// <param name="waitMs">the number of milliseconds to wait after each spin before
    /// starting the next spin</param>
    void spin(int waitMs) {
        if (state != ACTIVE) {
            throw std::exception("TCP Server state must be active to spin");
        }
        while (state == ACTIVE) {
            handle_socket_read();
            Sleep(waitMs);
        }
    }

    /// <summary>
    /// Sends a message to the specified client
    /// </summary>
    /// <param name="s">socket to the send the message on</param>
    /// <param name="message">the message to send</param>
    void send_client_message(SOCKET& s, const std::string& message) {
        std::cout << "[ ] Sending SOCKET=" << s << " the message: \"" << message << "\"" << std::endl;
        send(s, message.c_str(), message.size() + 1, 0);
    }
};

