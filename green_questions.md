# Green Questions — Tasks 2 and 3

Answers to the theoretical questions from the assignment PDF.
Not for submission, but expected to be understood for the grading session.

---

# Task 2 Questions (page 8)

## Q1: How does calling a system call differ from calling a regular function? How does it work?

A regular function call stays entirely in user space. The CPU executes a jump instruction (`jal`/`jalr` on RISC-V) to another address in the same address space, with no privilege change and no involvement from the kernel.

A system call crosses the user/kernel boundary. The flow is:

1. User code calls the stub function (e.g. `memsize()`), generated from `usys.pl`.
2. The stub loads the syscall number into register `a7` and executes the `ecall` instruction.
3. `ecall` triggers a trap: the CPU switches to supervisor mode and jumps to the trap handler.
4. `usertrap()` in `kernel/trap.c` identifies this as a syscall and calls `syscall()` in `kernel/syscall.c`.
5. `syscall()` reads `a7` from the trapframe and dispatches to the matching `sys_*` handler via the syscall table.
6. The handler runs in kernel mode with full kernel privileges.
7. The return value is written into `trapframe->a0`.
8. The kernel returns to user space via `sret`, restoring user mode. Register `a0` now holds the return value.

The key difference: a function call is a jump within the same privilege level and address space. A system call is a controlled, hardware-enforced entry point into the kernel.

---

## Q2: How are parameters passed to the system call function? How is the return value returned to userspace?

**Parameters:** passed in registers `a0`, `a1`, `a2`, ... (the standard RISC-V calling convention). When the trap fires, the kernel saves all user registers into the process's `trapframe`. The kernel then reads arguments from the trapframe using helper functions like `argint()` and `argaddr()` defined in `kernel/syscall.c`.

For `memsize` there are no arguments. For a syscall like `write(fd, buf, n)`, the kernel would call:
```c
argint(0, &fd);    // reads trapframe->a0
argaddr(1, &buf);  // reads trapframe->a1
argint(2, &n);     // reads trapframe->a2
```

**Return value:** the `sys_*` handler returns a `uint64`. `syscall()` stores it into `trapframe->a0`. When the kernel returns to user space, `a0` holds that value, which the C calling convention treats as the function return value.

---

## Q3: What is the purpose of the `usys.pl` file?

`usys.pl` is a Perl script that auto-generates `user/usys.S` at build time. For each `entry("name")` call in the script, it produces a small assembly stub like:

```asm
memsize:
  li a7, SYS_memsize
  ecall
  ret
```

This means we never hand-write assembly for syscall stubs. Adding a new syscall only requires one line in `usys.pl`. The generated `usys.S` is compiled and linked into every user program, so calling `memsize()` from C transparently triggers the correct `ecall`.

---

## Q4: What is `struct proc` and where is it defined? Why do we need it? Does a real-world OS have a similar structure?

`struct proc` is defined in `kernel/proc.h` at line 85. It is xv6's **Process Control Block (PCB)** — the kernel's per-process data structure. It holds everything the kernel needs to manage a process:

```c
struct proc {
  struct spinlock lock;
  enum procstate state;        // UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE
  void *chan;                  // sleep channel
  int killed;
  int pid;
  struct proc *parent;
  uint64 sz;                   // size of process memory in bytes  ← used by memsize()
  pagetable_t pagetable;       // user page table
  struct trapframe *trapframe; // saved user registers
  struct context context;      // saved kernel registers (for swtch)
  struct file *ofile[NOFILE];  // open file descriptors
  struct inode *cwd;           // current working directory
  char name[16];
};
```

Without the PCB, the kernel has no way to save a process's state when it is not running, to schedule it, to restore it, or to track its resources. It is the fundamental unit the scheduler operates on.

Every real OS has an equivalent structure. Linux calls it `task_struct` (defined in `include/linux/sched.h`) and it is much larger (~700 fields), but serves the same purpose. Windows has `EPROCESS`. The concept is universal.

---

## Q5: How much memory does our program use before and after the allocation?

```
memory before allocation: 16384 bytes
memory after allocation:  81920 bytes
```

The process starts at 16384 bytes (the initial code + data + stack loaded by the OS). After `malloc(20000)`, the process size jumps to 81920 bytes — an increase of 65536 bytes, much more than the 20000 requested.

---

## Q6: What is the difference between the memory size before and after the release?

```
memory after allocation: 81920 bytes
memory after free:       81920 bytes
```

There is no difference. `free()` does not reduce `memsize()`.

---

## Q7: Why does `free()` not reduce the memory size? (Advanced: look at `malloc` and `free`)

`free()` in xv6 is implemented in `user/umalloc.c` as a classic K&R free-list allocator. It works entirely in user space: it links the freed block back into a linked list (`freep`) so that future `malloc` calls can reuse it. It never calls `sbrk` with a negative value to return memory to the kernel.

`memsize()` returns `p->sz`, the kernel-tracked process size. The kernel only changes `sz` when `sbrk` is called. Since `free()` does not call `sbrk`, `p->sz` stays at 81920.

**Why did `malloc(20000)` cause a 65536-byte jump?**

When `malloc` first needs memory from the kernel, it calls `morecore()`. `morecore` always requests at least 4096 allocation units (`if(nu < 4096) nu = 4096`), to avoid calling `sbrk` on every small allocation. Each unit is `sizeof(Header)` = 16 bytes (a union aligned to `long` on 64-bit). So:

```
4096 units × 16 bytes = 65536 bytes
16384 (before) + 65536 = 81920 bytes
```

This matches the observed output exactly. The extra memory beyond the 20000 requested sits in the allocator's free list, ready for future `malloc` calls without needing another `sbrk`.

---

# Task 3 Questions (page 15)

## Q1: What is the difference between cooperative and preemptive multitasking? How does `co_yield` implement cooperative multitasking?

**Preemptive multitasking:** the OS can forcibly interrupt a running process at any time (typically via a timer interrupt) and switch to another. The process has no say in when it loses the CPU. This is how xv6's normal scheduler works — a timer fires, triggers `usertrap`, and the scheduler picks the next RUNNABLE process.

**Cooperative multitasking:** a process runs until it explicitly volunteers to give up the CPU. No process can be interrupted against its will. The system only switches when a process calls a yield-like operation.

`co_yield` implements cooperative multitasking because:
- A process only gives up the CPU when it explicitly calls `co_yield`.
- The target is chosen by the calling process (by PID), not by the scheduler.
- The kernel performs the switch immediately and directly, without involving the timer or the scheduler loop.

---

## Q2: How does `co_yield` bypass the normal scheduler? What are the advantages and disadvantages?

**How it bypasses the scheduler:**

Instead of calling `sched()` (which returns to the scheduler loop, which then picks the next RUNNABLE process), `co_yield` directly calls:

```c
mycpu()->proc = target;
swtch(&p->context, &target->context);
```

This overwrites the CPU's current process pointer and jumps straight into the target's kernel context. The scheduler loop is never entered.

**Advantages:**
- Zero scheduling overhead — no loop through the process table, no lock contention on unrelated processes.
- Deterministic: you know exactly which process runs next.
- Lower latency for tightly coupled producer/consumer pairs.

**Disadvantages:**
- Only works correctly with 1 CPU (multi-CPU requires much more complex synchronization).
- Breaks scheduler assumptions about lock ownership (hence the scheduler fix).
- Not general-purpose: only handles the two-process reciprocal case cleanly.
- A misbehaving process that never calls `co_yield` back will starve its partner indefinitely.

---

## Q3: How is the integer value passed between processes? Where is it stored and how is it retrieved?

The value travels through the trapframe registers, reusing fields that already exist per-process:

1. When a process calls `co_yield(pid, value)`, the kernel stores `value` in `p->trapframe->a1`. This works because `a1` already held the second syscall argument when the trap fired — no new storage needed.

2. When the second process arrives and both sides are ready, the kernel cross-copies:
```c
p->trapframe->a0 = target->trapframe->a1;   // we receive target's value
target->trapframe->a0 = value;              // target receives our value
```

3. After resuming from `swtch`, each process reads its return value from `trapframe->a0`:
```c
ret = p->trapframe->a0;
return ret;
```

`a0` is the register the kernel uses for syscall return values, so writing into `trapframe->a0` is exactly how you set what gets returned to user space.

---

## Q4: In what real-world scenarios is a coroutine-style yield mechanism useful?

