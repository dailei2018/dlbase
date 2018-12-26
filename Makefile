CC =	cc
CFLAGS =  -pipe  -O0 -g3 -fPIC #-DDL_MEM_DEBUG
MY_CFLAGS = 

XLIBS = -shared

LINK =	$(CC)

CORE_INCS =


build: binary

binary:	libdlbase.so

libdlbase.so:	dl_file.o dl_log.o dl_pool.o dl_string.o dl_array.o dl_list.o dl_queue.o \
				dl_phash.o dl_hash.o dl_bstree.o dl_rbtree.o dl_shm.o dl_buf.o dl_inet.o
	
	$(LINK) -o libdlbase.so dl_file.o dl_log.o dl_pool.o dl_string.o dl_array.o dl_list.o \
			   dl_queue.o dl_phash.o dl_hash.o dl_bstree.o dl_rbtree.o dl_shm.o dl_buf.o \
			   dl_inet.o \
			   $(XLIBS)	
	
	rm -f `find ./ -name '*.o'`
	cp libdlbase.so /usr/lib/x86_64-linux-gnu/
	cp *.h /usr/include/dlbase/

dl_file.o:
	$(CC) -c $(CFLAGS) $(MY_CFLAGS) $(CORE_INCS) -o dl_file.o dl_file.c

dl_log.o:
	$(CC) -c $(CFLAGS) $(MY_CFLAGS) $(CORE_INCS) -o dl_log.o dl_log.c

dl_pool.o:
	$(CC) -c $(CFLAGS) $(MY_CFLAGS) $(CORE_INCS) -o dl_pool.o dl_pool.c

dl_string.o:
	$(CC) -c $(CFLAGS) $(MY_CFLAGS) $(CORE_INCS) -o dl_string.o dl_string.c

dl_array.o:
	$(CC) -c $(CFLAGS) $(MY_CFLAGS) $(CORE_INCS) -o dl_array.o dl_array.c

dl_list.o:
	$(CC) -c $(CFLAGS) $(MY_CFLAGS) $(CORE_INCS) -o dl_list.o dl_list.c

dl_queue.o:
	$(CC) -c $(CFLAGS) $(MY_CFLAGS) $(CORE_INCS) -o dl_queue.o dl_queue.c

dl_phash.o:
	$(CC) -c $(CFLAGS) $(MY_CFLAGS) $(CORE_INCS) -o dl_phash.o dl_phash.c

dl_hash.o:
	$(CC) -c $(CFLAGS) $(MY_CFLAGS) $(CORE_INCS) -o dl_hash.o dl_hash.c

dl_bstree.o:
	$(CC) -c $(CFLAGS) $(MY_CFLAGS) $(CORE_INCS) -o dl_bstree.o dl_bstree.c

dl_rbtree.o:
	$(CC) -c $(CFLAGS) $(MY_CFLAGS) $(CORE_INCS) -o dl_rbtree.o dl_rbtree.c

dl_shm.o:
	$(CC) -c $(CFLAGS) $(MY_CFLAGS) $(CORE_INCS) -o dl_shm.o dl_shm.c

dl_buf.o:
	$(CC) -c $(CFLAGS) $(MY_CFLAGS) $(CORE_INCS) -o dl_buf.o dl_buf.c

dl_inet.o:
	$(CC) -c $(CFLAGS) $(MY_CFLAGS) $(CORE_INCS) -o dl_inet.o dl_inet.c

clean:
	rm -f `find ./ -name '*.o'`





