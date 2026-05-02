# OS Assignment 1 Study Notes

This file explains what we changed for Assignment 1, why each change exists,
and what we should be ready to explain in the grading session.

The final implementation is merged into `main`. The `stein` branch contains the
same final assignment code.

Source files to study with this README:

- `os262-assignment1.pdf`
  - The official assignment instructions.
  - Defines Tasks 1, 2, and 3.
  - Gives the required `co_test` style for Task 3.

- `moodle_qa.md`
  - Course staff answers and clarifications.
  - Important for understanding what the staff considers acceptable.

- This README
  - Our implementation notes.
  - The reasoning behind the code changes.
  - Questions we should be ready to answer orally.

## Quick Status

Implemented tasks:

- Task 0: xv6 builds and boots in the devcontainer.
- Task 1: `helloworld` user program.
- Task 2: `memsize()` system call and `memsize_test`.
- Task 3: `co_yield(pid, value)` system call and `co_test`.
- The three required Task 3 error checks run at the start of `co_test`
  (before the PDF ping-pong loop), so there is no separate `co_error_test`
  program in the final version.

Important commands:

```sh
make qemu
```

Inside xv6:

```sh
helloworld
memsize_test
co_test
```

Exit QEMU:

```text
Ctrl-a
x
```

Before final submission:

```sh
make clean
```

The assignment requires submitting a full xv6 source tree, not just changed
files.

## Repository And Workflow

We started from the course repository:

```text
https://github.com/BGU-CS-OS/xv6-riscv
```

Then we created our own GitHub repository:

```text
https://github.com/GuyStein1/OSAssignment1
```

The remotes are:

```text
origin   -> our repository
upstream -> the course repository
```

Why this matters:

- `origin` is where we push our work.
- `upstream` is only the source repository we started from.
- We should not push assignment work to the course repository.

We kept `main` as the shared base branch and implemented on `stein`:

```sh
git switch main
git pull --ff-only origin main
git switch -c stein
```

Why we used a separate branch:

- It keeps `main` clean while we experiment.
- The teammate can work on another branch.
- Later we can compare branches before choosing the final version.

The main assignment commits are:

```text
Add xv6 hello world user program
Add memsize syscall and test program
Add co_yield coroutine syscall
```

Useful Git commands:

```sh
git status
git log --oneline --decorate --max-count=8
git diff stein..origin/<teammate-branch>
git push -u origin stein
```

## Development Environment

We used VS Code Dev Containers with Rancher Desktop as the Docker engine.
That works because Rancher Desktop provides a Docker-compatible runtime.

The devcontainer files are:

- `.devcontainer/devcontainer.json`
- `.devcontainer/Dockerfile`

The Dockerfile installs the tools xv6 needs:

- `gcc-riscv64-linux-gnu`
- `binutils-riscv64-linux-gnu`
- `qemu-system-misc`
- `gdb-multiarch`
- `build-essential`
- `git`

Why we use the container:

- xv6 needs a RISC-V cross compiler.
- xv6 runs under QEMU, not directly on macOS.
- The container gives everyone the same compiler and emulator setup.

In the container, the repo path is usually:

```text
/workspaces/xv6-riscv
```

On the host machine, the same repo is:

```text
/Users/guyst/CS/LastSemester!/OperatingSystems/xv6-riscv
```

Important terminal detail:

- `make qemu` runs on the Linux shell in the container.
- After xv6 boots, the prompt changes to xv6's own shell: `$`.
- Commands like `helloworld`, `memsize_test`, and `co_test` run inside xv6.
- Host/container commands like `make clean` do not work inside xv6.

That is why this happened:

```text
$ make clean
exec make failed
```

It was not a build problem. It only means `make clean` was typed inside xv6
instead of the container shell.

Exit xv6/QEMU first:

```text
Ctrl-a
x
```

Then run host/container commands like:

```sh
make clean
```

## Files We Changed

Main files:

- `Makefile`
  - Adds our user programs to `UPROGS`.
  - Sets `CPUS := 1`, as required for Task 3.

- `user/helloworld.c`
  - Task 1 user program.

- `kernel/syscall.h`
  - Adds syscall numbers for `memsize` and `co_yield`.

- `kernel/syscall.c`
  - Registers kernel syscall handlers in the syscall table.

- `kernel/sysproc.c`
  - Implements syscall wrappers:
    - `sys_memsize`
    - `sys_co_yield`

- `kernel/defs.h`
  - Exposes the kernel helper `co_yield`.

- `kernel/proc.c`
  - Implements the real coroutine handoff logic.
  - Adjusts the scheduler to handle direct process switching.

- `user/usys.pl`
  - Adds generated user-space syscall stubs.

- `user/user.h`
  - Adds user-space declarations.

- `user/memsize_test.c`
  - Task 2 test program.

- `user/co_test.c`
  - Required Task 3 test program from the PDF.
  - Runs the three required error cases first, then enters the PDF
    parent/child handoff loop.

## How xv6 System Calls Work

The important path for a system call is:

```text
user C code
  -> generated assembly stub from user/usys.pl
  -> ecall
  -> usertrap() in kernel/trap.c
  -> syscall() in kernel/syscall.c
  -> sys_* handler
  -> return value placed in trapframe->a0
  -> back to user space
```

The user process passes the syscall number in register `a7`. Arguments are
passed in registers `a0`, `a1`, etc. xv6 stores those registers in the current
process's `trapframe`.

That is why adding a syscall always needs several edits:

1. A syscall number in `kernel/syscall.h`.
2. A kernel handler declaration and table entry in `kernel/syscall.c`.
3. A `sys_*` function in a kernel file, here `kernel/sysproc.c`.
4. A user-space stub in `user/usys.pl`.
5. A user-space declaration in `user/user.h`.

`user/usys.S` is generated by `user/usys.pl`. We do not edit it manually.

## Task 1: `helloworld`

Task goal: create a user-space xv6 program that prints:

```text
Hello World xv6
```

What we did:

- Added `user/helloworld.c`.
- Added `$U/_helloworld` to `UPROGS` in `Makefile`.

Why `_helloworld` in the Makefile:

- User programs in xv6 are compiled into files whose names start with `_`.
- The xv6 filesystem image includes the `_program` file.
- The shell runs it as `program`, without the underscore.

So:

```text
user/helloworld.c -> user/_helloworld -> run as helloworld in xv6
```

Expected output:

```text
$ helloworld
Hello World xv6
```

What to know for grading:

- This is a user-space program, not host macOS/Linux code.
- It runs inside xv6 under QEMU.
- It uses xv6's own `printf` and `exit`, declared in `user/user.h`.

## Task 2: `memsize()`

Task goal: add:

```c
int memsize(void);
```

It returns the current process memory size in bytes.

What we changed:

- `kernel/syscall.h`
  - Added `SYS_memsize 22`.

- `kernel/syscall.c`
  - Added `extern uint64 sys_memsize(void);`.
  - Added `[SYS_memsize] sys_memsize` to the syscall table.

- `kernel/sysproc.c`
  - Added:

```c
uint64
sys_memsize(void)
{
  return myproc()->sz;
}
```

- `user/usys.pl`
  - Added `entry("memsize");`.

- `user/user.h`
  - Added `int memsize(void);`.

- `user/memsize_test.c`
  - Prints memory before allocation.
  - Allocates 20000 bytes.
  - Prints memory after allocation.
  - Frees the allocation.
  - Prints memory after free.

Why `myproc()->sz`:

- `myproc()` returns the current process's `struct proc`.
- `struct proc` has a field named `sz`.
- `sz` is the size of the process memory in bytes.
- The assignment asks for the running process's memory size, so this is the
  direct source of truth.

Expected output from our run:

```text
memory before allocation: 16384 bytes
memory after allocation: 81920 bytes
memory after free: 81920 bytes
```

Why it does not increase by exactly 20000:

- xv6 `malloc` is a user-space allocator.
- It asks the kernel for memory using `sbrk`.
- xv6's allocator requests memory in chunks, not necessarily exactly the user
  requested size.