**Scenarios:**
- **Producer/consumer pipelines:** a parser feeds tokens to a code generator. Each side yields to the other when it has produced or consumed a unit of work.
- **Game engines:** game logic and rendering alternate control cooperatively each frame without thread overhead.
- **Async I/O frameworks:** Node.js, Python's asyncio, and Go's goroutines all use cooperative yielding so that I/O-bound tasks hand the CPU to the event loop when waiting.
- **Coroutine libraries:** Python generators (`yield`), Lua coroutines, and C++20 coroutines all implement the same rendezvous pattern at the language level.

Go is the most direct analogue: goroutines are scheduled cooperatively (with some preemption added later), and `runtime.Gosched()` is essentially a `co_yield` to the Go runtime scheduler.

---

## Q5: How does direct process switching in `co_yield` differ from a normal scheduler context switch?

| | Normal scheduler switch | `co_yield` direct switch |
|---|---|---|
| Who decides the target? | Scheduler loop (round-robin over RUNNABLE) | The calling process (by PID) |
| Path taken | `sched()` → scheduler loop → `swtch` to scheduler context → scheduler picks next → `swtch` to process | Direct `swtch` from current process to target process |
| Intermediate context | Goes through the scheduler's per-CPU stack | None — two-way jump |
| Target state before switch | Must be RUNNABLE | Can be SLEEPING (already in co_yield) or RUNNABLE |
| Lock discipline | Caller holds own lock entering `sched()`, scheduler releases it | Caller releases own lock, target's lock is held across `swtch` |

The key difference: a normal switch goes current → scheduler → next (two hops). A `co_yield` switch goes current → target (one hop), which is why it is lower latency and why it creates the scheduler lock ownership problem described in the README.

---

## Q6: What would happen if you tried to implement this using only existing `sleep` and `wakeup`?

Using plain `sleep`/`wakeup` you could put a process to sleep and wake the target, but you would be missing two critical things:

**1. No value passing.** `sleep` and `wakeup` have no mechanism to deliver a payload. You would need a shared memory region or an additional syscall, which is effectively adding a global data structure — forbidden by the assignment.

**2. No direct CPU handoff.** `sleep` calls `sched()`, which returns to the scheduler loop. The scheduler then picks whatever RUNNABLE process it finds next — there is no guarantee the target runs immediately. The target would be woken (set RUNNABLE) but might wait several scheduler ticks before actually running. `co_yield`'s direct `swtch` guarantees the target runs next with zero intermediate scheduling.

So `sleep`/`wakeup` could implement a loose rendezvous but not a true coroutine handoff with value exchange and guaranteed direct CPU transfer.

---

## Q7: How does `co_yield` integrate with xv6's process states?

`co_yield` uses only the four existing states and repurposes `SLEEPING` with a special channel to distinguish coroutine-sleeping from regular sleeping:

- **RUNNING → SLEEPING:** the calling process sets itself SLEEPING with `chan = co_chan(target)` before switching away.
- **SLEEPING → RUNNING:** the target process (already sleeping in `co_yield`) is set directly to RUNNING by the arriving process. `target->chan` is cleared.
- **RUNNABLE → RUNNING:** if the target has not yet called `co_yield`, it is RUNNABLE. The first caller sets it RUNNING directly, skipping the RUNNABLE state entirely as required by the PDF.

The special channel value `&p->context` is what distinguishes a coroutine-sleeping process from a process sleeping on a pipe, lock, or disk wait. The scheduler ignores SLEEPING processes by design, so coroutine-sleeping processes are never accidentally scheduled — they can only be woken by their co_yield peer.

---

## Q8: What are the performance implications of bypassing the scheduler?

**Beneficial when:**
- The two processes are tightly coupled and always ready to exchange immediately. Each handoff is one `swtch` call instead of two (current→scheduler→next).
- Latency matters more than fairness — e.g. real-time or latency-sensitive pipelines.
- There are many idle processes: the scheduler would waste time scanning the process table looking for RUNNABLE entries; `co_yield` jumps straight to the right one.

**Detrimental when:**
- The target is not ready (e.g. waiting on I/O). With a direct handoff, the target immediately returns from `co_yield` with `-1` or another error, whereas the scheduler would have found a productive process to run instead.
- Fairness is required. Bypassing the scheduler means other processes get no CPU time during the coroutine exchange. On a heavily loaded system, two processes ping-ponging via `co_yield` can starve everyone else.
- Multiple CPUs are involved. Direct switching without cross-CPU synchronization leads to races — which is why the assignment restricts `co_yield` to `CPUS := 1`.
