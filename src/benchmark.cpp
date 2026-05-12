#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>

// ─── BST ───────────────────────────────────────────────
struct BSTNode {
    int rating; std::string handle;
    BSTNode* left; BSTNode* right; int size;
    BSTNode(int r, std::string h) : rating(r), handle(h), left(nullptr), right(nullptr), size(1) {}
};
void bstUpdate(BSTNode* n) {
    if (n) n->size = 1 + (n->left ? n->left->size : 0) + (n->right ? n->right->size : 0);
}
BSTNode* bstInsert(BSTNode* root, int rating, std::string handle) {
    BSTNode* newNode = new BSTNode(rating, handle);
    if (!root) return newNode;
    std::vector<BSTNode*> path;
    BSTNode* cur = root;
    while (true) {
        path.push_back(cur);
        if (rating < cur->rating) {
            if (!cur->left) { cur->left = newNode; break; }
            cur = cur->left;
        } else {
            if (!cur->right) { cur->right = newNode; break; }
            cur = cur->right;
        }
    }
    for (int i = path.size() - 1; i >= 0; i--)
        bstUpdate(path[i]);
    return root;
}
int bstHeight(BSTNode* root) {
    if (!root) return 0;
    int height = 0;
    std::vector<std::pair<BSTNode*, int>> stack;
    stack.push_back({root, 1});
    while (!stack.empty()) {
        auto [node, depth] = stack.back();
        stack.pop_back();
        height = std::max(height, depth);
        if (node->left)  stack.push_back({node->left,  depth + 1});
        if (node->right) stack.push_back({node->right, depth + 1});
    }
    return height;
}
int bstKthMax(BSTNode* root, int k) {
    BSTNode* cur = root;
    while (cur) {
        int rs = cur->right ? cur->right->size : 0;
        if (k == rs + 1) return cur->rating;
        if (k <= rs)     cur = cur->right;
        else { k -= rs + 1; cur = cur->left; }
    }
    return -1;
}
void bstFree(BSTNode* root) {
    if (!root) return;
    std::vector<BSTNode*> stack;
    stack.push_back(root);
    while (!stack.empty()) {
        BSTNode* node = stack.back();
        stack.pop_back();
        if (node->left)  stack.push_back(node->left);
        if (node->right) stack.push_back(node->right);
        delete node;
    }
}

// ─── AVL ───────────────────────────────────────────────
struct AVLNode {
    int rating; std::string handle;
    AVLNode* left; AVLNode* right; int height; int size;
    AVLNode(int r, std::string h) : rating(r), handle(h), left(nullptr), right(nullptr), height(1), size(1) {}
};
int avlH(AVLNode* n) { return n ? n->height : 0; }
int avlS(AVLNode* n) { return n ? n->size : 0; }
void avlUpdate(AVLNode* n) {
    if (n) {
        n->height = 1 + std::max(avlH(n->left), avlH(n->right));
        n->size   = 1 + avlS(n->left) + avlS(n->right);
    }
}
int avlBF(AVLNode* n) { return n ? avlH(n->left) - avlH(n->right) : 0; }
AVLNode* avlRotR(AVLNode* y) {
    AVLNode* x = y->left; AVLNode* T = x->right;
    x->right = y; y->left = T; avlUpdate(y); avlUpdate(x); return x;
}
AVLNode* avlRotL(AVLNode* x) {
    AVLNode* y = x->right; AVLNode* T = y->left;
    y->left = x; x->right = T; avlUpdate(x); avlUpdate(y); return y;
}
AVLNode* avlBalance(AVLNode* root) {
    int bf = avlBF(root);
    if (bf > 1)  { if (avlBF(root->left)  < 0) root->left  = avlRotL(root->left);  return avlRotR(root); }
    if (bf < -1) { if (avlBF(root->right) > 0) root->right = avlRotR(root->right); return avlRotL(root); }
    return root;
}
AVLNode* avlInsert(AVLNode* root, int rating, std::string handle) {
    if (!root) return new AVLNode(rating, handle);
    std::vector<AVLNode**> path;
    AVLNode** cur = &root;
    while (*cur) {
        path.push_back(cur);
        if (rating < (*cur)->rating) cur = &(*cur)->left;
        else                         cur = &(*cur)->right;
    }
    *cur = new AVLNode(rating, handle);
    for (int i = path.size() - 1; i >= 0; i--) {
        avlUpdate(*path[i]);
        *path[i] = avlBalance(*path[i]);
    }
    return root;
}
int avlHeight(AVLNode* root) { return root ? root->height : 0; }
int avlKthMax(AVLNode* root, int k) {
    AVLNode* cur = root;
    while (cur) {
        int rs = avlS(cur->right);
        if (k == rs + 1) return cur->rating;
        if (k <= rs)     cur = cur->right;
        else { k -= rs + 1; cur = cur->left; }
    }
    return -1;
}
void avlFree(AVLNode* root) {
    if (!root) return;
    std::vector<AVLNode*> stack;
    stack.push_back(root);
    while (!stack.empty()) {
        AVLNode* node = stack.back();
        stack.pop_back();
        if (node->left)  stack.push_back(node->left);
        if (node->right) stack.push_back(node->right);
        delete node;
    }
}

