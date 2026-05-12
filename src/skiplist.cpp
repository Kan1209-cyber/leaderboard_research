#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>

#define MAX_LEVEL 20

// ─── SKIP LIST ───────────────────────────────────────────────
struct SLNode {
    int rating;
    std::string handle;
    int size; // nodes below and including this in this lane
    std::vector<SLNode*> forward;
    SLNode(int r, std::string h, int level) : rating(r), handle(h), size(0) {
        forward.resize(level + 1, nullptr);
    }
};

struct SkipList {
    SLNode* header;
    int level;
    int totalNodes;

    SkipList() {
        header = new SLNode(-1, "", MAX_LEVEL);
        level = 0;
        totalNodes = 0;
    }

    int randomLevel() {
        int lvl = 0;
        while (lvl < MAX_LEVEL && (rand() % 2 == 0))
            lvl++;
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
            for (int i = level + 1; i <= newLevel; i++)
                update[i] = header;
            level = newLevel;
        }
        SLNode* newNode = new SLNode(rating, handle, newLevel);
        for (int i = 0; i <= newLevel; i++) {
            newNode->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = newNode;
        }
        totalNodes++;
    }

    // Kth Max: skip list is sorted descending so just walk level 0
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
};

long long median(std::vector<long long> v) {
    std::sort(v.begin(), v.end());
    return v[v.size() / 2];
}

int main() {
    srand(42); // fixed seed for reproducibility

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

    std::ofstream out("results/skiplist_benchmark.csv");
    out << "N,structure,data_order,insert_ms,kthmax_us,height" << std::endl;

    std::vector<int> sizes = {1000, 5000, 10000, 50000, 100000};
    std::vector<std::string> orders = {"original", "ascending", "descending"};

    for (int N : sizes) {
        for (std::string order : orders) {
            std::cout << "Running N = " << N << " order = " << order << std::endl;
            auto subset = std::vector<std::pair<std::string,int>>(users.begin(), users.begin() + N);

            if (order == "ascending")
                std::sort(subset.begin(), subset.end(), [](auto& a, auto& b){ return a.second < b.second; });
            else if (order == "descending")
                std::sort(subset.begin(), subset.end(), [](auto& a, auto& b){ return a.second > b.second; });

            std::vector<long long> insertRuns, queryRuns;
            int slHeight = 0;

            for (int run = 0; run < 10; run++) {
                std::cout << "  Run " << run+1 << "/10" << std::endl;

                SkipList sl;
                auto s = std::chrono::high_resolution_clock::now();
                for (auto& u : subset) sl.insert(u.second, u.first);
                auto e = std::chrono::high_resolution_clock::now();
                insertRuns.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(e - s).count());

                s = std::chrono::high_resolution_clock::now();
                for (int k = 1; k <= 1000; k++) sl.kthMax(k);
                e = std::chrono::high_resolution_clock::now();
                queryRuns.push_back(std::chrono::duration_cast<std::chrono::microseconds>(e - s).count());

                if (run == 0) slHeight = sl.height();
            }

            out << N << ",SkipList," << order << "," << median(insertRuns) << "," << median(queryRuns) << "," << slHeight << std::endl;
        }
    }

    std::cout << "Done. Results saved to results/skiplist_benchmark.csv" << std::endl;
    return 0;
}
