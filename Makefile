poll:
	gcc -O3 -g -o test-poll test_poll.c network-poll.a -I./include -D_POLL -lpthread -lrt -ltcmalloc
poll-debug:
	gcc -g -o test-poll test_poll.c network-poll.a -I./include -D_POLL -lpthread -lrt -ltcmalloc
comp:
	gcc -O3 -g -o test-comp test_comp.c network-comp.a -I./include -lpthread -lrt -ltcmalloc
comp-debug:
	gcc -g -o test-comp test_comp.c network-comp.a -I./include -lpthread -lrt -ltcmalloc
