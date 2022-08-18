#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int main(int argc, char *argv[])
{
    int flag1=0,flag2=0,flag3=0;
    if (argc < 2)
    {
    printf(2,"Illegal command or arguments\n");
    exit(-1);
    }

    int procid=atoi(argv[1]);
    flag1=numOpenFiles(procid);
    flag2=memAlloc(procid);
    flag3=getprocesstimedetails(procid);
    if (flag1==-1 || flag2==-1 || flag3==-1)
    exit(-1);
    else    
    exit(0);
}