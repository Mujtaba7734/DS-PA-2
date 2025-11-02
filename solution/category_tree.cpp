#include "../headers/category_tree.h"
#include "../headers/post.h"
#include <iostream>
#include <queue>
#include <algorithm>
#include <climits>
#include <functional>
using namespace std;

CategoryNode::CategoryNode(const string& name) 
    : categoryName(name), totalPostCount(0) {
}

void CategoryNode::addChild(shared_ptr<CategoryNode> child) 
{
    if (!child) return;
    child->parent = weak_from_this();
    children.push_back(child); 
}   

bool CategoryNode::removeChild(const string& childName) 
{
    if (childName == "") return false;

    auto entry = children.begin();
    while (entry != children.end())
    {
        if ((*entry)->categoryName == childName)
        {
            (*entry)->parent.reset(); //! safely detach it
            children.erase(entry);
            return true;
        }
        entry++;
    }
    return false;

}

shared_ptr<CategoryNode> CategoryNode::findChild(const string& childName) const 
{
    if (childName == "") return nullptr;

    for (auto child: children)
    {
        if (child->categoryName == childName)
        {
            return child;
        }
    }
    return nullptr;
}

void CategoryNode::addPost(Post* post) 
{
    if (!post) return;

    posts.push_back(post);
    updatePostCounts();
}

bool CategoryNode::removePost(Post* post) 
{
    if (!post) return false;

    auto entry = posts.begin();
    while (entry != posts.end())
    {
        if (*entry == post)
        {
            posts.erase(entry);
            updatePostCounts();
            return true;
        }
        entry++;
    }
    return false;

}

void CategoryNode::updatePostCounts() 
{
    totalPostCount = posts.size();
    for (auto child: children)
    {
        totalPostCount += child->totalPostCount;
    }
    if (auto p = parent.lock())
    {
        p->updatePostCounts();
    }
}

CategoryTree::CategoryTree() {
    root = make_shared<CategoryNode>("root");
}

bool CategoryTree::addCategory(const string& categoryPath) 
{
    if (categoryPath == "") return false;
    if (findCategory(categoryPath)) return false;
    auto components = parseCategoryPath(categoryPath);
    auto curr = root;
    for (auto name: components)
    {
        auto child = curr->findChild(name);
        if (!child)
        {
            child = make_shared<CategoryNode>(name);
            curr->addChild(child);
        }
        curr = child;
    }
    return true;
}

bool CategoryTree::removeCategory(const string& categoryPath) 
{
    if (categoryPath == "" || categoryPath == "root") return false;

    auto node = findCategory(categoryPath);
    if (!node) return false;

    auto parent = node->parent.lock();
    if (!parent) return false;

    for (auto temp = parent; temp; temp = temp->parent.lock())
    {
        temp->totalPostCount -= node->totalPostCount;
    }

    parent->removeChild(node->categoryName);
    node->parent.reset();
    return true;
}

bool CategoryTree::moveCategory(const string& fromPath, const string& toPath) 
{
    auto fromNode = findCategory(fromPath);
    if (!fromNode || fromPath == "root") return false;

    auto oldparent = fromNode->parent.lock();
    if (!oldparent) return false;

    for (auto temp = oldparent; temp; temp = temp->parent.lock())
    {
        temp->totalPostCount -= fromNode->totalPostCount;
    }

    oldparent->removeChild(fromNode->categoryName);

    addCategory(toPath);
    auto newParent = findCategory(toPath);
    if (!newParent) return false;

    newParent->addChild(fromNode);
    
    for (auto temp = newParent; temp; temp = temp->parent.lock())
    {
        temp->totalPostCount += fromNode->totalPostCount;
    }

    return true;
}

shared_ptr<CategoryNode> CategoryTree::findCategory(const string& categoryPath) const 
{
    if (categoryPath == "") return root;

    auto components = parseCategoryPath(categoryPath);
    auto curr = root;

    for (auto name: components)
    {
        auto child = curr->findChild(name);
        if (!child) return nullptr;
        curr = child;
    }
    return curr;
}

void CategoryTree::addPost(Post* post) 
{
    if (!post) return;

    addCategory(post->category);
    auto node = findCategory(post->category);
    if (!node) return;
    
    node->addPost(post);
}

