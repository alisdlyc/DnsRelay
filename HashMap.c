//
// Created by alisdlyc on 2020/8/12.
//

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
    struct Entry *next;
} entry;

typedef struct HashMap {
    int size;
    int listSize;
    struct Entry *list;
} hashmap;


struct HashMap createHashMap() {
    struct HashMap MyHashMap;
    struct Entry items[3000];
    struct Entry *p = items;
    for (int i = 0; i < MyHashMap.listSize; i++) {
        p[i].next = NULL;
    }
    MyHashMap.size = 0;
    MyHashMap.listSize = 3000;
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
    struct Entry* items = MyHashMap->list;

    // 该地址为空时直接存储
    if (!strlen(items[index].key)) {
        MyHashMap->size++;
        strncpy(items[index].key, key, strlen(key) + 1);
        strncpy(items[index].value,  value, strlen(value) + 1);
    }
    else {
        struct Entry* current;
        current = &items[index];

        while (current != NULL) {
            if (!strcmp(key, current->value)) {
                // 对于键值已经存在的直接覆盖
                strncpy(current->value, value, strlen(value) + 1);
                return;
            }
            current = current->next;
        }

        // 若发生冲突 则创建节点 并挂到相应位置的next上
        struct Entry node;
        strncpy(node.key, key, strlen(key) + 1);
        strncpy(node.value, value, strlen(value) + 1);
        node.next = items[index].next;

        items[index].next = &node;
        MyHashMap->size++;
    }
}

struct HashMap* InitHashMap(struct HashMap* MyHashMap) {
    FILE* fp;
    char ip[16];
    char domain[100];

    fopen_s(&fp, "/home/alisdlyc/VsCode/DnsRelay/Dns/dnsrelay.txt", "r");

    while (!feof(fp))
    {
        fscanf(fp, "%s", ip);
        fscanf(fp, "%s", domain);
        PutItem(MyHashMap, domain, ip);
    }

    fclose(fp);
    return MyHashMap;
}


int main() {
    struct HashMap map = createHashMap();
    InitHashMap(&map);
    printf("qwq");
}