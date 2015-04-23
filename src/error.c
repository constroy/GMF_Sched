#include <string.h>
#include <errno.h>
#include "job.h"

/* 错误处理 */
void error_doit(int errnoflag,const char *fmt,va_list ap)
{
  int errno_save;
  char buf[BUFLEN];

  errno_save=errno;
  
  //送格式化输出到buf中
  vsprintf(buf,fmt,ap);
  
  //如果要输出错误代码，则将错误代码格式化追加到buf之后，并添加换行符
  if (errnoflag)
    sprintf(buf+strlen(buf),":%s",strerror(errno_save));
  strcat(buf,"\n");
  
  //刷新输出缓冲区
  fflush(stdout);
  fputs(buf,stderr);
  fflush(NULL);
  return;
}

//系统出错处理函数
void error_sys(const char *fmt,...)
{
  va_list ap;
  
  //获取参数列表的参数
  va_start(ap,fmt);
  //调用出错处理函数
  error_doit(1,fmt,ap);
  //使用完毕后结束
  va_end(ap);
  exit(1);
}

void error_quit(const char *fmt,...)
{
  va_list ap;
  
  va_start(ap,fmt);
  error_doit(0,fmt,ap);
  va_end(ap);
  exit(1);
}

void error_msg(const char *fmt,...)
{
  va_list ap;
  
  va_start(ap,fmt);
  error_doit(0,fmt,ap);
  va_end(ap);
  return;
}
