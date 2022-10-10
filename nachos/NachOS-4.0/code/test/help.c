#include "syscall.h"

int main()
{
    PrintString("\nNhom lap trinh SystemCall, gom 3 thanh vien:\n");
    PrintString("\t1. Pham Gia Khiem       - MSSV: 20120307\n");
    PrintString("\t2. Nguyen Thi Thuy Lieu - MSSV: 20120519\n");
    PrintString("\t3. Vo Hoang Thao Nguyen - MSSV: 20120540\n");

    PrintString("\nChuong trinh duoc lap trinh: \n");
    PrintString("\t1. bubblesort - User se nhap vao mang A gom n phan tu va lua chon phuong thuc sort: \n");
    PrintString("\t\t+ Phuong thuc 0: Sort tang dan.\n");
    PrintString("\t\t+ Phuong thuc 1: Sort giam dan.\n");
    PrintString("\t2. ascii      - Chuong trinh in ra ky tu thuoc bo ma ASCII.\n");

    Halt();
}