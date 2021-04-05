#ifdef WIN32
#include <stdio.h>
#include <Winsock2.h>
#include "HashMap.h"
#include "DnsRequests.h"
#include "LRUCache.h"
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>   //connect,send,recv,set sockopt��
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

// ����⵽Socket�е�������Ϣ�������߳�ִ�д˺���������������������������͵��ͻ���
char* ParseDomain(char* domain) {
    printf("domain is %s \n", domain);
    char* IPMsg = LRUCacheGet((LRUCache *) cache, domain);
    if (strlen(IPMsg) == 0) {
        printf("δ�ڻ��������У����ұ���txt\n");
		IPMsg = GetItem(&map, domain);
        if (strlen(IPMsg) == 0) {
            IPMsg = GetRemoteDns(domain);
			if (strlen(IPMsg) > 0) {
				LRUCachePut((LRUCache*)cache, domain, IPMsg);
			}
        }
        return IPMsg;
    } else {
        printf("�ڻ���������\n");
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
    int size;//��ǰ�����С
    int capacity;//��������
    struct hash* table;//��ϣ��
    //ά��һ��˫���������ڼ�¼���ݵ�δʹ��ʱ��
    struct node* head;//��� ָ�� ���ʹ�õ�����
    struct node* tail;//ǰ�� ָ�� ���δʹ�õ�����
} LRUCache;
LRUCache* LRUCacheCreate(int capacity);
char* LRUCacheGet(LRUCache* cache, char* key);
void LRUCachePut(LRUCache* cache, char* key, char* value);
void LRUCacheFree(LRUCache* cache);
#endif //DNS_LRUCACHE_H


void ThreadRecv() {
	// �����׽���
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	char buffer[MAX_PATH];

	// ���׽���
	struct sockaddr_in servAddr;
		
	memset(&servAddr, 0, sizeof(servAddr));// ÿ���ֽڶ���0���
	servAddr.sin_family = PF_INET;// ʹ��IPv4��ַ
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);// �Զ���ȡIP��ַ
	servAddr.sin_port = htons(DNS_RELAY_PORT);// �˿�
	bind(sock, (struct sockaddr*)&servAddr, sizeof(SOCKADDR));
	// ���տͻ������� ��ȡ�ͻ��˵�ַ��Ϣ
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