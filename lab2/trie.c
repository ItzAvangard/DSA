#include "trie.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

Trie* trie_create() {
    Trie* node = (Trie*) malloc(sizeof(Trie));

    if (!node) return NULL;

    // Если значение в ноде равно INT_MIN, то это не конец ключа
    node->value = INT_MIN;

    node->ch = '\0';

    node->child = NULL;    // Левый дочерний
    node->sibling = NULL;  // Правый сестринский

    return node;
}

Trie* trie_insert(Trie* root, char* key, int value) {
    Trie* node = NULL;    // Для прохода по сестринским
    Trie* parent = NULL;  // Родитель сестринских
    Trie* list = root;    // Для прохода по потомкам

    // Просмотр всех символов ключа
    while (*key) {
        node = list;

        // Проход по сестринским узлам
        while (node) {
            if (node->ch == *key) break;

            /* Если ключ в текущем сестринском не совпал с символом в ключе,
             * перебираем дальше по сестринским. */
            node = node->sibling;
        }

        if (!node) {
            /* Если дошли до крайнего сестринского и его символ не совпал,
             * то полностью ключа в словаре нет, добавляем. */
            node = trie_create();
            if (!node) return root; // Allocation check

            node->ch = *key;

#ifdef ORDERED_INSERT
            /* Упорядоченное дерево.
             * Вставляем созданный узел между сестринскими меньшим и бОльшим.
             * Например, символы в сестринских узлах: A -> H -> W -> Y -> Z, нужно вставить X
             * X > A, X > H, X > W, но X < Y, тогда после вставки: A -> H -> W -> X -> Y -> Z */
            if (parent) {
                /* Если символ в крайнем левом сестринском узле больше,
                 * чем добавляемый символ, то созданный узел делаем потомком родителя. */
                if (!list || list->ch > node->ch) {
                    node->sibling = list;
                    parent->child = node;
                } else {
                    /* Проходим по всем сестринским, начиная с левого крайнего,
                     * пока не найдем такой, в котором символ будет больше */
                    while (list->sibling && (list->sibling->ch < node->ch)) {
                        list = list->sibling;
                    }

                    // Вставка нового узла между сестринскими меньшим и бОльшим
                    node->sibling = list->sibling;
                    list->sibling = node;
                }
            } else if (root) {
                /* Если символ в крайнем левом сестринском узле больше,
                 * чем добавляемый символ, то созданный узел помещаем на его место. */
                if (root->ch > node->ch) {
                    node->sibling = root;
                    root = node;
                } else {
                    /* Проходим по всем сестринским от корневого,
                     * пока не найдем такой узел, в котором символ будет больше */
                    while (list->sibling && (list->sibling->ch < node->ch)) {
                        list = list->sibling;
                    }

                    // Вставка нового узла между сестринскими меньшим и бОльшим
                    node->sibling = list->sibling;
                    list->sibling = node;
                }
            } else {
                // Если нет корневого узла, то делаем им созданный узел
                root = node;
            }
#else
            /* Неупорядоченное дерево.
             * Текущий узел делаем сестринским для созданного.
             * Если у этих сестринских есть родитель, то делаем созданный узел его потомком.
             * Если родителя не было, значит имеем дело с корневым узлом. */
            node->sibling = list;
            if (parent)
                parent->child = node;
            else
                root = node;
#endif
        }

        /* Если у текущего сестринского есть потомок, то спускаемся к нему.
         * Если на каком-то этапе сработало условие if (!node), то
         * дальше всегда будут создаваться новые узлы, пока не закончится ключ */
        list = node->child;

        // Обновляем родительский при переходе к потомку
        parent = node;

        key++;
    }

    /* Проверка node на NULL (clang-analyzer-core.NullDereference bypass).
     * После добавления всех символов ключа записываем в листовой узел значение. */
    if (node) node->value = value;

    return root;
}

