poll:
	gcc -O3 -g -c Connection.c -I../include -D_POLL
	gcc -O3 -g -o test-poll test_poll.c Connection.o ../network-poll.a -I../include -D_POLL -lpthread -lrt
poll-debug:
	gcc -g -c Connection.c -I../include -D_POLL
	gcc -g -o test-poll test_poll.c ../network-poll.a -I../include -D_POLL -lpthread -lrt -ltcmalloc
comp:
	gcc -O3 -g -o test-comp test_comp.c ../network-comp.a -I../include -lpthread -lrt -ltcmalloc
comp-debug:
	gcc -g -o test-comp test_comp.c ../network-comp.a -I../include -lpthread -lrt -ltcmalloc
