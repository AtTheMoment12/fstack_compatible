#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "msgList.h"
#include "msg.h"

extern void *malloc (size_t __size) __THROW __attribute_malloc__ __wur;

/*新建消息资源*/
pMsg_t msg_new(unsigned int type,unsigned char *data,int len,int flag)
{
    pMsg_t pMsg = NULL;
    /*根据消息数据长度申请空间*/
    pMsg = (struct msg *)malloc(sizeof(struct msg));
    if(pMsg==NULL)
        return NULL;
    /*初始化消息参数*/
    pMsg->len = len;
    pMsg->type = type;
    pMsg->flag = flag;
    if(len != 0)
    {
        pMsg->pdata = (void*)malloc(len);
        if(pMsg->pdata == NULL)
            return NULL;
        memcpy(pMsg->pdata,data,len);
    }
    return pMsg;
}
/*消息队列在取得消息数据后，必须释放资源*/
void msg_free(pMsg_t pMsg)
{
    if(pMsg!=NULL){
        if(pMsg->pdata != NULL){
            free(pMsg->pdata);
            pMsg->pdata = NULL;
        }
        free(pMsg);
        pMsg = NULL;
    }
}