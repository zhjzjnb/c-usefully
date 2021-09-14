

#include "rb_tree.h"



static int default_compare(Key a, Key b) {
    if (a == b) {
        return 0;
    }
    if (a > b) {
        return 1;
    }
    return -1;
}

void tree_init(Tree* tree, sort_fuc fun) {
    tree->root = NULL;

    tree->compare = fun == NULL ? default_compare : fun;
}


/*
 * 对红黑树的节点(x)进行左旋转
 *
 * 左旋示意图(对节点x进行左旋)：
 *      px                              px
 *     /                               /
 *    x                               y
 *   /  \      --(左旋)-->           / \                #
 *  lx   y                          x  ry
 *     /   \                       /  \
 *    ly   ry                     lx  ly
 *
 *
 */
void tree_left_rotate(Tree* tree, Node* x) {
    Node* y = x->right;
    Node* px = x->parent;

    //修改右，父
    x->right = y->left;
    y->parent = px;

    //左指针
    if (y->left != NULL) {
        y->left->parent = x;
    }

    //处理父节点
    if (px == NULL) {
        tree->root = y;
    } else {
        if (px->left == x) {
            px->left = y;
        } else {
            px->right = y;
        }
    }

    y->left = x;
    x->parent = y;
}


/*
 * 对红黑树的节点(x)进行右旋转
 *
 * 右旋示意图(对节点x进行左旋)：
 *            px                               px
 *           /                                /
 *          x                                y
 *         /  \      --(右旋)-->            /  \                     #
 *        y   rx                           ly   x
 *       / \                                   / \                   #
 *      ly  ry                                ry  rx
 *
 */
void tree_right_rotate(Tree* tree, Node* x) {
    Node* y = x->left;
    Node* px = x->parent;

    y->parent = px;
    x->left = y->right;


    if (y->right != NULL) {
        y->right->parent = x;
    }

    if (px == NULL) {
        tree->root = y;
    } else {
        if (px->left == x) {
            px->left = y;
        } else {
            px->right = y;
        }
    }


    y->right = x;
    x->parent = y;
}


int tree_find(Tree* tree, Node* node) {
    return 0;
}

void tree_insert_fix(Tree* tree, Node* node) {
    Node* parent, * gparent, * uncle;

    //黑色 自平衡
    // 若“父节点存在，并且父节点的颜色是红色” => 必定有祖父
    while ((parent = node->parent) && IS_RED(parent)) {
        gparent = parent->parent;

        //处理左孩子
        if (parent == gparent->left) {
            uncle = gparent->right;


//*          gp(b)            gp(r)
//*         /  \             /  \
//*        p(r) u(r)       p(b) u(b)
//*       /                /
//*      n(r)             n(r)
            // 叔叔存在是红
            if (uncle && IS_RED(uncle)) {
                //变色 指针指向祖父，递归
                MARK_BLACK(uncle);
                MARK_BLACK(parent);
                MARK_RED(gparent);
                node = gparent;
                continue;
            }

            //叔叔不管存在不存在 就是黑色的

            if (parent->right == node) {
                //左旋
                //current 是右节点
                //            LR 双红
                // 指针对象修改
                tree_left_rotate(tree, parent);
                Node* tmp = parent;
                parent = node;
                node = tmp;
            }

            //current 左节点
            //            LL 双红
            MARK_BLACK(parent);
            MARK_RED(gparent);
            tree_right_rotate(tree, gparent);


        } else {
            //处理右孩子
            uncle = gparent->left;
            // 叔叔存在是红
            if (uncle && IS_RED(uncle)) {
                //变色 指针指向祖父，递归
                MARK_BLACK(uncle);
                MARK_BLACK(parent);
                MARK_RED(gparent);
                node = gparent;
                continue;
            }

            if (parent->left == node) {
                tree_right_rotate(tree, parent);
                Node* tmp = parent;
                parent = node;
                node = tmp;
            }


            //current 右节点
            //            RR 双红
            MARK_BLACK(parent);
            MARK_RED(gparent);
            tree_left_rotate(tree, gparent);

        }

    }


    MARK_BLACK(tree->root);
}


void tree_insert(Tree* tree, Node* node) {
    Node* parent = NULL;
    Node* root = tree->root;

    int ret = 0;
    while (root != NULL) {
        parent = root;
        ret = tree->compare(node->key, root->key);
        //小于
        if (ret == -1) {
            root = root->left;
        } else if (ret == 1) {
            root = root->right;
        } else if (ret == 0) {
            return;
        }
    }


    node->parent = parent;

    if (parent != NULL) {
        if (ret == 1) {
            parent->right = node;
        } else {
            parent->left = node;
        }

    } else {
        //空树
        tree->root = node;
    }


    MARK_RED(node);
    tree_insert_fix(tree, node);

}

