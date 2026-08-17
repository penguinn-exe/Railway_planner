#ifndef AVL_TREE_HPP
#define AVL_TREE_HPP

#include <iostream>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <functional>

template <typename K, typename V>
class AVLTree {
public:
    struct Node {
        K key;
        V value;
        int height;
        Node* left;
        Node* right;

        Node(const K& k, const V& v)
            : key(k), value(v), height(1), left(nullptr), right(nullptr) {}
    };

private:
    Node* root;
    size_t treeSize;

    int getHeight(Node* node) const {
        return node ? node->height : 0;
    }

    int getBalanceFactor(Node* node) const {
        return node ? getHeight(node->left) - getHeight(node->right) : 0;
    }

    void updateHeight(Node* node) {
        if (node) {
            node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));
        }
    }

    Node* rightRotate(Node* y) {
        Node* x = y->left;
        Node* T2 = x->right;

        x->right = y;
        y->left = T2;

        updateHeight(y);
        updateHeight(x);

        return x;
    }

    Node* leftRotate(Node* x) {
        Node* y = x->right;
        Node* T2 = y->left;

        y->left = x;
        x->right = T2;

        updateHeight(x);
        updateHeight(y);

        return y;
    }

    Node* insertNode(Node* node, const K& key, const V& value, bool& inserted) {
        if (!node) {
            inserted = true;
            return new Node(key, value);
        }

        if (key < node->key) {
            node->left = insertNode(node->left, key, value, inserted);
        } else if (key > node->key) {
            node->right = insertNode(node->right, key, value, inserted);
        } else {
            // Key already exists, update value
            node->value = value;
            inserted = false;
            return node;
        }

        updateHeight(node);

        int balance = getBalanceFactor(node);

        // Left Left Case
        if (balance > 1 && key < node->left->key) {
            return rightRotate(node);
        }

        // Right Right Case
        if (balance < -1 && key > node->right->key) {
            return leftRotate(node);
        }

        // Left Right Case
        if (balance > 1 && key > node->left->key) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }

        // Right Left Case
        if (balance < -1 && key < node->right->key) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }

        return node;
    }

    Node* getMinValueNode(Node* node) const {
        Node* current = node;
        while (current && current->left) {
            current = current->left;
        }
        return current;
    }

    Node* removeNode(Node* node, const K& key, bool& removed) {
        if (!node) {
            removed = false;
            return nullptr;
        }

        if (key < node->key) {
            node->left = removeNode(node->left, key, removed);
        } else if (key > node->key) {
            node->right = removeNode(node->right, key, removed);
        } else {
            removed = true;
            if (!node->left || !node->right) {
                Node* temp = node->left ? node->left : node->right;
                if (!temp) {
                    delete node;
                    node = nullptr;
                } else {
                    Node* old = node;
                    node = temp;
                    delete old;
                }
            } else {
                Node* temp = getMinValueNode(node->right);
                node->key = temp->key;
                node->value = temp->value;
                node->right = removeNode(node->right, temp->key, removed);
            }
        }

        if (!node) return nullptr;

        updateHeight(node);
        int balance = getBalanceFactor(node);

        // Left Left
        if (balance > 1 && getBalanceFactor(node->left) >= 0) {
            return rightRotate(node);
        }

        // Left Right
        if (balance > 1 && getBalanceFactor(node->left) < 0) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }

        // Right Right
        if (balance < -1 && getBalanceFactor(node->right) <= 0) {
            return leftRotate(node);
        }

        // Right Left
        if (balance < -1 && getBalanceFactor(node->right) > 0) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }

        return node;
    }

    void inOrderTraversal(Node* node, std::vector<std::pair<K, V>>& result) const {
        if (!node) return;
        inOrderTraversal(node->left, result);
        result.push_back({node->key, node->value});
        inOrderTraversal(node->right, result);
    }

    void rangeQuery(Node* node, const K& low, const K& high, std::vector<std::pair<K, V>>& result) const {
        if (!node) return;
        if (low < node->key) {
            rangeQuery(node->left, low, high, result);
        }
        if (low <= node->key && node->key <= high) {
            result.push_back({node->key, node->value});
        }
        if (high > node->key) {
            rangeQuery(node->right, low, high, result);
        }
    }

    void clearTree(Node* node) {
        if (!node) return;
        clearTree(node->left);
        clearTree(node->right);
        delete node;
    }

public:
    AVLTree() : root(nullptr), treeSize(0) {}

    ~AVLTree() {
        clearTree(root);
    }

    void insert(const K& key, const V& value) {
        bool inserted = false;
        root = insertNode(root, key, value, inserted);
        if (inserted) {
            treeSize++;
        }
    }

    bool remove(const K& key) {
        bool removed = false;
        root = removeNode(root, key, removed);
        if (removed) {
            treeSize--;
        }
        return removed;
    }

    V* find(const K& key) {
        Node* curr = root;
        while (curr) {
            if (key == curr->key) return &(curr->value);
            if (key < curr->key) curr = curr->left;
            else curr = curr->right;
        }
        return nullptr;
    }

    const V* find(const K& key) const {
        Node* curr = root;
        while (curr) {
            if (key == curr->key) return &(curr->value);
            if (key < curr->key) curr = curr->left;
            else curr = curr->right;
        }
        return nullptr;
    }

    bool contains(const K& key) const {
        return find(key) != nullptr;
    }

    size_t size() const {
        return treeSize;
    }

    bool empty() const {
        return treeSize == 0;
    }

    int height() const {
        return getHeight(root);
    }

    std::vector<std::pair<K, V>> inOrder() const {
        std::vector<std::pair<K, V>> result;
        inOrderTraversal(root, result);
        return result;
    }

    std::vector<std::pair<K, V>> rangeSearch(const K& low, const K& high) const {
        std::vector<std::pair<K, V>> result;
        rangeQuery(root, low, high, result);
        return result;
    }

    void clear() {
        clearTree(root);
        root = nullptr;
        treeSize = 0;
    }
};

#endif // AVL_TREE_HPP
