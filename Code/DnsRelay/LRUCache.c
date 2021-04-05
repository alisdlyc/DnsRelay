//
// Created by alisdlyc on 2020/8/28.
//

#include "LRUCache.h"
#include <stdio.h>
#include <malloc.h>
#include <memory.h>

//˫������
struct node {
    char* key[100];
    char* value[18];
    struct node* prev;
    struct node* next;
};

struct hash {
    struct node* unused; //���ݵ�δʹ��ʱ��
    struct hash* next;   //�����������ϣ��ͻ
};//��ϣ��ṹ

/*
typedef struct {
    int size;//��ǰ�����С
    int capacity;//��������
    struct hash* table;//��ϣ��
    //ά��һ��˫���������ڼ�¼���ݵ�δʹ��ʱ��
    struct node* head;//��� ָ�� ���ʹ�õ�����
    struct node* tail;//ǰ�� ָ�� ���δʹ�õ�����
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


// ��ϣ��ַ
struct hash* HashMap(struct hash* table, char* key, int capacity) {
    int addr = cacheHashCode(capacity, key) % capacity;//������
    return &table[addr];
}

// ˫����ͷ�巨
void headInsertion(struct node* head, struct node* cur) {
    // cur����������
    if (cur->prev == NULL && cur->next == NULL) {
        cur->prev = head;
        cur->next = head->next;
        head->next->prev = cur;
        head->next = cur;
    }
    // cur��������
    else {
        struct node* first = head->next;//����ĵ�һ�����ݽ��
        // cur�Ƿ����ڵ�һ��
        if (first != cur) {
            cur->prev->next = cur->next;//�ı�ǰ�����ָ��
            cur->next->prev = cur->prev;//�ı��̽��ָ��
            cur->next = first;//���뵽��һ�����λ��
            cur->prev = head;
            head->next = cur;
            first->prev = cur;
        }
    }
}

// TODO ��Ҫ��Cache�еĳ�ʼip��ַ����Ϊ""
LRUCache* LRUCacheCreate(int capacity) {
    LRUCache* cache = (LRUCache*)malloc(sizeof(LRUCache));
    cache->table = (struct hash*)malloc(capacity * sizeof(struct hash));
    cache->head = (struct node*)malloc(sizeof(struct node));
    cache->tail = (struct node*)malloc(sizeof(struct node));

    memset(cache->table, 0, capacity * sizeof(struct hash));
    //memset(cache->head, 0, capacity * sizeof(struct node));
    //memset(cache->tail, 0, capacity * sizeof(struct node));

    //����ͷ��β��㲢��ʼ��
    cache->head->prev = NULL;
    cache->head->next = cache->tail;
    cache->tail->prev = cache->head;
    cache->tail->next = NULL;
    //��ʼ�������С������
    cache->size = 0;
    cache->capacity = capacity;
    return cache;
}

char* LRUCacheGet(LRUCache* cache, char* key) {
    struct hash* addr = HashMap(cache->table, key, cache->capacity);//ȡ�ù�ϣ��ַ
    addr = addr->next;//����ͷ���
    if (addr == NULL) {
        return "";
    }
    //    while ( addr->next != NULL && addr->unused->key != key) {//Ѱ����Կ�Ƿ����
    while (addr->next != NULL && strcmp(addr->unused->key, key) != 0) {//Ѱ����Կ�Ƿ����
        addr = addr->next;
    }
    //    if (addr->unused->key == key) {//���ҳɹ�
    if (strcmp(addr->unused->key, key) == 0) {//���ҳɹ�
        headInsertion(cache->head, addr->unused);//��������ͷ
        return addr->unused->value;
    }
    return "";
}

void LRUCachePut(LRUCache* cache, char* key, char* value) {
    printf("key is %s \n", key);

    struct hash* addr = HashMap(cache->table, key, cache->capacity);

    if (strlen(LRUCacheGet(cache, key)) == 0) {
        if (cache->size >= cache->capacity) {//���������ﵽ����
            struct node* last = cache->tail->prev;//���һ�����ݽ��
            struct hash* remove = HashMap(cache->table, *last->key, cache->capacity);//�������Ĺ�ϣ��ַ
            struct hash* ptr = remove;
            remove = remove->next;//����ͷ���
            //�ҵ����δʹ�õĽ��
            while (strcmp(*remove->unused->key, *last->key) != 0) {
                ptr = remove;
                remove = remove->next;
            }
            ptr->next = remove->next;//�� table[last->key % capacity] ������ɾ�����
            remove->next = NULL;
            remove->unused = NULL;
            free(remove);
            struct hash* new_node = (struct hash*)malloc(sizeof(struct hash));
            new_node->next = addr->next;
            addr->next = new_node;
            new_node->unused = last;

            strncpy(*last->key, key, strlen(key) + 1);
            strncpy(*last->value, value, strlen(value) + 1);
            headInsertion(cache->head, last);//�������ʹ�õ�����
        }
        else {
            struct hash* new_node = (struct hash*)malloc(sizeof(struct hash));
            new_node->unused = (struct node*)malloc(sizeof(struct node));
            new_node->next = addr->next;//���ӵ� table[key % capacity] ��������
            addr->next = new_node;
            new_node->unused->prev = NULL;//��Ǹý�����´�����,����˫��������
            new_node->unused->next = NULL;
            strncpy((char*)new_node->unused->key, key, strlen(key) + 1);
            strncpy((char*)new_node->unused->value, value, strlen(value) + 1);
            headInsertion(cache->head, new_node->unused);//�������ʹ�õ�����
            ++(cache->size);//�����С+1
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
        printf("û�ҵ�\n");
    }
    printf("%s\n", LRUCacheGet(cache, "www.baidu1.com"));
    printf("%s\n", LRUCacheGet(cache, "www.baidu2.com"));
    printf("%s\n", LRUCacheGet(cache, "www.baidu3.com"));
    printf("%s\n", LRUCacheGet(cache, "www.baidu4.com"));
    LRUCacheFree(cache);
    return 0;
}*/
