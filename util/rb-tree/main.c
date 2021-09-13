

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "rb_tree.h"

void test_insert(Tree *tree){
    int arr[] = {43,45,49,56,58,60,64,66,68,72};
    
    int l = sizeof(arr)/sizeof(arr[0]);
    
    for (int i=0; i<l; i++) {
        Node *n = malloc(sizeof(*n));
        memset(n,0,sizeof(*n));
        n->key = arr[i];
        tree_insert(tree,n);
    }
}

int main(int argc, const char * argv[]) {
    Tree t;
    tree_init(&t,NULL);
    test_insert(&t);

    Node *n = tree_search(&t,60);
    
    tree_print_inorder(&t);
    
    tree_delete(&t,n);
    
    tree_print_inorder(&t);
    
    tree_free(&t);
    
    return 0;
}
