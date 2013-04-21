/*	
    Copyright (C) <2012>  <huangweilook@21cn.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/	
#ifndef _KENDYNET_H
#define _KENDYNET_H
#include <stdint.h>
typedef struct list_node
{
	struct list_node *next;
}list_node;

#define LIST_NODE list_node node;

/*IOÇëÇóºÍÍê³É¶ÓÁÐÊ¹ÓÃµÄœá¹¹*/
typedef struct
{
	LIST_NODE;
	struct iovec *iovec;
	int    iovec_count;
	int    bytes_transfer;
	int    error_code;
}st_io;

//³õÊŒ»¯ÍøÂçÏµÍ³
int      InitNetSystem();

typedef int HANDLE;
HANDLE   CreateEngine();
void     CloseEngine(HANDLE);
int      EngineRun(HANDLE);

int     Bind2Engine(HANDLE,HANDLE);


enum
{	
	NO_WAIT = -1,
	INFINITE = 0,
};

int      GetQueueEvent(HANDLE,st_io **,int timeout);
int      PutQueueEvent(HANDLE,st_io *);

enum
{
	RECV_NOW = 1,
	RECV_POST = 0,
	SEND_NOW = 1,
	SEND_POST = 0,
};

#ifdef _POLL
int WSASend(HANDLE,st_io*,int32_t flag);
int WSARecv(HANDLE,st_io*,int32_t flag);
#else
int WSASend(HANDLE,st_io*);
int WSARecv(HANDLE,st_io*);
#endif


#endif
