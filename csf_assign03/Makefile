CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic
SRC = main.cpp csimfuncs.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = csim

all: $(TARGET)

csim: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJ) $(TARGET)
