#pragma once
#include<iostream>
#include<string>

#include<Windows.h>
#include<vector>
#include"tools.h"
#include"Drives.h"

using namespace std;

class Directory
{
private:
	int index;			//向数组下标一样，用来匹配不是文件的
	int num = 0;		//表示当前目录占多少个目录项
	byte str[SECTIONSIZE];
	DWORD firstClusters = 0;	//起始簇号
	DWORD length = 0;			//文件长度
	string createTime;		//创建日期时间
	string updateTime;		//修改日期时间
	string visitTime;		//最后访问日期
	unsigned int type;		//文件类型 0x20是归档 0x10是子目录
	string name;			//文件名

public:
	Directory();
	~Directory();

	int getI();

	int getIndex();
	void setIndex(int index);

	byte* getStr();
	void setStr(byte* str1);

	DWORD getFirstClusters();
	void setFirstClusters(DWORD num);

	DWORD getLength();
	void setLength(DWORD num);

	string getName();
	void setName(string name);

	unsigned int getType();
	void setType(unsigned int type);

	string getCreateTime();
	void setCreateTime(string time);

	string getUpdateTime();
	void setUpdateTime(string time);

	string getVisitTime();
	void setVisitTime(string time);

	//解析时间
	static string parseTime(unsigned char* time);

	//解析日期
	static string parseDate(unsigned char* date);

	//获取文件所占扇区数
	void getFileSize(DWORD* num) {
		num[0] = length / 512;
		num[1] = length % 512;
	}

	//当匹配到的是未带删除标志的文件时，直接略过，将记录的数组置为\0
	void setStrNULL();
};

typedef struct DirectoryLink {
	Directory* directory;
	int directoryType;//0表示文件,1表示目录，2表示根目录
	vector<DirectoryLink*> childrens;
	//struct DirectoryLink* next;
}DirectoryStruct, *PDirectoryStruct;



//遍历保存文件的树形结构,转换文件信息
void traverseTree(PDirectoryStruct& PNode);

//根据文件或目录的16进制码，读取文件或目录的信息
void getDetailMessage(PDirectoryStruct& PNode);


//遍历树形节点信息并打印出来
void traverseNode(PDirectoryStruct& PNode, string parentName, int* n, vector<string>& showDetails, string fileName);

//保存要恢复的文件或目录
BOOL saveRecovery(Drives drive, PDirectoryStruct PNode, string parentName);

//判断文件名是否已存在，如果存在则在后面加_1
void isCreateFile(string& name);