Building Instructions:
 * got to the directory build.<host>, where <host> is your working OS
 * do a "make depend" to build depenencies (DO IT!)
 * do a "make" to build NachOS

Usage:
see "nachos -u" for all command line options

Building and starting user-level programs in NachOS:
 * use Mips cross-compiler to build and link coff-binaries
 * use coff2noff to translate the binaries to the NachOS-format
 * start binary with nachos -x <path_to_file/file>


## Command

# Verification example
To test FS_test1, we have to compile FS_test1.c and copy it to the NachOS first.
../build.linux/nachos -cp FS_test1 /FS_test1

 ../build.linux/nachos -e /FS_test1

## Things about size

Directory::Directory(int size)

## Trace Code (未改變前的nachos)


- Max file size: 3KB
- no synchronization for concurrent accesses
- Max number of files: limited (不知道多少)

- bitmap that indicates which sectors are free -> on Sector 0
```cpp
// synchdisk.h
#define FreeMapSector 0
```

- directory -> on sector 1
```cpp
// synchdisk.h
#define DirectorySector 1
```

### Track, Sector limit

from: machine/disk.h

```cpp
// MP4 Hint: DO NOT change the SectorSize, but other constants are allowed
const int SectorSize = 128;		// number of bytes per disk sector [不能改!!]
const int SectorsPerTrack  = 32;	// number of sectors per disk track 
const int NumTracks = 32;		// number of tracks per disk
const int NumSectors = (SectorsPerTrack * NumTracks); // total # of sectors per disk
```

### FreeMap

- 一個disk 只有一個freeMap (should be)
- 記錄disk上的哪些sector是free的

|method|description|
|---|---|
|Bitmap::NumClear()| return no. clean sectors in the bitmap (not in use)|
|Bitmap::Mark(int which)| mark the nth bit of butmap as "in use"|


## Concept 
- bitmap 該格 = 0 = not set = clear = not in use
- bitmap 該格 = 1 = set = not clear = in use

### filehdr.h (未詳看)

`NumDirect` is a constant that represents the maximum number of direct pointers that a file header can have. It is calculated based on the size of a disk sector, subtracting the space needed for the numBytes and numSectors fields, and then dividing by the size of an int (which is the type of the pointers).

`dataSectors` is an array of NumDirect integers. Each integer in this array is a direct pointer to a disk sector. In other words, it's the sector number of a disk sector where a part of the file's data is stored.

When you want to read or write data from/to the file, you would use the dataSectors array to find out which disk sectors you need to access.

The dataSectors array is part of the "disk part" of the FileHeader class, which means it gets written to the disk when the file header is saved.

# !!! need to read what is `dataSectors` !!!

參考資料

# A road map through Nachos
https://users.cs.duke.edu/~narten/110/nachos/main/main.html

https://users.cs.duke.edu/~narten/110/nachos/main/node25.html#SECTION00063000000000000000

# Lab5_FileSystem of some scl (github 解釋+架構圖)
https://github.com/daviddwlee84/OperatingSystem/blob/master/Lab/Lab5_FileSystem/README.md

- Nachos file header = Unix's inode (ch11)
- 
- ![Alt text](NachosFileSystemStructure.jpg)


## Trace Code問題

### (1)
filesys.cc

用bitmap

#define FreeMapSector 0 < ---- bitmap的sector number

看FileSystem::FileSystem(bool format)

### (2)

看disk.h

### (3)
看filesys.cc
```cpp

Directory *directory = new Directory(NumDirEntries);
//....
FileHeader *dirHdr = new FileHeader;
//...
ASSERT(dirHdr->Allocate(freeMap, DirectoryFileSize));
 dirHdr->WriteBack(DirectorySector);

```

### (5)
32 (忘記在哪看到)