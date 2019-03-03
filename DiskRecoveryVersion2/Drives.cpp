#include"Drives.h"


//////////////////////////////////////////////��ȡ���޸�˽�б�������////////////////////////////////////////////////
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

//��ȡ����������
float Drives::getAllSize() {
	return allSize;
}
//���÷���������
void Drives::setAllSize(float allSize) {
	this->allSize = allSize;
}

//��ȡ�������ô�С
float Drives::getAvailableSize() {
	return availableSize;
}
//���÷������ô�С
void Drives::setAvailableSize(float availableSize) {
	this->availableSize = availableSize;
}

//��ȡ������ʹ�õ��ļ�ϵͳ
string Drives::getFileSystem() {
	return fileSystem;
}
//���÷�����ʹ�õ��ļ�ϵͳ
void Drives::setFileSystem(string fileSystem) {
	this->fileSystem = fileSystem;
}

//��ȡ�߼��������ͣ����ػ����ƶ�
int Drives::getdrivesType() {
	return drivesType;
}
//�����߼��������ͣ����ػ����ƶ�
void Drives::setdrivesType(int drivesType) {
	this->drivesType = drivesType;
}

//��ȡ�����ܵĴ�
DWORD Drives::getTotalClusters() {
	return totalClusters;
}
//���÷����ܵĴ�
void Drives::setTotalClusters(DWORD totalClusters) {
	this->totalClusters = totalClusters;
}

//��ȡ�������õĴ�
DWORD Drives::getFreeClusters() {
	return freeClusters;
}
//���÷������õĴ�
void Drives::setFreeClusters(DWORD freeClusters) {
	this->freeClusters = freeClusters;
}

//��ȡÿ�����ж��ٸ�����
DWORD Drives::getSectPerClust() {
	return sectPerClust;
}
//����ÿ�����ж��ٸ�����
void Drives::setSectPerClust(DWORD sectPerClust) {
	this->sectPerClust = sectPerClust;
}

//��ȡÿ�������ж��ٸ��ֽ�
DWORD Drives::getBytesPerSect() {
	return bytesPerSect;
}
//����ÿ�������ж��ٸ��ֽ�
void Drives::setBytesPerSect(DWORD bytesPerSect) {
	this->bytesPerSect = bytesPerSect;
}

//���÷������к�
DWORD Drives::getVolumeSerialNumber() {
	return volumeSerialNumber;
}
//��ȡ�������к�
void Drives::setVolumeSerialNumber(DWORD volumeSerialNumber) {
	this->volumeSerialNumber = volumeSerialNumber;
}

//��ȡ�������
string Drives::getVolumeName() {
	return volumeName;
}
//���÷������
void Drives::setVolumeName(string volumeName) {
	this->volumeName = volumeName;
}

//��ȡ��Ŀ¼��ռ����ʼ����
DWORD Drives::getFirstSectByClust() {
	return firstSectByClust;
}
//���ø�Ŀ¼��ռ����ʼ����
BOOL Drives::setFirstSectByClust() {
	DWORD directoryLocation;	//Ŀ¼��ռ��������λ��
	byte buffer[SECTIONSIZE];

	//��ȡ������������ȡ��Ŀ¼��ռ��������λ��
	if (!this->getDriveByN(buffer, 0)) {
		cout << "��ȡ��������" << endl;
		return FALSE;
	}
	directoryLocation = Drives::getRootDirectoryLocation(buffer);

	this->firstSectByClust = directoryLocation;
	return TRUE;
}


//�����Ŀ¼��ռ��������λ��
DWORD Drives::getRootDirectoryLocation(byte* lpBuffer) {
	//������ƫ�� = ���������� +  FAT�������� * FAT�����(ͨ��Ϊ2) + (��ʼ�غ�-2) * ÿ��������
	DWORD result = getKeepSector(lpBuffer) + getFATNum(lpBuffer)*getSectorByFAT(lpBuffer) +
		(getFirstRootClusters(lpBuffer) - 2)*(DWORD)lpBuffer[13];
	return result;
}


