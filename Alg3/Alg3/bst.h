#include <vector>
#include <iostream>
#include <algorithm>
#include <exception>


using namespace std;

template<class Data, class Key>
class TNode
{
public:
    Key key;                                        //ключ объекта
    Data data;                                      //значение объекта в элементе
    int height;                                     //высота поддерева с данным корнем
    TNode<Data, Key>* parent;                       //указатель на родителя
    TNode<Data, Key>* left;                         //указатель на левого сына
    TNode<Data, Key>* right;                        //указатель на правого сына
    TNode(Data d, Key k) {                          //конструктор с параметрами
        key = k;
        data = d;
        height = 1;
        parent = left = right = NULL;
    }

};

template <class Data, class Key> class Tree
{
public:
    typedef TNode<Data, Key> Node;

protected:
    int length;                 //длина дерева
    Node* root;                 //указатель на корень
    bool ins;

public:
    Tree();                                                      //конструктор без параметров
    Tree(const Tree<Data, Key>& anotherTree);                    //конструктор копирования
    ~Tree(void);                                                 //деструктор
    int size();                                                  //опрос размера дерева
    void clear();                                                //очистка дерева
    bool empty();                                                //проверка дерева на пустоту
    Data& read(Key key, int* op = NULL);                         //доступ к данным с заданным ключом
    virtual bool add(Key key, Data obj, int* op = NULL);         //включение данных с заданным ключом
    virtual bool remove(Key key, int* op = NULL);                //удаление данных с заданным ключом
    void print();                                                //вывод структуры дерева на экран
    void walk();                                                 //обход узлов дерева по схеме
    int external_path_length();                                  //определение длины внешнего пути дерева  (рекурсивно)

protected:
    bool _add(Key key, Data obj, Node*& node, int* op = NULL);
    bool _remove(Key key, Node*& node, Node*& parent, int* op = NULL);
    void _copy(Node* r);                                         //вспомогательная функция для копирования дерева
    void _clear(Node* r);                                        //вспомогательная функция для очистки дерева
    virtual void _show(Node* r, int level);                      //вспомогательная функция для вывода структуры
    void _count_level(Node* r, int level, int& sum);             //вспомогательная функция для определения внешнего пути
    Node* _BST_predecessor(Node* x, int* op = NULL);             //поиск предыдущего по ключу узла
    Node* _BST_successor(Node* x, int* op = NULL);               //поиск следующего по ключу узла
    Node* _max(Node* t, int* op = NULL);                         //поиск максимального по ключу узла в поддереве
    Node* _min(Node* t, int* op = NULL);                         //поиск минимального по ключу узла в поддереве
    Node* _parent_right(Node* t, Node* x, int* op = NULL);       //поиск ближайшего правого родителя для заданного узла дерева
    Node* _parent_left(Node* t, Node* x, int* op = NULL);        //поиск ближайшего левого родителя для заданного узла дерева
    Data& _read(Key key, Node*& node, int* op = NULL);           //доступ к данным с заданным ключом в данном поддереве

public:
    class Iterator
    {
    private:
        Tree* ptr;    //указатель на объект коллекции
        Node* cur;    //указатель на текущий элемент коллекции
    public:
        //конструктор
        Iterator(Tree<Data, Key>& tree) {
            ptr = &tree;
            cur = NULL;
        }

        //установка на первый
        void begin() {
            cur = ptr->root;
            if (cur != NULL)
                while (cur->left != NULL)
                    cur = cur->left;
        }

        //установка на последний
        void end() {
            cur = ptr->root;
            if (cur != NULL)
                while (cur->right != NULL)
                    cur = cur->right;
        }

        //установка на следующий
        void next() {
            cur = ptr->_BST_successor(cur);
        }

        //установка на предыдущий
        void prev() {
            cur = ptr->_BST_predecessor(cur);
        }

        //проверка состояния итератора
        bool is_off() const {
            return (cur == NULL);
        }

        //доспуп к данным текущего элемента
        Data& operator*() {
            if (cur != NULL)
                return cur->data;
            else
                throw runtime_error("Итератор за пределами дерева");
        }
    };

    friend class Iterator;
};

