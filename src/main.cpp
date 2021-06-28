#include <chrono>
#include <iostream>
#include <parallel/algorithm>
#include <set>
#include <thread>
#include <vector>

// #include "threading/thread_pool.hpp"
// #include "threading/thread_pool_original.hpp"
// using namespace std::placeholders;

const int numThreads = std::thread::hardware_concurrency();
const int concurrentThreshold = 10000000;

template <typename T, typename U>
class Knapsack {
   private:
    // thread_pool_original pool;
    struct Point {
        T weight;
        U profit;

        Point(){};
        Point(const T &_weight, const U &_profit) : weight(_weight), profit(_profit){};

        inline bool operator<(const Point &other) const {
            return (weight < other.weight) || ((weight == other.weight) && (profit > other.profit));
        }

        inline Point operator+(const Point &other) {
            return {weight + other.weight, profit + other.profit};
        }
    };
    int m_size;
    T m_threshold;
    std::vector<Point> items;

   public:
    Knapsack() {}
    Knapsack(const int &size, const T &threshold)
        : m_size(size), m_threshold(threshold), items(size) {}

    void shiftPoints(std::vector<Point> &points, const Point &newPoint, const int &begin,
                     const int &end) {
        for (int i = begin; i < end; ++i) {
            points[i].weight += newPoint.weight;
            points[i].profit += newPoint.profit;
        }
    }

    auto shiftAllPoints(std::vector<Point> copy, const Point &newPoint) {
        // if (copy.size() < concurrentThreshold) {
        shiftPoints(copy, newPoint, 0, copy.size());
        return copy;
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
    }

    void parallel_nemhauser_ullmann() {
        auto pointsA = nemhauser_ullmann(0, m_size / 2);
        auto pointsB = nemhauser_ullmann(m_size / 2, m_size);

        std::vector<Point> sumPoints;
        sumPoints.reserve((pointsA.size() - 1) * (pointsB.size() - 1));

        auto cmp = [](const Point &a, const Point &b) { return a < b; };
        std::set<Point, decltype(cmp)> maxima(cmp);

        Point *pA, *pB;
        for (size_t i = 1; i < pointsA.size(); ++i) {
            pA = &pointsA[i];
            for (size_t j = 1; j < pointsB.size(); ++j) {
                pB = &pointsB[j];
                sumPoints.emplace_back(std::move(*pA + *pB));
            }
        }

        __gnu_parallel::sort(sumPoints.begin(), sumPoints.end(),
                             [](const Point &a, const Point &b) { return a < b; });

        std::vector<Point> cleanSumPoints;
        sumPoints.reserve(sumPoints.size());

        T previousWeight = -1;
        U pmax = -1;
        for (const auto &point : sumPoints) {
            if (point.weight != previousWeight && point.profit > pmax) {
                pmax = point.profit;
                cleanSumPoints.emplace_back(std::move(point));
            }
            previousWeight = point.weight;
        }

        auto points = merge(merge(pointsA, cleanSumPoints), pointsB);
        printPoints(points);
    }

    void nemhauser_ullmann() {
        auto points = nemhauser_ullmann(0, items.size());
        printPoints(points);
    }

    auto nemhauser_ullmann(const int &begin, const int &end) {
        // initialize Pareto set
        std::vector<Point> points{{0, 0}, items[begin]};
        // iterate
        for (int i = begin + 1; i < end; ++i) {
            points = merge(points, shiftAllPoints(points, items[i]));
        }
        return points;
    }

    auto merge(const std::vector<Point> &l, const std::vector<Point> &ll) {
        std::vector<Point> E;
        E.reserve(ll.size() * 2);

        U pmax = -1;
        auto pL = l.begin(), pLL = ll.begin();

        while (true) {
            for (; pL != l.end(); ++pL) {
                if (pL->profit > pmax) break;
            }
            for (; pLL != ll.end(); ++pLL) {
                if (pLL->profit > pmax) break;
            }
            if (pL == l.end()) {
                E.insert(E.end(), std::make_move_iterator(pLL), std::make_move_iterator(ll.end()));
                return E;
            }
            if (pLL == ll.end()) {
                E.insert(E.end(), std::make_move_iterator(pL), std::make_move_iterator(l.end()));
                return E;
            }
            // (weight < other.weight) || ((weight == other.weight) && (profit > other.profit));
            if ((*pL) < (*pLL)) {
                E.emplace_back(std::move(*pL));
                pmax = pL->profit;
            } else {
                E.emplace_back(std::move(*pLL));
                pmax = pLL->profit;
            }
        }
    }

    void printPoints(const std::vector<Point> &points) const {
        for (const auto &point : points) {
            std::cout << point.weight << ' ' << point.profit << '\n';
        }
        std::cout << '\n';
    }

    void read() {
        // number of items, threshold
        std::cin >> m_size >> m_threshold;

        // vector of items <weigth, profit>
        items.resize(m_size);

        // read items
        for (int i = 0; i < m_size; ++i) std::cin >> items[i].weight >> items[i].profit;
    }
};

int main() {
    std::ios_base::sync_with_stdio(0);
    std::cin.tie(0);

    Knapsack<int, int> sack;

    sack.read();

    auto start = std::chrono::high_resolution_clock::now();
    sack.parallel_nemhauser_ullmann();
    auto end = std::chrono::high_resolution_clock::now();
    std::cerr << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << '\n';
    return 0;
}
