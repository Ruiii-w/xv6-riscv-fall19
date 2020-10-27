
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
int main(int argc,char *argv[]){

    if(argc != 2){
        write(0,"Command Error",strlen("Command Error"));
        exit();
    }

    int x = atoi(argv[1]);

    sleep(x);
    write(1,"(nothing happen for a little while)",strlen("(nothing happen for a little while)"));
    exit();
}

