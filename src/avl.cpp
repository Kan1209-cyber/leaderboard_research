#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <vector>
#include <string>
#include <algorithm>

struct Node {
    int rating;
    std::string handle;
    Node* left;
    Node* right;
    int height;
    int size;
    Node(int r, std::string h) : rating(r), handle(h), left(nullptr), right(nullptr), height(1), size(1) {}
};

int getHeight(Node* n) { return n ? n->height : 0; }
int getSize(Node* n)   { return n ? n->size : 0; }

void update(Node* n) {
    if (n) {
        n->height = 1 + std::max(getHeight(n->left), getHeight(n->right));
        n->size   = 1 + getSize(n->left) + getSize(n->right);
    }
}

int balanceFactor(Node* n) {
    return n ? getHeight(n->left) - getHeight(n->right) : 0;
}

Node* rotateRight(Node* y) {
    Node* x  = y->left;
    Node* T2 = x->right;
    x->right = y;
    y->left  = T2;
    update(y);
    update(x);
    return x;
}

Node* rotateLeft(Node* x) {
    Node* y  = x->right;
    Node* T2 = y->left;
    y->left  = x;
    x->right = T2;
    update(x);
    update(y);
    return y;
}

Node* balance(Node* root) {
    int bf = balanceFactor(root);
    if (bf > 1) {
        if (balanceFactor(root->left) < 0)
            root->left = rotateLeft(root->left);
        return rotateRight(root);
    }
    if (bf < -1) {
        if (balanceFactor(root->right) > 0)
            root->right = rotateRight(root->right);
        return rotateLeft(root);
    }
    return root;
}

Node* insert(Node* root, int rating, std::string handle) {
    if (!root) return new Node(rating, handle);
    std::vector<Node**> path;
    Node** cur = &root;
    while (*cur) {
        path.push_back(cur);
        if (rating < (*cur)->rating)
            cur = &(*cur)->left;
        else
            cur = &(*cur)->right;
    }
    *cur = new Node(rating, handle);
    for (int i = path.size() - 1; i >= 0; i--) {
        update(*path[i]);
        *path[i] = balance(*path[i]);
    }
    return root;
}

int kthMax(Node* root, int k) {
    Node* cur = root;
    while (cur) {
        int rightSize = getSize(cur->right);
        if (k == rightSize + 1) return cur->rating;
        if (k <= rightSize)     cur = cur->right;
        else { k -= rightSize + 1; cur = cur->left; }
    }
    return -1;
}

int main() {
    std::ifstream file("C:/Users/kanis/OneDrive/Documents/Desktop/value/leaderboard_research/data/processed/users.csv");
    if (!file.is_open()) {
        std::cout << "ERROR: could not open file" << std::endl;
        return 1;
    }

    std::string line;
    std::getline(file, line);

    std::vector<std::pair<std::string, int>> users;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string handle, ratingStr;
        std::getline(ss, handle, ',');
        std::getline(ss, ratingStr, ',');
        if (!ratingStr.empty())
            users.push_back({handle, std::stoi(ratingStr)});
    }

    std::cout << "Loaded " << users.size() << " users" << std::endl;
    std::cout << "Starting insertion..." << std::endl;

    Node* root = nullptr;

    auto start = std::chrono::high_resolution_clock::now();
    for (auto& u : users)
        root = insert(root, u.second, u.first);
    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Insertion done" << std::endl;

    long long insertTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "AVL Insert time: " << insertTime << " ms" << std::endl;

    start = std::chrono::high_resolution_clock::now();
    for (int k = 1; k <= 1000; k++)
        kthMax(root, k);
    end = std::chrono::high_resolution_clock::now();

    long long queryTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "AVL Kth Max (1000 queries): " << queryTime << " us" << std::endl;

    return 0;
}