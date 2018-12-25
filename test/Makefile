
CFLAGS = -O0 -g3

build:
	gcc t_queue.c -o queue.out -I ../ -ldlbase $(CFLAGS)
	gcc t_phash.c -o phash.out -I ../ -ldlbase $(CFLAGS)
	gcc t_hash.c -o hash.out -I ../ -ldlbase $(CFLAGS)
	gcc t_bstree.c -o bstree.out -I ../ -ldlbase $(CFLAGS)
	gcc t_rbtree.c -o rbtree.out -I ../ -ldlbase $(CFLAGS)
	gcc t_str.c -o t_str.out -I ../ -ldlbase $(CFLAGS)
	gcc t_shm.c -o t_shm.out -I ../ -ldlbase $(CFLAGS)
	gcc t_log.c -o t_log.out -I ../ -ldlbase $(CFLAGS)
	
	gcc t_bstree_tmp.c -o a.out -I ../ -ldlbase $(CFLAGS)

clean:
	rm -f *.out