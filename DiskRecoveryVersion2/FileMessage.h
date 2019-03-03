#pragma once
#include<string>
#include<Windows.h>
#define HEADNUM 10
#define ENDNUM 12
using namespace std;

class FileMessage
{
private:
	byte fileHead[HEADNUM];
	byte fileEnd[ENDNUM];
	int fileLength[5];
	string suffix;

public:
	FileMessage();
	~FileMessage();

	byte* getFileHead();
	void setFileHead(byte* fileHead);

	byte* getFileEnd();
	void setFileEnd(byte* fileEnd);

	int* getFileLength();
	void setFileLength(int* fileLength);

	string getSuffix();
	void setSuffix(string suffix);

	void setMessage(string suffix, byte* fileHead, byte* fileEnd, int* fileLength);
};

