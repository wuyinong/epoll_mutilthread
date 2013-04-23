#ifndef _HANDLEMGR_H
#define _HANDLEMGR_H

#include "KendyNet.h"
#include "Socket.h"
#include "Engine.h"


//typedef socket_wrapper *socket_t;
//typedef engine         *engine_t;


//定义系统支持的最大套接字和engine的数量
#define MAX_ENGINE 1
#define MAX_SOCKET 4096

static inline socket_t GetSocketByHandle(HANDLE handle)
{
	return (socket_t)handle;
}

static inline engine_t GetEngineByHandle(HANDLE handle)
{
	return (engine_t)handle;
}

extern HANDLE   NewEngine();
extern void     ReleaseEngine(HANDLE);

extern HANDLE   NewSocketWrapper();
extern int      ReleaseSocketWrapper(HANDLE);

#endif
