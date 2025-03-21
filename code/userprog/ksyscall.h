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

void SysHalt()
{
	kernel->interrupt->Halt();
}

int SysAdd(int op1, int op2)
{
	return op1 + op2;
}

#ifdef FILESYS_STUB	
// ifdef FILESYS_STUB - >means use UNIX FS (nachos itself no real FS)
int SysCreate(char *filename)
{
	// return value
	// 1: success
	// 0: failed
	return kernel->interrupt->CreateFile(filename);
}
#else

// case for real nachos  (MP4)
// below added in MP4
// system calls
int SysCreate(char *filename, int size)
{
	return kernel->fileSystem->CreateAFile(filename, size);
}

OpenFileId SysOpen(char *name)
{
  return kernel->fileSystem->OpenAFile(name);
}

int SysWrite(char *buffer, int size, OpenFileId id)
{
  return kernel->fileSystem->Write(buffer, size, id);
}

int SysRead(char *buffer, int size, OpenFileId id)
{
  return kernel->fileSystem->Read(buffer, size, id);
}

int SysClose(OpenFileId id)
{
  return kernel->fileSystem->Close(id);
}


#endif


#endif /* ! __USERPROG_KSYSCALL_H__ */
