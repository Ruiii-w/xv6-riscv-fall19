#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"
int main(int argc,char *argv[])
{
    char *argvv = char[MAXPATH];
    char mediate = char[MAXPATH][MAXPATH];
    
    for(int i =0;i<MAXPATH;i++)
    {
        argvv[i] = mediate[i];
    }

    for(int i =1;i<argc;i++)
    {
        strcpy(mediate[i-1],argv[i]);
    }

    
}