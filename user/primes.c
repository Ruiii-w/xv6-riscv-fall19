#include "kernel/types.h"
#include "user/user.h"

void init(){
    int i;
    for (i = 2; i < 36; i++) 
    {
        write(1, &i, sizeof(i));
    }
}

void divideJudge(int p){
    int n;
    while (read(0, &n, sizeof(n))) 
    {
        if (n % p != 0) {
            write(1, &n, sizeof(n));
        }
    }
}

// 开启pd[k]端口
void changepd(int k, int pd[])
{
    close(k); 
    // 复制pd[i]端口
    dup(pd[k]);
    close(pd[0]);
    close(pd[1]);
}

// 掉到下一级
void drop(){
    int pd[2];
    int p;
    if(read(0,&p,sizeof(p))) // 只读出暂存区第一个字符  即 各个prime数
    {
        
        printf("prime %d\n",p);
        pipe(pd);
        if(fork()!=0)
        {
            // parent
            // 只开启读通道
            changepd(0,pd); 
            drop();
        }
        else
        {
            // child
            // 只开启写通道
            changepd(1,pd);
            // 求余判断
            divideJudge(p);
        }
    }
}

int main(int argc, char* argv[]){
    int pd[2];
    pipe(pd);
    if(fork()!=0)
    {
        // parent
        // 开启读通道
        changepd(0,pd);   
        drop();
    }
    else
    {
        // child
        // 开启写通道
        changepd(1,pd);
        //初始化：写入 2-35 
        init();
    }
    exit();
}
