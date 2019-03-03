#include"Drives.h"


//////////////////////////////////////////////获取和修改私有变量函数////////////////////////////////////////////////
Drives::Drives() {
}

Drives::~Drives() {
}

TCHAR* Drives::getName() {
	return name;
}
void Drives::setName(TCHAR* name) {
	this->name = name;
}

//获取分区总容量
float Drives::getAllSize() {
	return allSize;
}
//设置分区总容量
void Drives::setAllSize(float allSize) {
	this->allSize = allSize;
}

//获取分区可用大小
float Drives::getAvailableSize() {
	return availableSize;
}
//设置分区可用大小
void Drives::setAvailableSize(float availableSize) {
	this->availableSize = availableSize;
}

//获取分区所使用的文件系统
string Drives::getFileSystem() {
	return fileSystem;
}
//设置分区所使用的文件系统
void Drives::setFileSystem(string fileSystem) {
	this->fileSystem = fileSystem;
}

//获取逻辑分区类型，本地还是移动
int Drives::getdrivesType() {
	return drivesType;
}
//设置逻辑分区类型，本地还是移动
void Drives::setdrivesType(int drivesType) {
	this->drivesType = drivesType;
}

//获取分区总的簇
DWORD Drives::getTotalClusters() {
	return totalClusters;
}
//设置分区总的簇
void Drives::setTotalClusters(DWORD totalClusters) {
	this->totalClusters = totalClusters;
}

//获取分区可用的簇
DWORD Drives::getFreeClusters() {
	return freeClusters;
}
//设置分区可用的簇
void Drives::setFreeClusters(DWORD freeClusters) {
	this->freeClusters = freeClusters;
}

//获取每个簇有多少个扇区
DWORD Drives::getSectPerClust() {
	return sectPerClust;
}
//设置每个簇有多少个扇区
void Drives::setSectPerClust(DWORD sectPerClust) {
	this->sectPerClust = sectPerClust;
}

//获取每个扇区有多少个字节
DWORD Drives::getBytesPerSect() {
	return bytesPerSect;
}
//设置每个扇区有多少个字节
void Drives::setBytesPerSect(DWORD bytesPerSect) {
	this->bytesPerSect = bytesPerSect;
}

//设置分区序列号
DWORD Drives::getVolumeSerialNumber() {
	return volumeSerialNumber;
}
//获取分区序列号
void Drives::setVolumeSerialNumber(DWORD volumeSerialNumber) {
	this->volumeSerialNumber = volumeSerialNumber;
}

//获取分区卷标
string Drives::getVolumeName() {
	return volumeName;
}
//设置分区卷标
void Drives::setVolumeName(string volumeName) {
	this->volumeName = volumeName;
}

//获取根目录所占的起始扇区
DWORD Drives::getFirstSectByClust() {
	return firstSectByClust;
}
//设置根目录所占的起始扇区
BOOL Drives::setFirstSectByClust() {
	DWORD directoryLocation;	//目录所占首扇区的位置
	byte buffer[SECTIONSIZE];

	//读取引导扇区，获取根目录所占首扇区的位置
	if (!this->getDriveByN(buffer, 0)) {
		cout << "读取磁盘有误！" << endl;
		return FALSE;
	}
	directoryLocation = Drives::getRootDirectoryLocation(buffer);

	this->firstSectByClust = directoryLocation;
	return TRUE;
}


//计算根目录所占首扇区的位置
DWORD Drives::getRootDirectoryLocation(byte* lpBuffer) {
	//数据区偏移 = 保留扇区数 +  FAT表扇区数 * FAT表个数(通常为2) + (起始簇号-2) * 每簇扇区数
	DWORD result = getKeepSector(lpBuffer) + getFATNum(lpBuffer)*getSectorByFAT(lpBuffer) +
		(getFirstRootClusters(lpBuffer) - 2)*(DWORD)lpBuffer[13];
	return result;
}


