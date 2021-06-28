#include <algorithm>
#include <chrono>
#include <iostream>
#include <vector>

template <typename T, typename U>
struct Item {
    T weight;
    U profit;
};

template <typename T, typename U>
T knapsack(const std::vector<Item<T, U>>& items, const int& n, const T& threshold) {
    std::vector<int> dp(threshold + 1, 0);
    for (int i = 0; i < n; i++)
        for (int j = threshold; j >= items[i].weight; j--)
            dp[j] = std::max(dp[j], items[i].profit + dp[j - items[i].weight]);
    return dp[threshold];
}

int main() {
    std::ios_base::sync_with_stdio(0);
    std::cin.tie(0);

    int n, t;
    std::cin >> n >> t;

    std::vector<Item<int, int>> items(n);
    for (int i = 0; i < n; ++i) {
        std::cin >> items[i].weight >> items[i].profit;
    }

    auto start = std::chrono::high_resolution_clock::now();
    std::cout << knapsack<int, int>(items, n, t) << '\n';
    auto end = std::chrono::high_resolution_clock::now();

    std::cerr << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << '\n';

    return 0;
}
