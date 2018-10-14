CFLAGS = -Werror -Wall -ansi

src = $(wildcard *.c)
obj = $(src:.c=.o)

journalbase.out: $(obj)
	$(CC) $(CFLAGS) -o $@ $^
