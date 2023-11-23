#ifndef TRIE_H
#define TRIE_H

#define ORDERED_INSERT

typedef struct trie {
    struct trie* sibling;
    struct trie* child;

    int value;
    char ch;
} Trie;

Trie* trie_create();
Trie* trie_insert(Trie* root, char* key, int value);
Trie* trie_lookup(Trie* root, char* key);
Trie* trie_delete(Trie* root, char* key);
void trie_print(Trie* root, int level);

void trie_print_min(Trie* root);
void trie_print_max(Trie* root);

#endif  // TRIE_H