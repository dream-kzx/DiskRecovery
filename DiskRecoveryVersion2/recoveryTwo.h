#pragma once
#include"Drives.h"
#include"FileMessage.h"
#include"Directory.h"
#include"Drives.h"
#include<thread>
#include<future>
#include<string>

#include<iostream>
#include"tools.h"
using namespace std;

static DWORD clustersSum = 0;

//�ڶ�����ʼƥ���ļ�
BOOL matchFile(Drives drive, vector<PDirectoryStruct>& directoryHead);

//�����Ѵ��ڵ���Ŀ¼���дر�ʶ
void queryChildrenDirectory(Drives drive, DWORD directoryLocation, byte* clusterIdentity, int n);

//������Ŀ¼
int disposeDirectoryInTwo(Drives drive, PDirectoryStruct& direcotryHead, DWORD directoryLocation, byte* clusterIdentity, int n);


//����ÿ���ļ�����Ϣ
void eachDirectory(Directory* p);


//�ָ��ļ�
BOOL recoveryFileInTwo(Drives drive, vector<PDirectoryStruct> directoryHead);



//string����תDWORD����
//DWORD stringToDWORD(string str);



