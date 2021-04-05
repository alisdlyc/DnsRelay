//
// Created by alisdlyc on 2020/8/14.
//
#ifdef WIN32
#include <stdio.h>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <errno.h>
#pragma comment(lib, "ws2_32.lib")

#else
#include <sys/socket.h>   //connect,send,recv,set sockopt等
#include <sys/types.h>
#include <netinet/in.h>     // sock_addr_in, "man 7 ip" ,htons
#include <arpa/inet.h>   //inet_addr,inet_a ton
#include <stdio.h>
#include <string.h>          // memset
#include <stdlib.h>
#include <errno.h>
typedef int SOCKET;
typedef unsigned char BYTE;
typedef unsigned long DWORD;
int INVALID_SOCKET = -1;
typedef enum { false = 0, true = 1 } BOOL;
#endif

#define BUF_SIZE 1024
#define DNSPORT 53
typedef unsigned short U16;
//const char DNSIP[] = "114.114.114.114";
const char DNSIP[] = "8.8.8.8";

typedef struct DNSHeader
{
    U16 id;
    U16 tag;
    U16 numq;
    U16 numa;
    U16 numa1;
    U16 numa2;
}DnsHeader;

typedef struct DNSQuestion
{
    U16 type;
    U16 classes;
}DnsQuestion;

//BOOL WinSockInit() {
//    WSADATA wsaData = { 0 };
//    // 初始化成功时返回0
//    if (WSAStartup(MAKEWORD(2, 2), &wsaData) == NOERROR) {
//        return TRUE;
//    }
//    else {
//        return FALSE;
//    }
//}

char* GetRemoteDns(char* domain) {
    char buf[BUF_SIZE];

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (INVALID_SOCKET == sock) {
        return (char*)"Socket 创建失败";
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(DNSPORT);
    //addr.sin_addr.s_addr = inet_addr(DNSIP);
    addr.sin_addr.S_un.S_addr = inet_addr(DNSIP);

    memset(buf, 0, BUF_SIZE);

    // 构建DNS Header的相关信息
    DnsHeader* header = (DnsHeader*)buf;
    header->id = (U16)1;
    header->tag = htons(0x0100);
    header->numq = htons(1);
    header->numa = 0;

    // 将域名信息编码为长度－值的形式
    strncpy(buf + sizeof(DnsHeader) + 1, domain, strlen(domain) + 1);
    char* p = buf + sizeof(DnsHeader) + 1;
    int i = 0;
    while (p < (buf + sizeof(DnsHeader) + 1 + strlen(domain))) {
        if (*p == '.') {
            *(p - i - 1) = i;
            i = 0;
        }
        else {
            i++;
        }
        p++;
    }
    *(p - i - 1) = i;

    // 构建DNS Question的相关信息
    DnsQuestion* question = (DnsQuestion*)(buf + sizeof(DnsHeader));
    question = (DnsQuestion*)(buf + sizeof(DnsHeader) + 2 + strlen(domain));
    question->classes = htons(1);
    question->type = htons(1);

    // 设置超时
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 200;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv));
    unsigned int addr_len = sizeof(struct sockaddr_in);

    // 通过Socket向DNS服务器发送查询请求
    if (sendto(sock, buf, sizeof(DnsHeader) + sizeof(DnsQuestion) + strlen(domain) + 2, 0, &addr, addr_len) < 0) {
        printf("errno=%d\n", errno);
        return;
    }

    // 非阻塞监听响应
    while (1) {
        int flag = recvfrom(sock, buf, BUF_SIZE, 0, (struct sockaddr*)(&addr), &addr_len);
        if (flag != -1 && errno != EAGAIN) {
            printf("flag is %d and errno is %d \n", flag, errno);
            p = buf + flag - 4;
            printf("%s ==> %u.%u.%u.%u\n", domain, (unsigned char)*p, (unsigned char)*(p + 1), (unsigned char)*(p + 2), (unsigned char)*(p + 3));
            char str[INET_ADDRSTRLEN];
            char* IP = (char*)inet_ntop(AF_INET, p, str, sizeof(str));
            return IP;
        }
        else {
            printf("Failed: flag is %d and errno is %d \n", flag, errno);
            Sleep(300);
            sendto(sock, buf, sizeof(DnsHeader) + sizeof(DnsQuestion) + strlen(domain) + 2, 0, (struct sockaddr*)&addr, addr_len);
        }
    }
    return;
}