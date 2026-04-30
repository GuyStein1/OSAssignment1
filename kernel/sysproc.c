#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

extern struct proc proc[NPROC];

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// return the size of the running process's memory in bytes.
uint64
sys_memsize(void)
{
  return myproc()->sz;
}

uint64
sys_co_yield(void)
{
  int pid, value;
  argint(0, &pid);
  argint(1, &value);

  struct proc *me = myproc();
  struct proc *target = 0;

  if (pid <= 0 || pid == me->pid)
    return -1;

  for (struct proc *p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    if (p->pid == pid) {
      if (p->killed || p->state == UNUSED || p->state == ZOMBIE) {
        release(&p->lock);
        return -1;
      }
      target = p;
      release(&p->lock);
      break;
    }
    release(&p->lock);
  }

  if (target == 0)
    return -1;

  acquire(&tickslock);

  acquire(&target->lock);
  if (target->state == UNUSED || target->state == ZOMBIE || target->killed) {
    release(&target->lock);
    release(&tickslock);
    return -1;
  }
  int target_waiting = (target->state == SLEEPING && target->chan == (void*)me);
  release(&target->lock);

  if (target_waiting) {
    target->trapframe->a0 = value;
    wakeup(me);
  }
  
  sleep(target, &tickslock);
  release(&tickslock);

  if (killed(me))
    return -1;

  return me->trapframe->a0;
}
