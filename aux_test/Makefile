sources=$(wildcard *.cpp)
#sources+=../assignment/msg.cpp
sources+=msg.cpp
objects=$(patsubst %.cpp, %.o, $(sources))
CXXFLAGS=-Wpedantic -Wall -Weffc++ -std=c++1y -I../assignment
target=test

$(target): $(objects)
	$(CXX) -o $(target) $(CXXFLAGS) $(objects)

msg.o: ../assignment/msg.cpp
	$(CXX) -c $^

.PHONY: clean

clean:
	rm -rf $(target) $(objects)
