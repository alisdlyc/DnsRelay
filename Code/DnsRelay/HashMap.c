//
// Created by alisdlyc on 2020/8/12.

#ifdef WIN32

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <string.h>

#else
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define fopen_s(pFile, filename, mode) ((*(pFile))=fopen((filename),  (mode)))==NULL
#endif

typedef struct Entry {
    char key[100];
    char value[18];
    struct Entry* next;
} entry;

typedef struct HashMap {
    int size;
    int listSize;
    struct Entry* list;
} hashmap;


struct HashMap createHashMap() {
    struct HashMap MyHashMap;
    struct Entry items[3000];
    struct Entry* p = items;
    MyHashMap.size = 0;
    MyHashMap.listSize = 3000;
    for (int i = 0; i < MyHashMap.listSize; i++) {
        memset(p[i].key, 0x00, sizeof(char) * 100);
        memset(p[i].value, 0x00, sizeof(char) * 18);
        p[i].next = NULL;
    }
    MyHashMap.list = items;
    return MyHashMap;
}

int hashCode(struct HashMap* MyHashMap, char* key)
{
    char* k = key;
    unsigned long h = 0;
    while (*k) {
        h = (h << 4) + *k++;
        unsigned long g = h & 0xF0000000L;
        if (g) {
            h ^= g >> 24;
        }
        h &= ~g;
    }
    return h % MyHashMap->listSize;
}

void PutItem(struct HashMap* MyHashMap, char* key, char* value) {

    int index = hashCode(MyHashMap, key);
    struct Entry* items = &MyHashMap->list[index];

    // �õ�ַΪ��ʱֱ�Ӵ洢
    if (!strlen(items->key)) {
        MyHashMap->size++;
        strncpy(items->key, key, strlen(key) + 1);
        strncpy(items->value, value, strlen(value) + 1);
        items->next = NULL;
    }
    else {
        // ����ǰ��¼��֮ǰ�ļ�¼��ͻ �򴴽��ڵ� �����ص���ͻ������
        struct Entry* node = (struct Entry*)malloc(sizeof(struct Entry));
        strncpy(node->key, key, strlen(key) + 1);
        strncpy(node->value, value, strlen(value) + 1);
        node->next = items->next;
        items->next = node;
        MyHashMap->size++;
    }
}

struct HashMap* InitHashMap(struct HashMap* MyHashMap) {
    FILE* fp;
    char ip[16];
    char domain[100];

    //    fopen_s(&fp, "./dnsrelay.txt", "r");
    fopen_s(&fp, "./dnsrelay.txt", "r");
    if (&fp == NULL) {
        printf("�ļ���ʧ��\n");
        exit(1);
    }

    while (!feof(fp))
    {
        fscanf(fp, "%s", ip);
        fscanf(fp, "%s", domain);
        PutItem(MyHashMap, domain, ip);
    }

    fclose(fp);
    return MyHashMap;
}

char* GetItem(struct HashMap* MyHashMap, char* key) {
    int index = hashCode(MyHashMap, key);
    struct Entry* item = &MyHashMap->list[index];
    while (item != NULL && item->key != NULL) {
        if (!strcmp(key, item->key)) {
            if (!strcmp(item->value, (char*)"0.0.0.0")) {
                return (char*)"����������";
            }
            return (char*)item->value;
        }
        item = item->next;
    }
    return  (char*)"";
}