
CORE_INCS = -I./ -I../mpi -I../tls -I../ -I../../

FILE_SO = libdlcrypt.so

#DLBASE_LIB = -Ldlbase -ldlbase
#LUAJIT_LIB = -Lluajit2/src -lluajit
#MY_LIB = $(DLBASE_LIB) $(LUAJIT_LIB)

XLIBS =

MY_CFLAGS = -g3 -O0

export CFLAGS := $(CFLAGS) $(MY_CFLAGS)

all: build

build:
	$(CC) rsa_keygen.c -o rsa_keygen ../$(FILE_SO) -ldlbase $(CFLAGS) $(CORE_INCS) -Wl,-rpath=../:../../
	$(CC) rsa_sig_verify.c -o rsa_sig_verify ../$(FILE_SO) -ldlbase $(CFLAGS) $(CORE_INCS) -Wl,-rpath=../:../../
	$(CC) aes.c -o aes ../$(FILE_SO) ../../libdlbase.so $(CFLAGS) $(CORE_INCS) -Wl,-rpath=../:../../
	$(CC) des.c -o des ../$(FILE_SO) ../../libdlbase.so $(CFLAGS) $(CORE_INCS) -Wl,-rpath=../:../../
	$(CC) x509.c -o x509 ../$(FILE_SO) ../../libdlbase.so $(CFLAGS) $(CORE_INCS) -Wl,-rpath=../:../../

clean:
	$(RM) rsa_keygen rsa_sig_verify aes des x509
