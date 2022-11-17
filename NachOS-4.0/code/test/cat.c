#include "syscall.h"
#define maxlen 32

char s[256];

int main() {
    int id;
    int len;
    char* filename;

    PrintString("Nhap ten file ban muon doc: ");
    ReadString(filename, maxlen);

    id = Open(filename, 1);
    if (id != -1) {
        len = Seek(-1, id);
        Seek(0, id);
        Read(s, len, id);
        Close(id);

        PrintString("\nNoi dung file: \n");
        PrintString(s);
    }
    else    
        PrintString("< ERROR > Khong the mo file de doc !!");
    Halt();
}