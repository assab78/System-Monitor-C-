CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O2 `pkg-config --cflags gtk+-3.0 libnotify`
LDFLAGS = `pkg-config --libs gtk+-3.0 libnotify`

SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

# Source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRC_FILES))

# Executable name
APP_NAME = system-monitor

.PHONY: all clean dirs

all: dirs $(BIN_DIR)/$(APP_NAME)

# Create necessary directories
dirs:
	mkdir -p $(BUILD_DIR) $(BIN_DIR)

# Link object files to create executable
$(BIN_DIR)/$(APP_NAME): $(OBJ_FILES)
	$(CXX) -o $@ $^ $(LDFLAGS)

# Compile source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

run: all
	$(BIN_DIR)/$(APP_NAME)
