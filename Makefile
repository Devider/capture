console.o : src/console.c
	gcc -c src/console.c -O2
common.o : src/common.c
	gcc -c src/common.c -O2
vcap.o:  src/vcap.c
	gcc -c src/vcap.c -O2
main.o : src/main.c
	gcc -c src/main.c -O2
window.o:  src/window.c
	gcc `pkg-config --cflags gtk+-3.0` -c src/window.c -O2
all: main.o window.o vcap.o common.o console.o
	gcc -o "soereader"  ./console.o ./window.o ./vcap.o ./main.o ./common.o   -lpthread -lgtk-3 -ljpeg `pkg-config --libs gtk+-3.0` -Wall
clean: 
	rm -f *.o; rm -f soereader
	rm *.jpg 

