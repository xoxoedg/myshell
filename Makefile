CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -Wpedantic -g
LDFLAGS :=

TARGET  := myshell
SRCS    := $(wildcard *.c)
OBJS    := $(SRCS:.c=.o)
DEPS    := $(OBJS:.o=.d)

.PHONY: all clean run debug

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)

run: $(TARGET)
	./$(TARGET)

debug: CFLAGS += -O0 -fsanitize=address,undefined
debug: LDFLAGS += -fsanitize=address,undefined
debug: clean $(TARGET)

clean:
	$(RM) $(TARGET) $(OBJS) $(DEPS)
