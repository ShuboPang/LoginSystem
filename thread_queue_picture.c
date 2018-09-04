#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include "queue.h"
#include "login.h"

#define MAX_SIZE 256

typedef struct tagBmpFileHeader //文件头  
{  
    //unsigned short bfType;      //标识该文件为bmp文件,判断文件是否为bmp文件，即用该值与"0x4d42"比较是否相等即可，0x4d42 = 19778  
    unsigned long  bfSize;      //文件大小  
    unsigned short bfReserved1; //预保留位  
    unsigned short bfReserved2; //预保留位  
    unsigned long  bfOffBits;   //图像数据区的起始位置  
}BmpFileHeader;//14字节  
typedef struct tagBmpInfoHeader //信息头  
{  
    unsigned long  biSize;  //图像数据块大小  
    long     biWidth;   //宽度  
    long     biHeight;  //高度  
    unsigned short biPlanes;//为1  
    unsigned short biBitCount; //像素位数，8-灰度图；24-真彩色  
    unsigned long biCompression;//压缩方式  
    unsigned long biSizeImage;  //图像区数据大小  
    long     biXPelsPerMeter;  //水平分辨率，像素每米  
    long     biYPelsPerMeter;  
    unsigned long biClrUsed;   //位图实际用到的颜色数  
    unsigned short biClrImportant;//位图显示过程，重要的颜色数；0--所有都重要  
}BmpInfoHeader;//40字节  

typedef struct
{
	unsigned char blue;
	unsigned char green;
	unsigned char red;
}Pixel;

typedef struct
{
	int fd;
	char fileName[MAX_SIZE];
	int width;
	int height;
	//file head
	unsigned short bfType;
	BmpFileHeader file_head;
	//info head
	BmpInfoHeader info_head;
	//command string
	//char command[MAX_SIZE];
}FileInfo;

enum sysCommand
{
	command_circl,
	command_rect,
	command_updown,
	command_rightleft,
	command_turnright,
	command_turnleft,
	command_turnover,
	command_turnV,
	command_turnH,
	command_about,
	command_max
};

typedef struct
{
	enum sysCommand commandNo;
	char commandStr[MAX_SIZE];
	void (*p)(FileInfo* pFileInfo,
		char pCommandList[MAX_SIZE][MAX_SIZE]);
}SystemCommand;

const Pixel p_white={255,255,255};
SqQueue *g_pQueue;

void Command_Circl(FileInfo* pFileInfo,char pCommandList[MAX_SIZE][MAX_SIZE])
{
	int i,j;
	int x,y,r;
	Pixel di;
	
	pFileInfo->fd=open(pFileInfo->fileName,O_RDWR,0777);
	if(-1==pFileInfo->fd)
	{
		printf("%s open error!\n",pFileInfo->fileName);
		return;
	}
	lseek(pFileInfo->fd,54,SEEK_SET);
	x=atoi(pCommandList[1]);
	y=atoi(pCommandList[2]);
	r=atoi(pCommandList[3]);
	di.blue=atoi(pCommandList[4]);
	di.green=atoi(pCommandList[5]);
	di.red=atoi(pCommandList[6]);
	for(i=0;i<pFileInfo->height;i++)
	{
		for(j=0;j<pFileInfo->width;j++)
		{
			if((j-x)*(j-x)+(i-(pFileInfo->height-y))*(i-(pFileInfo->height-y))<r*r)
			{
				write(pFileInfo->fd,&di,sizeof(di));	
			}
			else
			{
				lseek(pFileInfo->fd,sizeof(Pixel),SEEK_CUR);
			}
		}
	}	
}

