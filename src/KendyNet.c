#include "KendyNet.h"
#include "Engine.h"
#include "Socket.h"
#include "link_list.h"
#include "HandleMgr.h"
#include <assert.h>

int InitNetSystem()
{
	return InitHandleMgr();
}

int EngineRun(HANDLE engine)
{
	engine_t e = GetEngineByHandle(engine);
	if(!e)
		return -1;
	e->Loop(e);	
	return 0;
}

HANDLE CreateEngine()
{
	HANDLE engine = NewEngine();
	if(engine >= 0)
	{
		engine_t e = GetEngineByHandle(engine);
		if(0 != e->Init(e))
		{
			CloseEngine(engine);
			engine = NULL;
		}
	}
	return engine;
}

void CloseEngine(HANDLE handle)
{
	ReleaseEngine(handle);
}

int Bind2Engine(HANDLE e,HANDLE s)
{
	engine_t engine = GetEngineByHandle(e);
	socket_t sock   = GetSocketByHandle(s);
	if(!engine || ! sock)
		return -1;
	if(engine->Register(engine,sock) == 0)
	{
		sock->engine = engine;
		return 0;
	}
	return -1;
}

void StopEngine(HANDLE e)
{
	engine_t engine = GetEngineByHandle(e);
	if(engine)
		stop_engine(engine);
}

int	GetQueueEvent(HANDLE handle,st_io **io ,int timeout)
{
	assert(io);

	engine_t e = GetEngineByHandle(handle);
	if(!e)
		return -1;
		
	if(e->status == 0)
		return -1;
	if(timeout < 0)
		*io = (st_io*)mq_pop(e->event_queue,0);//不等待,立即返回
	else if(timeout == 0)
	{
		//无限等待,直到队列中有元素或engine停止才返回
		while((*io = (st_io*)mq_pop(e->event_queue,10)) == NULL)
		{
			if(e->status == 0)
				return -1;
		}
	}
	else
	{
		//等待一定的时间
		int32_t ms_remain = 10;
		if(timeout > ms_remain)
			ms_remain = timeout;
		while(ms_remain > 0)
		{
			if(e->status == 0)
				return -1;
			*io = (st_io*)mq_pop(e->event_queue,10);
			if(*io)
				return 0;
			ms_remain -= 10;	
		}				
	}
	return 0;
}

int	PutQueueEvent(HANDLE handle,st_io *io)
{
	assert(io);
	engine_t e = GetEngineByHandle(handle);
	if(!e)
		return -1;
	
	return put_event(e,io);
}

extern int put_event(engine_t e,st_io *io);

#ifdef _POLL
int WSASend(HANDLE sock,st_io *io,int32_t flag)
{
	assert(io);
	socket_t s = GetSocketByHandle(sock);
	if(!s)
		return -1;
	
	int active_send_count = -1;
	int ret = 0;
	
	spin_lock(s->send_mtx);
	//保证顺序,先插入队列
	LINK_LIST_PUSH_BACK(s->pending_send,io);
	io = 0;
	if(s->writeable)
	{
		io = LINK_LIST_POP(st_io*,s->pending_send);
		active_send_count = s->active_write_count;
	}
	spin_unlock(s->send_mtx);
	if(io)
	{
		if(flag == SEND_POST)
			put_event(s->engine,io);
		else
			ret = _send(s,io,active_send_count);
			
	}
	return ret;
}

int WSARecv(HANDLE sock,st_io *io,int32_t flag)
{
	assert(io);
	socket_t s = GetSocketByHandle(sock);
	if(!s)
		return -1;

	int active_recv_count = -1;
	int ret = 0;
	
	spin_lock(s->recv_mtx);
	//保证顺序,先插入队列
	LINK_LIST_PUSH_BACK(s->pending_recv,io);
	io  = 0;
	if(s->readable)
	{
		io = LINK_LIST_POP(st_io*,s->pending_recv);
		active_recv_count = s->active_read_count;
	}
	spin_unlock(s->recv_mtx);
	
	if(io)
	{
		if(flag == RECV_POST)
			put_event(s->engine,io);
		else
			ret =  _recv(s,io,active_recv_count);
	}
	return ret;
}
#else
int WSASend(HANDLE sock,st_io *io)
{
	assert(io);
	socket_t s = GetSocketByHandle(sock);
	if(!s)
		return -1;
	
	int active_send_count = -1;
	int ret = 0;
	
	spin_lock(s->send_mtx);
	//Îª±£Ö€ÖŽÐÐË³ÐòÓëÇëÇóµÄË³ÐòÒ»ÖÂ,ÏÈœ«ÇëÇó²åÈë¶ÓÁÐÎ²²¿,ÔÙµ¯³ö¶ÓÁÐÊ×ÔªËØ
	LINK_LIST_PUSH_BACK(s->pending_send,io);
	io = 0;
	if(s->writeable)
	{
		io = LINK_LIST_POP(st_io*,s->pending_send);
		active_send_count = s->active_write_count;
	}
	spin_unlock(s->send_mtx);
	if(io)
		ret =  _send(s,io,active_send_count,SLIENT_ON_COMPLETE);
	return ret;
}

int WSARecv(HANDLE sock,st_io *io)
{
	assert(io);
	socket_t s = GetSocketByHandle(sock);
	if(!s)
		return -1;

	int active_recv_count = -1;
	int ret = 0;
	
	spin_lock(s->recv_mtx);
	//Îª±£Ö€ÖŽÐÐË³ÐòÓëÇëÇóµÄË³ÐòÒ»ÖÂ,ÏÈœ«ÇëÇó²åÈë¶ÓÁÐÎ²²¿,ÔÙµ¯³ö¶ÓÁÐÊ×ÔªËØ
	LINK_LIST_PUSH_BACK(s->pending_recv,io);
	io  = 0;
	if(s->readable)
	{
		io = LINK_LIST_POP(st_io*,s->pending_recv);
		active_recv_count = s->active_read_count;
	}
	spin_unlock(s->recv_mtx);
	
	if(io)
		ret =  _recv(s,io,active_recv_count,SLIENT_ON_COMPLETE);
	return ret;
}
#endif


