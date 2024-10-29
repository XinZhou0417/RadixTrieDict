/*-------------------------------------------------------------- 
Created by Xin Zhou (xzzho10@student.unimelb.edu.au)
05 Sep 2023
..Project: Assignment2
radix_tree_dictionary.c :
        = Implementation of Radix tree dictionary. 
----------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "radix_tree_dictionary.h"
#include "my_stack.h"

#define BIT_PER_CHAR 8
#define INITIAL_LIST_SIZE 2

// Used for searching by key.
#define MATCHED_LIST_SIZE 2

#define BIT_ONE  0b00000001
#define BIT_ZERO 0b00000000

#define NO_DIFFERENCE    0
#define FOUND_DIFFERENCE 1

#define ALL_ZERO_BYTE 0b00000000


typedef unsigned char BYTE;

typedef struct RadixTreeNode RNode;
struct RadixTreeNode {
    size_t prefixBits;
    BYTE *prefix;
    RNode* branchA;
    RNode* branchB;
    void** list;
    size_t listSize;
    size_t recordNum;
};


struct RadixTree {
    RNode* root;
};


size_t modulo(size_t divident, size_t divisor) {
    return divident % divisor;
}


size_t ceiling(size_t dividend, size_t divisor) {
    if (modulo(dividend, divisor) == 0) {
        return dividend / divisor;
    } else {
        return dividend / divisor + 1;
    }
}


size_t min(size_t num1, size_t num2) {
    return (num1 < num2 ? num1 : num2);
}

// Radix Tree Dictionary creation.
RDictionary* createRDict() {
    RDictionary* rDict = (RDictionary*) malloc (sizeof(RDictionary));
    assert(rDict);
    rDict->root = NULL;
    return rDict;
}


// Construct a new radix tree node using given data.
RNode* getNewNode(size_t prefixBits, BYTE* prefix, RNode* branchA, RNode* branchB,
                void** list, size_t listSize, size_t recordNum) {

    RNode* newNode = (RNode*) malloc(sizeof(RNode));
    assert(newNode);
    newNode->prefixBits = prefixBits;
    newNode->prefix = prefix;
    newNode->branchA = branchA;
    newNode->branchB = branchB;
    newNode->list = list;
    newNode->listSize = listSize;
    newNode->recordNum = recordNum;
    return newNode;
}


/**
 * @brief Get the bit at given index from a byte.
 * 
 * @param byte 
 * @param index index of the bit that will be checked. Each byte has 8 bits, the range of index: [0, 7]
 *        This shows how the digits in a byte are indexed: |0|1|2|3|4|5|6|7|
 * @return 0b00000001 if the digit at the index is 1; 0b00000000 if the it is 0
 */
BYTE getBitFromByte(BYTE byte, size_t index) {
    BYTE mask = 0b00000001;
    size_t shift = (BIT_PER_CHAR - 1) - index;
    mask = mask << shift;
    return (mask & byte) >> shift;
}


/**
 * @brief Get the bit at given position from a key.
 * 
 * @param key 
 * @param keyBitNum number of valid bits
 * @param index the position of the bit
 * @return 0b00000001 if the digit at the index is 1; 0b00000000 if the it is 0 
 */
BYTE getBitFromKey(BYTE* key, size_t keyBitNum, size_t index) {
    assert(index < keyBitNum);
    size_t byteIdx = index / BIT_PER_CHAR;
    BYTE byte = key[byteIdx];
    BYTE bit = getBitFromByte(byte, modulo(index, BIT_PER_CHAR));
    return bit;
}


// Get a blank key with number of bits.
BYTE* getBlankKey(size_t bitNum) {
    size_t byteNum = ceiling(bitNum, BIT_PER_CHAR);
    BYTE* blankKey = (BYTE*) malloc(byteNum * sizeof(BYTE));
    assert(blankKey);
    return blankKey;
}


/**
 * @brief Compare two keys, the number of compared bits for each key will be recored in bitCount.
 * 
 * @param key1 
 * @param keyBits1 number of valid bits in key1
 * @param key2 
 * @param keyBits2 number of valid bits in key2
 * @param bitCount number of compared bits (for each key)
 * @return 0 if no different of bits have been found; otherwise an int that is not 0.
 */
