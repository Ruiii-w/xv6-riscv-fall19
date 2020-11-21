// Shell.

#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define MAXARGNUM 20

// Parsed command representation
#define EXEC  1
#define REDIR 2
#define PIPE  3
#define LIST  4
#define BACK  5
struct cmd {
  int type;
};
char whitespace[] = " \t\r\n\v";

int getcmd(char *, int);
void getargs(char*, int*, char* []);
void runcmd(int ,char* []);
void cndtPipe(int ,char* []);


int
getcmd(char *buf, int nbuf)
{
  fprintf(2, "@ ");
  memset(buf, 0, nbuf);
  gets(buf, nbuf);
  if(buf[0] == 0) // EOF
    return -1;
  return 0;
}

void
getargs(char* buf,int* argc, char* argv[])
{
    int wordnum = 0;
    int i;
    for(i = 0; buf[i]!='\n'&& buf[i]!='\0';i++)
    {
        //忽略命令之前的空格
        // while(buf[i]=='\t'||buf[i]==' '||buf[i]=='\n')
        while(strchr(whitespace,buf[i]) != 0)
        {
            i++;
        }

        //0123456789
        //         echo afoajfoaj
        argv[wordnum++] = buf + i; //e所在位置为基地址开始
        // fprintf(2,"%d.. %s\n", wordnum ,argv[wordnum-1]);
        // while(buf[i]!='\t'&&buf[i]!=' '&&buf[i]!='\n')
        while(strchr(whitespace,buf[i]) == 0)  // strchr未找到返回值为0
        {
            i++;
        }
        buf[i] = '\0'; //遇到' '将其置为结束符'\0' 这轮循环得到的argv为echo
        
        //结束之后执行i++ 不影响循环条件
    }
    *argc = wordnum;
    argv[wordnum] = 0;  //最后一个参数置0 

    // fprintf(2,"%d.. %s\n", wordnum ,argv[wordnum]);
    // for(int i = 0;i<*argc+1;i++)
    // {
    //     fprintf(2,"%d.. %s\n", i ,argv[i]);
    // }
}


void
cndtPipe(int argc, char* argv[]){
    int i;
    for (i = 0; i < argc; i++)
    {
        if(!strcmp(argv[i],"|"))
        {
            argv[i] = 0;  // 管道标识的位置作为 管道参数的分界
            break;
        }
    }

    // for(int i = 0;i<*argc+1;i++)
    // {
    //     fprintf(2,"%d.. %s\n", i ,argv[i]);
    // }t

    int p[2];
    pipe(p);
    if(fork() == 0){
      close(1); //关闭标准
      dup(p[1]);
      close(p[0]);
      close(p[1]);
      runcmd(i,argv);
    }
    else{
      close(0);
      dup(p[0]);
      close(p[0]);
      close(p[1]);
      runcmd(argc-i-1,argv+i+1);
    }
    close(p[0]);
    close(p[1]);
}


void
runcmd(int argc, char* argv[]){
    
    // for(int i = 0;i<argc+1;i++)
    // {
    //     fprintf(2,"%d. %s\n", i ,argv[i]);
    // }

    // for (int i = 1; i < argc; i++)
    // {
    //     if(!strcmp(argv[i],"|"))
    //     {
    //         cndtPipe(argc,argv);
    //     }
    // }
    

    for(int i =1;i<argc;i++)
    {
        // if(!strcmp(argv[i],"|"))
        // {
        //     cndtPipe(argc,argv);
        // }
        if(!strcmp(argv[i],"|"))
        {
            cndtPipe(argc,argv);
        }

        if(!strcmp(argv[i],">"))
        {//输出重定向
            close(1);
            open(argv[i+1],O_CREATE|O_WRONLY);   //O_WRONLY 只写
            argv[i] = 0;   // 重定向标识的位置作为参数结尾， 否则无限执行
            
        }
        if(!strcmp(argv[i],"<"))
        {//输入重定向
            close(0);
            open(argv[i+1], O_RDONLY);  //O_RDONLY 只读
            argv[i] = 0;
        }
    }
    // for(int i = 0;i<argc+1;i++)
    // {
    //     fprintf(2,"%d.. %s\n", i ,argv[i]);
    // }
    exec(argv[0],argv);
}


// main 函数主要参考sh.c中的main
int
main(void)
{
    static char buf[100];
    int argc = 0;
    char* argv[MAXARGNUM];
    // Read and run input commands.
    while(getcmd(buf, sizeof(buf)) >= 0){
        // cd ????
        if(buf[0] == 'c' && buf[1] == 'd' && buf[2] == ' '){
        // Chdir must be called by the parent, not the child.
        buf[strlen(buf)-1] = 0;  // chop \n
        if(chdir(buf+3) < 0)
            fprintf(2, "cannot cd %s\n", buf+3);
        continue;
        }
        
        if(fork() == 0){  
            // runcmd(parsecmd(buf));

            // int argc;
            // char* argv[MAXARGNUM];
            getargs(buf,&argc,argv);    // 读取参数和参数个数
            runcmd(argc,argv);
        }
        wait(0);  //保证一个命令执行完再执行下个命令  fork用子程序执行  父程序等待,结束后再进入下一个循环
    }
    exit(0);
}