///////////////////////////////////////��ȡ�����߼���������Ϣ////////////////////////////////////////////////////////////
Drives* Drives::getDrivesMessage() {
	int i = 0;
	//�ж��߼������ĸ���
	int diskNum = 0;

	DWORD diskInfo = GetLogicalDrives();//Ϊһ��2��������λ����ʾ���̴�A��ʼ

	while (diskInfo != 0) {
		if (diskInfo & 1) {//λ����Ϊ0���ʾ�����̴���
			diskNum++;
		}
		diskInfo >>= 1;
	}

	Drives* drives = new Drives[diskNum];
	Drives::num = diskNum;
	//cout << "�߼����̵ĸ���Ϊ��" << diskNum << endl;

	//��ȡ�ַ�����Ϣ����
	int diskLength = GetLogicalDriveStrings(0, NULL);
	TCHAR* diskStr = new TCHAR[diskLength];

	GetLogicalDriveStrings(diskLength, (LPTSTR)diskStr);

	TCHAR* test = diskStr;
	UINT driveType = NULL;
	for (i = 0; i < diskNum; i++) {
		//��ȡ������;
		drives[i].setName(test);
		//��ȡ���̵����� 
		driveType = GetDriveType(test);
		drives[i].setdrivesType(driveType);

		//��ȡÿ����������Ϣ
		eachDrives(drives[i]);

		test += 4;
	}

	return drives;
}

////////////////////////////////////////////////////��ȡ�����е�16��������////////////////////////////////////////////
//CreateFile(
//	LPCTSTR lpFileName,		//�ļ������豸·��		��ʽΪ��\\\\?\\C:
//	DWORD dwDesiredAccess,	//���ʷ�ʽ
//	DWORD dwShareMode,		//����ʽ
//	LPSECURITY_ATTRIBUTES lpSecurityAttributes,//��ȫ������ָ��
//	DWORD dwCreationDisposition,	//������ʽ
//	DWORD dwFlagsAndAttributes,		//�ļ����Լ���־
//	HANDLE hTemplateFile			//ģ���ļ����
//)
//һ��أ�����CreateFile����豸���ʱ��
//���ʷ�ʽ��������Ϊ0��GENERIC_READ|GENERIC_WRITE��dwDesiredAccess
//	0	��ϣ�����豸��ȡ���ݻ����豸д�����ݡ����ֻ��ı��豸������(�磺�޸��ļ���ʱ���)�����Դ�0
//	GENERIC_READ	������豸����ֻ������
//	GENERIC_WRITE	������豸����ֻд���ʣ���ѡ�û������ GENERIC_READ��־
//����ʽ��������ΪFILE_SHARE_READ|FILE_SHARE_WRITE��
//������ʽ��������ΪOPEN_EXISTING��
//������������Ϊ0��NULL
//
//BOOL DeviceIoControl(
//	HANDLE hDevice, // �豸���
//	DWORD dwIoControlCode, // ������
//	LPVOID lpInBuffer, // �������ݻ�����ָ��
//	DWORD nInBufferSize, // �������ݻ���������
//	LPVOID lpOutBuffer, // ������ݻ�����ָ��
//	DWORD nOutBufferSize, // ������ݻ���������
//	LPDWORD lpBytesReturned, // �������ʵ�ʳ��ȵ�Ԫ����
//	LPOVERLAPPED lpOverlapped // �ص������ṹָ��
//);
//IOCTL_DISK_GET_DRIVE_GEOMETRY �Ƕ�����������ȡ�ṹ���� ���������͡� ��������ÿ����ŵ����� ÿ�ŵ��������ȣ� �Ŀ�����
//FSCTL_LOCK_VOLUME�Ƕ��߼��������ľ�����Ŀ�����
//
//typedef struct _DISK_GEOMETRY
//{
//LARGE_INTEGER Cylinders;    // ������
//MEDIA_TYPE MediaType;       // �������ͣ���MSDN
//DWORD TracksPerCylinder;    // ÿ������Ĺ����
//DWORD SectorsPerTrack;      // ÿ�������������
//DWORD BytesPerSector;       // ÿ�����ֽ���
//} DISK_GEOMETRY;
//

