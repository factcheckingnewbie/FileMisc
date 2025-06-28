CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LDFLAGS = 

# nlohmann/json is in the standard include path
INCLUDES = 

TARGET = json_to_dsl_parser
SOURCES = json_to_dsl_parser.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

json_to_dsl_parser.o: json_to_dsl_parser.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c json_to_dsl_parser.cpp -o json_to_dsl_parser.o

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
