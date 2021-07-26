CXX	  		:= g++
CXX_FLAGS	:= -Wall -Wextra -g -ggdb -Werror -pedantic -Wshadow -std=c++17 -DNDEBUG -pthread -fopenmp -Ofast
LIBRARIES	:= -lm

all: parallel parallel_merge sequential

parallel: src/parallel.cpp
	$(CXX) $(CXX_FLAGS) $^ -o bin/$@ $(LIBRARIES)

parallel_merge: src/parallel_merge.cpp
	$(CXX) $(CXX_FLAGS) $^ -o bin/$@ $(LIBRARIES)

sequential: src/sequential.cpp
	$(CXX) $(CXX_FLAGS) $^ -o bin/$@ $(LIBRARIES)

knapsack: src/utils/knapsack.cpp
	$(CXX) $(CXX_FLAGS) $^ -o bin/$@ $(LIBRARIES)

performance: src/threading/performance_test.cpp
	$(CXX) $(CXX_FLAGS) $^ -o bin/$@ $(LIBRARIES)
