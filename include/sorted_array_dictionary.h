/*-------------------------------------------------------------- 
Created by Xin Zhou (xzzho10@student.unimelb.edu.au)
04 Sep 2023
..Project: Assignment2
sorted_array_dictionary.h :
        = Interface of sorted array dictionary
----------------------------------------------------------------*/


#ifndef _SORTED_ARRAY_DICTIONARY_H_
#define _SORTED_ARRAY_DICTIONARY_H_

typedef struct SortedArray SDictionary;


// creation of sorted array dictionary
SDictionary* createSDict();


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
void sDictInsert(SDictionary* sDict, void* key, void* data, int (*compare)(void*, void*));


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
                            int* countCompare, int (*compare)(void*, void*, int*));

/**
 * @brief Free the entire sorted array dictionary
 * 
 * @param sDict 
 * @param fFreeKey 
 * @param fFreeData 
 */
void freeSDict(SDictionary* sDict, void (*fFreeKey)(void*), void (*fFreeData)(void*));


#endif