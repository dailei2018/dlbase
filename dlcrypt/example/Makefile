
CORE_INCS = -I./ -I../mpi -I../ -I../../

FILE_SO = libdlcrypt.so

#DLBASE_LIB = -Ldlbase -ldlbase
#LUAJIT_LIB = -Lluajit2/src -lluajit
#MY_LIB = $(DLBASE_LIB) $(LUAJIT_LIB)

XLIBS =

MY_CFLAGS = -g3 -O0

export CFLAGS := $(CFLAGS) $(MY_CFLAGS)

FILE_O = mpi/gmp.o mpi/bignum.o mpi/bignum_random.o mpi/bignum_next_prime.o dlc_rsa.o dlc_md5.o dlc_sha1.o \
		 dlc_sha256.o dlc_random.o

all: build

build:
	$(CC) rsa_keygen.c -o rsa_keygen ../$(FILE_SO) -ldlbase $(CFLAGS) $(CORE_INCS) -Wl,-rpath=../:../../
	


clean:
	$(RM) rsa_keygen
