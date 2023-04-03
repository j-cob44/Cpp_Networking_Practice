// NetworkingPractice_Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Jacob Burton 2023

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib") // link with Winsock library

#define PORT 8888

int main()
{
    // Initialize Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    // Winsock Initialization Error Check
    if (iResult != 0) {
        std::cerr << "WSAStartup failed: " << iResult << std::endl;
        return 1;
    }

    // Create a socket for the server on TCP
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    // Server Socket Creation Error Check
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Bind the socket to a local address and port
    sockaddr_in localAddress;
    localAddress.sin_family = AF_INET; // IPv4
    localAddress.sin_port = htons(PORT); // Port number
    localAddress.sin_addr.s_addr = htonl(INADDR_ANY); // Any local address

    iResult = bind(serverSocket, (sockaddr*)&localAddress, sizeof(localAddress));
    // Server Socket Binding Error Check
    if (iResult == SOCKET_ERROR) {
        std::cerr << "Socket binding failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    iResult = listen(serverSocket, SOMAXCONN);
    // Server Listening Error Check
    if (iResult == SOCKET_ERROR) {
        std::cerr << "Listening failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server started at: localhost:" << PORT << std::endl;

    // Accept incoming connections and handle them
    while (true) {
        // Initialize client information
        sockaddr_in clientAddress;
        int clientAddressLength = sizeof(clientAddress);

        // Accept Client Connections
        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddress, &clientAddressLength);
        // Client Connection Error Check
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "accept failed: " << WSAGetLastError() << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }

        char ipStr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddress.sin_addr, ipStr, INET_ADDRSTRLEN);
        std::cout << "New client connected from " << ipStr << std::endl;

        // TODO: Handle client connection

    }

    // Clean up
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