Trie* trie_lookup(Trie* root, char* key) {
    Trie* list = root;  // Для прохода по потомкам
    Trie* node = NULL;  // Для прохода по сестринским

    // Просмотр всех символов ключа
    while (*key) {
        node = list;

        // Проход по сестринским узлам
        while (node) {
            if (node->ch == *key) break;

            /* Если ключ в текущем сестринском не совпал с символом в ключе,
             * перебираем дальше по сестринским. */
            node = node->sibling;
        }

        if (node) {
            /* Символ в текущем сестринском узле совпал с символом в ключе,
             * спускаемся к потомку. */
            list = node->child;
        } else {
            /* Если дошли до крайнего сестринского и его символ не совпал,
             * то искомого ключа нет в словаре. */
            return NULL;
        }

        key++;
    }

    /* Проверка node на NULL (clang-analyzer-core.NullDereference bypass).
     * Если после перебора всех символов ключа остановились на узле,
     * у которого не задано значение, то ключа в словаре нет. */
    if (node && node->value == INT_MIN) return NULL;

    return node;
}

Trie* trie_delete_dfs(Trie* root, Trie* parent, char* key, int* found) {
    /* Если текущий корень пустой И достигнут конец ключа,
     * то мы нашли ключ в дереве. */
    *found = (!root && *key == '\0') ? 1 : 0;

    /* Если текущий корень пустой ИЛИ достигнут конец ключа,
     * то просто возвращаемся к предыдыщей рекурсивной функции. */
    if (!root || *key == '\0') return root;

    // Для прохода по сестринским
    Trie* node = root;
    Trie* prev = NULL;

    // Проход по сестринским узлам
    while (node) {
        if (node->ch == *key) break;

        prev = node;  // Предыдущий сестринский

        /* Если ключ в текущем сестринском не совпал с символом в ключе,
         * перебираем дальше по сестринским. */
        node = node->sibling;
    }

    /* Если дошли до крайнего сестринского и его символ не совпал,
     * то удаляемого ключа нет в словаре. */
    if (!node) return root;

    // Пытаемся удалить остальную последовательность символов ключа
    trie_delete_dfs(node->child, node, key + 1, found);

    // Если удаляемый ключ найден в дереве
    if (*found && !node->child) {
        // Удаляем его
        if (prev) {
            /* Связываем два сестринских узла, между которыми
             * находится удаляемый узел. */
            prev->sibling = node->sibling;
        } else if (parent)
            /* Если узел самый первый из сестринских, то он является
             * потомком родителя. Нужно сделать следующий
             * сестринский узел потомком родителя. */
            parent->child = node->sibling;
        else {
            /* Если нет предыдущего и нет родителя, то находимся в корне,
             * значит нужно сделать корнем следующий сестринский. */
            root = node->sibling;
        }

        // Освобождаем память из-под удаляемого узла
        free(node);
    }

    /* clang-analyzer кидает варнинг об использовании уже освобожденной памяти,
     * но его можно игнорировать, т.к. нет такой ситуации, где в конце будет
     * node == root. */
    return root;
}

Trie* trie_delete(Trie* root, char* key) {
    int found = 0;

    // У корневого узла нет родителя
    return trie_delete_dfs(root, NULL, key, &found);
}

void trie_print(Trie* root, int level) {
    Trie* node = root;

    // Проход по сестринским узлам
    while (node) {
        // Чтобы визуально разделять сестринские узлы
        for (int i = 0; i < level; i++) printf("_");

        // Если значение в ноде не равно INT_MIN, то это конец ключа
        if (node->value != INT_MIN)
            printf("%c $(%d)\n", node->ch, node->value);
        else
            printf("%c \n", node->ch);

        // Если у узла есть потомок, спускаемся к нему
        if (node->child) trie_print(node->child, level + 1);

        node = node->sibling;
    }
}

void trie_print_min(Trie* root) {
    Trie* node = root;

    // Проходим по всем потомкам и выводим символы в них
    while (node->child) {
        printf("%c", node->ch);
        node = node->child;
    }

    /* Если потомки закончились, значит мы дошли до конца минимального ключа.
     * Выводим его последний символ и значение */
    if (node) printf("%c $(%d)\n", node->ch, node->value);
}

void trie_print_max(Trie* root) {
    Trie* node = root;

    // Доходим до крайнего правого сестринского узла
    while (node && node->sibling) node = node->sibling;

    if (node) {
        // Выводим символ в крайнем правом сестринском узле
        printf("%c", node->ch);

        /* Если в текущем узле конец ключа, то выводим значение и завершаем работу.
         * Иначе рекурсивно переходим к потомку */
        if (node->value != INT_MIN)
            printf(" $(%d)\n", node->value);
        else
            trie_print_max(node->child);
    }
}