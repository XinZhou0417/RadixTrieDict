#pragma once
#include "radix_tree_dictionary.h"

/**
 * @brief Get a JSON string representing the notebook.
 * 
 * @param notebook
 * @return char*
 */
char* getNotebookTrieJson(RDictionary* notebook);


/**
 * @brief Insert a new data item with its key.
 * 
 * @param notebook
 * @param jsonPayload
 * @param execPath
 * 
 */
void insertNotebook(RDictionary* notebook, char* jsonPayload, char** execPath);


/**
 * @brief Search notebook using given key (prefix).
 * 
 * @param notebook
 * @param jsonPayload
 * @param matchedNum number of data that matches the prefix
 * @param comparedStr number of strings compared
 * @param comparedChar number of char compared
 * @param comparedBit number of bit compared
 * @param execPath
 * @return all data records that matches the given prefix
 */
char** searchNotebook(RDictionary* notebook, char* jsonPayload, int* matchedNum, int* comparedStr, int* comparedChar, int* comparedBit, char** execPath);


/**
 * @brief Create a new notebook
 */
RDictionary* createNotebook();


/**
 * @brief Process a request and return the result in JSON format.
 * 
 * @param jsonRequest 
 * @param notebook 
 * @return cJSON* 
 */
cJSON* processRequest(cJSON* jsonRequest, RDictionary* notebook);