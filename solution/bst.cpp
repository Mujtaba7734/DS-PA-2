#include "headers/bst.h"
#include <iostream>
#include <algorithm>
using namespace std;

template<typename K, typename V>
BST<K, V>::BST() : root(nullptr), nodeCount(0) {
    comparator = [](const K& a, const K& b) { return a < b; };
}

template<typename K, typename V>
BST<K, V>::BST(function<bool(const K&, const K&)> comp) 
    : root(nullptr), nodeCount(0), comparator(comp) {
}

template<typename K, typename V>
bool BST<K, V>::insert(const K& key, const V& value) 
{
    int count = nodeCount;
    root = insertHelper(root, key, value);
    if (nodeCount != count) return true;
    else return false;
}

template<typename K, typename V>
shared_ptr<typename BST<K, V>::BSTNode> BST<K, V>::insertHelper(shared_ptr<BSTNode> node, const K& key, const V& value) 
{
    if (!node)
    {
        auto new_Node = make_shared<BSTNode>(key, value);
        nodeCount++;
        return new_Node;
    }
    if (key < node->key)
    {
        node.left = insertHelper(node->left, key, value);
    }
    else if (key > node->key)
    {
        node.right = insertHelper(node->right, key, value);
    }
    else
    {
        return node->key = key;
    }
}

template<typename K, typename V>
bool BST<K, V>::remove(const K& key) 
{
    int count = nodeCount;
    root = removeHelper(root, key);
    if (nodeCount != count) return true;
    else return false;
}

template<typename K, typename V>
shared_ptr<typename BST<K, V>::BSTNode> BST<K, V>::removeHelper(shared_ptr<BSTNode> node, const K& key) 
{
    if (!node ) return node;
    if (key > node->key) node->right = removeHelper(node->right, key);
    if (key < node->key) node->left = removeHelper(node->left, key);
    else
    {
        if (!node->left && !node->right) //! No Children
        {
            delete node;
            nodeCount--;
            return nullptr;
        }
        if (!node->left) //! Only 1 child (Right)
        {
            auto *temp = node->right; 
            delete node;
            nodeCount--;
            return temp;
        }
        if (!node->right) //! Only 1 child (Left)
        {
            auto *temp = node->left;
            delete node;
            nodeCount--;
            return temp;
        }
        else //! 2 children
        {
            auto *temp = findMinHelper(node->right); //!Gets the smallest value from right subtree
            node->data = temp->data;
            node->right = removeHelper(node->right, temp->key);
        }
    }
}

template<typename K, typename V>
V* BST<K, V>::find(const K& key) 
{
    if (!findHelper(root,key)) return NULL;
    return &findHelper(root, key)->value; 
}

template<typename K, typename V>
const V* BST<K, V>::find(const K& key) const 
{
    
}

template<typename K, typename V>
shared_ptr<typename BST<K, V>::BSTNode> BST<K, V>::findHelper(shared_ptr<BSTNode> node, const K& key) const 
{
    if (!node) return nullptr;
    if (node->key == key) return node;
    if (key < node->key) return findHelper(node->left, key);
    if (key > node->key) return findHelper(node->right, key);
}

template<typename K, typename V>
pair<K, V> BST<K, V>::min() const 
{
    auto *min = findMinHelper(root);
    return {min->key, min->value};
}

template<typename K, typename V>
shared_ptr<typename BST<K, V>::BSTNode> BST<K, V>::findMinHelper(shared_ptr<BSTNode> node) const 
{
    if (!node->left) return node;
    return findMinHelper(node->left);
}

template<typename K, typename V>
pair<K, V> BST<K, V>::max() const 
{
    auto *max = findMinHelper(root);
    return {max->key, max->value};
}

template<typename K, typename V>
shared_ptr<typename BST<K, V>::BSTNode> BST<K, V>::findMaxHelper(shared_ptr<BSTNode> node) const 
{
    if (!node->right) return node;
    return findMinHelper(node->right);
}

template<typename K, typename V>
vector<pair<K, V>> BST<K, V>::findRange(const K& minKey, const K& maxKey) const 
{
    vector<pair<K, V>> result;
    return findRange(root, result);
}

template<typename K, typename V>
void BST<K, V>::rangeHelper(shared_ptr<BSTNode> node, const K& minKey, const K& maxKey, vector<pair<K, V>>& result) const 
{
    if (!node) return;
    if (minKey < node->key) rangeHelper(node->left, minKey, maxKey, result); //! if the value of node is less than min, go left  
    if (!(minKey < node->key) && !(maxKey > node->key)) result.push_back({node->key, node->value})
    if (minKey > node->key) rangeHelper(node->right, minKey, maxKey, result); //! if the value of node is greater than max, go right
}

template<typename K, typename V>
vector<pair<K, V>> BST<K, V>::inOrderTraversal() const 
{
    vector<pair<K, V>> result;
    return inOrderTraversal(root, result);
}

template<typename K, typename V>
void BST<K, V>::inOrderHelper(shared_ptr<BSTNode> node, vector<pair<K, V>>& result) const 
{
    if (!root) return;
    inOrderHelper(node->left);
    result.push_back({node->key, node->value});
    inOrderHelper(node->right);
}

template<typename K, typename V>
void BST<K, V>::displayTree() const {
}

template<typename K, typename V>
void BST<K, V>::displayHelper(shared_ptr<BSTNode> node, int depth) const 
{
}

template<typename K, typename V>
bool BST<K, V>::isValidBST() const {
}

template<typename K, typename V>
bool BST<K, V>::isValidBSTHelper(shared_ptr<BSTNode> node, const K* minVal, const K* maxVal) const {
}

template<typename K, typename V>
void BST<K, V>::updateHeight(shared_ptr<BSTNode> node) {
}

template<typename K, typename V>
int BST<K, V>::getHeight(shared_ptr<BSTNode> node) const {
}

template class BST<int, string>;
template class BST<string, string>;
template class BST<int, int>;
template class BST<string, void*>;