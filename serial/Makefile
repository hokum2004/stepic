target=serialmsort.out
sources=$(wildcard *.cpp)
objects=$(patsubst %.cpp,%.o)
CXXFLAGS=-std=c++1y -Weffc++ -Wall -Wpedantic

$(target): $(objects)
	$(CXX) -o $(target) $(CXXFLAGS) $(objects)

.PHONY: clean

clean:
	rm -rf $(target) $(source)
