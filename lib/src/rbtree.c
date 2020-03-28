#include "rbtree.h"

void left_rotate(struct rb_node **root, struct rb_node *x)
{
    struct rb_node *y = x->right;
    x->right = y->left;
    if (y->left)
        y->left->parent = x;
    y->parent = x->parent;
    if (!(x->parent))
        (*root) = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->left = x;
    x->parent = y;
}

void right_rotate(struct rb_node **root, struct rb_node *x)
{
    struct rb_node *y = x->left;
    x->left = y->right;
    if (y->right)
        y->right->parent = x;
    y->parent = x->parent;
    if (!(x->parent))
        (*root) = y;
    else if (x == x->parent->right)
        x->parent->right = y;
    else
        x->parent->left = y;
    y->right = x;
    x->parent = y;
}

struct rb_node *insert(struct rb_node **root, void *new_key)
{
    struct rb_node *x, *y;
    /* allocate memory for the new rb_node */
    struct rb_node *z;
    if (!(z = (struct rb_node *)malloc(sizeof(struct rb_node))))
        return NULL;
    z->key = new_key;
    z->left = z->right = z->parent = NULL;
    /* add new node to tree */
    if (!(*root))
    {
        z->color = BLACK;
        (*root) = z;
    }
    else
    {
        y = NULL;
        x = (*root);
        while (x)
        {
            y = x;
            if (z->key < x->key)
                x = x->left;
            else
                x = x->right;
        }
        z->parent = y;
        if (z->key > y->key)
            y->right = z;
        else
            y->left = z;
        z->color = RED;
        insert_fixup(root, z);
    }
    return z;
}

void insert_fixup(struct rb_node **root, struct rb_node *z)
{
    struct rb_node *parent = NULL, *grand_parent = NULL, *uncle = NULL;
    unsigned int color;
    while (z != (*root) && z->color != BLACK && z->parent->color == RED)
    {
        parent = z->parent;
        grand_parent = z->parent->parent;
        /*  Case : A 
            Parent of z is left child of Grand-parent of z */
        if (parent == grand_parent->left)
        {
            uncle = grand_parent->right;
            /* Case : 1 
               The uncle of z is also red 
               Only Recoloring required */
            if (uncle && uncle->color == RED)
            {
                grand_parent->color = RED;
                parent->color = BLACK;
                uncle->color = BLACK;
                z = grand_parent;
            }
            else
            {
                /* Case : 2 
                   z is right child of its parent 
                   Left-rotation required */
                if (z == parent->right)
                {
                    left_rotate(root, parent);
                    z = parent;
                    parent = z->parent;
                }

                /* Case : 3 
                   z is left child of its parent 
                   Right-rotation required */
                right_rotate(root, grand_parent);
                color = parent->color;
                parent->color = grand_parent->color;
                grand_parent->color = color;
                z = parent;
            }
        }
        /* Case : B 
           Parent of z is right child of Grand-parent of z */
        else
        {
            uncle = grand_parent->left;

            /*  Case : 1 
                The uncle of z is also red 
                Only Recoloring required */
            if (uncle && uncle->color == RED)
            {
                grand_parent->color = RED;
                parent->color = BLACK;
                uncle->color = BLACK;
                z = grand_parent;
            }
            else
            {
                /* Case : 2 
                   z is left child of its parent 
                   Right-rotation required */
                if (z == parent->left)
                {
                    right_rotate(root, parent);
                    z = parent;
                    parent = z->parent;
                }

                /* Case : 3 
                   z is right child of its parent 
                   Left-rotation required */
                left_rotate(root, grand_parent);
                color = parent->color;
                parent->color = grand_parent->color;
                grand_parent->color = color;
                z = parent;
            }
        }
    }
    (*root)->color = BLACK;
}

