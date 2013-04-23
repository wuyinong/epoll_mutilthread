#include "HandleMgr.h"
HANDLE	NewSocketWrapper()
{
	return (HANDLE)create_socket();
}


inline static int RemoveBinding(engine_t e, socket_t sock)
{
	return e ? e->UnRegister(e,sock) : -1;
}


int  ReleaseSocketWrapper(HANDLE handle)
{
	socket_t s = (socket_t)handle;
	RemoveBinding(s->engine,s);
	close(s->fd);
	free_socket(&s);
} 

HANDLE	NewEngine()
{
	return (HANDLE)create_engine();
}

void  ReleaseEngine(HANDLE handle)
{
	engine_t e = (engine_t)handle;
	free_engine(&e);
} 
