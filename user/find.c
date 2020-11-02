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
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    // printf("file: %s %d %d %l\n", fmtname(path), st.type, st.ino, st.size);
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      // printf("de.name is %s",de.name);
      // printf("de.name.length is %d\n",strlen(de.name));
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf("ls: cannot stat %s\n", buf);
        continue;
      }

//    不要递归进入”.”和”..”
      if(strlen(de.name) == 1 && de.name[0] == '.')
        continue;
      if(strlen(de.name) == 2 && de.name[0] == '.' && de.name[1] == '.')
        continue;
      // printf("dir:%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
      if(strcmp(de.name,name) == 0){
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
  // int i;

  if(argc <= 2){
    printf("error: too few arguments to function 'find\n");
  }

  find(argv[1],argv[2]);
  exit();
}
