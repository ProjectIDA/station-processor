/*
File:       prioqueue.c
Copyright:  (C) 2012 by Albuquerque Seismological Laboratory
Author:     Joel Edwards
Purpose:    Priority queue implementation
Routines:
        ****

Update History:
yyyy-mm-dd WHO - Changes
==============================================================================
2012-04-17 FCS - Creation
******************************************************************************/

#include "include/prioqueue.h"

// Provides ordering for the priority queue elements in the AVL Tree.
int _queue_element_compare(const struct avltree_node *nodeA, const struct avltree_node *nodeB)
{
    queue_element_t *a = (queue_element_t *)avltree_container_of(nodeA, queue_element_t, node);
    queue_element_t *b = (queue_element_t *)avltree_container_of(nodeB, queue_element_t, node);

    if (a->priority != b->priority) {
        return a->priority - b->priority;
    }
    if (a->time_received.tv_sec != b->time_received.tv_sec) {
        return b->time_received.tv_sec - a->time_received.tv_sec;
    }
    return b->time_received.tv_usec - a->time_received.tv_usec;
}

int prioqueue_init(queue_t *queue) {
    return avltree_init(&queue->tree, _queue_element_compare, 0);
}

// Adds arbitrary data to the queue.
int prioqueue_add(queue_t *queue, void *data, int priority)
{
    int result = 0;
    queue_element_t *element;
    struct timezone tz_info;

    if ((element = (queue_element_t *)malloc(sizeof(queue_element_t))) == NULL) {
        result = -1;
    }
    else {
        element->data = data;
        tz_info.tz_minuteswest = 0;
        tz_info.tz_dsttime = 0;
        gettimeofday(&element->time_received, &tz_info);
        element->priority = priority;
        element->data = data;
        avltree_insert(&element->node, &queue->tree);
    }
    return result;
}

// helper function for peek/pop operations
void *_fetch_queue_item(queue_t *queue, int head, int remove)
{
    struct avltree_node *node;
    queue_element_t *element;
    void *data = NULL;

    if (head) {
        node = avltree_last(&queue->tree);
    } else {
        node = avltree_first(&queue->tree);
    }

    if (node) {
        element = (queue_element_t *)avltree_container_of(node, queue_element_t, node);
        data = element->data;

        if (remove) {
            avltree_remove(node, &queue->tree);
            free(element);
        }
    }

    return data;
}

// return highest priority item
void *prioqueue_peek_high(queue_t *queue)
{
    return _fetch_queue_item(queue, 1, 0);
}

// remove and return highest priority item
void *prioqueue_pop_high(queue_t *queue)
{
    return _fetch_queue_item(queue, 1, 1);
}

// return lowest priority item
void *prioqueue_peek_low(queue_t *queue)
{
    return _fetch_queue_item(queue, 0, 0);
}

// remove and return lowest priority item
void *prioqueue_pop_low(queue_t *queue)
{
    return _fetch_queue_item(queue, 0, 1);
}

