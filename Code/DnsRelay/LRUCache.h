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
