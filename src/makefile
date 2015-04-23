job: job.c job.h error.c enq deq stat Demo
	cc -Wall -o job job.c job.h error.c
enq: enq.c job.h error.c
	cc -Wall -o enq enq.c job.h error.c
deq: deq.c job.h error.c
	cc -Wall -o deq deq.c job.h error.c
stat: stat.c job.h error.c
	cc -Wall -o stat stat.c job.h error.c
Demo: Demo.c
	cc -Wall -o Demo Demo.c
clean:
	rm job enq deq stat Demo
