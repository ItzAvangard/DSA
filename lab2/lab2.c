#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct trie
{
    char *value;
    char ch;
    struct trie *sibling;
    struct trie *child;
} trie;

trie *trie_create()
{
    trie *node;

    if ((node = (trie *)malloc(sizeof(*node))) == NULL)
        return NULL;
    node->ch = '\0';
    node->value = NULL;
    node->sibling = NULL;
    node->child = NULL;

    return node;
}

char *trie_lookup(struct trie *root, char *key)
{
    struct trie *node, *list;

    for (list = root; *key != '\0'; key++)
    {
        for (node = list; node != NULL; node = node->sibling)
        {
            if (node->ch == *key)
                break;
        }
        if (node != NULL)
            list = node->child;
        else
            return NULL;
    }
    return node->value; /* Node must be a leaf */
}

struct trie *trie_insert(struct trie *root, char *key, char *value)
{
    struct trie *node, *parent, *list;

    parent = NULL;
    list = root;
    for (; *key != '\0'; key++)
    {
        /* Lookup sibling node */
        for (node = list; node != NULL; node = node->sibling)
        {
            if (node->ch == *key)
                break;
        }
        if (node == NULL)
        { /* Node not found. Add new node */
            node = trie_create();
            node->ch = *key;
            node->sibling = list;
            if (parent != NULL)
                parent->child = node;
            else
                root = node;
            list = NULL;
        }
        else
        { /* Node found. Move to next level */
            list = node->child;
        }
        parent = node;
    }
    /* Update value in leaf */
    if (node->value != NULL)
        free(node->value);
    node->value = strdup(value);
    return root;
}

struct trie *
trie_delete_dfs(struct trie *root, struct trie *parent, char *key, int *found)
{
    struct trie *node, *prev = NULL;

    *found = (*key == '\0' && root == NULL) ? 1 : 0;
    if (root == NULL || *key == '\0')
        return root;
    for (node = root; node != NULL; node = node->sibling)
    {
        if (node->ch == *key)
            break;
        prev = node;
    }
    if (node == NULL)
        return root;
    trie_delete_dfs(node->child, node, key + 1, found);
    if (*found > 0 && node->child == NULL)
    {
        /* Delete node */
        if (prev != NULL)
            prev->sibling = node->sibling;
        else
        {
            if (parent != NULL)
                parent->child = node->sibling;
            else
                root = node->sibling;
        }
        free(node->value);
        free(node);
    }
    return root;
}

struct trie *trie_delete(struct trie *root, char *key)
{
    int found;

    return trie_delete_dfs(root, NULL, key, &found);
}

void print_trie(trie *node, int level)
{
    if (node == NULL)
        return;

    for (int i = 0; i < level; i++)
        printf("  "); // Adjust the indentation for better visualization

    printf("%c", node->ch);

    if (node->value != NULL)
        printf(" (%s)", node->value);

    printf("\n");

    print_trie(node->child, level + 1);
    print_trie(node->sibling, level);
}

int main()
{
    trie *tree = trie_create();
    tree = trie_insert(tree, "geek", "a");
    tree = trie_insert(tree, "genius", "b");
    tree = trie_insert(tree, "genetic", "c");
    tree = trie_insert(tree, "gene", "d");
    char *s = trie_lookup(tree, "gene");
    printf("%s\n", s);
    printf("Trie Structure:\n");
    print_trie(tree, 0);

    tree = trie_delete(tree, "geek");

    printf("\nTrie Structure after deletion:\n");
    print_trie(tree, 0);

    return 0;
}