// synchconsole.cc 
//	Routines providing synchronized access to the keyboard 
//	and console display hardware devices.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synchconsole.h"

//----------------------------------------------------------------------
// SynchConsoleInput::SynchConsoleInput
//      Initialize synchronized access to the keyboard
//
//      "inputFile" -- if NULL, use stdin as console device
//              otherwise, read from this file
//----------------------------------------------------------------------

SynchConsoleInput::SynchConsoleInput(char *inputFile)
{
    consoleInput = new ConsoleInput(inputFile, this);
    lock = new Lock("console in");
    waitFor = new Semaphore("console in", 0);
}

//----------------------------------------------------------------------
// SynchConsoleInput::~SynchConsoleInput
//      Deallocate data structures for synchronized access to the keyboard
//----------------------------------------------------------------------

SynchConsoleInput::~SynchConsoleInput()
{ 
    delete consoleInput; 
    delete lock; 
    delete waitFor;
}

//----------------------------------------------------------------------
// SynchConsoleInput::GetChar
//      Read a character typed at the keyboard, waiting if necessary.
//----------------------------------------------------------------------

char SynchConsoleInput::GetChar()
{
    char ch;

    lock->Acquire();
    waitFor->P();	// wait for EOF or a char to be available.
    ch = consoleInput->GetChar();
    lock->Release();
    
    return ch;
}

int SynchConsoleInput::Read(char* buffer, int max_bytes)
{
    // Thực hiện vòng lặp
    int i;
    // xác định kết thúc line đọc vào hay chưa
    int EOL = FALSE;
    char ch;

    // Khởi tạo giá trị cho s_char truyền vào:
    for (i = 0; i < max_bytes; i++)
        buffer[i] = 0;
    // Khởi tạo lại i
    i = 0;

    lock->Acquire();                                // Khóa lại để đọc 1 dòng

    while ((i < max_bytes) && (EOL == FALSE))
    {
        do {
            waitFor->P();	// wait for EOF or a char to be available.
            ch = consoleInput->GetChar();
        } while (ch == EOF);
        if ((ch == '\012') || (ch == '\001'))
            EOL = TRUE;
        else
        {
            buffer[i] = ch;
            i++;
        }
    }

    lock->Release();                                // Mở khóa

    if (ch == '\001')
        return -1;
    else    
        return i;            // Số lượng ký tự nhập vào 
}

//----------------------------------------------------------------------
// SynchConsoleInput::CallBack
//      Interrupt handler called when keystroke is hit; wake up
//	anyone waiting.
//----------------------------------------------------------------------

void SynchConsoleInput::CallBack()
{
    waitFor->V();
}

//----------------------------------------------------------------------
// SynchConsoleOutput::SynchConsoleOutput
//      Initialize synchronized access to the console display
//
//      "outputFile" -- if NULL, use stdout as console device
//              otherwise, read from this file
//----------------------------------------------------------------------

SynchConsoleOutput::SynchConsoleOutput(char *outputFile)
{
    consoleOutput = new ConsoleOutput(outputFile, this);
    lock = new Lock("console out");
    waitFor = new Semaphore("console out", 0);
}

//----------------------------------------------------------------------
// SynchConsoleOutput::~SynchConsoleOutput
//      Deallocate data structures for synchronized access to the keyboard
//----------------------------------------------------------------------

SynchConsoleOutput::~SynchConsoleOutput()
{ 
    delete consoleOutput; 
    delete lock; 
    delete waitFor;
}

//----------------------------------------------------------------------
// SynchConsoleOutput::PutChar
//      Write a character to the console display, waiting if necessary.
//----------------------------------------------------------------------

void SynchConsoleOutput::PutChar(char ch)
{
    lock->Acquire();

    consoleOutput->PutChar(ch);

    waitFor->P();
    lock->Release();
}

void SynchConsoleOutput::Write(char* buffer, int max_bytes)
{
    // Khởi tạo vòng lặp
    int i;

    lock->Acquire();            // Khóa line
    
    for (i = 0; i < max_bytes; i++)
    {
        consoleOutput->PutChar(buffer[i]);
        waitFor->P();
    }
    
    lock->Release();            // Mở khóa
}
//----------------------------------------------------------------------
// SynchConsoleOutput::CallBack
//      Interrupt handler called when it's safe to send the next 
//	character can be sent to the display.
//----------------------------------------------------------------------

void SynchConsoleOutput::CallBack()
{
    waitFor->V();
}
