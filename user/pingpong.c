#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"


int main(int argc, char *argv[]){

    int parent_fd[2];
    int child_fd[2];

    pipe(parent_fd);
    pipe(child_fd);
    // int forkpid;
    char buffer[100];
    if(fork()!=0){
        
        // parents
        
        // forkpid= getpid();
        // printf("this pid is %d\n",forkpid);
        write(parent_fd[1],"ping",strlen("ping"));
        read(child_fd[0],buffer,strlen("ping"));
        printf("%d: received %s\n",getpid(),buffer);
    }
    else{

        //child
        write(child_fd[1],"pong",strlen("pong"));
        read(parent_fd[0],buffer,strlen("pong"));
        printf("%d: received %s\n",getpid(),buffer);
    }

    exit();
}