//конструктор без параметров
template<class Data, class Key>
Tree<Data, Key>::Tree(void)
{
    length = 0;
    root = NULL; //в начале дерево пусто
}

//конструктор копирования
template<class Data, class Key>
Tree<Data, Key>::Tree(const Tree<Data, Key>& anotherTree)
{
    root = NULL;
    length = 0;
    _copy(anotherTree.root);
}

//копирование дерева
template<class Data, class Key>
void Tree<Data, Key>::_copy(Node* r)
{
    if (r == NULL)
        return;
    add(r->key, r->data);
    _copy(r->left);
    _copy(r->right);
}

template<class Data, class Key>
bool Tree<Data, Key>::_add(Key key, Data obj, Node*& node, int* op)
{
	
    if (node == NULL) {
        node = new Node(obj, key);
        length++;
        return true;
    }
    if (op)
        ++*op;
    if (key < node->key) {
        bool result = _add(key, obj, node->left, op);
        node->left->parent = node;
        node->height = std::max(node->height, node->left->height + 1);
        return result;
    }
    if (key > node->key) {
        bool result = _add(key, obj, node->right, op);
        node->right->parent = node;
        node->height = std::max(node->height, node->right->height + 1);
        return result;
    }
    // key == node->key
    return false;
}

template<class Data, class Key>
Data& Tree<Data, Key>::_read(Key key, Node*& node, int* op)
{
    if (node == nullptr) {
        throw runtime_error("Узел с таким ключом отсутствует");
    }
    if (op)
        ++*op;
    if (key == node->key) {
        return node->data;
    }
    if (key > node->key) {
        return _read(key, node->right, op);
    }
    // key < node->key
    return _read(key, node->left, op);
}

//деструктор
template<class Data, class Key>
Tree<Data, Key>::~Tree(void)
{
    clear();
}

//опрос размера дерева
template<class Data, class Key>
int Tree<Data, Key>::size()
{
    return length;
}

//очистка дерева
template<class Data, class Key>
void Tree<Data, Key>::clear()
{
    _clear(root);
    ///looked = length;
    root = NULL;
    length = 0;
}

//очистка по обходу LtR дерева
template<class Data, class Key>
void Tree<Data, Key>::_clear(Node* r)
{
    if (r == NULL)
        return;
    _clear(r->left);
    Node* rtree = r->right;
    delete r;
    _clear(rtree);
}

//проверка дерева на пустоту
template<class Data, class Key>
bool Tree<Data, Key>::empty()
{
    return (length == 0 && root == NULL);
}

//доступ к данным с заданным ключом
template<class Data, class Key>
Data& Tree<Data, Key>::read(Key key, int* op)
{
    if (op)
        *op = 0;
    return _read(key, root, op);
}

//включение данных с заданным ключом
template<class Data, class Key>
bool Tree<Data, Key>::add(Key key, Data obj, int* op)
{
    if (op)
        *op = 0;
    return _add(key, obj, root, op);
}

//удаление данных с заданным ключом
template<class Data, class Key>
bool Tree<Data, Key>::remove(Key key, int* op)
{
    Node* parent;
    if (op)
        *op = 0;
    return _remove(key, root, parent, op);
}

template<class Data, class Key>
bool Tree<Data, Key>::_remove(Key key, Node*& node, Node*& parent, int* op)
{
    if (node == NULL)
        return false;
    if (op)
        ++*op;
    if (key < node->key)
        return _remove(key, node->left, parent, op);
    if (key > node->key)
        return _remove(key, node->right, parent, op);
    if (key == node->key && node->left == NULL && node->right == NULL) {
        length--;
        parent = node->parent;
        node = NULL;
        return true;
    }
    if (key == node->key && node->right == NULL) {
        length--;
        parent = node->parent;
        node->left->parent = node->parent;
        node = node->left;
        return true;
    }
    if (key == node->key && node->left == NULL) {
        length--;
        parent = node->parent;
        node->right->parent = node->parent;
        node = node->right;
        return true;
    }
    // key == node->key
    if (node->left->height < node->right->height) {
        node->key = _BST_successor(node)->key;
        node->data = _BST_successor(node)->data;
        return _remove(node->key, node->right, parent, op);
    }
    node->key = _BST_predecessor(node)->key;
    node->data = _BST_predecessor(node)->data;
    return _remove(node->key, node->left, parent, op);

}

