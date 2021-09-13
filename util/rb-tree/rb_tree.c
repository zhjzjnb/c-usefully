

#include "rb_tree.h"
#include <stdlib.h>

static int default_compare(Key a, Key b){
    if (a==b) {
        return 0;
    }
    if (a>b) {
        return 1;
    }
    return -1;
}

void tree_init(Tree *tree, sort_fuc fun){
    tree->root = NULL;
    
    tree->compare = fun==NULL?default_compare:fun;
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
void tree_left_rotate(Tree *tree, Node *x){
    Node *y = x->right;
    Node *px = x->parent;
    
    //修改右，父
    x->right = y->left;
    y->parent = px;
    
    //左指针
    if(y->left!=NULL){
        y->left->parent = x;
    }
    
    //处理父节点
    if (px==NULL) {
        tree->root = y;
    }else{
        if (px->left==x) {
            px->left = y;
        }else{
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
void tree_right_rotate(Tree *tree, Node *x){
    Node *y = x->left;
    Node *px = x->parent;
    
    y->parent = px;
    x->left = y->left;
    
    
    
    if (y->right!=NULL) {
        y->right->parent = x;
    }
    
    if (px==NULL) {
        tree->root = y;
    }else{
        if (px->left==x) {
            px->left = y;
        }else{
            px->right = y;
        }
    }
    

    y->right = x;
    x->parent = y;
}


int tree_find(Tree *tree, Node *node){
    return 0;
}

void tree_insert_fix(Tree *tree, Node *node){
    Node *parent, *gparent, *uncle;
    
    //黑色 自平衡
    // 若“父节点存在，并且父节点的颜色是红色” => 必定有祖父
    while ((parent = node->parent) && IS_RED(parent)){
        gparent = parent->parent;
        
        //处理左孩子
        if (parent==gparent->left) {
            uncle = gparent->right;
            

//*          gp(b)            gp(r)
//*         /  \             /  \
//*        p(r) u(r)       p(b) u(b)
//*       /                /
//*      n(r)             n(r)
            // 叔叔存在是红
            if (uncle&&IS_RED(uncle)) {
                //变色 指针指向祖父，递归
                MARK_BLACK(uncle);
                MARK_BLACK(parent);
                MARK_RED(gparent);
                node = gparent;
                continue;
            }
            
            //叔叔不管存在不存在 就是黑色的

            if(parent->right==node){
                //左旋
                //current 是右节点
    //            LR 双红
                // 指针对象修改
                tree_left_rotate(tree,parent);
                Node *tmp = parent;
                parent = node;
                node = tmp;
            }
            
            //current 左节点
            //            LL 双红
            MARK_BLACK(parent);
            MARK_RED(gparent);
            tree_right_rotate(tree,gparent);
            
            
        }else{
        //处理右孩子
            uncle = gparent->left;
            // 叔叔存在是红
            if (uncle&&IS_RED(uncle)) {
                //变色 指针指向祖父，递归
                MARK_BLACK(uncle);
                MARK_BLACK(parent);
                MARK_RED(gparent);
                node = gparent;
                continue;
            }
            
            if(parent->left==node){
                tree_right_rotate(tree,parent);
                Node *tmp = parent;
                parent = node;
                node = tmp;
            }
            
            
            //current 右节点
            //            RR 双红
            MARK_BLACK(parent);
            MARK_RED(gparent);
            tree_left_rotate(tree,gparent);
            
        }
        
    }
    
    
    MARK_BLACK(tree->root);
}




void tree_insert(Tree *tree, Node *node){
    Node *parent = NULL;
    Node *root = tree->root;
    
    int ret = 0;
    while (root!=NULL) {
        parent = root;
        ret = tree->compare(node->key,root->key);
        //小于
        if (ret==-1) {
            root = root->left;
        }else if(ret==1){
            root = root->right;
        }else if(ret==0){
            return;
        }
    }

    
    node->parent = parent;
        
    if (parent!=NULL) {
        if (ret==1) {
            parent->right = node;
        }else{
            parent->left = node;
        }
        
    }else{
        //空树
        tree->root = node;
    }
    
    
    MARK_RED(node);
    tree_insert_fix(tree,node);
    
}

static void inner_free(Node *node){
    if (node) {
        inner_free(node->left);
        inner_free(node->right);
        free(node);
    }
}
void tree_free(Tree *tree){
    if (tree) {
        inner_free(tree->root);
    }
}

static void inorder(Node *node){
    if (node!=NULL) {
        inorder(node->left);
        printf("%d ",node->key);
        inorder(node->right);
    }
}
void tree_print_inorder(Tree *tree){
    printf("tree inorder:\n");
    inorder(tree->root);
    printf("\n");
}

Node *tree_search(Tree *tree, Key key){
    Node *root = tree->root;
    int ret;
COMPARE:
    if (root==NULL) {
        return NULL;
    }
    ret = tree->compare(root->key,key);
    if (ret==0) {
        return root;
    }else if(ret>0){
        root = root->left;
        goto COMPARE;
    }else if(ret<0){
        root = root->right;
        goto COMPARE;
    }

    return NULL;
}

void tree_delete(Tree *tree,Node *node){
    Node *child, *parent;
    
    
    // 被删除节点的"左右孩子都不为空"的情况。
    if ( (node->left!=NULL) && (node->right!=NULL) ){
        
    }
    
    
    
    
}
