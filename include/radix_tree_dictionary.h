/*-------------------------------------------------------------- 
Created by Xin Zhou (xzzho10@student.unimelb.edu.au)
05 Sep 2023
..Project: Assignment2
radix_tree_dictionary.h :
        = Interface of Radix tree dictionary. 
----------------------------------------------------------------*/


#ifndef _RADIX_TREE_DICTIONARY_H_
#define _RADIX_TREE_DICTIONARY_H_

typedef struct RadixTree RDictionary;


// Radix Tree Dictionary creation.
RDictionary* createRDict();


/**
 * @brief Insert a new data item with its key. '\0' at the end of strings will be counted in inserting process.
 * 
 * @param rDict 
 * @param key 
 * @param data 
 */
void rDictInsert(RDictionary* rDict, char* key, void* data);


/**
 * @brief Search radix tree using given key (prefix).
 *        '\0' at the end of strings will be ignored in searching process.
 * 
 * @param rDict 
 * @param givenKey 
 * @param matchedNum number of data that matches the prefix
 * @param comparedStr number of strings compared
 * @param comparedChar number of char compared
 * @param comparedBit number of bit compared
 * @return all data records that matches the given prefix 
 */
void** prefixMatching(RDictionary* rDict, char* givenKey, int* matchedNum,
                    int* comparedStr, int* comparedChar, int* comparedBit);


/**
 * @brief Free an entire radix tree dictionary
 * 
 * @param rDict 
 * @param fFreeData method used to free data entries
 */
void freeRDict(RDictionary* rDict, void (*fFreeData)(void*));


#endif