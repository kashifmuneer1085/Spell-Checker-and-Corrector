CXX      = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra
TARGET   = spellchecker
SRC      = src/main.cpp

.PHONY: all clean run demo

all:
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)
	@echo "Build successful → ./$(TARGET)"

# Run with the bundled dictionary
run: all
	./$(TARGET) dictionary.txt

# Quick demo (no dictionary file needed)
demo: all
	echo "demo" | ./$(TARGET)

clean:
	rm -f $(TARGET)
