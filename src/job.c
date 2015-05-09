//作业调度程序

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>
#include "job.h"

#define DEBUG
#define UPD_DEBUG
#define CMD_DEBUG
//时间片
const int quantum[3]={1,2,5};

int jobid=0;
int siginfo=1;
int fifo;
//int globalfd;

struct waitqueue *head[3]={NULL,NULL,NULL};
struct waitqueue *next=NULL,*current=NULL;

/* 调度程序 */
void scheduler()
{
	struct jobinfo *newjob=NULL;
	struct jobcmd cmd;
	struct waitqueue *p;
	int i;
	int  count = 0;
	memset(&cmd,0,DATALEN);
	while (1){
		if((count=read(fifo,&cmd,DATALEN))<0)
			error_sys("read fifo failed");
#ifdef DEBUG
		printf("任务三：\nReading whether other process send command!\n");
		if(count){
			printf("任务三：\ncmd cmdtype\t%d\ncmd defpri\t%d\ncmd data\t%s\n",cmd.type,cmd.defpri,cmd.data);
		}
		else{
			break;
		}
#endif

		/* 更新等待队列中的作业 */
#ifdef DEBUG
		   printf("任务三：\nUpdate jobs in wait queue!\n");
#endif

#ifdef CMD_DEBUG
		printf("BEFORE CMD:\n");
		if(current) {
			printf("current process: \nJOBID\tPID\tSTATE\n%d\t%d\t%d\n", current->job->jid,current->job->pid,current->job->state);
		}
		else {
			printf("no current process!\n");
		}
		for (i=0;i<3;++i){
			if(head[i]){		
				printf("\nwaitqueue %d: \nJOBID\tPID\tSTATE\n",i);
			}else{		
				printf("\nwaitqueue %d is empty!\n",i);
			}

			for(p=head[i]; p!=NULL; p=p->next) {
				printf("%d\t%d\t%d\n", p->job->jid, p->job->pid, p->job->state);
			}
		}
#endif

		switch(cmd.type){
		case ENQ:
#ifdef DEBUG
			printf("任务三：\nExecute enq!\n");
#endif

			do_enq(newjob,cmd);
			break;
		case DEQ:
#ifdef DEBUG
			printf("任务三：\nExecute deq!\n");
#endif
			do_deq(cmd);
			break;
		case STAT:
#ifdef DEBUG
			printf("任务三：\nExecute stat!\n");
#endif
			do_stat(cmd);
			break;
		default:
			break;
		}
	}
#ifdef CMD_DEBUG
	printf("AFTER CMD:\n");
	if(current) {
		printf("current process: \nJOBID\tPID\tSTATE\n%d\t%d\t%d\n", current->job->jid,current->job->pid,current->job->state);
	}
	else {
		printf("no current process!\n");
	}
	for (i=0;i<3;++i){
		if(head[i]){		
			printf("\nwaitqueue %d: \nJOBID\tPID\tSTATE\n",i);
		}else{		
			printf("\nwaitqueue %d is empty!\n",i);
		}

		for(p=head[i]; p!=NULL; p=p->next) {
			printf("%d\t%d\t%d\n", p->job->jid, p->job->pid, p->job->state);
		}
	}
#endif
#ifdef UPD_DEBUG														//liuhaibo
	printf("任务六：\nBEFORE UPDATEALL:\n");
	if(current) {
		printf("current process: \nJOBID\tPID\tSTATE\n%d\t%d\t%d\n", current->job->jid,current->job->pid,current->job->state);
	}
	else {
		printf("no current process!\n");
	}
	for (i=0;i<3;++i){
		if(head[i]){		
			printf("\nwaitqueue %d: \nJOBID\tPID\tSTATE\n",i);
		}else{		
			printf("\nwaitqueue %d is empty!\n",i);		
		}

		for(p=head[i]; p!=NULL; p=p->next) {
			printf("%d\t%d\t%d\n", p->job->jid, p->job->pid, p->job->state);
		}
	}
#endif

	updateall();

#ifdef UPD_DEBUG														//liuhaibo
	printf("任务六：\nAFTER UPDATEALL:\n");
	if(current) {
		printf("current process: \nJOBID\tPID\tSTATE\n%d\t%d\t%d\n", current->job->jid,current->job->pid,current->job->state);
	}
	else {
		printf("no current process!\n");
	}
	for (i=0;i<3;++i){
		if(head[i]){		
			printf("\nwaitqueue %d: \nJOBID\tPID\tSTATE\n",i);
		}else{		
			printf("\nwaitqueue %d is empty!\n",i);
		}

		for(p=head[i]; p!=NULL; p=p->next) {
			printf("%d\t%d\t%d\n", p->job->jid, p->job->pid, p->job->state);
		}
	}
#endif
#ifdef DEBUG
	printf("Select which job to run next\n");
#endif
	if (current == NULL || current->job->state == DONE ||
		current->job->run_time == quantum[current->job->level]) {
		/* 选择高优先级作业 */
		next=jobselect();
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    if(next) {
		printf("TASK_8:next process: \nJOBID\tPID\tSTATE\n%d\t%d\t%d\n", next->job->jid,next->job->pid,next->job->state);
	}
	else {
		printf("TASK_8:no next process!\n");
	}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



#ifdef DEBUG
	printf("Switch to the next job!\n");
#endif
		/* 作业切换 */
		jobswitch();
	}
}

