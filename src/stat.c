//��ҵ״̬�鿴����

#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include "job.h"

/* 
 * �����﷨��ʽ
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

    //�����������������ʾ�����÷���Ϣ
	if(argc!=1)
	{
		usage();
		return 1;
	}
    
    //��¼��ҵ״̬�鿴����
	statcmd.type=STAT;
	statcmd.defpri=0;
	statcmd.owner=getuid();
	statcmd.argnum=0;

    //��fifo�ļ�
	if((fd=open("/tmp/server",O_WRONLY))<0)
		error_sys("stat open fifo failed");

    //��fifo�ļ�д����
	if(write(fd,&statcmd,DATALEN)<0)
		error_sys("stat write failed");

	close(fd);
	return 0;
}
