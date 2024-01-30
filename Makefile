src = ./src/main.c
obj = ./main
flags = -Wall -lm -lncurses -std=c99
compiler = gcc
editor = nvim

build:
	$(compiler) $(src) -o $(obj) $(flags)

run:
	$(obj)

clean:
	rm -f $(obj)

code:
	$(editor) $(src)