int bitCompare(BYTE* key1, size_t keyBits1, BYTE* key2, size_t keyBits2, int* bitCount) {
    (*bitCount) = 0;

    size_t minKeyBits = min(keyBits1, keyBits2);
    
    // maxBytesToCmp: the maximum number of bytes that will be compared (ceiling)
    size_t maxBytesToCmp = ceiling(minKeyBits, BIT_PER_CHAR);
    size_t byteIdx = 0;
    while ((byteIdx <= maxBytesToCmp) && ((*bitCount) < minKeyBits)) {
        BYTE byteFromKey1 = key1[byteIdx];
        BYTE byteFromKey2 = key2[byteIdx];
        for (size_t bitIdx = 0; (bitIdx < BIT_PER_CHAR) && ((*bitCount) < minKeyBits); bitIdx ++) {
            (*bitCount) ++;
            BYTE bit1 = getBitFromByte(byteFromKey1, bitIdx);
            BYTE bit2 = getBitFromByte(byteFromKey2, bitIdx);
            if (bit1 != bit2) {
                return FOUND_DIFFERENCE;
            } 
        }
        byteIdx ++;
    }
    return NO_DIFFERENCE;
}


/**
 * @brief Get next 8 digits from src starting at a given index and form a byte.
 * 
 * @param src source key
 * @param srcBits number of valid bits in source key
 * @param startAt index of the starting point
 * @return BYTE* 
 */
BYTE nextByte(BYTE* src, size_t srcBits, size_t startAt) {
    assert(startAt < srcBits);
    BYTE byte = ALL_ZERO_BYTE;
    size_t byteNum = ceiling(srcBits, BIT_PER_CHAR);
    size_t curentByteIdx = startAt / BIT_PER_CHAR;
    BYTE firstByte = src[curentByteIdx];
    BYTE secondByte;
    if (curentByteIdx == byteNum - 1) {
        secondByte = ALL_ZERO_BYTE;
    } else {
        secondByte = src[curentByteIdx + 1];
    }
    size_t firstByteLeftShift = modulo(startAt, BIT_PER_CHAR);
    size_t secondByteRightShift = BIT_PER_CHAR - firstByteLeftShift;
    // shift two bytes separately and use "|" to combine the useful digits
    byte = (firstByte << firstByteLeftShift) | (secondByte >> secondByteRightShift); 

    return byte;
}


/**
 * @brief Slice the source key and form a new key (dest)
 *        The slicing starts at [startAt] and will end at [startAt + destBits]
 *        The value is copied byte by byte, not bit by bit.
 * 
 * @param dest 
 * @param destBits valid bits of new key
 * @param src 
 * @param srcBits valid bits of source key
 * @param startAt the index where the slicing starts.
 */
void sliceKey(BYTE* dest, size_t destBits, BYTE* src, size_t srcBits, size_t startAt) {
    assert((startAt + destBits) <= srcBits);
    size_t destByteNum = ceiling((destBits), BIT_PER_CHAR); 

    // the index of the bit that every iteration below starts (it's the index of the src, not dest)
    size_t srcBitIdx = startAt;

    for (size_t i = 0; (i < destByteNum) && (srcBitIdx < srcBits) ; i++) {
        // get a byte (8 digits) from source key
        BYTE byte = nextByte(src, srcBits, srcBitIdx);
        srcBitIdx += BIT_PER_CHAR;
        dest[i] = byte;
    }

}


/**
 * @brief Insert a new data item with its key. '\0' at the end of strings will be counted in inserting process.
 * 
 * @param rDict 
 * @param key 
 * @param data 
 */
