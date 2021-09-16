

#include "rb_tree.h"

void test_insert(Tree* tree) {
//    int arr[] = {43,45,49,56,58,60,64,66,68,72};
//    int arr[] = {10, 40, 30, 60, 90, 70, 20, 80};
//    int arr[] = {1,11,18,21,25,31,41,45,48,51,61,71,81,91};
//    int arr[] = {1,2,3,4,5};
//    int arr[] = {10,6,12,11};
//
//    int l = sizeof(arr) / sizeof(arr[0]);
//
//    for (int i = 0; i < l; i++) {
//        Node* n = malloc(sizeof(*n));
//        memset(n, 0, sizeof(*n));
//        n->key = arr[i];
//        tree_insert(tree, n);
//    }

#define GEN_NODE(val,idx) Node *(node##idx) = malloc(sizeof(*node##idx));memset(node##idx, 0, sizeof(*node##idx));(node##idx)->key=val

    GEN_NODE(10,1);
    GEN_NODE(20,2);
    GEN_NODE(30,3);
    GEN_NODE(40,4);
    GEN_NODE(50,5);
    GEN_NODE(60,6);
    GEN_NODE(55,7);


    MARK_BLACK(node3);
    tree->root = node3;

    MARK_BLACK(node2);
    node3->left = node2;
    node2->parent = node3;

    MARK_BLACK(node1);
    node2->left = node1;
    node1->parent = node2;


    MARK_BLACK(node4);
    node3->right = node4;
    node4->parent = node3;

    MARK_RED(node5);
    node4->right = node5;
    node5->parent = node4;

    MARK_BLACK(node6);
    node5->right = node6;
    node6->parent = node5;



    MARK_BLACK(node7);
    node5->left = node7;
    node7->parent = node5;

}

int main(int argc, const char* argv[]) {
    Tree t;
    tree_init(&t, NULL);
    test_insert(&t);

    Node* n = tree_search(&t, 20);

    tree_print_inorder(&t);
    printf("height:%d\n", tree_height(&t));

    tree_test(&t);
    tree_print(&t);
    if(n){
        Node* d = tree_delete(&t, n);
        free(d);
        tree_print(&t);
    }




    tree_free(&t);

    return 0;
}
