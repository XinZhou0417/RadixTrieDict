/*-------------------------------------------------------------- 
Created by Xin Zhou (xzzho10@student.unimelb.edu.au)
07 Sep 2023
..Project: Assignment2
my_queue.c :
        = <File Description Here>
----------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "my_queue.h"


typedef struct QueueNode QNode;
struct QueueNode {
    void* val;
    QNode* next;
};


struct MyQueue {
    QNode* head;
    QNode* tail;
    size_t size;
};


size_t getQueueSize(Queue* queue) {
    return queue->size;
}


Queue* newQueue() {
    Queue* queue = (Queue*) malloc(sizeof(Queue));
    assert(queue);
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;
    return queue;
}


void* dequeue(Queue* queue) {
    if (queue->head == NULL) {
        return NULL;
    }
    void* data = queue->head->val;
    QNode* tmp = queue->head;
    queue->head = queue->head->next;
    if (queue->head == NULL) {
        queue->tail = NULL;
    }
    free(tmp);
    queue->size --;
    return data;
}


// Fixed some bug in reused file here
void enqueue(Queue* queue, void* data) {
    assert(data);
    if (queue->head == NULL) {
        queue->head = (QNode*) malloc(sizeof(QNode));
        assert(queue->head);
        queue->head->val = data;
        queue->head->next = NULL;
        queue->tail = queue->head;
    } else {
        queue->tail->next = (QNode*) malloc(sizeof(QNode));
        assert(queue->tail->next);
        queue->tail = queue->tail->next;
        queue->tail->val = data;
    }
    queue->tail->next = NULL;
    queue->size ++;
}


/**
 * @brief  New feature: view data of head node of the queue without dequeuing it
 * @note   
 * @param  queue: 
 * @retval None
 */
void* peekHeadData(Queue* queue) {
    return queue->head->val;
}


/**
 * @brief  New feature: print all data from a queue
 * @note   
 * @param  queue: 
 * @param  (*printData)(void*): A function pointer to data printing function
 * @retval None
 */
void printQueue(Queue* queue, void (*printData)(void*)) {
    QNode* currentNode = queue->head;
    printf("Printing Queue: [\n");
    while (currentNode != NULL) {
        printData(currentNode->val);
        currentNode = currentNode->next;
    }
    printf("] Printing Ended\n");
}