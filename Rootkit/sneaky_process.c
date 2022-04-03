#include <stdio.h>
#include <stdlib.h>

void copy_pwd(const char *srcName, const char *tarName)
{
    FILE *srcFile = fopen(srcName, "r");
    FILE *tarFile = fopen(tarName, "w");
    if (srcFile == NULL)
    {
        printf("Cannot open file %s \n", srcName);
    }
    if (tarFile == NULL)
    {
        printf("Cannot open file %s \n", srcName);
    }

    char buf[BUFSIZ];
    size_t size;
    while (size = fread(buf, 1, BUFSIZ, srcFile))
    {
        fwrite(buf, 1, size, tarFile);
    }

    fclose(tarFile);
    fclose(tarFile);
}

int main()
{
    printf("sneaky_process pid=%d\n", getpid());
    copy_pwd("/etc/passwd", "/tmp/passwd");
    return EXIT_SUCCESS;
}