//обход структуры по LtR
template <class Data, class Key>
void Tree<Data, Key>::walk()
{
    if (root == NULL)
        throw runtime_error("Нет данных");

    Node* it = root;
    std::vector<Node*> up(length);
    int top = 0;

    while (it != NULL) {
        while (it != NULL) {
            if (it->right != NULL)
                up[top++] = it->right;

            up[top++] = it;
            it = it->left;
        }

        it = up[--top];

        while (top != 0 && it->right == NULL) {
            cout << it->key << "->";
            it = up[--top];
        }

        cout << it->key << "->";

        if (top == 0)
            break;

        it = up[--top];
    }
    cout << endl;
}

//вспомогательная функция для вывода структуры
template <class Data, class Key>
void Tree<Data, Key>::_show(typename Tree<Data, Key>::Node* r, int level)
{
    if (r == NULL)
        return;
    _show(r->right, level + 1);
    for (int i = 0; i <= 2 * level; i++)
        cout << " ";
    cout << r->key << endl;
    _show(r->left, level + 1);
}

//вывод структуры дерева на экран
template<class Data, class Key>
void Tree<Data, Key>::print()
{
    if (root == NULL) {
        return;
    }
    _show(root, 0);
}


//определение длины внешнего пути дерева 
template<class Data, class Key>
int Tree<Data, Key>::external_path_length()
{
    if (root == NULL)
        return -1;
    int sum = 0;
    _count_level(root, 1, sum);
    return sum;
}

//вспомогательная функция для определения внешнего пути
template <class Data, class Key>
void Tree<Data, Key>::_count_level(Node* r, int level, int& sum)
{
    if (r == NULL)
        return;
    _count_level(r->left, level + 1, sum);
    _count_level(r->right, level + 1, sum);
    if (r->right == NULL || r->left == NULL)
        sum += level;
}

template <class Data, class Key> 
typename Tree<Data, Key>::Node* Tree<Data, Key>::_BST_successor(typename Tree<Data, Key>::Node* x, int* op)
{
    if (x == NULL)
        return NULL;
    if (op)
        ++*op;
    if (x->right != NULL)
    {
        Node* t = x->right;
        while (t->left != NULL) {
            t = t->left;
            if (op)
                ++*op;
        }

        return t;
    }
    else
        return _parent_left(root, x, op);
}

template <class Data, class Key> 
typename Tree<Data, Key>::Node* Tree<Data, Key>::_parent_left(typename Tree<Data, Key>::Node* t, typename Tree<Data, Key>::Node* x, int* op) {
    if (t == x)
        return NULL;
    if (op)
        ++*op;
    if (x->key < t->key) {
        Node* rp = _parent_left(t->left, x, op);
        if (rp != NULL)
            return rp;
        else
            return t;
    }
    else
        return _parent_left(t->right, x, op);
}

template <class Data, class Key> typename Tree<Data, Key>::Node* Tree<Data, Key>::_BST_predecessor(typename Tree<Data, Key>::Node* x, int* op) {
    if (x == NULL)
        return NULL;
    if (op)
        ++*op;
    if (x->left != NULL)
    {
        Node* t = x->left;
        while (t->right != NULL) {
            t = t->right;
            if (op)
                ++*op;
        }

        return t;
    }
    else
        return _parent_right(root, x, op);
}

template <class Data, class Key> typename Tree<Data, Key>::Node* Tree<Data, Key>::_parent_right(typename Tree<Data, Key>::Node* t, typename Tree<Data, Key>::Node* x, int* op) {
    if (t == x)
        return NULL;
    if (x->key > t->key) {
        Node* rp = _parent_right(t->right, x, op);
        if (rp != NULL)
            return rp;
        else
            return t;
    }
    else
        return _parent_right(t->left, x, op);
}
