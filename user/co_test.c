#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(void) {
    int pid1 = getpid();
    int pid2 = fork();

    if (pid2 == 0) {  // Child
        for (int i=0; i<3; i++) {
            int value = co_yield(pid1, 1);
            printf("Child received: %d\n", value);  // Should print 2
        }
        
        // Let parent finish the loop
        sleep(10);
        exit(0);
    } else {  // Parent
        for (int i=0; i<3; i++) {
            int value = co_yield(pid2, 2);
            printf("parent received: %d\n", value);  // Should print 1
        }
        
        wait(0);
        
        // Error condition tests
        printf("\n--- Error Tests ---\n");
        int r = co_yield(99999, 42);
        printf("non-existent PID: %d (expected -1)\n", r);

        r = co_yield(getpid(), 42);
        printf("self-yield: %d (expected -1)\n", r);

        int child = fork();
        if (child == 0) { exit(0); }
        wait(0); // child is now dead
        r = co_yield(child, 42);
        printf("dead process: %d (expected -1)\n", r);
    }
    exit(0);
}
