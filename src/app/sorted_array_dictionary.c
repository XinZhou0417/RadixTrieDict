/*-------------------------------------------------------------- 
Created by Xin Zhou (xzzho10@student.unimelb.edu.au)
04 Sep 2023
..Project: Assignment2
sorted_array_dictionary.c :
        = Implementation of sorted array dictionary
----------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "sorted_array_dictionary.h"

#define INITIAL_SORTED_ARRAY_SIZE 2

// Used for searching by key.
#define MATCHED_LIST_SIZE 2

typedef struct SNode SDictItem;
struct SNode {
    void *key;
    void *data;
};


struct SortedArray {
    SDictItem **sArray;
    size_t size;
    size_t num;
};


// creation of sorted array dictionary
SDictionary* createSDict() {
    SDictionary *sDict = (SDictionary*) malloc(sizeof(SDictionary));
    assert(sDict);
    sDict->size = INITIAL_SORTED_ARRAY_SIZE;
    sDict->num = 0;
    sDict->sArray = (SDictItem **) malloc(sDict->size * sizeof(SDictItem*));
    assert(sDict->sArray);
    return sDict;
}


// ensure size of the sorted array, double the size of array if not enough
void ensureSortedArraySize(SDictionary* sDict) {
    if (sDict->num == sDict->size) {
        sDict->size *= 2;
        sDict->sArray = (SDictItem**) realloc(sDict->sArray, sDict->size * sizeof(SDictItem*));
        assert(sDict->sArray);
    }
}

// minimize the size of sorted array
void shrinkSortedArray(SDictionary* sDict) {
    if (sDict->num < sDict->size) {
        sDict->size = (sDict->num > INITIAL_SORTED_ARRAY_SIZE) ? sDict->num : INITIAL_SORTED_ARRAY_SIZE;
        sDict->sArray = (SDictItem**) realloc(sDict->sArray, sDict->size * sizeof(SDictItem*));
        assert(sDict->sArray);
    }
}


/**
 * @brief Insert new data item into sorted array
 * 
 * @param sDict 
 * @param key 
 * @param data 
 * @param compare a pointer to a function which compares two keys. 
 *                return value of (*compare):
 *                  - a positive int if the first key is "larger" (numerically or alphabetically) 
 *                    than the second key.
 *                  - a negative int if the first key is "smaller" than the second key.
 *                  - 0 if the first key "equals to" the second key.
 */
void sDictInsert(SDictionary* sDict, void* givenKey, void* data, int (*compare)(void*, void*)) {
    SDictItem* newItem = (SDictItem*) malloc(sizeof(SDictItem));
    newItem->key = givenKey;
    newItem->data = data;

    if (sDict->num == 0) {
        sDict->sArray[0] = newItem;
        sDict->num ++;
        return;
    }

    int left = 0, right = sDict->num - 1;
    int mid;
    int insertIndex;

    // binary search, find the index to insert
    while (left <= right) {
        mid = (left + right) / 2;
        void* midKey = (sDict->sArray[mid])->key;
        int cmpResult = compare(givenKey, midKey);

        if (cmpResult > 0) {
            left = mid + 1;
            insertIndex = mid + 1;
        } else if (cmpResult < 0) {
            right = mid - 1;
            insertIndex = mid;
        } else {
            insertIndex = mid;
            break;
        }
    }

    // only traverse right side to ensure stable sort
    for (int i = insertIndex; i < sDict->num; i++) {
        void* tmpKey = (sDict->sArray[i])->key;
        int cmpResult = compare(givenKey, tmpKey);
        if (cmpResult != 0) {
            insertIndex = i;
            break;
        }
    }

    // insert
    ensureSortedArraySize(sDict);

    sDict->sArray[(sDict->num)] = newItem;
    (sDict->num) ++;

    // swap 
    for (int i = insertIndex; i < sDict->num; i++) {
        SDictItem* tmp = sDict->sArray[i];
        sDict->sArray[i] = sDict->sArray[sDict->num - 1];
        sDict->sArray[sDict->num - 1] = tmp;        
    }
}


/**
 * @brief Search data entries using a given key.
 * 
 * @param sDict 
 * @param givenKey 
 * @param matchedNum number of data entries matched the key
 * @param comparedKeyNum number of keys (strings) that have been 
 * @param countCompare number of comparison (number of char compared)
 * @param compare 
 * @return void** 
 */
void** findAndTraverseSDict(SDictionary* sDict, void* givenKey, int* matchedNum, int* comparedKeyNum, 
                            int* countCompare, int (*compare)(void*, void*, int*)) {
    
    *matchedNum = 0;
    int matchedListSize = MATCHED_LIST_SIZE;
    void** matched = (void**) malloc(matchedListSize * sizeof(void*));
    assert(matched);

    int left = 0, right = sDict->num - 1;
    int mid;

    (*comparedKeyNum) = 0;
    (*countCompare) = 0;

    // binary search
    while (left <= right) {
        int tmpCount = 0;
        mid = (left + right) / 2;
        void* midKey = (sDict->sArray[mid])->key;
        int cmpResult = compare(givenKey, midKey, &tmpCount);
        (*comparedKeyNum) ++;
        (*countCompare) += tmpCount;

        if (cmpResult > 0) {
            left = mid + 1;
        } else if (cmpResult < 0) {
            right = mid - 1;
        } else {
            (*matchedNum) ++;
            break;
        }
    }

    // nothing was matched
    if (*matchedNum == 0) {
        return matched;
    }

    // traverse left
    int matchStartIndex = mid; // inclusive
    for (int i = mid - 1; i >= 0; i--) {
        int tmpCount = 0;
        void* tmpKey = (sDict->sArray[i])->key;
        int cmpResult = compare(givenKey, tmpKey, &tmpCount);
        (*comparedKeyNum) ++;
        (*countCompare) += tmpCount;

        if (cmpResult != 0) {
            break;
        } else {
            (*matchedNum) ++;
            matchStartIndex = i;
        }
    }

    // traverse right
    for (int i = mid + 1; i < sDict->num; i++) {
        int tmpCount = 0;
        void* tmpKey = (sDict->sArray[i])->key;
        int cmpResult = compare(givenKey, tmpKey, &tmpCount);
        (*comparedKeyNum) ++;
        (*countCompare) += tmpCount;

        if (cmpResult != 0) {
            break;
        } else {
            (*matchedNum) ++;
        }
    }

    // ensure the size of matchedList
    if (*matchedNum > matchedListSize) {
        matchedListSize = *matchedNum;
        matched = (void**) realloc(matched, matchedListSize * sizeof(void*));
        assert(matched);
    }

    // collect matched records
    int matchEndIndex = matchStartIndex + *matchedNum; // exclusive
    for (int i = matchStartIndex; i < matchEndIndex; i++) {
        SDictItem* item = (sDict->sArray)[i];
        matched[i - matchStartIndex] = item->data;
    }

    return matched;
}


/**
 * @brief Free the entire sorted array dictionary
 * 
 * @param sDict 
 * @param fFreeKey 
 * @param fFreeData 
 */
void freeSDict(SDictionary* sDict, void (*fFreeKey)(void*), void (*fFreeData)(void*)) {
    for (int i = 0; i < sDict->num; i++) {
        SDictItem* tmp = (sDict->sArray)[i];
        fFreeKey(tmp->key);
        fFreeData(tmp->data);
        free(tmp);
    }
    free(sDict->sArray);
    free(sDict);
}

