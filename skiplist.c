//
// Created by fox on 2017/10/10.
// Skip Lists: A Probabilistic Alternative to Balanced Trees
// indexable skip list
// duplicated score and node

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include "skiplist.h"
#include <time.h>

// from 0 to n
#define SKIPLIST_MAX_LEVEL 15
#define LEVEL_SIZE (SKIPLIST_MAX_LEVEL+1)

skiplist *skiplist_init(void)
{
    skiplist *list = (skiplist *)imalloc(sizeof(skiplist));
    size_t forward_mem_size = sizeof(snode*) * (LEVEL_SIZE);
    snode *header = (snode *)imalloc(sizeof(struct snode));
    list->header = header;
    header->score = INT_MAX;
    header->value = INT_MAX;
    header->width = (int *)imalloc(sizeof(int) * LEVEL_SIZE);
    for (int i = 0; i < LEVEL_SIZE; ++i) {
        header->width[i] = 1;
    }
    header->forward = (snode **)icalloc(forward_mem_size);

    list->level = 0;
    list->size = 0;

    return list;
}

static int rand_level()
{
    int level = 0;
//    while (rand() < RAND_MAX / 2 && level < SKIPLIST_MAX_LEVEL)
    while (arc4random() < INT_MAX && level < SKIPLIST_MAX_LEVEL)
        level++;
    return level;
}

// insert score,node into a skiplist, return 0
int skiplist_insert(skiplist *list, int score, int value)
{
    snode **update = (snode **)imalloc(sizeof(snode*) * (list->level + 1));
    int *fore_width = (int *)imalloc(sizeof(int) * (list->level + 1));
    snode *x = list->header;
    int i;
    for (i = list->level; i >= 0; i--) {
        fore_width[i] = 0;
        while (x->forward[i] && x->forward[i]->score <= score) {
            fore_width[i] += x->width[i];
            x = x->forward[i];
        }
        update[i] = x;
    }
    // assert(x->score <= score);

    int level = rand_level();
    list->size = list->size+1;

    x = (snode *)imalloc(sizeof(snode));
    x->score = score;
    x->value = value;
    x->forward = (snode **)imalloc(sizeof(snode*) * (level + 1));
    x->width = (int *)imalloc(sizeof(int) * (level + 1));

    // the lowest layer is one by one
    x->forward[0] = update[0]->forward[0];
    x->width[0] = 1;
    update[0]->forward[0] = x;

    int temp_width;
    for (i = 1; i <= (list->level < level ? list->level : level); i++) {
        temp_width = fore_width[i-1] + update[i-1]->width[i-1];
        x->width[i] = update[i]->width[i] + 1 - temp_width;
        x->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = x;
        update[i]->width[i] = temp_width;
    }

    if (level > list->level) {
        // complete the new level
        temp_width = fore_width[list->level] + update[list->level]->width[list->level];
        for (i = list->level+1; i <= level; i++) {
            list->header->width[i] = temp_width;
            list->header->forward[i] = x;
            x->forward[i] = NULL;
            x->width[i] = list->size + 1 - temp_width;
        }
        list->level = level;
    }
    else {
        // complete the unreached level
        for (; i <= list->level; ++i) {
            update[i]->width[i]++;
        }
    }

    ifree(update);
    ifree(fore_width);
    return 0;
}

int skiplist_update(skiplist *list, int score, int value, int old_score)
{
    skiplist_delete(list, old_score, value);
    return skiplist_insert(list, score, value);
}

// search score in skiplist.
// set the struct with index and first node if exists, otherwise set index 0
void skiplist_search(skiplist *list, int key, skiplist_search_ret *ret)
{
    snode *x = list->header;
    int temp_width = 0;
    for (int i = list->level; i >= 0; i--) {
        while (x->forward[i] && x->forward[i]->score < key) {
            temp_width += x->width[i];
            x = x->forward[i];
        }
    }
    x = x->forward[0];

    // check if existed score
    if (x && x->score == key) {
        ret->index = temp_width + 1;
        ret->node = x;
    } else{
        ret->index = 0;
        ret->node = NULL;
    }
}

// search skiplist by index. Return the node if exists, otherwise NULL
snode *skiplist_at(skiplist *list, int index)
{
    snode *x = list->header;
    for (int i = list->level; i >= 0; i--) {
        while (x->forward[i]) {
            if (x->width[i] == index) {
                return x->forward[i];
            }
            if (x->width[i] < index) {
                index -= x->width[i];
                x = x->forward[i];
            }
            else {
                break;
            }
        }
    }

    return NULL;
}

static void skiplist_node_free(snode *x)
{
    if (x) {
        ifree(x->forward);
        ifree(x->width);
        ifree(x);
    }
}

