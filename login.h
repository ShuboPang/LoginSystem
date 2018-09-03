#ifndef _LOGIN_H_
#define _LOGIN_H_

#include <sqlite3.h>

#define LOGIN_MIN_SIZE 64
#define LOGIN_MAX_SIZE 256


typedef struct
{
	char userName[LOGIN_MIN_SIZE];
	char passWord[LOGIN_MIN_SIZE];
	char level[LOGIN_MIN_SIZE];
}Account;


int checkUser(void);
void accountConfig(void);


#endif
