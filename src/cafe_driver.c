/*-------------------------------------------------------------- 
Created by Xin Zhou (xzzho10@student.unimelb.edu.au)
16 Aug 2023
..Project: Assignment1
driver.c :
    = main program of the project

    Read data from csv file and build a dictionary based on 
    the data. 
    Use the keys in an input file to search items in the 
    dictionary, then print the search result into an output file.
----------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "cafe_data.h"
#include "dictionary.h"
#include "sorted_array_dictionary.h"
#include "radix_tree_dictionary.h"


#define COMMAND_LINE_ARG_NUM 4
#define BIT_PER_CHAR 8

#define STAGE_MAX 3
#define STAGE_MIN 1

#define LINKED_LIST  1
#define SORTED_ARRAY 2
#define RADIX_TREE   3

#define DEFAULT_KEY_LEN 100


void processArg(int argc, char* argv[], int* stage);
void* readData(char* dataFilename, int stage);
void queryDict(char* outFilename, void* dict, int stage);
void freeAll(void* dict, int stage);

int run_cafe_address_book(int argc, char* argv[]) {
    int stage;
    char *dataFilename, *outFilename;
    
    processArg(argc, argv, &stage);
    dataFilename = argv[2];
    outFilename = argv[3];

    void* dict = readData(dataFilename, stage);
    queryDict(outFilename, dict, stage);

    freeAll(dict, stage);
    
    return 0;
}

/**
 * @brief Check the command line arguments, get the stage of program.
 * 
 * @param argc 
 * @param argv 
 * @param stage 
 */
void processArg(int argc, char* argv[], int* stage) {
    if (argc < COMMAND_LINE_ARG_NUM || atoi(argv[1])<STAGE_MIN || atoi(argv[1])>STAGE_MAX) {
        fprintf(stderr, "[!Invalid input!]\n");
        fprintf(stderr, "[Usage]: %s  stage  dataFilename  outputFilename\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    *stage = atoi(argv[1]);
}

/**
 * @brief Read data from data file, and construct a dictionary
 * 
 * @param dataFilename 
 * @return  
 */
void* readData(char* dataFilename, int stage){
    FILE* dataFile = fopen(dataFilename, "r");
    assert(dataFile);
    readHeadLine(dataFile);

    void* dict = NULL;
    if (stage == LINKED_LIST) {
        dict = createDict();
    } else if (stage == SORTED_ARRAY) {
        dict = createSDict();
    } else {
        dict = createRDict();
    }
    assert(dict);

    while(1) {
        Cafe* cafe = readCafe(dataFile);
        if (cafe == NULL) break;
        char* key = getTradingName(cafe);

        if (stage == LINKED_LIST) {
            dictAppend((Dictionary*) dict, key, cafe);
        } else if (stage == SORTED_ARRAY) {
            sDictInsert((SDictionary*) dict, key, cafe, cmpTradingName);
        } else {
            rDictInsert((RDictionary*) dict, key, cafe);
            free(key);
        }
    }

    fclose(dataFile);
    return dict;
}

/**
 * @brief Read lines in the query file. Each line is used as a key for searching.
 *        Print the results into output file and stdout.
 * 
 * @param outFilename 
 * @param dict 
 */
void queryDict(char* outFilename, void* dict, int stage) {
    FILE* outFile = fopen(outFilename, "w");
    assert(outFile);
    // use an array of char to store current search key
    char key[DEFAULT_KEY_LEN];
    for (int i=1;scanf(" %[^\n]", key) == 1;i++) {
        fprintf(outFile, "%s\n", key);
        int matchCount = 0;
        int comparedStringNum = 0;
        int comparedCharNum = 0;
        int comparedBitNum = 0;
        void** queryResult = NULL;
        
        if (stage == LINKED_LIST) {
            queryResult = searchDictByKey((Dictionary*) dict, key, &matchCount, &comparedStringNum, 
                                            &comparedCharNum, cmpTradingNameAndCount);
            comparedBitNum = BIT_PER_CHAR * comparedCharNum;
        } else if (stage == SORTED_ARRAY) {
            queryResult = findAndTraverseSDict((SDictionary*) dict, key, &matchCount, &comparedStringNum, 
                                            &comparedCharNum, cmpTradingNameAndCount);
            comparedBitNum = BIT_PER_CHAR * comparedCharNum;
        } else { // Radix tree
            queryResult = prefixMatching((RDictionary*) dict, key, &matchCount, &comparedStringNum, &comparedCharNum, &comparedBitNum);
        }
        assert(queryResult);

        // Print result to output file and stdout
        for (int i = 0; i < matchCount; i++) {
            printCafe(outFile, (Cafe*)queryResult[i]);
        }
        printf("%s --> b%d c%d s%d\n", key, comparedBitNum, comparedCharNum, comparedStringNum);

        free(queryResult);
    }
    fclose(outFile);
}

/**
 * @brief Free the spaces used by the dictionary
 * 
 * @param dict 
 */
void freeAll(void* dict, int stage){
    if (stage == LINKED_LIST) {
        freeDict((Dictionary*) dict, freeTradingNameString, freeCafe);
    } else if (stage == SORTED_ARRAY) {
        freeSDict((SDictionary*) dict, freeTradingNameString, freeCafe);
    } else {
        freeRDict((RDictionary*) dict, freeCafe);
    }
}
