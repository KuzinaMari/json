CC=gcc

all: json_print

json_print: json.o json_print.o
	$(CC) json.o json_print.o -o json_print

json: json.o json_print.o
	$(CC) json.o json_print.o -o json

json.o:
	$(CC) -c json.c

json_print.o:
	$(CC) -c json_print.c

clean:
	rm -rf *.o json

run: clean all
	./json_print ${ARGS}