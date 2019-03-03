#pragma once
#include<Windows.h>
#include<string>
#include<vector>
#include<queue>
#include<map>


#define SECTIONSIZE 512		//ÿ�������ֽ���Ϊ512
#define MyDict map<byte, acNode*>
using namespace std;




//��������
void copyStr(TCHAR* str1, TCHAR* str2);

void copyStr(byte* str1, byte* str2);

//��unsigned char����ת��Ϊstring
string ucharToString(unsigned char* str, int n);

//��unsigned char����ת��Ϊstring,����typeת����Сд
string ucharToString(unsigned char* str, int n, byte type);


//��TCHARתΪstring����
string tcharToString(TCHAR *str);

//��char�д洢��unicode����wchar_t�洢
void charSaveWchar_t(wchar_t*& str1, unsigned char* str2, int n);

//��charתΪwchar_t�洢
void charToWchar_t(wchar_t*& str1, const char* str2, int n);

//ȥ���ַ����ո�
BOOL myTrim(string& s);

//��string���͵��ַ���������Ƭ
vector<string> split(const string& src, string separate_character);


//��ȡĿ¼����
BOOL readDirectoryEnd(byte* buffer);

//�����ַ��е�'\0','\n','\r'
BOOL disposeStr(string& str);

//�����Ÿ�Ϊ�����ţ��������.csv�ļ�
BOOL replaceComma(string& str);


//�����ʱ���������ַ���16bit
//0~4bitΪ�룬5~10bit Ϊ�֣�11~15bit Ϊʱ
//�õ���ʱ��Ϊhh/mm/ss
string parseTime(unsigned char* time);

//�����������2���ַ���16bit
//0~4bit Ϊ��,5~8bit Ϊ��,9~15bit Ϊ��+1980
string parseDate(unsigned char* date);



//����4�ֽڵĳ���
DWORD parseLength4(byte* length);
//����2�ֽڵĳ���
int parseLength2(byte* length);







