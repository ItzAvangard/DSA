#include <stdio.h>
#include <stdlib.h>

#include "trie.h"

#define WORDS_ARR_SIZE 4

int main(void) {
    Trie* t = NULL;

    char* words_to_insert[WORDS_ARR_SIZE] = {"geek", "genius", "gene", "genetic"};

    for (int i = 0; i < WORDS_ARR_SIZE; i++) {
        t = trie_insert(t, words_to_insert[i], i + 1);
    }

    printf("\n");
    trie_print(t, 0);

    char* words_to_lookup[WORDS_ARR_SIZE] = {"geek", "genius", "gene", "genetic"};

    Trie* fn = NULL;

    printf("\n");
    for (int i = 0; i < WORDS_ARR_SIZE; i++) {
        fn = trie_lookup(t, words_to_lookup[i]);
        if (fn)
            printf("Word \"%s\" $(%d)\n", words_to_lookup[i], fn->value);
        else
            printf("Word \"%s\" not found\n", words_to_lookup[i]);
    }

    printf("\nTry to delete \"%s\"\n", words_to_insert[WORDS_ARR_SIZE - 1]);
    t = trie_delete(t, words_to_insert[WORDS_ARR_SIZE - 1]);

    printf("\nThen update \"zetta\" value\n");
    t = trie_insert(t, "zetta", 139);
    trie_print(t, 0);

    printf("\n");
    for (int i = 0; i < WORDS_ARR_SIZE; i++) {
        fn = trie_lookup(t, words_to_lookup[i]);
        if (fn)
            printf("Word \"%s\" $(%d)\n", words_to_lookup[i], fn->value);
        else
            printf("Word \"%s\" not found\n", words_to_lookup[i]);
    }

    printf("\nMin key: ");
    trie_print_min(t);

    printf("Max key: ");
    trie_print_max(t);

    return 0;
}