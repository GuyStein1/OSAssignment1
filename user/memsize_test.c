#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(void) {
    printf("Memory before malloc: %d bytes\n", memsize());

    char *buf = malloc(20000);
    printf("Memory after malloc:  %d bytes\n", memsize());

    free(buf);
    printf("Memory after free:    %d bytes\n", memsize());

    exit(0);
}
