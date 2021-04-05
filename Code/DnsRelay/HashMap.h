#pragma once
#ifndef _HASHMAP_H
#define _HASHMAP_H

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

#define newEntry() (Entry)malloc(sizeof(struct Entry))

struct HashMap createHashMap();
struct HashMap* InitHashMap(struct HashMap* MyHashMap);
char* GetItem(struct HashMap* MyHashMap, char* key);
void PutItem(struct HashMap* MyHashMap, char* key, char* value);
int hashCode(struct HashMap* MyHashMap, char* key);
#endif