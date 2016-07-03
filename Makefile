target=s7-3-4.out
sources=$(wildcard *.cpp)
objects=$(patsubst %.cpp, %.o, $(sources))
LIBS=pthread
CXXFLAGS=-Wall -Weffc++ -Wpedantic -std=c++1y $(addprefix -l, $(LIBS))

$(target): $(objects)
	$(CXX) -o $(target) $(objects) $(CXXFLAGS)

.PHONY: clean

clean:
	rm -rf $(target) $(objects)

