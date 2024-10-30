#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <cjson/cJSON.h>

#include "notebook_driver.h"
#include "radix_tree_dictionary.h"
#include "my_bool.h"

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

/**
 * @brief Create a new notebook
 */
RDictionary* createNotebook() {
    return createRDict();
}


/**
 * @brief Process a request and return the result in JSON format.
 * 
 * @param jsonRequest 
 * @param notebook 
 * @return cJSON* 
 */
cJSON* processRequest(cJSON* jsonRequest, RDictionary* notebook) {
    printf("Processing request...\n");
    cJSON* mode = cJSON_GetObjectItem(jsonRequest, "mode");
    if (cJSON_IsString(mode) && mode->valuestring != NULL) {
        if (strcmp(mode->valuestring, "insert") == 0) {
            cJSON* payload = cJSON_GetObjectItem(jsonRequest, "payload");
            if (cJSON_IsString(payload) && payload->valuestring != NULL) {
                char* execPath = NULL;
                insertNotebook(notebook, payload->valuestring, &execPath);
                cJSON* result = cJSON_CreateObject();
                cJSON_AddStringToObject(result, "execPath", execPath);
                return result;
            }
        } else if (strcmp(mode->valuestring, "search") == 0) {
            cJSON* payload = cJSON_GetObjectItem(jsonRequest, "payload");
            if (cJSON_IsString(payload) && payload->valuestring != NULL) {
                int matchedNum = 0;
                int comparedStr = 0;
                int comparedChar = 0;
                int comparedBit = 0;
                char* execPath = NULL;
                char** queryResult = searchNotebook(notebook, payload->valuestring, &matchedNum, &comparedStr, &comparedChar, &comparedBit, &execPath);
                cJSON* result = cJSON_CreateObject();
                cJSON_AddItemToObject(result, "queryResult", cJSON_CreateStringArray((const char**) queryResult, matchedNum));
                cJSON_AddNumberToObject(result, "matchedNum", matchedNum);
                cJSON_AddNumberToObject(result, "comparedStr", comparedStr);
                cJSON_AddNumberToObject(result, "comparedChar", comparedChar);
                cJSON_AddNumberToObject(result, "comparedBit", comparedBit);
                cJSON_AddStringToObject(result, "execPath", execPath);
                return result;
            }
        } else if (strcmp(mode->valuestring, "get_tree") == 0) {
            char* notebookJson = getNotebookTrieJson(notebook);
            cJSON* result = cJSON_CreateObject();
            cJSON_AddStringToObject(result, "notebookJson", notebookJson);
            return result;
        }
    }
    return NULL;
}
