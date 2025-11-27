do_so:
	gcc -shared -fPIC -I./include -o src/libown1.so src/lib1.c -lm
	gcc -shared -fPIC -I./include -o src/libown2.so src/lib2.c -lm

comp_stat:
	gcc -I./include example_static.c -L./src -lown1 -Wl,-rpath,./src -o example_static -lm

comp_dyn:
	gcc -I./include example_dynamic.c -ldl -o example_dynamic

clean:
	rm -f src/*.so example_static example_dynamic

all: do_so comp_stat comp_dyn