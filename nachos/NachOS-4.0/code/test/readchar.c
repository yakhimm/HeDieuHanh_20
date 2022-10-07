#include "syscall.h"

int main()
{
    Create("readchar.txt")
    int ch = ReadChar();
    printf(ch);
    Halt();
}