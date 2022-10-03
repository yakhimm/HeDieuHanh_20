/////////////////////////////////////////////////
// 	DH KHTN - DHQG TPHCM						/
// 	20120307		Phạm Gia Khiêm				/
// 	20120519		Nguyễn Thị Thúy Liễu		/
// 	20120540		Võ Hoàng Thảo Nguyên		/
/////////////////////////////////////////////////


// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	is in machine.h.
//----------------------------------------------------------------------

#define MaxFileLength 32

void ExceptionHandler(ExceptionType which)
{
    int type = kernel->machine->ReadRegister(2);

    DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

    switch (which) {
	// Add from slide [2]
	case NoException:
		return;

	case PageFaultException:
		/* A page fault occurs when a program attempts to access 
		data or code that is in its address space,
		but is not currently located in the system RAM
		*/
		DEBUG(dbgSys, "Invalid page found.\n");
		printf("\n\n Invalid page found");
		ASSERTNOTREACHED();
		break;
	
	case ReadOnlyException:
		/* Read-only is a file attribute which only allows a user to view a file,
		restricting any writing to the file. */
		DEBUG(dbgSys, "Restrict any writing to the read-only page.\n");
		printf("\n\n Restrict any writing to the read-only page.");
		ASSERTNOTREACHED();
		break;

	case BusErrorException:
		/*  bus error is a fault raised by hardware, notifying an operating system (OS)
		that a process is trying to access memory that the CPU cannot physically address:
		an invalid address for the address bus, hence the name.
		*/
		DEBUG(dbgSys, "Translation resulted invalid physical address.\n");
		printf("\n\n Translation resulted invalid physical address");
		ASSERTNOTREACHED();
		break;

	case AddressErrorException:
		/* An addressing error is detected by determining whether 
		the target address output on the address path is detected at the memory.
		*/
		DEBUG(dbgSys, "Unaligned reference or one that was beyond the end of the address space.\n");
		printf("\n\n Unaligned reference or one that was beyond the end of the address space");
		ASSERTNOTREACHED();
		break;

	case OverflowException:
		DEBUG(dbgSys, "Overflow in add or sub type.\n");
		printf("\n\n Overflow in add or sub type.");
		ASSERTNOTREACHED();
		break;

	case IllegalInstrException:
		// They arise because not every possible memory value corresponds to a valid instruction.
		DEBUG(dbgSys, "Unimplemented or reserved instruction.\n");
		printf("\n\n Unimplemented or reserved instruction.");
		ASSERTNOTREACHED();
		break;

	case NumExceptionTypes:
		DEBUG(dbgSys, "Number exception types.\n");
		printf("\n\n Number exception types");
		ASSERTNOTREACHED();
		break;

    case SyscallException:
		switch(type) {
		case SC_Halt:
			/*
				Input	: NULL
				Output	: Notify for shut down
				Used	: Shut down Operating System
			*/
			DEBUG(dbgSys, "Shutdown, initiated by user program.\n");
			printf ("\n\n Shutdown, initiated by user program.");
			SysHalt();
			ASSERTNOTREACHED();
			break;

		case SC_Add:
			/*
				Input	: r4, r5
				Output	: NULL
				Used	: Tính tổng giá trị của r4 và r5
			*/
			DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");
	
			/* Process SysAdd Systemcall*/
			int result;
			result = SysAdd(/* int op1 */(int)kernel->machine->ReadRegister(4),
					/* int op2 */(int)kernel->machine->ReadRegister(5));

			DEBUG(dbgSys, "Add returning with " << result << "\n");
			/* Prepare Result */
			kernel->machine->WriteRegister(2, (int)result);
	
			// Hàm tăng giá trị PC --------------------------------------------------------------
			/* Modify return point */
			{
				/* set previous programm counter (debugging only)*/
				kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

				/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
				kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
				
				/* set next programm counter for brach execution */
				kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
			}
			return;		
			ASSERTNOTREACHED();
			break;

	  	case SC_Create:
			/*
				Input	: Register 4 (chứa vùng nhớ của tên file)
				Output	: Thông báo kết quả tạo file (Lỗi = -1 | Thành công = 0)
				Used	: Tạo file với tên là filename
			*/
			int virtAddr;
			char* filename;

			DEBUG(dbgSys, "\n SC_Create call ...");
			DEBUG(dbgSys, "\n Reading virtual address of filename");
			// Lấy tham số trên tập tin từ thanh ghi r4
			virtAddr = kernel->machine->ReadRegister(4);
			DEBUG(dbgSys, "\n Reading filename.");
			// MaxFileLenght là 32
			filename = User2System(virtAddr, MaxFileLength + 1);
			if (filename == NULL)
			{
				printf("\n Not enough memory in system");
				DEBUG(dbgSys, "\n Not enough memory in system");
				kernel->machine->WriteRegister(2, -1); // Trả lỗi về cho chương trình người dùng
				delete filename;
				return;
			}
			DEBUG(dbgSys, "\n Finish reading filename.");
			//DEBUG(‘a’,"\n File name : '"<<filename<<"'");
			// Create file with size = 0

			// Dùng đối tượng fileSystem của lớp OpenFile để tạo file,
			// việc tạo file này là sử dụng các thủ tục tạo file của hệ điều
			// hành Linux, chúng ta không quản ly trực tiếp các block trên
			// đĩa cứng cấp phát cho file, việc quản ly các block của file
			// trên ổ đĩa là một đồ án khác

			if (!kernel->fileSystem->Create(filename, 0))
			{
				printf("\n Error create file '%s'",filename);
				kernel->machine->WriteRegister(2, -1);
				delete filename;
				return;
			}

			kernel->machine->WriteRegister(2, 0); // Trả về cho chương trình người dùng thành công !!
			delete filename;
			break;

		case SC_Sub:
			/*
				Input	: r4 r5
				Output	: NULL
				Used	: Tính hiệu của giá trị r4 - r5
			*/
			int result;

			result = SysSub(kernel->machine->ReadRegister(4)    // int op1
							, kernel->machine->ReadRegister(5));// int op2 
			
			kernel->machine->WriteRegister(2, (int)result);
			ASSERTNOTREACHED()

		case SC_ReadChar:
			/*
				Input	: NULL
				Output	: 1 ký tự (char)
				Used	: Đọc một ký tự từ người dùng nhập vào
			*/
			// Số lượng kí tự nhập vào 
			int c_char = kernel->synchConRead();

			if (c_char > 1)
			{
				printf("ERROR: So luong ky tu nhap vao phai la 1.\n");
				DEBUG(dbgSys, "ERROR: So luong ky tu nhap vao phai la 1.\n");
				kernel->machine->WriteRegister(2, 0);
			}
			if (c_char == 0)
			{
				printf("ERROR: Ky tu rong !!.\n");
				DEBUG(dbgSys, "ERROR: Ky tu rong !!.\n");
				kernel->machine->WriteRegister(2, 0);
			}
			if (c_char > 1)
			{
				// Chuỗi vừa lấy chỉ có 1 ký tự -> index = 0
				char _char = kernel->synchConGetChar();
				kernel->machine->WriteRegister(2, _char);
			}
			
			// Tăng PC
			break;
		case SC_PrintChar:
			/*
				Input	: Ký tự (char)
				Output	: Ký tự (char)
				Used	: Xuất 1 ký tự là tham số argv ra màn hình
			*/
			char _char = (char)kernel->machine->ReadRegister(4);
			kernel->synchConPutChar(_char);
			break;
      	default:
			cerr << "Unexpected system call " << type << "\n";
			break;
      	}
      	break;

	default:
		cerr << "Unexpected user mode exception" << (int)which << "\n";
		break;
    }
    ASSERTNOTREACHED();
}
