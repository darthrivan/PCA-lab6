#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
 
int main(int argc, char *argv[])
{
    char c1, c2;
    int n1, n2;
 
    while (((n1=read(0, &c1, 1)) > 0) && ((n2=read(0, &c2, 1)) > 0))
    {
        write(1, &c2, 1);
        write(1, &c1, 1);
    }
 
    if ((n1==1) && (n2==0)) write(1, &c1, 1);
 
    return 0;
}
