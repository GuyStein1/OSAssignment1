#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int pid;

  printf("invalid pid: %d\n", co_yield(9999, 1));
  printf("self yield: %d\n", co_yield(getpid(), 1));

  pid = fork();
  if(pid < 0){
    printf("fork failed\n");
    exit(1);
  }

  if(pid == 0){
    for(;;)
      sleep(100);
  }

  sleep(1);
  kill(pid);
  printf("killed target: %d\n", co_yield(pid, 1));
  wait(0);

  exit(0);
}
