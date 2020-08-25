#include "bst.h"

#include <vector>
#include <iostream>
#include <algorithm>
#include <exception>


using namespace std;

template <class Data, class Key> class AVLTree: public Tree<Data, Key> {

public:
    typedef TNode<Data, Key> Node;

    AVLTree();                                              //конструктор без параметров
    ~AVLTree(void);                                         //деструктор

    virtual bool add(Key key, Data obj, int* op = NULL);    //включение данных с заданным ключом
    virtual bool remove(Key key, int* op = NULL);           //удаление данных с заданным ключом
    bool check();                                           //проверка структуры узлов на корректность


private:
    virtual void _show(Node* r, int level);                    //вспомогательная функция для вывода структуры

    bool _check(Node* root);                                   //проверка на внутреннюю целостность дерева
    int _bfactor(Node* node);                                  //разность высот левого и правого поддерева

    Node* L(Node* a);                                          //малый левый поворот вокруг a
    Node* LL(Node* a);                                         //большой левый поворот вокруг а
    Node* R(Node* a);                                          //малый правый поворот вокруг а
    Node* RR(Node* a);                                         //большой правый поворот вокруг а
    void _fix_son(Node* parent, Node* old_son, Node* new_son); //поправить родителю old_son соответствующего сына на new_son
    void _fix_height(Node* node);                              //в предположении, что высоты всех поддеревьев node верны, выставить высоту node

    Node* _just_add(Key key, Data obj, int* op = NULL);
};

//конструктор без параметров
template<class Data, class Key>
AVLTree<Data, Key>::AVLTree(void)
{
}

//проверка корректности дерева
template<class Data, class Key>
bool AVLTree<Data, Key>::check()
{
    if (!this->root)
        return true;
    return _check(this->root);
}

//деструктор
template<class Data, class Key>
AVLTree<Data, Key>::~AVLTree(void)
{
}

//разность высот левого и правого поддерева
template<class Data, class Key>
int AVLTree<Data, Key>::_bfactor(Node* node)
{
    int lheight = (node->left) ? node->left->height : 0;
    int rheight = (node->right) ? node->right->height : 0;
    return lheight - rheight;
}

//вспомогательная функция для вывода структуры
template <class Data, class Key>
void AVLTree<Data, Key>::_show(Node* r, int level)
{
    if (r == NULL)
        return;
    _show(r->right, level + 1);
    for (int i = 0; i <= 2 * level; i++)
        cout << " ";
    cout << r->key << "," <<_bfactor(r) << endl;
    _show(r->left, level + 1);
}

// проверка на корректность структуры дерева: ссылки на родителей и детей взаимно согласованы,
// а высоты родителей согласованы с высотами детей.
template<class Data, class Key>
bool AVLTree<Data, Key>::_check(Node* node)
{
    int trueHeight = 1;
    if (node->left) {
        if (node->left->parent != node)
            return false;
        if (!_check(node->left))
            return false;
        trueHeight = 1 + node->left->height;
    }
    if (node->right) {
        if (node->right->parent != node)
            return false;
        if (!_check(node->right))
            return false;
        trueHeight = std::max(trueHeight, 1 + node->right->height);
    }
    if (node->height != trueHeight)
        return false;
    return true;
}

// вычислить высоту node в предположении, что высоты детей верны
template<class Data, class Key>
void AVLTree<Data, Key>::_fix_height(Node* node)
{
    if (!node)
        return;

    int lheight = (node->left) ? (node->left->height) : 0;
    int rheight = (node->right) ? (node->right->height) : 0;
    node->height = std::max(lheight, rheight) + 1;
};

// проставить родителю old_son нового сына вместо него
template<class Data, class Key>
void AVLTree<Data, Key>::_fix_son(TNode<Data, Key>* parent, TNode<Data, Key>* old_son, TNode<Data, Key>* new_son)
{
    if (!parent) {
        this->root = new_son;
        return;
    }

    if (parent->left == old_son)
        parent->left = new_son;
    else if (parent->right == old_son)
        parent->right = new_son;
    else {
        throw runtime_error("Оказалось, что родитель не родитель."); // this should not happen
    }
};

// малый правый поворот вокруг a
// (с корректировкой высот в поддереве)
template<class Data, class Key>
TNode<Data, Key>* AVLTree<Data, Key>::R(TNode<Data, Key>* a)
{
    Node* b = a->left;
    Node* c = b->right;

    b->parent = a->parent;
    a->parent = b;
    a->left = c;
    b->right = a;
    if (c)
        c->parent = a;
    _fix_son(b->parent, a, b);
    _fix_height(a);
    _fix_height(b);

    return b;
};

// малый левый поворот вокруг а
template<class Data, class Key>
TNode<Data, Key>* AVLTree<Data, Key>::L(TNode<Data, Key>* a)
{
    Node* b = a->right;
    Node* c = b->left;

    b->parent = a->parent;
    a->parent = b;
    a->right = c;
    b->left = a;
    if (c)
        c->parent = a;
    _fix_son(b->parent, a, b);
    _fix_height(a);
    _fix_height(b);

    return b;
};

