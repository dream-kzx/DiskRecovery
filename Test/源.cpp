#include<windows.h>
#include<iostream>
#include<thread>
#include<string>
#include<utility>
#include<functional>
#include<atomic>
#include<chrono>
#include<string>
#include<vector>
using namespace std;

//void test(int i) {
//	cout << i << endl;
//}
//
//void f1(int n) {
//	for (int i = 0; i < 5; i++) {
//		this_thread::sleep_for(chrono::milliseconds(100)); 
//		cout << "Thread" << n << " executing\n";
//
//	}
//}

//void f2(int& n) {
//	for (int i = 0; i < 5; i++) {
//		cout << "Thread 2 executing\n";
//		++n;
//		this_thread::sleep_for(chrono::milliseconds(10));
//	}
//}

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


std::string WideByte2Acsi(std::wstring& wstrcode)
{
	int asciisize = ::WideCharToMultiByte(CP_OEMCP, 0, wstrcode.c_str(), -1, NULL, 0, NULL, NULL);
	if (asciisize == ERROR_NO_UNICODE_TRANSLATION)
	{
		throw std::exception("Invalid UTF-8 sequence.");
	}
	if (asciisize == 0)
	{
		throw std::exception("Error in conversion.");
	}
	std::vector<char> resultstring(asciisize);
	int convresult = ::WideCharToMultiByte(CP_OEMCP, 0, wstrcode.c_str(), -1, &resultstring[0], asciisize, NULL, NULL);

	if (convresult != asciisize)
	{
		throw std::exception("La falla!");
	}

	return std::string(&resultstring[0]);
}


std::wstring Utf82Unicode(const std::string& utf8string)
{
	int widesize = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, NULL, 0);
	if (widesize == ERROR_NO_UNICODE_TRANSLATION)
	{
		throw std::exception("Invalid UTF-8 sequence.");
	}
	if (widesize == 0)
	{
		throw std::exception("Error in conversion.");
	}

	std::vector<wchar_t> resultstring(widesize);

	int convresult = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, &resultstring[0], widesize);

	if (convresult != widesize)
	{
		throw std::exception("La falla!");
	}

	return std::wstring(&resultstring[0]);
}

int main(void) {
	int n = 0;
	char a[7] = { 0xe4,0xbf,0xa1,0xe6,0x81,0xaf };
	std::string test = a;
	std::string strRet("");
	//先把 utf8 转为 unicode  
	std::wstring wstr = Utf82Unicode(test);
	//最后把 unicode 转为 ascii  
	strRet = WideByte2Acsi(wstr);

	return 0;
}









//int main(void) {
//
//	char text[] =  "I love  loveleleleleve fjdslj love fjksdljf!";
//								 
//	char pattern[4] = { 'l','e','l','e' };
//
//	int i = 0;
//	int j = 0;
//	int length1 = sizeof(text);
//	int length2 = sizeof(pattern);
//
//	int move[128];
//
//	memset(move, -1, 128*sizeof(int));
//	for (int k = 0; k < length2; k++) {
//		move[pattern[k]] = length2 - k;
//	}
//
//
//	int match = 0;
//	int compare = 0;
//	int moveNumber = 0;
//	char middle;
//
//	while ( i <= length1 - length2 ) {
//		int k = length2 - 1;
//		for (; k >= 0; k--) {
//			compare++;
//			if (pattern[k] != text[i + k]) {
//				break;
//			}
//		}
//
//		if (k < 0) {
//			cout << "匹配成功,位置为：" << i << endl;
//
//			match++;
//			//i += length2;
//
//			//middle = text[i + length2 + length2 - 1];
//			//if (move[middle] == -1) {//末尾下一位不匹配
//			//	i += 2 * length2;
//			//}
//			//else {
//			//	i += move[middle] + length2 - 1;
//			//}
//			//moveNumber++;
//		}
//		//else {
//		//	middle = text[i + length2];
//		//	if (move[middle] == -1) {//末尾下一位不匹配
//		//		i += length2 + 1;
//		//	}
//		//	else {
//		//		i += move[middle];
//		//	}
//		//	moveNumber++;
//		//}
//		middle = text[i + length2];
//		if (move[middle] == -1) {//末尾下一位不匹配
//			i += length2 + 1;
//		}
//		else {
//			i += move[middle];
//		}
//		moveNumber++;
//
//	}
//
//	cout << "匹配成功字符串个数为：" << match << endl;
//	cout << "比较次数为：" << compare << endl;
//	cout << "移动次数为：" << moveNumber << endl;
//	return 0;
//}