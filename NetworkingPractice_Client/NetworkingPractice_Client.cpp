// NetworkingPractice_Client.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Jacob Burton 2023
// With help from https://beej.us/guide/bgnet/ and https://learn.microsoft.com/en-us/windows/win32/api/winsock

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") // link with Winsock library

// Handle Sending
void handleSending(SOCKET clientSocket) {
    bool connected = true;
    while (connected) {
        // Send Message to the Server
        std::string message;
        std::getline(std::cin, message);
        if (message == "exit") {
            connected = false;
        }
        else if (!message.empty()) {
            int send_status = send(clientSocket, message.c_str(), 1024, 0);
            if (send_status == SOCKET_ERROR) {
                std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
                connected = false;
            }
        }
        else {
            int send_status = send(clientSocket, "", 0, 0);
            if (send_status == SOCKET_ERROR) {
                std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
                connected = false;
            }
        }
    }

    // Either by error or by choice, close connection
    closesocket(clientSocket);
}

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
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
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
        std::cerr << "Connection failed: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to server" << std::endl;

    // Vector of threads for sending
    std::thread sendingThread(handleSending, clientSocket);

    bool connected = true;
    char buffer[1024];
    while (connected) {
        // Client is constantly receiving data from the server, if something happens we will find out here
        ZeroMemory(buffer, 1024);
        int recv_status = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (recv_status == SOCKET_ERROR) {
            //std::cerr << "Receive failed: " << WSAGetLastError() << std::endl;
            connected = false;
        }
        else if (recv_status == 0) {
            //std::cout << "Server disconnected" << std::endl;
            connected = false;
        }
        else {
            std::cout << "Server says: " << buffer << std::endl;
        }

        ZeroMemory(buffer, 1024);
    }

    // End Sending Thread
    sendingThread.join();

    std::cout << "Disconnected from Server." << std::endl;

    // Clean up
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}