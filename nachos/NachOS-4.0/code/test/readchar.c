#include "syscall.h"
char s[256];

int main() {

    char ch;
    PrintString("\nNhap ki tu can in ra:\t");
    ch = ReadChar();
    PrintChar(ch);
    PrintString("\nNhap chuoi can in ra:\t");
    ReadString(s);
    // PrintString("\nIn ra 1 so ngau nhien\t");
    PrintString(s);
    // RandomNum();

    Halt();
}