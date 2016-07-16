prefix=$(dir $(word 1, $(MAKEFILE_LIST)))
target=$(prefix)final
dirs=$(prefix)src
sources=$(wildcard $(addsuffix /*.cpp, $(dirs)))
dependencies=$(notdir $(patsubst %.cpp, %.dependencies, $(sources)))
objects=$(notdir $(patsubst %.cpp, %.o, $(sources)))
includes=$(addprefix -I, $(dirs))

VPATH+=$(dirs)

CXXFLAGS=-std=c++11 $(includes) -Wall -Wpedantic -Weffc++ -Woverloaded-virtual

all: $(target)

$(target): $(objects)
	$(CXX) -o $(target) $(CXXFALGS) $(objects)

%.dependencies: %.cpp
	@echo ".cpp.d: "$< -\> $@
	$(CXX) -MM $< > $@

include $(dependencies)

.PHONY: clean all test
test:
	echo $(sources)
clean:
	rm -rf $(target) $(objects) $(dependencies)
