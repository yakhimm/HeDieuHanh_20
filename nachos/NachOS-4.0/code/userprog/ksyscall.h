/////////////////////////////////////////////////
// 	DH KHTN - DHQG TPHCM						/
// 	20120307		Phạm Gia Khiêm				/
// 	20120519		Nguyễn Thị Thúy Liễu		/
// 	20120540		Võ Hoàng Thảo Nguyên		/
/////////////////////////////////////////////////

/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__
#define __USERPROG_KSYSCALL_H__

#include "kernel.h"
#include "synchconsole.h"
#include "syscall.h"

#define MAXLENGTH 256
#define MODULUS 2147483647
#define CONST_C 16807

#define INT_MIN (-2147483647 - 1)

bool FIRSTRAND = true;

int RAND_NUM = 0;

// Input: - User space address (int)
// - Limit of buffer (int)
// Output:- Buffer (char*)
// Purpose: Copy buffer from User memory space to System memory space
char *User2System(int virtAddr, int limit) {
    int i; // index
    int oneChar;
    char *kernelBuf = NULL;
    kernelBuf = new char[limit + 1]; // need for terminal string

    if (kernelBuf == NULL)
        return kernelBuf;
    memset(kernelBuf, 0, limit + 1);

    for (i = 0; i < limit; i++) {
        kernel->machine->ReadMem(virtAddr + i, 1, &oneChar);
        kernelBuf[i] = (char)oneChar;
        if (oneChar == 0)
            break;
    }
    return kernelBuf;
}

// Input: - User space address (int)
// - Limit of buffer (int)
// - Buffer (char[])
// Output:- Number of bytes copied (int)
// Purpose: Copy buffer from System memory space to User memory space
int System2User(int virtAddr, char *buffer, int len) {
    if (len <= 0)
        return len;
    int i = 0;
    int oneChar = 0;
    do {
        oneChar = (int)buffer[i];
        kernel->machine->WriteMem(virtAddr + i, 1, oneChar);
        i++;
    } while (i < len && oneChar != 0);
    return i;
}

void IncreasePC()
/* Modify return point */
{
    /* set previous programm counter (debugging only)*/
    kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

    /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
    kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

    /* set next programm counter for brach execution */
    kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
}
// -------------------------------------------------------------------

void SysHalt() {
    kernel->interrupt->Halt();
}

int SysAdd(int op1, int op2) {
    return op1 + op2;
}

int SysSub(int op1, int op2) {
    return op1 - op2;
}

// Các hàm xử lý trong Project
void SysReadChar() {
    // Lấy kí tự từ người dùng nhập vào
    char ch = kernel->synchConsoleIn->GetChar();
    while (ch != '\n' && kernel->synchConsoleIn->GetChar() != '\n') {
    }
    // Ghi dữ liệu ký tự vào reg2
    kernel->machine->WriteRegister(2, ch);
}

void SysPrintChar() {
    // Lấy dữ liệu từ reg4
    char ch = (char)kernel->machine->ReadRegister(4);
    // Đưa lên system
    kernel->synchConsoleOut->PutChar(ch);
}

void SysReadString(int length) {

    // Tạo chuỗi ký tự buffer

    char *buffer = new char[length + 1];
    // Nếu buffer == NULL thì kết thúc (Do không đủ vùng nhớ để tạo)
    if (!buffer)
        return;

    // Vị trí i (index) của từng ký tự
    int i = 0;
    // Biến ch có kiểu char
    char ch;

    // Dùng vòng lặp để đọc chuỗi ký tự nhập vào
    while (i < length) {
        do {
            // Nhận ký tự từ system
            ch = kernel->synchConsoleIn->GetChar();
        } while (ch == EOF);
        // Khi nhập: Enter -> ket thuc nhap
        if (ch == '\0' || ch == '\n') {
            buffer[i] = '\0';
            break;
        }

        // Lưu ký tự vào chuỗi ký tự buffer
        buffer[i++] = ch;
    }

    // kiểm soát trường hợp tràn buffer;
    if (ch != '\0' && ch != '\n') {
        while (true) {
            ch = kernel->synchConsoleIn->GetChar();
            if (ch == '\0' || ch == '\n')
                break;
        }
    }

    // Gọi ptr là value của reg4
    int ptr = kernel->machine->ReadRegister(4);
    // Chuyển dữ liệu vùng nhớ buffer từ hệ thống (system) sang giao diện người dùng (user)
    // Với virAdd = ptr (giá trị reg4)
    //     buffer = buffer (dữ liệu của buffer từ system sẽ chuyển thành buffer của user)
    //     len = MAXLENGTH (Độ dài của buffer là MAXLENGTH)
    System2User(ptr, buffer, length);
    delete[] buffer;
}

void SysPrintString() {
    // Vị trí (index) của ký tự
    int i = 0;

    // Lấy giá trị (value) từ reg4
    int ptr = kernel->machine->ReadRegister(4);
    // printf("\n%d\n", ptr);
    // Đọc dữ liệu vùng nhớ từ giao diện người dùng (user)
    // Với virAdd = ptr (giá trị reg4)
    //     len = MAXLENGTH (độ dài của buffer người dùng)
    // Trả về giá trị char* (chuỗi dữ liệu) từ người dùng (user) gán vào char* buffer của hệ thống (system)
    char *buffer = User2System(ptr, MAXLENGTH);

    // Dùng vòng lặp để đưa lên hệ thống (system)
    while (buffer[i] != '\0') {
        kernel->synchConsoleOut->PutChar(buffer[i++]);
    }
    delete[] buffer;
}

