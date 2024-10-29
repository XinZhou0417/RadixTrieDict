/*-------------------------------------------------------------- 
Created by Xin Zhou (xzzho10@student.unimelb.edu.au)
15 Aug 2023
..Project: Assignment1
data.c :
    = implementation of the data module, including
    creation, printing, and freeing.
----------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "data.h"


struct CafeInfo {
    int censusYear;
    int blockID;
    int propertyID;
    int basePropertyID;
    char* buildingAddress;
    char* clueSmallArea;
    char* businessAddress;
    char* tradingName;
    int industryCode;
    char* industryDescription;
    char* seatingType;
    int seatNum;
    double longitude;
    double latitude;
};

/**
 * @brief Change the delimiters from "," to "|" in a line from data file
 * 
 * @param line 
 */
void lineChangeDelimiter(char* line) {
    int quoteFlag = 0;
    for (int i = 0; line[i] != '\0';i++) {
        if (line[i] == '"') {
            if (quoteFlag == 0){
                quoteFlag = 1;
            } else {
                quoteFlag = 0;
            }
        } else if (line[i] == ','){
            if (quoteFlag == 0) {
                line[i] = '|';
            }
        }
    }
}


/**
 * @brief Remove double quotes in a line from data file
 * 
 * @param line 
 */
void lineRemoveQuote(char* line) {
    for (int i = 0;line[i]!='\0';i++) {
        if (line[i] == '"') {
            for (int j = i;line[j]!='\0';j++) {
                line[j] = line[j+1];
            }
        }
    }
}


/**
 * @brief Allocate space for string attributes of a Cafe space-efficiently.
 *        Count the number of char between the delimiters(|). 
 *        Store these numbers to charNum[DATA_FIELD_NUM].
 *        Choose specific numbers to allocate spaces for string attributes.
 * 
 * @param line 
 * @param cafe 
 */
void allocCafeStrings(char* line, Cafe* cafe) {                                                                                                                                                                                        
    int charNum[DATA_FIELD_NUM] = {0};
    for (int i=0,j=0;;i++) {
        if (line[i] == '|') {
            if (j==0) {
                charNum[0] = i;
            } else {
                charNum[j] = i - charNum[j-1] - 1;
            }
            j++;
        } else if (line[i] =='\0') {
            charNum[j] = i - charNum[j-1] - 1;
            break;
        }
    }
    // These indexes are related to the fields of a data item
    // Each data item have 14 fields, so the charNum will have 14 numbers
    // Each number represents the length of each field.
    cafe->buildingAddress = (char*) malloc(sizeof(char) * (charNum[4] + 1));
    assert(cafe->buildingAddress);
    cafe->clueSmallArea = (char*) malloc(sizeof(char) * (charNum[5] + 1));
    assert(cafe->clueSmallArea);
    cafe->businessAddress = (char*) malloc(sizeof(char) * (charNum[6] + 1));
    assert(cafe->businessAddress);
    cafe->tradingName = (char*) malloc(sizeof(char) * (charNum[7] + 1));
    assert(cafe->tradingName);
    cafe->industryDescription = (char*) malloc(sizeof(char) * (charNum[9] + 1));
    assert(cafe->industryDescription);
    cafe->seatingType = (char*) malloc(sizeof(char) * (charNum[10] + 1));
    assert(cafe->seatingType);
}

/**
 * @brief Read a line from data file and parse it to form a Cafe item then return this item.
 * 
 * @param file 
 * @return Cafe* 
 */
Cafe* readCafe(FILE *file) {
    char* line = (char*) malloc(sizeof(char) * MAX_LINE_LEN);
    assert(line);
    char* tmpLine = fgets(line, MAX_LINE_LEN, file);
    if (tmpLine != NULL) {
        Cafe* c = (Cafe*) malloc(sizeof(Cafe));
        assert(c);
        // Process new line
        lineChangeDelimiter(line);
        lineRemoveQuote(line);
        allocCafeStrings(line, c);
        // Each field is separated by "|"
        sscanf(tmpLine, "%d|%d|%d|%d|%[^|]|%[^|]|%[^|]|%[^|]|%d|%[^|]|%[^|]|%d|%lf|%lf",
                &c->censusYear,&c->blockID,&c->propertyID,&c->basePropertyID,
                c->buildingAddress,c->clueSmallArea,c->businessAddress,c->tradingName,&c->industryCode,
                c->industryDescription,c->seatingType,&c->seatNum,
                &c->longitude,&c->latitude);
        free(tmpLine);
        return c;
    }
    free(line);
    return NULL;
}

