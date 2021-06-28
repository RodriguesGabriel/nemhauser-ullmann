CXX	  := g++
CXX_FLAGS := -Wall -Wextra -g -ggdb -Werror -pedantic -Wshadow -std=c++17 -DNDEBUG -O3 -pthread -fopenmp
#  -fopenmp
LIBRARIES := -lm

all: main

main: src/main.cpp
	$(CXX) $(CXX_FLAGS) $^ -o bin/$@ $(LIBRARIES)

initial: src/initial.cpp
	$(CXX) $(CXX_FLAGS) $^ -o bin/$@ $(LIBRARIES)

knapsack: src/utils/knapsack.cpp
	$(CXX) $(CXX_FLAGS) $^ -o bin/$@ $(LIBRARIES)

performance: src/threading/performance_test.cpp
	$(CXX) $(CXX_FLAGS) $^ -o bin/$@ $(LIBRARIES)