void rDictInsert(RDictionary* rDict, char* key, void* data) {
    int byteNum = strlen(key) + 1;
    if (rDict->root == NULL) {
        // '\0' is also counted
        BYTE* prefix = (BYTE*) malloc(byteNum * sizeof (BYTE));
        assert(prefix);
        memcpy(prefix, key, byteNum);
        size_t prefixBits = BIT_PER_CHAR * byteNum;
        void** list = (void**) malloc(INITIAL_LIST_SIZE * sizeof(void*));
        assert(list);
        list[0] = data;
        size_t num = 1;
        rDict->root = getNewNode(prefixBits, prefix, NULL, NULL, list, INITIAL_LIST_SIZE, num);
        return;
    }

    RNode* currentNode = rDict->root;

    BYTE* tmpKey = (BYTE*) malloc(byteNum * sizeof(BYTE));
    assert(tmpKey);
    memcpy(tmpKey, key, byteNum);
    size_t tmpKeyBitNum = byteNum * BIT_PER_CHAR;

    while (1) {
        BYTE* currentPrefix = currentNode->prefix;
        size_t currentPrefixBitNum = currentNode->prefixBits;
        
        int bitCount = 0;
        int cmpResult = bitCompare(tmpKey, tmpKeyBitNum, currentPrefix, currentPrefixBitNum, &bitCount);

        /* 
        Possible cases after the comparison:
        1. Bitwise difference has been found. This means tmpKey is different from currentPrefix at some point. So,
           currentPrefix need to be split and new branch will be created.
           In this case, the following inequity will be true:
           -->  bitCount < min(tmpKeyBitNum, currentPrefixBitNum)  -->   bitCount < currentPrefixBitNum

        2. No difference has been found yet. At least one of these two keys has finished comparison.
            - One key is finished. 
                The finished key must be currentPrefix. Since a tmpKey always have a '\0' at its end, if tmpKey is 
                finished first, that means currentPrefix contains a \0 before its real end, and that's illegal in 
                C language.
                In this case, the following inequity will be true:
                --> bitCount == currentPrefixBitNum < tmpKeyBitNum

            - Both of the two keys are finished with no difference found. 
                That means these two keys are identical. New data can be directly stored in currentNode.
                In this case, the following equation will be true:
                --> bitcount == currentPrefixBitNum == tmpKeyBitNum
        */
        if (cmpResult == FOUND_DIFFERENCE) { // Bitwise difference has been found.
            size_t commonPrefixBitNum = bitCount - 1;
            BYTE* commonPrefix = getBlankKey(commonPrefixBitNum);
            sliceKey(commonPrefix, commonPrefixBitNum, currentPrefix, currentPrefixBitNum, 0);
            
            // node with common prefix become the parent of new nodes.
            currentNode->prefix = commonPrefix;
            currentNode->prefixBits = commonPrefixBitNum;

            // create new node with the rest of the prefix, parent node's data list will be transfered to this node
            size_t slicedPrefixBitNum = currentPrefixBitNum - commonPrefixBitNum;
            BYTE* slicedPrefix = getBlankKey(slicedPrefixBitNum);
            sliceKey(slicedPrefix, slicedPrefixBitNum, currentPrefix, currentPrefixBitNum, bitCount - 1);
            RNode* slicedPrefixNode = getNewNode(slicedPrefixBitNum, slicedPrefix, 
                                                currentNode->branchA, currentNode->branchB, 
                                                currentNode->list, currentNode->listSize, currentNode->recordNum);
            free(currentPrefix);
            // non-leaf nodes don't store data record
            currentNode->list = NULL;
            currentNode->listSize = INITIAL_LIST_SIZE;
            currentNode->recordNum = 0;

            // create new node with the rest of the given key, new data list will be created in this node
            size_t slicedTmpKeyBitNum = tmpKeyBitNum - commonPrefixBitNum;
            BYTE* slicedTmpKey = getBlankKey(slicedTmpKeyBitNum);
            sliceKey(slicedTmpKey, slicedTmpKeyBitNum, tmpKey, tmpKeyBitNum, bitCount - 1);
            RNode* slicedTmpKeyNode = getNewNode(slicedTmpKeyBitNum, slicedTmpKey, NULL, NULL, NULL, INITIAL_LIST_SIZE, 0);
            slicedTmpKeyNode->list = (void**) malloc(INITIAL_LIST_SIZE * sizeof(void*));
            assert(slicedTmpKeyNode->list);
            slicedTmpKeyNode->list[0] = data;
            slicedTmpKeyNode->recordNum += 1;

            // get first bits from two new prefix and decide order
            BYTE bitFromSlicedPrefix = getBitFromKey(slicedPrefix, slicedPrefixBitNum, 0);
            BYTE bitFromSlicedTmpKey = getBitFromKey(slicedTmpKey, slicedTmpKeyBitNum, 0);

            if (bitFromSlicedPrefix < bitFromSlicedTmpKey) {
                currentNode->branchA = slicedPrefixNode;
                currentNode->branchB = slicedTmpKeyNode;
            } else {
                currentNode->branchA = slicedTmpKeyNode;
                currentNode->branchB = slicedPrefixNode;
            }

            break;
        } else { // No difference has been found yet.
            if (bitCount < tmpKeyBitNum) { // tmpKey is not finished, but currentPrefix has been finished.
                size_t slicedBitNum = tmpKeyBitNum - bitCount;
                BYTE* slicedKey = getBlankKey(slicedBitNum);
                sliceKey(slicedKey, slicedBitNum, tmpKey, tmpKeyBitNum, bitCount);
                free(tmpKey);
                tmpKey = slicedKey;
                tmpKeyBitNum = slicedBitNum;
                
                // get the first bit and decide which branch to go.
                BYTE firstBitOfSlicedKey = getBitFromKey(slicedKey, slicedBitNum, 0);
                if (firstBitOfSlicedKey == BIT_ZERO) { // first bit of the sliced key is 0
                    if (currentNode->branchA == NULL) {
                        // new branch here
                        currentNode->branchA = getNewNode(slicedBitNum, tmpKey, NULL, NULL, NULL, INITIAL_LIST_SIZE, 0);
                        currentNode->branchA->list = (void**) malloc(INITIAL_LIST_SIZE * sizeof(void*));
                        assert(currentNode->branchA->list);
                        currentNode->branchA->list[0] = data;
                        currentNode->branchA->recordNum += 1;
                        break;
                    } else {
                        // Search in branchA
                        currentNode = currentNode->branchA;
                        continue;
                    }
                } else { // first bit of the sliced key is 1
                    if (currentNode->branchB == NULL) {
                        // new branch here
                        currentNode->branchB = getNewNode(slicedBitNum, tmpKey, NULL, NULL, NULL, INITIAL_LIST_SIZE, 0);
                        currentNode->branchB->list = (void**) malloc(INITIAL_LIST_SIZE * sizeof(void*));
                        assert(currentNode->branchB->list);
                        currentNode->branchB->list[0] = data;
                        currentNode->branchB->recordNum += 1;
                        break;
                    } else {
                        // Search in branchB
                        currentNode = currentNode->branchB;
                        continue;
                    }
                }
                
            } else { // bitcount == currentPrefixBitNum == tmpKeyBitNum, both two keys has been finished.
                if (currentNode->recordNum == currentNode->listSize) {
                    currentNode->listSize *= 2;
                    currentNode->list = (void**) realloc(currentNode->list, (currentNode->listSize) * sizeof(void*));
                    assert(currentNode->list);
                }
                currentNode->list[currentNode->recordNum ++] = data;
                break;
            }
        }
    }
    free(tmpKey);
}


