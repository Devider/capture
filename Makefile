libjpeg.o : src/libjpeg.c
	gcc -c src/libjpeg.c
vcap.o:  src/vcap.c
	gcc -I/usr/include/gtk-3.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/glib-2.0  -I/usr/include/pango-1.0 -I/usr/include/cairo -I/usr/include/gdk-pixbuf-2.0  -I/usr/include/atk-1.0 -c src/vcap.c
mainform.o:  src/mainform.c
	gcc -I/usr/include/cairo -I/usr/include/gtk-3.0 -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/pango-1.0 -I/usr/include/glib-2.0 -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng12 -I/usr/include/atk-1.0 -I/usr/include/gio-unix-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -c src/mainform.c
soereader.o:  src/soereader.c
	gcc -c  src/soereader.c 
all: libjpeg.o mainform.o vcap.o soereader.o 
	gcc  -o "soereader"  ./libjpeg.o ./mainform.o ./soereader.o ./vcap.o   -lpthread -lgtk-3 -ljpeg -Wall
clean: 
	rm -f *.o; rm -f soereader 

