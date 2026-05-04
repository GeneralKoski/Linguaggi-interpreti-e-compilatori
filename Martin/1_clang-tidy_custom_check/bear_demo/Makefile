CXX := clang++
CXXFLAGS := -std=c++20 -O0 -Wall

SRCS := main.cpp logger.cpp net.cpp util.cpp
OBJS := $(SRCS:.cpp=.o)

app: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) app compile_commands.json

.PHONY: clean
