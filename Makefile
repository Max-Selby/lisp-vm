CC := gcc
CFLAGS := -Wall -Wextra -O2 -I./src

SRC_DIR := src
BUILD_DIR := build

SRCS := $(SRC_DIR)/main.c $(SRC_DIR)/vm.c $(SRC_DIR)/vmstring.c
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

TARGET := $(BUILD_DIR)/program

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(OBJS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean

