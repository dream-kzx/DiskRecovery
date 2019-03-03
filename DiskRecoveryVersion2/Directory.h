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
	int index;			//�������±�һ��������ƥ�䲻���ļ���
	int num = 0;		//��ʾ��ǰĿ¼ռ���ٸ�Ŀ¼��
	byte str[SECTIONSIZE];
	DWORD firstClusters = 0;	//��ʼ�غ�
	DWORD length = 0;			//�ļ�����
	string createTime;		//��������ʱ��
	string updateTime;		//�޸�����ʱ��
	string visitTime;		//����������
	unsigned int type;		//�ļ����� 0x20�ǹ鵵 0x10����Ŀ¼
	string name;			//�ļ���

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

	//����ʱ��
	static string parseTime(unsigned char* time);

	//��������
	static string parseDate(unsigned char* date);

	//��ȡ�ļ���ռ������
	void getFileSize(DWORD* num) {
		num[0] = length / 512;
		num[1] = length % 512;
	}

	//��ƥ�䵽����δ��ɾ����־���ļ�ʱ��ֱ���Թ�������¼��������Ϊ\0
	void setStrNULL();
};

typedef struct DirectoryLink {
	Directory* directory;
	int directoryType;//0��ʾ�ļ�,1��ʾĿ¼��2��ʾ��Ŀ¼
	vector<DirectoryLink*> childrens;
	//struct DirectoryLink* next;
}DirectoryStruct, *PDirectoryStruct;



//���������ļ������νṹ,ת���ļ���Ϣ
void traverseTree(PDirectoryStruct& PNode);

//�����ļ���Ŀ¼��16�����룬��ȡ�ļ���Ŀ¼����Ϣ
void getDetailMessage(PDirectoryStruct& PNode);


//�������νڵ���Ϣ����ӡ����
void traverseNode(PDirectoryStruct& PNode, string parentName, int* n, vector<string>& showDetails, string fileName);

//����Ҫ�ָ����ļ���Ŀ¼
BOOL saveRecovery(Drives drive, PDirectoryStruct PNode, string parentName);

//�ж��ļ����Ƿ��Ѵ��ڣ�����������ں����_1
void isCreateFile(string& name);