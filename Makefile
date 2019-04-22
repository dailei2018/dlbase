CC =	cc
CFLAGS =  -pipe  -O0 -g3 -fPIC #-DDL_MEM_DEBUG

MY_CFLAGS = -I re -I math -I json -I ./ -I./dlcrypt

XLIBS = -shared -lpcre2-8

LINK =	$(CC)

CORE_INCS =

FILE_SO = libdlbase.so
MPI_SO = dlcrypt/libdlcrypt.so

build: binary

binary:	$(FILE_SO) $(MPI_SO)

$(MPI_SO):
	$(MAKE) -C dlcrypt

$(FILE_SO):	dl_file.o dl_log.o dl_pool.o dl_string.o dl_array.o dl_list.o dl_queue.o \
				dl_phash.o dl_hash.o dl_bstree.o dl_rbtree.o dl_shm.o dl_buf.o dl_inet.o \
				dl_time.o dl_table.o re/dl_pcre2.o math/dl_math.o \
				json/dl_json.o
				
				
	
	$(LINK) -o $(FILE_SO) dl_file.o dl_log.o dl_pool.o dl_string.o dl_array.o dl_list.o \
			   dl_queue.o dl_phash.o dl_hash.o dl_bstree.o dl_rbtree.o dl_shm.o dl_buf.o \
			   dl_inet.o dl_time.o dl_table.o re/dl_pcre2.o math/dl_math.o json/dl_json.o \
			   $(XLIBS)
	
	#rm -f `find ./ -name '*.o'`
	#cp libdlbase.so /usr/lib/x86_64-linux-gnu/
	#cp *.h re/*.h algo/*.h algo/rsa/*.h /usr/include/dlbase/

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

dl_time.o:
	$(CC) -c $(CFLAGS) $(MY_CFLAGS) $(CORE_INCS) -o dl_time.o dl_time.c

dl_table.o:
	$(CC) -c $(CFLAGS) $(MY_CFLAGS) $(CORE_INCS) -o dl_table.o dl_table.c

re/dl_pcre2.o:
	$(CC) -c $(CFLAGS) $(MY_CFLAGS) $(CORE_INCS) -o re/dl_pcre2.o re/dl_pcre2.c

math/dl_math.o:
	$(CC) -c $(CFLAGS) $(MY_CFLAGS) $(CORE_INCS) -o math/dl_math.o math/dl_math.c


json/dl_json.o:
	$(CC) -c $(CFLAGS) $(MY_CFLAGS) $(CORE_INCS) -o json/dl_json.o json/dl_json.c

clean:
	$(MAKE) -C dlcrypt $@
	$(RM) `find ./ -name '*.o'`
	$(RM) $(FILE_SO)