int allocjid()
{
	return ++jobid;
}

void updateall()
{
	int i;
	struct waitqueue *p,*prev,*q;

	/* 更新作业运行时间 */
	if(current)
		current->job->run_time += 1; /* 加1代表1000ms */

	/* 更新作业等待时间及优先级 */
	for (i=0;i<3;++i){
		for (prev = head[i], p = head[i]; p != NULL; prev = p,p = p->next){
			p->job->wait_time += 1000;
			if (p->job->wait_time >= 10000 && i != 0){
				p->job->level=i-1;
				p->job->wait_time = 0;
				if (head[i]==p) head[i]=head[i]->next;
				else prev->next = p->next;
				p->next=NULL;
				if (head[i-1]){
					for(q=head[i-1];q->next != NULL; q=q->next);
					q->next=p;
				} else {
					head[i-1]=p;
				}
			}
		}
	}
}

struct waitqueue* jobselect()
{
	int i;
	struct waitqueue *select = NULL;
	for (i=0;i<3;++i){
		if(head[i]){
			select=head[i];
			head[i]=head[i]->next;
			select->next=NULL;
			break;
		}
	}
	return select;
}

void jobswitch()
{
	struct waitqueue *p;
	int i;

#ifdef DEBUG
	printf("BEFORE JOBSWITCH:\n");
	if(current) {
		printf("current process: \nJOBID\tPID\tSTATE\n%d\t%d\t%d\n", current->job->jid,current->job->pid,current->job->state);
	}
	else {
		printf("no current process!\n");
	}
	for (i=0;i<3;++i){
		if(head[i]){		
			printf("\nwaitqueue %d: \nJOBID\tPID\tSTATE\n",i);
		}else{		
			printf("\nwaitqueue %d is empty!\n",i);
		}

		for(p=head[i]; p!=NULL; p=p->next) {
			printf("%d\t%d\t%d\n", p->job->jid, p->job->pid, p->job->state);
		}
	}
#endif

	if(current && current->job->state == DONE){ /* 当前作业完成 */
		/* 作业完成，删除它 */
		for(i = 0;(current->job->cmdarg)[i] != NULL; i++){
			free((current->job->cmdarg)[i]);
			(current->job->cmdarg)[i] = NULL;
		}
		/* 释放空间 */
		free(current->job->cmdarg);
		free(current->job);
		free(current);

		current = NULL;
	}

	if(next == NULL && current == NULL){ /* 没有作业要运行 */
		;
	}
	else if (next != NULL && current == NULL){ /* 开始新的作业 */
		printf("begin start new job\n");
		printf("%d\n",next->job->pid);
		current = next;
		next = NULL;
		current->job->state = RUNNING;
		kill(current->job->pid,SIGCONT);
		
	}
	else if (next != NULL && current != NULL){ /* 切换作业 */

		printf("switch to Pid: %d\n",next->job->pid);
		kill(current->job->pid,SIGSTOP);
		current->job->curpri = current->job->defpri;
		current->job->wait_time = 0;
		current->job->state = READY;

		if (current->job->run_time >= quantum[current->job->level]){
				current->job->run_time=0;
				if (current->job->level<2) ++current->job->level;
		}
		i=current->job->level;
		printf("level:%d-------\n",i);
		/* 放回等待队列 */
		if(head[i]){
			for(p = head[i]; p->next != NULL; p = p->next);
			p->next = current;
		}else{
			head[i] = current;
		}
		current = next;
		next = NULL;
		current->job->state = RUNNING;
		current->job->wait_time = 0;
		kill(current->job->pid,SIGCONT);
		
	}else{ /* next == NULL且current != NULL，不切换 */
		current->job->run_time = 0;
	}

#ifdef DEBUG
	printf("\nAFTER JOBSWITCH:\n");
	if(current) {
		printf("current process: \nJOBID\tPID\tSTATE\n%d\t%d\t%d\n", current->job->jid,current->job->pid,current->job->state);
	}
	else {
		printf("no current process!\n");
	}
	for (i=0;i<3;++i){
		if(head[i]){		
			printf("\nwaitqueue %d: \nJOBID\tPID\tSTATE\n",i);
		}else{		
			printf("\nwaitqueue %d is empty!\n",i);		
		}

		for(p=head[i]; p!=NULL; p=p->next) {
			printf("%d\t%d\t%d\n", p->job->jid, p->job->pid, p->job->state);
		}
	}
#endif
}