//��ȡ��n��������16��������
//��һ������Ϊ�����������ݵ�����
//�ڶ�������ΪҪ��ȡ�ĵ�n������
BOOL Drives::getDriveByN(byte* lpBuffer, DWORD n) {
	HANDLE hDrive = NULL;		//������

	TCHAR name[256] = { TEXT("\\\\?\\") };
	TCHAR t[3] = { this->getName()[0],this->getName()[1] };//����һ����ʱ���飬����ȡ�ַ���
	lstrcat(name, t);//���ַ���ƴ��Ϊ���ƣ�\\\\?\\C:����������

	//�򿪴��̣���ȡ���
	hDrive = CreateFile(
		name,
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
	);

	//���߼�����ʧ��
	if (hDrive == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	LARGE_INTEGER offest;	//���ö�ȡ������ƫ��
	DWORD dwCB;
	offest.QuadPart = (ULONGLONG)n * (ULONGLONG)SECTIONSIZE;	//����Ϊ��n����������0��ʼ

	//����ƫ�ƣ�Ϊreadfile��׼��
	if (SetFilePointer(hDrive, offest.LowPart, &offest.HighPart, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
		CloseHandle(hDrive);
		return FALSE;
	}

	//��ȡ�����е�����
	if (!ReadFile(hDrive, lpBuffer, SECTIONSIZE, &dwCB, NULL)) {
		CloseHandle(hDrive);
		return FALSE;
	}

	CloseHandle(hDrive);
	return TRUE;
}


//��ȡƫ��Ϊn��16��������
//��һ������Ϊ�����������ݵ�����
//�ڶ�������ΪҪ��ȡ��ƫ��
//������ΪҪ��ƫ�ƿ�ʼҪ��ȡ���ֽ���
BOOL Drives::getDriveByOffset(byte* lpBuffer, DWORD offsetHead, int num) {
	HANDLE hDrive = NULL;		//������

	TCHAR name[256] = { TEXT("\\\\?\\") };
	TCHAR t[3] = { this->getName()[0],this->getName()[1] };//����һ����ʱ���飬����ȡ�ַ���
	lstrcat(name, t);//���ַ���ƴ��Ϊ���ƣ�\\\\?\\C:����������

	//�򿪴��̣���ȡ���
	hDrive = CreateFile(
		name,
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
	);

	//���߼�����ʧ��
	if (hDrive == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	//����ƫ�ƣ���Ϊ��APIֻ���ǰ��������Ĵ�С����ƫ��
	int sectors = offsetHead / SECTIONSIZE;
	int remainder = offsetHead % SECTIONSIZE;

	LARGE_INTEGER offest;	//���ö�ȡ������ƫ��
	DWORD dwCB;
	offest.QuadPart = (ULONGLONG)sectors * (ULONGLONG)SECTIONSIZE;	//����Ϊ��n����������0��ʼ

	//����ƫ�ƣ�Ϊreadfile��׼��
	if (SetFilePointer(hDrive, offest.LowPart, &offest.HighPart, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
		CloseHandle(hDrive);
		return FALSE;
	}

	byte* buffer;
	if (remainder + num <= SECTIONSIZE) {
		buffer = new byte[SECTIONSIZE];
		//��ȡ�����е�����
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

		//��ȡ�����е�����
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


//��ȡ������������
DWORD Drives::getSectorsSum() {
	return getFirstSectByClust() + totalClusters * sectPerClust;
}

//��ȡ�ص���ʼ����
DWORD Drives::getLocationByClus(DWORD num) {
	return firstSectByClust + (num - 2)*sectPerClust;
}

int Drives::num = 0;

//��ȡ��n���ص�16�������ݣ�each��ʾһ�ζ�ȡ���ٵĴ�
BOOL Drives::getClustersByN(Drives drive, unsigned char* lpBuffer, DWORD n, int each) {
	HANDLE hDrive = NULL;		//������

	TCHAR name[256] = { TEXT("\\\\?\\") };
	TCHAR t[3] = { this->getName()[0],this->getName()[1] };//����һ����ʱ���飬����ȡ�ַ���
	lstrcat(name, t);//���ַ���ƴ��Ϊ���ƣ�\\\\?\\C:����������

	//�򿪴��̣���ȡ���
	hDrive = CreateFile(
		name,
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
	);

	//���߼�����ʧ��
	if (hDrive == INVALID_HANDLE_VALUE) {
		return FALSE;
	}


	LARGE_INTEGER offest;	//���ö�ȡ������ƫ��
	DWORD dwCB;
	DWORD byteSectClut = drive.getSectPerClust()*drive.getBytesPerSect();//ÿ�����ֽ���
	offest.QuadPart = (ULONGLONG)(n - 2) * (ULONGLONG)byteSectClut + drive.getFirstSectByClust()*drive.getBytesPerSect();	//����Ϊ��n���أ���0��ʼ

	//����ƫ�ƣ�Ϊreadfile��׼��
	if (SetFilePointer(hDrive, offest.LowPart, &offest.HighPart, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
		CloseHandle(hDrive);
		return FALSE;
	}

	//��ȡ�����е�����
	if (!ReadFile(hDrive, lpBuffer, byteSectClut*each, &dwCB, NULL)) {
		CloseHandle(hDrive);
		return FALSE;
	}

	CloseHandle(hDrive);
	return TRUE;
}


////////////////////////////////////////�����������еĺ���/////////////////////////////////////////////////////////////////////////

////��ȡÿ����������Ϣ
void eachDrives(Drives& drive) {
	//�ó����̵Ŀ��ÿռ�
	DWORD dwTotalClusters;//�ܵĴ�
	DWORD dwFreeClusters;//���õĴ�
	DWORD dwSectPerClust;//ÿ�����ж��ٸ�����
	DWORD dwBytesPerSect;//ÿ�������ж��ٸ��ֽ�

	BOOL bResult = GetDiskFreeSpace(drive.getName(), &dwSectPerClust, &dwBytesPerSect, &dwFreeClusters, &dwTotalClusters);
	if (bResult) {
		drive.setTotalClusters(dwTotalClusters);
		drive.setFreeClusters(dwFreeClusters);
		drive.setSectPerClust(dwSectPerClust);
		drive.setBytesPerSect(dwBytesPerSect);
		//�����ܷ�����С
		float allSize = dwTotalClusters * (DWORD64)dwSectPerClust*(DWORD64)dwBytesPerSect / (float)1024 / 1024 / 1024;
		//������÷�����С
		float availableSize = dwFreeClusters * (DWORD64)dwSectPerClust*(DWORD64)dwBytesPerSect / (float)1024 / 1024 / 1024;
		drive.setAllSize(allSize);
		drive.setAvailableSize(availableSize);
	}
	else {
		cout << "��ȡ����ʧ�ܣ����Թ���Ա������У�" << endl;
		return;
	}

	//��ȡ�����ļ����ͼ�����
	DWORD   VolumeSerialNumber;	//�������к�
	TCHAR   FileSystemName[256];	//�ļ�ϵͳ
	TCHAR   VolumeName[256];		//�������
	bResult = GetVolumeInformation(drive.getName(), VolumeName, 12, &VolumeSerialNumber, NULL, NULL, FileSystemName, 10);
	if (bResult) {
		drive.setVolumeName(tcharToString(VolumeName));
		drive.setVolumeSerialNumber(VolumeSerialNumber);
		drive.setFileSystem(tcharToString(FileSystemName));
	}
	else {
		cout << "��ȡ����ʧ�ܣ����Թ���Ա������У�" << endl;
		return;
	}
}



//��ȡ����������
DWORD getKeepSector(byte* lpBuffer) {
	DWORD high = lpBuffer[15];	//0x0f
	DWORD low = lpBuffer[14];	//0x0e

	DWORD result = high * 16 * 16 + low;
	return result;
}

//��ȡFAT����
DWORD getFATNum(byte* lpBuffer) {
	DWORD num = lpBuffer[16];	//0x10
	return num;
}

//��ȡÿ��FAT��ռ�õ�������
DWORD getSectorByFAT(byte* lpBuffer) {
	DWORD one = lpBuffer[36];	//0x24
	DWORD two = lpBuffer[37];	//0x25
	DWORD three = lpBuffer[38];	//0x26
	DWORD four = lpBuffer[39];	//0x27

	DWORD sum = one + two * 16 * 16 + three * 16 * 16 * 16 * 16 + four * 16 * 16 * 16 * 16 * 16 * 16;
	return sum;
}

//��ø�Ŀ¼��ʼ�غ�
DWORD getFirstRootClusters(byte* lpBuffer) {
	DWORD one = lpBuffer[44];	//0x2c
	DWORD two = lpBuffer[45];	//0x2d
	DWORD three = lpBuffer[46];	//0x2e
	DWORD four = lpBuffer[47];	//0x2f
	DWORD sum = one + two * 16 * 16 + three * 16 * 16 * 16 * 16 + four * 16 * 16 * 16 * 16 * 16 * 16;
	return sum;
}

//��ȡFAT1��ռ��������λ��
DWORD getFAT1Location(byte* lpBuffer) {
	return getKeepSector(lpBuffer);
}

//��ȡFAT2��ռ��������λ��
DWORD getFAT2Location(byte* lpBuffer) {
	return getKeepSector(lpBuffer) + getSectorByFAT(lpBuffer);
}
