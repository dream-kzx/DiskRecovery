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

//"DRIVE_UNKNOWN"		�޷�ʶ����豸
//"DRIVE_NO_ROOT_DIR"   ���������ֲ�����
//"DRIVE_REMOVABLE"		���ƶ��豸
//"DRIVE_FIXED"			�����ƶ��Ĵ���
//"DRIVE_REMOTE"		����Ӳ��
//"DRIVE_CDROM"			CD����
//"DRIVE_RAMDISK"		�ڴ�������  
const string typeMessage[7] = {
	"�޷�ʶ����豸",
	"���������ֲ�����",
	"���ƶ��豸",
	"�����ƶ��Ĵ���",
	"����Ӳ��",
	"CD����",
	"�ڴ�������"
};


class Drives
{
private:

	TCHAR* name;	//������
	float allSize;	//����������
	float availableSize;	//���ô�С
	string fileSystem;	//�ļ�ϵͳ
	int drivesType;		//�߼��������ͣ��磺���ƶ����̣����ش��̵�
	DWORD totalClusters;//�ܵĴ�
	DWORD freeClusters;//���õĴ�
	DWORD sectPerClust;//ÿ�����ж��ٸ�����
	DWORD bytesPerSect;//ÿ�������ж��ٸ��ֽ�
	DWORD volumeSerialNumber;	//�������к�
	string volumeName;			//�������
	DWORD firstSectByClust;	//��Ŀ¼��ռ����ʼ����

public:
	static int num;	//��¼�ܷ�����

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

	//��ȡ��Ŀ¼��ռ����ʼ����
	DWORD getFirstSectByClust();
	BOOL setFirstSectByClust();

	//�����Ŀ¼��ռ��������λ��
	static DWORD getRootDirectoryLocation(byte* lpBuffer);

	//��ȡ�����߼���������Ϣ
	static Drives* getDrivesMessage();

	//��ȡ��n��������16��������
	//��һ������ΪDrives�����
	//�ڶ�������Ϊ�����������ݵ�����
	//����������ΪҪ��ȡ�ĵ�n������
	BOOL getDriveByN(byte* lpBuffer, DWORD n);

	//��ȡƫ��Ϊn��16��������
	//��һ������Ϊ�����������ݵ�����
	//�ڶ�������ΪҪ��ȡ��ƫ��
	//������ΪҪ��ƫ�ƿ�ʼҪ��ȡ���ֽ���
	BOOL getDriveByOffset(byte* lpBuffer, DWORD offsetHead, int num);

	//��ȡ��n���ص�16�������ݣ�each��ʾһ�ζ�ȡ���ٵĴ�
	BOOL getClustersByN(Drives drive, unsigned char* lpBuffer, DWORD n, int each);

	//��ȡ������������
	DWORD getSectorsSum();


	//BOOL getDriveByN(char* lpBuffer, DWORD n);

	//��ȡ�������ֽ���
	BIGNUM getDriveByteNumber() {
		return totalClusters * (DWORD64)sectPerClust*(DWORD64)bytesPerSect;
	}

	//��ȡ�ص���ʼ����
	DWORD getLocationByClus(DWORD num);







};


/////////////////////////////////////////����ͷ���е���������///////////////////////////




//��ȡÿ����������Ϣ
void eachDrives(Drives& drive);

//��ȡ����������
DWORD getKeepSector(byte* lpBuffer);

//��ȡFAT����
DWORD getFATNum(byte* lpBuffer);

//��ȡÿ��FAT��ռ�õ�������
DWORD getSectorByFAT(byte* lpBuffer);

//��ø�Ŀ¼��ʼ�غ�
DWORD getFirstRootClusters(byte* lpBuffer);

//��ȡFAT1��ռ��������λ��
DWORD getFAT1Location(byte* lpBuffer);

//��ȡFAT2��ռ��������λ��
DWORD getFAT2Location(byte* lpBuffer);

//��ȡÿ����������Ϣ
//void eachDrives(Drives& drive);



//���16��������
//void HexOutput(unsigned char * buf, size_t len);







