#include "syscall.h"

char s[256];
int main() {
    int id;
    int len;
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
    Write("Hello from inside the file\n", 27, id);
    Write("Hello from inside the file", 26, id);
    Close(id);
    id = Open("in.txt", 1);
    len = Seek(-1, id);
    Seek(0, id);
    Read(s, len, id);
    Close(id);
    PrintString(s);

    // Remove("in.txt");
    // PrintString("Xoa file thanh cong !!");
    Halt();
}