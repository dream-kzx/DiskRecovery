#include "tools.h"

void copyStr(TCHAR* str1, TCHAR* str2) {
	for (int i = 0; i < sizeof(str2); i++) {
		str1[i] = str2[i];
	}
}


void copyStr(byte* str1, byte* str2) {
	for (int i = 0; i < sizeof(str2); i++) {
		str1[i] = str2[i];
	}
}

//将unsigned char数组转化为string
string ucharToString(unsigned char* str, int n) {
	string s = "";

	for (int i = 0; i < n; i++) {
		s += str[i];
	}
	return s;
}

//将unsigned char数组转化为string,根据type转换大小写
string ucharToString(unsigned char* str, int n, byte type) {
	string s = "";

	if (n <= 8) {//如果是文件夹
		if ((type == 0x18)||(type == 0x08)) {//如果文件名是小写，则进行转换
			for (int i = 0; i < n; i++) {
				if (str[i] >= 'A'&&str[i] <= 'Z') {
					s += (str[i] - 'A' + 'a');
				}
				else {
					s += str[i];
				}
				
			}
		}
		else {//如果文件名是大写
			for (int i = 0; i < n; i++) {
				s += str[i];
			}
		}
	}
	else {//如果是文件
		int middle = 8;
		//遍历查找“.”的位置
		for (int i = 0; i < n; i++) {
			if (str[i] == '.') {
				middle = i;
				break;
			}
		}

		for (int i = 0; i < middle; i++) {//处理文件名
			if ((type == 0x18) || (type == 0x08)) {//如果文件名是小写，则进行转换
				if (str[i] >= 'A'&&str[i] <= 'Z') {
					s += (str[i] - 'A' + 'a');
				}
				else {
					s += str[i];
				}
			}
			else {//如果文件名是大写
				s += str[i];
			}
		}

		for (int i = middle; i < n; i++) {//处理文件后缀
			if ((type == 0x18) || (type == 0x10)) {//如果后缀是小写，则进行转换
				if (str[i] >= 'A'&&str[i] <= 'Z') {
					s += (str[i] - 'A' + 'a');
				}
				else {
					s += str[i];
				}
			}
			else {//如果后缀名是大写
				s += str[i];
			}
		}
	}


	return s;
}

/////////////////////////将TCHAR转为string类型/////////////////////////////////
string tcharToString(TCHAR *str)
{
	if (str == NULL) {
		return "\0";
	}
	//获取转换的长度
	int iLen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);

	//定义一个新数组保存转换后的字符
	char* chRtn = new char[iLen * sizeof(char)];

	//将转换的字符保存在数组中
	WideCharToMultiByte(CP_ACP, 0, str, -1, chRtn, iLen, NULL, NULL);

	//将数组格式化为string类型
	string str1(chRtn);

	return str1;
}

//将char中存储的unicode，用wchar_t存储
void charSaveWchar_t(wchar_t*& str1, unsigned char* str2, int n) {
	str1 = new wchar_t[n / 2 + 1];
	for (int i = 0, j = 0; i < (n / 2 + 1); i++, j += 2) {
		str1[i] = str2[j] + str2[j + 1] * 16 * 16;
	}
}



//将char中存储的unicode，用wchar_t存储
void charToWchar_t(wchar_t*& str1, const char* str2, int n) {
	int length = n + 1;
	str1 = new wchar_t[length];
	memset(str1, 0, length * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, str2, n, str1, length);

}

//去除字符串空格
BOOL myTrim(string& s) {
	if (s.empty()) {
		return TRUE;
	}
	int x = s.find_last_of(" ");
	if (x >= 0) {
		s.erase(x, 1);
		myTrim(s);
	}
	else {
		return TRUE;
	}
}

//对string类型的字符串进行切片
vector<string> split(const string& src, string separate_character)
{
	vector<string> strs;

	int separate_characterLen = separate_character.size();//分割字符串的长度,这样就可以支持如“,,”多字符串的分隔符
	int lastPosition = 0, index = -1;
	while (-1 != (index = src.find(separate_character, lastPosition)))
	{
		strs.push_back(src.substr(lastPosition, index - lastPosition));
		lastPosition = index + separate_characterLen;
	}
	string lastString = src.substr(lastPosition);//截取最后一个分隔符后的内容
	if (!lastString.empty()) {
		strs.push_back(lastString);//如果最后一个分隔符后还有内容就入队
	}


	return strs;
}

//当读取的目录项前3个字节都为0时，表示根目录信息已读完
BOOL readDirectoryEnd(byte* buffer) {
	if (buffer[0] == 0 && buffer[1] == 0 && buffer[2] == 0) {
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//将逗号改为单引号，方便存入.csv文件
BOOL replaceComma(string& str) {
	if (str.empty()) {
		return TRUE;
	}
	int x = str.find(",");

	if (x >= 0) {
		str[x] = '\'';
		replaceComma(str);
	}
	else {
		return TRUE;
	}
}

//处理字符中的'\0','\n','\r'
BOOL disposeStr(string& str) {
	if (str.empty()) {
		return TRUE;
	}
	int y = str.find("\r");
	int x = str.find("\n");
	int z = str.find('\0');
	if (x >= 0) {
		str.erase(x, 1);
		disposeStr(str);
	}
	else if (y >= 0) {
		str.erase(y, 1);
		disposeStr(str);
	}
	else if (z >= 0) {
		str.erase(z, 1);
		disposeStr(str);
	}
	else {
		return TRUE;
	}
}


//传入的时间是两个字符，16bit
//0~4bit为秒，5~10bit 为分，11~15bit 为时
//得到的时间为hh/mm/ss
string parseTime(unsigned char* time) {
	int second = (time[0] & 0b00011111) * 2;
	int min = (time[0] >> 5) + ((time[1] & 0b00000111) << 3);
	int hour = time[1] >> 3;
	if (second > 60 || min > 60 || hour > 24) {
		return "";
	}
	char okTime[9];
	sprintf_s(okTime, "%02d:%02d:%02d", hour, min, second);

	return okTime;
}

//传入的日期是2个字符，16bit
//0~4bit 为日,5~8bit 为月,9~15bit 为年+1980
string parseDate(unsigned char* date) {
	int day = date[0] & 0b00011111;
	int month = (date[0] >> 5) + ((date[1] & 0b0001) << 1);
	int year = (date[1] >> 1) + 1980;
	if (year <= 1980 || month > 12 || day > 31) {
		return "";
	}
	char okDate[11];

	sprintf_s(okDate, "%04d/%02d/%02d", year, month, day);

	return okDate;
}


//解析4字节的长度
DWORD parseLength4(byte* length) {
	return length[0] + length[1] * 256 + length[2] * 256 * 256 + length[3] * 256 * 256 * 256;
}

//解析2字节的长度
int parseLength2(byte* length) {
	return length[0] + length[1] * 256;
}


