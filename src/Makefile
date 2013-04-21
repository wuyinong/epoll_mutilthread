poll:
	gcc -O3 -g -c *.c -I../include -D_POLL
	ar -rc network-poll.a *.o
	mv network-poll.a ../	
poll-debug:
	gcc -g -c *.c -I../include -D_POLL
	ar -rc network-poll.a *.o
	mv network-poll.a ../
comp:
	gcc -O3 -g -c *.c -I../include
	ar -rc network-comp.a *.o
	mv network-comp.a ../	
comp-debug:
	gcc -g -c *.c -I../include
	ar -rc network-comp.a *.o
	mv network-comp.a ../	
