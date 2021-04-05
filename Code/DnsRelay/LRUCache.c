//
// Created by alisdlyc on 2020/8/28.
//

#include "LRUCache.h"
#include <stdio.h>
#include <malloc.h>
#include <memory.h>

//双向链表
struct node {
    char* key[100];
    char* value[18];
    struct node* prev;
    struct node* next;
};

struct hash {
    struct node* unused; //数据的未使用时长
    struct hash* next;   //拉链法解决哈希冲突
};//哈希表结构

/*
typedef struct {
    int size;//当前缓存大小
    int capacity;//缓存容量
    struct hash* table;//哈希表
    //维护一个双向链表用于记录数据的未使用时长
    struct node* head;//后继 指向 最近使用的数据
    struct node* tail;//前驱 指向 最久未使用的数据
} LRUCache;
*/


int cacheHashCode(int capacity, char* key) {
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
    return h % capacity;
}


// 哈希地址
struct hash* HashMap(struct hash* table, char* key, int capacity) {
    int addr = cacheHashCode(capacity, key) % capacity;//求余数
    return &table[addr];
}

// 双链表头插法
void headInsertion(struct node* head, struct node* cur) {
    // cur不在链表中
    if (cur->prev == NULL && cur->next == NULL) {
        cur->prev = head;
        cur->next = head->next;
        head->next->prev = cur;
        head->next = cur;
    }
    // cur在链表中
    else {
        struct node* first = head->next;//链表的第一个数据结点
        // cur是否已在第一个
        if (first != cur) {
            cur->prev->next = cur->next;//改变前驱结点指向
            cur->next->prev = cur->prev;//改变后继结点指向
            cur->next = first;//插入到第一个结点位置
            cur->prev = head;
            head->next = cur;
            first->prev = cur;
        }
    }
}

// TODO 需要将Cache中的初始ip地址设置为""
LRUCache* LRUCacheCreate(int capacity) {
    LRUCache* cache = (LRUCache*)malloc(sizeof(LRUCache));
    cache->table = (struct hash*)malloc(capacity * sizeof(struct hash));
    cache->head = (struct node*)malloc(sizeof(struct node));
    cache->tail = (struct node*)malloc(sizeof(struct node));

    memset(cache->table, 0, capacity * sizeof(struct hash));
    //memset(cache->head, 0, capacity * sizeof(struct node));
    //memset(cache->tail, 0, capacity * sizeof(struct node));

    //创建头、尾结点并初始化
    cache->head->prev = NULL;
    cache->head->next = cache->tail;
    cache->tail->prev = cache->head;
    cache->tail->next = NULL;
    //初始化缓存大小和容量
    cache->size = 0;
    cache->capacity = capacity;
    return cache;
}

char* LRUCacheGet(LRUCache* cache, char* key) {
    struct hash* addr = HashMap(cache->table, key, cache->capacity);//取得哈希地址
    addr = addr->next;//跳过头结点
    if (addr == NULL) {
        return "";
    }
    //    while ( addr->next != NULL && addr->unused->key != key) {//寻找密钥是否存在
    while (addr->next != NULL && strcmp(addr->unused->key, key) != 0) {//寻找密钥是否存在
        addr = addr->next;
    }
    //    if (addr->unused->key == key) {//查找成功
    if (strcmp(addr->unused->key, key) == 0) {//查找成功
        headInsertion(cache->head, addr->unused);//更新至表头
        return addr->unused->value;
    }
    return "";
}

void LRUCachePut(LRUCache* cache, char* key, char* value) {
    printf("key is %s \n", key);

    struct hash* addr = HashMap(cache->table, key, cache->capacity);

    if (strlen(LRUCacheGet(cache, key)) == 0) {
        if (cache->size >= cache->capacity) {//缓存容量达到上限
            struct node* last = cache->tail->prev;//最后一个数据结点
            struct hash* remove = HashMap(cache->table, *last->key, cache->capacity);//舍弃结点的哈希地址
            struct hash* ptr = remove;
            remove = remove->next;//跳过头结点
            //找到最久未使用的结点
            while (strcmp(*remove->unused->key, *last->key) != 0) {
                ptr = remove;
                remove = remove->next;
            }
            ptr->next = remove->next;//在 table[last->key % capacity] 链表中删除结点
            remove->next = NULL;
            remove->unused = NULL;
            free(remove);
            struct hash* new_node = (struct hash*)malloc(sizeof(struct hash));
            new_node->next = addr->next;
            addr->next = new_node;
            new_node->unused = last;

            strncpy(*last->key, key, strlen(key) + 1);
            strncpy(*last->value, value, strlen(value) + 1);
            headInsertion(cache->head, last);//更新最近使用的数据
        }
        else {
            struct hash* new_node = (struct hash*)malloc(sizeof(struct hash));
            new_node->unused = (struct node*)malloc(sizeof(struct node));
            new_node->next = addr->next;//连接到 table[key % capacity] 的链表中
            addr->next = new_node;
            new_node->unused->prev = NULL;//标记该结点是新创建的,不在双向链表中
            new_node->unused->next = NULL;
            strncpy((char*)new_node->unused->key, key, strlen(key) + 1);
            strncpy((char*)new_node->unused->value, value, strlen(value) + 1);
            headInsertion(cache->head, new_node->unused);//更新最近使用的数据
            ++(cache->size);//缓存大小+1
        }
    }
    else {
        strncpy(*cache->head->value, value, strlen(value) + 1);
    }
}

void LRUCacheFree(LRUCache* cache) {
    free(cache->table);
    free(cache->head);
    free(cache->tail);
    free(cache);
}

/*
int main() {
    LRUCache* cache = (LRUCache*)LRUCacheCreate(20);
    LRUCachePut(cache, (char*)"www.baidu.com", (char*)"182.92.193.59");
    LRUCachePut(cache, (char*)"www.baidu1.com", (char*)"182.92.193.1");
    LRUCachePut(cache, (char*)"www.baidu2.com", (char*)"182.92.193.2");
    LRUCachePut(cache, (char*)"www.baidu3.com", (char*)"182.92.193.3");
    LRUCachePut(cache, (char*)"www.baidu4.com", (char*)"182.92.193.4");
    printf("qwq\n");
    printf("%s\n", LRUCacheGet(cache, "www.baidu.com"));
    if (strlen(LRUCacheGet(cache, "www.baidu0.com")) == 0) {
        printf("没找到\n");
    }
    printf("%s\n", LRUCacheGet(cache, "www.baidu1.com"));
    printf("%s\n", LRUCacheGet(cache, "www.baidu2.com"));
    printf("%s\n", LRUCacheGet(cache, "www.baidu3.com"));
    printf("%s\n", LRUCacheGet(cache, "www.baidu4.com"));
    LRUCacheFree(cache);
    return 0;
}*/
