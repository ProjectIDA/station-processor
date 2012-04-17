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
}
queue_t;

// initialize this priority queue
int prioqueue_init(queue_t *queue);

// add an element to the queue
int prioqueue_add(queue_t *queue, void *data, int priority);

// highest priority items
void *prioqueue_peek_head(queue_t *queue);
void *prioqueue_pop_head(queue_t *queue);

// highest priority items
void *prioqueue_peek_tail(queue_t *queue);
void *prioqueue_pop_tail(queue_t *queue);

#endif // _PRIOQUEUE_H_ defined

