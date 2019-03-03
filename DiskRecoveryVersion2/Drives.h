#pragma once
#include<iostream>
#include<Windows.h>
#include<tchar.h>
#include<string>
#include<winioctl.h>
#include<iomanip>
#include<fstream>
#include<io.h>
#include"tools.h"
//#include"Directory.h"


using namespace std;

typedef unsigned long long BIGNUM;

//"DRIVE_UNKNOWN"		无法识别的设备
//"DRIVE_NO_ROOT_DIR"   给出的名字不存在
//"DRIVE_REMOVABLE"		可移动设备
//"DRIVE_FIXED"			不可移动的磁盘
//"DRIVE_REMOTE"		网络硬盘
//"DRIVE_CDROM"			CD光驱
//"DRIVE_RAMDISK"		内存虚拟盘  
const string typeMessage[7] = {
	"无法识别的设备",
	"给出的名字不存在",
	"可移动设备",
	"不可移动的磁盘",
	"网络硬盘",
	"CD光驱",
	"内存虚拟盘"
};


class Drives
{
private:

	TCHAR* name;	//分区名
	float allSize;	//分区总容量
	float availableSize;	//可用大小
	string fileSystem;	//文件系统
	int drivesType;		//逻辑分区类型，如：可移动磁盘，本地磁盘等
	DWORD totalClusters;//总的簇
	DWORD freeClusters;//可用的簇
	DWORD sectPerClust;//每个簇有多少个扇区
	DWORD bytesPerSect;//每个扇区有多少个字节
	DWORD volumeSerialNumber;	//分区序列号
	string volumeName;			//分区卷标
	DWORD firstSectByClust;	//根目录所占的起始扇区

public:
	static int num;	//记录总分区数

	Drives();
	~Drives();

	TCHAR * getName();
	void setName(TCHAR* name);

	float getAllSize();
	void setAllSize(float allSize);

	float getAvailableSize();
	void setAvailableSize(float availableSize);

	string getFileSystem();
	void setFileSystem(string fileSystem);

	int getdrivesType();
	void setdrivesType(int drivesType);

	DWORD getTotalClusters();
	void setTotalClusters(DWORD totalClusters);

	DWORD getFreeClusters();
	void setFreeClusters(DWORD freeClusters);

	DWORD getSectPerClust();
	void setSectPerClust(DWORD sectPerClust);

	DWORD getBytesPerSect();
	void setBytesPerSect(DWORD bytesPerSect);

	DWORD getVolumeSerialNumber();
	void setVolumeSerialNumber(DWORD volumeSerialNumber);

	string getVolumeName();
	void setVolumeName(string volumeName);

	//获取根目录所占的起始扇区
	DWORD getFirstSectByClust();
	BOOL setFirstSectByClust();

	//计算根目录所占首扇区的位置
	static DWORD getRootDirectoryLocation(byte* lpBuffer);

	//获取本机逻辑分区的信息
	static Drives* getDrivesMessage();

	//读取第n个扇区的16进制数据
	//第一个参数为Drives类对象
	//第二个参数为保存扇区数据的数组
	//第三个参数为要读取的第n个扇区
	BOOL getDriveByN(byte* lpBuffer, DWORD n);

	//读取偏移为n的16进制数据
	//第一个参数为保存扇区数据的数组
	//第二个参数为要读取的偏移
	//第三个为要从偏移开始要读取的字节数
	BOOL getDriveByOffset(byte* lpBuffer, DWORD offsetHead, int num);

	//读取第n个簇的16进制数据，each表示一次读取多少的簇
	BOOL getClustersByN(Drives drive, unsigned char* lpBuffer, DWORD n, int each);

	//获取磁盘总扇区数
	DWORD getSectorsSum();


	//BOOL getDriveByN(char* lpBuffer, DWORD n);

	//获取磁盘总字节数
	BIGNUM getDriveByteNumber() {
		return totalClusters * (DWORD64)sectPerClust*(DWORD64)bytesPerSect;
	}

	//获取簇的起始扇区
	DWORD getLocationByClus(DWORD num);







};


/////////////////////////////////////////定义头部中的其他函数///////////////////////////




//获取每个分区的信息
void eachDrives(Drives& drive);

//获取保留扇区数
DWORD getKeepSector(byte* lpBuffer);

//获取FAT表数
DWORD getFATNum(byte* lpBuffer);

//获取每个FAT表占用的扇区数
DWORD getSectorByFAT(byte* lpBuffer);

//获得根目录起始簇号
DWORD getFirstRootClusters(byte* lpBuffer);

//获取FAT1所占首扇区的位置
DWORD getFAT1Location(byte* lpBuffer);

//获取FAT2所占首扇区的位置
DWORD getFAT2Location(byte* lpBuffer);

//获取每个分区的信息
//void eachDrives(Drives& drive);



//输出16进制数据
//void HexOutput(unsigned char * buf, size_t len);







