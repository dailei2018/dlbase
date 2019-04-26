
CORE_INCS = -I./ -Impi/ -Itls -I../

FILE_SO = libdlcrypt.so

#DLBASE_LIB = -Ldlbase -ldlbase
#LUAJIT_LIB = -Lluajit2/src -lluajit
#MY_LIB = $(DLBASE_LIB) $(LUAJIT_LIB)

XLIBS = -shared

MY_CFLAGS = -g3 -O0

export CFLAGS := $(CFLAGS) $(MY_CFLAGS) -fPIC

FILE_O = mpi/gmp.o mpi/bignum.o mpi/bignum_random.o mpi/bignum_next_prime.o dlc_rsa.o\
		 dlc_md5.o dlc_sha1.o dlc_sha2-32.o dlc_sha2-64.o dlc_shau.o dlc_sha-hmac.o  \
		 dlc_random.o dlc_pkcs1.o dlc_rsa_signature.o dlc_des.o dlc_aes_internal.o \
		 dlc_aes.o dlc_des_amd64.o \
		 tls/dlc_x509_cert.o tls/dlc_asn1.o tls/dlc_x509.o tls/dlc_oid.o tls/dlc_pk.o \
		 

all: build

build: $(FILE_SO)

$(FILE_SO): clean $(FILE_O)
	$(CC) -o $@ $(FILE_O) $(XLIBS)
	
	$(CC) main.c $@ ../libdlbase.so $(CFLAGS) $(CORE_INCS) -Wl,-rpath=../:./
	
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



dlc_random.o: dlc_random.h
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o $@ dlc_random.c

dlc_pkcs1.o: dlc_pkcs1.h
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o $@ dlc_pkcs1.c

dlc_rsa_signature.o:
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o $@ dlc_rsa_signature.c


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


dlc_des.o: dlc_des.h dlc_des_internal.h
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o $@ dlc_des.c
dlc_des_amd64.o:
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o $@ dlc_des_amd64.S


dlc_aes_internal.o: dlc_aes.h dlc_aes_internal.h
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o $@ dlc_aes_internal.c
dlc_aes.o: dlc_aes.h dlc_aes_internal.h
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o $@ dlc_aes.c


tls/dlc_x509_cert.o: tls/dlc_x509.h
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o $@ tls/dlc_x509_cert.c
tls/dlc_asn1.o: tls/dlc_asn1.h
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o $@ tls/dlc_asn1.c
tls/dlc_x509.o: tls/dlc_x509.h
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o $@ tls/dlc_x509.c
tls/dlc_oid.o: tls/dlc_oid.h
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o $@ tls/dlc_oid.c
tls/dlc_pk.o: tls/dlc_pk.h
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o $@ tls/dlc_pk.c

clean:
	$(RM) `find ./ -name '*.o'`
	$(RM) $(FILE_SO) a.out
	$(MAKE) $@ -C example
