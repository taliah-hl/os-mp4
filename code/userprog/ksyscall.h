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
int SysCreate(char *filename)
{
	// return value
	// 1: success
	// 0: failed
	return kernel->interrupt->CreateFile(filename);
}
#else
// 200112[J]: MP4�s�W
int SysCreate(char *filename, int size)
{
	DEBUG(dbgMp4, "Syscreatein ksyscall.h is called");
  return kernel->fileSystem->CreateAFile(filename, size);
}

OpenFileId SysOpen(char *name)
{
  DEBUG(dbgMp4, "SysOpen in ksyscall.h is called");
  return kernel->fileSystem->OpenAFile(name);
}

int SysWrite(char *buffer, int size, OpenFileId id)
{
  DEBUG(dbgMp4, "SysWrite in ksyscall.h is called");
  return kernel->fileSystem->Write(buffer, size, id);
}

int SysRead(char *buffer, int size, OpenFileId id)
{
  DEBUG(dbgMp4, "SysRead in ksyscall.h is called");
  return kernel->fileSystem->Read(buffer, size, id);
}

int SysClose(OpenFileId id)
{
  DEBUG(dbgMp4, "SysClose in ksyscall.h is called");
  return kernel->fileSystem->Close(id);
}
// **********************************************************************
#endif


#endif /* ! __USERPROG_KSYSCALL_H__ */
