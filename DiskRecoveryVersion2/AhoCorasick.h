#pragma once
#include<Windows.h>
#include<iostream>
#include<queue>
#include<map>
#include<string>
#include<fstream>



using namespace std;

struct acNode {
	acNode* fail;
	acNode* next[256];
	byte str;
	int count;

	acNode() {
		fail = NULL;
		memset(next, NULL, sizeof(next));
		count = 0;
		str = '\0';
		
	}
};


//����ʱ�����ַ�����������
void insert(char* str, acNode* root, int identity);

//����fail
void buildACFail(acNode* root);


int searchKeyword(byte* buffer, int length, acNode* root);