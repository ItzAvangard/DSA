#include<stdio.h>
#include<conio.h>
#include<iostream>
#include <chrono>
#include <vector>

using namespace std;

int k = 0;

struct FusionTreeNode
{
    int* data;
    FusionTreeNode** child_ptr;
    bool leaf;
    int n;
}*root = NULL, * np = NULL, * x = NULL;

FusionTreeNode* init()
{
    int i;
    np = new FusionTreeNode;
    np->data = new int[5];
    np->child_ptr = new FusionTreeNode * [6];
    np->leaf = true;
    np->n = 0;
    for (i = 0; i < 6; i++)
    {
        np->child_ptr[i] = NULL;
    }
    return np;
}

FusionTreeNode* search(FusionTreeNode* node, int value) {
    if (node == NULL) {
        return NULL;
    }

    int i = 0;
    while (i < node->n && value > node->data[i]) {
        i++;
    }

    if (i < node->n && value == node->data[i]) {
        return node; // Найдено значение в текущем узле
    }

    if (node->leaf) {
        return NULL; // Не найдено значение в листе
    }

    return search(node->child_ptr[i], value); // Рекурсивный поиск в поддереве
}


void traverse(FusionTreeNode* p)
{
    cout << endl;
    int i;
    for (i = 0; i < p->n; i++)
    {
        if (p->leaf == false)
        {
            traverse(p->child_ptr[i]);
        }
        cout << " " << p->data[i];
    }
    if (p->leaf == false)
    {
        traverse(p->child_ptr[i]);
    }
    cout << endl;
}

void sort(int* p, int n)
{
    int i, j, temp;
    for (i = 0; i < n; i++)
    {
        for (j = i; j <= n; j++)
        {
            if (p[i] > p[j])
            {
                temp = p[i];
                p[i] = p[j];
                p[j] = temp;
            }
        }
    }
}

int split_child(FusionTreeNode* x, int i)
{
    int j, mid;
    FusionTreeNode* np1, * np3, * y;
    np3 = init();
    np3->leaf = true;
    if (i == -1)
    {
        mid = x->data[2];
        x->data[2] = 0;
        x->n--;
        np1 = init();
        np1->leaf = false;
        x->leaf = true;
        for (j = 3; j < 5; j++)
        {
            np3->data[j - 3] = x->data[j];
            np3->child_ptr[j - 3] = x->child_ptr[j];
            np3->n++;
            x->data[j] = 0;
            x->n--;
        }
        for (j = 0; j < 6; j++)
        {
            x->child_ptr[j] = NULL;
        }
        np1->data[0] = mid;
        np1->child_ptr[np1->n] = x;
        np1->child_ptr[np1->n + 1] = np3;
        np1->n++;
        root = np1;
    }
    else
    {
        y = x->child_ptr[i];
        mid = y->data[2];
        y->data[2] = 0;
        y->n--;
        for (j = 3; j < 5; j++)
        {
            np3->data[j - 3] = y->data[j];
            np3->n++;
            y->data[j] = 0;
            y->n--;
        }
        x->child_ptr[i + 1] = y;
        x->child_ptr[i + 1] = np3;
    }
    return mid;
}

void insert(int a)
{
    int i, temp;
    x = root;
    if (x == NULL)
    {
        root = init();
        x = root;
    }
    else
    {
        if (x->leaf == true && x->n == 5)
        {
            temp = split_child(x, -1);
            x = root;
            for (i = 0; i < (x->n); i++)
            {
                if ((a > x->data[i]) && (a < x->data[i + 1]))
                {
                    i++;
                    break;
                }
                else if (a < x->data[0])
                {
                    break;
                }
                else
                {
                    continue;
                }
            }
            x = x->child_ptr[i];
        }
        else
        {
            while (x->leaf == false)
            {
                for (i = 0; i < (x->n); i++)
                {
                    if ((a > x->data[i]) && (a < x->data[i + 1]))
                    {
                        i++;
                        break;
                    }
                    else if (a < x->data[0])
                    {
                        break;
                    }
                    else
                    {
                        continue;
                    }
                }
                if ((x->child_ptr[i])->n == 5)
                {
                    temp = split_child(x, i);
                    x->data[x->n] = temp;
                    x->n++;
                    continue;
                }
                else
                {
                    x = x->child_ptr[i];
                }
            }
        }
    }
    x->data[x->n] = a;
    sort(x->data, x->n);
    x->n++;
}

void remove(int key, FusionTreeNode* node) 
{
    if (node == NULL)
        return;
    int i = 0;
    while (i < node->n && key > node->data[i])
        i++;
    if (i < node->n && key == node->data[i])
    {
        for (int j = i; j < node->n - 1; j++)
            node->data[j] = node->data[j + 1];
        node->n--;

        return;
    }
    if (node->leaf) 
    {
        return;
    }
    bool lastChild = (i == node->n);
    remove(key, node->child_ptr[i]);
    if (lastChild && i > 0 && node->child_ptr[i]->n < 2) 
    {
        FusionTreeNode* child = node->child_ptr[i];
        FusionTreeNode* sibling = node->child_ptr[i - 1];
        child->data[0] = node->data[i - 1];
        child->n++;
        node->data[i - 1] = sibling->data[sibling->n - 1];
        sibling->n--;
        child->child_ptr[child->n] = sibling->child_ptr[sibling->n + 1];
        sibling->child_ptr[sibling->n + 1] = NULL;
    }
}


int main()
{
    int i, n, t;
    cout << "Enter the number of elements to be inserted: \n";
    cin >> n;
    for (i = 0; i < n; i++)
    {
        cout << "Enter the binary element: \n";
        cin >> t;
        insert(t);
    }
    cout << "Traversal of constructed fusion tree: \n";
    traverse(root);

    int valueToSearch = 101;
    FusionTreeNode* result = search(root, valueToSearch);
    if (result != NULL) {
        cout << "Value " << valueToSearch << " find\n" << endl;
    }
    else {
        cout << "Value " << valueToSearch << " not find\n" << endl;
    }

    int key_to_remove;
    cout << "Enter the key to remove: \n";
    cin >> key_to_remove;
    remove(key_to_remove, root);
    cout << "Traversal after remove: \n";
    traverse(root);
}