bool CategoryTree::removePost(Post* post) 
{
    if (!post) return false;

    auto node = findCategory(post->category);
    if (!node) return false;

    if (!node->removePost(post)) return false;
    
    return true;
}

vector<Post*> CategoryTree::getPostsInCategory(const string& categoryPath, bool includeSubcategories) const 
{
    vector<Post*> PostsInCat;
    
    auto node = findCategory(categoryPath);
    if (!node) return PostsInCat;

    if (!includeSubcategories)
    {
        for (auto p: node->posts)
        {
            PostsInCat.push_back(p);
        }
        return PostsInCat;
    }
    queue<shared_ptr<CategoryNode>> q;
    q.push(node);

    while (!q.empty())
    {
        auto current = q.front();
        q.pop();

        for (auto p: current->posts)
        {
            PostsInCat.push_back(p);
        }  

        for (auto child: current->children)
        {
            q.push(child);
        }
    }
    
    return PostsInCat;
}

void CategoryTree::displayTree() const {
}

CategoryTree::PreOrderIterator::PreOrderIterator(shared_ptr<CategoryNode> root) 
{
    current = nullptr;
}

CategoryTree::PreOrderIterator::PreOrderIterator() : current(nullptr) {
}

shared_ptr<CategoryNode> CategoryTree::PreOrderIterator::operator*() const
{
    return current;
}

CategoryTree::PreOrderIterator& CategoryTree::PreOrderIterator::operator++() 
{
    current = nullptr;
    return *this;
}

bool CategoryTree::PreOrderIterator::operator!=(const PreOrderIterator& other) const 
{
    return current != other.current;
}

bool CategoryTree::PreOrderIterator::operator==(const PreOrderIterator& other) const 
{
    return current == other.current;
}

CategoryTree::PostOrderIterator::PostOrderIterator(shared_ptr<CategoryNode> root) 
    : current(nullptr), lastVisited(nullptr) {
}

CategoryTree::PostOrderIterator::PostOrderIterator() 
    : current(nullptr), lastVisited(nullptr) {
}

shared_ptr<CategoryNode> CategoryTree::PostOrderIterator::operator*() const 
{
    return current;
}

CategoryTree::PostOrderIterator& CategoryTree::PostOrderIterator::operator++() 
{
    current = nullptr;
    return *this;
}

bool CategoryTree::PostOrderIterator::operator!=(const PostOrderIterator& other) const 
{
    return current != other.current;
}

bool CategoryTree::PostOrderIterator::operator==(const PostOrderIterator& other) const 
{
    return current == other.current;
}

CategoryTree::BreadthFirstIterator::BreadthFirstIterator(shared_ptr<CategoryNode> root) 
    : currentIndex(0) 
{
    currentIndex = SIZE_MAX;
}

CategoryTree::BreadthFirstIterator::BreadthFirstIterator() : currentIndex(SIZE_MAX) {
}

shared_ptr<CategoryNode> CategoryTree::BreadthFirstIterator::operator*() const 
{
    return nullptr;
}

CategoryTree::BreadthFirstIterator& CategoryTree::BreadthFirstIterator::operator++() 
{
    currentIndex = SIZE_MAX;
    return *this;
}

bool CategoryTree::BreadthFirstIterator::operator!=(const BreadthFirstIterator& other) const 
{
    return currentIndex != other.currentIndex;
}

bool CategoryTree::BreadthFirstIterator::operator==(const BreadthFirstIterator& other) const 
{
    return currentIndex == other.currentIndex;
}

CategoryTree::PreOrderIterator CategoryTree::preOrderBegin() const 
{
    return PreOrderIterator(root);
}

CategoryTree::PreOrderIterator CategoryTree::preOrderEnd() const 
{
    return PreOrderIterator();
}

CategoryTree::PostOrderIterator CategoryTree::postOrderBegin() const 
{
    return PostOrderIterator(root);
}

CategoryTree::PostOrderIterator CategoryTree::postOrderEnd() const 
{
    return PostOrderIterator();
}

CategoryTree::BreadthFirstIterator CategoryTree::breadthFirstBegin() const 
{
    return BreadthFirstIterator(root);
}

CategoryTree::BreadthFirstIterator CategoryTree::breadthFirstEnd() const 
{
    return BreadthFirstIterator();
}