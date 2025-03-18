CC = gcc
CFLAGS = -Wall -Wextra -lm

all: infusion_pump

infusion_pump: infusionvuln.c
	$(CC) $(CFLAGS) -o $@ $<

run: infusion_pump
	./infusion_pump

clean:
	rm -f infusion_pump

.PHONY: all clean run