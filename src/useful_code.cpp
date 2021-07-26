// #include "threading/thread_pool.hpp"
// #include "threading/thread_pool_original.hpp"
// using namespace std::placeholders;
// const int numThreads = std::thread::hardware_concurrency();
// const int concurrentThreshold = 10000000;

// if (copy.size() < concurrentThreshold) {
// }

/**
 * GNU Parallel
 */
// __gnu_parallel::for_each(copy.begin(), copy.end(), [&newPoint](Point &point) {
//     point.weight += newPoint.weight;
//     point.profit += newPoint.profit;
// });
/**
 * OpenMP
 */
// #pragma omp parallel for if (copy.size() >= concurrentThreshold)
// for (size_t i = 0; i < copy.size(); ++i) {
//     copy[i].shift(newPoint);
// }
// return copy;
/**
 * Thread Pool (thread pool loop division)
 */
// ++count;
// pool.parallelize_loop(
//     0, (int)copy.size(),
//     std::bind(&Knapsack<T, U>::shiftPoints, this, std::ref(copy), newPoint, _1, _2));

// return copy;
/**
 * C++ Threads
 */
// std::vector<std::thread> threads;
// auto step = copy.size() / 8;
// size_t i = 0;
// for (; i < copy.size() - step; i += step) {
//     threads.emplace_back(&Knapsack<T, U>::shiftPoints, this, std::ref(copy), newPoint, i,
//                          i + step);
// }
// threads.emplace_back(&Knapsack<T, U>::shiftPoints, this, std::ref(copy), newPoint, i,
//                      copy.size());
// for (auto &thread : threads) thread.join();
// return copy;
/**
 * Thread Pool (custom loop division)
 */
// auto step = copy.size() / numThreads;
// size_t i = 0;
// for (; i < copy.size() - step * 2; i += step) {
//     pool.push_task(
//         std::bind(&Knapsack<T, U>::shiftPoints, this, std::ref(copy), newPoint, i, i +
//         step));
// }
// pool.push_task(
//     std::bind(&Knapsack<T, U>::shiftPoints, this, std::ref(copy), newPoint, i,
//     copy.size()));
// pool.wait_for_tasks();
// return copy;

// std::mutex mut;
// {
//     std::scoped_lock lock(mut);
//     std::cerr << start << ' ' << end << '\n';
// }