// delete by score,node. Return 0 if success, 1 if fail.
int skiplist_delete(skiplist *list, int score, int value)
{
    int i;
    snode **update = (snode **)imalloc(sizeof(snode*) * (list->level + 1));
    snode *x = list->header;

    // find every level before the specified node
    for (i = list->level; i >= 0; --i) {
        while (1) {
            if (!(x->forward[i]) || x->forward[i]->score > score) {
                update[i] = x;
                break;
            }

            if (x->forward[i]->score < score) {
                x = x->forward[i];
                continue;
            }

            // find the first node with same score
            int j;
            update[i] = x;
            for (j = i-1; j >= 0; --j) {
                while (x->forward[j]->score < score)
                    x = x->forward[j];

                update[j] = x;
            }
            x = x->forward[0];
            snode *x_start_search = x;

            // find the first node with same score and node
            while (x && x->value != value && x->score == score) {
                x = x->forward[0];
            }
            if (x && x->score == score) {
                // now x is the node to find
                // find nodes for every level before the node to find
                if (x == x_start_search) {
                    // done
                    i = 0;
                    break;
                }

                // j is used to judge if change the x_start_search
                j = 0;
                snode *iter;
                for (; i >= 0; --i)
                {
                    if (j) {
                        update[i] = x_start_search;
                        iter = x_start_search->forward[0];
                    } else{
                        iter = x_start_search;
                    }

                    while (iter != x) {
                        if (iter == update[i]->forward[i]) {
                            j = 1;
                            x_start_search = iter;
                            iter = iter->forward[0];
                            update[i] = update[i]->forward[i];
                            continue;
                        }

                        iter = iter->forward[0];
                    }
                }
                i = 0;
                break;
            }
            else {
                // not found the node
                ifree(update);
                return 1;
            }
        }
    }

    if (x->score != score) {
        // not found the node
        ifree(update);
        return 1;
    }

    for (i = 0; i <= list->level && update[i]->forward[i] == x; i++) {
        update[i]->forward[i] = x->forward[i];
        update[i]->width[i] += x->width[i] - 1;
    }
    for (; i <= list->level; i++) {
        --(update[i]->width[i]);
    }
    skiplist_node_free(x);

    while (list->level > 0 && list->header->forward[list->level] == NULL)
        list->level--;
    list->size--;

    ifree(update);
    return 0;
}

// ifree the skiplist
void skiplist_free(skiplist *list)
{
    snode *current_node = list->header->forward[0];
    while(current_node != NULL) {
        snode *next_node = current_node->forward[0];
        skiplist_node_free(current_node);
        current_node = next_node;
    }
    ifree(list);
}

// print the skip list, just for test.
static void skiplist_dump(skiplist *list)
{
    int *width = (int *)imalloc(sizeof(int) * (list->level + 1) * list->size);
    memset(width, 0, sizeof(int) * (list->level + 1) * list->size);
    snode **tempn = (snode **)imalloc(sizeof(snode*) * (list->level + 1));
    int i = 0, j;
    snode *x = list->header->forward[0];

    for (j = 0; j <= list->level; ++j) {
        tempn[j] = list->header->forward[j];
    }

    while (tempn[0] != NULL) {
        for (j = 1; j <= list->level; ++j) {
            if (tempn[j] == tempn[0]) {
                width[list->size * j + i] = tempn[j]->width[j];
                tempn[j] = tempn[j]->forward[j];
            } else {
                break;
            }
        }
        tempn[0] = tempn[0]->forward[0];
        ++i;
    }

    for (j = list->level; j > 0; --j) {
        for (i = 0; i < list->size; ++i) {
            if (width[j * list->size + i] == 0)
                printf("     ");
            else
                printf("%d    ", width[j * list->size + i]);
        }
        printf("\n");
    }
    while (x != NULL) {
        printf("%d:%d->", x->score, x->value);
        x = x->forward[0];
    }
    printf("NIL\n");

    ifree(width);
    ifree(tempn);
}

void test_skiplist(void) {
    int arr[][2] = { {3, 1}, {3,2}, {6,6}, {9,9}, {3, 3}, {1, 1}, {4, 4}, {8, 8}, {7, 7}, {5,5}}, i;
//    int arr[] = { 3, 6, 9}, i;
    skiplist_search_ret tempx;

    skiplist *list = skiplist_init();

    printf("search empty:--------------------\n");
    skiplist_search(list, 5, &tempx);
    if (tempx.index > 0) {
        printf("error, found not existed item!\n");
    }

    printf("delete empty:--------------------\n");
    skiplist_delete(list, 5, 2);

    printf("Insert:--------------------\n");
    for (i = 0; i < sizeof(arr) / sizeof(arr[0]); i++) {
        skiplist_insert(list, arr[i][0], arr[i][1]);
    }
    skiplist_dump(list);

    printf("search empty:--------------------\n");
    skiplist_search(list, 5, &tempx);
    if (tempx.index > 0) {
        printf("index = %d, value = %d\n", tempx.index, tempx.node->value);
    } else {
        printf("not fuound\n");
    }

    printf("Search by index:-----------\n");
    int indexes[] = { 11, 3, 10 };

    for (i = 0; i < sizeof(indexes) / sizeof(indexes[0]); i++) {
        snode *tempnode = skiplist_at(list, indexes[i]);
        if (tempnode) {
            printf("index = %d, score = %d, value = %d\n", indexes[i], tempnode->score, tempnode->value);
        } else {
            printf("no index = %d\n", indexes[i]);
        }
    }

    printf("Delete:--------------------\n");
    skiplist_delete(list, 3, 2);
    skiplist_delete(list, 3, 1);
    skiplist_delete(list, 6, 6);
    skiplist_dump(list);

    clock_t start, finish;
    start = clock();
    for (i = 0; i < 30*1000; ++i) {
        if (arc4random() < UINT_MAX / 5 * 3) {
            skiplist_insert(list, arc4random_uniform(100), arc4random_uniform(20));
        }
        else {
            skiplist_delete(list, arc4random_uniform(100), arc4random_uniform(20));
        }
    }
    finish = clock();
    double duration = (double)(finish - start) / CLOCKS_PER_SEC;
    printf( "%f seconds\n", duration );

    printf("Search:--------------------\n");
    int keys[] = { 0, 3, 7, 100, 11 };

    for (i = 0; i < sizeof(keys) / sizeof(keys[0]); i++) {
        skiplist_search(list, keys[i], &tempx);
        if (tempx.index > 0) {
            printf("index = %d, score = %d, value = %d\n", tempx.index, keys[i], tempx.node->value);
        } else {
            printf("score = %d, not found\n", keys[i]);
        }
    }

    skiplist_free(list);
};
