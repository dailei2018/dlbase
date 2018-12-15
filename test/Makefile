
CFLAGS = -O0 -g3

build:
	gcc t_queue.c -o queue -I ../ -ldlbase $(CFLAGS)
	gcc t_phash.c -o phash -I ../ -ldlbase $(CFLAGS)
	gcc t_hash.c -o hash -I ../ -ldlbase $(CFLAGS)

clean:
	rm -f queue
	rm -f phash
	rm -f hash