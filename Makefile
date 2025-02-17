CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra
LDFLAGS = -lcurl $(shell pkg-config --libs simdjson)
CPPFLAGS = $(shell pkg-config --cflags simdjson) -I/usr/include/gtest/ -I/opt/homebrew/Cellar/googletest/1.16.0/include
GTEST_FLAGS = -lgtest -lgtest_main -pthread -L/opt/homebrew/Cellar/googletest/1.16.0/lib -L/usr/lib

TARGET = fakerest_test_brightsign
TEST_TARGET = run_tests

SRC_DIR = src
TEST_DIR = test
BUILD_DIR = build

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))
TEST_SRCS = $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJS = $(patsubst $(TEST_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(TEST_SRCS))

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

test: $(TEST_TARGET)

$(TEST_TARGET): $(filter-out $(BUILD_DIR)/main.o,$(OBJS)) $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) $(GTEST_FLAGS) -o $@

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(TEST_TARGET)
