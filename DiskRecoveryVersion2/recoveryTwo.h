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

//第二步开始匹配文件
BOOL matchFile(Drives drive, vector<PDirectoryStruct>& directoryHead);

//遍历已存在的子目录进行簇标识
void queryChildrenDirectory(Drives drive, DWORD directoryLocation, byte* clusterIdentity, int n);

//处理子目录
int disposeDirectoryInTwo(Drives drive, PDirectoryStruct& direcotryHead, DWORD directoryLocation, byte* clusterIdentity, int n);


//设置每个文件的信息
void eachDirectory(Directory* p);


//恢复文件
BOOL recoveryFileInTwo(Drives drive, vector<PDirectoryStruct> directoryHead);



//string类型转DWORD类型
//DWORD stringToDWORD(string str);



