#include "syscall.h"

int main()
{
    int f;
    f=Open("test.txt",0);
    PrintString("open = ");
    PrintNum(f); 
    f=Close(f);
    PrintString("\nclose = ");
    PrintNum(f); 
    Halt();
}