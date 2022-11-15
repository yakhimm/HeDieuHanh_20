#include "syscall.h"

char s[256];
int main() {

    // int res;
    // res = Create("in.txt");
    // PrintNum(res);
    // Close(res);
    // res = Open("in.txt", 0);
    // PrintNum(res);
    // Write("hello", 5, res);
    // Close(res);
    // res = Open("in.txt", 0);
    // Read(s, 5, res);
    // Close(res);
    // PrintString(s);
    // Halt();

    int id;
    id = Create("in.txt");
    Close(id);
    if (id == 0) {
        PrintString("\nTao file thanh cong\n");
    }
    else {
        PrintString("\nTao file that bai\n");
        return 0;
    }
    id = Open("in.txt", 0);
    Write("Hello from inside the file", 26, id);
    Close(id);
    id = Open("in.txt", 1);
    Read(s, 26, id);
    Close(id);
    PrintString(s);
    Halt();
}