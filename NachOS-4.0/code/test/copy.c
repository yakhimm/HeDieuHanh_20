#include "syscall.h"
#define maxlen 32
char s[256];
int main() {
    int ids, idd, idd_c;
    int len;
    char* filenameSource;
    char* filenameDestinate;

    PrintString("Nhap ten file nguon: ");
    ReadString(filenameSource, maxlen);

    PrintString("Nhap ten file dich: ");
    ReadString(filenameDestinate, maxlen);

    ids = Open(filenameSource, 1);
    if (ids != -1) {
        len = Seek(-1, ids);
        Seek(0, ids);
        Read(s, len, ids); 

        idd = Open(filenameSource, 0);
        if (idd == -1) {
            if (Create(filenameDestinate) == -1) {
                PrintString("< ERROR > Khong the tao file dich !! Chuong trinh huy bo !!");
                Close(ids);
                Halt();
            }   
            else
                PrintString("Tao file dich thanh cong !!");
        }   
        Write(s, len, idd);
        PrintString("Copy du lieu thanh cong !!");
        Close(idd);  
    }
    else 
        PrintString("< ERROR > File nguon khong ton tai !!");
    Close(ids);

    Halt();
}