///////////////////////////////////////获取本机逻辑分区的信息////////////////////////////////////////////////////////////
Drives* Drives::getDrivesMessage() {
	int i = 0;
	//判断逻辑驱动的个数
	int diskNum = 0;

	DWORD diskInfo = GetLogicalDrives();//为一个2进制数，位数表示磁盘从A开始

	while (diskInfo != 0) {
		if (diskInfo & 1) {//位数不为0则表示本磁盘存在
			diskNum++;
		}
		diskInfo >>= 1;
	}

	Drives* drives = new Drives[diskNum];
	Drives::num = diskNum;
	//cout << "逻辑磁盘的个数为：" << diskNum << endl;

	//获取字符串信息长度
	int diskLength = GetLogicalDriveStrings(0, NULL);
	TCHAR* diskStr = new TCHAR[diskLength];

	GetLogicalDriveStrings(diskLength, (LPTSTR)diskStr);

	TCHAR* test = diskStr;
	UINT driveType = NULL;
	for (i = 0; i < diskNum; i++) {
		//获取磁盘名;
		drives[i].setName(test);
		//获取磁盘的类型 
		driveType = GetDriveType(test);
		drives[i].setdrivesType(driveType);

		//获取每个分区的信息
		eachDrives(drives[i]);

		test += 4;
	}

	return drives;
}

////////////////////////////////////////////////////读取分区中的16进制数据////////////////////////////////////////////
//CreateFile(
//	LPCTSTR lpFileName,		//文件名或设备路径		格式为：\\\\?\\C:
//	DWORD dwDesiredAccess,	//访问方式
//	DWORD dwShareMode,		//共享方式
//	LPSECURITY_ATTRIBUTES lpSecurityAttributes,//安全描述符指针
//	DWORD dwCreationDisposition,	//创建方式
//	DWORD dwFlagsAndAttributes,		//文件属性及标志
//	HANDLE hTemplateFile			//模板文件句柄
//)
//一般地，调用CreateFile获得设备句柄时，
//访问方式参数设置为0或GENERIC_READ|GENERIC_WRITE，dwDesiredAccess
//	0	不希望从设备读取数据或向设备写入数据。如果只想改变设备的配置(如：修改文件的时间戳)，可以传0
//	GENERIC_READ	允许对设备进行只读访问
//	GENERIC_WRITE	允许对设备进行只写访问，该选项并没有隐含 GENERIC_READ标志
//共享方式参数设置为FILE_SHARE_READ|FILE_SHARE_WRITE，
//创建方式参数设置为OPEN_EXISTING，
//其它参数设置为0或NULL
//
//BOOL DeviceIoControl(
//	HANDLE hDevice, // 设备句柄
//	DWORD dwIoControlCode, // 控制码
//	LPVOID lpInBuffer, // 输入数据缓冲区指针
//	DWORD nInBufferSize, // 输入数据缓冲区长度
//	LPVOID lpOutBuffer, // 输出数据缓冲区指针
//	DWORD nOutBufferSize, // 输出数据缓冲区长度
//	LPDWORD lpBytesReturned, // 输出数据实际长度单元长度
//	LPOVERLAPPED lpOverlapped // 重叠操作结构指针
//);
//IOCTL_DISK_GET_DRIVE_GEOMETRY 是对物理驱动器取结构参数 （介质类型、 柱面数、每柱面磁道数、 每磁道扇区数等） 的控制码
//FSCTL_LOCK_VOLUME是对逻辑驱动器的卷加锁的控制码
//
//typedef struct _DISK_GEOMETRY
//{
//LARGE_INTEGER Cylinders;    // 柱面数
//MEDIA_TYPE MediaType;       // 磁盘类型，见MSDN
//DWORD TracksPerCylinder;    // 每个柱面的轨道数
//DWORD SectorsPerTrack;      // 每个轨道道扇区数
//DWORD BytesPerSector;       // 每扇区字节数
//} DISK_GEOMETRY;
//

