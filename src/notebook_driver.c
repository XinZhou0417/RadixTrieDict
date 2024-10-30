#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <cjson/cJSON.h>

#include "notebook_driver.h"
#include "radix_tree_dictionary.h"

/**
 * @brief Get a JSON string representing the notebook.
 * 
 * @param notebook
 * @return char*
 */
char* getNotebookTrieJson(RDictionary* notebook) {
    return rDict2Json(notebook, NULL);
}

/**
 * @brief Insert a new data item with its key.
 * 
 * @param notebook
 * @param jsonPayload
 * @param execPath
 * 
 */
void insertNotebook(RDictionary* notebook, char* jsonPayload, char** execPath) {
    char* insertKey = NULL;
    char* data = NULL;
    cJSON* json = cJSON_Parse(jsonPayload);
    assert(json);
    cJSON* insertKeyJSON = cJSON_GetObjectItem(json, "key");
    if (cJSON_IsString(insertKeyJSON) && insertKeyJSON->valuestring != NULL) {
        insertKey = insertKeyJSON->valuestring;
    }
    cJSON* dataJSON = cJSON_GetObjectItem(json, "data");
    if (cJSON_IsString(dataJSON) && dataJSON->valuestring != NULL) {
        data = dataJSON->valuestring;
    }
    rDictInsert(notebook, insertKey, data, execPath);
}

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
char** searchNotebook(RDictionary* notebook, char* jsonPayload, int* matchedNum, int* comparedStr, int* comparedChar, int* comparedBit, char** execPath) {
    char* searchKey = NULL;
    cJSON* json = cJSON_Parse(jsonPayload);
    assert(json);
    cJSON* searchKeyJSON = cJSON_GetObjectItem(json, "key");
    if (cJSON_IsString(searchKeyJSON) && searchKeyJSON->valuestring != NULL) {
        searchKey = searchKeyJSON->valuestring;
    }
    
    char** queryResult = (char**) prefixMatching(notebook, searchKey, matchedNum, comparedStr, comparedChar, comparedBit, execPath);
    assert(queryResult);
    return queryResult;
}
