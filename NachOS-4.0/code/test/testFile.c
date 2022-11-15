#include "syscall.h"
char s[256];
int main() {
    int f;
    f = Open("test.txt", 0);
    PrintString("open = ");
    PrintNum(f);
    f = Close(f);
    PrintString("\nclose = ");
    PrintNum(f);
    Read(s, 10, 2);
    PrintString("Chuoi vua nhap la:\t");
    PrintString(s);
    Halt();
}