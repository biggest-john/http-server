# I still have not learnt how to actually use the Makefile to track header dependency and integrate it into the compiler process
CC		:= gcc
CFLAGS	:= -Wall -Wextra -Werror -g -Iinclude

SRCDIR	:= src
INCDIR	:= include
BINDIR	:= bin
OBJDIR	:= obj

TARGET	:= $(BINDIR)/server

SRCS	:= $(wildcard $(SRCDIR)/*.c)
OBJS	:= $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

.PHONY: all debug clean memcheck

all: $(TARGET)

$(TARGET): $(OBJS) | $(BINDIR)
		@echo "Linking executable: $@"
		$(CC) $(CFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
		@echo "Compiling Source file: $@"
		$(CC) $(CFLAGS) -c $^ -o $@

$(BINDIR) $(OBJDIR):
		mkdir -p $@

clean:
		rm -rf $(OBJDIR) $(BINDIR)

debug: CFLAGS += -O0
debug: clean all
		@echo "Starting debugging session..."
		gdb $(TARGET)

memcheck: all
		@echo "Starting Valgrind analysis..."
		valgrind --leak-check=full \
				 --show-leak-kinds=all \
				 --track-origins=yes \
				 $(TARGET)