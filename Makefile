CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -Wpedantic -g
LDFLAGS :=

SRC_DIR  := src
TEST_DIR := test
BIN_DIR  := bin
OBJ_DIR  := obj

TARGET   := $(BIN_DIR)/myshell
SRCS     := $(wildcard $(SRC_DIR)/*.c)
OBJS     := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DEPS     := $(OBJS:.o=.d)

TEST_SRCS := $(wildcard $(TEST_DIR)/test_*.c)
TEST_BINS := $(TEST_SRCS:$(TEST_DIR)/test_%.c=$(BIN_DIR)/test_%)

.PHONY: all clean run debug test

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

test: $(TEST_BINS)
	@for t in $(TEST_BINS); do \
		echo "-- Running $$t --"; \
		./$$t || exit 1; \
	done

$(BIN_DIR)/test_%: $(TEST_DIR)/test_%.c $(OBJ_DIR)/%.o | $(BIN_DIR)
	$(CC) $(CFLAGS) -I. $^ -o $@

debug: CFLAGS += -O0 -fsanitize=address,undefined
debug: LDFLAGS += -fsanitize=address,undefined
debug: clean $(TARGET)

clean:
	$(RM) -r $(BIN_DIR) $(OBJ_DIR)