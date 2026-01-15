CC := gcc
CFLAGS := -Wall -Wextra -O2 -I./src

SRC_DIR := src
TEST_DIR := tests
BUILD_DIR := build

# Main
SRCS := $(SRC_DIR)/main.c $(SRC_DIR)/vm.c $(SRC_DIR)/vmstring.c
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
TARGET := $(BUILD_DIR)/program

# Tests
TEST_SRCS := $(wildcard $(TEST_DIR)/*.c)
TEST_OBJS := $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/%.o,$(TEST_SRCS))
TEST_TARGET := $(BUILD_DIR)/tests

# Source objects excluding main.c
CORE_SRCS := $(filter-out $(SRC_DIR)/main.c,$(SRCS))
CORE_OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(CORE_SRCS))

# == Rules ==
all: $(TARGET)

test: $(TEST_TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(OBJS) -o $@

$(TEST_TARGET): $(TEST_OBJS) $(CORE_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(TEST_OBJS) $(CORE_OBJS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all test clean

