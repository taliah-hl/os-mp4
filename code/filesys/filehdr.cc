// filehdr.cc
//	Routines for managing the disk file header (in UNIX, this
//	would be called the i-node).
//
//	The file header is used to locate where on disk the
//	file's data is stored.  We implement this as a fixed size
//	table of pointers -- each entry in the table points to the
//	disk sector containing that portion of the file data
//	(in other words, there are no indirect or doubly indirect
//	blocks). The table size is chosen so that the file header
//	will be just big enough to fit in one disk sector,
//
//      Unlike in a real system, we do not keep track of file permissions,
//	ownership, last modification date, etc., in the file header.
//
//	A file header can be initialized in two ways:
//	   for a new file, by modifying the in-memory data structure
//	     to point to the newly allocated data blocks
//	   for a file already on disk, by reading the file header from disk
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#include "filehdr.h"
#include "debug.h"
#include "synchdisk.h"
#include "main.h"

//----------------------------------------------------------------------
// MP4 mod tag
// FileHeader::FileHeader
//	There is no need to initialize a fileheader,
//	since all the information should be initialized by Allocate or FetchFrom.
//	The purpose of this function is to keep valgrind happy.
//----------------------------------------------------------------------
FileHeader::FileHeader()
{

	DEBUG(dbgMp4, "constructor of fileheader is called");
	numBytes = -1;
	numSectors = -1;
	memset(dataSectors, -1, sizeof(dataSectors));
	lastOccupiedIdx = -1;
	nextFileHdrSector = -1;
	nextFileHdr=NULL;
}

//----------------------------------------------------------------------
// MP4 mod tag
// FileHeader::~FileHeader
//	Currently, there is not need to do anything in destructor function.
//	However, if you decide to add some "in-core" data in header
//	Always remember to deallocate their space or you will leak memory
//----------------------------------------------------------------------
FileHeader::~FileHeader()
{
	// nothing to do now
	// if (nextFileHdr != NULL)
	// 	delete nextFileHdr;
}

//----------------------------------------------------------------------
// FileHeader::Allocate
// 	Initialize a fresh file header for a newly created file.
//	Allocate data blocks for the file out of the map of free disk blocks.
//	Return FALSE if there are not enough free blocks to accomodate
//	the new file.
//
//	"freeMap" is the bit map of free disk sectors
//	"fileSize" is the bit map of free disk sectors
//----------------------------------------------------------------------

bool FileHeader::Allocate(PersistentBitmap *freeMap, int fileSize)		// return true = have space; false= no enough space
{
	
	DEBUG(dbgMp4, "FileHeader::Allocate is allocating fileSize: " << fileSize);
	// return: success or not
	
	//bool success = FALSE;
	numBytes = fileSize < MaxFileSize? fileSize:MaxFileSize;
	// calculate numbyte of this layer
	int notAllocatedBytes = fileSize - numBytes;	// 這層layer不夠位裝的byte數

	numSectors = divRoundUp(numBytes, SectorSize);
	if (freeMap->NumClear() < numSectors)
		return FALSE; // not enough space


	// 因一個file會用到超過一個sector
	// dataSectors 每一格存這個file 的哪個sector對應到disk 上面第幾個sector(=bitmap上第幾個index)
	for (int i = 0; i < numSectors; i++)
	{


		dataSectors[i] = freeMap->FindAndSet();		// 找出freeMap中第一格not in use 的index
													// if -1 means all are used
		// since we checked that there was enough free space,
		// we expect this to succeed
		ASSERT(dataSectors[i] >= 0);
	}
	if(notAllocatedBytes > 0){
		// allocate next layer
		nextFileHdrSector = freeMap->FindAndSet();
		if (nextFileHdrSector < 0){
			DEBUG(dbgMp4, "in FileHeader::Allocate: no enough space for next layer");
			return FALSE;
		}
		DEBUG(dbgMp4, "in FileHeader::Allocate, allocated to sector " << nextFileHdrSector);
			
		nextFileHdr = new FileHeader();
		
		ASSERT(nextFileHdrSector >= 0);
		// allocate next layer
		return nextFileHdr->Allocate(freeMap, notAllocatedBytes);	
		// return success or not from next layer 
	}
	

	return TRUE;
}

//----------------------------------------------------------------------
// FileHeader::Deallocate
// 	De-allocate all the space allocated for data blocks for this file.
//
//	"freeMap" is the bit map of free disk sectors
//----------------------------------------------------------------------

