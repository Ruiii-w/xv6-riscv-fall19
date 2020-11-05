#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

void
find(char *path, char* name)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    // printf("file: %s %d %d %l\n", fmtname(path), st.type, st.ino, st.size);
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("find error: path too long\n");
      break;
    }
    strcpy(buf, path);
    // buf 为上一级path
    p = buf+strlen(buf);
    // 在buf尾部加上'/' 相当于进入该级目录
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      // 该级目录加上文件名(若de是一个dictionary 则会作为下一级目录名)
      memmove(p, de.name, DIRSIZ);
      // printf("de.name is %s",de.name);
      // printf("de.name.length is %d\n",strlen(de.name));
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf("find error: cannot stat %s\n", buf);
        continue;
      }

//    不要递归进入”.”和”..”
      if(de.name[0] == '.' && strlen(de.name) == 1)
        continue;
      if(de.name[0] == '.' && de.name[1] == '.' && strlen(de.name) == 2)
        continue;
      // printf("dir:%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);

      //de.name 为当前目录下的文件(夹)名 
      if(strcmp(de.name, name) == 0){
        // printf("the path is %s\n",buf);
        // printf("%d\n",strlen(buf));
        printf("%s\n",buf);
      }
      find(buf, name);
    }
    break;
  }


  close(fd);
}
 
int
main(int argc, char *argv[])
{
  if(argc <= 2){
    printf("error: too few arguments to function 'find'\n");
    exit();
  }

// argv[0]为命令名   例: find . b  argv[1] = '.' argv[2] = 'b'
  find(argv[1],argv[2]);
  exit();
}
