#include "Socket.h"
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include "SocketWrapper.h"
#include "KendyNet.h"

socket_t create_socket()
{
	socket_t s = malloc(sizeof(*s));
	if(s)
	{
		s->mtx = spin_create();
		s->recv_mtx = spin_create();
		s->send_mtx = spin_create();
		s->pending_send = LINK_LIST_CREATE();
		s->pending_recv = LINK_LIST_CREATE();
		s->status = 0;
		s->engine = 0;
	}
	return s;
}

void free_socket(socket_t *s)
{
	assert(s);assert(*s);
	spin_destroy(&(*s)->mtx);
	spin_destroy(&(*s)->recv_mtx);
	spin_destroy(&(*s)->send_mtx);
	LINK_LIST_DESTROY(&(*s)->pending_send);
	LINK_LIST_DESTROY(&(*s)->pending_recv);
	free(*s);
	*s = 0;
}

#ifdef _POLL
#include "Socket_poll.h"
#else
#include "Socket_comp.h"
#endif
