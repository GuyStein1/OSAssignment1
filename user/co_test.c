#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int pid1 = getpid();
  int pid2 = fork();

  if(pid2 < 0){
    printf("fork failed\n");
    exit(1);
  }

  if(pid2 == 0){
    for(;;){
      int value = co_yield(pid1, 1);
      printf("Child received: %d\n", value);
    }
  } else {
    for(;;){
      int value = co_yield(pid2, 2);
      printf("parent received: %d\n", value);
    }
  }
}
