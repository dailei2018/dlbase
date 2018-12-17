
CFLAGS = -O0 -g3

build:
	gcc t_queue.c -o queue.out -I ../ -ldlbase $(CFLAGS)
	gcc t_phash.c -o phash.out -I ../ -ldlbase $(CFLAGS)
	gcc t_hash.c -o hash.out -I ../ -ldlbase $(CFLAGS)
	gcc t_bstree.c -o bstree.out -I ../ -ldlbase $(CFLAGS)
	gcc t_rbtree.c -o rbtree.out -I ../ -ldlbase $(CFLAGS)

clean:
	rm -f *.out