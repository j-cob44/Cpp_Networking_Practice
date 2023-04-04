// NetworkingPractice_Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Jacob Burton 2023
// With help from https://beej.us/guide/bgnet/ and https://learn.microsoft.com/en-us/windows/win32/api/winsock

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <vector>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib") // link with Winsock library

#define PORT 8888

// Server will send data asynchronously 
void handleSending(SOCKET clientSocket, sockaddr_in clientAddress) {
    char ipStr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientAddress.sin_addr, ipStr, INET_ADDRSTRLEN);

    bool clientPresent = true;
    while (clientPresent) {
        Sleep(1000); // Wait 1 second

        // Ping Client
        char buffer[1024] = ".Ping.";
        int len = strlen(buffer);
        int send_status = send(clientSocket, buffer, len, 0);
        // Error Check ping to see if client is still there,
        if (send_status == 0) {
            // Socket Gracefully Closed
            clientPresent = false;
        }
        else if (send_status == SOCKET_ERROR) {
			// Socket Error
			clientPresent = false;
		}
        else {
            std::cout << "Sent: " << buffer << " To: " << ipStr << std::endl;
        }
    }
}

// Server will receive data asynchronously
void handleReceiving(SOCKET clientSocket, sockaddr_in clientAddress) {
    char ipStr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientAddress.sin_addr, ipStr, INET_ADDRSTRLEN);

    char buffer[1024] = "";
    bool clientPresent = true;
    while (clientPresent) {
        Sleep(1000); // Wait 1 second

        // Receive Client Response
        ZeroMemory(buffer, 1024);
        int recv_status = recv(clientSocket, buffer, 1024, 0);
        // If data is received, print it out, if recv_status == 0, client has disconnected
        if(recv_status > 0) {
            std::cout << "Received: " << buffer << " From: " << ipStr << std::endl;
        }
        else if (recv_status == 0) {
            // Socket Gracefully Closed
            clientPresent = false;
        }
        else {
            // Socket Error
            clientPresent = false;
        }
            
        ZeroMemory(buffer, 1024);
    }
}

// Function to handle client connections, runs in a separate threads for multiple connections
void handleClient(SOCKET clientSocket, sockaddr_in clientAddress) {
    char ipStr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientAddress.sin_addr, ipStr, INET_ADDRSTRLEN);
    std::cout << "New client connected from " << ipStr << std::endl;

    // Create Threads for client
    std::vector<std::thread> clientActionsThread;

    // Add Thread for Sending and Receiving
    clientActionsThread.emplace_back(handleSending, clientSocket, clientAddress);
    clientActionsThread.emplace_back(handleReceiving, clientSocket, clientAddress);

    int i = 0;
    bool clientPresent = true;
    while (clientPresent) {
        // Wait 5 seconds
        Sleep(5000);

        // Check if the Connection is still alive
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(clientSocket, &readSet);
        timeval timeout = { 0, 0 }; // timeout of 0 seconds
        int selectResult = select(0, &readSet, nullptr, nullptr, &timeout);
        if (selectResult == SOCKET_ERROR) {
            std::cerr << "Select failed: " << WSAGetLastError() << std::endl;
            clientPresent = false;
        }
        else if (selectResult == 0) {
            // Socket healthy
		}
        else {
			// Socket Error
			clientPresent = false;
        }
    }
    // Close the socket and clean up
    closesocket(clientSocket);

    // Close client action threads
    for (auto& i : clientActionsThread) {
        i.join();
    }

    std::cout << "Client " << ipStr << " Disconnected." << std::endl;
}

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

    // Vector of Client Threads
    std::vector<std::thread> clientThreads;

    // Accept incoming connections and handle them
    while (true) {
        // Initialize client information
        sockaddr_in clientAddress;
        int clientAddressLength = sizeof(clientAddress);

        // Accept Client Connections
        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddress, &clientAddressLength);
        // Client Connection Error Check
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }

        // Start a new thread to handle a client's connection
        clientThreads.emplace_back(handleClient, clientSocket, clientAddress);
    }

    // Clean up
    closesocket(serverSocket);

    // Close client threads
    for (auto& i : clientThreads) {
        i.join();
    }

    WSACleanup();
    return 0;
}
