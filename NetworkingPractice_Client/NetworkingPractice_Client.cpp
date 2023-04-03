// NetworkingPractice_Client.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Jacob Burton 2023

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <string>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") // link with Winsock library

int main() {
    // Initialize Winsock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    // WSAStartup Error Check
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return 1;
    }

    // Create a socket for the client
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    // Socket Creation Error Check
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Connect to the server
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8888); // Port number
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server IP address

    result = connect(clientSocket, (sockaddr*)&serverAddress, sizeof(serverAddress));
    // Socket Connection Error Check
    if (result == SOCKET_ERROR) {
        std::cerr << "connection failed: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to server" << std::endl;

    // TODO: Send and receive messages to/from the server
    while (1) {

    }

    // Clean up
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}