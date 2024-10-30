/** 
 * @brief  Utilities interface
 */

#ifndef _UTILS_H_
#define _UTILS_H_

#include "my_bool.h"

/**
 * @brief  Round up int division
 */
size_t ceiling(size_t dividend, size_t divisor);

/**
 * @brief  Concatenate two int arrays
 */
void concatIntList(int** origin, int originSize, int* addition, int additionSize);


/**
 * @brief  Convert integer to string
 * @note   Max 10 digits, do not use for large numbers~
 */
char* my_itoa(int num);


/**
 * @brief  Concatenate multiple strings from an array to one string with delimiters.
 * @note   
 * @param  strArray: 
 * @param  num: 
 * @param  delimiter: use '\0' to avoid delimiters
 * @retval 
 */
char* concatMultipleStrings(char** strArray, int num, char delimiter);

/**
 * @brief  Concatenate two strings
 * @note
 * @param  str1:
 * @param  len1:
 * @param  str2:
 * @param  len2:
 * @retval
 */
char* concatTwoStrings(char* str1, size_t len1, char* str2, size_t len2);

/**
 * @brief  Given two integers, return the smaller one
 */
int smallerInt(int a, int b);

#endif