
MY_CFLAGS := -O0 -g3 -I./ -I../ -I../json -I../crypt -L../ -ldlbase -Wl,-rpath=../

CFLAGS := $(MY_CFLAGS) $(CFLAGS)

build:
	$(CC) aes.c -o aes $(MY_CFLAGS)
	$(CC) table.c -o table $(MY_CFLAGS)
	$(CC) fifo.c -o fifo $(MY_CFLAGS) -lpthread
	$(CC) fifo_2.c -o fifo_2 $(MY_CFLAGS) -lpthread

clean:
	$(RM) aes table fifo fifo_2





