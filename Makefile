
MY_CFLAGS := -pipe  -O0 -g3 -fPIC -I./ -Ijson -Icrypt

CFLAGS := $(MY_CFLAGS) $(CFLAGS)

FILE_O = 	dl_pool.o dl_log.o dl_string.o dl_array.o dl_list.o dl_re.o dl_bstree.o dl_hash.o dl_phash.o \
			dl_queue.o dl_rbtree.o dl_table.o dl_time.o dl_shm.o dl_fifo.o \
			json/dl_json.o crypt/dl_sum.o \
			crypt/dl_aes.o
			
FILE_SO = libdlbase.so

XLIBS := -shared -lssl -lpcre2-8

build: $(FILE_SO) test

$(FILE_SO): $(FILE_O)
	$(CC) -o $(FILE_SO) $(FILE_O) $(XLIBS)

$(FILE_SO):	$(FILE_O)

dl_pool.o: dl_pool.c dl_pool.h dl_log.h
dl_log.o: dl_log.c dl_log.h dl_string.h
dl_string.o: dl_string.c dl_string.h
dl_array.o: dl_array.c dl_array.h dl_pool.h
dl_list.o: dl_list.c dl_list.h dl_pool.h
dl_re.o: dl_re.c dl_re.h dl_pool.h dl_list.h dl_string.h
dl_bstree.o: dl_bstree.c dl_bstree.h
dl_hash.o: dl_hash.c dl_hash.h dl_pool.h dl_string.h dl_log.h
dl_phash.o: dl_phash.c dl_phash.h dl_pool.h dl_string.h
dl_queue.o: dl_queue.c dl_queue.h
dl_fifo.o: dl_fifo.c dl_fifo.h
dl_rbtree.o: dl_rbtree.c dl_rbtree.h
dl_table.o: dl_table.c dl_table.h dl_string.h dl_log.h
dl_time.o: dl_time.c dl_time.h
dl_shm.o: dl_shm.c dl_shm.h dl_log.h
json/dl_json.o: json/dl_json.c json/dl_json.h
crypt/dl_sum.o: crypt/dl_sum.c crypt/dl_sum.h
	$(CC) -c -o $@ crypt/dl_sum.c $(CFLAGS)
crypt/dl_aes.o: crypt/dl_aes.c crypt/dl_aes.h dl_string.h dl_pool.h
	$(CC) -c -o $@ crypt/dl_aes.c $(CFLAGS)

test: $(FILE_SO)
	$(MAKE) -C example

clean:
	$(RM) $(FILE_O)
	$(RM) $(FILE_SO)
	$(MAKE) -C example $@




