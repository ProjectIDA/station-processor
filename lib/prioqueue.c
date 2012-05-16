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
2012-04-17 JDE - Creation
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

int prioqueue_init(queue_t *queue, int capacity,
                   prioqueue_duplicate duplicate,
                   prioqueue_deallocate deallocate)
{
    int result = -1;
    if (queue) {
        queue->capacity = capacity;
        queue->size = 0;
        queue->callback_duplicate = duplicate;
        queue->callback_deallocate = deallocate;
        result = avltree_init(&queue->tree, _queue_element_compare, 0);
    }
    return result;
}

int prioqueue_clone(queue_t *target, queue_t *source)
{
    int result = -1;
    struct avltree_node *node;
    queue_element_t *element = NULL;

    if (target && source) {
        target->capacity = source->capacity;
        target->size = 0;
        target->callback_duplicate = source->callback_duplicate;
        target->callback_deallocate = source->callback_deallocate;
        if (!(result = avltree_init(&target->tree, _queue_element_compare, 0))) {
            // Duplicate the source's data to the target
            node = avltree_first(&source->tree);
            while (node != NULL) {
                element = (queue_element_t *)avltree_container_of(node, queue_element_t, node);
                prioqueue_add(target, element->data, element->priority);
                node = avltree_next(node);
            }
            result = 0;
        }
    }

    return result;
}

int prioqueue_full(queue_t *queue)
{
    if (queue == NULL)
        return -1;
    if ((queue->capacity > -1) && (queue->size >= queue->capacity))
        return 1;
    return 0;
}

int prioqueue_empty(queue_t *queue)
{
    if (queue == NULL)
        return -1;
    return (queue->size == 0);
}

int prioqueue_size(queue_t *queue)
{
    if (queue == NULL)
        return -1;
    return queue->size;
}

// Adds arbitrary data to the queue.
int prioqueue_add(queue_t *queue, void *data, int priority)
{
    struct avltree_node *node = NULL;
    queue_element_t *element = NULL;
    struct timezone tz_info;

    // We can never store a record if our capacity is zero
    if (queue->capacity == 0) {
        return -1;
    }

    while (queue->capacity > 0 && queue->size >= queue->capacity) {
        // Remove the lowest priority item, and re-use it's queue element
        node = avltree_first(&queue->tree);
        avltree_remove(node, &queue->tree);
        queue->size--;
        element = (queue_element_t *)avltree_container_of(node, queue_element_t, node);
        if (queue->callback_deallocate != NULL) {
            queue->callback_deallocate(element->data);
        }
        element->data = NULL;
        node = NULL;
    }

    if ( (element == NULL) &&
         (element = (queue_element_t *)malloc(sizeof(queue_element_t))) == NULL)
    {
        return -1;
    }
    else {
        tz_info.tz_minuteswest = 0;
        tz_info.tz_dsttime = 0;
        gettimeofday(&element->time_received, &tz_info);
        element->priority = priority;
        element->data = data;
        if (queue->callback_duplicate != NULL) {
            queue->callback_duplicate(data, &element->data);
        }
        avltree_insert(&element->node, &queue->tree);
        queue->size++;
    }
    return 0;
}

// helper function for peek/pop operations
void *_fetch_queue_item(queue_t *queue, int high, int remove)
{
    struct avltree_node *node;
    queue_element_t *element;
    void *data = NULL;

    if (queue) {
        if (high) {
            node = avltree_last(&queue->tree);
        } else {
            node = avltree_first(&queue->tree);
        }

        if (node) {
            element = (queue_element_t *)avltree_container_of(node, queue_element_t, node);
            if (element) {
                data = element->data;
            }

            if (remove) {
                avltree_remove(node, &queue->tree);
                if (element) {
                    free(element);
                }
                queue->size--;
            }
        }
    }

    return data;
}

// return highest priority item
void *prioqueue_peek_high(queue_t *queue)
{
    return _fetch_queue_item(queue, 1/*high*/, 0/*remove*/);
}

// remove and return highest priority item
void *prioqueue_pop_high(queue_t *queue)
{
    return _fetch_queue_item(queue, 1/*high*/, 1/*remove*/);
}

// return lowest priority item
void *prioqueue_peek_low(queue_t *queue)
{
    return _fetch_queue_item(queue, 0/*high*/, 0/*remove*/);
}

// remove and return lowest priority item
void *prioqueue_pop_low(queue_t *queue)
{
    return _fetch_queue_item(queue, 0/*high*/, 1/*remove*/);
}

void prioqueue_print_summary(queue_t *queue, FILE *fp, const char *prefix, const char *suffix)
{
    struct avltree_node *node;
    queue_element_t *highest;
    queue_element_t *lowest;

    if (prefix == NULL) {
        prefix = "";
    }
    if (suffix == NULL) {
        suffix = "";
    }
    if (queue) {
        node = avltree_last(&queue->tree);
        highest = (queue_element_t *)avltree_container_of(node, queue_element_t, node);
        node  = avltree_first(&queue->tree);
        lowest = (queue_element_t *)avltree_container_of(node, queue_element_t, node);
        fprintf(fp, "%sprioqueue, capacity=%d, count=%d, highest [%d:%li.%li], lowest [%d:%li.%li]%s\n",
                prefix, queue->capacity, queue->size,
                highest->priority, highest->time_received.tv_sec, highest->time_received.tv_usec,
                lowest->priority, lowest->time_received.tv_sec, lowest->time_received.tv_usec,
                suffix);
    }
    else {
        fprintf(fp, "%sprioqueue, NULL%s\n",
                prefix, suffix);
    }
}

