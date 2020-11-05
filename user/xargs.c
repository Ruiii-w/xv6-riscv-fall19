#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"
int main(int argc,char *argv[])
{
    char bufread[FSSIZE]; 
    char bufref[MAXARG][MAXARG];
    char *refs[MAXARG];

    for(int i =0;i<32;i++)
    {
        refs[i] = bufref[i];
    }
    for(int i =1; i<argc ;i++)
    {
        strcpy(bufref[i-1],argv[i]);  //xargs 后面的所有参数 第一个参数为命令
    }
    int n ;
    while((n=read(0,bufread,sizeof(bufread)))>0)
    {
        int pos = argc - 1;
        char* ref = bufref[pos];    //从pos(argc-1)开始，为了保留xargs后面的参数
        for(char *p = bufread;*p;p++)
        {
            if(*p == ' '||*p =='\n')
            {
                *ref = '\0';
                pos ++;
                ref = bufref[pos];
            }
            else
            {
                // printf("\n ref is %s\n",*ref);
                *ref++ = *p;
            }
        }
        *ref = '\0';  
        pos++;
        refs[pos] = 0;  //最后一个参数argv[size-1]必须为0，否则将执行失败
        // 使用fork()和exec()系统对每一行输入调用命令
        if(fork())
        {
            wait();  // 使用wait()等待子进程运行完命令
        }
        else
        {
            exec(refs[0],refs);  //字程序
        } 
    }
    if(n < 0)
    {
        printf("read error");
        exit();
    }
    exit();
}