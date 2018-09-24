src = $(wildcard *.c)
obj = $(src:.c=.o)

journalbase.out: $(obj)
	$(CC) -o $@ $^
