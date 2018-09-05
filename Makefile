cflags=-lpthread -lsqlite3 -I queue -I login

all:thread_queue_picture thread_picture normal_picture pfun

thread_queue_picture:thread_queue_picture.o queue.o login.o 
	gcc $^ -o $@  $(cflags)

thread_queue_picture.o:thread_queue_picture.c
	gcc  -c $^  $(cflags)
	
login.o:login/login.c queue.o
	gcc -c $^ $(cflags)

queue.o:queue/queue.c
	gcc -c $^

.PHONY:clean
clean:
	rm -rf  thread_queue_picture *.o