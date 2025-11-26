CC = gcc
CFLAGS = -Wall -fPIC -Iinclude
LDFLAGS = -ldl

all: libs programs

libs: libpi_leibniz.so libpi_wallis.so

libpi_leibniz.so: src/pi_leibniz.c include/pi_lib.h
	$(CC) $(CFLAGS) -c src/pi_leibniz.c -o pi_leibniz.o
	$(CC) -shared -o libpi_leibniz.so pi_leibniz.o

libpi_wallis.so: src/pi_wallis.c include/pi_lib.h
	$(CC) $(CFLAGS) -c src/pi_wallis.c -o pi_wallis.o
	$(CC) -shared -o libpi_wallis.so pi_wallis.o

programs: test_static test_dynamic

test_static: test_static.c include/pi_lib.h libpi_leibniz.so
	$(CC) test_static.c -L. -lpi_leibniz -o test_static $(LDFLAGS)

test_dynamic: test_dynamic.c include/pi_lib.h
	$(CC) test_dynamic.c -o test_dynamic $(LDFLAGS)

run:
	LD_LIBRARY_PATH=. ./test_static
	LD_LIBRARY_PATH=. ./test_dynamic

clean:
	rm -f *.o *.so test_static test_dynamic
	@echo "Очищено"

.PHONY: all libs programs run clean
