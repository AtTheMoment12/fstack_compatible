#ifndef __MESSEGE_QUEUE_H__
#define __MESSEGE_QUEUE_H__

#include "msg.h"

struct messege_queue{
    struct list queueHead;
    unsigned int length;
    unsigned char lock;
};
typedef struct messege_queue msgQueue_t;
typedef struct messege_queue * pMsgQueue_t;


/*创建消息队列，成功返回消息队列handle，失败返回NULL*/
pMsgQueue_t msgQueue_create(void);

/*从消息队列中获取消息*/
pMsg_t msgQueue_getMsg(pMsgQueue_t q);

/*将消息添加到队列尾部*/
int msgQueue_putMsg(pMsgQueue_t q,pMsg_t m);
/*判断消息队列是否为空 为空返回1 否则返回0*/
int msgQueue_isEmpty(pMsgQueue_t q);
/*清除消息队列*/
int msgQueue_delete(pMsgQueue_t q);

#endif