

#include "rb_tree.h"

void test_insert(Tree* tree) {
//    int arr[] = {43,45,49,56,58,60,64,66,68,72};
    int arr[] = {10, 40, 30, 60, 90, 70, 20, 80};
//    int arr[] = {43,45,49};

    int l = sizeof(arr) / sizeof(arr[0]);

    for (int i = 0; i < l; i++) {
        Node* n = malloc(sizeof(*n));
        memset(n, 0, sizeof(*n));
        n->key = arr[i];
        tree_insert(tree, n);
    }
}

int main(int argc, const char* argv[]) {
    Tree t;
    tree_init(&t, NULL);
    test_insert(&t);

    Node* n = tree_search(&t, 70);

    tree_print_inorder(&t);
    printf("height:%d\n", tree_height(&t));

    tree_test(&t);
    tree_print(&t);

    Node* d = tree_delete(&t, n);
    free(d);

    tree_print(&t);

    tree_free(&t);

    return 0;
}
