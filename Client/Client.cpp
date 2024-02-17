#define _CRT_SECURE_NO_WARNINGS         // �ֽ� VC++ ������ �� ��� ����
#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

#define BUFSIZE    1024

// ���� �Լ� ���� ��� �� ����
void err_quit(char* msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
    LocalFree(lpMsgBuf);
    exit(1);
}

// ���� �Լ� ���� ���
void err_display(char* msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    printf("[%s] %s", msg, (char*)lpMsgBuf);
    LocalFree(lpMsgBuf);
}

// ���� ������ �Լ�
int _recv_ahead(SOCKET s, char* p)
{
    __declspec(thread) static int nbytes = 0;
    __declspec(thread) static char buf[1024];
    __declspec(thread) static char* ptr;

    if (nbytes == 0 || nbytes == SOCKET_ERROR) {
        nbytes = recv(s, buf, sizeof(buf), 0);
        if (nbytes == SOCKET_ERROR) {
            return SOCKET_ERROR;
        }
        else if (nbytes == 0)
            return 0;
        ptr = buf;
    }

    --nbytes;
    *p = *ptr++;
    return 1;
}
// ����� ���� ������ ���� �Լ�
int recvline(SOCKET s, char* buf, int maxlen)
{
    int n, nbytes;
    char c, * ptr = buf;

    for (n = 1; n < maxlen; n++) {
        nbytes = _recv_ahead(s, &c);
        if (nbytes == 1) {
            *ptr++ = c;
            if (c == '\n')
                break;
        }
        else if (nbytes == 0) {
            *ptr = 0;
            return n - 1;
        }
        else
            return SOCKET_ERROR;
    }

    *ptr = 0;
    return n;
}

int main() {

    int retval;

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) err_quit("socket()");

    // get SERVERIP, SERVERPORT
    int SERVERPORT = 2500;
    char SERVERIP[512] = { 0 }, PORT[512] = { 0 };

    printf("Server IP(default: 127.0.0.1): ");
    scanf("%[^\n]", &SERVERIP);
    if (!strcmp(SERVERIP, "\0")) strcpy(SERVERIP, "127.0.0.1");
    fflush(stdin);
    printf("port(default: 2500): ");
    scanf("%[^\n]", &PORT);
    if (strcmp(PORT, "\0")) SERVERPORT = atoi(PORT);

    // connect()
    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) err_quit("connect()");
    printf("\nConnected to %s \n", inet_ntoa(serveraddr.sin_addr));

    char buf[BUFSIZE + 1];

    while (1) {
        retval = recvline(sock, buf, BUFSIZE + 1);
        if (retval == SOCKET_ERROR) {
            err_display("recv()");
            break;
        }
        else if (retval == 0)
            break;
        printf("%s", buf);

    }
    closesocket(sock);
    WSACleanup();
    return 0;
}