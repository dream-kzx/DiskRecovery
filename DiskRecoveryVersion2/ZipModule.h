#pragma once
#include<iostream>
#include<Windows.h>
#include<string>
#include"tools.h"
using namespace std;

/*
用来匹配office文件的zip对象
*/
class ZipModule
{
private:
	//byte * zipData = NULL;//存储一个zip的数据
	//byte identifyHead[4] = { 0x50,0x4b,0x03,0x04 };//0-3bit
	//byte zipHead[30];//zip文件头信息（不包含扩展区）
	string lastTime ="";//10~11bit	最后修改时间
	string lastDate = "";//12~13bit	最后修改日期
	int compressSize = 0;//18~21bit	压缩后大小	
	int unCompressSize = 0;//22~25bit 未压缩大小
	int fileNameLength = 0;//26~27bit 文件名长度
	int extraLength = 0;//28~29bit 扩展区长度
	string fileName ="";//30~(330+fileNameLength-1) 文件名
	int zipSize = 0;//zip文件的大小  =compressSzie+30+fileNameLength+extraLength
	DWORD location = 0;//记录改zip在磁盘中的偏移位置

	byte identifyEnd[4] = { 0x50,0x4b,0x05,0x06 };
	byte identifyCore[4]= { 0x50,0x4b,0x01,0x02 };
	
public:
	ZipModule();
	~ZipModule();

	//解析zip文件头部
	void parseZip(byte* zipHead);

	//解析zip文件文件名
	void parseZipName(byte* zipHead);

	//根据zip文件标志判断是不是zip文件,
	//返回1：为0x504b0304
	//返回2：为0x504b0102
	//返回3：为0x50460506
	//返回-1: 为其他
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

