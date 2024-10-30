/*-------------------------------------------------------------- 
Created by Xin Zhou (xzzho10@student.unimelb.edu.au)
05 Sep 2023
..Project: Assignment2
radix_tree_dictionary.h :
        = Interface of Radix tree dictionary. 
----------------------------------------------------------------*/


#ifndef _RADIX_TREE_DICTIONARY_H_
#define _RADIX_TREE_DICTIONARY_H_

// Tokens representing the path of an execution in the radix tree.
#define EXEC_PATH_ROOT          "O"
#define EXEC_PATH_LEFT          "L"
#define EXEC_PATH_RIGHT         "R"
#define EXEC_PATH_NEW_LEFT      "A"
#define EXEC_PATH_NEW_RIGHT     "B"
#define EXEC_PATH_MATCH         "M"
#define EXEC_PATH_NOT_MATCH     "N"


typedef struct RadixTree RDictionary;


// Radix Tree Dictionary creation.
RDictionary* createRDict();


/**
 * @brief Insert a new data item with its key. '\0' at the end of strings will be counted in inserting process.
 * 
 * @param rDict 
 * @param key 
 * @param data 
 * @param execPath
 */
void rDictInsert(RDictionary* rDict, char* key, void* data, char** execPath);


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
 * @param execPath
 * @return all data records that matches the given prefix 
 */
void** prefixMatching(RDictionary* rDict, char* givenKey, int* matchedNum,
                    int* comparedStr, int* comparedChar, int* comparedBit, char** execPath);


/**
 * @brief Free an entire radix tree dictionary
 * 
 * @param rDict 
 * @param fFreeData method used to free data entries
 */
void freeRDict(RDictionary* rDict, void (*fFreeData)(void*));


/**
 * @brief Convert a radix tree dictionary to a cJSON object using BFS with Queue
 * @note The maximum number of nodes in the JSON object is limited to {MAX_NODES_IN_JSON}.
 * 
 * @param rDict 
 * @param stringifyData method used to convert data entries to string
 * @return A JSON string, representing the radix tree dictionary.
 */
char* rDict2Json(RDictionary* rDict, char* (*stringifyData)(void*));


#endif