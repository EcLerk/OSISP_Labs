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

void ReceiveMessages(SOCKET ConnectSocket) {
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;

    do {
        iResult = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
        if (iResult > 0) {
            printf("\nServer says: %.*s\n", iResult, recvbuf);
        }
        else if (iResult == 0) {
            printf("Connection closed by server.\n");
            break;
        }
        else {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            break;
        }
    } while (1);
}

int main() {
    WSADATA wsaData;
    int iResult;

    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL, * ptr = NULL, hints;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    iResult = getaddrinfo("localhost", DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    printf("Connected to the server. Start typing messages...\n");

    // «апускаем поток дл€ приема сообщений от сервера
    std::thread receiveThread(ReceiveMessages, ConnectSocket);

    // ќсновной поток дл€ отправки сообщений от клиента
    char sendbuf[DEFAULT_BUFLEN];
    do {
        printf("Enter a message (or type 'exit' to quit): ");
        fgets(sendbuf, DEFAULT_BUFLEN, stdin);

        iResult = send(ConnectSocket, sendbuf, strlen(sendbuf), 0);
        if (iResult == SOCKET_ERROR) {
            printf("send failed with error: %d\n", WSAGetLastError());
            break;
        }

        if (strcmp(sendbuf, "exit\n") == 0) {
            printf("Client is closing the connection.\n");
            break;
        }

    } while (1);

    // ƒожидаемс€ завершени€ потока приема сообщений от сервера
    receiveThread.join();

    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}
