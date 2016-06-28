target=test
sources=$(wildcard *.cpp)
objects=$(patsubst %.cpp, %.o, $(sources))

CXXFLAGS := -Wall -Wpedantic -Weffc++ -std=c++1y
LIBS := rt

$(target): $(objects)
	$(CXX) -o $(target) $(CXXFLAGS) $(objects) $(addprefix -l, $(LIBS))

.PHONY: clean

clean:
	rm -rf $(target) $(objects)

