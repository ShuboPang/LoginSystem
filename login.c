#include "login.h"
#include <stdio.h>
#include <string.h>
#include "queue.h"
#include <pthread.h>


//#define ZZ_DEBUG
#ifdef ZZ_DEBUG
	#define POS_DEBUG() printf("file:%s func:%s line:%d\n",__FILE__,__func__,__LINE__)
#else
	#define POS_DEBUG()
#endif

#define ERROR_TIME 3

enum logCommand
{
	logCommand_add,
	logCommand_del,
	logCommand_chg,
	logCommand_lis,
	logCommand_max
};

typedef struct
{
	enum logCommand commandNo;
	char commandStr[LOGIN_MIN_SIZE];
	void (*p)(char pCommandBlock[LOGIN_MIN_SIZE][LOGIN_MIN_SIZE]);
}LoginCommand;



const char db_path[]={"userLog.db"};



static SqQueue *g_pLogQueue;



static int zMaxId(void *notUse,int argc,char** argv,char** azColName)
{
	int *tmp = (int*)notUse;
	*tmp=atoi(argv[0]);
	return 0;
}

void logComAdd(char pCommandBlock[LOGIN_MIN_SIZE][LOGIN_MIN_SIZE])
{
	sqlite3 *db;
	char * zerrmsg;
	int maxId=0;
	char tmpSql[LOGIN_MAX_SIZE];
	int rc;
	rc=sqlite3_open(db_path,&db);
	if(rc!=0)
	{
		printf("can't open %s \n",db_path);
		return;
	}
	//obtain exist max uID
	memset(tmpSql,0,LOGIN_MAX_SIZE);
	strcpy(tmpSql,
		"select * from User order by uId desc limit 1;");
	rc=sqlite3_exec(db,tmpSql,zMaxId,(void*)&maxId,&zerrmsg);
	if(rc!=SQLITE_OK)
	{
		printf("sqlite error:%s\n",zerrmsg);
		sqlite3_free(zerrmsg);
	}
	//add user record
	memset(tmpSql,0,LOGIN_MAX_SIZE);
	sprintf(tmpSql,
		"insert into User values(%d,'%s','%s','%s');",
		maxId+1,pCommandBlock[1],pCommandBlock[2],
		pCommandBlock[3]);
	rc=sqlite3_exec(db,tmpSql,NULL,NULL,&zerrmsg);
	if(rc!=SQLITE_OK)
	{
		printf("sqlite error:%s\n",zerrmsg);
		sqlite3_free(zerrmsg);
	}
	sqlite3_close(db);		
}

void logComDel(char pCommandBlock[LOGIN_MIN_SIZE][LOGIN_MIN_SIZE])
{
	sqlite3 *db;
	char * zerrmsg;
	char tmpSql[LOGIN_MAX_SIZE];
	int rc;
	rc=sqlite3_open(db_path,&db);
	if(rc!=0)
	{
		printf("can't open %s \n",db_path);
		return;
	}
	//delete the account
	memset(tmpSql,0,LOGIN_MAX_SIZE);
	sprintf(tmpSql,
		"delete from User where uName='%s';",
		pCommandBlock[1]);
	printf("tmpSql=%s \n",tmpSql);
	rc=sqlite3_exec(db,tmpSql,NULL,NULL,&zerrmsg);
	if(rc!=SQLITE_OK)
	{
		printf("sqlite error:%s\n",zerrmsg);
		sqlite3_free(zerrmsg);
	}
	sqlite3_close(db);
}

void logComChg(char pCommandBlock[LOGIN_MIN_SIZE][LOGIN_MIN_SIZE])
{
	sqlite3 *db;
	char * zerrmsg;
	char tmpSql[LOGIN_MAX_SIZE];
	int rc;
	rc=sqlite3_open(db_path,&db);
	if(rc!=0)
	{
		printf("can't open %s \n",db_path);
		return;
	}
	//change the account
	memset(tmpSql,0,LOGIN_MAX_SIZE);
	sprintf(tmpSql,
"update User set uLevel='%s',uPassword='%s' where uName='%s';",
		pCommandBlock[2],pCommandBlock[3],
		pCommandBlock[1]);
	rc=sqlite3_exec(db,tmpSql,NULL,NULL,&zerrmsg);
	if(rc!=SQLITE_OK)
	{
		printf("sqlite error:%s\n",zerrmsg);
		sqlite3_free(zerrmsg);
	}
	sqlite3_close(db);
}
static int displayList(void *notUse,int argc,char** argv,char** azColName)
{
	int i;
	int*flag = (int*)notUse;
	if(0==*flag)
	{
		for(i=0;i<argc;i++)
		{
			printf("%s\t",azColName[i]);
		}
		*flag=1;
		printf("\n");
	}
	
	for(i=0;i<argc;i++)
	{
		printf("%s\t",argv[i]);
	}
	printf("\n");
	return 0;
}

