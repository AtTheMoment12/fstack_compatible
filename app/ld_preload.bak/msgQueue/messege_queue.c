#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "messege_queue.h"
#include "msgList.h"
#include "msg.h"

/*创建消息队列，成功返回消息队列handle，失败返回NULL*/
pMsgQueue_t msgQueue_create(void)
{
    pMsgQueue_t q = (pMsgQueue_t)malloc(sizeof(msgQueue_t));
    if(q == NULL)
        return NULL;
    list_init(&(q->queueHead));
    q->length = 0;
    q->lock = 0;
    return q;
}

/*加锁 防止资源抢占*/
static void msgQueue_addLock(pMsgQueue_t q)
{
    q->lock = 1;
}
/*解锁 释放资源占有*/
static void msgQueue_delLock(pMsgQueue_t q)
{
    q->lock = 0;
}
/*判断资源是否被占用*/
static unsigned char msgQueue_isLocked(pMsgQueue_t q)
{   
    return (q->lock);
}


/*从消息队列中获取消息*/
pMsg_t msgQueue_getMsg(pMsgQueue_t q)
{
    pMsg_t m;
    if(q==NULL)
        return NULL;
    if(msgQueue_isLocked(q))
        return NULL;
    if(msgQueue_isEmpty(q))
        return NULL;
    msgQueue_addLock(q);

    m = list_head(&(q->queueHead),msg_t,nodeHead);
    if(m==NULL)
    {
        msgQueue_delLock(q);
        return NULL;
    }
    list_remove((&(q->queueHead))->next);
    msgQueue_delLock(q);
    return m;
}
/*将消息添加到队列尾部*/
int msgQueue_putMsg(pMsgQueue_t q,pMsg_t m)
{
    if(q==NULL){
        printf("q==NULL\n");
        return -1;
    }
    /*长度大于0但是数据为空*/
    if((m != NULL) && (m->len != 0) && (m->pdata == NULL)){
        printf("m->pdata==NULL\n");
        return -2;
    }
    while (msgQueue_isLocked(q));

    msgQueue_addLock(q);
    list_append(&(q->queueHead),&(m->nodeHead));
    msgQueue_delLock(q);
    m->len++;
    return 0;
    
}
/*判断消息队列是否为空 为空返回1 否则返回0*/
int msgQueue_isEmpty(pMsgQueue_t q)
{
    return list_empty(&(q->queueHead));
}
/*清除消息队列*/
int msgQueue_delete(pMsgQueue_t q)
{
    if(q!=NULL)
    {
        list_remove(&(q->queueHead));
        free(q);
    }
    return 0;
}