- In this case, allocating 20000 bytes caused the process size to grow by a
  larger chunk.

Why `free()` does not reduce `memsize()`:

- `free()` returns memory to the user-space allocator's free list.
- It does not necessarily call `sbrk` with a negative value.
- Therefore the process still owns the same amount of memory from the kernel's
  perspective.

What to know for grading:

- `memsize()` returns kernel-known process size, not "currently used malloc
  bytes".
- `malloc` and `free` are user-space library functions in xv6.
- The kernel only sees the process size managed through `sbrk` / `growproc`.

## Task 3: `co_yield(pid, value)`

Task goal:

```c
int co_yield(int pid, int value);
```

The system call lets two processes cooperatively exchange values and hand off
execution to each other.

The required behavior from the PDF:

- The caller gives a target PID and a positive value.
- If the target has not called `co_yield` yet, the caller sleeps.
- Once both processes have called `co_yield`, the values are exchanged.
- The CPU switches directly from one process to the other.
- A process sleeping inside `co_yield` should not be chosen by the normal
  scheduler.
- On success, return the value sent by the other process.
- On error, return `-1`.

### Important Constraints

The assignment explicitly forbids:

- Adding fields to `struct proc`.
- Adding process states.
- Adding global or process-level kernel data structures.
- Relying on multiple CPUs.

The assignment requires:

- Set QEMU to one CPU.
- Use direct `swtch` for the final handoff.
- Keep the existing xv6 process model working.

We set:

```make
CPUS := 1
```

in `Makefile`.

### The Required Test

`user/co_test.c` follows the PDF test:

```c
int pid1 = getpid();
int pid2 = fork();

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
```

Expected output repeats:

```text
parent received: 1
Child received: 2
parent received: 1
Child received: 2
...
```

The program is intentionally infinite.

## The `co_yield` Design

Since we cannot add fields to `struct proc`, we reuse existing per-process
fields:

- `p->chan`
  - Marks that a process is sleeping inside `co_yield`.
  - Stores what peer it is waiting for.

- `p->trapframe->a1`
  - Stores the value this process wants to send.
  - This works because `a1` originally contains the second syscall argument.

- `p->trapframe->a0`
  - Stores the return value that should be returned to user space.
  - `-1` is used as a temporary "not answered yet" value.

The private wait channel is:

```c
(void*)&p->context
```

This address is unique per process and not used by normal xv6 sleep/wakeup
code, so it is a good marker for "waiting for this process in co_yield".

## `co_yield` Flow

There are two main cases.

### Case 1: First Process Arrives

Example:

```text
parent calls co_yield(child_pid, 2)
child has not called co_yield yet
```

The parent:

1. Finds the child process.
2. Stores its outgoing value in `trapframe->a1`.
3. Stores `-1` in `trapframe->a0` as a pending return value.
4. Sets `chan` to the child's private channel.
5. Sets its own state to `SLEEPING`.
6. Sets the child to `RUNNING`.
7. Updates `mycpu()->proc` to the child.
8. Calls:

```c
swtch(&parent->context, &child->context);
```

At this point the parent is not `RUNNABLE`, so the normal scheduler will not
choose it. The child is not chosen by the normal scheduler either; `co_yield`
switches to it directly.

This matters because the staff clarified that the final solution should not use
`sched()` inside `co_yield`. The first caller still sleeps, but the CPU is
given directly to the target process so it can reach its own `co_yield`.

### Case 2: Second Process Arrives

Example:

```text
child calls co_yield(parent_pid, 1)
parent is already sleeping in co_yield waiting for the child
```

The child:

1. Finds the parent process.
2. Checks that the parent is sleeping on the child's private channel.
3. Stores its own outgoing value in `trapframe->a1`.
4. Copies the parent's outgoing value into the child's return register.
5. Copies the child's outgoing value into the parent's return register.
6. Sets the child to `SLEEPING`.
7. Sets the parent to `RUNNING`.
8. Updates `mycpu()->proc` to the parent.
9. Calls:

```c
swtch(&child->context, &parent->context);
```

