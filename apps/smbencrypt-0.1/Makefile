OBJS=SMBPasswdGen.o main.o md4.o smbencrypt.o
LDFLAGS=-L.
CC=gcc

all: smbencrypt


libdes.a: 
	(cd libdes; make)
	ln -s libdes/libdes.a libdes.a

smbencrypt: libdes.a $(OBJS)
	$(CC) -o smbencrypt $(OBJS) $(LDFLAGS) -ldes

clean:
	/bin/rm -f *.o */*.o

