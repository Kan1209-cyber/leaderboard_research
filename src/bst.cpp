#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <vector>
#include <string>

struct Node {
    int rating;
    std::string handle;
    Node* left;
    Node* right;
    int size; // counts nodes in subtree, needed for Kth Max

    Node(int r, std::string h) {
        rating = r;
        handle = h;
        left = right = nullptr;
        size = 1;
    }
};

// update size after every insert
void updateSize(Node* n) {
    if (n) {
        n->size = 1;
        if (n->left)  n->size += n->left->size;
        if (n->right) n->size += n->right->size;
    }
}

Node* insert(Node* root, int rating, std::string handle) {
    if (!root) return new Node(rating, handle);
    if (rating < root->rating)
        root->left = insert(root->left, rating, handle);
    else
        root->right = insert(root->right, rating, handle);
    updateSize(root);
    return root;
}

// Kth Max: find the Kth largest rating
int kthMax(Node* root, int k) {
    if (!root) return -1;
    int rightSize = root->right ? root->right->size : 0;
    if (k == rightSize + 1) return root->rating;
    if (k <= rightSize)     return kthMax(root->right, k);
    return kthMax(root->left, k - rightSize - 1);
}

int main() {
    std::ifstream file("data/processed/users.csv");
    std::string line;
    std::getline(file, line); // skip header

    std::vector<std::pair<std::string, int>> users;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string handle, ratingStr;
        std::getline(ss, handle, ',');
        std::getline(ss, ratingStr, ',');
        users.push_back({handle, std::stoi(ratingStr)});
    }

    Node* root = nullptr;

    // benchmark insertion
    auto start = std::chrono::high_resolution_clock::now();
    for (auto& u : users)
        root = insert(root, u.second, u.first);
    auto end = std::chrono::high_resolution_clock::now();

    long long insertTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "BST Insert time: " << insertTime << " ms" << std::endl;

    // benchmark Kth Max
    start = std::chrono::high_resolution_clock::now();
    for (int k = 1; k <= 1000; k++)
        kthMax(root, k);
    end = std::chrono::high_resolution_clock::now();

    long long queryTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "BST Kth Max (1000 queries): " << queryTime << " us" << std::endl;

    return 0;
}
