#include <stdio.h>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")  //���� ws2_32.dll
#define BUF_SIZE 100
/*
    _WINSOCK_DEPRECATED_NO_WARNINGS
*/

int main() {
    //��ʼ��DLL
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    //�����׽���
    SOCKET sock = socket(PF_INET, SOCK_DGRAM, 0);

    //��������ַ��Ϣ
    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));  //ÿ���ֽڶ���0���
    servAddr.sin_family = PF_INET;
    servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servAddr.sin_port = htons(9527);

    //���ϻ�ȡ�û����벢���͸���������Ȼ����ܷ���������
    struct sockaddr fromAddr;
    int addrLen = sizeof(fromAddr);
    while (1) {
        char buffer[BUF_SIZE] = { 0 };
        printf("Input a string: ");
        gets(buffer);
        sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr*)&servAddr, sizeof(servAddr));
        int strLen = recvfrom(sock, buffer, BUF_SIZE, 0, &fromAddr, &addrLen);
        buffer[strLen] = 0;
        printf("Message form server: %s\n", buffer);
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}