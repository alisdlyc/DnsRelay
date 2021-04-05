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
