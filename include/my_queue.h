/*-------------------------------------------------------------- 
Created by Xin Zhou (xzzho10@student.unimelb.edu.au)
07 Sep 2023
..Project: Assignment2
my_queue.h :
        = <File Description Here>
----------------------------------------------------------------*/


#ifndef _MY_QUEUE_H_
#define _MY_QUEUE_H_
#include <stdio.h>

typedef struct MyQueue Queue;

Queue* newQueue();

void* dequeue(Queue* queue);

void enqueue(Queue* queue, void* data);

size_t getQueueSize(Queue* queue);

/**
 * @brief  New feature: view data of head node of the queue without dequeuing it
 * @note   
 * @param  queue: 
 * @retval None
 */
void* peekHeadData(Queue* queue);


/**
 * @brief  New feature: print all data from a queue
 * @note   
 * @param  queue: 
 * @param  (*printData)(void*): A function pointer to data printing function
 * @retval None
 */
void printQueue(Queue* queue, void (*printData)(void*));

#endif