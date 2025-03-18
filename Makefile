CC = gcc
CFLAGS = -Wall -Wextra

all: infusion_pump test_overflow

infusion_pump: infusionvuln.c
	$(CC) $(CFLAGS) -o $@ $<

test_overflow: test_overflow.c
	$(CC) $(CFLAGS) -o $@ $<

test: test_overflow
	./test_overflow

run: infusion_pump
	./infusion_pump

clean:
	rm -f infusion_pump test_overflow

.PHONY: all clean test run