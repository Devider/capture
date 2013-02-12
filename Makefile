libjpeg.o : src/libjpeg.c
	gcc -c src/libjpeg.c -O2
vcap.o:  src/vcap.c
	gcc -c src/vcap.c -O2
mainform.o:  src/mainform.c
	gcc `pkg-config --cflags gtk+-3.0` -c src/mainform.c -O2
soereader.o:  src/soereader.c
	gcc -c  src/soereader.c -O2
all: libjpeg.o mainform.o vcap.o soereader.o 
	gcc -o "soereader"  ./libjpeg.o ./mainform.o ./soereader.o ./vcap.o   -lpthread -lgtk-3 -ljpeg -Wall
clean: 
	rm -f *.o; rm -f soereader 

