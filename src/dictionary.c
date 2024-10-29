/*-------------------------------------------------------------- 
Created by Xin Zhou (xzzho10@student.unimelb.edu.au)
15 Aug 2023
..Project: Assignment1
dictionary.c :
    = implementation of linked dictionary module, including 
    creation, appendage, searching, deletion, and freeing.
----------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "dictionary.h"

// Used for searching by key.
#define MATCHED_LIST_SIZE 10

// use pointers to void to adapt various types of data.
typedef struct Node DictItem;
struct Node {
    void *key;
    void *data;
    DictItem *next;
};


struct LinkedList {
    DictItem *head;
    DictItem *tail;
    size_t size;
};

/**
 * @brief Create a Dictionary
 * 
 * @return Dictionary* 
 */
Dictionary* createDict() {
    Dictionary* dict = (Dictionary*) malloc(sizeof(Dictionary));
    assert(dict);
    dict->size = 0;
    return dict;
}

/**
 * @brief Append a new node to the tail of a dictionary (linked list).
 * 
 * @param dict 
 * @param key 
 * @param data 
 */
void dictAppend(Dictionary* dict, void* key, void* data) {
    if (dict->size == 0) {
        dict->head = (DictItem*) malloc(sizeof(DictItem));
        assert(dict->head);
        dict->head->key = key;
        dict->head->data = data;
        dict->tail = dict->head;
    } else {
        dict->tail->next = (DictItem*) malloc(sizeof(DictItem));
        assert(dict->tail->next);
        dict->tail = dict->tail->next;
        dict->tail->key = key;
        dict->tail->data = data;
    }
    dict->tail->next = NULL;
    dict->size++;
}

/**
 * @brief Delete the head node of a dictionary (linked list).
 * 
 * @param dict 
 * @return void* 
 */
void* dictDeleteHead(Dictionary* dict) {
    if (dict->size == 0) {
        return NULL;
    } else {
        DictItem* tmp = dict->head;
        dict->head = dict->head->next;
        if ((dict->size--) == 1) {
            dict->tail = dict->head;
        }
        tmp->next = NULL;
        return tmp->data;
    }
}

/**
 * @brief Use the key to search nodes in the dictionary. 
 *        A pointer to function is used to decouple this module from the data module.
 * 
 * @param dict 
 * @param matchedNum 
 * @param key Given key from query file.
 * @param equal A pointer to a function that compares two keys.
 * @return void** A list of pointers to the matched values in the given dictionary.
 */
void** searchDictByKey(Dictionary* dict, void* key, int* matchedNum, int* comparedKeyNum, 
                        int* countCompare, int (*compare)(void*, void*, int*)) {
    DictItem* tmp = dict->head;
    *matchedNum = 0;
    *comparedKeyNum = 0;
    *countCompare = 0;
    int matchedListSize = MATCHED_LIST_SIZE;
    void** matched = (void**) malloc(matchedListSize*sizeof(void*));
    assert(matched);
    while(tmp != NULL) {
        int tmpCount = 0;
        int cmpRes = compare(tmp->key, key, &tmpCount);
        (*comparedKeyNum) ++;
        (*countCompare) += tmpCount;

        if (cmpRes == 0) {
            if (*matchedNum == matchedListSize) {
                matchedListSize *= 2;
                matched = (void**) realloc(matched, matchedListSize);
                assert(matched);
            }
            matched[*matchedNum] = tmp->data;
            (*matchedNum)++;
        }
        tmp = tmp->next;
    }
    return matched;
}

/**
 * @brief Free a dictionary, including the key and value in each node.
 * 
 * @param dict 
 * @param fFreeKey 
 * @param fFreeData 
 */
void freeDict(Dictionary* dict, void (*fFreeKey)(void*), void (*fFreeData)(void*)) {
    if (dict->size != 0) {
        DictItem* tmp = dict->head;
        while(tmp != NULL) {
            dict->head = dict->head->next;
            if ((dict->size--) == 1) {
                dict->tail = dict->head;
            }
            tmp->next = NULL;
            fFreeKey(tmp->key);
            fFreeData(tmp->data);
            free(tmp);
            tmp = dict->head;
        }
    }
    free(dict);
}

/**
 * @brief Get the size of a dictionary
 * 
 * @param dict 
 * @return size_t 
 */
size_t getDictSize(Dictionary* dict) {
    return dict->size;
}
