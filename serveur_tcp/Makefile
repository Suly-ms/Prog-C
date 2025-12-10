CC = gcc
CFLAGS = -Wall -Wextra -g
SRC_DIR = .
OUT_DIR = out

SRCS = $(wildcard $(SRC_DIR)/*.c)
EXECS = $(patsubst $(SRC_DIR)/%.c, $(OUT_DIR)/%, $(SRCS))

all: $(EXECS)

$(OUT_DIR)/%: $(SRC_DIR)/%.c | $(OUT_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

clean:
	rm -rf $(OUT_DIR)

re: clean all

.PHONY: all clean re