void Command_Rect(FileInfo* pFileInfo,char pCommandList[MAX_SIZE][MAX_SIZE])
{
     int i,j;
     int x,y,w,h;
	 Pixel di;
     pFileInfo->fd=open(pFileInfo->fileName,O_RDWR,0777);
	if(-1==pFileInfo->fd)
	{
		printf("%s open error!\n",pFileInfo->fileName);
		return;
	}
	lseek(pFileInfo->fd,54,SEEK_SET);
	x=atoi(pCommandList[1]);
	y=atoi(pCommandList[2]);
	w=atoi(pCommandList[3]);
	h=atoi(pCommandList[4]);
	di.blue=atoi(pCommandList[5]);
	di.green=atoi(pCommandList[6]);
	di.red=atoi(pCommandList[7]);
	
	for(i=0;i<pFileInfo->height;i++)
	{
		for(j=0;j<pFileInfo->width;j++)
		{
			if(i>=(pFileInfo->height-y-h) && i<=(pFileInfo->height-y) && j>=x && j<=(x+w))
			{
			  write(pFileInfo->fd,&di,sizeof(di));
			}
		  	else
			{
			  lseek(pFileInfo->fd,sizeof(Pixel),SEEK_CUR);
		  	}
		}
	}	
}

void Command_About(FileInfo* pFileInfo, char pCommandList[MAX_SIZE][MAX_SIZE])
{
	printf("\t	<system Version v1.0.0.0>\n");
	printf("\t	<copyright: Oliver>\n");
	printf("\t	<adress: JiLin City>\n");
	printMessage(0);
}


const SystemCommand sys_Command[]={
	{command_circl,"circl",Command_Circl},
	{command_rect,"rect",Command_Rect},
	{command_updown,"updown"},
	{command_rightleft,"rightleft"},
	{command_turnright,"turnright"},
	{command_turnleft,"turnleft"},
	{command_turnover,"turnover"},
	{command_turnV,"turnV"},
	{command_turnH,"turnH"},
	{command_about,"about",Command_About},
	//{command_quit,"quit"}
	};

void welcomeAndBye(int i)
{
	printf("***************************************\n");
	if(i == 0)
	{
		printf("**************system begin*************\n");
	}
	else if(i == 1)
	{
		printf("***************system end**************\n");
	}
	printf("***************************************\n");
}

void printMessage(int number)
{
	//0:  _>
	if(0 == number)
	{
		printf("_>");
	}
	//1: path information
	else if(1 == number)
	{
		printf("\nplease input bmp file save path:");
	}
	//2: resolution information
	else if(2 == number)
	{
		printf("\nplease input resolution:");
	}
	else if (3==number)
	{
		printf("\nyou have no power to act this command!\n");
	}
	else
	{
		printf("\nprint message parameter error!\n");
	}
}

void initFileInfo(FileInfo *pFileInfo)
{
	pFileInfo->bfType=0x4d42;
	pFileInfo->file_head.bfSize=
		pFileInfo->width*pFileInfo->height*3+54;
	pFileInfo->file_head.bfReserved1=0;
	pFileInfo->file_head.bfReserved2=0;
	pFileInfo->file_head.bfOffBits=54;

	pFileInfo->info_head.biSize=40;
	pFileInfo->info_head.biWidth=pFileInfo->width;
	pFileInfo->info_head.biHeight=pFileInfo->height;
	pFileInfo->info_head.biPlanes=1;
	pFileInfo->info_head.biBitCount=24;
	pFileInfo->info_head.biCompression=0;
	pFileInfo->info_head.biSizeImage=
		pFileInfo->width*pFileInfo->height*3;
	pFileInfo->info_head.biXPelsPerMeter=0;
	pFileInfo->info_head.biYPelsPerMeter=0;
	pFileInfo->info_head.biClrUsed=0;
	pFileInfo->info_head.biClrImportant=0;	
}

