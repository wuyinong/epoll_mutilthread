#include "Engine.h"
#include "link_list.h"
#include "KendyNet.h"
#include <stdlib.h>
#include "epoll.h"
#include <assert.h>

engine_t create_engine()
{
	engine_t e = malloc(sizeof(*e));
	if(e)
	{
		e->mtx = spin_create();//mutex_create();
		e->status = 0;
		e->event_queue = create_mq(1024,NULL);
		e->Init = epoll_init;
		e->Loop = epoll_loop;
		e->Register = epoll_register;
		e->UnRegister = epoll_unregister;
	}
	return e;
}

void   free_engine(engine_t *e)
{
	assert(e);
	assert(*e);
	//mutex_destroy(&(*e)->mtx);
	spin_destroy(&(*e)->mtx);
	destroy_mq(&(*e)->event_queue);
	free(*e);
	*e = 0;
}

int put_event(engine_t e,st_io *io)
{
	assert(e);
	assert(io);
	mq_push_now(e->event_queue,(struct list_node*)io);
	return 0;
}

int put_event2(engine_t e,st_io *io)
{
	assert(e);
	assert(io);
	mq_push(e->event_queue,(struct list_node*)io);
	return 0;
}

void   stop_engine(engine_t e)
{
	assert(e);
	e->status = 0;
}
