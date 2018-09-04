cflags=-lpthread -lsqlite3 -I queue -I login

all:thread_queue_picture thread_picture normal_picture pfun

thread_queue_picture:thread_queue_picture.o queue.o login.o 
	gcc $^ -o $@  $(cflags)

thread_queue_picture.o:thread_queue_picture.c
	gcc  -c $^  $(cflags)
	
thread_picture:thread_picture.o
	gcc $^ -o $@  -lpthread

normal_picture:normal_picture.o
	gcc $^ -o $@ 

pfun:pfun.o
	gcc $^ -o $@

.PHONY:clean
clean:
	rm -rf pfun normal_picture thread_picture thread_queue_picture *.o