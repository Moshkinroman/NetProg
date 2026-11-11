CXX = g++
CXXFLAGS = -std=c++11 -Wall 

TARGET = daytime_client
SRC = daytime_client.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: all clean
