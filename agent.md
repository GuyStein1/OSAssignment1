# Agent Instructions — OS Assignment 1: Processes and Scheduling

## Assignment Overview

This is **BGU Operating Systems 2026, Assignment 1**.  
The base system is **xv6-riscv** (MIT's RISC-V port of xv6).  
All work takes place inside a Docker dev-container using VS Code Dev Containers.

---

## Submission Rules (from PDF + Moodle)

- Submit **in pairs only**, via Moodle. Email submissions are not accepted.
- Submit **a single copy of the entire xv6 source tree** (including all changes for all tasks) as a `.tar.gz` or `.zip` file.
  - This means: the full xv6 directory, **not** just the changed files.
  - Include `co_test.c` and every file you modified.
- Before submitting, run `make clean` to remove compiled files and the `obj/` directory.
- Code must compile **without errors or warnings** and run correctly.
- Comment your code and explain design choices — graders may ask about them in a grading session.
- Theoretical questions throughout the PDF are **not for submission**, but you should understand the answers for the grading session.

---

## Tasks

### Task 0 — Compile and Run xv6 (no submission required)

1. Clone the repository: `git clone https://github.com/BGU-CS-OS/xv6-riscv.git`
2. Open in VS Code → reopen in dev container.
3. Build and run: `make qemu`
4. Exit QEMU: `Ctrl-A` then `X`.

**Submission:** Nothing to submit.

---

### Task 1 — Hello World (no submission required)

1. Create `user/helloworld.c` that prints `Hello World xv6` to the screen.
2. Add `_helloworld` to the `UPROGS` list in `Makefile`.
3. Build with `make qemu` and run `helloworld` in the xv6 shell.

**Hint:** Look at `user/echo.c` and how `_echo` appears in the Makefile.

**Submission:** Nothing to submit.

---

### Task 2 — `memsize` System Call (no submission required)

Add a new system call `int memsize(void)` that returns the size of the calling process's memory in bytes.

**Files to modify:**
- `kernel/syscall.h` — add a new syscall number for `memsize`.
- `kernel/syscall.c` — register the new syscall.
- `kernel/sysproc.c` — implement `sys_memsize` (read process size from the PCB).
- `user/usys.pl` — add the userspace stub entry.
- `user/user.h` — add the userspace declaration.

**Test program** (`user/memsize_test.c`):
1. Print memory size before allocation.
2. `malloc` 20 000 bytes.
3. Print memory size after allocation.
4. `free` the allocated memory.
5. Print memory size after free.

**Submission:** Nothing to submit.

---

### Task 3 — `co_yield` Coroutine System Call (**THIS IS THE GRADED TASK**)

#### What it does

`int co_yield(int pid, int value)` lets a process hand off the CPU **directly** to another process (identified by `pid`) and pass it an integer `value`, bypassing the standard xv6 scheduler.

- The `value` passed by the calling process becomes the **return value** of `co_yield` in the target process.
- If the target has not yet called `co_yield` itself, the caller **sleeps** until the target is ready.
- Once both sides have called `co_yield`, execution transfers directly from caller to target (no scheduler involvement).
- A process sleeping inside `co_yield` waiting for a counterpart **must be invisible to the scheduler** — the scheduler must always skip such processes.

#### Return values

| Situation | Return value |
|-----------|-------------|
| Success | The positive integer passed by the other process |
| Any error | `-1` |

#### Error conditions (return -1)

- `pid` does not exist or belongs to a killed process.
- `pid` is not a valid PID (zero, negative).
- `pid` equals the calling process's own PID (self-yield is not allowed).
- Any other condition you deem appropriate (document it in code).

#### Test code (must be placed in `user/co_test.c`)

```c
int pid1 = getpid();      // Parent PID
int pid2 = fork();        // Child PID (returned to parent); 0 in child

if (pid2 == 0) {          // Child
    for (;;) {
        int value = co_yield(pid1, 1);
        printf("Child received: %d\n", value);   // Should print 2
    }
} else {                  // Parent
    for (;;) {
        int value = co_yield(pid2, 2);
        printf("parent received: %d\n", value);  // Should print 1
    }
}
```

#### Constraints (from PDF — mandatory)

1. **Single CPU**: Reduce QEMU to 1 CPU by editing `Makefile` (find the `CPUS` variable and set it to `1`). The implementation only needs to be correct on a single CPU.
2. **No struct changes**: Do **not** modify `struct proc`, do **not** add new process states or new fields to any process structure.
3. **No new global/process-level kernel data structures**: Do **not** add or modify any global or process-level kernel data structures.
4. **Direct switching**: The final implementation must perform a direct process-to-process context switch using `swtch`, skipping the RUNNABLE state entirely and bypassing the scheduler loop.
5. **Do not break** the existing scheduler, process model, or any existing system calls.

#### Clarifications from Moodle Q&A

- **Direct switching meaning**: The switch from process A to process B must happen entirely within the `co_yield` syscall — you must not return to the scheduler loop in between. The only acceptable exceptions are acquiring/releasing locks around edge cases.  
  *(Source: Q1 — לוטם סקירה, 26 April 2026)*

- **After the handoff**: Once both processes have exchanged values and control, normal scheduling resumes as usual.  
  *(Source: Q2 — לוטם סקירה, 26 April 2026)*

- **Only the given test matters**: Your code will be graded **only** against the test provided in the PDF. You may add more tests, but they will not be evaluated.  
  *(Source: Q3 — לוטם סקירה, 26 April 2026)*

- **Using `sleep`/`wakeup`/`sched`**: You may use these as a starting prototype (Proposed approach step 2), but the **final** implementation must use direct `swtch`-based switching (Proposed approach step 5).  
  *(Source: Q7 — לוטם סקירה, 15 April 2026)*

- **Value overwriting**: When the first process calls `co_yield`, its value is held until the second process calls `co_yield`. Think about where to store it. Study tuts 1 & 2 for the right mental model. Refer to PDF pages 11–12 for the full scenario.  
  *(Source: Q8 — לוטם סקירה, 15 April 2026)*

- **Timer interrupt / panic: release**: If you are getting a `panic: release` from a timer interrupt, this is a symptom of **incorrect implementation**, not an edge case. When you hold a lock, interrupts are disabled — the problem is likely that locks are being acquired/released in the wrong order or on the wrong CPU context.  
  *(Source: Q9 — לוטם סקירה, 14 April 2026)*

#### Proposed implementation approach (from PDF)

1. Read and understand `kernel/proc.c`, context switching (`kernel/swtch.S`), and the sleep/wakeup protocol.
2. First implement `co_yield` using the normal scheduler (`sleep`/`wakeup`) — don't worry about direct switching yet. Get the logic right: sleeping, waking, passing values.
3. Verify the basic version works with `co_test`.
4. Verify locking and sleep/wakeup protocols are correct.
5. Modify to perform **direct process switching** using `swtch`, bypassing the scheduler.
6. Test thoroughly.

#### Error conditions to test (required)

- Yield to a non-existent PID.
- Yield to a killed process.
- Yield to self (same PID as caller).

#### Files likely to modify for Task 3

| File | Purpose |
|------|---------|
| `kernel/syscall.h` | Add `SYS_co_yield` number |
| `kernel/syscall.c` | Register the syscall handler |
| `kernel/sysproc.c` | Implement `sys_co_yield` |
| `kernel/proc.c` | Core switching logic (possibly helpers) |
| `kernel/defs.h` | Expose any new helper functions |
| `user/usys.pl` | Add userspace stub |
| `user/user.h` | Add userspace declaration |
| `user/co_test.c` | **New file** — the test program |
| `Makefile` | Add `_co_test` to `UPROGS`; set `CPUS := 1` |

---

## Build & Test Commands

```bash
# Build and run
make qemu

# Clean before submission
make clean

# Run specific userspace program inside xv6 shell
co_test
```

---

## Notes for Grading Session

Be prepared to explain:
- Why you chose a particular storage location for the passed value (given you cannot add fields to `struct proc` or global structures).
- How locking ensures correctness during the rendezvous.
- How your direct `swtch` call bypasses the scheduler.
- How the scheduler is prevented from scheduling a process that is sleeping inside `co_yield`.
- How each error condition is detected and handled.
- Any non-trivial edge cases you chose to handle or consciously not handle (and why).
