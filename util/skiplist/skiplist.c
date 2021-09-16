
#include "skiplist.h"
#include <stdlib.h>
#include <stdio.h>

static int key_compare(Key a, Key b) {
    return a - b;
}

int sl_random_level(void) {
    int level = 1;
    while ((random() & 0xFFFF) < (SKIPLIST_P * 0xFFFF))
        level += 1;
    return (level < SKIPLIST_MAXLEVEL) ? level : SKIPLIST_MAXLEVEL;
}

static SkipNode* sl_create_node(int level, Key key) {

    SkipNode* node = malloc(sizeof(*node) + level * sizeof(struct SkipListLevel));
    node->key = key;
    return node;
}

void sl_init(SkipList* sl) {
    sl->tail = NULL;
    sl->len = 0;
    sl->level = 1;
    sl->compare = key_compare;

    sl->head = sl_create_node(SKIPLIST_MAXLEVEL, 0);
    for (int i = 0; i < SKIPLIST_MAXLEVEL; ++i) {
        sl->head->level[i].forward = NULL;
        sl->head->level[i].span = 0;
    }
    sl->head->backward = NULL;

}

SkipNode* sl_insert(SkipList* sl, Key key) {
    SkipNode* update[SKIPLIST_MAXLEVEL] = {NULL};
    int rank[SKIPLIST_MAXLEVEL] = {0};

    SkipNode* p = sl->head;

    for (int i = sl->level - 1; i >= 0; --i) {
        rank[i] = i == (sl->level - 1) ? 0 : rank[i + 1];

        while (p->level[i].forward && (sl->compare(p->level[i].forward->key, key) < 0)) {
            rank[i] += p->level[i].span;
            p = p->level[i].forward;
        }
        update[i] = p;
    }

    int level = sl_random_level();

    if (level > sl->level) {
        for (int i = sl->level; i < level; i++) {
            rank[i] = 0;
            update[i] = sl->head;
            update[i]->level[i].span = sl->len;
        }
        sl->level = level;
    }

    p = sl_create_node(level, key);

    for (int i = 0; i < level; i++) {
        //双向指针
        p->level[i].forward = update[i]->level[i].forward;
        update[i]->level[i].forward = p;

        //计算距离
        p->level[i].span = update[i]->level[i].span - (rank[0] - rank[i]);
        update[i]->level[i].span = (rank[0] - rank[i]) + 1;
    }


    for (int i = level; i < sl->level; i++) {
        update[i]->level[i].span++;
    }

    //处理头
    p->backward = (update[0] == sl->head) ? NULL : update[0];

    //处理尾
    if (p->level[0].forward)
        p->level[0].forward->backward = p;
    else
        sl->tail = p;

    sl->len++;

    return p;

}

void sl_delete_node(SkipList* sl, SkipNode* p, SkipNode** update) {
// debug convert *( struct SkipListLevel(*)[32])(p->level)
    for (int i = 0; i < sl->level; i++) {
        if (update[i]->level[i].forward == p) {
            update[i]->level[i].span += p->level[i].span - 1;
            update[i]->level[i].forward = p->level[i].forward;
        } else {
            update[i]->level[i].span -= 1;
        }
    }
    if (p->level[0].forward) {
        p->level[0].forward->backward = p->backward;
    } else {
        sl->tail = p->backward;
    }
    while (sl->level > 1 && sl->head->level[sl->level - 1].forward == NULL)
        sl->level--;
    sl->len--;
}

SkipNode* sl_delete(SkipList* sl, Key key) {
    SkipNode* update[SKIPLIST_MAXLEVEL] = {NULL};
    SkipNode* p = sl->head;

    //跳过所有小的节点
    for (int i = sl->level - 1; i >= 0; --i) {
        while (p->level[i].forward && (sl->compare(p->level[i].forward->key, key) < 0)) {
            p = p->level[i].forward;
        }
        update[i] = p;
    }

    //查看下一个节点 是否为查找节点
    p = p->level[0].forward;
    if (p && sl->compare(p->key, key) == 0) {
        sl_delete_node(sl, p, update);
        return p;
    }
    return NULL;
}


int sl_get_rank(SkipList* sl, Key key) {
    SkipNode* p = sl->head;
    int rank = 0;
    int i;


    for (i = sl->level - 1; i >= 0; i--) {
        while (p->level[i].forward && sl->compare(p->level[i].forward->key, key) <= 0) {
            rank += p->level[i].span;
            p = p->level[i].forward;
        }
        if (sl->compare(p->key, key) == 0) {
            return rank;
        }
    }
    return 0;
}

void sl_print(SkipList* sl) {
    printf("sl level:%d len:%d\n", sl->level, sl->len);
    for (int i = sl->level - 1; i >= 0; i--) {
        SkipNode* p = sl->head;
        printf("span:%2d|",p->level[i].span);
        while (p->level[i].forward) {
            for (int j = 1; j < p->level[i].span; ++j) {
                printf("   ");
            }
            p = p->level[i].forward;
            printf(" %-2d", p->key);
        }
        printf("\n");
    }

}


void sl_free(SkipList* sl) {
    SkipNode* forward = sl->head->level[0].forward;
    SkipNode* next;
    free(sl->head);

    while (forward) {
        next = forward->level[0].forward;
        free(forward);
        forward = next;
    }
}