/**
 * @brief collect all data entries from a radix tree node and all its child nodes (using DFS) 
 * 
 * @param node 
 * @param num number of data entries collected
 * @return collected data list 
 */
void** collectData(RNode* node, int* num) {
    size_t collectionSize = MATCHED_LIST_SIZE;
    void** collection = (void**) malloc(collectionSize * sizeof(void*));
    assert(collection);

    Stack* stack = newStack();
    push(stack, node);
    while (getStackSize(stack) != 0) {
        RNode* currentNode = (RNode*) pop(stack);
        if (currentNode->branchB != NULL) {
            push(stack, currentNode->branchB);
        }
        if (currentNode->branchA != NULL) {
            push(stack, currentNode->branchA);
        }

        if (currentNode->branchA == NULL && currentNode->branchB == NULL) {
            for (size_t i = 0; i < currentNode->recordNum; i++) {
                if ((*num) == collectionSize) {
                    collectionSize *= 2;
                    collection = (void**) realloc(collection, collectionSize * sizeof(void*));
                    assert(collection);
                }
                collection[*num] = currentNode->list[i];
                (*num) ++;
            }
        }
    }
    free(stack);
    return collection;
}


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
 * @return all data records that matches the given prefix 
 */
void** prefixMatching(RDictionary* rDict, char* givenKey, int* matchedNum,
                    int* comparedStr, int* comparedChar, int* comparedBit) {
    
    void** matchedList = NULL;
    *matchedNum = 0;
    *comparedStr = 0;
    *comparedChar = 0;
    *comparedBit = 0;

    size_t keyByteNum = strlen(givenKey); // ignoring the ending '\0'
    size_t keyBitNum = keyByteNum * BIT_PER_CHAR;
    BYTE* key = getBlankKey(keyBitNum);
    memcpy(key, givenKey, keyByteNum);

    RNode* currentNode = rDict->root;

    while (currentNode != NULL) {
        int tmpBitCount = 0;
        BYTE* currentPrefix = currentNode->prefix;
        size_t currentPrefixBitNum = currentNode->prefixBits;

        int cmpResult = bitCompare(key, keyBitNum, currentPrefix, currentPrefixBitNum, &tmpBitCount);
        (*comparedBit) += tmpBitCount;
        (*comparedChar) += ceiling(tmpBitCount, BIT_PER_CHAR);

        /*
        There are three cases after comparison:
        1. Bitwise difference has been found: This key is different from currentPrefix, no matching records!
        2. No bitwise difference has been found yet. At least one of these two keys has finished comparison.
            - key is finished: This node matches the givenKey! All of its child notes will be travesed.
                --> tmpBitCount == keyBitNum <= currentPrefixBitNum
            - key is not finished but currentPrefix is finished: Need to check the child notes.
                --> tmpBitCount == currentPrefixBitNum < keyBitNum
        */
        if (cmpResult == FOUND_DIFFERENCE) { // Bitwise difference has been found.
            // No matching records, Search ended!
            break;
        } else { // No bitwise difference has been found yet.
            if (tmpBitCount == keyBitNum) { // key is finished.
                // traverse all the child nodes of currentNode to gather matched data.
                matchedList = collectData(currentNode, matchedNum);
                break;
            } else { // key is not finished but currentPrefix is finished.
                size_t slicedBitNum = keyBitNum - tmpBitCount;
                BYTE* slicedKey = getBlankKey(slicedBitNum);
                sliceKey(slicedKey, slicedBitNum, key, keyBitNum, tmpBitCount);
                free(key);
                key = slicedKey;
                keyBitNum = slicedBitNum;

                BYTE firstBitOfSlicedKey = getBitFromKey(slicedKey, slicedBitNum, 0);
                if (firstBitOfSlicedKey == BIT_ZERO) {
                    if (currentNode->branchA == NULL) {
                        // No matching records, Search ended!
                        break;
                    } else {
                        // search in branchA
                        currentNode = currentNode->branchA;
                        continue;
                    }
                } else { // first bit of sliced key is 1
                    if (currentNode->branchB == NULL) {
                        // No matching records, Search ended!
                        break;
                    } else {
                        // search in branchA
                        currentNode = currentNode->branchB;
                        continue;
                    }
                }
            }
        }

    }
    free(key);

    // (*comparedChar) = (*comparedBit) / BIT_PER_CHAR;
    
    // The number of compared string is always 1 in radix tree, since all the comparings happens on the 
    // "segments" of one particular string.
    (*comparedStr) = 1;
    if (matchedList == NULL) {
        matchedList = (void*) malloc(MATCHED_LIST_SIZE * sizeof(void*));
    }
    return matchedList;
}


