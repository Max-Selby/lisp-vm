CC := gcc
CFLAGS := -Wall -Wextra -O2 -I./src
LDFLAGS := -lm

SRC_DIR := src
TEST_DIR := tests
BUILD_DIR := build

# Main
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
TARGET := $(BUILD_DIR)/lvm

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
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(TEST_TARGET): $(TEST_OBJS) $(CORE_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(TEST_OBJS) $(CORE_OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all test clean

