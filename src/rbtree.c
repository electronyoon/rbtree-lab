#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  
  p->nil = (node_t *)calloc(1, sizeof(node_t));
  p->nil->color = RBTREE_BLACK;
  p->root = p->nil;

  return p;
}

void delete_rbtree(rbtree *t) {
  if (t->root != t->nil)
    delete_node(t, t->root);

  free(t->nil);
  free(t);
}

void delete_node(rbtree *t, node_t *node) {
  if (node != t->nil) {
    delete_node(t, node->left);
    delete_node(t, node->right);

    free(node);
  }
}

void rbtree_insert_fixup(rbtree *t, node_t *z) {
  node_t* y;
  while (z->parent->color == RBTREE_RED) {
    if (z->parent == z->parent->parent->left) {
      y = z->parent->parent->right;
      if (y->color == RBTREE_RED) {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      } else {
        if (z == z->parent->right) {
          z = z->parent;
          left_rotate(t, z);
        }
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotate(t, z->parent->parent);
      }
    } else {
      y = z->parent->parent->left;
      if (y->color == RBTREE_RED) {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      } else {
        if (z == z->parent->left) {
          z = z->parent;
          right_rotate(t, z);
        }
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        left_rotate(t, z->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
};

void left_rotate(rbtree *t, node_t *x) {
  // LEFT-ROTATE
  node_t *y = x->right;
  x->right = y->left;
  if (y->left != t->nil)
    y->left->parent = x;
  
  y->parent = x->parent;
  if (x->parent == t->nil)
    t->root = y;
  else if (x == x->parent->left)
    x->parent->left = y;
  else
    x->parent->right = y;

  y->left = x;
  x->parent = y;
};

void right_rotate(rbtree *t, node_t *y) {
  // RIGHT-ROTATE
  node_t *x = y->left;
  y->left = x->right;
  if (x->right != t->nil)
    x->right->parent = y;

  x->parent = y->parent;
  if (y->parent == t->nil)
    t->root = x;
  else if (y == y->parent->left)
    y->parent->left = x;
  else
    y->parent->right = x;

  x->right = y;
  y->parent = x;
};

node_t *rbtree_insert(rbtree *t, const key_t key) {
  node_t *y = t->nil;
  node_t *x = t->root;
  node_t *z = (node_t *)calloc(1, sizeof(node_t));
  z->key = key;
  while (x != t->nil) {
    y = x;
    if (key < x->key)
      x = x->left;
    else
      x = x->right;
  }
  z->parent = y;
  if (y == t->nil)
    t->root = z;
  else if (key < y->key)
    y->left = z;
  else
    y->right = z;
  z->left = t->nil;
  z->right = t->nil;
  z->color = RBTREE_RED;

  rbtree_insert_fixup(t, z);
  return t->root;
};

node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *current = t->root;
  node_t *nil = t->nil;

  while (current != nil) {
    if (key == current->key)
      return current;
    else if (key < current->key)
      current = current->left;
    else
      current = current->right;
  }

  return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  node_t *x = t->root;
  if (x == t->nil)
    return NULL;
  while (x->left != t->nil)
    x = x->left;
  return x;
};

node_t *rbtree_max(const rbtree *t) {
  node_t *x = t->root;
  if (x == t->nil)
    return NULL;
  while (x->right != t->nil)
    x = x->right;
  return x;
}

int rbtree_erase(rbtree *t, node_t *p) {
  node_t *y = p;
  node_t *x;
  color_t original_color = y->color;
  if (p->left == t->nil) {
    x = p->right;
    rbtree_transplant(t, p, p->right);
  } else if (p->right == t->nil) {
    x = p->left;
    rbtree_transplant(t, p, p->left);
  } else {
    node_t *original_root = t->root;
    t->root = p->right;
    y = rbtree_min(t);
    t->root = original_root;
    original_color = y->color;
    x = y->right;
    if (y->parent == p) {
      x->parent = y;
    } else {
      rbtree_transplant(t, y, y->right);
      y->right = p->right;
      y->right->parent = y;
    }
    rbtree_transplant(t, p, y);
    y->left = p->left;
    y->left->parent = y;
    y->color = p->color;
  }
  
  if (original_color == RBTREE_BLACK)
    rbtree_delete_fixup(t, x);
  
  free(p);
  return 0;
}

void rbtree_transplant(rbtree *t, node_t *u, node_t *v) {
  if (u->parent == t->nil)
    t->root = v;
  else if (u == u->parent->left)
    u->parent->left = v;
  else
    u->parent->right = v;

  v->parent = u->parent;
};

void rbtree_delete_fixup(rbtree *t, node_t *x) {
  node_t *w;
  while (x != t->root && x->color == RBTREE_BLACK) {
    if (x == x->parent->left) {
      w = x->parent->right;
      // case 1
      if (w->color == RBTREE_RED) {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t, x->parent);
        w = x->parent->right;
      }
      // case 2
      else if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK) {
        w->color = RBTREE_RED;
        x = x->parent;
      }
      // case 3
      else if (w->right->color == RBTREE_BLACK) {
        w->left->color = RBTREE_BLACK;
        w->color = RBTREE_RED;
        right_rotate(t, w);
        w = x->parent->right;
      }
      // case 4
      else {
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        left_rotate(t, x->parent);
        x = t->root;
      }
    }
    else {
      w = x->parent->left;
      // case 1
      if (w->color == RBTREE_RED) {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent);
        w = x->parent->left;
      }
      // case 2
      else if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK) {
        w->color = RBTREE_RED;
        x = x->parent;
      }
      // case 3
      else if (w->left->color == RBTREE_BLACK) {
        w->right->color = RBTREE_BLACK;
        w->color = RBTREE_RED;
        left_rotate(t, w);
        w = x->parent->left;
      }
      // case 4
      else {
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        right_rotate(t, x->parent);
        x = t->root;
      }
    }
  }
  x->color = RBTREE_BLACK;
};

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  int counter = 0;
  inorder_traverse(arr, t->root, t->nil, &counter);
  return 0;
}

void inorder_traverse(key_t arr[], node_t *x, node_t *nil, int *i) {
  if (x == nil)
    return;

  inorder_traverse(arr, x->left, nil, i);
  arr[(*i)++] = x->key;
  inorder_traverse(arr, x->right, nil, i);
  return;
};