// большой левый поворот вокруг а
template<class Data, class Key>
TNode<Data, Key>* AVLTree<Data, Key>::LL(TNode<Data, Key>* a)
{
    // точно ненулевые
    TNode<Data, Key>* b = a->right;
    TNode<Data, Key>* c = b->left;
    // могут быть нулевыми
    TNode<Data, Key>* m = c->left;
    TNode<Data, Key>* n = c->right;

    c->parent = a->parent;
    c->left = a;
    c->right = b;
    a->parent = c;
    a->right = m;
    b->parent = c;
    b->left = n;
    if (m)
        m->parent = a;
    if (n)
        n->parent = b;
    _fix_son(c->parent, a, c);
    _fix_height(a);
    _fix_height(b);
    _fix_height(c);

    return c;
}

// большой правый поворот вокруг а
template<class Data, class Key>
TNode<Data, Key>* AVLTree<Data, Key>::RR(TNode<Data, Key>* a)
{
    // точно ненулевые
    TNode<Data, Key>* b = a->left;
    TNode<Data, Key>* c = b->right;
    // могут быть нулевыми
    TNode<Data, Key>* m = c->right;
    TNode<Data, Key>* n = c->left;

    c->parent = a->parent;
    c->right = a;
    c->left = b;
    a->parent = c;
    a->left = m;
    b->parent = c;
    b->right = n;
    if (m)
        m->parent = a;
    if (n)
        n->parent = b;
    _fix_son(c->parent, a, c);
    _fix_height(a);
    _fix_height(b);
    _fix_height(c);

    return c;
}

// Добавить новый узел в подходящее место дерева поиска и возвратить соответствующий Node.
template<class Data, class Key>
TNode<Data, Key>* AVLTree<Data, Key>::_just_add(Key key, Data obj, int* op)
{
    if (!this->root) {
        this->root = new Node(obj, key);
        ++(this->length);
        return this->root;
    }

    Node* node = this->root;
    while (1) {

        if (op)
            ++*op;

        if (key < node->key) {
            if (!node->left) {
                Node* target = new Node(obj, key);
                target->parent = node;
                node->left = target;
                ++(this->length);
                return target;
            }
            node = node->left;
            continue;
        }

        if (key > node->key) {
            if (!node->right) {
                Node* target = new Node(obj, key);
                target->parent = node;
                node->right = target;
                ++(this->length);
                return target;
            }
            node = node->right;
            continue;
        }

        // key == node->key
        return NULL;
    }

    throw runtime_error("Что-то пошло не так. Эта ошибка не должна произойти.");
}

// добавление элемента в дерево с последующей нерекурсивной балансировкой от места добавления вверх
template<class Data, class Key>
bool AVLTree<Data, Key>::add(Key key, Data data, int* op)
{
    if (op)
        *op = 0;
    TNode<Data, Key>* new_node = _just_add(key, data, op);
    if (!new_node) // не добавлен
        return false;

    // Инвариант цикла. Перед исполнением цикла вершина a -- неизмененная вершина поддерева, в котором появился новый элемент.
    // Поддеревья уже AVL с верно проставленной высотой.
    // Потом перестройки внутри поддерева a, чтобы оно стало AVL-поддеревом.
    // Если после перестроек высота a не изменилась по сравнению с "до добавления", то можно останавливаться.
    // Если a --- корень, то можно останавливаться. Это будет условие while(a)
    // В противном случае изучить родителя

    TNode<Data, Key>* a = new_node->parent;
    while (a) {
        if (op)
            ++*op;

        int old_height = a->height;

        _fix_height(a);

        if (_bfactor(a) == 2) {
            int l_height = (!a->right) ? 0 : a->right->height;
            int c_height = (!a->left->right) ? 0 : a->left->right->height;
            if (c_height <= l_height) {
                a = R(a);
            } else {
                a = RR(a);
            }
        } else if (_bfactor(a) == -2) {
            int l_height = (!a->left) ? 0 : a->left->height;
            int c_height = (!a->right->left) ? 0 : a->right->left->height;
            if (c_height <= l_height) {
                a = L(a);
            } else {
                a = LL(a);
            }
        }

        if (a->height == old_height)
            break;
        a = a->parent;
    }

    return true;
}

// Перебалансируем так же, как и при добавлении: идем вверх от родителя удаленной вершины,
// пока не встретим поддерево, в котором после перебалансировки не изменилась высота.
template<class Data, class Key>
bool AVLTree<Data, Key>::remove(Key key, int* op)
{
    if (op)
        *op = 0;

    TNode<Data, Key>* a;
    bool removed = this->_remove(key, this->root, a, op);
    if (!removed) // не удален
        return false;

    while (a) {
        if (op)
            ++*op;

        int old_height = a->height;

        _fix_height(a);

        if (_bfactor(a) == 2) {
            int l_height = (!a->right) ? 0 : a->right->height;
            int c_height = (!a->left->right) ? 0 : a->left->right->height;
            if (c_height <= l_height) {
                a = R(a);
            } else {
                a = RR(a);
            }
        } else if (_bfactor(a) == -2) {
            int l_height = (!a->left) ? 0 : a->left->height;
            int c_height = (!a->right->left) ? 0 : a->right->left->height;
            if (c_height <= l_height) {
                a = L(a);
            } else {
                a = LL(a);
            }
        }

        if (a->height == old_height)
            break;
        a = a->parent;
    }

    return true;
}
