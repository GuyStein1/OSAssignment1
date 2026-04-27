#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  void *buf;

  printf("memory before allocation: %d bytes\n", memsize());

  buf = malloc(20000);
  if(buf == 0){
    printf("malloc failed\n");
    exit(1);
  }

  printf("memory after allocation: %d bytes\n", memsize());

  free(buf);

  printf("memory after free: %d bytes\n", memsize());

  exit(0);
}
