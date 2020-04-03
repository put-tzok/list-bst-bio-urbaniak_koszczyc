#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

unsigned int ns[] = { 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000/* TODO: fill values which will be used as lists' sizes */ };

// each tree node contains an integer key and pointers to left and right children nodes
struct node {
    int key;
    struct node *left;
    struct node *right;
};

// tree's beginning is called the root
struct node *root = NULL;

struct node **tree_search(struct node **candidate, int value) {
    if(*candidate == NULL)
      return candidate;
    if(value < (**candidate).key)
      return tree_search(&(**candidate).left, value);
    if(value > (**candidate).key)
      return tree_search(&(**candidate).right, value);
      return candidate;
}

struct node **candidate;
// struct node **maxnodeptr;
struct node* tree_insert(int value) {
    // TODO: implement
    candidate = tree_search(&root, value);
    struct node *createNode = malloc(sizeof(*createNode));
    (*createNode).key = value;
    (*createNode).left = NULL;
    (*createNode).right = NULL;
    *candidate = createNode;
}

struct node **tree_maximum(struct node **candidate) {
    // TODO: implement
    if((**candidate).right != NULL)
      return tree_maximum(&(**candidate).right);
    return candidate;
}

void tree_delete(int value) {
    // TODO: implement
    candidate = tree_search(&root, value);
    if(((**candidate).left == NULL) && ((**candidate).right == NULL))
      *candidate = NULL;
    else if(((**candidate).left != NULL) && ((**candidate).right == NULL))
      *candidate = (**candidate).left;
    else if(((**candidate).left == NULL) && ((**candidate).right != NULL))
      *candidate = (**candidate).right;
    else{
      struct node **maxnodeptr;
      maxnodeptr = tree_maximum(&(**candidate).left);
      (**candidate).key = (**maxnodeptr).key;
      *maxnodeptr = (**maxnodeptr).left;
    }
}

unsigned int tree_size(struct node *element) {
    // TODO: implement
    if(element == NULL)
      return 0;
    else
      return(tree_size((*element).left) + 1 + tree_size((*element).right));
}

/*
 * Fill an array with increasing values.
 *
 * Parameters:
 *      int *t:     pointer to the array
 *      int n:      number of elements in the array
 */
void fill_increasing(int *t, int n) {
    for (int i = 0; i < n; i++) {
        t[i] = i;
    }
}

/*
 * Reorder array elements in a random way.
 *
 * Parameters:
 *      int *t:     pointer to the array
 *      int n:      number of elements in the array
 */
void shuffle(int *t, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % i;
        int temp = t[i];
        t[i] = t[j];
        t[j] = temp;
    }
}

/*
 * Check if tree is a valid BST.
 *
 * Parameters:
 *      struct node *element:   pointer to node to be checked
 *
 * Returns:
 *      bool:                   true if subtree rooted in "element" is a BST
 */
bool is_bst(struct node *element) {
    // empty tree is a valid BST
    if (element == NULL) {
        return true;
    }
    // leaf node is valid
    if (element->left == NULL && element->right == NULL) {
        return true;
    }
    // only right subnode? check it recursively
    if (element->left == NULL && element->right != NULL) {
        return (element->key < element->right->key) && is_bst(element->right);
    }
    // only left subnode? check it recursively
    if (element->left != NULL && element->right == NULL) {
        return (element->key > element->left->key) && is_bst(element->left);
    }
    // both subnodes present? check both recursively
    return (element->key > element->left->key)
        && (element->key < element->right->key)
        && is_bst(element->left)
        && is_bst(element->right);
}

void insert_increasing(int *t, int n) {
    for (int i = 0; i < n; i++) {
        tree_insert(t[i]);
    }
}

void insert_random(int *t, int n) {
    shuffle(t, n);
    for (int i = 0; i < n; i++) {
        tree_insert(t[i]);
    }
}

void tree_insert_biject(int *t, int p, int r)
{
  if(p == r)
  {
    tree_insert(t[p]);
  }
  else if(r-p == 1)
  {
    tree_insert(t[p]);
    tree_insert(t[r]);
  }
  else
  {
    int q = p + (r - p)/2;
    tree_insert(t[q]);
    tree_insert_biject(t, p, q-1);
    tree_insert_biject(t, q+1, r);
  }
}
void insert_binary(int *t, int n){
    // TODO: implement
    tree_insert_biject(t, 0, n-1);
}
char *insert_names[] = { "Increasing", "Random", "Binary" };
void (*insert_functions[])(int*, int) = { insert_increasing, insert_random, insert_binary };

int main(int argc, char **argv) {
    for (unsigned int i = 0; i < sizeof(insert_functions) / sizeof(*insert_functions); i++) {
        void (*insert)(int*, int) = insert_functions[i];

        for (unsigned int j = 0; j < sizeof(ns) / sizeof(*ns); j++) {
            unsigned int n = ns[j];

            // crate an array of increasing integers: 0, 1, 2, ...
            int *t = malloc(n * sizeof(*t));
            fill_increasing(t, n);

            // insert data using one of methods
            clock_t insertion_time = clock();
            insert(t, n);
            insertion_time = clock() - insertion_time;

            assert(tree_size(root) == n);       // after all insertions, tree size must be `n`
            assert(is_bst(root));               // after all insertions, tree must be valid BST

            // reorder array elements before searching
            shuffle(t, n);

            // search for every element in the order present in array `t`
            clock_t search_time = clock();
            for (unsigned int k = 0; k < n; k++) {
                struct node **pnode = tree_search(&root, t[k]);
                struct node *iter = *pnode;
                assert(iter != NULL);           // found element cannot be NULL
                assert(iter->key == t[k]);      // found element must contain the expected value
            }
            search_time = clock() - search_time;

            // reorder array elements before deletion
            shuffle(t, n);

            // delete every element in the order present in array `t`
            for (unsigned int l = 0, m = n; l < n; l++, m--) {
                assert(tree_size(root) == m);   // tree size must be equal to the expected value
                tree_delete(t[l]);
                assert(is_bst(root));           // after deletion, tree must still be valid BST
            }
            assert(tree_size(root) == 0);       // after all deletions, tree has size zero

            free(root);
            free(t);

            printf("%d\t%s\t%f\t%f\n",
                    n,
                    insert_names[i],
                    (double)insertion_time / CLOCKS_PER_SEC,
                    (double)search_time / CLOCKS_PER_SEC);
        }
    }
    return 0;
}
