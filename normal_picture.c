#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#define MAX_SIZE 256

typedef struct tagBmpFileHeader //�ļ�ͷ  
{  
    //unsigned short bfType;      //��ʶ���ļ�Ϊbmp�ļ�,�ж��ļ��Ƿ�Ϊbmp�ļ������ø�ֵ��"0x4d42"�Ƚ��Ƿ���ȼ��ɣ�0x4d42 = 19778  
    unsigned long  bfSize;      //�ļ���С  
    unsigned short bfReserved1; //Ԥ����λ  
    unsigned short bfReserved2; //Ԥ����λ  
    unsigned long  bfOffBits;   //ͼ������������ʼλ��  
}BmpFileHeader;//14�ֽ�  
typedef struct tagBmpInfoHeader //��Ϣͷ  
{  
    unsigned long  biSize;  //ͼ�����ݿ��С  
    long     biWidth;   //���  
    long     biHeight;  //�߶�  
    unsigned short biPlanes;//Ϊ1  
    unsigned short biBitCount; //����λ����8-�Ҷ�ͼ��24-���ɫ  
    unsigned long biCompression;//ѹ����ʽ  
    unsigned long biSizeImage;  //ͼ�������ݴ�С  
    long     biXPelsPerMeter;  //ˮƽ�ֱ��ʣ�����ÿ��  
    long     biYPelsPerMeter;  
    unsigned long biClrUsed;   //λͼʵ���õ�����ɫ��  
    unsigned short biClrImportant;//λͼ��ʾ���̣���Ҫ����ɫ����0--���ж���Ҫ  
}BmpInfoHeader;//40�ֽ�  

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

void parseAndActCommand(FileInfo *pFileInfo,char*command)
{
	int i;
	char a[MAX_SIZE][MAX_SIZE]={0};
	if(command == NULL || pFileInfo == NULL)
	{
		return;
	}
	//parse command string
	str_depart(command,a);
	
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
	
void main()
{
	FileInfo fileInfo;
	int ret;
	char command[MAX_SIZE]={0};
	//welcome gui
	welcomeAndBye(0);
	//create bmp file
	while(fileCreate(&fileInfo));
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
		parseAndActCommand(&fileInfo,command);
	}
	//bye gui
	welcomeAndBye(1);
}
