CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -Wpedantic -g
LDFLAGS :=

SRC_DIR := src
BIN_DIR := bin
OBJ_DIR := obj

TARGET  := $(BIN_DIR)/myshell
SRCS    := $(wildcard $(SRC_DIR)/*.c)
OBJS    := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DEPS    := $(OBJS:.o=.d)

.PHONY: all clean run debug

all: $(TARGET)

$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

-include $(DEPS)

run: $(TARGET)
	./$(TARGET)

debug: CFLAGS += -O0 -fsanitize=address,undefined
debug: LDFLAGS += -fsanitize=address,undefined
debug: clean $(TARGET)

clean:
	$(RM) -r $(BIN_DIR) $(OBJ_DIR)