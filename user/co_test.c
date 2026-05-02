#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

static void
run_error_tests(void)
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
}

int
main(int argc, char *argv[])
{
  int pid1;
  int pid2;

  run_error_tests();

  pid1 = getpid();
  pid2 = fork();

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
