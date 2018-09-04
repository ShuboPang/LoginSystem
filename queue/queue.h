#ifndef _QUEUE_H_
#define _QUEUE_H_

#define MAX_QUEUE_SIZE 256
#define STRING_LENGTH 512

typedef struct
{
	int commandID;
	char command[STRING_LENGTH];
}queueType;

enum status
{
	RET_OK,
	RET_ERROR,
};

typedef queueType ElemType;
typedef enum status Status;

typedef struct queue
{
	ElemType Queue_array[MAX_QUEUE_SIZE];
	int front;
	int rear;
}SqQueue;

SqQueue* init_CirQueue();		//初始化队
Status in_CirQueue(SqQueue*Q,ElemType e);		//入队
Status out_CirQueue(SqQueue*Q,ElemType *e);	//出队
Status is_EmptyQueue(SqQueue*Q);
void uinit_CirQueue(SqQueue *Q);		//销毁队列
//void print_CirQueue(SqQueue *Q);		//打印队列

#endif