This is the direct handoff. The scheduler does not choose the target process.
The code directly switches from the child context to the parent context.

Later, when the parent calls `co_yield` again, the same mechanism wakes the
child.

## Why We Changed The Scheduler

Original xv6 scheduler logic assumes:

```text
scheduler picks process P
P eventually switches back to scheduler
scheduler releases P's lock
```

Direct process switching can break that assumption.

Example:

```text
scheduler starts process B
B direct-switches to A inside co_yield
A later returns to the scheduler
```

Now the scheduler frame that originally ran B is resumed by A. If the scheduler
blindly releases B's lock, it can release the wrong lock and panic with:

```text
panic: release
```

So we changed the scheduler to release the process currently recorded on the
CPU:

```c
ran = c->proc;
c->proc = 0;
release(&ran->lock);
```

This keeps normal xv6 behavior working, but also handles the direct-switch
case correctly.

This is not using the scheduler to choose the coroutine target. The coroutine
target is still chosen inside `co_yield`, and the handoff still uses direct
`swtch`.

## Staff Clarification About Scheduler Changes

Outside the Moodle Q&A file, the staff later clarified that:

- A working solution that crashes after a few iterations with `panic: release`
  will be accepted.
- A solution that changes the scheduler to return to the relevant process will
  also be accepted.
- A fully correct solution without those issues will also be accepted.

Why they likely gave this clarification:

- Direct process switching is hard to fit into xv6's scheduler invariants.
- Many students probably got the value exchange and direct switch working, but
  hit `panic: release` after a timer interrupt or later scheduler return.
- The original assignment did not fully explain how to preserve the scheduler's
  lock ownership assumptions after direct switching.

How this affects us:

- It is relevant because we intentionally changed the scheduler.
- Our change is exactly in the category the staff said is accepted.
- Our implementation did not crash in testing.
- We should explain that the scheduler change is for lock correctness, not for
  choosing the target process.

Good grading explanation:

```text
The handoff itself happens inside co_yield with swtch. We changed the scheduler
only because after a direct process switch, the process returning to the
scheduler may not be the same process originally selected by the scheduler.
So the scheduler releases c->proc, the process actually returning, instead of
the old loop variable.
```

This clarification is not one of the numbered entries in `moodle_qa.md`; it was
published separately. If asked about it, do not say it came from the Moodle Q&A.

## Error Handling

`co_yield` returns `-1` for:

- PID is zero or negative.
- PID is the caller's own PID.
- PID does not exist.
- Target process is killed.
- Target is `UNUSED` or `ZOMBIE`.
- Target is sleeping, but not as the expected reciprocal `co_yield` peer.
- Target is in a state that is not useful for this limited two-process
  coroutine test.

`user/co_test.c` also runs the three required error cases at the very start,
before entering the PDF handoff loop. Inside xv6 a plain `co_test` prints:

```text
invalid pid: -1
self yield: -1
killed target: -1
parent received: 1
Child received: 2
...
```

The error lines come from `run_error_tests()` called unconditionally at the
top of `main()`. The PDF infinite parent/child loop runs immediately after.

## Locking Notes

Important xv6 rules:

- A process's `state` and `chan` are protected by `p->lock`.
- `swtch()` saves the current kernel context and restores another kernel
  context.
- `acquire()` disables interrupts through `push_off()`.

In our direct handoff:

- The target process lock is held across `swtch`.
- The current process lock is released before switching away.
- This matches xv6's convention that a process resumes with its own lock held
  and then releases it in the normal resume path.

The lock handoff is the subtle part:

- First caller `P` targets a `RUNNABLE` process `C`.
- `find_proc` returns with `C->lock` held.
- `P` marks itself `SLEEPING`, releases `P->lock`, and switches directly to
  `C`.
- `C` resumes holding `C->lock`, which is the normal xv6 convention after a
  scheduler-style context switch.
- When `C` later calls `co_yield` back to `P`, it finds `P` sleeping, sets the
  return values, releases `C->lock`, and switches directly to `P`.
- `P` resumes holding `P->lock`, clears `chan`, releases `P->lock`, and returns
  from the syscall.

