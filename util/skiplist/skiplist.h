//
// Created by jk on 2021/9/16.
//

#ifndef _SKIPLIST_H
#define _SKIPLIST_H

#define SKIPLIST_MAXLEVEL 32 /* Should be enough for 2^32 elements */
#define SKIPLIST_P 0.25      /* Skiplist P = 1/4 */

typedef int Key;

typedef int(* compare_fun)(Key, Key);



typedef struct _SkipNode{
    Key key;
    struct _SkipNode* backward;

    struct SkipListLevel{
        struct _SkipNode* forward;
        int span;
    } level[];
}SkipNode;

typedef struct{
    compare_fun compare;
    SkipNode* head, * tail;
    int level;
    int len;
} SkipList;

void sl_init(SkipList* sl);

SkipNode* sl_insert(SkipList* sl, Key key);

SkipNode* sl_delete(SkipList *sl, Key key);

int sl_get_rank(SkipList *sl,Key key);

void sl_free(SkipList* sl);

void sl_print(SkipList *sl);

#endif //_SKIPLIST_H