void SysReadNum() {
    /*Input: NULL
    Output: số nguyên Int
    CN: Đọc số nguyên từ bàn phím
    */
    char *buffer = new char[MAXLENGTH + 1];
    if (!buffer)
        return;
    int i = 0;
    char ch;
    bool isInt = true;
    bool isNegative = false;
    int result = 0;
    while (i < MAXLENGTH) {
        do {
            ch = kernel->synchConsoleIn->GetChar();
        } while (ch == EOF);
        if (ch == '\0' || ch == '\n') // enter -> ket thuc nhap
            break;
        buffer[i++] = ch;
    }
    buffer[i] = '\0';

    // xoá bớt kí tự trắng ở đầu (nếu có)
    while (buffer[0] == ' ') {
        int n = strlen(buffer);
        for (int j = 0; j < n - 1; j++) {
            buffer[j] = buffer[j + 1];
        }
        buffer[n - 1] = '\0';
    }
    // xoá bớt kí tự trắng ở cuối (nếu có)
    while (buffer[strlen(buffer) - 1] == ' ' && strlen(buffer) > 0) {
        buffer[strlen(buffer) - 1] = '\0';
    }
    if (strlen(buffer) == 0) {

        isInt = false;
    }

    // Kiểm tra số nhập vào có phải là số âm hay không
    // Kiểm tra tràn số
    if (buffer[0] == '-') {
        isNegative = true;
        i = 1;
        if (strlen(buffer) > 11) {
            isInt = false;
            printf("bbbbb%d\n", strlen(buffer));
        }
        if (strlen(buffer) == 11) {
            if (strcmp(buffer, "-2147483648") > 0) {
                isInt = false;
            }
            if (strcmp(buffer, "-2147483648") == 0) {
                result = INT_MIN;
            }
        }
    }
    else {
        isNegative = false;
        i = 0;
        if (strlen(buffer) > 10) {
            isInt = false;
        }
        if (strlen(buffer) == 10) {
            if (strcmp(buffer, "2147483647") > 0) {
                isInt = false;
            }
        }
    }
    // Kiểm tra các kí tự nhập vào có phải số hay không
    if (isInt) {
        while (buffer[i] != '\0') {
            if (buffer[i] < 48 || buffer[i] > 57) {
                isInt = false;
                break;
            }
            i++;
        }
    }
    int num = 0;
    // chuyển chuỗi thành số
    if (isInt) {
        if (result != INT_MIN) {
            i = 0;
            if (isNegative) {
                i = 1;
            }
            while (buffer[i] != '\0') {
                num = num * 10 + int(buffer[i]) - '0';
                i++;
            }
            if (isNegative) {
                num = -num;
            }
            result = num;
        }
    }
    else {
        // printf("Input khong phai so nguyen\n");
        result = 0;
    }
    delete[] buffer;
    kernel->machine->WriteRegister(2, result);
}

void SysPrintNum() {
    /*Input: số nguyên Int
    Output: NULL
    CN: In một số nguyên ra màn hình
    */
    bool isNegative = false;
    int so = kernel->machine->ReadRegister(4);
    int i = 0;
    char *buffer = new char[12 + 1];
    if (so == INT_MIN) {
        strcpy(buffer, "-2147483648");
        buffer[strlen(buffer)] = '\0';
        i = 0;
        while (buffer[i] != '\0') {
            kernel->synchConsoleOut->PutChar(buffer[i++]);
        }
    }
    else {
        if (so != 0) {
            if (so < 0) {
                isNegative = true;
                so = -so;
            }
            int chuso = 0;
            while (so != 0) {
                chuso = so % 10;
                buffer[i] = chuso + '0';
                so = so / 10;
                i++;
            }
            if (isNegative) {
                buffer[i] = '-';
                i++;
            }
        }
        else {
            buffer[i] = '0';
            i++;
        }
        buffer[i] = '\0';
        while (i >= 0) {
            kernel->synchConsoleOut->PutChar(buffer[i--]);
        }
    }
    delete[] buffer;
}

// https://en.wikipedia.org/wiki/Linear_congruential_generator
void SysRandomNum() {
    int seed = kernel->stats->totalTicks;
    if (FIRSTRAND) {
        RAND_NUM = seed;
        FIRSTRAND = false;
    }
    RAND_NUM = (RAND_NUM * seed + CONST_C) % MODULUS;
    if (RAND_NUM < 0) {
        RAND_NUM = -(RAND_NUM);
    }
    kernel->machine->WriteRegister(2, RAND_NUM);
}
/* Xu ly syscall Read
    input: buffer la bo nho can doc, size la so luong ki tu can doc, id la id cua file
    output -1 neu loi nguoc lai tra ve so ki tu doc duoc
*/

int SysRead() {
    // Nguyen dang test may cac sysfile
    //  kernel->fileSystem->Print();
    //  kernel->fileSystem->List();
    //  kernel->fileSystem->Create("asad", 0);

    return 1;
}
#endif /* ! __USERPROG_KSYSCALL_H__ */