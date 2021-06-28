#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "thread_pool.hpp"
// #include "thread_pool_original.hpp"

void sum(int start, const int end, std::vector<int>& sums, const int index) {
    int s = 0;
    for (; start < end; ++start) s += start;
    sums[index] = s;
}

int main() {
    auto start = std::chrono::high_resolution_clock::now();

    thread_pool pool;
    std::vector<int> sums(100);
    for (int i = 0; i < 100; ++i) {
        pool.push_task(std::bind(sum, 0, 100000000, std::ref(sums), i));
    }
    pool.wait_for_tasks();

    std::size_t sum = 0;
    for (const auto& i : sums) {
        sum += i % 100000007;
    }
    std::cerr << sum << '\n';
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << '\n';
    return 0;
}
