//作业出队命令

#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include "job.h"

#define DEBUG

/* 
 * 命令语法格式
 *     deq jid
 */
void usage()
{
	printf("Usage: deq jid\n"
		"\tjid\t\t the job id\n");
}

int main(int argc,char *argv[])
{
	struct jobcmd deqcmd;
	int fd;
	
	//命令行参数个数出错
	if(argc!=2)
	{
		usage();
		return 1;
	}
    
    //记录出队命令
	deqcmd.type=DEQ;
	deqcmd.defpri=0;
	deqcmd.owner=getuid();
	deqcmd.argnum=1;

	strcpy(deqcmd.data,*++argv);
	printf("jid %s\n",deqcmd.data);

	#ifdef DEBUG
		printf("cmdtype:%d\n", deqcmd.type);
		printf("owner:%d\n", deqcmd.owner);
	#endif

    //打开fifo文件
	if((fd=open("/tmp/server",O_WRONLY))<0)
		error_sys("deq open fifo failed");

    //向fifo文件写数据
	if(write(fd,&deqcmd,DATALEN)<0)
		error_sys("deq write failed");

	close(fd);
	return 0;
}
