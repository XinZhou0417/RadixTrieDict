/** 
 * @brief  Utilities implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "utils.h"


/**
 * @brief  Round up int division
 */
size_t ceiling(size_t dividend, size_t divisor) {
    return ((dividend - 1) / divisor) + 1;
}


/**
 * @brief  Concatenate two int arrays
 */
void concatIntList(int** origin, int originSize, int* addition, int additionSize) {
    if ((*origin) == NULL) {
        *origin = addition;
    } else {
        *origin = (int*) realloc((*origin), sizeof (int) * (originSize + additionSize));
        memcpy((*origin) + originSize, addition, additionSize * sizeof (int));
        free(addition);
    }
}


/**
 * @brief  Convert integer to string
 * @note   Max 10 digits, do not use for large numbers~
 *         Only for positive numbers!
 */
char* my_itoa(int num) {
    assert(num >= 0);
    int maxDigitNum = 10;
    int digitNum = 0;
    char digits[maxDigitNum];
    
    int base = 10;
    while (digitNum < maxDigitNum) {
        int quotient = num / base;  // This will be used to judge if we have done all the digits
        int mod      = num % base;  // This will be the digit 
        char digit   = mod + '0';   // Simply convert a single-digit int to a char
        digits[digitNum] = digit;   
        digitNum ++;
        if (quotient == 0) break;
        num = quotient;
    }

    char* result = (char*) malloc(sizeof(char) * (digitNum + 1));
    result[digitNum] = '\0';
    for (int i = 0; i < digitNum ; i++) {
        result[i] = digits[digitNum - 1 - i];
    }
    return result;
}


/**
 * @brief  Concatenate multiple strings from an array to one string with delimiters.
 * @note   
 * @param  strArray: 
 * @param  num: 
 * @param  delimiter: use '\0' to avoid delimiters
 * @retval 
 */
char* concatMultipleStrings(char** strArray, int num, char delimiter) {
    assert(num > 0);
    int length = 0;
    for (int i = 0; i < num ; i++ ) {
        length += strlen(strArray[i]);
    }
    // No delimiter
    if (delimiter == '\0') {
        length ++;          // Space for the final '\0'       
    } else {
        length += num;      // Space for delimiters and the final '\0'
    }
    char* str = (char*) malloc(sizeof(char) * length);
    // Add the first string 
    int currentIndex = 0;
    int len = strlen(strArray[0]);
    memcpy(str, strArray[0],  len * sizeof (char));
    currentIndex += len;

    // Loop add the rest strings with delimiters.
    for (int i = 1; i < num ; i++ ) {
        if (delimiter != '\0') {
            str[currentIndex] = delimiter;
            currentIndex ++;
        }
        len = strlen(strArray[i]);
        memcpy((str + currentIndex), strArray[i], len * sizeof (char));
        currentIndex += len;
    }
    str[length - 1] = '\0';
    return str;
}

/**
 * @brief  Concatenate two strings
 * @note
 * @param  str1:
 * @param  len1:
 * @param  str2:
 * @param  len2:
 * @retval
 */
char* concatTwoStrings(char* str1, size_t len1, char* str2, size_t len2) {
    char* result = (char*) malloc(sizeof(char) * (len1 + len2 + 1));
    memcpy(result, str1, len1);
    memcpy(result + len1, str2, len2);
    result[len1 + len2] = '\0';
    return result;
}


/**
 * @brief  Given two integers, return the smaller one
 */
int smallerInt(int a, int b) {
    return a < b ? a : b;
}