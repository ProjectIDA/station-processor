/*
File:       prioqueue.h
Copyright:  (C) 2012 by Albuquerque Seismological Laboratory
Author:     Joel Edwards
Purpose:    Common definitions for using archive server

Update History:
yyyy-mm-dd WHO - Changes
==============================================================================
2012-04-17 JDE - Creation
 ******************************************************************************/

#ifndef _PRIOQUEUE_H_
#define _PRIOQUEUE_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/time.h>
#include "libtree/libtree.h"

// duplication and deallocation function prototypes
typedef void (* prioqueue_duplicate)(void *item, void **duplicate);
typedef void (* prioqueue_deallocate)(void *item);

// priority queue element structure
typedef struct QUEUE_ELEMENT
{
    int priority;
    struct timeval time_received;

    void *data;

    struct avltree_node node;
}
queue_element_t;

// priority queue structure
typedef struct QUEUE
{
    struct avltree tree;
    int capacity;
    int size;

    prioqueue_duplicate callback_duplicate;
    prioqueue_deallocate callback_deallocate;
}
queue_t;

// initialize this priority queue
int prioqueue_init(queue_t *queue, int capacity,
                   prioqueue_duplicate duplicate,
                   prioqueue_deallocate deallocate);

// create a new queue with the data and settings cloned from another
int prioqueue_clone(queue_t *target, queue_t *source);

// add an element to the queue
int prioqueue_add(queue_t *queue, void *data, int priority);

// highest priority items
void *prioqueue_peek_high(queue_t *queue);
void *prioqueue_pop_high(queue_t *queue);

// highest priority items
void *prioqueue_peek_low(queue_t *queue);
void *prioqueue_pop_low(queue_t *queue);

// print a summary about this priority queue to the specified stream
void prioqueue_print_summary(queue_t *queue, FILE *fp, const char *prefix, const char *suffix);

// reports whether the queue is full
int prioqueue_full(queue_t *queue);

// reports whether the queue is empty
int prioqueue_empty(queue_t *queue);

// reports the number of elements in the queue
int prioqueue_size(queue_t *queue);

#endif // _PRIOQUEUE_H_ defined

