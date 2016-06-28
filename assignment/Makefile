sources=$(wildcard *.cpp)
objects=$(patsubst %.cpp, %.o, $(sources))
target=assignment_5-6-8
CXXFLAGS=-Wpedantic -Wall -Weffc++ -std=c++1y

$(target): $(objects)
	$(CXX) -o $(target) $(CXXFLAGS) $(objects)

.PHONY: clean

clean:
	rm -rf $(target) $(objects)