void sig_handler(int sig,siginfo_t *info,void *notused)
{
	int status;
	int ret;
	int i;
	//task 10:
#ifdef DEBUG
		struct waitqueue *p;
#endif

	switch (sig) {
case SIGVTALRM: /* 到达计时器所设置的计时间隔 */
	scheduler();
	//task 2
#ifdef DEBUG
		printf("SIGVTALRM RECEIVED!\n");
#endif
	return;
case SIGCHLD: /* 子进程结束时传送给父进程的信号 */
	ret = waitpid(-1,&status,WNOHANG);
	if (ret == 0)
		return;
	if(WIFEXITED(status)){
		current->job->state = DONE;
		printf("normal termation, exit status = %d\n",WEXITSTATUS(status));
	}else if (WIFSIGNALED(status)){
		printf("abnormal termation, signal number = %d\n",WTERMSIG(status));
	}else if (WIFSTOPPED(status)){
		printf("child stopped, signal number = %d\n",WSTOPSIG(status));
	}

	//task 10:
#ifdef DEBUG
	if(current) {
		printf("current process: %d\n", current->job->pid);
	}
	else {
		printf("no current process!\n");
	}
	
	printf("JOBID\ttPID\tSTATE\n");
	for (i=0;i<3;++i) {
		
		for(p=head[i]; p!=NULL; p=p->next) {
			printf("%d\t%d\t%d\n", p->job->jid, p->job->pid, p->job->state);
		}
	}
#endif

	return;
	default:
		return;
	}
}

void do_enq(struct jobinfo *newjob,struct jobcmd enqcmd)
{
	int i=0,pid;
	char *offset,*argvec,*q;
	char **arglist;
	sigset_t zeromask;

	sigemptyset(&zeromask);

	/* 封装jobinfo数据结构 */
	newjob = (struct jobinfo *)malloc(sizeof(struct jobinfo));
	newjob->jid = allocjid();
	newjob->defpri = enqcmd.defpri;
	newjob->curpri = enqcmd.defpri;
	newjob->ownerid = enqcmd.owner;
	newjob->state = READY;
	newjob->create_time = time(NULL);
	newjob->wait_time = 0;
	newjob->run_time = 0;
	newjob->level=0;
	arglist = (char**)malloc(sizeof(char*)*(enqcmd.argnum+1));
	newjob->cmdarg = arglist;
	offset = enqcmd.data;
	argvec = enqcmd.data;
	while (i < enqcmd.argnum){
		if(*offset == ':'){
			*offset++ = '\0';
			q = (char*)malloc(offset - argvec);
			strcpy(q,argvec);
			arglist[i++] = q;
			argvec = offset;
		}else
			offset++;
	}

	arglist[i] = NULL;

#ifdef DEBUG

	printf("enqcmd argnum %d\n",enqcmd.argnum);
	for(i = 0;i < enqcmd.argnum; i++)
		printf("parse enqcmd:%s\n",arglist[i]);

#endif

	/*为作业创建进程*/
	if((pid=fork())<0)
		error_sys("enq fork failed");

	if(pid==0){
		/*阻塞子进程,等等执行*/
		raise(SIGSTOP);
#ifdef DEBUG

		printf("begin running\n");
		for(i=0;arglist[i]!=NULL;i++)
			printf("arglist %s\n",arglist[i]);
#endif

		/*复制文件描述符到标准输出*/
		//dup2(globalfd,1);
		/* 执行命令 */
		if(execv(arglist[0],arglist)<0)
			printf("exec failed\n");
		exit(1);
	}else{
		newjob->pid=pid;
		waitpid(pid,NULL,0);
	}
	/*向等待队列中增加新的作业*/
	next = (struct waitqueue*)malloc(sizeof(struct waitqueue));
	next->job=newjob;
	next->next =NULL;
	jobswitch();
}

