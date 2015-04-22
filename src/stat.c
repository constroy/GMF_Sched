//作业状态查看命令

#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include "job.h"

/* 
 * 命令语法格式
 *     stat
 */
void usage()
{
	printf("Usage: stat\n");		
}

int main(int argc,char *argv[])
{
	struct jobcmd statcmd;
	int fd;

    //命令参数个数出错，提示命令用法信息
	if(argc!=1)
	{
		usage();
		return 1;
	}
    
    //记录作业状态查看命令
	statcmd.type=STAT;
	statcmd.defpri=0;
	statcmd.owner=getuid();
	statcmd.argnum=0;

    //打开fifo文件
	if((fd=open("/tmp/server",O_WRONLY))<0)
		error_sys("stat open fifo failed");

    //向fifo文件写数据
	if(write(fd,&statcmd,DATALEN)<0)
		error_sys("stat write failed");

	close(fd);
	return 0;
}
