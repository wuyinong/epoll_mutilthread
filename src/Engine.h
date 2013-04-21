#ifndef _ENGINE_H
#define _ENGINE_H

#include "sync.h"
//#include "thread.h"
#include "mq.h"
#include "link_list.h"
#include "spinlock.h"

struct socket_wrapper;
typedef struct engine
{
	int  (*Init)(struct engine*);
	void (*Loop)(struct engine*);
	int  (*Register)(struct engine*,struct socket_wrapper*);
	int  (*UnRegister)(struct engine*,struct socket_wrapper*);
	
	spinlock_t mtx;
	volatile int status; /*0:¹Ø±Õ×´Ì¬,1:¿ªÆô×´Ì¬*/
	int poller_fd;
	mq_t event_queue;
		
}*engine_t;

engine_t create_engine();
void   free_engine(engine_t *);
void   stop_engine(engine_t);


#endif