/**
 * @brief Free radix tree nodes
 * 
 * @param node 
 * @param fFreeData 
 */
void freeNode(RNode* node, void (*fFreeData)(void*)) {
    if (node->recordNum != 0) {
        for (size_t i = 0; i < node->recordNum; i++) {
            fFreeData(node->list[i]);
        }
        free(node->list);
    }
    if (node->prefix != NULL) {
        free(node->prefix);
    }
    free(node);
}


/**
 * @brief Free an entire radix tree dictionary
 * 
 * @param rDict 
 * @param fFreeData method used to free data entries
 */
void freeRDict(RDictionary* rDict, void (*fFreeData)(void*)) {
    assert(rDict);
    if (rDict->root != NULL) {
        Stack* stack = newStack();
        push(stack, rDict->root);

        // DFS search with a stack
        while (getStackSize(stack) != 0) {
            RNode* currentNode = (RNode*) pop(stack);
            if (currentNode->branchB != NULL) {
                push(stack, currentNode->branchB);
            }
            if (currentNode->branchA != NULL) {
                push(stack, currentNode->branchA);
            }
            freeNode(currentNode, fFreeData);

            // if (currentNode->branchA == NULL && currentNode->branchB == NULL) {
            // }
        }
        free(stack);
    }
    free(rDict);
}