//读取第n个扇区的16进制数据
//第一个参数为保存扇区数据的数组
//第二个参数为要读取的第n个扇区
BOOL Drives::getDriveByN(byte* lpBuffer, DWORD n) {
	HANDLE hDrive = NULL;		//定义句柄

	TCHAR name[256] = { TEXT("\\\\?\\") };
	TCHAR t[3] = { this->getName()[0],this->getName()[1] };//创建一个临时数组，来截取字符串
	lstrcat(name, t);//将字符串拼接为类似：\\\\?\\C:这样的类型

	//打开磁盘，获取句柄
	hDrive = CreateFile(
		name,
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
	);

	//打开逻辑分区失败
	if (hDrive == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	LARGE_INTEGER offest;	//设置读取扇区的偏移
	DWORD dwCB;
	offest.QuadPart = (ULONGLONG)n * (ULONGLONG)SECTIONSIZE;	//设置为第n个扇区，从0开始

	//设置偏移，为readfile做准备
	if (SetFilePointer(hDrive, offest.LowPart, &offest.HighPart, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
		CloseHandle(hDrive);
		return FALSE;
	}

	//读取磁盘中的数据
	if (!ReadFile(hDrive, lpBuffer, SECTIONSIZE, &dwCB, NULL)) {
		CloseHandle(hDrive);
		return FALSE;
	}

	CloseHandle(hDrive);
	return TRUE;
}


//读取偏移为n的16进制数据
//第一个参数为保存扇区数据的数组
//第二个参数为要读取的偏移
//第三个为要从偏移开始要读取的字节数
BOOL Drives::getDriveByOffset(byte* lpBuffer, DWORD offsetHead, int num) {
	HANDLE hDrive = NULL;		//定义句柄

	TCHAR name[256] = { TEXT("\\\\?\\") };
	TCHAR t[3] = { this->getName()[0],this->getName()[1] };//创建一个临时数组，来截取字符串
	lstrcat(name, t);//将字符串拼接为类似：\\\\?\\C:这样的类型

	//打开磁盘，获取句柄
	hDrive = CreateFile(
		name,
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
	);

	//打开逻辑分区失败
	if (hDrive == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	//计算偏移，因为该API只能是按照扇区的大小设置偏移
	int sectors = offsetHead / SECTIONSIZE;
	int remainder = offsetHead % SECTIONSIZE;

	LARGE_INTEGER offest;	//设置读取扇区的偏移
	DWORD dwCB;
	offest.QuadPart = (ULONGLONG)sectors * (ULONGLONG)SECTIONSIZE;	//设置为第n个扇区，从0开始

	//设置偏移，为readfile做准备
	if (SetFilePointer(hDrive, offest.LowPart, &offest.HighPart, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
		CloseHandle(hDrive);
		return FALSE;
	}

	byte* buffer;
	if (remainder + num <= SECTIONSIZE) {
		buffer = new byte[SECTIONSIZE];
		//读取磁盘中的数据
		if (!ReadFile(hDrive, buffer, SECTIONSIZE, &dwCB, NULL)) {
			CloseHandle(hDrive);
			return FALSE;
		}
	}
	else {
		int realRead = (remainder + num) / SECTIONSIZE;
		if ((remainder + num) % SECTIONSIZE != 0) {
			realRead += 1;
		}

		buffer = new byte[realRead * SECTIONSIZE];

		//读取磁盘中的数据
		if (!ReadFile(hDrive, buffer, realRead*SECTIONSIZE, &dwCB, NULL)) {
			CloseHandle(hDrive);
			return FALSE;
		}
	}

	for (int i = 0; i < num; i++) {
		lpBuffer[i] = buffer[remainder + i];
	}
	
	delete buffer;
	

	CloseHandle(hDrive);
	return TRUE;
}


//获取磁盘总扇区数
DWORD Drives::getSectorsSum() {
	return getFirstSectByClust() + totalClusters * sectPerClust;
}

//获取簇的起始扇区
DWORD Drives::getLocationByClus(DWORD num) {
	return firstSectByClust + (num - 2)*sectPerClust;
}

int Drives::num = 0;

//读取第n个簇的16进制数据，each表示一次读取多少的簇
BOOL Drives::getClustersByN(Drives drive, unsigned char* lpBuffer, DWORD n, int each) {
	HANDLE hDrive = NULL;		//定义句柄

	TCHAR name[256] = { TEXT("\\\\?\\") };
	TCHAR t[3] = { this->getName()[0],this->getName()[1] };//创建一个临时数组，来截取字符串
	lstrcat(name, t);//将字符串拼接为类似：\\\\?\\C:这样的类型

	//打开磁盘，获取句柄
	hDrive = CreateFile(
		name,
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
	);

	//打开逻辑分区失败
	if (hDrive == INVALID_HANDLE_VALUE) {
		return FALSE;
	}


	LARGE_INTEGER offest;	//设置读取扇区的偏移
	DWORD dwCB;
	DWORD byteSectClut = drive.getSectPerClust()*drive.getBytesPerSect();//每个簇字节数
	offest.QuadPart = (ULONGLONG)(n - 2) * (ULONGLONG)byteSectClut + drive.getFirstSectByClust()*drive.getBytesPerSect();	//设置为第n个簇，从0开始

	//设置偏移，为readfile做准备
	if (SetFilePointer(hDrive, offest.LowPart, &offest.HighPart, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
		CloseHandle(hDrive);
		return FALSE;
	}

	//读取磁盘中的数据
	if (!ReadFile(hDrive, lpBuffer, byteSectClut*each, &dwCB, NULL)) {
		CloseHandle(hDrive);
		return FALSE;
	}

	CloseHandle(hDrive);
	return TRUE;
}


////////////////////////////////////////不包含在类中的函数/////////////////////////////////////////////////////////////////////////

////获取每个分区的信息
void eachDrives(Drives& drive) {
	//得出磁盘的可用空间
	DWORD dwTotalClusters;//总的簇
	DWORD dwFreeClusters;//可用的簇
	DWORD dwSectPerClust;//每个簇有多少个扇区
	DWORD dwBytesPerSect;//每个扇区有多少个字节

	BOOL bResult = GetDiskFreeSpace(drive.getName(), &dwSectPerClust, &dwBytesPerSect, &dwFreeClusters, &dwTotalClusters);
	if (bResult) {
		drive.setTotalClusters(dwTotalClusters);
		drive.setFreeClusters(dwFreeClusters);
		drive.setSectPerClust(dwSectPerClust);
		drive.setBytesPerSect(dwBytesPerSect);
		//计算总分区大小
		float allSize = dwTotalClusters * (DWORD64)dwSectPerClust*(DWORD64)dwBytesPerSect / (float)1024 / 1024 / 1024;
		//计算可用分区大小
		float availableSize = dwFreeClusters * (DWORD64)dwSectPerClust*(DWORD64)dwBytesPerSect / (float)1024 / 1024 / 1024;
		drive.setAllSize(allSize);
		drive.setAvailableSize(availableSize);
	}
	else {
		cout << "读取磁盘失败，请以管理员身份运行！" << endl;
		return;
	}

	//获取分区文件类型及其他
	DWORD   VolumeSerialNumber;	//分区序列号
	TCHAR   FileSystemName[256];	//文件系统
	TCHAR   VolumeName[256];		//分区卷标
	bResult = GetVolumeInformation(drive.getName(), VolumeName, 12, &VolumeSerialNumber, NULL, NULL, FileSystemName, 10);
	if (bResult) {
		drive.setVolumeName(tcharToString(VolumeName));
		drive.setVolumeSerialNumber(VolumeSerialNumber);
		drive.setFileSystem(tcharToString(FileSystemName));
	}
	else {
		cout << "读取磁盘失败，请以管理员身份运行！" << endl;
		return;
	}
}



//获取保留扇区数
DWORD getKeepSector(byte* lpBuffer) {
	DWORD high = lpBuffer[15];	//0x0f
	DWORD low = lpBuffer[14];	//0x0e

	DWORD result = high * 16 * 16 + low;
	return result;
}

//获取FAT表数
DWORD getFATNum(byte* lpBuffer) {
	DWORD num = lpBuffer[16];	//0x10
	return num;
}

//获取每个FAT表占用的扇区数
DWORD getSectorByFAT(byte* lpBuffer) {
	DWORD one = lpBuffer[36];	//0x24
	DWORD two = lpBuffer[37];	//0x25
	DWORD three = lpBuffer[38];	//0x26
	DWORD four = lpBuffer[39];	//0x27

	DWORD sum = one + two * 16 * 16 + three * 16 * 16 * 16 * 16 + four * 16 * 16 * 16 * 16 * 16 * 16;
	return sum;
}

//获得根目录起始簇号
DWORD getFirstRootClusters(byte* lpBuffer) {
	DWORD one = lpBuffer[44];	//0x2c
	DWORD two = lpBuffer[45];	//0x2d
	DWORD three = lpBuffer[46];	//0x2e
	DWORD four = lpBuffer[47];	//0x2f
	DWORD sum = one + two * 16 * 16 + three * 16 * 16 * 16 * 16 + four * 16 * 16 * 16 * 16 * 16 * 16;
	return sum;
}

//获取FAT1所占首扇区的位置
DWORD getFAT1Location(byte* lpBuffer) {
	return getKeepSector(lpBuffer);
}

//获取FAT2所占首扇区的位置
DWORD getFAT2Location(byte* lpBuffer) {
	return getKeepSector(lpBuffer) + getSectorByFAT(lpBuffer);
}
