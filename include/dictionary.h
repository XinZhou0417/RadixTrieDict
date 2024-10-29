/*-------------------------------------------------------------- 
Created by Xin Zhou (xzzho10@student.unimelb.edu.au)
16 Aug 2023
..Project: Assignment1
dictionary.h :
    = interface of linked dictionary module
----------------------------------------------------------------*/


#ifndef _LIST_H_
#define _LIST_H_
#include <stdio.h>

#define TRUE 1
#define FALSE 0

typedef struct LinkedList Dictionary;

/**
 * @brief Create a Dictionary
 * 
 * @return Dictionary* 
 */
Dictionary* createDict();

/**
 * @brief Append a new node to the tail of a dictionary (linked list).
 * 
 * @param dict 
 * @param key 
 * @param data 
 */
void dictAppend(Dictionary* dict, void* key, void* data);

/**
 * @brief Delete the head node of a dictionary (linked list).
 * 
 * @param dict 
 * @return void* 
 */
void* dictDeleteHead(Dictionary* dict);

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
                        int* countCompare, int (*compare)(void*, void*, int*));

/**
 * @brief Free a dictionary, including the key and value in each node.
 * 
 * @param dict 
 * @param fFreeKey 
 * @param fFreeData 
 */
void freeDict(Dictionary* dict, void (*fFreeKey)(void*), void (*fFreeData)(void*));

/**
 * @brief Get the size of a dictionary
 * 
 * @param dict 
 * @return size_t 
 */
size_t getDictSize(Dictionary* dict);

#endif
