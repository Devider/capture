libjpeg.o : src/libjpeg.c
	gcc -c src/libjpeg.c
vcap.o:  src/vcap.c
	gcc -c src/vcap.c
mainform.o:  src/mainform.c
	gcc -c src/mainform.c
soereader.o: src/soereader.c
	gcc -c src/soereader.c
types.o: src/types.h
	gcc -c src/types.h
all: soereader libjpeg.o mainform.o vcap.o soereader.o types.o

clean: 
	rm -f *.o; rm -f soereader 