static void inner_free(Node* node) {
    if (node) {
        inner_free(node->left);
        inner_free(node->right);
        free(node);
    }
}

void tree_free(Tree* tree) {
    if (tree) {
        inner_free(tree->root);
    }
}

static void inorder(Node* node, int lv) {
    if (node != NULL) {

        inorder(node->left, lv + 3);
        for (int i = 0; i < lv; ++i) {
            printf(" ");
        }
        printf("%d\n", node->key);
        inorder(node->right, lv + 3);
    }
}

void tree_print_inorder(Tree* tree) {
    printf("tree inorder:\n");
    inorder(tree->root, 0);
    printf("\n");
}

Node* tree_search(Tree* tree, Key key) {
    Node* root = tree->root;
    int ret;
    COMPARE:
    if (root == NULL) {
        return NULL;
    }
    ret = tree->compare(root->key, key);
    if (ret == 0) {
        return root;
    } else if (ret > 0) {
        root = root->left;
        goto COMPARE;
    } else if (ret < 0) {
        root = root->right;
        goto COMPARE;
    }

    return NULL;
}


void tree_delete_fixup(Tree* tree, Node* node, Node* parent) {
    Node* other;

    while ((!node || IS_BLACK(node)) && node != tree->root) {

        if (parent->left == node) {
            other = parent->right;
            if (IS_RED(other)) {
                // Case 1: x的兄弟w是红色的
                MARK_BLACK(other);
                MARK_RED(parent);
                tree_left_rotate(tree, parent);
                other = parent->right;
            }
            if ((!other->left || IS_BLACK(other->left)) &&
                (!other->right || IS_BLACK(other->right))) {
                // Case 2: x的兄弟w是黑色，且w的俩个孩子也都是黑色的
                MARK_RED(other);
                node = parent;
                parent = node->parent;
            } else {
                if (!other->right || IS_BLACK(other->right)) {
                    // Case 3: x的兄弟w是黑色的，并且w的左孩子是红色，右孩子为黑色。
                    MARK_BLACK(other->left);
                    MARK_RED(other);
                    tree_right_rotate(tree, other);
                    other = parent->right;
                }
                // Case 4: x的兄弟w是黑色的；并且w的右孩子是红色的，左孩子任意颜色。
                MARK_COLOR(other, parent->c);
                MARK_BLACK(parent);
                MARK_BLACK(other->right);
                tree_left_rotate(tree, parent);
                node = tree->root;
                break;
            }
        } else {
            other = parent->left;
            if (IS_RED(other)) {
                // Case 1: x的兄弟w是红色的
                MARK_BLACK(other);
                MARK_RED(parent);
                tree_right_rotate(tree, parent);
                other = parent->left;
            }
            if ((!other->left || IS_BLACK(other->left)) &&
                (!other->right || IS_BLACK(other->right))) {
                // Case 2: x的兄弟w是黑色，且w的俩个孩子也都是黑色的
                MARK_RED(other);
                node = parent;
                parent = node->parent;
            } else {
                if (!other->left || IS_BLACK(other->left)) {
                    // Case 3: x的兄弟w是黑色的，并且w的左孩子是红色，右孩子为黑色。
                    MARK_BLACK(other->right);
                    MARK_RED(other);
                    tree_left_rotate(tree, other);
                    other = parent->left;
                }
                // Case 4: x的兄弟w是黑色的；并且w的右孩子是红色的，左孩子任意颜色。
                MARK_COLOR(other, parent->c);
                MARK_BLACK(parent);
                MARK_BLACK(other->left);
                tree_right_rotate(tree, parent);
                node = tree->root;
                break;
            }
        }


    }

    if (node)
        MARK_BLACK(node);

}