/**
 * @brief Pass the first line
 * 
 * @param file 
 */
void readHeadLine(FILE* file) {
    char* line = (char*) malloc(sizeof(char)*MAX_LINE_LEN);
    assert(line);
    fgets(line, MAX_LINE_LEN, file);
    free(line);
}

/**
 * @brief Print a Cafe item in a particular format to the output file.
 * 
 * @param f output file
 * @param cafe 
 */
void printCafe(FILE *f, Cafe* cafe) {
    fprintf(f, "--> census_year: %d || block_id: %d || property_id: %d || \
base_property_id: %d || building_address: %s || clue_small_area: %s || \
business_address: %s || trading_name: %s || industry_code: %d || \
industry_description: %s || seating_type: %s || number_of_seats: %d || \
longitude: %.5f || latitude: %.5f || \n",
            cafe->censusYear, cafe->blockID, cafe->propertyID,
            cafe->basePropertyID, cafe->buildingAddress, cafe->clueSmallArea,
            cafe->businessAddress, cafe->tradingName, cafe->industryCode,
            cafe->industryDescription, cafe->seatingType, cafe->seatNum,
            cafe->longitude, cafe->latitude);
}

/**
 * @brief Free a Cafe item, including it's string attributes.
 * 
 * @param cafe 
 */
void freeCafe(void* cafe) {
    assert(cafe);
    Cafe* c = (Cafe*) cafe;
    free(c->buildingAddress);
    free(c->businessAddress);
    free(c->tradingName);
    free(c->industryDescription);
    free(c->seatingType);
    free(c->clueSmallArea);
    free(cafe);
}

/**
 * @brief Get the Trading Name string
 * 
 * @param cafe 
 * @return void* trading name string, in void* form.
 */
void* getTradingName(void* cafe) {
    assert(cafe);
    Cafe* c = (Cafe*) cafe;
    char* tradingName = (char*) malloc(strlen(c->tradingName)+1);
    assert(tradingName);
    strcpy(tradingName, c->tradingName);
    return tradingName;
}

/**
 * @brief Free space for a trading name string.
 * 
 * @param tradingName 
 */
void freeTradingNameString(void* tradingName) {
    assert(tradingName);
    free(tradingName);
}

/**
 * @brief Compare two trading names
 * 
 * @param name1 
 * @param name2 
 * @return int result > 0 if name1 is greater (in ASCII) than name2; 
 *             result = 0 if name1 equals to name2; 
 *             result < 0 if name1 is smaller than name2. 
 */
int cmpTradingName(void* name1, void* name2) {
    char *n1 = (char*) name1, *n2 = (char*) name2;
    return strcmp(n1, n2);
}


/**
 * @brief compare trading name char by char, and record the number of char comparison
 * 
 * @param key 
 * @param tradingName 
 * @param count number of char compared for each string
 * @return a positive value if key is larger than tradingName
 *         0 if key equals to tradingName or it is a prefix of tradingName
 *         a negative value if key is smaller than trandingName
 */
int cmpTradingNameAndCount(void* key, void* tradingName, int* count) {
    *count = 0;
    int result;
    char *n1 = (char*) key, *n2 = (char*) tradingName;
    int keyLen = strlen(n1), nameLen = strlen(n2);
    assert(keyLen > 0 && nameLen > 0);
    for (int i = 0; i < keyLen && i < nameLen; i++) {
        (*count) ++;
        // compare one char at the same index in different string
        if (n1[i] > n2[i]) {
            result = 1;
            break;
        } else if (n1[i] < n2[i]) {
            result = -1;
            break;
        } else {
            result = 0;
        }
    }

    // No different char has been found yet.
    if (result == 0) {
        if (keyLen > nameLen) {
            result = 1;
        } 
        // if keyLen <= nameLen that means the given key is a prefix of the tradingName
        (*count) ++; 
    }

    return result;
}

