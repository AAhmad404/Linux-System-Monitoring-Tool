CC = gcc

CFLAGS = -Wall -Werror -Wextra -std=c99

TARGET = myMonitoringTool

OBJS = main.o sender.o system_data.o system_visualizer.o

## all: compile code with default options.
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

## debug: compile code with debug flag (-g).
debug: $(OBJS)
	$(CC) -g -o $(TARGET) $^

%.o: %.c %.h
	$(CC) $(CCFLAGS) -c $<

## clean: Remove auto-generated files.
clean:
	rm -f $(TARGET) $(OBJS)

help : Makefile
	@sed -n 's/^##//p' $<

.PHONY: help clean run all debug
