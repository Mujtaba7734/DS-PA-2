#include "../headers/bst.h"
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
    size_t count = nodeCount;
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
        new_Node->height = 1;
        nodeCount++;
        return new_Node;
    }
    if (key < node->key)
    {
        node->left = insertHelper(node->left, key, value);
    }
    else if (key > node->key)
    {
        node->right = insertHelper(node->right, key, value);
    }
    updateHeight(node);
    return node;
}

template<typename K, typename V>
bool BST<K, V>::remove(const K& key) 
{
    size_t count = nodeCount;
    root = removeHelper(root, key);
    if (nodeCount != count) return true;
    else return false;
}

template<typename K, typename V>
shared_ptr<typename BST<K, V>::BSTNode> BST<K, V>::removeHelper(shared_ptr<BSTNode> node, const K& key) 
{
    if (!node ) return node;
    if (key > node->key) node->right = removeHelper(node->right, key);
    else if (key < node->key) node->left = removeHelper(node->left, key);
    else
    {
        if (!node->left && !node->right) //! No Children
        {
            nodeCount--;
            return nullptr;
        }
        if (!node->left) //! Only 1 child (Right)
        {
            auto temp = node->right; 
            nodeCount--;
            return temp;
        }
        if (!node->right) //! Only 1 child (Left)
        {
            auto temp = node->left;
            nodeCount--;
            return temp;
        }
        else //! 2 children
        {
            auto temp = findMinHelper(node->right); //!Gets the smallest value from right subtree
            node->key = temp->key;
            node->value = temp->value;
            node->right = removeHelper(node->right, temp->key);
        }
    }
    updateHeight(node);
    return node;
}

template<typename K, typename V>
V* BST<K, V>::find(const K& key) 
{
    auto find = findHelper(root,key);
    if (!find) return nullptr;
    return &find->value; 
}

template<typename K, typename V>
const V* BST<K, V>::find(const K& key) const 
{
    auto find = findHelper(root,key);
    if (!find) return nullptr;
    return &find->value; 
}

template<typename K, typename V>
shared_ptr<typename BST<K, V>::BSTNode> BST<K, V>::findHelper(shared_ptr<BSTNode> node, const K& key) const 
{
    if (!node) return nullptr;
    if (node->key == key) return node;
    if (key < node->key) return findHelper(node->left, key);
    if (key > node->key) return findHelper(node->right, key);
    return nullptr;
}

template<typename K, typename V>
pair<K, V> BST<K, V>::min() const 
{
    auto min = findMinHelper(root);
    if (!min) throw runtime_error("tree is empty"); //! Checked from slack (sarfraz)
    return {min->key, min->value};
}

template<typename K, typename V>
shared_ptr<typename BST<K, V>::BSTNode> BST<K, V>::findMinHelper(shared_ptr<BSTNode> node) const 
{
    if (!node) return nullptr;
    if (!node->left) return node;
    return findMinHelper(node->left);
}

template<typename K, typename V>
pair<K, V> BST<K, V>::max() const 
{
    auto max = findMaxHelper(root);
    if (!max) throw runtime_error("tree is empty");  //! Checked from slack (sarfraz)
    return {max->key, max->value};
}

template<typename K, typename V>
shared_ptr<typename BST<K, V>::BSTNode> BST<K, V>::findMaxHelper(shared_ptr<BSTNode> node) const 
{
    if (!node) return nullptr;
    if (!node->right) return node;
    return findMaxHelper(node->right);
}

template<typename K, typename V>
vector<pair<K, V>> BST<K, V>::findRange(const K& minKey, const K& maxKey) const 
{
    vector<pair<K, V>> result;
    rangeHelper(root, minKey, maxKey, result);
    return result;
}

template<typename K, typename V>
void BST<K, V>::rangeHelper(shared_ptr<BSTNode> node, const K& minKey, const K& maxKey, vector<pair<K, V>>& result) const 
{
    if (!node) return;
    if (node->key > minKey) rangeHelper(node->left, minKey, maxKey, result); //! if the value of node is less than min, go left  
    if ((node->key >= minKey) && (node->key <= maxKey)) result.push_back({node->key, node->value});
    if (node->key < maxKey) rangeHelper(node->right, minKey, maxKey, result); //! if the value of node is greater than max, go right
}

template<typename K, typename V>
vector<pair<K, V>> BST<K, V>::inOrderTraversal() const 
{
    vector<pair<K, V>> result;
    inOrderHelper(root, result);
    return result;
}

template<typename K, typename V>
void BST<K, V>::inOrderHelper(shared_ptr<BSTNode> node, vector<pair<K, V>>& result) const 
{
    if (!node) return;
    inOrderHelper(node->left, result);
    result.push_back({node->key, node->value});
    inOrderHelper(node->right, result);
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
void BST<K, V>::updateHeight(shared_ptr<BSTNode> node) 
{
    node->height = getHeight(node);
}

template<typename K, typename V>
int BST<K, V>::getHeight(shared_ptr<BSTNode> node) const 
{
    if (!node) return 0;
    return 1 + std::max(getHeight(node->left), getHeight(node->right));
}

template class BST<int, string>;
template class BST<string, string>;
template class BST<int, int>;
template class BST<string, void*>;