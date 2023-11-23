#undef UNICODE
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

void ReceiveMessages(SOCKET ClientSocket) {
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;

    do {
        iResult = recv(ClientSocket, recvbuf, DEFAULT_BUFLEN, 0);
        if (iResult > 0) {
            printf("\nClient says: %.*s\n", iResult, recvbuf);
        }
        else if (iResult == 0) {
            printf("Connection closed by client.\n");
            break;
        }
        else {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            break;
        }
    } while (1);
}

int main() {
    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo* result = NULL;
    struct addrinfo hints;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    printf("Server is waiting for incoming connections...\n");

    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    closesocket(ListenSocket);

    // «апускаем поток дл€ приема сообщений от клиента
    std::thread receiveThread(ReceiveMessages, ClientSocket);

    // ќсновной поток дл€ отправки сообщений от сервера
    char sendbuf[DEFAULT_BUFLEN];
    do {
        printf("Enter a message (or type 'exit' to quit): ");
        fgets(sendbuf, DEFAULT_BUFLEN, stdin);

        iResult = send(ClientSocket, sendbuf, strlen(sendbuf), 0);
        if (iResult == SOCKET_ERROR) {
            printf("send failed with error: %d\n", WSAGetLastError());
            break;
        }

        if (strcmp(sendbuf, "exit\n") == 0) {
            printf("Server is closing the connection.\n");
            break;
        }

    } while (1);

    // ƒожидаемс€ завершени€ потока приема сообщений от клиента
    receiveThread.join();

    closesocket(ClientSocket);
    WSACleanup();

    return 0;
}
