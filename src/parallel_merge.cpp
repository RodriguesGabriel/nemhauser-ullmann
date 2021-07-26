#include <chrono>
#include <future>
#include <iostream>
#include <vector>

template <typename T, typename U>
class Knapsack {
   private:
    struct Point {
        T weight;
        U profit;

        Point(){};
        Point(const T &_weight, const U &_profit) : weight(_weight), profit(_profit){};

        inline bool operator<(const Point &other) const {
            return (weight < other.weight) || ((weight == other.weight) && (profit > other.profit));
        }

        inline Point operator+(const Point &other) const {
            return {weight + other.weight, profit + other.profit};
        }
    };
    size_t m_size;
    T m_threshold;
    std::vector<Point> items;

   public:
    Knapsack() {}
    Knapsack(const int size, const T &threshold)
        : m_size(size), m_threshold(threshold), items(size) {}

    void shiftPoints(std::vector<Point> &points, const Point &newPoint, const int begin,
                     const int &end) {
        for (int i = begin; i < end; ++i) {
            points[i].weight += newPoint.weight;
            points[i].profit += newPoint.profit;
        }
    }

    auto shiftAllPoints(std::vector<Point> copy, const Point &newPoint) {
        shiftPoints(copy, newPoint, 0, copy.size());
        return copy;
    }

    auto bigSetNemhauserUllmann(const std::vector<Point> &pointsA, const std::vector<Point> &pointsB) {
        auto futureA = std::async(std::launch::async, [&]() {
            std::vector<Point> sumPoints{{0, 0}};

            for (size_t i = 0; i < pointsA.size() / 2; ++i) {
                sumPoints = merge(sumPoints, shiftAllPoints(pointsB, pointsA[i]));
            }
            return sumPoints;
        });
        auto futureB = std::async(std::launch::async, [&]() {
            std::vector<Point> sumPoints{{0, 0}};

            for (size_t i = pointsA.size() / 2; i < pointsA.size(); ++i) {
                sumPoints = merge(sumPoints, shiftAllPoints(pointsB, pointsA[i]));
            }
            return sumPoints;
        });
        auto pA = futureA.get();
        auto pB = futureB.get();
        std::vector<Point> sumPoints{{0, 0}};

        for (size_t i = 0; i < pA.size(); ++i) {
            sumPoints = merge(sumPoints, shiftAllPoints(pB, pA[i]));
        }
        return sumPoints;
    }

    auto recursion(const size_t &start, const size_t &end, const int divisions, const int depth) {
        if (depth == divisions) {
            return nemhauserUllmann(start, end);
        }
        auto futureA = std::async(std::launch::async, [&]() {
            return recursion(start, end / 2 + start / 2, divisions, depth + 1);
        });
        auto futureB = std::async(std::launch::async, [&]() {
            return recursion(end / 2 + start / 2, end, divisions, depth + 1);
        });
        return bigSetNemhauserUllmann(futureA.get(), futureB.get());
    }

    void nemhauserUllmann() { printPoints(nemhauserUllmann(0, m_size)); }

    void parallelNemhauserUllmann() { printPoints(recursion(0, m_size, 1, 0)); }

    auto nemhauserUllmann(const int begin, const int end) {
        // initialize Pareto set
        std::vector<Point> points{{0, 0}, std::move(items[begin])};
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
            if (*pL < *pLL) {
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
        for (size_t i = 0; i < m_size; ++i) std::cin >> items[i].weight >> items[i].profit;
    }
};

int main() {
    std::ios_base::sync_with_stdio(0);
    std::cin.tie(0);

    Knapsack<int, int> sack;

    sack.read();

    auto parallelStart = std::chrono::high_resolution_clock::now();
    sack.parallelNemhauserUllmann();
    auto parallelEnd = std::chrono::high_resolution_clock::now();
    std::cerr << "Parallel:\t"
              << std::chrono::duration_cast<std::chrono::milliseconds>(parallelEnd - parallelStart)
                     .count()
              << " ms\n";

    // auto sequentialStart = std::chrono::high_resolution_clock::now();
    // sack.nemhauserUllmann();
    // auto sequentialEnd = std::chrono::high_resolution_clock::now();
    // std::cerr << "Sequential:\t"
    //           << std::chrono::duration_cast<std::chrono::milliseconds>(sequentialEnd -
    //                                                                    sequentialStart)
    //                  .count()
    //           << " ms\n";
    return 0;
}
