default: build

build: mdc.c
	clear
	gcc mdc.c -lraylib -lgdi32 -lwinmm -o mdc
	./mdc
	make clean

test: mdc_test.c
	gcc mdc_test.c -o mdc_test
	./mdc_test

clean:
	$(RM) *.exe
