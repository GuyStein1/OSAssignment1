#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define ROUNDS 2000
#define PARENT_VALUE 222
#define CHILD_VALUE 111

static void
run_case(char *name, int child_signals_first)
{
  int parent = getpid();
  int pfd[2];
  int child;
  int status;
  char ready;
  int i;

  if(pipe(pfd) < 0){
    printf("%s: pipe failed\n", name);
    exit(1);
  }

  child = fork();
  if(child < 0){
    printf("%s: fork failed\n", name);
    exit(1);
  }

  if(child == 0){
    close(pfd[0]);
    if(child_signals_first)
      write(pfd[1], "x", 1);
    close(pfd[1]);

    for(;;){
      int got = co_yield(parent, CHILD_VALUE);
      if(got != PARENT_VALUE){
        printf("%s: child got %d\n", name, got);
        exit(1);
      }
    }
  }

  close(pfd[1]);
  if(child_signals_first && read(pfd[0], &ready, 1) != 1){
    printf("%s: readiness read failed\n", name);
    kill(child);
    wait(0);
    exit(1);
  }
  close(pfd[0]);

  for(i = 0; i < ROUNDS; i++){
    int got = co_yield(child, PARENT_VALUE);
    if(got != CHILD_VALUE){
      printf("%s: parent got %d at round %d\n", name, got, i);
      kill(child);
      wait(0);
      exit(1);
    }
  }

  kill(child);
  wait(&status);
  printf("%s: passed %d handoffs\n", name, ROUNDS);
}

int
main(int argc, char *argv[])
{
  run_case("parent-first", 0);
  run_case("child-first", 1);
  printf("co_stress_test: passed\n");
  exit(0);
}