void FileHeader::Deallocate(PersistentBitmap *freeMap)
{
	DEBUG(dbgMp4, "FileHeader::Deallocate is called");
	for (int i = 0; i < numSectors; i++)
	{
		ASSERT(freeMap->Test((int)dataSectors[i])); // ought to be marked!
		freeMap->Clear((int)dataSectors[i]);
	}
	if((nextFileHdrSector != -1) && (nextFileHdr == NULL) ){
		DEBUG(dbgMp4, "in FileHeader::Deallocate, potential error: nextFileHdrSector != -1 but nextFileHdr == NULL");
	}
	if(nextFileHdr != NULL){
		nextFileHdr->Deallocate(freeMap);
	}
}

//----------------------------------------------------------------------
// FileHeader::FetchFrom
// 	Fetch contents of file header from disk.
//
//	"sector" is the disk sector containing the file header
//----------------------------------------------------------------------

void FileHeader::FetchFrom(int sector)
{
	DEBUG(dbgMp4, "FileHeader::FetchFrom is fetching from sector " << sector);
	kernel->synchDisk->ReadSector(sector, (char *)this);
	
	/*
		MP4 Hint:
		After you add some in-core informations, you will need to rebuild the header's structure
	*/
	if(nextFileHdrSector != -1){	// this is necceaary becaause next filehdr may not have been loaded to memory
		nextFileHdr = new FileHeader();
		nextFileHdr->FetchFrom(nextFileHdrSector);
	}
	
		
}

//----------------------------------------------------------------------
// FileHeader::WriteBack
// 	Write the modified contents of the file header back to disk.
//
//	"sector" is the disk sector to contain the file header
//----------------------------------------------------------------------

void FileHeader::WriteBack(int sector)
{
	DEBUG(dbgMp4, "FileHeader::WriteBack is running");
	kernel->synchDisk->WriteSector(sector, (char *)this); 

	/*
		MP4 Hint:
		After you add some in-core informations, you may not want to write all fields into disk.
		Use this instead:
		char buf[SectorSize];
		memcpy(buf + offset, &dataToBeWritten, sizeof(dataToBeWritten));
		...
	*/
	if((nextFileHdrSector != -1) && (nextFileHdr == NULL) ){
		DEBUG(dbgMp4, "in FileHeader::WriteBack, potential error: nextFileHdrSector != -1 but nextFileHdr == NULL");
	}
	if(nextFileHdr != NULL){
		nextFileHdr->WriteBack(nextFileHdrSector);
	}
}

//----------------------------------------------------------------------
// FileHeader::ByteToSector
// 	Return which disk sector is storing a particular byte within the file.
//      This is essentially a translation from a virtual address (the
//	offset in the file) to a physical address (the sector where the
//	data at the offset is stored).
//
//	"offset" is the location within the file of the byte in question
//----------------------------------------------------------------------

int FileHeader::ByteToSector(int offset)
{
	DEBUG(dbgMp4, "in FileHeader::ByteToSector, offset = " << offset);
	int idx = offset / SectorSize;
	if(idx >= NumDirect){
		if(nextFileHdr == NULL){
			DEBUG(dbgMp4, "in FileHeader::ByteToSector, potential error: filesize > MaxFilesize but no next file header");
			ASSERT(FALSE); // kill the program
		}
		return nextFileHdr->ByteToSector(offset - MaxFileSize);
	}
	return (dataSectors[idx]);
}

//----------------------------------------------------------------------
// FileHeader::FileLength
// 	Return the number of bytes in the file.
//----------------------------------------------------------------------

int FileHeader::FileLength()	// Return the length of total file
{
	
	if((nextFileHdrSector != -1) && (nextFileHdr == NULL) ){
		DEBUG(dbgMp4, "in FileHeader::FileLength, potential error: nextFileHdrSector != -1 but nextFileHdr == NULL");
	}
	if(nextFileHdr != NULL){
		int len = numBytes + nextFileHdr->FileLength();
		return len;
	}
	return numBytes;
	
}


//----------------------------------------------------------------------
// FileHeader::Print
// 	Print the contents of the file header, and the contents of all
//	the data blocks pointed to by the file header.
//----------------------------------------------------------------------

void FileHeader::Print()
{
	int i, j, k;
	char *data = new char[SectorSize];

	printf("FileHeader contents.  File size: %d.  File blocks:\n", numBytes);
	for (i = 0; i < numSectors; i++)
		printf("%d ", dataSectors[i]);
	printf("\nFile contents:\n");
	for (i = k = 0; i < numSectors; i++)
	{
		kernel->synchDisk->ReadSector(dataSectors[i], data);
		for (j = 0; (j < SectorSize) && (k < numBytes); j++, k++)
		{
			if ('\040' <= data[j] && data[j] <= '\176') // isprint(data[j])
				printf("%c", data[j]);
			else
				printf("\\%x", (unsigned char)data[j]);
		}
		printf("\n");
	}
	delete[] data;
}
