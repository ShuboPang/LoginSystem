#include"queue.h"
#include<stdio.h>
#include<stdlib.h>

SqQueue* init_CirQueue()//初始化队
{
	SqQueue *s;
	s=(SqQueue*)malloc(sizeof(SqQueue));
	if(!s)
	{
		return NULL;
	}
	s->rear=0;
	s->front=0;
	return s;
}

Status in_CirQueue(SqQueue*Q,ElemType e)//入队
{
	//?full
	if(((Q->rear+1)%MAX_QUEUE_SIZE)==Q->front)
	{
		return RET_ERROR;
	}
	Q->Queue_array[Q->rear]=e;
	Q->rear=(Q->rear+1)%MAX_QUEUE_SIZE;
	return RET_OK;
}

Status out_CirQueue(SqQueue*Q,ElemType *e)//出队
{
	//?empty
	if(Q->front==Q->rear)
	{
		return RET_ERROR;
	}
	*e=Q->Queue_array[Q->front];
	Q->front=(Q->front+1)%MAX_QUEUE_SIZE;
	return RET_OK;	
}

/*
void print_CirQueue(SqQueue *Q)//打印队列
{
	int i;
	if(Q->front==Q->rear)
	{
		printf("queue is empty!!!\n");
		return ;
	}
	for(i=Q->front;i!=Q->rear;i=(i+1)%MAX_QUEUE_SIZE)
	{
		printf("%d   ",Q->Queue_array[i]);
	}
	printf("\n");
	
	for(i=0;i<MAX_QUEUE_SIZE;i++)
		{
		printf("%d   ",Q->Queue_array[i]);
		}
	
	printf("\n");
	printf("front=%d  rear=%d\n",Q->front,Q->rear);
}
*/
Status is_EmptyQueue(SqQueue*Q)
{
	if(Q->front==Q->rear)
	{
		return RET_ERROR;
	}
	return RET_OK;
}

void uinit_CirQueue(SqQueue *Q)//销毁队列
{
	Q->rear=Q->front=0;
	free(Q);
}



