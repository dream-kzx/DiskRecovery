#pragma once

#include<Windows.h>
#include"Drives.h"
#include<map>
#include"AhoCorasick.h"
#include<time.h>
//#include"ThreadPool.h"
#include<thread>
#include"Semaphore.h"
#include"ZipModule.h"
#include"ReadWriteCSV.h"
#include<queue>


using namespace std;




void matchFileFragmentation(Drives drive);

//�����Ѵ��ڵ���Ŀ¼���дر�ʶ
void queryChildrenDirectoryThree(Drives drive, DWORD directoryLocation,byte* sectorsIdentity, int n);

int searchZipIdentity(Drives drive, DWORD sectorsNumber, byte* sectorsIdentity, byte* buffer, int length, acNode* root);

void SundaySearch(byte* buffer, int length, DWORD sectorsNumber, byte* sectorsIdentity, Drives drive);
