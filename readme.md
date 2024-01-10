## Command

# Verification example
To test FS_test1, we have to compile FS_test1.c and copy it to the NachOS first.

```
cd NachOS-4.0_MP4
cd code
cd build.linux
```

../build.linux/nachos -cp FS_test1 /FS_test1

 ../build.linux/nachos -e /FS_test1

# Part 2-1 ver command

## Test case 1
```
../build.linux/nachos -f
../build.linux/nachos -d x -cp FS_test1 /FS_test1 
../build.linux/nachos -d x -e /FS_test1
../build.linux/nachos -p /file1
../build.linux/nachos -cp FS_test2 /FS_test2
../build.linux/nachos -e /FS_test2

```

`./FS_partII_a.sh`
## Things about size

Directory::Directory(int size)

## Trace Code (未改變前的nachos)


- Max file size: 3KB
- no synchronization for concurrent accesses
- Max number of files: 10

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
- freeMap 的一格->對應一個sector

|method|description|
|---|---|
|Bitmap::NumClear()| return no. clean sectors in the bitmap (not in use)|
|Bitmap::Mark(int which)| mark the nth bit of butmap as "in use"|


## Concept 
- bitmap 該格 = 0 = 該sector not set = clear = not in use
- bitmap 該格 = 1 = 該sector set = not clear = in use

### filehdr.h (未詳看)

`NumDirect` is a constant that represents the maximum number of direct pointers that a file header can have. It is calculated based on the size of a disk sector, subtracting the space needed for the numBytes and numSectors fields, and then dividing by the size of an int (which is the type of the pointers).

`dataSectors` is an array of NumDirect integers. Each integer in this array is a direct pointer to a disk sector. In other words, it's the sector number of a disk sector where a part of the file's data is stored.

When you want to read or write data from/to the file, you would use the dataSectors array to find out which disk sectors you need to access.

The dataSectors array is part of the "disk part" of the FileHeader class, which means it gets written to the disk when the file header is saved.



# 參考資料

## A road map through Nachos
https://users.cs.duke.edu/~narten/110/nachos/main/main.html

https://users.cs.duke.edu/~narten/110/nachos/main/node25.html#SECTION00063000000000000000

## Lab5_FileSystem of some scl (github 解釋+架構圖)
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

### (2) What is the maximum disk size

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

### (4)

- inode 應該就是 file header
- file header 有:
  - file's current size (類似allocate了的之中有多少是真的有用到的) (其他可能只是垃圾資訊(未初始化/之前沒flush到的))
  - allocate (分配)給file的sector數量
  - array of sector numbers (告知file的data block貯存在哪些sector)
  - (pointer to physical disk block?) (不確定)
![Alt text](nachos_file_hdr_diagram(before-modified).jpg)

- allocation scheme: indexed allocation

file size 限制

-   => file header 最多指向`NumDirect` 這麼多個sector
-  `NumDirect` = 30
-  ==> 一個file最多只能有30個sector
-  每sector = 128 Bytes
-  ==> 每個file = 30 * 128 Bytes = 3840 Bytes => 3840/1024 = 3.75KB

### (5)
3KB

filesys.cc 原本給的comment裡說的

不知道實際在code 的哪裡限制

```cpp
// filesys.cc
//	   files cannot be bigger than about 3KB in size

```