void remove_node(struct rb_node **root, struct rb_node *z)
{
    struct rb_node *replacement = NULL, *sibling = NULL, *parent = z->parent;

    /* we have to find z's replacement */

    /* when z has 2 children */
    if (z->left && z->right)
    {
        /* replace with its successor */
        replacement = z;
        while (replacement->left)
            replacement = replacement->left;
    }
    /* when z has a single child*/
    else if (z->left && !(z->right))
        replacement = z->left;
    else if (z->right && !(z->left))
        replacement = z->right;

    /* perform replacement */

    /* when z has no children(is leaf) */
    if (!replacement)
    {
        if ((*root) == z)   //if z is root
            (*root) = NULL; //make root NULL
        else
        {
            if (z->color == BLACK)
                fix_double_black(root, z);
            else
            {
                /* get z's sibling and make it red */
                sibling = (z == parent->left) ? parent->right : parent->left;
                if (sibling)
                    sibling->color = RED;
            }
            /* delete z's reference from the tree */
            if (z == parent->left)
                parent->left = NULL;
            else
                parent->right = NULL;
        }
        /* delete z */
        free(z);
        return;
    }
    /* z has 1 child*/
    else if ((z->left && !(z->right)) || (z->right && !(z->left)))
    {
        if ((*root) == z) //if z is root
        {
            /* make replacement the new root */
            (*root) = replacement;
            replacement->parent = NULL;
        }
        else
        {
            /* remove z from tree and move replacement up */
            if (z == parent->left)
                parent->left = replacement;
            else
                parent->right = replacement;
            replacement->parent = parent;
            if (z->color == BLACK && replacement->color == BLACK)
                fix_double_black(root, replacement);
            else
                replacement->color = BLACK;
        }
        /* delete z*/
        free(z);
        return;
    }
    /* z has 2 children*/
    else
    {
        /* swap key and record pointer of z with replacement */
        void *z_key = z->key;
        struct lkrecord *z_record_ptr = z->record_ptr;
        z->key = replacement->key;
        z->record_ptr = replacement->record_ptr;
        replacement->key = z_key;
        replacement->record_ptr = z_record_ptr;
        /* recurse with replacement (z's values) */
        remove_node(root, replacement);
    }
}

void fix_double_black(struct rb_node **root, struct rb_node *x)
{
    struct rb_node *sibling = NULL, *parent = x->parent;
    if ((*root) == x)
        return;

    sibling = (x == (parent->left)) ? (parent->right) : (parent->left);
    if (!sibling)
        fix_double_black(root, parent);
    else
    {
        /* sibling is red */
        if (sibling->color == RED)
        {
            parent->color = RED;
            sibling->color = BLACK;
            if (sibling == sibling->parent->left)
                right_rotate(root, parent);
            else
                left_rotate(root, parent);
            fix_double_black(root, x);
        }
        /* sibling is black */
        else
        {
            /* check for red children */
            if (sibling->left && sibling->left->color == RED)
            {
                /*Left Left Case*/
                if (sibling == sibling->parent->left)
                {
                    sibling->left->color = sibling->color;
                    sibling->color = parent->color;
                    right_rotate(root, parent);
                }
                /*Right Left Case*/
                else
                {
                    sibling->left->color = parent->color;
                    right_rotate(root, sibling);
                    left_rotate(root, parent);
                }
            }
            else if (sibling->right && sibling->right->color == RED)
            {
                /*Left Right Case*/
                if (sibling == sibling->parent->left)
                {
                    sibling->right->color = parent->color;
                    left_rotate(root, parent);
                    right_rotate(root, parent);
                }
                /*Right Right Case*/
                else
                {
                    sibling->right->color = sibling->color;
                    sibling->color = parent->color;
                    left_rotate(root, parent);
                }
            }
            else
            {
                /* has black children */
                sibling->color = RED;
                if (parent->color == BLACK)
                    fix_double_black(root, parent);
                else
                    parent->color = BLACK;
            }
        }
    }
}

struct rb_node *find_node_exact(struct rb_node *node, void *addr)
{
    /* start at the root */
    if (!node)
        return NULL;
    /* check if the root is the desired node*/
    if (node->key == addr)
        return node;
    /* key is greater than root's key */
    if (node->key < addr)
        return find_node_exact(node->right, addr);
    /* key is less than root's key */
    return find_node_exact(node->left, addr);
}

struct rb_node *find_node_approx(struct rb_node *node, void *addr)
{
    /* start at the root */
    if (!node)
        return NULL;
    /* check if the root is the desired node (check within the malloc block) */
    if ((node->record_ptr->data.malloc_info.real_ptr <= addr) && (addr <= (node->record_ptr->data.malloc_info.real_ptr) + (node->record_ptr->data.malloc_info.real_size)))
        return node;
    /* key is greater than root's key */
    if (node->key < addr)
        return find_node_approx(node->right, addr);
    /* key is less than root's key */
    return find_node_approx(node->left, addr);
}

void destroy_tree(struct rb_node **root)
{
    struct rb_node *node = NULL;
    struct lkrecord *record = NULL;
    if (!(*root))
        return;
    while ((node = *root))
    {
        while ((record = pop_record_from_node(node)))
            destroy_record(record);
        remove_node(root, node);
    }
}