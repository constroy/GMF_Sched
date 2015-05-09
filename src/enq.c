#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include "job.h"
#define ENQ_DEBUG
/* 
 * 命令语法格式
 *     enq [-p num] e_file args
 */
void usage()
{
	printf("Usage: enq[-p num] e_file args\n"
		"\t-p num\t\t specify the job priority\n"
		"\te_file\t\t the absolute path of the exefile\n"
		"\targs\t\t the args passed to the e_file\n");
}

int main(int argc,char *argv[])
{
	int p=0;
	int fd;
	char c,*offset;
	struct jobcmd enqcmd;

    //命令格式出错的提示用法
	if(argc==1)
	{
		usage();
		return 1;
	}

    //若指定了优先级，获取之
	while(--argc>0 && (*++argv)[0]=='-')
	{
		while((c=*++argv[0]))
		{
			switch(c)
			{
		    //存取优先级级数
			case 'p':p=atoi(*(++argv));
			argc--;
			break;
			default:
				printf("Illegal option %c\n",c);
				return 1;
			}
		}
	}
    
    //指定优先级级数非法
	if(p<0||p>3)
	{
		printf("invalid priority:must between 0 and 3\n");
		return 1;
	}

    //记录入队命令
	enqcmd.type=ENQ;
	enqcmd.defpri=p;
	enqcmd.owner=getuid();
	enqcmd.argnum=argc;
	offset=enqcmd.data;

    //将入队命令中制定的可执行文件名及其参数用冒号隔开
	while (argc-->0)
	{
		strcpy(offset,*argv);
		strcat(offset,":");
		offset=offset+strlen(*argv)+1;
		argv++;
	}

    //输出调试信息
    #ifdef ENQ_DEBUG
		printf("enqcmd cmdtype\t%d (-1 means ENQ, -2 means DEQ, -3 means STAT)\n"
			"enqcmd owner\t%d\n"
			"enqcmd defpri\t%d\n"
			"enqcmd data\t%s\n"
			"enqcmd argnum\t%d\n",
			enqcmd.type,enqcmd.owner,enqcmd.defpri,enqcmd.data,enqcmd.argnum);

    #endif 
        
        //打开fifo文件
		if((fd=open("/tmp/server",O_WRONLY))<0)
			error_sys("enq open fifo failed");
        
        //向fifo文件写数据
		if(write(fd,&enqcmd,DATALEN)<0)
			error_sys("enq write failed");

		close(fd);
		return 0;
}

