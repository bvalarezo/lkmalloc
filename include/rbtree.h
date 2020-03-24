#ifndef RBTREE_H
#define RBTREE_H

/* Include */
#include <stdlib.h>
#include <stdio.h>

/* rb_node */
struct rb_node
{
    unsigned long key;           //address returned
    unsigned int color : 1;      //0 = black, 1 = red
    struct lkrecord *record_ptr; //pointer to record
    struct rb_node *left, *right, *parent;
};

/* Colors for Tree*/
#define BLACK 0
#define RED 1

/* Prototypes */
static void left_rotate(struct rb_node **root, struct rb_node *x);
static void right_rotate(struct rb_node **root, struct rb_node *x);
struct rb_node *insert(struct rb_node **root, unsigned long new_key);
static void insert_fixup(struct rb_node **root, struct rb_node *z);
void remove_node(struct rb_node **root, struct rb_node *z);
static void fix_double_black(struct rb_node **root, struct rb_node *x);
struct rb_node *find_node_exact(struct rb_node **root, unsigned long key);
struct rb_node *find_node_approx(struct rb_node **root, unsigned long key);
#endif