target=openmp_msort.out
sources=$(wildcard *.cpp)
objects=$(patsubst %.cpp, %.o, $(sources))
CXXFLAGS=-std=c++1y -fopenmp -Wall -Wpedantic -Weffc++

$(target): $(objects)
	$(CXX) -o $(target) $(CXXFLAGS) $(objects)

.PHONY: clean
clean:
	rm -rf $(target) $(objects)

