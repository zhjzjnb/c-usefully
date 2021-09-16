#include <stdio.h>
#include <stdlib.h>

#include "skiplist.h"

int main() {

    SkipList sl;
    sl_init(&sl);

    for (int i = 0; i < 20; ++i) {
        sl_insert(&sl,1+rand()%50);
    }
    sl_insert(&sl,1);
    sl_insert(&sl,100);


    int r = sl_get_rank(&sl,100);
    printf("rank:%d\n",r);

    sl_print(&sl);


    SkipNode *n = sl_delete(&sl,100);
    if(n){
        printf("find:%d\n",n->key);
        free(n);
        sl_print(&sl);
    }

    sl_free(&sl);
    return 0;
}
