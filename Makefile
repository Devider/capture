libjpeg.o : src/libjpeg.c
	gcc -c src/libjpeg.c -O2
vcap.o:  src/vcap.c
	gcc -c src/vcap.c -O2
mainform.o:  src/mainform.c
	gcc `pkg-config --cflags gtk+-3.0` -c src/mainform.c -O2
all: libjpeg.o mainform.o vcap.o 
	gcc -o "soereader"  ./libjpeg.o ./mainform.o ./vcap.o   -lpthread -lgtk-3 -ljpeg `pkg-config --libs gtk+-3.0` -Wall
clean: 
	rm -f *.o; rm -f soereader 

