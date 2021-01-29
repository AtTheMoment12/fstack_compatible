#ifndef __MSG_H__
#define __MSG_H__

#include "msgList.h"

#define TID_MSG 0

struct msg {
 	struct list nodeHead;/*消息队列的头*/
 	unsigned int type;
	unsigned int len;
	//unsigned int lock;
	void *pdata;
	int flag;
};
typedef struct msg msg_t;
typedef struct msg * pMsg_t;

/*新建消息资源*/
pMsg_t msg_new(unsigned int type,unsigned char *data,int len,int flag);
/*消息队列在取得消息数据后，必须释放资源*/
void msg_free(pMsg_t pMsg);
#endif