src = $(wildcard *.c)
obj = $(src:.c=.o)

journalbase: $(obj)
	$(CC) -o $@ $^
