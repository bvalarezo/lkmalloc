#ifndef RBTREE_H
#define RBTREE_H

/* red black tree node */
struct rb_node
{
    void *key;                   //address returned
    unsigned int color : 1;      //0 = black, 1 = red
    struct lkrecord *record_ptr; //pointer to record
    struct rb_node *left, *right, *parent;
};

/* Colors for Tree*/
#define BLACK 0
#define RED 1

/* Include */
#include <stdlib.h>
#include <stdio.h>
#include "record.h"

/* Prototypes */
void left_rotate(struct rb_node **root, struct rb_node *x);
void right_rotate(struct rb_node **root, struct rb_node *x);
struct rb_node *insert(struct rb_node **root, void *new_key);
void insert_fixup(struct rb_node **root, struct rb_node *z);
void remove_node(struct rb_node **root, struct rb_node *z);
void fix_double_black(struct rb_node **root, struct rb_node *x);
struct rb_node *find_node_exact(struct rb_node **root, void *key);
struct rb_node *find_node_approx(struct rb_node **root, void *key);
void destroy_tree(struct rb_node **root);

#endif