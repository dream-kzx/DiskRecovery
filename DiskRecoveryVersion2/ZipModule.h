#pragma once
#include<iostream>
#include<Windows.h>
#include<string>
#include"tools.h"
using namespace std;

/*
����ƥ��office�ļ���zip����
*/
class ZipModule
{
private:
	//byte * zipData = NULL;//�洢һ��zip������
	//byte identifyHead[4] = { 0x50,0x4b,0x03,0x04 };//0-3bit
	//byte zipHead[30];//zip�ļ�ͷ��Ϣ����������չ����
	string lastTime ="";//10~11bit	����޸�ʱ��
	string lastDate = "";//12~13bit	����޸�����
	int compressSize = 0;//18~21bit	ѹ�����С	
	int unCompressSize = 0;//22~25bit δѹ����С
	int fileNameLength = 0;//26~27bit �ļ�������
	int extraLength = 0;//28~29bit ��չ������
	string fileName ="";//30~(330+fileNameLength-1) �ļ���
	int zipSize = 0;//zip�ļ��Ĵ�С  =compressSzie+30+fileNameLength+extraLength
	DWORD location = 0;//��¼��zip�ڴ����е�ƫ��λ��

	byte identifyEnd[4] = { 0x50,0x4b,0x05,0x06 };
	byte identifyCore[4]= { 0x50,0x4b,0x01,0x02 };
	
public:
	ZipModule();
	~ZipModule();

	//����zip�ļ�ͷ��
	void parseZip(byte* zipHead);

	//����zip�ļ��ļ���
	void parseZipName(byte* zipHead);

	//����zip�ļ���־�ж��ǲ���zip�ļ�,
	//����1��Ϊ0x504b0304
	//����2��Ϊ0x504b0102
	//����3��Ϊ0x50460506
	//����-1: Ϊ����
	//int isZipIdentify(byte* identify);

	//byte* getIdentifyHead();

	//byte* getZipData();
	//void setZipData(byte* zipData);

	//byte* getZipHead();
	//void setZipHead(byte* zipHead);
	
	string getLastTime();
	
	int getCompressSize();

	int getUnCompressSize();
	
	int getFileNameLength();

	int getExtraLength();
	
	string getFileName();

	int getZipSize();

	DWORD getLocation();
	void setLocation(DWORD location);

};

