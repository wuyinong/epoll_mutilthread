#include <stdio.h>
#include <stdlib.h>
#include "common_define.h"
#include "KendyNet.h"
#include "thread.h"
#include "SocketWrapper.h"
#include "atomic.h"
#include "SysTime.h"
#include "Connection.h"
#include <assert.h>


HANDLE engine;
const char *ip;
long port;
int total_bytes_recv = 0;


#define MAX_CLIENT 2000
static struct connection *clients[MAX_CLIENT];

void init_clients()
{
	uint32_t i = 0;
	for(; i < MAX_CLIENT;++i)
		clients[i] = 0;
}

void add_client(struct connection *c)
{
	uint32_t i = 0;
	for(; i < MAX_CLIENT; ++i)
	{
		if(clients[i] == 0)
		{
			clients[i] = c;
			break;
		}
	}
}

void send2_all_client(rpacket_t r)
{
	uint32_t i = 0;
	wpacket_t w;
	for(; i < MAX_CLIENT; ++i)
	{
		if(clients[i])
		{
			w = wpacket_create_by_rpacket(NULL,r);
			assert(w);
			connection_send(clients[i],w,NULL);
			//connection_push_packet(clients[i],w,NULL);
		}
	}
}

void remove_client(struct connection *c,int32_t reason)
{
	uint32_t i = 0;
	for(; i < MAX_CLIENT; ++i)
	{
		if(clients[i] == c)
		{
			clients[i] = 0;
			break;
		}
	}
	HANDLE sock = c->socket;
	if(0 == connection_destroy(&c))
	{
		ReleaseSocketWrapper(sock);
	}
}

void on_process_packet(struct connection *c,rpacket_t r)
{
	send2_all_client(r);
	//wpacket_t w = wpacket_create_by_rpacket(NULL,r);
	//connection_send(c,w,NULL);
	//++send_request;

	total_bytes_recv += rpacket_len(r);
	rpacket_destroy(&r);
	//++packet_recv;
}

void *ListerRoutine(void *arg)
{
	//thread_t thread = (thread)arg//(thread_t)CUSTOM_ARG(arg);
	struct sockaddr_in servaddr;
	HANDLE listerfd;
	if((listerfd = Tcp_Listen(ip,port,&servaddr,5)) >= 0)
	{
		while(1)
		{
			struct sockaddr_in sa;
			socklen_t salen = sizeof(sa);
			HANDLE sock = Accept(listerfd,(struct sockaddr*)&sa,&salen);
			if(sock >= 0)
			{
				printf("a new client\n");
				struct connection *c = connection_create(sock,0,MUTIL_THREAD,on_process_packet,remove_client);
				add_client(c);
				setNonblock(sock);
				//发出第一个读请求
				connection_start_recv(c);
				Bind2Engine(engine,sock);
			}
			else
			{
				printf("accept出错\n");
			}
		}
		printf("listener 终止\n");
	}
	return 0;
}

void *IORoutine(void *arg)
{
	st_io* ioComp;
	while(1)
	{
		ioComp = 0;
		if(0 > GetQueueEvent(engine,&ioComp,INFINITE))
		{
			printf("poller终止\n");
			break;
		}
		if(ioComp)
		{

			struct OVERLAPCONTEXT *_CONTEXT = (struct OVERLAPCONTEXT *)ioComp;
			struct connection *c = _CONTEXT->c;
			if(_CONTEXT == &c->recv_overlap)
				do_recv((st_io*)_CONTEXT);
			else if(_CONTEXT == &c->send_overlap)
				do_send((st_io*)_CONTEXT);
			else
			{
				printf("error\n");
			}
		}
	}
	printf("IO end\n");
	return 0;
}


void *EngineRoutine(void *arg)
{
	EngineRun(engine);
	printf("Engine stop\n");
	return 0;
}

int main(int argc,char **argv)
{
	ip = argv[1];
	port = atoi(argv[2]);
	signal(SIGPIPE,SIG_IGN);
	if(InitNetSystem() != 0)
	{
		printf("Init error\n");
		return 0;
	}
	init_mq_system();
	engine = CreateEngine();
	thread_t listener = create_thread(0);
	start_run(listener,ListerRoutine,listener);
	
	int complete_count = atoi(argv[3]);
	int i = 0;
	for( ; i < complete_count; ++i)
	{
		thread_t complete_t = create_thread(0);
		start_run(complete_t,IORoutine,0);
	}
	thread_t engine_thread = create_thread(1);
	start_run(engine_thread,EngineRoutine,0);
	getchar();
	CloseEngine(engine);
	join(engine_thread);
	return 0;
}
