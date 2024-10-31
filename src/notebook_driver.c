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
void insertNotebook(RDictionary* notebook, cJSON* payload, char** execPath) {
    char* insertKey = NULL;
    char* data = NULL;
    cJSON* insertKeyJSON = cJSON_GetObjectItem(payload, "key");
    if (cJSON_IsString(insertKeyJSON) && insertKeyJSON->valuestring != NULL) {
        insertKey = insertKeyJSON->valuestring;
    }
    cJSON* dataJSON = cJSON_GetObjectItem(payload, "data");
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
MatchedData** searchNotebook(RDictionary* notebook, cJSON* payload, int* matchedKeyNum, int* matchedNum, int* comparedStr, int* comparedChar, int* comparedBit, char** execPath) {
    char* searchKey = NULL;
    cJSON* searchKeyJSON = cJSON_GetObjectItem(payload, "key");
    if (cJSON_IsString(searchKeyJSON) && searchKeyJSON->valuestring != NULL) {
        searchKey = searchKeyJSON->valuestring;
    }
    
    MatchedData** queryResult = (MatchedData**) prefixMatching(notebook, searchKey, matchedKeyNum, matchedNum, comparedStr, comparedChar, comparedBit, execPath);
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
    printf("Mode: %s\n", mode->valuestring);
    if (cJSON_IsString(mode) && mode->valuestring != NULL) {
        if (strcmp(mode->valuestring, "insert") == 0) {
            printf("Inserting...\n");
            cJSON* payload = cJSON_GetObjectItem(jsonRequest, "payload");
            char* execPath = NULL;
            if (payload != NULL) {
                insertNotebook(notebook, payload, &execPath);
            } else {
                execPath = EXEC_PATH_ERROR;
            }
            printf("Result ExecPath: %s\n", execPath);
            cJSON* result = cJSON_CreateObject();
            cJSON_AddStringToObject(result, "execPath", execPath);
            return result;
        } else if (strcmp(mode->valuestring, "search") == 0) {
            cJSON* payload = cJSON_GetObjectItem(jsonRequest, "payload");
            char* execPath = NULL;
            cJSON* result = cJSON_CreateObject();
            if (payload != NULL) {
                int matchedKeyNum = 0;
                int matchedRecordNum = 0;
                int comparedStr = 0;
                int comparedChar = 0;
                int comparedBit = 0;
                MatchedData** queryResult = searchNotebook(notebook, payload, &matchedKeyNum, &matchedRecordNum, &comparedStr, &comparedChar, &comparedBit, &execPath);
                cJSON_AddNumberToObject(result, "matchedKeyNum", matchedKeyNum);
                cJSON_AddNumberToObject(result, "matchedRecordNum", matchedRecordNum);
                cJSON_AddNumberToObject(result, "comparedStr", comparedStr);
                cJSON_AddNumberToObject(result, "comparedChar", comparedChar);
                cJSON_AddNumberToObject(result, "comparedBit", comparedBit);
                cJSON* matchedDataArray = cJSON_CreateArray();
                cJSON_AddItemToObject(result, "matchedData", matchedDataArray);
                for (int i = 0; i < matchedKeyNum; i++) {
                    cJSON* matchedDataItem = cJSON_CreateObject();
                    cJSON_AddStringToObject(matchedDataItem, "key", queryResult[i]->key);
                    cJSON_AddItemToObject(matchedDataItem, "list", cJSON_CreateStringArray((const char**) queryResult[i]->list, queryResult[i]->recordNum));
                    cJSON_AddNumberToObject(matchedDataItem, "recordNum", queryResult[i]->recordNum);
                    cJSON_AddItemToArray(matchedDataArray, matchedDataItem);
                }
            } else {
                execPath = EXEC_PATH_ERROR;
            }
            cJSON_AddStringToObject(result, "execPath", execPath);
            return result;
        } else if (strcmp(mode->valuestring, "get_tree") == 0) {
            char* notebookJson = getNotebookTrieJson(notebook);
            cJSON* result = cJSON_CreateObject();
            cJSON_AddStringToObject(result, "notebookJson", notebookJson);
            return result;
        }
    }
    return NULL;
}


/**
 * @brief Free the result of a request
 * 
 * @param jsonRequest
 * @param result 
 */
void freeResult(cJSON* jsonRequest, cJSON* result) {
    cJSON* mode = cJSON_GetObjectItem(jsonRequest, "mode");
    if (cJSON_IsString(mode) && mode->valuestring != NULL) {
        if (strcmp(mode->valuestring, "search") == 0) {
            cJSON* matchedDataArray = cJSON_GetObjectItem(result, "matchedData");
            if (cJSON_IsArray(matchedDataArray)) {
                for (int i = 0; i < cJSON_GetArraySize(matchedDataArray); i++) {
                    cJSON* matchedDataItem = cJSON_GetArrayItem(matchedDataArray, i);
                    cJSON* list = cJSON_GetObjectItem(matchedDataItem, "list");
                    if (cJSON_IsArray(list)) {
                        cJSON_Delete(list);
                    }
                }
                cJSON_Delete(matchedDataArray);
            }
        } 
    }
    cJSON_free(result);
}
