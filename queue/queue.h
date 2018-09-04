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

SqQueue* init_CirQueue();		//��ʼ����
Status in_CirQueue(SqQueue*Q,ElemType e);		//���
Status out_CirQueue(SqQueue*Q,ElemType *e);	//����
Status is_EmptyQueue(SqQueue*Q);
void uinit_CirQueue(SqQueue *Q);		//���ٶ���
//void print_CirQueue(SqQueue *Q);		//��ӡ����

#endif