int fileCreate(FileInfo *pFileInfo)
{
	int i,j;
	//receive user input data,create file
	memset(pFileInfo,0,sizeof(FileInfo));
	printMessage(1);
	scanf("%s",pFileInfo->fileName);
	pFileInfo->fd=open(pFileInfo->fileName,
		O_CREAT|O_RDWR,0777);
	if(-1==pFileInfo->fd)
	{
		printf("%s path error,please input again!\n",
			pFileInfo->fileName);
		return 1;
	}
	//receive user input resolution,finish bmp file head
	printMessage(2);
	printf("\nwidth:");
	scanf("%d",&(pFileInfo->width));
	printf("height:");
	scanf("%d",&(pFileInfo->height));
	//create bmp file head and info head
	initFileInfo(pFileInfo);
	//write bmp head block
	write(pFileInfo->fd,&(pFileInfo->bfType),
		sizeof(pFileInfo->bfType));
	write(pFileInfo->fd,&(pFileInfo->file_head),
		sizeof(pFileInfo->file_head));
	write(pFileInfo->fd,&(pFileInfo->info_head),
		sizeof(pFileInfo->info_head));
	//write bmp data block
	for(i=0;i<pFileInfo->height;i++)
	{
		for(j=0;j<pFileInfo->width;j++)
		{
			write(pFileInfo->fd,&p_white,sizeof(p_white));
		}
	}
	close(pFileInfo->fd);
	return 0;
}

void str_depart(char *str, char c[MAX_SIZE][MAX_SIZE])
{
	int len,k,m=0,n=0;
	len = strlen(str);
	for(k=0;k<len;k++)
	{
		if(str[k]!='(' && str[k]!=')' && str[k]!= ',')
		{
			c[m][n++]=str[k];
		}
		else
		{
			if(n!=0)
			{
				m++;
				n=0;
			}
		}
	}
}

void zzThread(void* para)
{
	FileInfo *pFileInfo =(FileInfo*)para;
	ElemType message;
	int i;
	char a[MAX_SIZE][MAX_SIZE]={0};
	while(1)
	{
		//out queue
		if(is_EmptyQueue(g_pQueue))
		{
			sleep(2);
			continue;
		}
		memset(&message,0,sizeof(message));
		out_CirQueue(g_pQueue,&message);
	
		//parse command string
		memset(a,0,sizeof(a));
		str_depart(message.command,a);
		
		for(i=0;i<command_max;i++)
		{
			if(0 == strcmp(a[0],sys_Command[i].commandStr))
			{
				sys_Command[i].p(pFileInfo,a);
				break;
			}
		}
		if(i>=command_max)
		{
			printf("command error,please input again!\n");
		}
	}
}
	
void main()
{
	//thread used
	pthread_t id;
	//queue used
	ElemType message;

	FileInfo fileInfo;
	int ret,i=0;
	char command[MAX_SIZE];
	//welcome gui
	welcomeAndBye(0);
	//check user
	ret=checkUser();
	if(1 == ret)
	{
		goto OUTPORT;
	}

	
	//create bmp file
	while(fileCreate(&fileInfo));

	//create thread
	ret=pthread_create(&id,NULL,(void*)zzThread,&fileInfo);
	if(-1==ret)
	{
		printf("thread create failure,system quit!\n");
		goto OUTPORT;
	}

	//create queue
	g_pQueue=init_CirQueue();
	if(NULL == g_pQueue)
	{
		printf("queue create failure,system quit!\n");
		goto OUTPORT;
	}
	
	//command process
	while(1)
	{
		printMessage(0);
		memset(command,0,sizeof(command));
		ret=scanf("%s",command);
		if(ret<=0)
		{
			printf("command %s is invalid",command);
			printf(",please input again!\n");
			continue;
		}
		if(!strcmp(command,"quit") || !strcmp(command,"QUIT"))
		{
			break;
		}
		else if(!strcmp(command,"Account"))
		{
			ret = checkPass(command);
			if (ret)
			{
				printMessage(3);
				continue;
			}
			//account control
			accountConfig();
			continue;
		}
		//in queue
		memset(&message,0,sizeof(message));
		message.commandID=i++;
		strcpy(message.command,command);
		while(1)
		{
			if(RET_OK==in_CirQueue(g_pQueue,message))
			{
				break;
			}
			sleep(2);
		}
	}
	uinit_CirQueue(g_pQueue);
	//bye gui
OUTPORT:
	welcomeAndBye(1);
}
