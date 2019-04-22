
CORE_INCS = -I./ -Impi/ -I../

FILE_SO = libdlcrypt.so

#DLBASE_LIB = -Ldlbase -ldlbase
#LUAJIT_LIB = -Lluajit2/src -lluajit
#MY_LIB = $(DLBASE_LIB) $(LUAJIT_LIB)

XLIBS = -shared

MY_CFLAGS = -g3 -O0

export CFLAGS := $(CFLAGS) $(MY_CFLAGS) -fPIC

FILE_O = mpi/gmp.o mpi/bignum.o mpi/bignum_random.o mpi/bignum_next_prime.o dlc_rsa.o\
		 dlc_md5.o dlc_sha1.o dlc_sha2-32.o dlc_sha2-64.o dlc_shau.o dlc_sha-hmac.o  \
		 dlc_random.o dlc_pkcs1.o

all: build

build: $(FILE_SO)

$(FILE_SO): clean $(FILE_O)
	$(CC) -o $@ $(FILE_O) $(XLIBS)
	
	$(CC) main.c $@ ../libdlbase.so $(CFLAGS) -I../ -Impi -I./ -Wl,-rpath=../:./
	
	$(MAKE) -C example

mpi/gmp.o: mpi/gmp.h
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o $@ mpi/gmp.c

mpi/bignum.o: mpi/bignum.h
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o $@ mpi/bignum.c

mpi/bignum_random.o: mpi/bignum.h
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o $@ mpi/bignum_random.c

mpi/bignum_next_prime.o: mpi/bignum.h
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o $@ mpi/bignum_next_prime.c

dlc_rsa.o: dlc_rsa.h
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o $@ dlc_rsa.c


dlc_md5.o: dlc_md5.h
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o $@ dlc_md5.c

dlc_sha1.o: dlc_sha.h
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o $@ dlc_sha1.c

dlc_sha2-32.o: dlc_sha.h
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o $@ dlc_sha2-32.c

dlc_sha2-64.o: dlc_sha.h
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o $@ dlc_sha2-64.c

dlc_shau.o: dlc_sha.h
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o $@ dlc_shau.c

dlc_sha-hmac.o: dlc_sha.h
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o $@ dlc_sha-hmac.c




dlc_random.o: dlc_random.h
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o $@ dlc_random.c

dlc_pkcs1.o: dlc_pkcs1.h
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o $@ dlc_pkcs1.c

clean:
	$(RM) `find ./ -name '*.o'`
	$(RM) $(FILE_SO) a.out
	$(MAKE) $@ -C example