// ─── SKIP LIST ─────────────────────────────────────────
#define MAX_LEVEL 20
struct SLNode {
    int rating; std::string handle;
    std::vector<SLNode*> forward;
    SLNode(int r, std::string h, int level) : rating(r), handle(h) {
        forward.resize(level + 1, nullptr);
    }
};
struct SkipList {
    SLNode* header; int level; int total;
    SkipList() { header = new SLNode(-1, "", MAX_LEVEL); level = 0; total = 0; }
    int randomLevel() {
        int lvl = 0;
        while (lvl < MAX_LEVEL && (rand() % 2 == 0)) lvl++;
        return lvl;
    }
    void insert(int rating, std::string handle) {
        std::vector<SLNode*> update(MAX_LEVEL + 1);
        SLNode* cur = header;
        for (int i = level; i >= 0; i--) {
            while (cur->forward[i] && cur->forward[i]->rating > rating)
                cur = cur->forward[i];
            update[i] = cur;
        }
        int newLevel = randomLevel();
        if (newLevel > level) {
            for (int i = level + 1; i <= newLevel; i++) update[i] = header;
            level = newLevel;
        }
        SLNode* newNode = new SLNode(rating, handle, newLevel);
        for (int i = 0; i <= newLevel; i++) {
            newNode->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = newNode;
        }
        total++;
    }
    int kthMax(int k) {
        SLNode* cur = header->forward[0];
        int count = 0;
        while (cur) {
            count++;
            if (count == k) return cur->rating;
            cur = cur->forward[0];
        }
        return -1;
    }
    int height() { return level; }
    void freeAll() {
        SLNode* cur = header->forward[0];
        while (cur) {
            SLNode* next = cur->forward[0];
            delete cur;
            cur = next;
        }
        delete header;
    }
};

// ─── UTILS ─────────────────────────────────────────────
long long median(std::vector<long long> v) {
    std::sort(v.begin(), v.end());
    return v[v.size() / 2];
}

// ─── MAIN ──────────────────────────────────────────────
int main() {
    srand(42);

    std::ifstream file("C:/Users/kanis/OneDrive/Documents/Desktop/value/leaderboard_research/data/processed/users.csv");
    if (!file.is_open()) { std::cout << "ERROR: cannot open file" << std::endl; return 1; }

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

    std::ofstream out("results/benchmark.csv");
    out << "N,structure,data_order,insert_ms,kthmax_us,height" << std::endl;

    std::vector<int> sizes = {1000, 5000, 10000, 50000, 100000};
    std::vector<std::string> orders = {"original", "ascending", "descending"};

    for (int N : sizes) {
        for (std::string order : orders) {
            std::cout << "Running N=" << N << " order=" << order << std::endl;
            auto subset = std::vector<std::pair<std::string,int>>(users.begin(), users.begin() + N);

            if (order == "ascending")
                std::sort(subset.begin(), subset.end(), [](auto& a, auto& b){ return a.second < b.second; });
            else if (order == "descending")
                std::sort(subset.begin(), subset.end(), [](auto& a, auto& b){ return a.second > b.second; });

            std::vector<long long> bIns, bQry, aIns, aQry, sIns, sQry;

            for (int run = 0; run < 10; run++) {
                std::cout << "  Run " << run+1 << "/10" << std::endl;

                // BST
                BSTNode* broot = nullptr;
                auto s = std::chrono::high_resolution_clock::now();
                for (auto& u : subset) broot = bstInsert(broot, u.second, u.first);
                auto e = std::chrono::high_resolution_clock::now();
                bIns.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(e-s).count());
                s = std::chrono::high_resolution_clock::now();
                for (int k = 1; k <= 1000; k++) bstKthMax(broot, k);
                e = std::chrono::high_resolution_clock::now();
                bQry.push_back(std::chrono::duration_cast<std::chrono::microseconds>(e-s).count());
                bstFree(broot);

                // AVL
                AVLNode* aroot = nullptr;
                s = std::chrono::high_resolution_clock::now();
                for (auto& u : subset) aroot = avlInsert(aroot, u.second, u.first);
                e = std::chrono::high_resolution_clock::now();
                aIns.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(e-s).count());
                s = std::chrono::high_resolution_clock::now();
                for (int k = 1; k <= 1000; k++) avlKthMax(aroot, k);
                e = std::chrono::high_resolution_clock::now();
                aQry.push_back(std::chrono::duration_cast<std::chrono::microseconds>(e-s).count());
                avlFree(aroot);

                // Skip List
                SkipList sl;
                s = std::chrono::high_resolution_clock::now();
                for (auto& u : subset) sl.insert(u.second, u.first);
                e = std::chrono::high_resolution_clock::now();
                sIns.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(e-s).count());
                s = std::chrono::high_resolution_clock::now();
                for (int k = 1; k <= 1000; k++) sl.kthMax(k);
                e = std::chrono::high_resolution_clock::now();
                sQry.push_back(std::chrono::duration_cast<std::chrono::microseconds>(e-s).count());
                sl.freeAll();
            }

            // heights
            BSTNode* broot2 = nullptr;
            for (auto& u : subset) broot2 = bstInsert(broot2, u.second, u.first);
            int bH = bstHeight(broot2); bstFree(broot2);

            AVLNode* aroot2 = nullptr;
            for (auto& u : subset) aroot2 = avlInsert(aroot2, u.second, u.first);
            int aH = avlHeight(aroot2); avlFree(aroot2);

            SkipList sl2;
            for (auto& u : subset) sl2.insert(u.second, u.first);
            int sH = sl2.height(); sl2.freeAll();

            out << N << ",BST,"      << order << "," << median(bIns) << "," << median(bQry) << "," << bH << std::endl;
            out << N << ",AVL,"      << order << "," << median(aIns) << "," << median(aQry) << "," << aH << std::endl;
            out << N << ",SkipList," << order << "," << median(sIns) << "," << median(sQry) << "," << sH << std::endl;
        }
    }

    std::cout << "Done. Results saved to results/benchmark.csv" << std::endl;
    return 0;
}