Why this matters:

- Timer interrupts should not interrupt the middle of a lock-critical direct
  switch.
- The target resumes in a state where it can safely finish its syscall and
  release its lock.

## What We Tested

We ran a clean rebuild:

```sh
make clean
make qemu
```

Then inside xv6:

```text
helloworld
memsize_test
usertests -q
co_test
```

Observed results:

```text
Hello World xv6
```

```text
memory before allocation: 16384 bytes
memory after allocation: 81920 bytes
memory after free: 81920 bytes
```

```text
invalid pid: -1
self yield: -1
killed target: -1
```

`usertests -q` finished with:

```text
ALL TESTS PASSED
```

`co_test` ran for several seconds and repeatedly printed:

```text
parent received: 1
Child received: 2
```

No `panic: release`, no deadlock, and no unknown syscall occurred.

Note: `usertests` intentionally prints some `usertrap(): unexpected scause`
messages during memory-protection tests. Those are expected as long as the
test prints `OK` and eventually `ALL TESTS PASSED`.

## Optional Local Tests

We keep an extra stress test source file for our own use:

```text
local_tests/co_stress_test.c
```

It checks 2000 coroutine handoffs in two cases:

- Parent reaches `co_yield` first.
- Child reaches `co_yield` first.

Status: the Makefile plumbing that built this test (`EXTRA_TESTS=1` flag,
`EXTRA_TEST_BINS`, the `_co_stress_test` rule) was stripped from the
final Makefile to keep the submission diff minimal. The source still
lives in `local_tests/` for reference. To run it again locally, restore
the build rule (a few lines: an `_co_stress_test` target that links from
`local_tests/co_stress_test.o`, and an entry in `UPROGS`).

For final Moodle submission, exclude `local_tests/`.

## Questions We Should Be Ready To Answer

### How does a system call differ from a normal function call?

A normal function call stays in user space and jumps to another user-space
address.

A system call uses `ecall`, traps into the kernel, runs a kernel handler, and
returns through the trap mechanism. The syscall number is in `a7`, arguments
are in registers like `a0` and `a1`, and the return value comes back in `a0`.

### What is `usys.pl`?

It generates assembly stubs for user-space syscall functions. For example,
`entry("memsize")` generates a `memsize` function that puts `SYS_memsize` in
`a7`, runs `ecall`, and returns.

### Why does `memsize()` return `myproc()->sz`?

Because `sz` is the current process memory size in bytes, stored in the
process control block.

### Why does memory not shrink after `free()`?

Because `free()` only returns memory to the user-space allocator. It does not
necessarily return pages to the kernel or reduce `p->sz`.

### How does `co_yield` pass values?

Each process stores its outgoing value in `trapframe->a1`. When the second
process arrives, the kernel copies each saved value into the other process's
return register, `trapframe->a0`.

### Where is coroutine waiting state stored?

In existing process fields:

- `state = SLEEPING`
- `chan = co_chan(peer)`

No new process field or global table is used.

### How does `co_yield` bypass the scheduler?

The kernel sets the target process to `RUNNING`, updates `mycpu()->proc`, and
calls `swtch` directly from the current process context to the target process
context. This happens both when the peer is already sleeping inside `co_yield`
and when the first caller directly runs a `RUNNABLE` peer.

### Why is the scheduler still changed?

Because direct switching can make a different process return to an old
scheduler frame. Releasing `c->proc->lock` releases the lock of the process
that actually returned.

### Is this allowed?

Yes. The staff clarification explicitly accepts solutions that modify the
scheduler to return to the relevant process.

### What are the limits of our implementation?

It is designed for the assignment's two-process reciprocal handoff test. It
does not try to implement a full general-purpose coroutine framework for many
processes and arbitrary waiting graphs.

This is aligned with the staff Q&A, which says the provided test is what they
care about.

## Final Submission Checklist

Short answer:

Submit the full xv6 source tree with our assignment changes, but do not include
our study/admin files.

So yes, conceptually it is:

```text
the whole xv6 codebase
minus files that are only for us, like the study README, Moodle Q&A, PDF, and agent notes
minus build artifacts
```

