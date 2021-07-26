#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

template <typename T, typename U>
class Knapsack {
   private:
    struct Point {
        T weight;
        U profit;

        inline bool operator<(const Point &other) const {
            return (weight < other.weight) || ((weight == other.weight) && (profit > other.profit));
        }
    };
    size_t m_size;
    T m_threshold;
    std::vector<Point> items;

   public:
    Knapsack() {}
    Knapsack(const int size, const T threshold)
        : m_size(size), m_threshold(threshold), items(size) {}

    auto shiftItems(std::vector<Point> copy, const Point &newItem) {
        for (size_t i = 0; i < copy.size(); ++i) {
            copy[i].weight += newItem.weight;
            copy[i].profit += newItem.profit;
        }
        return copy;
    }

    void nemhauserUllmann() {
        // initialize Pareto set
        std::vector<Point> points{{0, 0}, items[0]};
        // iterate
        for (size_t i = 1; i < m_size; ++i) {
            points = merge(points, shiftItems(points, items[i]));
        }
        printPoints(points);
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
        for (const auto &item : points) {
            std::cout << item.weight << ' ' << item.profit << '\n';
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

    Knapsack<int, float> sack;

    sack.read();

    auto start = std::chrono::high_resolution_clock::now();
    sack.nemhauserUllmann();
    auto end = std::chrono::high_resolution_clock::now();
    std::cerr << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << '\n';
    return EXIT_SUCCESS;
}