Node* tree_delete(Tree* tree, Node* node) {
    Node* child, * parent;
    Color color;

    // 被删除节点的"左右孩子都不为空"的情况。
    //找后继 值替换 ！！！ 树中大于节点的最小的元素
    if ((node->left != NULL) && (node->right != NULL)) {
        Node* replace = node->right;
        while (replace->left) {
            replace = replace->left;
        }

//        /* 方法2 递归一次调用 拷贝值 变成删除后继节点
//         *
//         *
//         */

//        node->key = replace->key;
//        return tree_delete(tree, replace);


//        /* 方法1 直接把后继节点的指针引用修改了，并变色


        if (node->parent) {
            if (node->parent->left == node) {
                node->parent->left = replace;
            } else {
                node->parent->right = replace;
            }

        } else {
            //没有父节点 就是根了
            tree->root = replace;
        }


        // child是"取代节点"的右孩子，也是需要"调整的节点"。
        // "取代节点"肯定不存在左孩子！因为它是一个后继节点。
        child = replace->right;
        parent = replace->parent;
        // 保存"取代节点"的颜色
        color = replace->c;

        // "被删除节点"是"它的后继节点的父节点"
        if (parent == node) {
            parent = replace;
        } else {
            // child不为空
            if (child)
                child->parent = parent;

            parent->left = child;
            replace->right = node->right;
            node->right->parent = replace;
        }

        replace->parent = node->parent;
        replace->c = node->c;
        replace->left = node->left;
        node->left->parent = replace;


        if (color == COLOR_BLACK)
            tree_delete_fixup(tree, child, parent);

        return node;
    }


    if (node->left != NULL)
        child = node->left;
    else
        child = node->right;

    parent = node->parent;
    // 保存"取代节点"的颜色
    color = node->c;

    if (child)
        child->parent = parent;

    // "node节点"不是根节点
    if (parent) {
        if (parent->left == node)
            parent->left = child;
        else
            parent->right = child;
    } else
        tree->root = child;

    if (color == COLOR_BLACK)
        tree_delete_fixup(tree, child, parent);

    return node;
}

static int tree_inner_height(Node* node) {
    if (node == NULL) {
        return 0;
    }
    int lh = tree_inner_height(node->left);
    int rh = tree_inner_height(node->right);
    return lh > rh ? lh + 1 : rh + 1;

}

int tree_height(Tree* tree) {
    return tree_inner_height(tree->root);
}

//循环队列
typedef struct queue_s queue_t;
struct queue_s{
    int head;
    int tail;
    int cap;
    void** arr;
};

static void queue_expand(queue_t* q) {

    void** arr = malloc(2 * q->cap * sizeof(void*));
    for (int i = 0; i < q->cap; i++) {
        arr[i] = q->arr[(q->head + i) % q->cap];
    }

    q->head = 0;
    q->tail = q->cap;
    q->cap *= 2;

    free(q->arr);
    q->arr = arr;
}

static queue_t* queue_create(int cap) {
    queue_t* q = malloc(sizeof(*q));
    q->cap = cap;
    q->head = q->tail = 0;
    q->arr = malloc(cap * sizeof(void*));
    return q;
}

static void queue_free(queue_t* q) {
    free(q->arr);
    free(q);
}

static void queue_push(queue_t* q, void* p) {
    q->arr[q->tail++] = p;
    if (q->tail == q->cap) {
        q->tail = 0;
    }
    if (q->tail == q->head) {
        queue_expand(q);
    }
}

static void* queue_pop(queue_t* q) {
    void* data = NULL;
    if (q->head != q->tail) {
        data = q->arr[q->head++];
        if (q->head >= q->cap) {
            q->head = 0;
        }
    }

    return data;
}

static int queue_len(queue_t* q) {
    if (q->head <= q->tail) {
        return q->tail - q->head;
    }
    return q->tail + q->cap - q->head;
}

static void print_help(Node* node, int row, int col, unsigned char** res, int height) {
    if (node == NULL) return;

    res[row][col] = IS_BLACK(node) ? (100 + node->key) : node->key;

    // 计算当前位于树的第几层
    int currLevel = ((row + 1) / 2);
    // 若到了最后一层，则返回
    if (currLevel == height) return;
    // 计算当前行到下一行，每个元素之间的间隔（下一行的列索引与当前元素的列索引之间的间隔）
    int gap = height - currLevel - 1;

    // 对左儿子进行判断，若有左儿子，则记录相应的"/"与左儿子的值
    if (node->left != NULL) {
        res[row + 1][col - gap] = '/';
        print_help(node->left, row + 2, col - gap * 2, res, height);
    }

    // 对右儿子进行判断，若有右儿子，则记录相应的"\"与右儿子的值
    if (node->right != NULL) {
        res[row + 1][col + gap] = '\\';
        print_help(node->right, row + 2, col + gap * 2, res, height);
    }


}