void logComLis(char pCommandBlock[LOGIN_MIN_SIZE][LOGIN_MIN_SIZE])
{
	sqlite3 *db;
	char * zerrmsg;
	int flag=0;
	char tmpSql[LOGIN_MAX_SIZE];
	int rc;
	rc=sqlite3_open(db_path,&db);
	if(rc!=0)
	{
		printf("can't open %s \n",db_path);
		return;
	}
	//change the account
	memset(tmpSql,0,LOGIN_MAX_SIZE);
	strcpy(tmpSql,"select * from User;");
	rc=sqlite3_exec(db,tmpSql,displayList,(void*)&flag,&zerrmsg);
	if(rc!=SQLITE_OK)
	{
		printf("sqlite error:%s\n",zerrmsg);
		sqlite3_free(zerrmsg);
	}
	sqlite3_close(db);
}

const LoginCommand loginCom[]={
{logCommand_add,"add",logComAdd},
{logCommand_del,"del",logComDel},
{logCommand_chg,"chg",logComChg},
{logCommand_lis,"list",logComLis}
};




static int zFind(void *notUse,int argc,char** argv,char** azColName)
{
	int *tmp = (int*)notUse;
	*tmp = 1;
	return 0;
}

int checkUser()
{
	int i;
	sqlite3 *db;
	char * zerrmsg;
	int found=0;
	Account tmpAccount;
	char tmpSql[LOGIN_MAX_SIZE];

	for(i=0;i<ERROR_TIME;i++)
	{
		printf("please input user name:");
		gets(tmpAccount.userName);
		printf("please input password:");
		gets(tmpAccount.passWord);

		//在数据库中user表内，查找用户输入的用户名和密码
		int rc;
		rc=sqlite3_open(db_path,&db);
		if(rc!=0)
		{
			printf("can't open %s \n",db_path);
			return 1;
		}
		memset(tmpSql,0,LOGIN_MAX_SIZE);
		sprintf(tmpSql,
"select * from User where uName='%s' and uPassword = '%s';",
tmpAccount.userName,tmpAccount.passWord);
		
		sqlite3_exec(db,tmpSql,zFind,(void*)(&found),&zerrmsg);
		
		if(1==found)
		{
			sqlite3_close(db);
			return 0;
		}
		sqlite3_close(db);		
		printf("your user name or password error,");
		printf("please input again!\n");
	}
	printf("your input error three times, system quit!\n");
	return 1;
}
void gmz_depart(char *str, char c[LOGIN_MIN_SIZE][LOGIN_MIN_SIZE])
{
	int len,k,m=0,n=0;
	len = strlen(str);
	for(k=0;k<len;k++)
	{
		if(str[k]!=' ')
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

void logThread()
{
	ElemType message;
	char a[LOGIN_MIN_SIZE][LOGIN_MIN_SIZE]={0};
	int i;
	while(1)
	{
		//check queue empty or not
		if(is_EmptyQueue(g_pLogQueue))
		{
			sleep(2);
			continue;
		}
		//out queue
		memset(&message,0,sizeof(message));
		out_CirQueue(g_pLogQueue,&message);
		//parse and act
		memset(a,0,sizeof(a));
		gmz_depart(message.command,a);

		for(i=0;i<logCommand_max;i++)
		{
			if(!strcmp(a[0],loginCom[i].commandStr))
			{
				loginCom[i].p(a);
			}
		}
	}

}

void accountConfig()
{
	char tmpCommand[LOGIN_MAX_SIZE]={0};
	ElemType message;
	int ret,i=0;
	//创建子线程
	pthread_t id;
	pthread_attr_t attr;
	//线程属性初始化
	pthread_attr_init(&attr);
	//设置绑定属性
	pthread_attr_setscope(&attr,PTHREAD_SCOPE_SYSTEM);
	//创建线程
	ret=pthread_create(&id,&attr,(void*)logThread,NULL);
	if(-1 == ret)
	{
		printf("thread create failure,Account control quit!\n");
		return;
	}
	//创建队列
	g_pLogQueue=init_CirQueue();
	if(NULL == g_pLogQueue)
	{
		printf("queue create failure,Account control quit!\n");
		goto quit;
	}
	setbuf(stdin,NULL);
	while(1)
	{	
		printf(">>");		
		memset(tmpCommand,0,LOGIN_MAX_SIZE);
		gets(tmpCommand);
		if(!strcmp(tmpCommand,"q"))
		{
			break;
		}
		//入队
		message.commandID = i++;
		memset(message.command,0,sizeof(message.command));
		strcpy(message.command,tmpCommand);
		while(1)
		{
			if(RET_OK == in_CirQueue(g_pLogQueue,message))
			{
				break;
			}
			sleep(2);
		}
	}

	uinit_CirQueue(g_pLogQueue);
quit:
	pthread_cancel(id);
	pthread_join(id,NULL);
}