void do_deq(struct jobcmd deqcmd)
{
	int deqid,i;
	struct waitqueue *p,*prev,*select,*selectprev;
	sigset_t zeromask;

	sigemptyset(&zeromask);
	
	deqid=atoi(deqcmd.data);

#ifdef DEBUG
	printf("deq jid %d\n",deqid);
#endif

	/*current jodid==deqid,终止当前作业*/
	if (current && current->job->jid ==deqid){
		printf("terminate current job\n");
		kill(current->job->pid,SIGKILL);
		for(i=0;(current->job->cmdarg)[i]!=NULL;i++){
			free((current->job->cmdarg)[i]);
			(current->job->cmdarg)[i]=NULL;
		}
		free(current->job->cmdarg);
		free(current->job);
		free(current);
		current=NULL;
	}
	else{ /* 或者在等待队列中查找deqid */
		for (i=0;i<3;++i){
			select=NULL;
			selectprev=NULL;
			if(head[i]){
				for(prev=head[i],p=head[i];p!=NULL;prev=p,p=p->next){
					if(p->job->jid==deqid){
						select=p;
						selectprev=prev;
						break;
					}
				}
				if (select){
					selectprev->next=select->next;
					if(select==selectprev)
						head[i]=head[i]->next;
					break;
				}	
			}
		}
		if(select){
			for(i=0;(select->job->cmdarg)[i]!=NULL;i++){
				free((select->job->cmdarg)[i]);
				(select->job->cmdarg)[i]=NULL;
			}
			free(select->job->cmdarg);
			free(select->job);
			free(select);
			select=NULL;
		}
		else{
			printf("job %d not found!\n",deqid);
		}
	}
}

void do_stat(struct jobcmd statcmd)
{
	int i;
	struct waitqueue *p;
	char timebuf[BUFLEN];
	sigset_t zeromask;

	sigemptyset(&zeromask);
	/*
	*打印所有作业的统计信息:
	*1.作业ID
	*2.进程ID
	*3.作业所有者
	*4.作业运行时间
	*5.作业等待时间
	*6.作业创建时间
	*7.作业状态
	*/

	/* 打印信息头部 */
	printf("JOBID\tPID\tOWNER\tRUNTIME\tWAITTIME\tCREATTIME\t\tSTATE\n");
	if(current){
		strcpy(timebuf,ctime(&(current->job->create_time)));
		timebuf[strlen(timebuf)-1]='\0';
		printf("%d\t%d\t%d\t%d\t%d\t%s\t%s\n",
			current->job->jid,
			current->job->pid,
			current->job->ownerid,
			current->job->run_time,
			current->job->wait_time,
			timebuf,"RUNNING");
	}
	for (i=0;i<3;++i) {
		for(p=head[i];p!=NULL;p=p->next){
			strcpy(timebuf,ctime(&(p->job->create_time)));
			timebuf[strlen(timebuf)-1]='\0';
			printf("%d\t%d\t%d\t%d\t%d\t%s\t%s\n",
				p->job->jid,
				p->job->pid,
				p->job->ownerid,
				p->job->run_time,
				p->job->wait_time,
				timebuf,
				"READY");
		}
	}
}

int main()
{
	struct timeval interval;
	struct itimerval new,old;
	struct stat statbuf;
	struct sigaction newact,oldact1,oldact2;

#ifdef DEBUG
	printf("DEBUG IS OPEN!");
#endif

	if(stat("/tmp/server",&statbuf)==0){
		/* 如果FIFO文件存在,删掉 */
		if(remove("/tmp/server")<0)
			error_sys("remove failed");
	}

	if(mkfifo("/tmp/server",0666)<0)
		error_sys("mkfifo failed");
	/* 在非阻塞模式下打开FIFO */
	if((fifo=open("/tmp/server",O_RDONLY|O_NONBLOCK))<0)
		error_sys("open fifo failed");

	/* 建立信号处理函数 */
	newact.sa_sigaction=sig_handler;
	sigemptyset(&newact.sa_mask);
	newact.sa_flags=SA_SIGINFO;
	sigaction(SIGCHLD,&newact,&oldact1);
	sigaction(SIGVTALRM,&newact,&oldact2);

	/* 设置时间间隔为1000毫秒 */
	interval.tv_sec=1;
	interval.tv_usec=0;

	new.it_interval=interval;
	new.it_value=interval;
	setitimer(ITIMER_VIRTUAL,&new,&old);

	while(siginfo==1);

	close(fifo);
	//close(globalfd);
	return 0;
}
