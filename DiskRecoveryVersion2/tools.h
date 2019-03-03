#pragma once
#include<Windows.h>
#include<string>
#include<vector>
#include<queue>
#include<map>


#define SECTIONSIZE 512		//每个扇区字节数为512
#define MyDict map<byte, acNode*>
using namespace std;




//复制数组
void copyStr(TCHAR* str1, TCHAR* str2);

void copyStr(byte* str1, byte* str2);

//将unsigned char数组转化为string
string ucharToString(unsigned char* str, int n);

//将unsigned char数组转化为string,根据type转换大小写
string ucharToString(unsigned char* str, int n, byte type);


//将TCHAR转为string类型
string tcharToString(TCHAR *str);

//将char中存储的unicode，用wchar_t存储
void charSaveWchar_t(wchar_t*& str1, unsigned char* str2, int n);

//将char转为wchar_t存储
void charToWchar_t(wchar_t*& str1, const char* str2, int n);

//去除字符串空格
BOOL myTrim(string& s);

//对string类型的字符串进行切片
vector<string> split(const string& src, string separate_character);


//读取目录结束
BOOL readDirectoryEnd(byte* buffer);

//处理字符中的'\0','\n','\r'
BOOL disposeStr(string& str);

//将逗号改为单引号，方便存入.csv文件
BOOL replaceComma(string& str);


//传入的时间是两个字符，16bit
//0~4bit为秒，5~10bit 为分，11~15bit 为时
//得到的时间为hh/mm/ss
string parseTime(unsigned char* time);

//传入的日期是2个字符，16bit
//0~4bit 为日,5~8bit 为月,9~15bit 为年+1980
string parseDate(unsigned char* date);



//解析4字节的长度
DWORD parseLength4(byte* length);
//解析2字节的长度
int parseLength2(byte* length);







