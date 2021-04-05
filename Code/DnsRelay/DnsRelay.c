#ifdef WIN32
#include <stdio.h>
#include <Winsock2.h>
#include "HashMap.h"
#include "DnsRequests.h"
#include "LRUCache.h"
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>   //connect,send,recv,set sockopt等
#include <netinet/in.h>     // sock_addr_in, "man 7 ip" ,htons
#include <stdio.h>
#include <string.h>          // memset
#include "HashMap.h"
#include "DnsRequests.h"
typedef int SOCKET;
typedef unsigned char BYTE;
typedef unsigned long DWORD;
typedef enum { false = 0, true = 1 } BOOL;
#define MAX_PATH 260
#endif

#define MAX_IP_LEN 16
#define DNS_RELAY_PORT  9527
struct HashMap map;
struct LRUCache* cache;

// 若检测到Socket中的域名信息，则开启线程执行此函数解析域名，并将解析结果回送到客户端
char* ParseDomain(char* domain) {
    printf("domain is %s \n", domain);
    char* IPMsg = LRUCacheGet((LRUCache *) cache, domain);
    if (strlen(IPMsg) == 0) {
        printf("未在缓存中命中，查找本地txt\n");
		IPMsg = GetItem(&map, domain);
        if (strlen(IPMsg) == 0) {
            IPMsg = GetRemoteDns(domain);
			if (strlen(IPMsg) > 0) {
				LRUCachePut((LRUCache*)cache, domain, IPMsg);
			}
        }
        return IPMsg;
    } else {
        printf("在缓存中命中\n");
        return IPMsg;
    }
}

BOOL WinSockInit() {
	WSADATA wsaData = { 0 };
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) == NOERROR) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

void WinSockUnLoad() {
	WSACleanup();
}


//
// Created by alisdlyc on 2020/8/28.
//

#ifndef DNS_LRUCACHE_H
#define DNS_LRUCACHE_H

typedef struct {
    int size;//当前缓存大小
    int capacity;//缓存容量
    struct hash* table;//哈希表
    //维护一个双向链表用于记录数据的未使用时长
    struct node* head;//后继 指向 最近使用的数据
    struct node* tail;//前驱 指向 最久未使用的数据
} LRUCache;
LRUCache* LRUCacheCreate(int capacity);
char* LRUCacheGet(LRUCache* cache, char* key);
void LRUCachePut(LRUCache* cache, char* key, char* value);
void LRUCacheFree(LRUCache* cache);
#endif //DNS_LRUCACHE_H


void ThreadRecv() {
	// 创建套接字
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	char buffer[MAX_PATH];

	// 绑定套接字
	struct sockaddr_in servAddr;
		
	memset(&servAddr, 0, sizeof(servAddr));// 每个字节都用0填充
	servAddr.sin_family = PF_INET;// 使用IPv4地址
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);// 自动获取IP地址
	servAddr.sin_port = htons(DNS_RELAY_PORT);// 端口
	bind(sock, (struct sockaddr*)&servAddr, sizeof(SOCKADDR));
	// 接收客户端请求 获取客户端地址信息
	struct sockaddr clientAddr;
	int nSize = sizeof(struct sockaddr);

	while (TRUE) {
		ZeroMemory(buffer, MAX_PATH);
		struct sockaddr_in addr;
		int iLen = sizeof(struct sockaddr_in);
		memset(&addr, 0, iLen);
		int strLen = recvfrom(sock, buffer, MAX_PATH, 0, (struct sockaddr*)(&clientAddr), &nSize);
		if (strlen(buffer)) {
			char* ipInfo = ParseDomain(buffer);
			sendto(sock, ipInfo, strlen(ipInfo), 0, &clientAddr, nSize);
		}
	}
	return NULL;
}

int main() {
	map = createHashMap();
	InitHashMap(&map);
	cache = (struct LRUCache*)LRUCacheCreate(20);
	WinSockInit();
	ThreadRecv();
	WinSockUnLoad();
}