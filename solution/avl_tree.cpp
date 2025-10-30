#include "../headers/avl_tree.h"
#include <algorithm>
#include <cmath>
using namespace std;

template<typename K, typename V>
AVLTree<K, V>::AVLTree() : BST<K, V>() {
}

template<typename K, typename V>
AVLTree<K, V>::AVLTree(function<bool(const K&, const K&)> comp) : BST<K, V>(comp) {
}

template<typename K, typename V>
bool AVLTree<K, V>::insert(const K& key, const V& value) 
{
    size_t count = this->nodeCount;
    this->root = insertAVL(this->root, key, value);
    if (this->nodeCount != count) return true;
    else return false;
}

template<typename K, typename V>
shared_ptr<typename AVLTree<K, V>::BSTNode> AVLTree<K, V>::insertAVL(shared_ptr<BSTNode> node, const K& key, const V& value) 
{
    if (!node)
    {
        auto new_Node = make_shared<BSTNode>(key, value);
        new_Node->height = 1;
        this->nodeCount++;
        return new_Node;
    }
    if (this->comparator(key, node->key))
    {
        node->left = insertAVL(node->left, key, value);
    }
    else if (this->comparator(node->key, key))
    {
        node->right = insertAVL(node->right, key, value);
    }
    this->updateHeight(node);
    int balance = getBalanceFactor(node);
    if (balance > 1)
    {
        if (this->comparator(key,node->left->key))
        {
            return rotateRight(node);
        }
        else
        {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
    }
    if (balance < -1)
    {
        if (this->comparator(node->right->key, key))
        {
            return rotateLeft(node);
        }
        else
        {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }
    }
    return node;
}

template<typename K, typename V>
bool AVLTree<K, V>::remove(const K& key) 
{
    size_t count = this->nodeCount;
    this->root = removeAVL(this->root, key);
    if (this->nodeCount != count) return true;
    else return false;
}

template<typename K, typename V>
shared_ptr<typename AVLTree<K, V>::BSTNode> AVLTree<K, V>::removeAVL(shared_ptr<BSTNode> node, const K& key) 
{
    if (!node ) return node;
    if (this->comparator(node->key, key)) node->right = removeAVL(node->right, key);
    else if (this->comparator(key, node->key)) node->left = removeAVL(node->left, key);
    else
    {
        if (!node->left && !node->right) //! No Children
        {
            this->nodeCount--;
            return nullptr;
        }
        if (!node->left) //! Only 1 child (Right)
        {
            auto temp = node->right; 
            this->nodeCount--;
            return temp;
        }
        if (!node->right) //! Only 1 child (Left)
        {
            auto temp = node->left;
            this->nodeCount--;
            return temp;
        }
        else //! 2 children
        {
            auto temp = this->findMinHelper(node->right); //!Gets the smallest value from right subtree
            node->key = temp->key;
            node->value = temp->value;
            node->right = removeAVL(node->right, temp->key);
        }
    }
    this->updateHeight(node);
    int balance = getBalanceFactor(node);
    if (balance > 1)
    {
        if (getBalanceFactor(node->left) >=0)
        {
            return rotateRight(node);
        }
        else
        {
            return rotateLeftRight(node);
        }
    }
    if (balance < -1)
    {
        if (getBalanceFactor(node->right) <= 0)
        {
            return rotateLeft(node);
        }
        else
        {
            return rotateRightLeft(node);
        }
    }
    return node;
}

template<typename K, typename V>
shared_ptr<typename AVLTree<K, V>::BSTNode> AVLTree<K, V>::rotateLeft(shared_ptr<BSTNode> node) 
{
    auto x = node->right;
    auto T2 = x->left;

    x->left = node;
    node->right = T2;

    this->updateHeight(x);
    this->updateHeight(node);

    return x;
}

template<typename K, typename V>
shared_ptr<typename AVLTree<K, V>::BSTNode> AVLTree<K, V>::rotateRight(shared_ptr<BSTNode> node) 
{
    auto x = node->left;
    auto T2 = x->right;

    x->right = node;
    node->left = T2;

    this->updateHeight(x);
    this->updateHeight(node);

    return x;
}

template<typename K, typename V>
shared_ptr<typename AVLTree<K, V>::BSTNode> AVLTree<K, V>::rotateLeftRight(shared_ptr<BSTNode> node) 
{
    node->left = rotateLeft(node->left);
    return rotateRight(node);
}

template<typename K, typename V>
shared_ptr<typename AVLTree<K, V>::BSTNode> AVLTree<K, V>::rotateRightLeft(shared_ptr<BSTNode> node) 
{
    node->right = rotateRight(node->right);
    return rotateLeft(node);
}

template<typename K, typename V>
int AVLTree<K, V>::getBalanceFactor(shared_ptr<BSTNode> node) const 
{
    if (!node) return 0;
    return this->getHeight(node->left) - this->getHeight(node->right);
}

template<typename K, typename V>
shared_ptr<typename AVLTree<K, V>::BSTNode> AVLTree<K, V>::rebalance(shared_ptr<BSTNode> node) {
}

template<typename K, typename V>
bool AVLTree<K, V>::isBalanced() const {
}

template<typename K, typename V>
bool AVLTree<K, V>::isValidAVLHelper(shared_ptr<BSTNode> node) const {
}

template<typename K, typename V>
int AVLTree<K, V>::getMaxDepth() const {
}

template<typename K, typename V>
double AVLTree<K, V>::getAverageDepth() const {
}

template<typename K, typename V>
void AVLTree<K, V>::calculateDepthStats(shared_ptr<BSTNode> node, int depth, int& totalDepth, int& nodeCount, int& maxDepth) const {
}

template<typename K, typename V>
bool AVLTree<K, V>::isValidAVL() const {
}

template class AVLTree<int, string>;
template class AVLTree<string, string>;
template class AVLTree<int, int>;
template class AVLTree<string, void*>;