Do not submit only the files we changed. The grader should receive an xv6 tree
that can be built normally with `make qemu`.

### Files That Must Be In The Submission

The package should include the normal xv6 source tree, including:

```text
Makefile
LICENSE
README
kernel/
mkfs/
user/
```

It must include our assignment source changes:

```text
Makefile
kernel/defs.h
kernel/proc.c
kernel/syscall.c
kernel/syscall.h
kernel/sysproc.c
user/user.h
user/usys.pl
user/helloworld.c
user/memsize_test.c
user/co_test.c
```

`user/co_test.c` is important because the assignment explicitly asks for the
test program. The error-condition checks are in the same program: they run
unconditionally at the start of `main()`, before the PDF handoff loop.

### Files To Exclude From The Submission

These files are for studying, setup, or local workflow. They should not be in
the final Moodle package:

```text
.git/
.DS_Store
.claude/
ASSIGNMENT_README.md
moodle_qa.md
os262-assignment1.pdf
agent.md
local_tests/
```

These are also not needed for grading:

```text
.devcontainer/
.vscode/
```

Build artifacts should also be excluded. Running `make clean` removes the
important ones:

```text
fs.img
kernel/kernel
kernel/*.o
kernel/*.d
kernel/*.asm
kernel/*.sym
user/*.o
user/*.d
user/_*
user/*.asm
user/*.sym
mkfs/mkfs
```

### Why There Is No Separate `co_error_test.c`

The PDF asks for the Task 3 test code in `user/co_test.c`, and it also asks us
to test error conditions. To keep the final submission simple, both live in
`user/co_test.c`. Running `co_test` inside xv6:

1. First runs `run_error_tests()`, which checks:

```text
non-existent PID
self-yield
killed PID
```

2. Then enters the PDF's infinite parent/child handoff loop.

A grader who runs `co_test` without arguments sees both. There is no separate
mode or argv flag.

### Exact Final Packaging Steps

Before submitting:

1. Decide which branch implementation to use after comparing with the teammate.
2. Merge the final version into the submission branch or create a final
   packaging copy.
3. Run:

```sh
make clean
make qemu
```

4. In xv6, run:

```text
helloworld
memsize_test
co_test
```

5. Exit QEMU.
6. Run:

```sh
make clean
```

7. Create the ZIP/TAR from the cleaned source tree.
8. Make sure the ZIP/TAR does not include:

```text
.git/
ASSIGNMENT_README.md
moodle_qa.md
os262-assignment1.pdf
agent.md
.devcontainer/
.vscode/
local_tests/
build artifacts
```

9. Submit that package to Moodle.

### Practical Packaging Command Sequence

One clean way is to make a temporary copy outside the repo and zip that copy.

From the parent directory of `xv6-riscv`, create a clean copy whose top-level
folder is still named `xv6-riscv`:

```sh
mkdir -p /tmp/os262-submit
rsync -a xv6-riscv/ /tmp/os262-submit/xv6-riscv/ \
  --exclude='.git' \
  --exclude='.DS_Store' \
  --exclude='.claude' \
  --exclude='ASSIGNMENT_README.md' \
  --exclude='moodle_qa.md' \
  --exclude='os262-assignment1.pdf' \
  --exclude='agent.md' \
  --exclude='local_tests' \
  --exclude='.devcontainer' \
  --exclude='.vscode'
```

Then, inside the copied folder:

```sh
cd /tmp/os262-submit/xv6-riscv
make clean
```

Then verify the copied folder still builds:

```sh
make qemu
```

Inside xv6, run:

```text
helloworld
memsize_test
co_test
```

Exit QEMU, clean again, and zip:

```sh
make clean
cd /tmp/os262-submit
zip -r xv6-riscv.zip xv6-riscv
```

The ZIP should contain `xv6-riscv/` with the source code inside it.

### Final Sanity Check

Before uploading, unzip the package somewhere temporary and check:

```sh
make qemu
```

The important point is that the submitted folder should build on its own. If it
only builds because of files outside the submitted ZIP, the package is wrong.
