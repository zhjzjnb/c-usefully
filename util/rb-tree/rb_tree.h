
#ifndef rb_tree_h
#define rb_tree_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int Key;
typedef char Color;


#define COLOR_RED 'r'
#define COLOR_BLACK 'b'


#define IS_RED(n) (n->c==COLOR_RED)
#define IS_BLACK(n) (n->c==COLOR_BLACK)
#define MARK_COLOR(n, c1) n->c=c1

#define MARK_RED(n) MARK_COLOR(n,COLOR_RED)
#define MARK_BLACK(n) MARK_COLOR(n,COLOR_BLACK)

typedef int(*sort_fuc)(Key, Key);

typedef struct _Node{
    struct _Node *left;
    struct _Node *right;
    struct _Node *parent;

    Key key;
    Color c;
} Node;

typedef struct{
    sort_fuc compare;
    Node *root;
} Tree;

void tree_init(Tree *tree, sort_fuc fun);


void tree_left_rotate(Tree *tree, Node *x);

void tree_right_rotate(Tree *tree, Node *x);

void tree_insert_fix(Tree *tree, Node *node);

void tree_insert(Tree *tree, Node *node);

Node *tree_search(Tree *tree, Key key);

Node *tree_delete(Tree *tree, Node *node);

void tree_free(Tree *tree);

void tree_print_inorder(Tree *tree);

int tree_height(Tree *tree);

void tree_print(Tree *tree);

void tree_test(Tree *tree);


#endif /* rb_tree_h */