void tree_print(Tree* tree) {
    printf("tree print:\n");
    int height = tree_height(tree);
    if (height == 0) {
        return;
    }

    int array_h = height * 2 - 1;
    int array_w = (2 << (height - 2)) * 3 + 1;
    unsigned char** res = malloc(array_h * sizeof(unsigned char*));


    for (int x = 0; x < array_h; ++x) {
        res[x] = malloc(array_w * sizeof(unsigned char));
        for (int i = 0; i < array_w; ++i) {
            res[x][i] = ' ';
        }
    }

    print_help(tree->root, 0, array_w / 2, res, height);
    for (int x = 0; x < array_h; ++x) {
        for (int i = 0; i < array_w; ++i) {
            if (res[x][i] == ' ') {
                printf(" ");
            } else if (res[x][i] == '\\') {
                printf("\\");
            } else if (res[x][i] == '/') {
                printf("/");
            } else {
                if (res[x][i] > 100) {
                    printf("\033[47;30m%d\033[0m", res[x][i] - 100);
                } else {
                    printf("\033[31;4m%d\033[0m", res[x][i]);
                }

            }
        }
        printf("\n");
    }

    for (int x = 0; x < array_h; ++x) {
        free(res[x]);
    }
    free(res);
    return;


#define NONE          "\033[0m"
#define RED           "\033[31m"
#define BLACK           "\033[30m"


    queue_t* q = queue_create(16);
    queue_t* q2 = queue_create(q->cap * 2);
    int tsize[height];
    int ih = 0;
    int len;
    queue_push(q, tree->root);


    while (len = queue_len(q), len != 0) {
        for (int i = 0; i < len; ++i) {
            Node* node = queue_pop(q);
            queue_push(q2, node);
            if (node->left) {
                queue_push(q, node->left);
            }
            if (node->right) {
                queue_push(q, node->right);
            }
        }
        tsize[ih++] = len;
//        queue_push(q2,NULL);
    }
//    len = queue_len(q2);

    for (int k = 0; k < height; ++k) {

        int ts = tsize[k];

        for (int i = 0; i < ts; ++i) {
            Node* node = queue_pop(q2);
            for (int j = 0; j < height; ++j) {
                printf(" ");
            }
            if (IS_RED(node)) {
                printf(RED "%d" NONE, node->key);
            } else {
                printf(BLACK "%d" NONE, node->key);
            }

        }
        printf("\n");

//        int rn = k==0;
//        Node *node = queue_pop(q2);
//        if(node==NULL){
//            for (int j = 0; j < 2 * height; ++j) {
//                printf(" ");
//            }
//            printf("/");
//            for (int j = 0; j < 3; ++j) {
//                printf(" ");
//            }
//            printf("\\");
//            printf("\n");
//            rn = 1;
//            continue;
//        }
//
//        for (int j = 0; j < 2 * height; ++j) {
//            printf(" ");
//        }
//        if (IS_RED(node)) {
//            printf(RED "%d" NONE, node->key);
//        } else {
//            printf(BLACK "%d" NONE, node->key);
//        }
//        if(rn){
//            printf("\n");
//        }


//        height--;

    }









//    Node *last = NULL;

//    while (len = queue_len(q), len != 0) {
//
//        for (int i = 0; i < len; ++i) {
//            Node *node = queue_pop(q);
//            for (int j = 0; j < 2 * height; ++j) {
//                printf(" ");
//            }
//            if (IS_RED(node)) {
//                printf(RED "%d\n" NONE, node->key);
//            } else {
//                printf(BLACK "%d\n" NONE, node->key);
//            }
////            if (last != NULL) {
////
////            }
//
//
////            last = node;
//
//            int rn = 0;
//
//            if (node->left) {
//                for (int j = 0; j < 2 * height - 1; ++j) {
//                    printf(" ");
//                }
//                printf("/");
//                queue_push(q, node->left);
//                rn = 1;
//            }
//            if (node->right) {
//                int jm = 2 * height;
//                if (rn > 0) {
//                    jm = 3;
//                }
//                for (int j = 0; j < jm; ++j) {
//                    printf(" ");
//                }
//                printf("\\");
//                queue_push(q, node->right);
//                rn = 1;
//            }
//            if (rn > 0) {
//                printf("\n");
//            }
//            height--;
//
//        }
//
//
//    }


//    printf("\n");
}

void tree_test(Tree* tree) {
    return;
    queue_t* q = queue_create(2);
    queue_push(q, (void*) 1);
    queue_push(q, (void*) 2);

    printf("%d\n", queue_len(q));

    int a1 = queue_pop(q);
    int a2 = queue_pop(q);

}