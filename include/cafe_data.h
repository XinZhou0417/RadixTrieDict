/*-------------------------------------------------------------- 
Created by Xin Zhou (xzzho10@student.unimelb.edu.au)
15 Aug 2023
..Project: Assignment1
data.h :
    = interface of the data module
----------------------------------------------------------------*/


#ifndef _DATA_H_
#define _DATA_H_
#include <stdio.h>

#define MAX_LINE_LEN 512
#define DATA_FIELD_NUM 14

typedef struct CafeInfo Cafe;

/**
 * @brief Read a line from data file and parse it to form a Cafe item then return this item.
 * 
 * @param file 
 * @return Cafe* 
 */
Cafe* readCafe(FILE *file);

/**
 * @brief Pass the first line
 * 
 * @param file 
 */
void readHeadLine(FILE* file);

/**
 * @brief Print a Cafe item in a particular format to the output file.
 * 
 * @param f output file
 * @param cafe 
 */
void printCafe(FILE *f, Cafe* cafe);

/**
 * @brief Free a Cafe item, including it's string attributes.
 * 
 * @param cafe 
 */
void freeCafe(void* cafe);

/**
 * @brief Get the Trading Name string
 * 
 * @param cafe 
 * @return void* trading name string, in void* form.
 */
void* getTradingName(void* cafe);

/**
 * @brief Free space for a trading name string.
 * 
 * @param tradingName 
 */
void freeTradingNameString(void* tradingName);

/**
 * @brief Compare two trading names
 * 
 * @param name1 
 * @param name2 
 * @return int result > 0 if name1 is greater (in ASCII) than name2; 
 *             result = 0 if name1 equals to name2; 
 *             result < 0 if name1 is smaller than name2. 
 */
int cmpTradingName(void* name1, void* name2);

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
int cmpTradingNameAndCount(void* name1, void* name2, int* count);

#endif
