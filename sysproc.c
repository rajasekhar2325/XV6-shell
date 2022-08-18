#include "types.h"
#include "x86.h"
#include "defs.h"
// #include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  int status;
  if (argint(0,&status)<0)
    return -1;
  exit(status);
  return 0;  // not reached
}

int
sys_wait(void)
{
  int *p;
  if (argptr(0,(void*)&p,sizeof(int))<0)
    return -1;
  return wait(p);
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;
  if(argint(0, &n) < 0)
  {
      return -1;
  }
    
  addr = myproc()->sz;


  
  if(growproc(n) < 0)
    return -1;

  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}


// prints hello world to console

int sys_helloWorld()
{
  acquire(&tickslock);
  cprintf("Hello World\n");
  release(&tickslock);
  return 0;
}

// returns number of open file descriptors in current process
int sys_numOpenFiles()
{
  int procid;
  if(argint(0,&procid)<0)
    return -1;
  
  return numOpenFiles(procid);
}

// returns memory allocated for the process

int sys_memAlloc()
{ 
  int procid;
  if(argint(0,&procid)<0)
    return -1;
  // cprintf("syproc: %d bytes",myproc()->ma);
  
 return memAlloc(procid);
}

//getprocesstimedetails()
int sys_getprocesstimedetails()
{
  int procid;
  if(argint(0,&procid)<0)
    return -1;
  // cprintf("sysproc start");
  
  // cprintf("sysproc end");
  return getprocesstimedetails(procid);
  
}

int sys_psinfo()
{
  psinfo();
  return 1;
}
