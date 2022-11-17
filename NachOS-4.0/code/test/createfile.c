#include "syscall.h"
#define maxlen 32

int main() {
    int len;
    char filename[maxlen + 1];
    /*Create a file*/

    PrintString("Enter file's name: ");
    ReadString(filename, maxlen);

    if (Create(filename) == 0) {
        PrintString("File ");
        PrintString(filename);
        PrintString(" created successfully!\n");
    }
    else
        PrintString("Create file failed\n");

    Halt();
}